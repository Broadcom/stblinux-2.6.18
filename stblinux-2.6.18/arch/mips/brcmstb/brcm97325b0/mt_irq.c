/*
 * arch/mips/brcm/mt_irq.c
 *
 * Copyright (C) 2001-2007 Broadcom Corporation
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
 * 03-10-2006   TDT    Modified for SMP
 * 01-11-2007   TDT    Modified for 7440B0
 * 01-15-2007	TDT    Adding interrupt control to both CPUs
 */
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/brcmstb/brcm97325b0/bcmuart.h>
#include <asm/brcmstb/brcm97325b0/bcmtimer.h>
#include <asm/brcmstb/brcm97325b0/bcmebi.h>
#include <asm/brcmstb/brcm97325b0/int1.h>
#include <asm/brcmstb/brcm97325b0/board.h>
#include <asm/brcmstb/brcm97325b0/bchp_irq0.h>
#include <asm/brcmstb/brcm97325b0/bchp_irq1.h>
#include <asm/brcmstb/brcm97325b0/bcmintrnum.h>
#include <asm/brcmstb/common/brcmstb.h>	/* this must be the last one */
#include <asm/smp.h>

//#define	DEBUG_UARTA_INTR	// jipeng - enable debugging of UART A interrupt

#ifdef	DEBUG_UARTA_INTR
#define DEBUG_UARTA_INTR_FROM_TIMERIRQ
#define DEBUG_UARTA_INTR_FROM_INT2IRQ
#endif

asmlinkage void plat_irq_dispatch(struct pt_regs *regs);

#ifdef	DEBUG_UARTA_INTR
static void dump_INTC_regs(void);
#define PRINTK(msg) uartB_puts(msg)
#else
#define PRINTK(msg) 
#endif	// DEBUG_UARTA_INTR

extern unsigned long irq_hwmask[NR_IRQS];

#define DECLARE_SMTC_IRQ(linux_irq, hw_irq) do { \
		irq_hwmask[linux_irq] = 0x100 << (hw_irq); \
	} while(0)

#ifdef CONFIG_REMOTE_DEBUG
#include <asm/gdb-stub.h>
extern void breakpoint(void);
#endif

#define USE_UARTA_AS_DEFAULT
//#undef USE_UARTA_AS_DEFAULT

/* define front end and backend int bit groups */
#define  BCM_UPG_IRQ0_IRQEN   	BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IRQ0_IRQEN)
#define  BCM_UPG_IRQ0_IRQSTAT   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IRQ0_IRQSTAT)
#define  BCM_UPG_IRQ1_IRQEN   	BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IRQ1_IRQEN)
#define  BCM_UPG_IRQ1_IRQSTAT   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IRQ1_IRQSTAT)

#define  BCM_PATA_IRQ_CPU_STATUS   	BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IDE_L2_CPU_STATUS)
#define  BCM_PATA_IRQ_CPU_SET  		BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IDE_L2_CPU_SET)
#define  BCM_PATA_IRQ_CPU_CLEAR  	BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IDE_L2_CPU_CLEAR)
#define  BCM_PATA_IRQ_MASK_STATUS  	BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IDE_L2_CPU_MASK_STATUS)
#define  BCM_PATA_IRQ_MASK_SET   	BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IDE_L2_CPU_MASK_SET)
#define  BCM_PATA_IRQ_MASK_CLEAR   	BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IDE_L2_CPU_MASK_CLEAR)

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
 *       0      Software Used for SMP IPC
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
 *        0         2         Chip Interrupt Controller/Dedicated Handler
 *      1- 32       2         The L1 32 Interrupt Controller Bits W0
 *      33 - 64     2         The L1 32 Interrupt Controller Bits W1
 *      65 - 96     2         The L1 32 Interrupt Controller Bits W2
 *       97         2      	  UARTA
 *       98         2         UARTB
 *       99         2         UARTC
 *      107         7      	  R4k timer (used for master system time)
 *	100	    0		Software IPI 0
 *	101	    1		Software IPI 1
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

	if (irq > 0 && irq <= 32)
	{
		shift = irq - 1;
		CPUINT1C->IntrW0MaskClear = (0x1UL<<shift);
		if (irq == BCM_LINUX_CPU_ENET_IRQ)
		*((volatile unsigned long *)0xb0082424) |= 0x2;
	}
	else if (irq > 32 && 
			irq <= 32+32)
	{
		shift = irq - 32 -1;
		CPUINT1C->IntrW1MaskClear = (0x1UL<<shift);
	}
	else if (irq > 64 && 
			irq <= 64+32)
	{
		shift = irq - 64 -1;
		CPUINT1C->IntrW2MaskClear = (0x1UL<<shift);
	}

	local_irq_restore(flags);
}

static void brcm_intc_disable(unsigned int irq)
{
	unsigned int shift;
	unsigned long flags;

	local_irq_save(flags);

	if (irq > 0 && irq <= 32)
	{
		shift = irq - 1;

		CPUINT1C->IntrW0MaskSet = (0x1UL<<shift);
		if (irq == BCM_LINUX_CPU_ENET_IRQ)
		*((volatile unsigned long *)0xb0082424) &= ~0x2;
	}
	else if (irq > 32 && 
			irq <= 32+32)
	{
		shift = irq - 32 -1;
		CPUINT1C->IntrW1MaskSet = (0x1UL<<shift);
	}
	else if (irq > 64 && 
			irq <= 64+32)
	{
		shift = irq - 64 -1;
		CPUINT1C->IntrW2MaskSet = (0x1UL<<shift);
	}

	local_irq_restore(flags);
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
 *
 */
static void brcm_uart_enable(unsigned int irq)
{
	unsigned int flags;
	local_irq_save(flags);

	if (irq == BCM_LINUX_UARTA_IRQ)
	{
PRINTK("$$$$$$$$ UART A irq enabled. \n");
		CPUINT1C->IntrW0MaskClear = BCHP_HIF_CPU_INTR1_INTR_W0_MASK_CLEAR_UPG_UART0_CPU_INTR_MASK;
	}
	else if (irq == BCM_LINUX_UARTB_IRQ)
	{
PRINTK("$$$$$$$$ UART B irq enabled. \n");
		CPUINT1C->IntrW2MaskClear = BCHP_HIF_CPU_INTR1_INTR_W2_MASK_CLEAR_UPG_UART1_CPU_INTR_MASK;
	}
	else if (irq == BCM_LINUX_UARTC_IRQ)
	{
PRINTK("$$$$$$$$ UART C irq enabled. \n");
		CPUINT1C->IntrW2MaskClear = BCHP_HIF_CPU_INTR1_INTR_W2_MASK_CLEAR_UPG_UART2_CPU_INTR_MASK;
	}

	local_irq_restore(flags);
}

static void brcm_uart_disable(unsigned int irq)
{
	unsigned long flags;
	local_irq_save(flags);

	if (irq == BCM_LINUX_UARTA_IRQ)
	{
PRINTK("########## UART A irq Disabled. \n");
		CPUINT1C->IntrW0MaskSet = BCHP_HIF_CPU_INTR1_INTR_W0_MASK_SET_UPG_UART0_CPU_INTR_MASK;
	}
	else if (irq == BCM_LINUX_UARTB_IRQ)
	{
PRINTK("########## UART B irq Disabled. \n");
		CPUINT1C->IntrW2MaskSet = BCHP_HIF_CPU_INTR1_INTR_W2_MASK_SET_UPG_UART1_CPU_INTR_MASK;
	}
	else if (irq == BCM_LINUX_UARTC_IRQ)
	{
PRINTK("########## UART C irq Disabled. \n");
		CPUINT1C->IntrW2MaskSet = BCHP_HIF_CPU_INTR1_INTR_W2_MASK_SET_UPG_UART2_CPU_INTR_MASK;
	}

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
 #define HEARTBEAT_FREQ 100
void brcm_mips_int7_dispatch(struct pt_regs *regs)
{		
	static int intNo = 0;

#if	!defined(DEBUG_UARTA)
	/* debug debug add ADT */
	if ((intNo++) == 0) {
		printk("brcm timer int\n");
		if (intNo >= HEARTBEAT_FREQ) /* Wrap */
			intNo = 1;
	} 
#elif	defined(DEBUG_UARTA_INTR_FROM_TIMERIRQ)
	// Print heart beat
	if ((intNo++) % HEARTBEAT_FREQ == 0)
	{
		char msg[40];
		
		sprintf(msg, "----> hb jif=%lu:\n", jiffies);
		uartB_puts(msg);		
		dump_INTC_regs();
	}
#endif	// !defined(DEBUG_UARTA)

#if defined(CONFIG_SMP) && ! defined(CONFIG_MIPS_MT)
	{
		int cpu = smp_processor_id();
		do_IRQ(cpu ? BCM_LINUX_SYSTIMER_1_IRQ : BCM_LINUX_SYSTIMER_IRQ, regs);
	}
#else
	do_IRQ(BCM_LINUX_SYSTIMER_IRQ, regs);
#endif

}

static void brcm_mips_int7_enable(unsigned int irq)
{
	unsigned int vpflags;

	if(!irqs_disabled())
	printk("WE GOT PROBLEM at %s:%d\n", __FUNCTION__, __LINE__);

	vpflags = dvpe();
	set_c0_status(STATUSF_IP7);
	irq_enable_hazard();
	evpe(vpflags);
}

static void brcm_mips_int7_disable(unsigned int irq)
{
	unsigned int vpflags;

	if(!irqs_disabled())
	printk("WE GOT PROBLEM at %s:%d\n", __FUNCTION__, __LINE__);

	vpflags = dvpe();
	clear_c0_status(STATUSF_IP7);
	irq_disable_hazard();
	evpe(vpflags);
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
 * IRQ2 functions - jipeng THIS FUNCTION has to be called within local_irq critical section
 */
static void brcm_mips_int2_enable(unsigned int irq)
{
	unsigned int vpflags;
	
	if(!irqs_disabled())
	printk("WE GOT PROBLEM at %s:%d\n", __FUNCTION__, __LINE__);

	vpflags = dmt();
	set_c0_status(STATUSF_IP2);
	irq_enable_hazard();
	emt(vpflags);
}

/*
 * IRQ2 functions - jipeng THIS FUNCTION has to be called within local_irq critical section
 */
static void brcm_mips_int2_disable(unsigned int irq)
{
	unsigned int vpflags;

	if(!irqs_disabled())
	printk("WE GOT PROBLEM at %s:%d\n", __FUNCTION__, __LINE__);

	vpflags = dmt();
	clear_c0_status(STATUSF_IP2);
	clear_c0_cause(0x100<<2);
	irq_disable_hazard();
	emt(vpflags);
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

static int g_brcm_intc_cnt[96];
static int g_intcnt = 0;
static int gDebugPendingIrq0, gDebugPendingIrq1, gDebugPendingIrq2;
static int gDebugMaskW0, gDebugMaskW1, gDebugMaskW2;

#ifdef	DEBUG_UARTA_INTR
static void dump_INTC_regs(void)
{
	unsigned int pendingIrqs,pendingIrqs1, pendingIrqs2, mask0, mask1, mask2;
	char msg[80];

	pendingIrqs = CPUINT1C->IntrW0Status;
	pendingIrqs &= ~(mask0 = CPUINT1C->IntrW0MaskStatus);
	pendingIrqs1 = CPUINT1C->IntrW1Status;
	pendingIrqs1 &= ~(mask1 = CPUINT1C->IntrW1MaskStatus);
	pendingIrqs2 = CPUINT1C->IntrW2Status;
	pendingIrqs2 &= ~(mask2 = CPUINT1C->IntrW2MaskStatus);


	sprintf(msg, "\nLast pending0=%08x, pd1=%08x, pd2=%08x\n", 
		gDebugPendingIrq0, gDebugPendingIrq1, gDebugPendingIrq2);
	uartB_puts(msg);

	sprintf(msg, "Last MaskW0=%08x, mW1=%08x, mW2=%08x\n", 
		gDebugMaskW0, gDebugMaskW1, gDebugMaskW2);
	uartB_puts(msg);
	
	sprintf(msg, "Current pending0=%08x, pd1=%08x,  pd2=%08x\n", 
		pendingIrqs, pendingIrqs1, pendingIrqs2);
	uartB_puts(msg);

	sprintf(msg, "Current MaskW0=%08x, mW1=%08x, mW2=%08x\n", 
		mask0, mask1, mask2);
	uartB_puts(msg);
}

EXPORT_SYMBOL(dump_INTC_regs);
#endif

// jipeng - this is helper function for int2 interrupt handler
static void brcm_int2_do_IRQ(unsigned int irq, struct pt_regs *regs)
{
	if(irq_desc[irq].action)
	do_IRQ(irq, regs);
}

void brcm_mips_int2_dispatch(struct pt_regs *regs)
{
    	unsigned int pendingIrqs,pendingIrqs1,pendingIrqs2, shift,irq;

	brcm_mips_int2_disable(0);

	pendingIrqs = CPUINT1C->IntrW0Status;
	pendingIrqs1 = CPUINT1C->IntrW1Status;
	pendingIrqs2 = CPUINT1C->IntrW2Status;

#ifdef	DEBUG_UARTA_INTR
#ifdef	DEBUG_UARTA_INTR_FROM_INT2IRQ
	dump_INTC_regs();
#endif
	gDebugPendingIrq0 = pendingIrqs &= ~(gDebugMaskW0 = CPUINT1C->IntrW0MaskStatus);
	gDebugPendingIrq1 = pendingIrqs1 &= ~(gDebugMaskW1 = CPUINT1C->IntrW1MaskStatus);
	gDebugPendingIrq2 = pendingIrqs2 &= ~(gDebugMaskW2 = CPUINT1C->IntrW2MaskStatus);
#endif	// DEBUG_UARTA_INTR

	for (irq=1; irq<=32; irq++)
	{
		shift = irq-1;
		if ((0x1 << shift) & pendingIrqs)
		{
			if (shift == BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_UART0_CPU_INTR_SHIFT) {
PRINTK("UART A\n");
				do_IRQ(BCM_LINUX_UARTA_IRQ, regs);
			}

			else if (shift == BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_SHIFT 
				&& (*((volatile unsigned long*)BCM_UPG_IRQ0_IRQSTAT) & BCHP_IRQ0_IRQSTAT_ubirq_MASK) 
				&& (*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) & BCHP_IRQ0_IRQEN_ub_irqen_MASK) )
			{
PRINTK("UART B\n");
					do_IRQ(BCM_LINUX_UARTB_IRQ, regs);
			}
			else if (shift == BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_SHIFT 
				&& (*((volatile unsigned long*)BCM_UPG_IRQ0_IRQSTAT) & BCHP_IRQ0_IRQSTAT_ucirq_MASK) 
				&& (*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) & BCHP_IRQ0_IRQEN_uc_irqen_MASK) )
			{
PRINTK("UART C\n");
					do_IRQ(BCM_LINUX_UARTC_IRQ, regs);
			}
			else if (irq == BCM_LINUX_CPU_ENET_IRQ)
			{
#ifndef CONFIG_MIPS_BRCM_IKOS
				if (*((volatile unsigned long *)0xb0082420) & *((volatile unsigned long *)0xb0082424) & 0x2 )
					do_IRQ(BCM_LINUX_CPU_ENET_IRQ, regs);
				else
#endif
					printk("unsolicited ENET interrupt!!!\n");

			}
			else
			brcm_int2_do_IRQ(irq, regs);
		}
	}
	
	for (irq = 32+1; irq <= 32+32; irq++)
	{
		shift = irq - 32 -1;
		if ((0x1 << shift) & pendingIrqs1)
			brcm_int2_do_IRQ(irq, regs);
	}
	
	for (irq = 64+1; irq <= 64+32; irq++)
	{
		shift = irq - 64 -1;
		if ((0x1 << shift) & pendingIrqs2)
			brcm_int2_do_IRQ(irq, regs);
	}

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
 * IRQ0 functions
 */
static void brcm_mips_int0_enable(unsigned int irq)
{
	set_c0_status(STATUSF_IP0);
}

static void brcm_mips_int0_disable(unsigned int irq)
{
	clear_c0_status(STATUSF_IP0);
}

static void brcm_mips_int0_ack(unsigned int irq)
{
	/* Already done in brcm_irq_dispatch */
}

static unsigned int brcm_mips_int0_startup(unsigned int irq)
{ 
	brcm_mips_int0_enable(irq);

	return 0; /* never anything pending */
}

static void brcm_mips_int0_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		brcm_mips_int0_enable(irq);
}

static struct hw_interrupt_type brcm_mips_int0_type = {
	typename: "BCM MIPS INT0",
	startup: brcm_mips_int0_startup,
	shutdown: brcm_mips_int0_disable,
	enable: brcm_mips_int0_enable,
	disable: brcm_mips_int0_disable,
	ack: brcm_mips_int0_ack,
	end: brcm_mips_int0_end,
	NULL
};

void brcm_mips_int0_dispatch(struct pt_regs *regs)
{
	do_IRQ(BCM_LINUX_IPC_0_IRQ,regs);
}

/*
 * IRQ1 functions
 */
static void brcm_mips_int1_enable(unsigned int irq)
{
	set_c0_status(STATUSF_IP1);
}

static void brcm_mips_int1_disable(unsigned int irq)
{
	clear_c0_status(STATUSF_IP1);
}

static void brcm_mips_int1_ack(unsigned int irq)
{
	/* Already done in brcm_irq_dispatch */
}

static unsigned int brcm_mips_int1_startup(unsigned int irq)
{ 
	brcm_mips_int1_enable(irq);

	return 0; /* never anything pending */
}

static void brcm_mips_int1_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		brcm_mips_int1_enable(irq);
}

static struct hw_interrupt_type brcm_mips_int1_type = {
	typename: "BCM MIPS INT1",
	startup: brcm_mips_int1_startup,
	shutdown: brcm_mips_int1_disable,
	enable: brcm_mips_int1_enable,
	disable: brcm_mips_int1_disable,
	ack: brcm_mips_int1_ack,
	end: brcm_mips_int1_end,
	NULL
};

void brcm_mips_int1_dispatch(struct pt_regs *regs)
{
	do_IRQ(BCM_LINUX_IPC_1_IRQ,regs);
}

static void brcm_mips_timer_dispatch(struct pt_regs *regs)
{
        do_IRQ(BCM_LINUX_SYSTIMER_IRQ, regs);
}

static struct irqaction brcm_dummy_irqaction = {
        .handler = no_action,
        .name = ""
};

void __init brcm_irq_setup(void)
{
	int irq;
	extern int noirqdebug;

	CPUINT1C->IntrW0MaskSet = 0xffffffff;
	CPUINT1C->IntrW1MaskSet = 0xffffffff; 
	CPUINT1C->IntrW2MaskSet = 0xffffffff; 	

	change_c0_status(ST0_IE, 0);
	
	/* Setup timer interrupt */
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].action = NULL;
	DECLARE_SMTC_IRQ(BCM_LINUX_SYSTIMER_IRQ, 7);

#if defined(CONFIG_SMP) && ! defined(CONFIG_MIPS_MT)
	/* Setup timer interrupt */
	irq_desc[BCM_LINUX_SYSTIMER_1_IRQ].chip = &brcm_mips_int7_type;
	irq_desc[BCM_LINUX_SYSTIMER_1_IRQ].action = NULL;

	/* S/W IPC interrupt */
	irq_desc[BCM_LINUX_IPC_0_IRQ].chip = &brcm_mips_int0_type;
	irq_desc[BCM_LINUX_IPC_0_IRQ].action = NULL;

	irq_desc[BCM_LINUX_IPC_1_IRQ].chip = &brcm_mips_int1_type;
	irq_desc[BCM_LINUX_IPC_0_IRQ].action = NULL;
#endif
	/* Install all the 7xxx IRQs */
	for (irq = 1; irq <= 96; irq++) 
	{
		irq_desc[irq].status = IRQ_PER_CPU;
		irq_desc[irq].chip = &brcm_intc_type;
		irq_desc[irq].action = NULL;
		DECLARE_SMTC_IRQ(irq, 2);
		g_brcm_intc_cnt[irq -1] = 0;
	}

	/* Handle the Serial IRQs differently so they can have unique IRQs */
	irq_desc[BCM_LINUX_UARTA_IRQ].status = IRQ_PER_CPU;
	irq_desc[BCM_LINUX_UARTA_IRQ].chip = &brcm_uart_type;
	irq_desc[BCM_LINUX_UARTA_IRQ].action = NULL;
	DECLARE_SMTC_IRQ(BCM_LINUX_UARTA_IRQ, 2);

	irq_desc[BCM_LINUX_UARTB_IRQ].status = IRQ_PER_CPU;
	irq_desc[BCM_LINUX_UARTB_IRQ].chip = &brcm_uart_type;
	irq_desc[BCM_LINUX_UARTB_IRQ].action = NULL;
	DECLARE_SMTC_IRQ(BCM_LINUX_UARTB_IRQ, 2);

	noirqdebug = 1; // THT Disable spurious interrupt checking, as UARTA would cause in BE, (USB also).
	brcm_mips_int2_enable(0);
	//enable the UPG level UARTA int. 
	*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) |= BCHP_IRQ0_IRQEN_uarta_irqen_MASK;
}

void __init arch_init_irq(void)
{
	mips_cpu_irq_init(BCM_LINUX_IPC_0_IRQ);
	brcm_irq_setup();

	set_vi_handler(2, plat_irq_dispatch);
	set_vi_handler(7, brcm_mips_timer_dispatch);
	change_c0_status(ST0_IE, 1);	/* global IE = 1 */
}

asmlinkage void plat_irq_dispatch(struct pt_regs *regs)
{
	brcm_mips_int2_dispatch(regs);
}
