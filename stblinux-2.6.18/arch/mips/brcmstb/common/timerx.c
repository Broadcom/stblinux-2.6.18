/*
<:copyright-gpl
 Copyright 2006 Broadcom Corp. All Rights Reserved.

 This program is free software; you can distribute it and/or modify it
 under the terms of the GNU General Public License (Version 2) as
 published by the Free Software Foundation.

 This program is distributed in the hope it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.

 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
:>
*/
/*
	Setup timer interrupt handlers for secondary CPUs (threads). This allows
	us to not have to modify the main mips timer code.
when	who what
-----	---	----
3/24/06	TDT	Created
*/
#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/timex.h>

#include <asm/mipsregs.h>
#include <asm/brcmstb/common/brcmstb.h>

// SMP Version
#ifdef CONFIG_SMP

void local_timer_interrupt(int , void *, struct pt_regs *);

extern unsigned int mips_hpt_frequency;

/* how many counter cycles in a jiffy */
static unsigned long cycles_per_jiffy;

/* expirelo is the count value for next CPU timer interrupt */
static unsigned int expirelo;

/*
 * Timer ack for an R4k-compatible timer of a known frequency.
 */
static void c0_timer_ack(void)
{
	unsigned int count;

	/* Ack this timer interrupt and set the next one.  */
	expirelo += cycles_per_jiffy;
	write_c0_compare(expirelo);

	/* Check to see if we have missed any timer interrupts.  */
	count = read_c0_count();
	if ((count - expirelo) < 0x7fffffff) {
		/* need to try to keep the count register on both TPs synched */
		unsigned int missed = (count - expirelo) / cycles_per_jiffy;

		expirelo += cycles_per_jiffy * (missed + 2);
		write_c0_compare(expirelo);
	}
}

/*
 * High-level timer interrupt service routines.  This function
 * is set as irqaction->handler and is invoked through do_IRQ.
 */
irqreturn_t timerx_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	c0_timer_ack();

	/*
	 * In UP mode, we call local_timer_interrupt() to do profiling
	 * and process accouting.
	 *
	 * In SMP mode, local_timer_interrupt() is invoked by appropriate
	 * low-level local timer interrupt handler.
	 */
	local_timer_interrupt(irq, dev_id, regs);

	return IRQ_HANDLED;
}

static unsigned long cp0_count, cp0_compare;

/* save TP0 timer state in a shared variable */
void brcm_save_timer(void)
{
	unsigned long jif_start = jiffies;
	
	while(jif_start == jiffies) { }
	cp0_compare = read_c0_compare();
	cp0_count = read_c0_count();
}

#define FUDGE		100

/* copy TP0 timer state to TP1 CP0 */
void brcm_sync_timer(void)
{
	write_c0_count(cp0_count + FUDGE);
	write_c0_compare(cp0_compare);
	expirelo = cp0_compare;
	set_c0_status(IE_IRQ5);
}

void __init brcm_timerx_setup(struct irqaction *irq)
{
	/* Needs to be calculated by first cpu */
	BUG_ON(!mips_hpt_frequency);

	/* Calculate cache parameters.  */
	cycles_per_jiffy = (mips_hpt_frequency + HZ / 2) / HZ;
	
	/* we are using the cpu counter for timer interrupts */
	irq->dev_id = (void *) irq;

	setup_irq(BCM_LINUX_SYSTIMER_1_IRQ, irq);
	
	/* don't enable until we have synched with TP0 */
}
#endif
