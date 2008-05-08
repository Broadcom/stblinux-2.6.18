/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2005, 2006 Broadcom Corp by Troy Trammel
 * Copyright (C) 2004 by Ralf Baechle from file op_model_rm9000.c
 */
#include <linux/init.h>
#include <linux/oprofile.h>
#include <linux/interrupt.h>
#include <linux/smp.h>
#include <linux/version.h>
#include <asm/brcmstb/common/brcmstb.h>

#include "op_impl.h"

//#define FORCE_INTERRUPT_EARLY
#define PERF_COUNTER_BASE 0xB1F20000
#define BCM_PERFCOUNT_IRQ BCM_LINUX_PERFCOUNT_IRQ


typedef struct BCM4380_register_config {
	unsigned long global_control;		// 0x00
	#define PCE_enable (1<<31)
	#define PCE_reserved (0x7ffffec0)
	#define PCSD_disable (1<<8)
	#define ModID(module) (module<<2)
	#define mod_BIU 1
	#define mod_Core 2
	#define mod_Data_Cache 4
	#define mod_Instruction_Cache 6
	#define mod_Readahead_Cache 0xb
	#define mod_L2_Cache 0xd
	#define SetID(id) (id)
	#define mod_set_mask 0x3f
	unsigned long control[2];		// 0x04, 0x08
	#define BCM4380_COUNTER_EVENT(c,event)		((event)<<  ((c)&1 ? 18:2))
	#define BCM4380_COUNTER_ENABLE(c)		(1      <<  ((c)&1 ? 31:15))
	#define BCM4380_COUNTER_TPID(c,pid)		((pid)  <<  ((c)&1 ? 29:13))
	#define BCM4380_COUNTER_AMID(c,amid)		((amid) <<  ((c)&1 ? 25:9))
	#define BCM4380_COUNTER_OVERFLOW(c)		(1    	<<  ((c)&1 ? 16:0))
	unsigned long unused;			// 0x0c
	unsigned long counter[4];		// 0x10, 0x14, 0x18, 0x1c
} Perf_Control;
#define PERFC ((volatile Perf_Control * const) PERF_COUNTER_BASE)
Perf_Control reset;

/* bcm4380 has some odd fields in the control registers that must be set in 
 * conjunction with certain events.  This table keeps track of them.
 */
struct event_mod_sets {
	int event;
	int modid;
	int setid;
} BCM4380_event_mod_sets[] = 
{
	{ 0x005, mod_Instruction_Cache, 0 },
	{ 0x006, mod_Instruction_Cache, 0 },
	{ 0x009, mod_Data_Cache, 1 },
	{ 0x00a, mod_Data_Cache, 1 },
	{ 0x042, mod_Readahead_Cache, 0 },
	{ 0x04b, mod_Readahead_Cache, 1 },
	{ 0x045, mod_BIU, 2 },
/*
 * NOTE: bits 11:8 of these event IDs were arbitrarily chosen to make the
 * event IDs unique
 */
	{ 0x101, mod_Core, 0 },         /* CPU_STALL_CYCLES */
	{ 0x103, mod_Core, 2 },         /* CPU_KERNEL_CYCLES */
	{ 0x201, mod_Core, 2 },         /* CPU_USER_CYCLES */
	{ 0x10a, mod_Core, 1 },         /* EXCEPTIONS */
	{ 0x301, mod_L2_Cache, 0 },     /* L2_HITS */
	{ 0x300, mod_L2_Cache, 0 },     /* L2_MISSES */

	{-1,-1,-1}
};

int test_all_counters(void)
{
	int i, overflow = 0;
	if(!(PERFC->global_control & PCE_enable))
		return 0;

	for(i=0;i<4;i++)
	{
		int cinx = i>>1;
		if ((PERFC->control[cinx] & BCM4380_COUNTER_OVERFLOW(i))
			&& (PERFC->counter[i] & 0x80000000))
		{
			PERFC->control[cinx] &= ~(BCM4380_COUNTER_ENABLE(i)|BCM4380_COUNTER_OVERFLOW(i));
			overflow = 1;
		}
	}
	return(overflow);
}

/* Watch dog to insure only compatible events are selected together */
static int BCM4380_mod_mask_ok(struct op_counter_config *ctr)
{
	struct event_mod_sets *mod_set;

 	/* Scan mod set for extra field settings */
	for(mod_set = BCM4380_event_mod_sets; mod_set->event!=-1; mod_set++)
		if(mod_set->event==ctr->event)
		{
			unsigned int mask = PERFC->global_control & mod_set_mask;
			unsigned int new_mask = ModID(mod_set->modid) |
				SetID(mod_set->setid);

			/* If the mod_set mask is being used, only allow compatable events */
			if(mask && (mask != new_mask))
				return 0;
			PERFC->global_control |= new_mask;
			return 1;
		}
	return 1;
}

/* Compute all of the registers in preparation for enabling profiling.  */
static void BCM4380_reg_setup(struct op_counter_config *ctr)
{
	int i;

	/* Compute the performance counter control word.  */
	reset.control[0] = reset.control[1] = 0x0;

	/* For now count kernel and user mode */
	for(i=0;i<4;i++)
	{
		PERFC->counter[i] = 0x7fffffff;
		if (ctr[i].enabled && BCM4380_mod_mask_ok(&ctr[i]))
		{
			int cinx = i>>1; 	/* Control index is half of counter index */

 			reset.counter[i] = ctr[i].count;
			reset.control[cinx] |=
				BCM4380_COUNTER_EVENT(i,ctr[i].event & 0x7f) |
				BCM4380_COUNTER_ENABLE(i) | BCM4380_COUNTER_OVERFLOW(i);
		}
	}
}

/* Program all of the registers in preparation for enabling profiling.  */
static void BCM4380_cpu_setup (void *args)
{
	PERFC->counter[0] = reset.counter[0];
 	PERFC->counter[1] = reset.counter[1];
	PERFC->counter[2] = reset.counter[2];
	PERFC->counter[3] = reset.counter[3];
}

static void BCM4380_cpu_start(void *args)
{
	/* Start all counters on current CPU */
	PERFC->global_control = PCE_enable;
	PERFC->control[0] = reset.control[0];
	PERFC->control[1] = reset.control[1];
}

static void BCM4380_cpu_stop(void *args)
{
	/* Stop all counters on current CPU */
	PERFC->control[0] = PERFC->control[1] = 0x0;
}

static irqreturn_t BCM4380_perfcount_handler(int irq, void * dev_id,
	struct pt_regs *regs)
{
	int i;
	/* See if this is our interrupt */
 	if(!(PERFC->global_control & PCE_enable))
		return IRQ_HANDLED;  // Nope

	/* Scan active counters and add to samples on overflow */
	for(i=0;i<4;i++)
	{
		int cinx = i>>1;
		if ((reset.control[cinx] & BCM4380_COUNTER_OVERFLOW(i))
			&& (PERFC->counter[i] & 0x80000000)) {

			PERFC->counter[i] = reset.counter[i];
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,12))
			oprofile_add_sample(instruction_pointer(regs),
				!user_mode(regs), i, smp_processor_id());
#else
			oprofile_add_sample(regs, i);
#endif

			/*
			 * SIGH.  Zero out reserved fields, otherwise we get
			 * stuck in the perf interrupt.
			 */
			PERFC->global_control &= ~PCE_reserved;
			PERFC->counter[i] = reset.counter[i];
			PERFC->control[cinx] = reset.control[cinx];
		}
	}
#ifdef FORCE_INTERRUPT_EARLY
	BCM4380_cpu_stop(NULL);
#endif
	return IRQ_HANDLED;
}

static int __init BCM4380_init(void)
{
	int ret;
#ifdef FORCE_INTERRUPT_EARLY
	struct op_counter_config ctr[4];
#endif
	ret = request_irq(BCM_PERFCOUNT_IRQ, BCM4380_perfcount_handler,
	                   0, "Perfcounter", NULL);

#ifdef FORCE_INTERRUPT_EARLY
	ctr[0].enabled = 1;
	ctr[1].enabled = ctr[2].enabled = ctr[3].enabled = 0;
	ctr[0].event = 0x12;   // CYCLES
	ctr[0].count = 2000000;	
	BCM4380_reg_setup((struct op_counter_config *)&ctr);
	BCM4380_cpu_setup ((void *)&ctr);
	BCM4380_cpu_start ((void *)&ctr);

#endif
	return ret;
}

static void BCM4380_exit(void)
{
	PERFC->global_control &= ~PCE_enable;
	free_irq(BCM_PERFCOUNT_IRQ, NULL);
}

struct op_mips_model op_model_bcm4380 = {
	.reg_setup	= BCM4380_reg_setup,
	.cpu_setup	= BCM4380_cpu_setup,
	.init		= BCM4380_init,
	.exit		= BCM4380_exit,
	.cpu_start	= BCM4380_cpu_start,
	.cpu_stop	= BCM4380_cpu_stop,
	.cpu_type	= "mips/bcm4380",
	.num_counters	= 4
};
