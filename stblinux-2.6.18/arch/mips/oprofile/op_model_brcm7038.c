/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2005 Broadcom Corp by Troy Trammel
 * Copyright (C) 2004 by Ralf Baechle
 */
#include <linux/init.h>
#include <linux/oprofile.h>
#include <linux/interrupt.h>
#include <linux/smp.h>

#include <asm/brcmstb/brcm97038c0/int1.h>
#include <asm/brcmstb/brcm97038c0/board.h>
#include <asm/brcmstb/brcm97038c0/bchp_irq0.h>
#include <asm/brcmstb/brcm97038c0/bcmintrnum.h>

#include "op_impl.h"

#define BCM7038_COUNTER_EVENT(event)	((event) << 5)
#define BCM7038_COUNTER_ENABLE			(1    <<  4)
#define BCM7038_COUNTER_USER			(1    <<  3)
#define BCM7038_COUNTER_SUPERVISOR		(1    <<  2)
#define BCM7038_COUNTER_KERNEL			(1    <<  1)

#define BCM7038_COUNTER_OVERFLOW		(1    << 31)


static struct BCM7038_register_config {
	unsigned int control1;
	unsigned int control2;
	unsigned int reset_counter1;
	unsigned int reset_counter2;
} reg;

/* Compute all of the registers in preparation for enabling profiling.  */

static void bcm7038_reg_setup(struct op_counter_config *ctr)
{
	reg.control1 = 0x0;
	reg.control2 = 0x0;
	reg.reset_counter1 = 0x0;
	reg.reset_counter2 = 0x0;

	/* Compute the performance counter control word.  */
	/* For now count kernel and user mode */
	if (ctr[0].enabled)
	{
		reg.control1 = BCM7038_COUNTER_EVENT(ctr[0].event) |
		           BCM7038_COUNTER_KERNEL |
		           BCM7038_COUNTER_USER |
		           BCM7038_COUNTER_ENABLE;
		reg.reset_counter1 = 0x80000000 - ctr[0].count;
	}
	if (ctr[1].enabled)
	{
		reg.control2 |= BCM7038_COUNTER_EVENT(ctr[1].event) |
		           BCM7038_COUNTER_KERNEL |
		           BCM7038_COUNTER_USER |
		           BCM7038_COUNTER_ENABLE;
		reg.reset_counter2 = 0x80000000 - ctr[1].count;
	}
}

/* Program all of the registers in preparation for enabling profiling.  */

static void bcm7038_cpu_setup (void *args)
{
	write_c0_perfcntr0(reg.reset_counter1);
	write_c0_perfcntr1(reg.reset_counter2);
}

static void bcm7038_cpu_start(void *args)
{
	/* Start all counters on current CPU */
	write_c0_perfctrl0(reg.control1);
	write_c0_perfctrl1(reg.control2);

}

static void bcm7038_cpu_stop(void *args)
{
	/* Stop all counters on current CPU */
	write_c0_perfctrl0(0);
	write_c0_perfctrl1(0);
}

static irqreturn_t bcm7038_perfcount_handler(int irq, void * dev_id,
	struct pt_regs *regs)
{
	uint32_t counter1, counter2;

	/*
	 * To avoid a race updating the
	 * registers we need to stop the counters while we're messing with
	 * them ...
	 */
	counter1 = read_c0_perfcntr0();
	counter2 = read_c0_perfcntr1();

	if (counter1 & BCM7038_COUNTER_OVERFLOW) {
		oprofile_add_sample(regs, 0);
		write_c0_perfcntr0(reg.reset_counter1);
	}
	if (counter2 & BCM7038_COUNTER_OVERFLOW) {
 		oprofile_add_sample(regs, 1);
		write_c0_perfcntr1(reg.reset_counter2);
	}

	/* Performance interrupts disabled in irq.c, we must re-enable at the end of this routine*/
	write_c0_perfctrl0(reg.control1);
	write_c0_perfctrl1(reg.control2);

	return IRQ_HANDLED;
}

static int __init bcm7038_init(void)
{
	return request_irq(BCM_PERFCOUNT_IRQ, bcm7038_perfcount_handler,
	                   0, "Perfcounter", NULL);
}

static void bcm7038_exit(void)
{
	free_irq(BCM_PERFCOUNT_IRQ, NULL);
}

struct op_mips_model op_model_bcm7038 = {
	.reg_setup	= bcm7038_reg_setup,
	.cpu_setup	= bcm7038_cpu_setup,
	.init		= bcm7038_init,
	.exit		= bcm7038_exit,
	.cpu_start	= bcm7038_cpu_start,
	.cpu_stop	= bcm7038_cpu_stop,
	.cpu_type	= "mips/bcm7038",
	.num_counters	= 2
};
