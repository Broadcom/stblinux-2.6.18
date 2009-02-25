/*
 * arch/mips/brcm/irq.c
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
 * 01-11-2007   TDT    Modified for 7601B0
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
#include <asm/brcmstb/brcm97601b0/bcmuart.h>
#include <asm/brcmstb/brcm97601b0/bcmtimer.h>
#include <asm/brcmstb/brcm97601b0/bcmebi.h>
#include <asm/brcmstb/brcm97601b0/int1.h>
#include <asm/brcmstb/brcm97601b0/board.h>
#include <asm/brcmstb/brcm97601b0/bchp_irq0.h>
#include <asm/brcmstb/brcm97601b0/bchp_irq1.h>
//#include <asm/brcmstb/brcm97601b0/bchp_ide_l2.h>
#include <asm/brcmstb/brcm97601b0/bcmintrnum.h>
#include <asm/brcmstb/common/brcmstb.h>	/* this must be the last one */


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

#ifdef CONFIG_MIPS_MT_SMTC
extern unsigned long irq_hwmask[NR_IRQS];
#define DECLARE_SMTC_IRQ(linux_irq, hw_irq) do { \
		irq_hwmask[linux_irq - 1] = 0x100 << (hw_irq); \
	} while(0)
#else
#define DECLARE_SMTC_IRQ(linux_irq, hw_irq) do { } while(0)
#endif

#ifdef CONFIG_REMOTE_DEBUG
#include <asm/gdb-stub.h>
extern void breakpoint(void);
#endif

#define USE_UARTA_AS_DEFAULT
//#undef USE_UARTA_AS_DEFAULT

/* define front end and backend int bit groups */
#define  BCM_UPG_IRQ0_IRQEN   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IRQ0_IRQEN)
#define  BCM_UPG_IRQ0_IRQSTAT   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IRQ0_IRQSTAT)
#define  BCM_UPG_IRQ1_IRQEN   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IRQ1_IRQEN)
#define  BCM_UPG_IRQ1_IRQSTAT   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IRQ1_IRQSTAT)

#define  BCM_PATA_IRQ_CPU_STATUS   	BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IDE_L2_CPU_STATUS)
#define  BCM_PATA_IRQ_CPU_SET  		BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IDE_L2_CPU_SET)
#define  BCM_PATA_IRQ_CPU_CLEAR  		BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IDE_L2_CPU_CLEAR)
#define  BCM_PATA_IRQ_MASK_STATUS  	BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IDE_L2_CPU_MASK_STATUS)
#define  BCM_PATA_IRQ_MASK_SET   		BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IDE_L2_CPU_MASK_SET)
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
 *       2      Hardware BRCMSTB chip Internal(CPU1)
 *       3      Hardware BRCMSTB chip Internal(CPU2)
 *       4      Hardware External *Unused*
 *       5      Hardware External *Unused*
 *       6      R4k timer 
 *       7      Performance Counters
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
 *       65         2      	  UARTA
 *       66         2         UARTB
 *       67         2         UARTC
 *       68         2         UARTD
 *       69         6         R4k timer (used for master system time)
 *       70         6         R4k timer CPU2 (used for SMP)
 *       71         2         POD
 *       72         0         IPC 0 (used for SMP)
 *       73         1         IPC 1 (used for SMP)
 *       74         7         Performance Counter
 *       75         2         PATA
 *       76         2         GR Bridge
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


		/* 
		 * PR17654: UPG IRQ is shared with UART, so do not disable it, UPG_shift == 18
		 */

		if (shift == BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_SHIFT) {
			local_irq_restore(flags);
			return;
		}
		CPUINT1C->IntrW0MaskSet = (0x1UL<<shift);
		if (irq == BCM_LINUX_CPU_ENET_IRQ)
		{
			//*((volatile unsigned long *)0xb008241c) &= ~0x2;
			*((volatile unsigned long *)0xb0082424) &= ~0x2;
		}
	}
	else if (irq > 32 && 
			irq <= 32+32)
	{
		shift = irq - 32 -1;
		CPUINT1C->IntrW1MaskSet = (0x1UL<<shift);
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
	unsigned long flags;

	local_irq_save(flags);
	if (irq == BCM_LINUX_UARTA_IRQ)
	{
PRINTK("$$$$$$$$ UART A irq enabled. \n");
		CPUINT1C->IntrW0MaskClear = 
			BCHP_HIF_CPU_INTR1_INTR_W0_MASK_CLEAR_UPG_UART0_CPU_INTR_MASK;
	}
	else if (irq == BCM_LINUX_UARTB_IRQ)
	{
PRINTK("$$$$$$$$ UART B irq enabled. \n");
		CPUINT1C->IntrW0MaskClear = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_MASK;
		*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) |= BCHP_IRQ0_IRQEN_ub_irqen_MASK;
	}
	else if (irq == BCM_LINUX_UARTC_IRQ)
	{
PRINTK("$$$$$$$$ UART C irq enabled. \n");
		CPUINT1C->IntrW0MaskClear = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_MASK;
		*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) |= BCHP_IRQ0_IRQEN_uc_irqen_MASK;
	}
	else if (irq == BCM_LINUX_UARTD_IRQ)
	{
PRINTK("$$$$$$$$ UART D irq enabled. \n");
		CPUINT1C->IntrW0MaskClear = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_MASK;
		*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) |= BCHP_IRQ0_IRQEN_ud_irqen_MASK;
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
PRINTK("########## UART A irq Disabled. \n");
		CPUINT1C->IntrW0MaskSet = BCHP_HIF_CPU_INTR1_INTR_W0_MASK_SET_UPG_UART0_CPU_INTR_MASK;
		//*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) &= ~(BCHP_IRQ0_IRQEN_uarta_irqen_MASK);
	}
	else if (irq == BCM_LINUX_UARTB_IRQ)
	{
PRINTK("########## UART B irq Disabled. \n");
		//CPUINT1C->IntrW0MaskSet = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_MASK;
		*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) &= ~(BCHP_IRQ0_IRQEN_ub_irqen_MASK);
	}
	else if (irq == BCM_LINUX_UARTC_IRQ)
	{
PRINTK("########## UART C irq Disabled. \n");
		//CPUINT1C->IntrW0MaskSet = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_MASK;
		*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) &= ~(BCHP_IRQ0_IRQEN_uc_irqen_MASK);
	}
	else if (irq == BCM_LINUX_UARTD_IRQ)
	{
PRINTK("########## UART D irq Disabled. \n");
		//CPUINT1C->IntrW0MaskSet = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_MASK;
		*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) &= ~(BCHP_IRQ0_IRQEN_ud_irqen_MASK);
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


#if 0 //vincent
/*------------------------------------------------------------------------------------------------
 * PATA functions
 *
 * On the 7601, the parallel ATA interface is on a L2 interrupt.
 *
 */
static void brcm_pata_enable(unsigned int irq)
{
	unsigned long flags;

	local_irq_save(flags);

	if (irq == BCM_LINUX_PATA_IRQ)
	{
		CPUINT1C->IntrW1MaskClear = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_IDE_CPU_INTR_MASK;
		*((volatile unsigned long*)BCM_PATA_IRQ_MASK_CLEAR) = BCHP_IDE_L2_CPU_STATUS_IDE_PRI_INT_MASK;
	}
/*
	else if (irq == BCM_GR_BRIDGE_ERROR_IRQ)
	{
	}
 */

	local_irq_restore(flags);
}

static void brcm_pata_disable(unsigned int irq)
{
	unsigned long flags;

	local_irq_save(flags);

	if (irq == BCM_LINUX_PATA_IRQ)
{		
		CPUINT1C->IntrW1MaskSet = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_IDE_CPU_INTR_MASK;
		*((volatile unsigned long*)BCM_PATA_IRQ_CPU_CLEAR) = BCHP_IDE_L2_CPU_STATUS_IDE_PRI_INT_MASK;
		*((volatile unsigned long*)BCM_PATA_IRQ_MASK_SET) = BCHP_IDE_L2_CPU_STATUS_IDE_PRI_INT_MASK;

	} 
/*
	else if (irq == BCM_GR_BRIDGE_ERROR_IRQ)
{		
		*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) &= ~(BCHP_IRQ0_IRQEN_uc_irqen_MASK);
	}
*/

	local_irq_restore(flags);
	} 

static unsigned int brcm_pata_startup(unsigned int irq)
	{
	brcm_pata_enable(irq);
		
	return 0; 
	}

static void brcm_pata_end(unsigned int irq)
	{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		brcm_pata_enable(irq);
	}

static struct hw_interrupt_type brcm_pata_type = {
	typename: "BCM7601 PATA",
	startup: brcm_pata_startup,
	shutdown: brcm_pata_disable,
	enable: brcm_pata_enable,
	disable: brcm_pata_disable,
	ack: brcm_pata_disable,
	end: brcm_pata_end,
	NULL
};
#endif //0


/*
 * IRQ7 functions (performance counters)
 */
void brcm_mips_int7_dispatch(struct pt_regs *regs)
{		
#ifdef CONFIG_OPROFILE
	do_IRQ(BCM_LINUX_PERFCOUNT_IRQ, regs);
#endif
}

static void brcm_mips_int7_enable(unsigned int irq)
{
#ifdef CONFIG_OPROFILE
	set_c0_status(STATUSF_IP7);
#endif
}

static void brcm_mips_int7_disable(unsigned int irq)
{
#ifdef CONFIG_OPROFILE
	clear_c0_status(STATUSF_IP7);
#endif
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
	typename: "BCM MIPS PC",
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
static int gDebugPendingIrq0, gDebugPendingIrq1;
void brcm_mips_int2_dispatch(struct pt_regs *regs)
{
    unsigned int pendingIrqs,pendingIrqs1, shift,irq;
    
	brcm_mips_int2_disable(0);

#ifdef CONFIG_MIPS_BCM7601B_SECOND_CPU
	pendingIrqs = CPUINT1C_TP1->IntrW0Status;
	gDebugPendingIrq0 = pendingIrqs &= ~(CPUINT1C_TP1->IntrW0MaskStatus);

	pendingIrqs1 = CPUINT1C_TP1->IntrW1Status;
	gDebugPendingIrq1 = pendingIrqs1 &= ~(CPUINT1C_TP1->IntrW1MaskStatus);
#else
	pendingIrqs = CPUINT1C->IntrW0Status;
	gDebugPendingIrq0 = pendingIrqs &= ~(CPUINT1C->IntrW0MaskStatus);

	pendingIrqs1 = CPUINT1C->IntrW1Status;
	gDebugPendingIrq1 = pendingIrqs1 &= ~(CPUINT1C->IntrW1MaskStatus);
#endif
//printk("pending irqs=%x\n",pendingIrqs);
	for (irq=1; irq<=32; irq++)
	{
		shift = irq-1;
		if ((0x1 << shift) & pendingIrqs)
		{
			if (shift == BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_UART0_CPU_INTR_SHIFT) {
				do_IRQ(BCM_LINUX_UARTA_IRQ, regs);
			}
			else if (shift == BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_SHIFT)  {
				if ((*((volatile unsigned long*)BCM_UPG_IRQ0_IRQSTAT) & BCHP_IRQ0_IRQSTAT_ubirq_MASK) 
				&& (*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) & BCHP_IRQ0_IRQEN_ub_irqen_MASK) )
				{
					do_IRQ(BCM_LINUX_UARTB_IRQ, regs);
				}

				if ((*((volatile unsigned long*)BCM_UPG_IRQ0_IRQSTAT) & BCHP_IRQ0_IRQSTAT_ucirq_MASK) 
				&& (*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) & BCHP_IRQ0_IRQEN_uc_irqen_MASK) )
				{
					do_IRQ(BCM_LINUX_UARTC_IRQ, regs);
				}

				if ((*((volatile unsigned long*)BCM_UPG_IRQ0_IRQSTAT) & BCHP_IRQ0_IRQSTAT_udirq_MASK) 
				&& (*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) & BCHP_IRQ0_IRQEN_ud_irqen_MASK) )
				{
					do_IRQ(BCM_LINUX_UARTD_IRQ, regs);
				}

				do_IRQ(irq, regs);

			}
			else if (irq == BCM_LINUX_CPU_ENET_IRQ)
			{
#ifndef CONFIG_MIPS_BRCM_SIM
				if (*((volatile unsigned long *)0xb0082420) & *((volatile unsigned long *)0xb0082424) & 0x2 )
					do_IRQ(BCM_LINUX_CPU_ENET_IRQ, regs);
				else
					printk("unsolicited ENET interrupt!!!\n");
#endif
			}
			else
				do_IRQ(irq, regs);
		}
	}

	for (irq = 32+1; irq <= 32+32; irq++)
	{
		shift = irq - 32 -1;


#if 0 //vincent
//#ifndef CONFIG_MIPS_BRCM_SIM
		if (shift == BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_IDE_CPU_INTR_SHIFT) 
		{
			unsigned long l2status = *((volatile unsigned long*)BCM_PATA_IRQ_CPU_STATUS);

			if (l2status & BCHP_IDE_L2_CPU_STATUS_IDE_PRI_INT_MASK) 
			{
				do_IRQ(BCM_LINUX_PATA_IRQ, regs);
			}

		}
		else 
#endif		
		if ((0x1 << shift) & pendingIrqs1)
			do_IRQ(irq, regs);
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
#ifdef CONFIG_SMP
	{
		int cpu = smp_processor_id();
		do_IRQ(cpu ? BCM_LINUX_SYSTIMER_1_IRQ : BCM_LINUX_SYSTIMER_IRQ, regs);
	}
#else
	do_IRQ(BCM_LINUX_SYSTIMER_IRQ, regs);
#endif
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


/*
 * Broadcom specific IRQ setup
 */
void __init brcm_irq_setup(void)
{
	int irq;
	extern int noirqdebug;

	//INTC->IrqMask = 0UL;
	//INTC->IrqStatus = 0UL;
	CPUINT1C->IntrW0MaskSet = 0xffffffff;
	CPUINT1C->IntrW1MaskSet = 0xffffffff;
	//CPUINT1C->IntrW2MaskSet = 0xffffffff;
	
	change_c0_status(ST0_IE, 0);
	
	/* Setup timer interrupt */
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].action = 0;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].depth = 1;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].chip = &brcm_mips_int6_type;

#ifdef CONFIG_SMP
	/* Setup 2nd timer interrupt */
	irq_desc[BCM_LINUX_SYSTIMER_1_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_SYSTIMER_1_IRQ].action = 0;
	irq_desc[BCM_LINUX_SYSTIMER_1_IRQ].depth = 1;
	irq_desc[BCM_LINUX_SYSTIMER_1_IRQ].chip = &brcm_mips_int6_type;

	/* S/W IPC interrupt */
	irq_desc[BCM_LINUX_IPC_0_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_IPC_0_IRQ].action = 0;
	irq_desc[BCM_LINUX_IPC_0_IRQ].depth = 1;
	irq_desc[BCM_LINUX_IPC_0_IRQ].chip = &brcm_mips_int0_type;

	irq_desc[BCM_LINUX_IPC_1_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_IPC_1_IRQ].action = 0;
	irq_desc[BCM_LINUX_IPC_1_IRQ].depth = 1;
	irq_desc[BCM_LINUX_IPC_1_IRQ].chip = &brcm_mips_int1_type;
#endif

	/* Install all the 7xxx IRQs */
	for (irq = 1; irq <= 32; irq++) 
	{
		irq_desc[irq].status = IRQ_DISABLED;
		irq_desc[irq].action = 0;
		irq_desc[irq].depth = 1;
		irq_desc[irq].chip = &brcm_intc_type;
		g_brcm_intc_cnt[irq -1] = 0;
	}
	for (irq = 32+1; irq <= 32+32; irq++)
	{
		irq_desc[irq].status = IRQ_DISABLED;
		irq_desc[irq].action = 0;
		irq_desc[irq].depth = 1;
		irq_desc[irq].chip = &brcm_intc_type;
		DECLARE_SMTC_IRQ(irq, 2);
		g_brcm_intc_cnt[irq -1] = 0;
	}
PRINTK("setup int 1 to 96\n");

	/* Handle the Serial IRQs differently so they can have unique IRQs */
	irq_desc[BCM_LINUX_UARTA_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_UARTA_IRQ].action = 0;
	irq_desc[BCM_LINUX_UARTA_IRQ].depth = 1;
	irq_desc[BCM_LINUX_UARTA_IRQ].chip = &brcm_uart_type;
	DECLARE_SMTC_IRQ(BCM_LINUX_UARTA_IRQ, 2);
PRINTK("setup UARTA int\n");

	irq_desc[BCM_LINUX_UARTB_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_UARTB_IRQ].action = 0;
	irq_desc[BCM_LINUX_UARTB_IRQ].depth = 1;
	irq_desc[BCM_LINUX_UARTB_IRQ].chip = &brcm_uart_type;
	DECLARE_SMTC_IRQ(BCM_LINUX_UARTB_IRQ, 2);
PRINTK("setup UARTB int\n");

	irq_desc[BCM_LINUX_UARTC_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_UARTC_IRQ].action = 0;
	irq_desc[BCM_LINUX_UARTC_IRQ].depth = 1;
	irq_desc[BCM_LINUX_UARTC_IRQ].chip = &brcm_uart_type;
	DECLARE_SMTC_IRQ(BCM_LINUX_UARTC_IRQ, 2);
PRINTK("setup UARTB int\n");

	irq_desc[BCM_LINUX_UARTD_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_UARTD_IRQ].action = 0;
	irq_desc[BCM_LINUX_UARTD_IRQ].depth = 1;
	irq_desc[BCM_LINUX_UARTD_IRQ].chip = &brcm_uart_type;
	DECLARE_SMTC_IRQ(BCM_LINUX_UARTD_IRQ, 2);
PRINTK("setup UARTB int\n");

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
	irq_desc[BCM_LINUX_PERFCOUNT_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_PERFCOUNT_IRQ].action = 0;
	irq_desc[BCM_LINUX_PERFCOUNT_IRQ].depth = 1;
	irq_desc[BCM_LINUX_PERFCOUNT_IRQ].chip = &brcm_mips_int7_type;
	//brcm_mips_performance_enable(0);
#endif

#if 0
/* Not on 7443 */
	irq_desc[BCM_LINUX_PATA_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_PATA_IRQ].action = 0;
	irq_desc[BCM_LINUX_PATA_IRQ].depth = 1;
	irq_desc[BCM_LINUX_PATA_IRQ].chip = &brcm_pata_type;

#endif
	noirqdebug = 1; // THT Disable spurious interrupt checking, as UARTA would cause in BE, (USB also).
	
	brcm_mips_int2_enable(0);

	//enable the UPG level UARTA int. 
	*((volatile unsigned long*)BCM_UPG_IRQ0_IRQEN) |= BCHP_IRQ0_IRQEN_uarta_irqen_MASK;
}

void __init arch_init_irq(void)
{
	//extern void __init init_generic_irq(void);

	//init_generic_irq();
	brcm_irq_setup();
}

asmlinkage void plat_irq_dispatch(struct pt_regs *regs)
{
       unsigned int pending = read_c0_cause();
#if ! defined(CONFIG_MIPS_MT)
       /* SMTC clears the status bit in genex.S */
       pending &= read_c0_status();
#endif

       if (pending & STATUSF_IP6)
               brcm_mips_int6_dispatch(regs);
       else if (pending & STATUSF_IP2)
               brcm_mips_int2_dispatch(regs);
#if defined(CONFIG_SMP) && ! defined(CONFIG_MIPS_MT)
       else if (pending & STATUSF_IP0)
               brcm_mips_int0_dispatch(regs);
       else if (pending & STATUSF_IP1)
               brcm_mips_int1_dispatch(regs);
#endif
       else
               spurious_interrupt(regs);
}
