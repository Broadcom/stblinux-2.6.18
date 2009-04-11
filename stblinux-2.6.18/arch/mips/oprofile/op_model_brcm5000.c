/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2004, 2005 by Ralf Baechle
 * Copyright (C) 2005 by MIPS Technologies, Inc.
 */
#include <linux/oprofile.h>
#include <linux/interrupt.h>
#include <linux/smp.h>

#include "op_impl.h"

#define M_PERFCTL_EXL			(1UL      <<  0)
#define M_PERFCTL_KERNEL		(1UL      <<  1)
#define M_PERFCTL_SUPERVISOR		(1UL      <<  2)
#define M_PERFCTL_USER			(1UL      <<  3)
#define M_PERFCTL_INTERRUPT_ENABLE	(1UL      <<  4)
#define M_PERFCTL_EVENT(event)		((event)  << 5)
#define M_PERFCTL_EVENT_MASK		(0x3f     << 5)
#define M_PERFCTL_ASID(idx)		((idx)    << 12)
#define M_PERFCTL_ASID_MASK	 	(0xff	  << 12)
#define M_PERFCTL_IDX(idx)		((idx)    << 20)
#define M_PERFCTL_IDX_MASK	 	(0xff	  << 20)
#define M_PERFCTL_IDXEN(idxen)		((idxen)  << 28)
#define M_PERFCTL_IDXEN_MASK 		(3UL      << 28)
#define M_PERFCTL_ERL			(1UL      << 29)
#define M_PERFCTL_TC			(1UL      << 30)
#define M_PERFCTL_MORE			(1UL      << 31)

#define M_COUNTER_OVERFLOW		(1UL      << 31)

#ifdef CONFIG_MIPS_MT_SMP
#define WHAT		(M_TC_EN_VPE | M_PERFCTL_VPEID(smp_processor_id()))
#define vpe_id()	smp_processor_id()
#else
#define WHAT		0
#define vpe_id()	smp_processor_id()
#endif

#define __define_perf_accessors(r, n, np)				\
									\
static inline unsigned int r_c0_ ## r ## n(void)			\
{									\
	unsigned int cpu = vpe_id();					\
									\
	switch (cpu) {							\
	case 0:								\
		return read_c0_ ## r ## n();				\
	case 1:								\
		return read_c0_ ## r ## np();				\
	default:							\
		BUG();							\
	}								\
	return 0;							\
}									\
									\
static inline void w_c0_ ## r ## n(unsigned int value)			\
{									\
	unsigned int cpu = vpe_id();					\
									\
	switch (cpu) {							\
	case 0:								\
		write_c0_ ## r ## n(value);				\
		return;							\
	case 1:								\
		write_c0_ ## r ## np(value);				\
		return;							\
	default:							\
		BUG();							\
	}								\
	return;								\
}									\

__define_perf_accessors(perfcntr, 0, 2)
__define_perf_accessors(perfcntr, 1, 3)
__define_perf_accessors(perfcntr, 2, 2)
__define_perf_accessors(perfcntr, 3, 2)

__define_perf_accessors(perfctrl, 0, 2)
__define_perf_accessors(perfctrl, 1, 3)
__define_perf_accessors(perfctrl, 2, 2)
__define_perf_accessors(perfctrl, 3, 2)

struct op_mips_model op_model_bcm5000_ops;

static struct bcm5000_register_config {
	unsigned int control[4];
	unsigned int counter[4];
} reg;

/* Compute all of the registers in preparation for enabling profiling.  */

static void bcm5000_reg_setup(struct op_counter_config *ctr)
{
	unsigned int counters = op_model_bcm5000_ops.num_counters;
	int i;

	/* Compute the performance counter control word.  */
	/* For now count kernel and user mode */
	for (i = 0; i < counters; i++) {
		reg.control[i] = 0;
		reg.counter[i] = 0;

		if (!ctr[i].enabled)
			continue;

		reg.control[i] = M_PERFCTL_EVENT(ctr[i].event) |
		                 M_PERFCTL_INTERRUPT_ENABLE;
		if (ctr[i].kernel)
			reg.control[i] |= M_PERFCTL_KERNEL;
		if (ctr[i].user)
			reg.control[i] |= M_PERFCTL_USER;
		if (ctr[i].exl)
			reg.control[i] |= M_PERFCTL_EXL;
		reg.counter[i] = 0x80000000 - ctr[i].count;
//	printk("setup perf cntrl reg.control[%d] = 0x%x \r\n", i, reg.control[i] );
	}
}

/* Program all of the registers in preparation for enabling profiling.  */

static void bcm5000_cpu_setup (void *args)
{
	unsigned int counters = op_model_bcm5000_ops.num_counters;

	switch (counters) {
	case 4:
		w_c0_perfctrl3(0);
		w_c0_perfcntr3(0);
//		printk("write perf count 3 = 0x%x \r\n", reg.counter[3]);
	case 3:
		w_c0_perfctrl2(0);
		w_c0_perfcntr2(0);
//		printk("write perf count 2 = 0x%x \r\n", reg.counter[2]);
	case 2:
		w_c0_perfctrl1(0);
		w_c0_perfcntr1(0);
//		printk("write perf count 1 = 0x%x \r\n", reg.counter[1]);
	case 1:
		w_c0_perfctrl0(0);
		w_c0_perfcntr0(0);
//		printk("write perf count 0 = 0x%x \r\n", reg.counter[0]);
	}
}
extern int performance_enabled;

/* Start all counters on current CPU */
static void bcm5000_cpu_start(void *args)
{
	unsigned int counters = op_model_bcm5000_ops.num_counters;

	performance_enabled = 1;
// printk("bcm5000_cpu_start +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");

	switch (counters) {
	case 4:
		w_c0_perfctrl3(reg.control[3]);
		w_c0_perfcntr3(reg.counter[3]);
	case 3:
		w_c0_perfctrl2(reg.control[2]);
		w_c0_perfcntr2(reg.counter[2]);
	case 2:
		w_c0_perfctrl1(reg.control[1]);
		w_c0_perfcntr1(reg.counter[1]);
	case 1:
		w_c0_perfctrl0(reg.control[0]);
		w_c0_perfcntr0(reg.counter[0]);
	}

//	set_c0_status(0x2000);
}

/* Stop all counters on current CPU */
static void bcm5000_cpu_stop(void *args)
{
	unsigned int counters = op_model_bcm5000_ops.num_counters;

	performance_enabled = 0;
// printk("bcm5000_cpu_stop --------------------------------------------------------------------r\n");

	switch (counters) {
	case 4:
		w_c0_perfctrl3(0);
		w_c0_perfcntr3(0);
	case 3:
		w_c0_perfctrl2(0);
		w_c0_perfcntr2(0);
	case 2:
		w_c0_perfctrl1(0);
		w_c0_perfcntr1(0);
	case 1:
		w_c0_perfctrl0(0);
		w_c0_perfcntr0(0);
	}
}

static int bcm5000_perfcount_handler(struct pt_regs *regs)
{
	unsigned int counters = op_model_bcm5000_ops.num_counters;
	unsigned int control;
	unsigned int counter;
	int handled = 0;

	switch (counters) {
#define HANDLE_COUNTER(n)						\
	case n + 1:							\
		control = r_c0_perfctrl ## n();				\
		counter = r_c0_perfcntr ## n();				\
		if ((control & M_PERFCTL_INTERRUPT_ENABLE) &&		\
		    (counter & M_COUNTER_OVERFLOW)) {			\
			oprofile_add_sample(regs, n);			\
			w_c0_perfcntr ## n(reg.counter[n]);		\
			handled = 1;					\
		}
	HANDLE_COUNTER(3)
	HANDLE_COUNTER(2)
	HANDLE_COUNTER(1)
	HANDLE_COUNTER(0)
	}

	return handled;
}

#define M_CONFIG1_PC	(1 << 4)

static inline int __n_counters(void)
{
	if (!(read_c0_config1() & M_CONFIG1_PC))
		return 0;
	if (!(r_c0_perfctrl0() & M_PERFCTL_MORE))
		return 1;
	if (!(r_c0_perfctrl1() & M_PERFCTL_MORE))
		return 2;
	if (!(r_c0_perfctrl2() & M_PERFCTL_MORE))
		return 3;

	return 4;
}

static inline int n_counters(void)
{
	int counters = __n_counters();

#ifdef CONFIG_MIPS_MT_SMP
	if (current_cpu_data.cputype == CPU_34K)
		return counters >> 1;
#endif

	return counters;
}

static inline void reset_counters(int counters)
{
	switch (counters) {
	case 4:
		w_c0_perfctrl3(0);
		w_c0_perfcntr3(0);
	case 3:
		w_c0_perfctrl2(0);
		w_c0_perfcntr2(0);
	case 2:
		w_c0_perfctrl1(0);
		w_c0_perfcntr1(0);
	case 1:
		w_c0_perfctrl0(0);
		w_c0_perfcntr0(0);
	}
}

static int __init bcm5000_init(void)
{
	int counters;

	counters = n_counters();
	if (counters == 0) {
		printk(KERN_ERR "Oprofile: CPU has no performance counters\n");
		return -ENODEV;
	}

	reset_counters(counters);

	op_model_bcm5000_ops.num_counters = counters;
	switch (current_cpu_data.cputype) {
	case CPU_20KC:
		op_model_bcm5000_ops.cpu_type = "mips/20K";
		break;

	case CPU_24K:
		op_model_bcm5000_ops.cpu_type = "mips/24K";
		break;

	case CPU_25KF:
		op_model_bcm5000_ops.cpu_type = "mips/25K";
		break;

	case CPU_34K:
		op_model_bcm5000_ops.cpu_type = "mips/34K";
		break;

	case CPU_74K:
		op_model_bcm5000_ops.cpu_type = "mips/74K";
		break;

	case CPU_5KC:
		op_model_bcm5000_ops.cpu_type = "mips/5K";
		break;

	case CPU_SB1:
	case CPU_SB1A:
		op_model_bcm5000_ops.cpu_type = "mips/sb1";
		break;

        case CPU_BMIPS5000:
		op_model_bcm5000_ops.cpu_type = "mips/bcm5000";
		break;

	default:
		printk(KERN_ERR "Profiling unsupported for this CPU\n");

		return -ENODEV;
	}

	perf_irq = bcm5000_perfcount_handler;

	return 0;
}


int test_all_counters(void)
{
	int overflow = 0;
	unsigned int control, counter;

#define CHECK_COUNTER(n)						\
		control = r_c0_perfctrl ## n();				\
		counter = r_c0_perfcntr ## n();				\
		if ((control & M_PERFCTL_INTERRUPT_ENABLE) &&		\
		    (counter & M_COUNTER_OVERFLOW)) {			\
			overflow = 1;					\
		}
	CHECK_COUNTER(0)
	CHECK_COUNTER(1)
	CHECK_COUNTER(2)
	CHECK_COUNTER(3)
   return(overflow);
}


static void bcm5000_exit(void)
{
	performance_enabled = 0;
	reset_counters(op_model_bcm5000_ops.num_counters);

	perf_irq = null_perf_irq;
}

struct op_mips_model op_model_bcm5000_ops = {
	.reg_setup	= bcm5000_reg_setup,
	.cpu_setup	= bcm5000_cpu_setup,
	.init		= bcm5000_init,
	.exit		= bcm5000_exit,
	.cpu_start	= bcm5000_cpu_start,
	.cpu_stop	= bcm5000_cpu_stop,
};
