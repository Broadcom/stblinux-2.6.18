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
//#define USE_PERFORMANCE_INTERRUPT
#define PERF_COUNTER_BASE 0xB1F20000
#define BCM_PERFCOUNT_IRQ BCM_LINUX_PERFCOUNT_IRQ


typedef struct BCM4350_register_config {
	unsigned long global_control;		// 0x00
	#define PCE_enable (1<<31)
	#define PCSD_disable (1<<8)
	#define ModID(module) (module<<2)
	#define mod_BIU 1
	#define mod_Core 2
	#define mod_Data_Cache 4
	#define mod_Instruction_Cache 6
	#define mod_Readahead_Cache 0xb
	#define SetID(id) (id)
	#define mod_set_mask 0x3f
	unsigned long control[2];			// 0x04, 0x08
	#define BCM4350_COUNTER_EVENT(c,event)		(event  <<  (c%2 ? 18:2))
	#define BCM4350_COUNTER_ENABLE(c)			(1      <<  (c%2 ? 31:15))
	#define BCM4350_COUNTER_TPID(c,pid)			(pid    <<  (c%2 ? 29:13))
	#define BCM4350_COUNTER_AMID(c,amid)		(amid   <<  (c%2 ? 25:9))
	#define BCM4350_COUNTER_OVERFLOW(c)			(1    	<<	(c%2 ? 16:0))
	unsigned long unused;		// 0x0c
	long counter[4];			// 0x10, 0x14, 0x18, 0x1c
} Perf_Control;
#define PERFC ((volatile Perf_Control * const) PERF_COUNTER_BASE)
Perf_Control reset;

/* bcm4350 has some odd fields in the control registers that must be set in 
 * conjunction with certain events.  This table keeps track of them.
 */
struct event_mod_sets {
	int event;
	int modid;
	int setid;
} BCM4350_event_mod_sets[] = 
{
	{ 5,	mod_Instruction_Cache ,	0 },
	{ 6,	mod_Instruction_Cache ,	0 },
	{ 9,	mod_Data_Cache,			1 },
	{10,	mod_Data_Cache,			1 },
	{66,	mod_Readahead_Cache,	0 },
	{75,	mod_Readahead_Cache,	1 },
	{69,	mod_BIU,				2 },
	{-1,-1,-1}
};

int test_all_counters(void);

int test_all_counters(void)
{
	if(!(PERFC->global_control & PCE_enable)) return 0;
	{
		int i;
 		for(i=0;i<4;i++)
		{
			int cinx = i>>1;
#ifdef USE_PERFORMANCE_INTERRUPT
			if (PERFC->control[cinx] & BCM4350_COUNTER_OVERFLOW(i) && PERFC->counter[i]<0)
#else
			if (PERFC->control[cinx] & BCM4350_COUNTER_ENABLE(i) && PERFC->counter[i]<0)
#endif
			{
				PERFC->control[cinx] &= ~(BCM4350_COUNTER_ENABLE(i)|BCM4350_COUNTER_OVERFLOW(i));
				return 1;
			}
		}
	}
	return 0;
}

/* Watch dog to insure only compatible events are selected together */
static int BCM4350_mod_mask_ok(struct op_counter_config *ctr)
{
struct event_mod_sets *mod_set;
 	/* Scan mod set for extra field settings */
	for(mod_set = BCM4350_event_mod_sets; mod_set->event!=-1; mod_set++)
		if(mod_set->event==ctr->event)
		{
			 /* If the mod_set mask is being used, only allow compatable events */
			 if(!(PERFC->global_control & mod_set_mask) && 
				!((PERFC->global_control & mod_set_mask) == (ModID(mod_set->modid) | SetID(mod_set->setid)))) return 0;
			PERFC->global_control |= ModID(mod_set->modid) | SetID(mod_set->setid);
			return 1;
		}
	return 1;
}

/* Compute all of the registers in preparation for enabling profiling.  */
static void BCM4350_reg_setup(struct op_counter_config *ctr)
{
	int i;
  	PERFC->global_control = PCE_enable;
	PERFC->control[0] = PERFC->control[1] = 0x0;
/* Compute the performance counter control word.  */
	/* For now count kernel and user mode */
	for(i=0;i<4;i++)
	{
		PERFC->counter[i] = -1;
		if (ctr[i].enabled && BCM4350_mod_mask_ok(&ctr[i]))
		{
			int cinx = i>>1; 	/* Control index is half of counter index */
 			reset.counter[i] = ctr[i].count;
    		reset.control[cinx] |=  BCM4350_COUNTER_EVENT(i,ctr[i].event) |
				BCM4350_COUNTER_ENABLE(i) 
#ifdef USE_PERFORMANCE_INTERRUPT
				|
				BCM4350_COUNTER_OVERFLOW(i)
#endif
				;
		}
	}
}

/* Program all of the registers in preparation for enabling profiling.  */
static void BCM4350_cpu_setup (void *args)
{
	PERFC->counter[0] = reset.counter[0];
 	PERFC->counter[1] = reset.counter[1];
	PERFC->counter[2] = reset.counter[2];
	PERFC->counter[3] = reset.counter[3];
}

static void BCM4350_cpu_start(void *args)
{
	/* Start all counters on current CPU */
	PERFC->control[0] = reset.control[0];
	PERFC->control[1] = reset.control[1];

}

static void BCM4350_cpu_stop(void *args)
{
	/* Stop all counters on current CPU */
	PERFC->control[0] = PERFC->control[1] = 0x0;
}

static irqreturn_t BCM4350_perfcount_handler(int , void * , struct pt_regs *);

static irqreturn_t BCM4350_perfcount_handler(int irq, void * dev_id,
	struct pt_regs *regs)
{
	int i;
	/* See if this is our interrupt */
 	if(!(PERFC->global_control & PCE_enable)) return IRQ_HANDLED;  // Nope
   /*
	 * Scan active counters and add to samples on overflow
	 */
	for(i=0;i<4;i++)
	{
		int cinx = i>>1;
#ifdef USE_PERFORMANCE_INTERRUPT
		if (PERFC->control[cinx] & BCM4350_COUNTER_OVERFLOW(i) && PERFC->counter[i]<0) {
#else
		if (reset.control[cinx] & BCM4350_COUNTER_ENABLE(i) && PERFC->counter[i]<0) {
#endif
			PERFC->control[cinx] &= ~(BCM4350_COUNTER_ENABLE(i)|BCM4350_COUNTER_OVERFLOW(i));
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,12))
			oprofile_add_sample(instruction_pointer(regs),!user_mode(regs),i, smp_processor_id());
#else
			oprofile_add_sample(regs, i);
#endif
			PERFC->counter[i] = reset.counter[i];
		 PERFC->control[cinx] = reset.control[cinx];
		}
	}
#ifdef FORCE_INTERRUPT_EARLY
	BCM4350_cpu_stop(NULL);
#endif
	return IRQ_HANDLED;
}

static int __init BCM4350_init(void)
{
	int ret;
#ifdef FORCE_INTERRUPT_EARLY
	struct op_counter_config ctr[4];
#endif
	ret = request_irq(BCM_PERFCOUNT_IRQ, BCM4350_perfcount_handler,
	                   0, "Perfcounter", NULL);

#ifdef FORCE_INTERRUPT_EARLY
	ctr[0].enabled = 1;
	ctr[1].enabled = ctr[2].enabled = ctr[3].enabled = 0;
	ctr[0].event = 0x12;   // CYCLES
	ctr[0].count = 2000000;	
	BCM4350_reg_setup((struct op_counter_config *)&ctr);
	BCM4350_cpu_setup ((void *)&ctr);
	BCM4350_cpu_start ((void *)&ctr);

#endif
	return ret;
}

static void BCM4350_exit(void)
{
	PERFC->global_control &= ~PCE_enable;
	free_irq(BCM_PERFCOUNT_IRQ, NULL);
}

struct op_mips_model op_model_bcm4350 = {
	.reg_setup	= BCM4350_reg_setup,
	.cpu_setup	= BCM4350_cpu_setup,
	.init		= BCM4350_init,
	.exit		= BCM4350_exit,
	.cpu_start	= BCM4350_cpu_start,
	.cpu_stop	= BCM4350_cpu_stop,
	.cpu_type	= "mips/bcm4350",
	.num_counters	= 4
};
