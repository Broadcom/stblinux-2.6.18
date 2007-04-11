/*
 * arch/mips/brcmstb/brcm97318/irq.c
 *
 * Copyright (C) 2001-2005 Broadcom Corporation
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
 * 11-10-04	THT    Created from 97317
 */
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/brcmstb/brcm97318/bcm97318.h>
#include <asm/brcmstb/brcm97318/bcmintrnum.h>
#include <asm/brcmstb/brcm97318/bcm7318.h>
#include <asm/brcmstb/brcm97318/bcmdma.h>

#ifdef CONFIG_REMOTE_DEBUG
#include <asm/gdb-stub.h>
extern void breakpoint(void);
#endif

/* define front end and backend int bit groups */
//#define BCM_BACKEND_INTS	(BGE_VEC1_IRQ|BGE_VEC0_IRQ|ADP_A_IRQ|ATP_A_IRQ|ADP_B_IRQ|ATP_B_IRQ|ATP_C_IRQ|ADP_C_IRQ|XPT_ICAM_IRQ|XPT_MSG_IRQ|XPT_OVFLOW_IRQ|XPT_STATUS_IRQ|MINITITAN_IRQ|BGE_IRQ)
//#define BCM_FRONTEND_PHY_INTS 	(SDS_RX2_IRQ|SDS_RX1_IRQ|EC_BRIDGE_IRQ|PERIPH_POST_DONE_IRQ|UPG_IRQ)
#define ENET_TOP_IUDMA_ENET_MIPS_PCI_IRQ_STS (ENET_ADR_BASE+0x2418)
#define ENET_TOP_IUDMA_ENET_MIPS_PCI_IRQ_MSK (ENET_ADR_BASE+0x241c)

extern asmlinkage unsigned int do_IRQ(unsigned int irq, struct pt_regs *regs);

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
 * Again, I cannot stress this enough, keep this table up to date!!!
 */

/*
 * INTC functions
 */
static void brcm_intc_enable(unsigned int irq)
{
	unsigned int shift = irq - 1;
	unsigned long flags;

    //    if (irq ==  BCM_LINUX_EBI_TX_IRQ) {
	//		printk("$$$$$$$$$$$$$$$ brcm_intc_enable: Enable BCM_LINUX_EBI_TX_IRQ\n");
	//	return;
    //    }


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
    if (irq ==  BCM_LINUX_SOFT_MODEM_DMA_IRQS) {
	   INTC->IrqMask |= BCM_LINUX_DMA_IRQ; /* 1 */
	   INTC->IrqMask |= BCM_LINUX_MODEM_IRQ; /* 9 */
           DMA_CHAN[MODEM_RX_CHAN].intMask |= (DMA_BUFF_DONE|DMA_DONE|DMA_NO_DESC);
           DMA_CHAN[MODEM_TX_CHAN].intMask |= (DMA_BUFF_DONE|DMA_DONE|DMA_NO_DESC);
	   return;
	}

	if (irq == BCM_LINUX_CPU_ENET_IRQ){
//printk("Enable BCM_LINUX_CPU_ENET_IRQ\n");
		local_irq_save(flags);
		INTC->IrqMask |= (0x1UL<<shift);
		//NTC->IrqMask |= 1; // DMA_IRQ
		
		//*((volatile unsigned long *)0xb008241c) |= 0x2;
		// 7038 *((volatile unsigned long *)0xb0082424) |= 0x2;
		*((volatile unsigned long *) ENET_TOP_IUDMA_ENET_MIPS_PCI_IRQ_MSK) |= 0x2;
		local_irq_restore(flags);
		return;

	}
	local_irq_save(flags);
	INTC->IrqMask |= (0x1UL<<shift);
	local_irq_restore(flags);
}

static void brcm_intc_disable(unsigned int irq)
{
	unsigned int shift = irq - 1;
	unsigned long flags;


    //if (irq ==  BCM_LINUX_EBI_TX_IRQ)
	//	return;


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
    if (irq ==  BCM_LINUX_SOFT_MODEM_DMA_IRQS) {
            local_irq_save(flags);
            DMA_CHAN[MODEM_RX_CHAN].intMask &= (~(DMA_BUFF_DONE|DMA_DONE|DMA_NO_DESC));
            DMA_CHAN[MODEM_TX_CHAN].intMask &= (~(DMA_BUFF_DONE|DMA_DONE|DMA_NO_DESC));
            local_irq_restore(flags);
	    return;
	}


	if (irq == BCM_LINUX_UPG_IRQ)
		return;
	if (irq == BCM_LINUX_CPU_ENET_IRQ){
 		local_irq_save(flags);
		INTC->IrqMask &= (~(0x1UL<<shift));
		//INTC->IrqMask &= (~(1)); // DMA_IRQ
		*((volatile unsigned long *) ENET_TOP_IUDMA_ENET_MIPS_PCI_IRQ_MSK) &= ~0x2;
		local_irq_restore(flags);
		return;
	}
	if (irq > 0 && irq <= 32) {
 		local_irq_save(flags);
		INTC->IrqMask &= (~(0x1UL<<shift));
		local_irq_restore(flags);
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
static struct irq_chip brcm_intc_type = {
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

static struct irq_chip brcm_intc2_type = {
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

	local_irq_save(flags);
	if (irq == BCM_LINUX_UARTA_IRQ)
		UPG_INTC->irqen_l |= UPG_UA_IRQ;
	else
		UPG_INTC->irqen_l |= UPG_UB_IRQ;
	local_irq_restore(flags);
}

static void brcm_uart_disable(unsigned int irq)
{
	unsigned long flags;

	local_irq_save(flags);
	if (irq == BCM_LINUX_UARTA_IRQ)
		UPG_INTC->irqen_l &= (~(UPG_UA_IRQ));
	else
		UPG_INTC->irqen_l &= (~(UPG_UB_IRQ));
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

static struct irq_chip brcm_mips_performance_type = {
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

static void brcm_dma_dispatch(unsigned int irq,struct pt_regs *regs)
{		
	if ( irq == BCM_LINUX_MODEM_IRQ) {
              do_IRQ(BCM_LINUX_SOFT_MODEM_DMA_IRQS, regs);
	      return;
	}
		

                if (*DMA_INTR_GLOBAL & (1<<(MODEM_RX_CHAN-1)) &&
                        DMA_CHAN[MODEM_RX_CHAN].intMask & (DMA_BUFF_DONE|DMA_DONE|DMA_NO_DESC))
                {
/*do not disable int here, The softmodem driver is responsible for disabling it. */
                        do_IRQ(BCM_LINUX_SOFT_MODEM_DMA_IRQS, regs);
                        return;
                }


                if (*DMA_INTR_GLOBAL & (1<<(MODEM_TX_CHAN-1)) &&
                        DMA_CHAN[MODEM_TX_CHAN].intMask & (DMA_BUFF_DONE|DMA_DONE|DMA_NO_DESC))
                {
/*do not disable int here, The softmodem driver is responsible for disabling it. */
                        do_IRQ(BCM_LINUX_SOFT_MODEM_DMA_IRQS, regs);
                        return;
                }

}
/*
 * IRQ7 functions
 */
void brcm_mips_int7_dispatch(struct pt_regs *regs)
{		
	static char firstint = 0;
	if (firstint == 0)
	{
		firstint = 1;
	}
#ifdef CONFIG_OPROFILE
/* TDT-Interrupt dispatch for perf. counters is broken, use timer interrupt for now */
#if 0
	/* Interrupts without IP bit set are due to perf. counter */
	if( (read_c0_cause()&(CAUSEF_IP|CAUSEF_EXCCODE))==0)
	{
		brcm_mips_performance_dispatch(regs);
	}
	else
#else
		brcm_mips_performance_dispatch(regs);
#endif
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
unsigned int status, mask;    
	brcm_mips_int2_disable(0);
pendingIrqs = (status=INTC->IrqStatus) & (mask=INTC->IrqMask);
	//pendingIrqs = INTC->IrqStatus & INTC->IrqMask;



	/* IRQs from 1 to 32 - 0 reserved for main IRQ and 60 for timer */
	for (irq = 1; irq <= 32; ++irq)
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
			else if (irq == BCM_LINUX_CPU_ENET_IRQ) {
				if (*((volatile unsigned long *)ENET_TOP_IUDMA_ENET_MIPS_PCI_IRQ_STS) & 
					*((volatile unsigned long *)ENET_TOP_IUDMA_ENET_MIPS_PCI_IRQ_MSK) & 0x2 ) {	
//printk("************* Calling do_IRQ(BCM_LINUX_CPU_ENET_IRQ, regs)\n");
					do_IRQ(BCM_LINUX_CPU_ENET_IRQ, regs);	
				}			
				//else					
				//	printk("unsolicited ENET interrupt!!!\n");			
			}			
			else if (irq == BCM_LINUX_DMA_IRQ || irq == BCM_LINUX_MODEM_IRQ)
				brcm_dma_dispatch(irq,regs);
			else if (irq == BCM_LINUX_EBI_TX_IRQ)
			        do_IRQ(irq, regs);
			else 
				do_IRQ(irq, regs);
			
			++g_brcm_intc_cnt[shift];
		}
	}
	brcm_mips_int2_enable(0);
}


/*-----------------------------------------------------------------------
 * IRQ3 functions
 *-----------------------------------------------------------------------
 */
static void brcm_mips_int3_enable(unsigned int irq)
{
        set_c0_status(STATUSF_IP3);
	 //INTC->extIrqConfig |= 0x03;
       INTC->extIrqMask |= (EXTIRQ1_EN);

}

static void brcm_mips_int3_disable(unsigned int irq)
{
        clear_c0_status(STATUSF_IP3);
        INTC->extIrqMask &= (~(EXTIRQ1_EN));

 }

static void brcm_mips_int3_ack(unsigned int irq)
{
        /* Already done in brcm_irq_dispatch */
}

static unsigned int brcm_mips_int3_startup(unsigned int irq)
{
        brcm_mips_int3_enable(irq);

        return 0; /* never anything pending */
}

static void brcm_mips_int3_end(unsigned int irq)
{
        if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
                brcm_mips_int3_enable(irq);
}

static struct irq_chip brcm_mips_int3_type = {
        typename: "BCM MIPS INT3",
        startup: brcm_mips_int3_startup,
        shutdown: brcm_mips_int3_disable,
        enable: brcm_mips_int3_enable,
        disable: brcm_mips_int3_disable,
        ack: brcm_mips_int3_ack,
        end: brcm_mips_int3_end,
        NULL
};

void brcm_mips_int3_dispatch(struct pt_regs *regs)
{
        clear_c0_status(STATUSF_IP3);
        INTC->extIrqMask &= (~(EXTIRQ1_EN));

        //brcm_mips_int6_disable(BCM_LINUX_4413ENET_IRQ);
        do_IRQ(BCM_LINUX_EXT_1_IRQ, regs);
	
}


/*-----------------------------------------------------------------------
 * IRQ4 functions
 *-----------------------------------------------------------------------
 */
static void brcm_mips_int4_enable(unsigned int irq)
{
	set_c0_status(STATUSF_IP4);
	INTC->extIrqMask |= (EXTIRQ2_EN);

}

static void brcm_mips_int4_disable(unsigned int irq)
{
	clear_c0_status(STATUSF_IP4);
	INTC->extIrqMask &= (~(EXTIRQ2_EN));

 }

static void brcm_mips_int4_ack(unsigned int irq)
{
	/* Already done in brcm_irq_dispatch */
}

static unsigned int brcm_mips_int4_startup(unsigned int irq)
{
	brcm_mips_int4_enable(irq);

	return 0; /* never anything pending */
}

static void brcm_mips_int4_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		brcm_mips_int4_enable(irq);
}

static struct irq_chip brcm_mips_int4_type = {
        typename: "BCM MIPS INT4",
        startup: brcm_mips_int4_startup,
        shutdown: brcm_mips_int4_disable,
        enable: brcm_mips_int4_enable,
        disable: brcm_mips_int4_disable,
        ack: brcm_mips_int4_ack,
        end: brcm_mips_int4_end,
        NULL
};

void brcm_mips_int4_dispatch(struct pt_regs *regs)
{
        clear_c0_status(STATUSF_IP4);
        INTC->extIrqMask &= (~(EXTIRQ2_EN));

        do_IRQ(BCM_LINUX_EXT_2_IRQ, regs);
	
}

/*-----------------------------------------------------------------------
 * IRQ5,6 functions are just placeholders for now
 *-----------------------------------------------------------------------
 */

void brcm_mips_int5_dispatch(struct pt_regs *regs)
{
	printk("brcm_mips_int5_dispatch: Should not be here \n");
}

void brcm_mips_int6_dispatch(struct pt_regs *regs)
{
	printk("brcm_mips_int6_dispatch: Should not be here \n");
}


/*
 * Broadcom specific IRQ setup
 */
void __init brcm_irq_setup(void)
{
	int irq;

	INTC->IrqMask = 0UL;
	INTC->IrqStatus = 0UL;

	/* Configure EXT_1 and EXT_2 to level triggered, EXT1 active high, EXT2 active low */
	INTC->extIrqConfig &= ~0x0f;
	INTC->extIrqConfig |= 0x0b;
	
	/* IMHO we do not need to do the following here, the request IRQ would take care of it. */
	//change_c0_status(ST0_IM, IE_IRQ0 | IE_IRQ5);  // right now we only care about IRQ0 and 5.
	change_c0_status(ST0_IE, 0);
	
	/* Setup timer interrupt */
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].action = 0;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].depth = 1;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].chip = &brcm_mips_int7_type;
	/* Install all the 7xxx IRQs */
 	/* IRQs from 1 to 32 - 0 reserved for main IRQ and 60 for timer */
	for (irq = 1; irq <= 32; ++irq)
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

	/* Set up soft modem interrupts. */
        irq_desc[BCM_LINUX_SOFT_MODEM_DMA_IRQS].status = IRQ_DISABLED;
        irq_desc[BCM_LINUX_SOFT_MODEM_DMA_IRQS].action = 0;
        irq_desc[BCM_LINUX_SOFT_MODEM_DMA_IRQS].depth = 1;
        irq_desc[BCM_LINUX_SOFT_MODEM_DMA_IRQS].chip = &brcm_intc_type;

        /* Set up smartcard interrupts. */
        irq_desc[BCM_LINUX_SCA_IRQ].status = IRQ_DISABLED;
        irq_desc[BCM_LINUX_SCA_IRQ].action = 0;
        irq_desc[BCM_LINUX_SCA_IRQ].depth = 1;
        irq_desc[BCM_LINUX_SCA_IRQ].chip = &brcm_intc2_type;
        
        irq_desc[BCM_LINUX_SCB_IRQ].status = IRQ_DISABLED;
        irq_desc[BCM_LINUX_SCB_IRQ].action = 0;
        irq_desc[BCM_LINUX_SCB_IRQ].depth = 1;
        irq_desc[BCM_LINUX_SCB_IRQ].chip = &brcm_intc2_type;

		/* Set up EXT_1 and EXT_2 interrupts */
        irq_desc[BCM_LINUX_EXT_1_IRQ].status = IRQ_DISABLED;
        irq_desc[BCM_LINUX_EXT_1_IRQ].action = 0;
        irq_desc[BCM_LINUX_EXT_1_IRQ].depth = 1;
        irq_desc[BCM_LINUX_EXT_1_IRQ].chip = &brcm_mips_int3_type;

        irq_desc[BCM_LINUX_EXT_2_IRQ].status = IRQ_DISABLED;
        irq_desc[BCM_LINUX_EXT_2_IRQ].action = 0;
        irq_desc[BCM_LINUX_EXT_2_IRQ].depth = 1;
        irq_desc[BCM_LINUX_EXT_2_IRQ].chip = &brcm_mips_int4_type;


#ifdef CONFIG_OPROFILE
	/* profile IRQ */
	irq_desc[BCM_PERFCOUNT_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_PERFCOUNT_IRQ].action = 0;
	irq_desc[BCM_PERFCOUNT_IRQ].depth = 1;
	irq_desc[BCM_PERFCOUNT_IRQ].chip = &brcm_mips_performance_type;
	brcm_mips_performance_enable(0);
#endif
	
	brcm_mips_int2_enable(0);
	INTC->IrqMask |= 0x1UL << (BCM_LINUX_UPG_IRQ -1);

}


void (*irq_setup)(void);

void __init arch_init_irq(void)
{
	irq_setup();
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

#if 1
void irq_display_intr_mask(void)
{
	printk("############## Interrupt mask=%08x ############ \n", INTC->IrqMask);
}
EXPORT_SYMBOL(irq_display_intr_mask);
#endif

