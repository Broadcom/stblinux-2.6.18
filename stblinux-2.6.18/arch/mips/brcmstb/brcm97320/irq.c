/*
 * arch/mips/brcm/irq.c
 *
 *  Copyright (C) 2001-2005 Broadcom Corporation
 *                    Steven J. Hill <shill@broadcom.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 * Interrupt routines for Broadcom eval boards
 *
 * 10-23-2001   SJH    Created
 * 10-25-2001   SJH    Added comments on interrupt handling
 */
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/brcmstb/brcm97320/bcm97320.h>
#include <asm/brcmstb/brcm97320/bcmintrnum.h>

#ifdef CONFIG_REMOTE_DEBUG
#include <asm/gdb-stub.h>
extern void breakpoint(void);
#endif

/* define front end and backend int bit groups */
#define BCM_BACKEND_INTS	(BGE_VEC1_IRQ|BGE_VEC0_IRQ|ADP_A_IRQ|ATP_A_IRQ|ADP_B_IRQ|ATP_B_IRQ|ATP_C_IRQ|ADP_C_IRQ|XPT_ICAM_IRQ|XPT_MSG_IRQ|XPT_OVFLOW_IRQ|XPT_STATUS_IRQ|MINITITAN_IRQ|BGE_IRQ)
#define BCM_FRONTEND_PHY_INTS 	(SDS_RX2_IRQ|SDS_RX1_IRQ|EC_BRIDGE_IRQ|PERIPH_POST_DONE_IRQ|UPG_IRQ)

extern asmlinkage unsigned int do_IRQ(int irq, struct pt_regs *regs);

/*
 * Following is the complete map of interrupts in the system. Please
 * keep this up to date and make sure you comment your changes in the
 * comment block above with the date, your initials and what you did.
 *
 * There are two different interrupts in the system. The first type
 * is an actual hardware interrupt. We have a total of eight MIPS
 * interrupts. Two of them are software interrupts and are ignored.
 * The remaining six interrupts are actually monitored and handled
 * by low-level interrupt code in 'int-handler.S' that call dispatch
 * functions in this file to call the IRQ descriptors in the Linux
 * kernel.
 * 
 * The second type is the Linux kernel system interrupt which is
 * a table of IRQ descriptors (see 'include/linux/irq.h' and
 * 'include/linux/interrupt.h') that relate the hardware interrupt
 * handler types to the software IRQ descriptors. This is where all
 * of the status information and function pointers are defined so
 * that registration, releasing, disabling and enabling of interrupts
 * can be performed. Multiple system interrupts can be tied to a
 * single hardware interrupt. Examining this file along with the
 * other three aforementioned files should solidify the concepts.
 *
 * The first table simply shows the MIPS IRQ mapping:
 *
 *   MIPS IRQ   Source
 *   --------   ------
 *       0      Software *Ignored*
 *       1      Software *Ignored*
 *       2      Hardware BRCMSTB chip Internal
 *       3      Hardware External *Unused*
 *       4      Hardware External *Unused*
 *       5      Hardware External *Unused*
 *       6      Hardware External *Unused*
 *       7      R4k timer 
 *
 * The second table shows the table of Linux system interrupt
 * descriptors and the mapping to the hardware IRQ sources:
 *
 *   System IRQ   MIPS IRQ   Source
 *   ----------   --------   ------
 *
 *        0          2        Chip Interrupt Controller/Dedicated Handler
 *      1- 32        2        The 32 Interrupt Controller Bits
 *       33          2        UARTA
 *       34          2        UARTB
 *     	 37,38	     2      Smart Card A and B
 *       60          7      R4k timer (used for master system time)
 *
 * Again, I cannot stress this enough, keep this table up to date!!!
 */

/*
 * INTC functions
 */
static void brcm_intc_enable(unsigned int irq)
{
	unsigned int shift = irq - 1;
	unsigned long flags;

	if (irq == BCM_LINUX_SCA_IRQ) {
//printk("Enable SCA\n");

	 	save_and_cli(flags);
		UPG_INTC->irqen_l |= UPG_SCA_IRQ;
		restore_flags(flags);
	 	return;
	}
	if (irq == BCM_LINUX_SCB_IRQ) {
//printk("Enable SCB\n");
	 	save_and_cli(flags);
		UPG_INTC->irqen_l |= UPG_SCB_IRQ;
		restore_flags(flags);
	 	return;
	}
	save_and_cli(flags);
	INTC->IrqMask |= (0x1UL<<shift);
	if (irq==BCM_LINUX_IDE0_IRQ)	/* jli- primary and secondary share IDE0 irq */
	{
		shift=BCM_LINUX_IDE1_IRQ - 1;
		INTC->IrqMask |= (0x1UL<<shift);
	}
	restore_flags(flags);
}

static void brcm_intc_disable(unsigned int irq)
{
	unsigned int shift = irq - 1;
	unsigned long flags;


	if (irq == BCM_LINUX_SCA_IRQ) {
		save_and_cli(flags);
		UPG_INTC->irqen_l &= ~UPG_SCA_IRQ;
		restore_flags(flags);
	 	return;
	}
	if (irq == BCM_LINUX_SCB_IRQ) {
	 	save_and_cli(flags);
		UPG_INTC->irqen_l &= ~UPG_SCB_IRQ;
		restore_flags(flags);
	 	return;
	}
	/* UPG IRQ is shared with UART, so do not disable it */
	if (irq == BCM_LINUX_UPG_IRQ)
	 	return;
	if (irq > 0 && irq <= 32) {
 		save_and_cli(flags);
		INTC->IrqMask &= (~(0x1UL<<shift));
		if (irq==BCM_LINUX_IDE0_IRQ)	/* jli- primary and secondary share IDE0 irq */
		{
			shift=BCM_LINUX_IDE1_IRQ - 1;
			INTC->IrqMask &= (~(0x1UL<<shift));
		}
		restore_flags(flags);
	}
}

static unsigned int brcm_intc_startup(unsigned int irq)
{ 
	brcm_intc_enable(irq);
	return 0; /* never anything pending */
}

static void brcm_intc_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		brcm_intc_enable(irq);
}

static void brcm_intc_ack(unsigned int irq)
{
	/* Do nothing */
}

/*
 * THT: These INTC disable the interrupt before calling the IRQ handler
 */
static struct hw_interrupt_type brcm_intc_type = {
	typename: "BCM INTC",
	startup: brcm_intc_startup,
	shutdown: brcm_intc_disable,
	enable: brcm_intc_enable,
	disable: brcm_intc_disable,
	ack: brcm_intc_disable,
	end: brcm_intc_end,
	NULL
};


static void brcm_intc2_end(unsigned int irq)
{
	/* Do nothing */
}

static void brcm_intc2_ack(unsigned int irq)
{
	/* Do nothing */
}

/*
 * THT: These INTC DO NOT disable the interrupt before calling the IRQ handler
 */

static struct hw_interrupt_type brcm_intc2_type = {
	typename: "BCM INTC2",
	startup: brcm_intc_startup,
	shutdown: brcm_intc_disable,
	enable: brcm_intc_enable,
	disable: brcm_intc_disable,
	ack: brcm_intc2_ack,
	end: brcm_intc2_end,
	NULL
};


/*
 * UART functions
 */
static void brcm_uart_enable(unsigned int irq)
{
	unsigned long flags;

	save_and_cli(flags);
	if (irq == BCM_LINUX_UARTA_IRQ)
		UPG_INTC->irqen_l |= UPG_UA_IRQ;
	else
		UPG_INTC->irqen_l |= UPG_UB_IRQ;
	restore_flags(flags);
}

static void brcm_uart_disable(unsigned int irq)
{
	unsigned long flags;

	save_and_cli(flags);
	if (irq == BCM_LINUX_UARTA_IRQ)
		UPG_INTC->irqen_l &= (~(UPG_UA_IRQ));
	else
		UPG_INTC->irqen_l &= (~(UPG_UB_IRQ));
	restore_flags(flags);
}

static unsigned int brcm_uart_startup(unsigned int irq)
{ 
	brcm_uart_enable(irq);

	return 0; /* never anything pending */
}

static void brcm_uart_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		brcm_uart_enable(irq);
}

static struct hw_interrupt_type brcm_uart_type = {
	typename: "BCM UART",
	startup: brcm_uart_startup,
	shutdown: brcm_uart_disable,
	enable: brcm_uart_enable,
	disable: brcm_uart_disable,
	ack: brcm_uart_disable,
	end: brcm_uart_end,
	NULL
};

/*
 * IRQ7 functions
 */
void brcm_mips_int7_dispatch(struct pt_regs *regs)
{		
	static char firstint = 0;
	if (firstint == 0)
	{
		printk("brcm timer int\n");
		firstint = 1;
	}
	do_IRQ(BCM_LINUX_SYSTIMER_IRQ, regs);
}

static void brcm_mips_int7_enable(unsigned int irq)
{
	set_c0_status(STATUSF_IP7);
}

static void brcm_mips_int7_disable(unsigned int irq)
{
	clear_c0_status(STATUSF_IP7);
}

static void brcm_mips_int7_ack(unsigned int irq)
{
	/* Already done in brcm_irq_dispatch */
}

static unsigned int brcm_mips_int7_startup(unsigned int irq)
{ 
	brcm_mips_int7_enable(irq);

	return 0; /* never anything pending */
}

static void brcm_mips_int7_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		brcm_mips_int7_enable(irq);
}

static struct hw_interrupt_type brcm_mips_int7_type = {
	typename: "BCM MIPS TIMER INT",
	startup: brcm_mips_int7_startup,
	shutdown: brcm_mips_int7_disable,
	enable: brcm_mips_int7_enable,
	disable: brcm_mips_int7_disable,
	ack: brcm_mips_int7_ack,
	end: brcm_mips_int7_end,
	NULL
};


/*
 * IRQ2 functions
 */

static void brcm_mips_int2_enable(unsigned int irq)
{
	set_c0_status(STATUSF_IP2);
#if 0  /* JPF */
	switch(irq)
	{
		case BCM_LINUX_IDE0_IRQ:
			INTC->IrqMask |= (IDE0_IRQ);
		break;

		case BCM_LINUX_IDE1_IRQ:
			INTC->IrqMask |= (IDE1_IRQ);
		break;

		case BCM_LINUX_USB_HOST1_IRQ:
			INTC->IrqMask |= (USB_HOST1_IRQ);
		break;

		case BCM_LINUX_UARTA_IRQ:
			// This has to change depending on the console tty device we choose to use 
			// (defined in cmdline).
			INTC->IrqMask |= UPG_IRQ;
			UPG_INTC->irqen_l |= UPG_UA_IRQ;
		break;

		default:
		break;
	}
#endif
}

static void brcm_mips_int2_disable(unsigned int irq)
{
	/* DO NOT DISABLE MIPS int2 so that we do not mess with other 
		int2 ints(THERE ARE A LOT OF THESE!). */
	clear_c0_status(STATUSF_IP2);
#if 0  /* JPF */
	switch(irq)
	{
		case BCM_LINUX_IDE0_IRQ:
			INTC->IrqMask &= (~(IDE0_IRQ));
		break;

		case BCM_LINUX_IDE1_IRQ:
			INTC->IrqMask &= (~(IDE1_IRQ));
		break;

		case BCM_LINUX_USB_HOST1_IRQ:
			INTC->IrqMask &= (~(USB_HOST1_IRQ));
		break;

		case BCM_LINUX_UARTA_IRQ:
			// This has to change depending on the console tty device we choose to use 
			// (defined in cmdline).
			// do NOT disable UPG int here! We may have something else enabled...
			// INTC->IrqMask &= (~(UPG_IRQ));
			UPG_INTC->irqen_l &= (~(UPG_UA_IRQ));
		break;

		default:
		break;
	}
#endif
}

static void brcm_mips_int2_ack(unsigned int irq)
{
	/* Already done in brcm_irq_dispatch */
}

static unsigned int brcm_mips_int2_startup(unsigned int irq)
{ 
	brcm_mips_int2_enable(irq);

	return 0; /* never anything pending */
}

static void brcm_mips_int2_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		brcm_mips_int2_enable(irq);
}

static struct hw_interrupt_type brcm_mips_int2_type = {
	typename: "BCM MIPS INT2",
	startup: brcm_mips_int2_startup,
	shutdown: brcm_mips_int2_disable,
	enable: brcm_mips_int2_enable,
	disable: brcm_mips_int2_disable,
	ack: brcm_mips_int2_ack,
	end: brcm_mips_int2_end,
	NULL
};

static int g_brcm_intc_cnt[64];
static int g_intcnt = 0;
void brcm_mips_int2_dispatch(struct pt_regs *regs)
{
    unsigned int pendingIrqs,shift,irq;
    
	brcm_mips_int2_disable(0);
        pendingIrqs = INTC->IrqStatus & INTC->IrqMask;
	/* Process the INTC 7320 IRQS */
	for (irq = 1; irq <= 32; ++irq) /* IRQs from 1 to 32 - 0 reserved for main IRQ and 60 for timer */
	{
		shift = irq -1;
		if ((0x1 << shift) & pendingIrqs)
		{
			if (irq == BCM_LINUX_UPG_IRQ) {
				if (UPG_INTC->irqstat_l & UPG_INTC->irqen_l & UPG_UA_IRQ) {
					do_IRQ(BCM_LINUX_UARTA_IRQ, regs);
				}
				else if (UPG_INTC->irqstat_l & UPG_INTC->irqen_l & UPG_SCA_IRQ) {
					//printk("Calling do_IRQ for SCA, INTC.stats=%08x, INTC.mask=%08x @%08x, called=%x\n", 
					 //	INTC->IrqStatus, INTC->IrqMask, &INTC->IrqStatus, doIrqCalled);
					 
					//doIrqCalled |=2;
					do_IRQ(BCM_LINUX_SCA_IRQ, regs);
				}
				else if (UPG_INTC->irqstat_l & UPG_INTC->irqen_l & UPG_SCB_IRQ) {
					//printk("Calling do_IRQ for SCB, INTC.stats=%08x, INTC.mask=%08x @%08x, called=%x\n", 
					 //	INTC->IrqStatus, INTC->IrqMask, &INTC->IrqStatus, doIrqCalled);
					//doIrqCalled |= 4;
					do_IRQ(BCM_LINUX_SCB_IRQ, regs);
				}
				else {
					do_IRQ(BCM_LINUX_UPG_IRQ, regs);
				}
			}
			else if ( irq == BCM_LINUX_IDE0_IRQ || irq == BCM_LINUX_IDE1_IRQ )	/* jli- shared IDE0 irq */
				do_IRQ(BCM_LINUX_IDE0_IRQ, regs);
			else
				do_IRQ(irq, regs);
			
			++g_brcm_intc_cnt[shift];
		}
	}
#if 0
	if (g_intcnt++ >= 0xFFFF)
	{
		g_intcnt = 0;
		for (irq = 1; irq <= 32; ++irq)
		{
			if (m_intc_cnt[irq - 1] != 0)
				printk("IRQ[%d] count = %d\n",irq,g_brcm_intc_cnt[irq - 1]);
		}
	}
#endif
	brcm_mips_int2_enable(0);
}

void brcm_mips_int3_dispatch(struct pt_regs *regs)
{
	printk("brcm_mips_int3_dispatch: Placeholder only, should not be here \n");
}


/*
 * IRQ6 functions
 */
static void brcm_mips_int6_enable(unsigned int irq)
{
	set_c0_status(STATUSF_IP6);
}

static void brcm_mips_int6_disable(unsigned int irq)
{
	clear_c0_status(STATUSF_IP6);
}

static void brcm_mips_int6_ack(unsigned int irq)
{
	/* Already done in brcm_irq_dispatch */
}

static unsigned int brcm_mips_int6_startup(unsigned int irq)
{ 
	brcm_mips_int6_enable(irq);

	return 0; /* never anything pending */
}

static void brcm_mips_int6_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		brcm_mips_int6_enable(irq);
}

static struct hw_interrupt_type brcm_mips_int6_type = {
	typename: "BCM MIPS INT6",
	startup: brcm_mips_int6_startup,
	shutdown: brcm_mips_int6_disable,
	enable: brcm_mips_int6_enable,
	disable: brcm_mips_int6_disable,
	ack: brcm_mips_int6_ack,
	end: brcm_mips_int6_end,
	NULL
};

void brcm_mips_int6_dispatch(struct pt_regs *regs)
{
	brcm_mips_int6_disable(6);
}


/*
 * Broadcom specific IRQ setup
 */
void __init brcm_irq_setup(void)
{
	int irq;
	extern asmlinkage void brcmIRQ(void);

	INTC->IrqMask = 0UL;
	INTC->IrqStatus = 0UL;
	
	set_except_vector(0, brcmIRQ);
	change_c0_status(ST0_IE, ST0_IE);
	
	/* Setup timer interrupt */
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].action = 0;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].depth = 1;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].handler = &brcm_mips_int7_type;

	/* Install all the 7xxx IRQs */
	for (irq = 1; irq <= 32; ++irq) /* IRQs from 1 to 32 - 0 reserved for main IRQ and 60 for timer */
	{
		irq_desc[irq].status = IRQ_DISABLED;
		irq_desc[irq].action = 0;
		irq_desc[irq].depth = 1;
		irq_desc[irq].handler = &brcm_intc_type;
		g_brcm_intc_cnt[irq -1] = 0;
	}
	/* Handle the Serial IRQs differently so they can have unique IRQs */
	irq_desc[BCM_LINUX_UARTA_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_UARTA_IRQ].action = 0;
	irq_desc[BCM_LINUX_UARTA_IRQ].depth = 1;
	irq_desc[BCM_LINUX_UARTA_IRQ].handler = &brcm_uart_type;

       /* Set up smartcard interrupts. */
        irq_desc[BCM_LINUX_SCA_IRQ].status = IRQ_DISABLED;
        irq_desc[BCM_LINUX_SCA_IRQ].action = 0;
        irq_desc[BCM_LINUX_SCA_IRQ].depth = 1;
        irq_desc[BCM_LINUX_SCA_IRQ].handler = &brcm_intc_type;
        
        irq_desc[BCM_LINUX_SCB_IRQ].status = IRQ_DISABLED;
        irq_desc[BCM_LINUX_SCB_IRQ].action = 0;
        irq_desc[BCM_LINUX_SCB_IRQ].depth = 1;
        irq_desc[BCM_LINUX_SCB_IRQ].handler = &brcm_intc_type;

	brcm_mips_int2_enable(0);
	INTC->IrqMask |= 0x1UL << (BCM_LINUX_UPG_IRQ -1);

}

void (*irq_setup)(void);

void __init init_IRQ(void)
{
	extern void __init init_generic_irq(void);

	init_generic_irq();
	irq_setup();
}
