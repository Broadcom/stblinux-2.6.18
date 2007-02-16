/*
 * arch/mips/brcm/irq.c
 *
 * Copyright (C) 2001-2006 Broadcom Corporation
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
 * 09-29-2003   QY     Added support for bcm97038
 * 06-03-2005   THT    Ported to 2.6.12
 * 09-27-2006   TDT    Ported to 2.6.18
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/brcmstb/brcm97038c0/bcmuart.h>
#include <asm/brcmstb/brcm97038c0/bcmtimer.h>
#include <asm/brcmstb/brcm97038c0/bcmebi.h>
#include <asm/brcmstb/brcm97038c0/int1.h>
#include <asm/brcmstb/brcm97038c0/board.h>
#include <asm/brcmstb/brcm97038c0/bchp_irq0.h>
#include <asm/brcmstb/brcm97038c0/bcmintrnum.h>

#ifdef CONFIG_REMOTE_DEBUG
#include <asm/gdb-stub.h>
extern void breakpoint(void);
#endif

/* define front end and backend int bit groups */
//#define BCM_BACKEND_INTS	(BGE_VEC1_IRQ|BGE_VEC0_IRQ|ADP_A_IRQ|ATP_A_IRQ|ADP_B_IRQ|ATP_B_IRQ|ATP_C_IRQ|ADP_C_IRQ|XPT_ICAM_IRQ|XPT_MSG_IRQ|XPT_OVFLOW_IRQ|XPT_STATUS_IRQ|MINITITAN_IRQ|BGE_IRQ)
//#define BCM_FRONTEND_PHY_INTS 	(SDS_RX2_IRQ|SDS_RX1_IRQ|EC_BRIDGE_IRQ|PERIPH_POST_DONE_IRQ|UPG_IRQ)

#define HYDRA_UART0_INTR_MASK  BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_UART0_CPU_INTR_MASK

#define  BCM_UPG_IRQ0_IRQEN   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IRQ0_IRQEN)
#define  BCM_UPG_IRQ0_IRQSTAT   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IRQ0_IRQSTAT)


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
	unsigned int shift;
	unsigned long flags;

	local_irq_save(flags);
	if (irq > 0 && irq <= BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT)
	{
		shift = irq - 1;
		CPUINT1C->IntrW0MaskClear = (0x1UL<<shift);
		if (irq == BCM_LINUX_CPU_ENET_IRQ)
		{
			//*((volatile unsigned long *)0xb008241c) |= 0x2;
			*((volatile unsigned long *)0xb0082424) |= 0x2;
		}
	}
	else if (irq > BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT && 
			irq <= BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT+32)
	{
		shift = irq - BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT -1;
		CPUINT1C->IntrW1MaskClear = (0x1UL<<shift);
	}
	local_irq_restore(flags);

#if 0
	if (irq == BCM_LINUX_SCA_IRQ) {
//printk("Enable SCA\n");

	 	local_irq_save(flags);
		UPG_INTC->irqen_l |= UPG_SCA_IRQ;
		local_irq_restore(flags);
	 	return;
	}
	if (irq == BCM_LINUX_SCB_IRQ) {
//printk("Enable SCB\n");
	 	local_irq_save(flags);
		UPG_INTC->irqen_l |= UPG_SCB_IRQ;
		local_irq_restore(flags);
	 	return;
	}
	local_irq_save(flags);
	INTC->IrqMask |= (0x1UL<<shift);
	if (irq==BCM_LINUX_IDE0_IRQ)	/* jli- primary and secondary share IDE0 irq */
	{
		shift=BCM_LINUX_IDE1_IRQ - 1;
		INTC->IrqMask |= (0x1UL<<shift);
	}
	local_irq_restore(flags);
#endif
}

static void brcm_intc_disable(unsigned int irq)
{
	unsigned int shift;
	unsigned long flags;

	local_irq_save(flags);
	if (irq > 0 && irq <= BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT)
	{
		shift = irq - 1;
		CPUINT1C->IntrW0MaskSet = (0x1UL<<shift);
		if (irq == BCM_LINUX_CPU_ENET_IRQ)
		{
			//*((volatile unsigned long *)0xb008241c) &= ~0x2;
			*((volatile unsigned long *)0xb0082424) &= ~0x2;
		}
	}
	else if (irq > BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT && 
			irq <= BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT+32)
	{
		shift = irq - BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT -1;
		CPUINT1C->IntrW1MaskSet = (0x1UL<<shift);
	}
	local_irq_restore(flags);

#if 0

	if (irq == BCM_LINUX_SCA_IRQ) {
		local_irq_save(flags);
		UPG_INTC->irqen_l &= ~UPG_SCA_IRQ;
		local_irq_restore(flags);
	 	return;
	}
	if (irq == BCM_LINUX_SCB_IRQ) {
	 	local_irq_save(flags);
		UPG_INTC->irqen_l &= ~UPG_SCB_IRQ;
		local_irq_restore(flags);
	 	return;
	}
	/* UPG IRQ is shared with UART, so do not disable it */
	if (irq == BCM_LINUX_UPG_IRQ)
	 	return;
	if (irq > 0 && irq <= 32) {
 		local_irq_save(flags);
		INTC->IrqMask &= (~(0x1UL<<shift));
		if (irq==BCM_LINUX_IDE0_IRQ)	/* jli- primary and secondary share IDE0 irq */
		{
			shift=BCM_LINUX_IDE1_IRQ - 1;
			INTC->IrqMask &= (~(0x1UL<<shift));
		}
		local_irq_restore(flags);
	}
#endif
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

/*
 * THT: These INTC disable the interrupt before calling the IRQ handler
 */
static struct irq_chip brcm_intc_type = {
	name: "BCM INTC",
	startup: brcm_intc_startup,
	shutdown: brcm_intc_disable,
	enable: brcm_intc_enable,
	disable: brcm_intc_disable,
	ack: brcm_intc_disable,
	end: brcm_intc_end,
	NULL
};

/*
 * UART functions
 */
static void brcm_uart_enable(unsigned int irq)
{
	unsigned long flags;

	local_irq_save(flags);
	if (irq == BCM_LINUX_UARTA_IRQ)
	{
		CPUINT1C->IntrW0MaskClear = HYDRA_UART0_INTR_MASK;
		//*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) |= BCHP_IRQ0_IRQEN_uarta_irqen_MASK;
	}
	else if (irq == BCM_LINUX_UARTB_IRQ)
	{
//printk("$$$$$$$$ UART B irq enabled. %d \n", irq);
		CPUINT1C->IntrW0MaskClear = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_MASK;
		*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) |= BCHP_IRQ0_IRQEN_ub_irqen_MASK;
	}
#if 0
	else
		UPG_INTC->irqen_l |= UPG_UB_IRQ;
#endif
	local_irq_restore(flags);
}

static void brcm_uart_disable(unsigned int irq)
{
	unsigned long flags;

	local_irq_save(flags);
	if (irq == BCM_LINUX_UARTA_IRQ)
	{
		CPUINT1C->IntrW0MaskSet = HYDRA_UART0_INTR_MASK;
		//*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) &= ~(BCHP_IRQ0_IRQEN_uarta_irqen_MASK);
	}
	else if (irq == BCM_LINUX_UARTB_IRQ)
	{
//printk("########## UART B irq Disabled. %d \n", irq);
		//CPUINT1C->IntrW0MaskSet = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_MASK;
		*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) &= ~(BCHP_IRQ0_IRQEN_ub_irqen_MASK);
	}
#if 0
	else
		UPG_INTC->irqen_l &= (~(UPG_UB_IRQ));
#endif
	local_irq_restore(flags);
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

static struct irq_chip brcm_uart_type = {
	name: "BCM UART",
	startup: brcm_uart_startup,
	shutdown: brcm_uart_disable,
	enable: brcm_uart_enable,
	disable: brcm_uart_disable,
	ack: brcm_uart_disable,
	end: brcm_uart_end,
	NULL
};

/*
 * Performance functions
 */
#ifdef CONFIG_OPROFILE
static int performance_enabled = 0;
static void brcm_mips_performance_enable(unsigned int irq)
{
	/* Interrupt line shared with timer so don't really enable/disable it */
	performance_enabled = 1;
}
static void brcm_mips_performance_disable(unsigned int irq)
{
	/* Interrupt line shared with timer so don't really enable/disable it */
	performance_enabled = 0;
}

static void brcm_mips_performance_ack(unsigned int irq)
{
	/* Already done in brcm_irq_dispatch */
}

static unsigned int brcm_mips_performance_startup(unsigned int irq)
{ 
	brcm_mips_performance_enable(irq);

	return 0; /* never anything pending */
}

static void brcm_mips_performance_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		brcm_mips_performance_enable(irq);
}

static struct hw_interrupt_type brcm_mips_performance_type = {
	typename: "BCM MIPS PERF",
	startup: brcm_mips_performance_startup,
	shutdown: brcm_mips_performance_disable,
	enable: brcm_mips_performance_enable,
	disable: brcm_mips_performance_disable,
	ack: brcm_mips_performance_ack,
	end: brcm_mips_performance_end,
	NULL
};

void brcm_mips_performance_dispatch(struct pt_regs *regs)
{
	if(performance_enabled)  do_IRQ(BCM_PERFCOUNT_IRQ, regs);
}

#endif


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
#ifdef CONFIG_OPROFILE
	if( ((read_c0_perfctrl0()&(1<<4)) &&  (read_c0_perfcntr0()&(1<<31))) ||
	    ((read_c0_perfctrl1()&(1<<4)) &&  (read_c0_perfcntr1()&(1<<31))) ) 
	{
		unsigned long control;
		/* Disable performance interrupts right away to avoid interrupt floods */
		control = read_c0_perfctrl0();
		control &= ~(1<<4);
		write_c0_perfctrl0(control);
		control = read_c0_perfctrl1();
		control &= ~(1<<4);
		write_c0_perfctrl1(0);

		brcm_mips_performance_dispatch(regs);
	}
	else
#endif
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

static struct irq_chip brcm_mips_int7_type = {
	name: "BCM MIPS TIMER",
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
}

static void brcm_mips_int2_disable(unsigned int irq)
{
	/* DO NOT DISABLE MIPS int2 so that we do not mess with other 
		int2 ints(THERE ARE A LOT OF THESE!). */
	clear_c0_status(STATUSF_IP2);
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

static struct irq_chip brcm_mips_int2_type = {
	name: "BCM MIPS INT2",
	startup: brcm_mips_int2_startup,
	shutdown: brcm_mips_int2_disable,
	enable: brcm_mips_int2_enable,
	disable: brcm_mips_int2_disable,
	ack: brcm_mips_int2_ack,
	end: brcm_mips_int2_end,
	NULL
};

static int g_brcm_intc_cnt[64];
static int gDebugPendingIrq0, gDebugPendingIrq1;
void brcm_mips_int2_dispatch(struct pt_regs *regs)
{
    unsigned int pendingIrqs,pendingIrqs1, shift,irq;
    
	brcm_mips_int2_disable(0);

	pendingIrqs = CPUINT1C->IntrW0Status;
	gDebugPendingIrq0 = pendingIrqs &= ~(CPUINT1C->IntrW0MaskStatus);

	pendingIrqs1 = CPUINT1C->IntrW1Status;
	gDebugPendingIrq1 = pendingIrqs1 &= ~(CPUINT1C->IntrW1MaskStatus);

	//if (pendingIrqs == HYDRA_UART0_INTR_MASK)
	//	do_IRQ(BCM_LINUX_UARTA_IRQ, regs);
	//else
	//	printk("unsolicited interrupt!!!\n");

	for (irq=1; irq<=BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT; irq++)
	{
		shift = irq-1;
		if ((0x1 << shift) & pendingIrqs)
		{
			if (shift == BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_UART0_CPU_INTR_SHIFT)
				do_IRQ(BCM_LINUX_UARTA_IRQ, regs);
			else if (irq == BCM_LINUX_CPU_ENET_IRQ)
			{
				//if (*((volatile unsigned long *)0xb0082418) & 0x2 )
				if (*((volatile unsigned long *)0xb0082420) & *((volatile unsigned long *)0xb0082424) & 0x2 )
					do_IRQ(BCM_LINUX_CPU_ENET_IRQ, regs);
				else
					printk("unsolicited ENET interrupt!!!\n");
			}
			else if ((shift == BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_SHIFT) 
					&& (*((volatile unsigned long*)BCM_UPG_IRQ0_IRQSTAT) & BCHP_IRQ0_IRQSTAT_ubirq_MASK) 
					&& (*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) & BCHP_IRQ0_IRQEN_ub_irqen_MASK) )
			{
//printk("@@@@@@@UARTB IRQ %d\n", irq);				
				do_IRQ(BCM_LINUX_UARTB_IRQ, regs);
			}
			else
				do_IRQ(irq, regs);
		}
	}

	for (irq = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT+1; irq <= BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT+32; irq++)
	{
		shift = irq - BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT -1;
		if ((0x1 << shift) & pendingIrqs1)
			do_IRQ(irq, regs);
	}
	
	brcm_mips_int2_enable(0);
}


void dump_INTC_regs(void)
{
	unsigned int pendingIrqs,pendingIrqs1;


	pendingIrqs = CPUINT1C->IntrW0Status;
	pendingIrqs &= ~(CPUINT1C->IntrW0MaskStatus);
	pendingIrqs1 = CPUINT1C->IntrW1Status;
	pendingIrqs1 &= ~(CPUINT1C->IntrW1MaskStatus);

	printk("last pending0=%08x, last pending1=%08x, curPending=%08x, curPending1=%08x\n", 
		gDebugPendingIrq0, gDebugPendingIrq1, pendingIrqs, pendingIrqs1);

		
}
EXPORT_SYMBOL(dump_INTC_regs);

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

static struct irq_chip brcm_mips_int6_type = {
	name: "BCM MIPS INT6",
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

struct irq_chip *dummy_chip_ref[] = {
	/* eliminates compiler warnings */
&brcm_mips_int2_type,
&brcm_mips_int6_type
};

/*
 * Broadcom specific IRQ setup
 */
void __init brcm_irq_setup(void)
{
	int irq;
	extern int noirqdebug;

printk("timer irq %d end %d\n",BCM_LINUX_SYSTIMER_IRQ, BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT+32);
	//INTC->IrqMask = 0UL;
	//INTC->IrqStatus = 0UL;
	CPUINT1C->IntrW0MaskSet = ~(BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_MASK);
	CPUINT1C->IntrW1MaskSet = 0xffffffff; //~(BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_reserved0_MASK);
	
	change_c0_status(ST0_IE, 0);
 	
	/* Setup timer interrupt */
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].action = 0;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].depth = 1;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].chip = &brcm_mips_int7_type;

	/* Install all the 7xxx IRQs */
	for (irq = 1; irq <= BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT; irq++) 
	{
		irq_desc[irq].status = IRQ_DISABLED;
		irq_desc[irq].action = 0;
		irq_desc[irq].depth = 1;
		irq_desc[irq].chip = &brcm_intc_type;
		g_brcm_intc_cnt[irq -1] = 0;
	}
	for (irq = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT+1; irq <= BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_reserved0_SHIFT+32; irq++)
	{
		irq_desc[irq].status = IRQ_DISABLED;
		irq_desc[irq].action = 0;
		irq_desc[irq].depth = 1;
		irq_desc[irq].chip = &brcm_intc_type;
		g_brcm_intc_cnt[irq -1] = 0;
	}

	/* Handle the Serial IRQs differently so they can have unique IRQs */
	irq_desc[BCM_LINUX_UARTA_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_UARTA_IRQ].action = 0;
	irq_desc[BCM_LINUX_UARTA_IRQ].depth = 1;
	irq_desc[BCM_LINUX_UARTA_IRQ].chip = &brcm_uart_type;

	irq_desc[BCM_LINUX_UARTB_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_UARTB_IRQ].action = 0;
	irq_desc[BCM_LINUX_UARTB_IRQ].depth = 1;
	irq_desc[BCM_LINUX_UARTB_IRQ].chip = &brcm_uart_type;


#if 0
       /* Set up smartcard interrupts. */
        irq_desc[BCM_LINUX_SCA_IRQ].status = IRQ_DISABLED;
        irq_desc[BCM_LINUX_SCA_IRQ].action = 0;
        irq_desc[BCM_LINUX_SCA_IRQ].depth = 1;
        irq_desc[BCM_LINUX_SCA_IRQ].chip = &brcm_intc_type;
        
        irq_desc[BCM_LINUX_SCB_IRQ].status = IRQ_DISABLED;
        irq_desc[BCM_LINUX_SCB_IRQ].action = 0;
        irq_desc[BCM_LINUX_SCB_IRQ].depth = 1;
        irq_desc[BCM_LINUX_SCB_IRQ].chip = &brcm_intc_type;

#endif

#ifdef CONFIG_OPROFILE
	/* profile IRQ */
	irq_desc[BCM_PERFCOUNT_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_PERFCOUNT_IRQ].action = 0;
	irq_desc[BCM_PERFCOUNT_IRQ].depth = 1;
	irq_desc[BCM_PERFCOUNT_IRQ].chip = &brcm_mips_performance_type;
	brcm_mips_performance_enable(0);
#endif
	
	noirqdebug = 1; // THT Disable spurious interrupt checking, as UARTA would cause in BE, (USB also).
	
	brcm_mips_int2_enable(0);
	//enable the UPG level UARTA int. 
	*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) |= BCHP_IRQ0_IRQEN_uarta_irqen_MASK;
	//INTC->IrqMask |= 0x1UL << (BCM_LINUX_UPG_IRQ -1);

}

void __init arch_init_irq(void)
{
	brcm_irq_setup();
}


asmlinkage void plat_irq_dispatch(struct pt_regs *regs)
{
	unsigned int pending = read_c0_cause() & read_c0_status();

	if (pending & STATUSF_IP7)
		brcm_mips_int7_dispatch(regs);
	else if (pending & STATUSF_IP2)
		brcm_mips_int2_dispatch(regs);
	else
		spurious_interrupt(regs);
 
}

