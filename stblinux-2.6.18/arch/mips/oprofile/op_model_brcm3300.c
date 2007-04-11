/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2005 Broadcom Corp by Troy Trammel
 * Copyright (C) 2004 by Ralf Baechle
 */
#include <linux/config.h>
#include <linux/init.h>
#include <linux/oprofile.h>
#include <linux/interrupt.h>
#include <linux/smp.h>


#include <asm/brcmstb/common/brcmstb.h>

#include "op_impl.h"

//#define USE_OVERFLOW_INTERRUPT

/* Standard MIPS performance counter definitions are slightly different from the BMIPS3300
 * Here we define new macros so as not to get confused.
 * NOTE: These macros are a bit touchy, if the sub-register agrument is not a constant,
 * the assembler chokes.  For now we use a couple of "paste value" macros and insure the arguments are constant.
 */
/* Global control register */
#define read_c0_perf_global_control() __read_32bit_c0_register($25, 6)
#define write_c0_perf_global_control(val) __write_32bit_c0_register($25, 6, val)

/* Control registers */
#define read_c0_perf_control0 __read_32bit_c0_register($25, 4)
#define write_c0_perf_control0(val) __write_32bit_c0_register($25, 4, val)
#define read_c0_perf_control1 __read_32bit_c0_register($25, 5)
#define write_c0_perf_control1(val) __write_32bit_c0_register($25, 5, val)
/* Paste value macros */
#define read_c0_perf_control(i) read_c0_perf_control##i
#define write_c0_perf_control(i,val) write_c0_perf_control##i(val)

/* Counter registers */
#define read_c0_perf_counter0 __read_32bit_c0_register($25, 0)
#define write_c0_perf_counter0(val) __write_32bit_c0_register($25, 0, val)
#define read_c0_perf_counter1 __read_32bit_c0_register($25, 1)
#define write_c0_perf_counter1(val) __write_32bit_c0_register($25, 1, val)
#define read_c0_perf_counter2 __read_32bit_c0_register($25, 2)
#define write_c0_perf_counter2(val) __write_32bit_c0_register($25, 2, val)
#define read_c0_perf_counter3 __read_32bit_c0_register($25, 3)
#define write_c0_perf_counter3(val) __write_32bit_c0_register($25, 3, val)
/* Paste value macros */
#define read_c0_perf_counter(i) read_c0_perf_counter##i
#define write_c0_perf_counter(i,val) write_c0_perf_counter##i(val)
  
	 

typedef struct BCM3300_register_config {
	unsigned long global_control;
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
	unsigned long control[2];
	#define BCM3300_COUNTER_EVENT(c,event)		(event  <<  (c%2 ? 18:2))
	#define BCM3300_COUNTER_ENABLE(c)			(1      <<  (c%2 ? 31:15))
	#define BCM3300_COUNTER_TPID(c,pid)			(pid    <<  (c%2 ? 29:13))
	#define BCM3300_COUNTER_AMID(c,amid)		(amid   <<  (c%2 ? 25:9))
	#define BCM3300_COUNTER_OVERFLOW(c)			(1    	<<	(c%2 ? 16:0))
	unsigned long unused;
	long counter[4];
} Perf_Control;
Perf_Control reset;

/* bcm3300 has some odd fields in the control registers that must be set in 
 * conjunction with certain events.  This table keeps track of them.
 */
struct event_mod_sets {
	int event;
	int modid;
	int setid;
} BCM3300_event_mod_sets[] = 
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


/* Watch dog to insure only compatible events are selected together */
static int BCM3300_mod_mask_ok(struct op_counter_config *ctr)
{
struct event_mod_sets *mod_set;
unsigned long global_control = read_c0_perf_global_control();
 	/* Scan mod set for extra field settings */
	for(mod_set = BCM3300_event_mod_sets; mod_set->event!=-1; mod_set++)
		if(mod_set->event==ctr->event)
		{
			 /* If the mod_set mask is being used, only allow compatable events */
			 if(!(global_control & mod_set_mask) && 
				!((global_control & mod_set_mask) == (ModID(mod_set->modid) | SetID(mod_set->setid)))) return 0;
			global_control |= ModID(mod_set->modid) | SetID(mod_set->setid);
			return 1;
		}
	return 1;
}

/* Compute all of the registers in preparation for enabling profiling.  */
static void BCM3300_reg_setup(struct op_counter_config *ctr)
{

	int i;
	unsigned long control;
	write_c0_perf_global_control(PCE_enable);
	write_c0_perf_control(0,0x0);
	write_c0_perf_control(1,0x0);
/* Compute the performance counter control word.  */
	/* For now count kernel and user mode */
	for(i=0;i<4;i++)
	{
		if (ctr[i].enabled && BCM3300_mod_mask_ok(&ctr[i]))
		{
			int cinx = i>>1; 	/* Control index is half of counter index */
			/* Basic control register set */
			control = (cinx) ? read_c0_perf_control(1) : read_c0_perf_control(0); 
			control |=  
				BCM3300_COUNTER_EVENT(i,ctr[i].event) |
				BCM3300_COUNTER_ENABLE(i); 
#ifdef USE_OVERFLOW_INTERRUPT
			 control |= BCM3300_COUNTER_OVERFLOW(i);
#endif
			if (cinx) write_c0_perf_control(1,control);
			else write_c0_perf_control(0,control);
			/* Counter reset values */
			reset.control[cinx] = control;
			reset.counter[i] = ctr[i].count;
			switch(i) {
			case 0: write_c0_perf_counter(0,reset.counter[i]); break;
			case 1: write_c0_perf_counter(1,reset.counter[i]); break;
			case 2: write_c0_perf_counter(2,reset.counter[i]); break;
			case 3: write_c0_perf_counter(3,reset.counter[i]); break;
			}
		}
	}
}

/* Program all of the registers in preparation for enabling profiling.  */
static void BCM3300_cpu_setup (void *args)
{
	write_c0_perf_counter(0,reset.counter[0]);
	write_c0_perf_counter(1,reset.counter[1]);
	write_c0_perf_counter(2,reset.counter[2]);
	write_c0_perf_counter(3,reset.counter[3]);
}

static void BCM3300_cpu_start(void *args)
{
	/* Start all counters on current CPU */
	write_c0_perf_control(0,reset.control[0]);
	write_c0_perf_control(1,reset.control[1]);
}

static void BCM3300_cpu_stop(void *args)
{
	/* Stop all counters on current CPU */
	write_c0_perf_control(0,0x0);
	write_c0_perf_control(1,0x0);
}

static irqreturn_t BCM3300_perfcount_handler(int irq, void * dev_id,
	struct pt_regs *regs)
{
	int i;
	/* See if this is our interrupt */
	if(!(read_c0_perf_global_control() & PCE_enable)) return IRQ_HANDLED;  // Nope
	/*
	 * Scan active counters and add to samples on overflow
	 */
	for(i=0;i<4;i++)
	{
		int cinx = i>>1;
   		unsigned long control = cinx ? read_c0_perf_control(1) : read_c0_perf_control(0);
		long counter = 0;
		switch(i) {
		case 0: counter = read_c0_perf_counter(0); break;
		case 1: counter = read_c0_perf_counter(1); break;
		case 2: counter = read_c0_perf_counter(2); break;
		case 3: counter = read_c0_perf_counter(3); break;
		}
#ifdef USE_OVERFLOW_INTERRUPT
		if (control & BCM3300_COUNTER_OVERFLOW(i) && counter<0) {
#else
		if (control & BCM3300_COUNTER_ENABLE(i) && counter<0) {
#endif
 			/* Clear enable and overflow bits */
			control &= ~(BCM3300_COUNTER_ENABLE(i)|BCM3300_COUNTER_OVERFLOW(i));
			if (cinx) write_c0_perf_control(1,control); 
			else write_c0_perf_control(0,control);
			oprofile_add_sample(regs, i);
			switch(i) {
			case 0: write_c0_perf_counter(0,reset.counter[i]); break;
			case 1: write_c0_perf_counter(1,reset.counter[i]); break;
			case 2: write_c0_perf_counter(2,reset.counter[i]); break;
			case 3: write_c0_perf_counter(3,reset.counter[i]); break;
			}
			if (cinx) write_c0_perf_control(1,reset.control[cinx]);
			else write_c0_perf_control(0,reset.control[cinx]);
		}
	}
	return IRQ_HANDLED;
}

static int __init BCM3300_init(void)
{
	return request_irq(BCM_PERFCOUNT_IRQ, BCM3300_perfcount_handler,
	                   0, "Perfcounter", NULL);
}

static void BCM3300_exit(void)
{
	free_irq(BCM_PERFCOUNT_IRQ, NULL);
}

struct op_mips_model op_model_bcm3300 = {
	.reg_setup	= BCM3300_reg_setup,
	.cpu_setup	= BCM3300_cpu_setup,
	.init		= BCM3300_init,
	.exit		= BCM3300_exit,
	.cpu_start	= BCM3300_cpu_start,
	.cpu_stop	= BCM3300_cpu_stop,
	.cpu_type	= "mips/bcm3300",
	.num_counters	= 4
};
