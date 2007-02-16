/*
 * arch/mips/brcm/irq.c
 *
 * Copyright (C) 2001-2005 Broadcom Corporation
 *                    
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
 * 02-05-04	THT    Created from 97315
 */
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/brcmstb/brcm97314/bcm97314.h>
#include <asm/brcmstb/brcm97314/bcmintrnum.h>

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
 *        0                  *Unused*
 *        1                  *Unused*
 *        2                  *Unused* 
 *        3                  *Unused*
 *        4                  *Unused*
 *        5                  *Unused*
 *        6                  *Unused*
 *        7                  *Unused* 
 *        8                  *Unused*  
 *        9                  *Unused* 
 *       10           2      USB_HOST1. 
 *       11                  *Unused* 
 *       12                  *Unused* 
 *       13                  *Unused*
 *       14           2      ide0 controller.
 *       15           2      ide1 controller.
 *       16                  *Unused*
 *       17           2      Broadcom  UARTA for console.
 *	 18	  	     *Unused*
*	 33		     UARTA
 *	 34		     UARTB
 *	 35		     SOFT MODEM
 *	 36		     4413
 *     	 37,38	      2      Smart Card A and B
 *       60           7      R4k timer (used for master system time)
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

        if (irq ==  BCM_LINUX_EBI_TX_IRQ)
		return;
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
        if (irq ==  BCM_LINUX_SOFT_MODEM_DMA_IRQS) {
	   INTC->IrqMask |= BCM_LINUX_DMA_IRQ; /* 1 */
	   INTC->IrqMask |= BCM_LINUX_MODEM_IRQ; /* 9 */
           DMA_CHAN[MODEM_RX_CHAN].intMask |= (DMA_BUFF_DONE|DMA_DONE|DMA_NO_DESC);
           DMA_CHAN[MODEM_TX_CHAN].intMask |= (DMA_BUFF_DONE|DMA_DONE|DMA_NO_DESC);
	   return;
	}
	save_and_cli(flags);
	INTC->IrqMask |= (0x1UL<<shift);
	restore_flags(flags);
}

static void brcm_intc_disable(unsigned int irq)
{
	unsigned int shift = irq - 1;
	unsigned long flags;


        if (irq ==  BCM_LINUX_EBI_TX_IRQ)
		return;

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
	if (irq == BCM_LINUX_UPG_IRQ)
		return;
        if (irq ==  BCM_LINUX_SOFT_MODEM_DMA_IRQS) {
            save_and_cli(flags);
            DMA_CHAN[MODEM_RX_CHAN].intMask &= (~(DMA_BUFF_DONE|DMA_DONE|DMA_NO_DESC));
            DMA_CHAN[MODEM_TX_CHAN].intMask &= (~(DMA_BUFF_DONE|DMA_DONE|DMA_NO_DESC));
            restore_flags(flags);
	    return;
	}
	if (irq > 0 && irq <= 32) {
 		save_and_cli(flags);
		INTC->IrqMask &= (~(0x1UL<<shift));
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

void brcm_mips_int3_dispatch(struct pt_regs *regs)
{
	printk("brcm_mips_int3_dispatch: Placeholder only, should not be here \n");
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
	extern asmlinkage void brcmIRQ(void);

	INTC->IrqMask = 0UL;
	INTC->IrqStatus = 0UL;
	
	set_except_vector(0, brcmIRQ);
	/* IMHO we do not need to do the following here, the request IRQ would take care of it. */
	//change_c0_status(ST0_IM, IE_IRQ0 | IE_IRQ5);  // right now we only care about IRQ0 and 5.
	change_c0_status(ST0_IE, ST0_IE);
	
	/* Setup timer interrupt */
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].status = IRQ_DISABLED;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].action = 0;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].depth = 1;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].handler = &brcm_mips_int7_type;
	/* Install all the 7xxx IRQs */
 	/* IRQs from 1 to 32 - 0 reserved for main IRQ and 60 for timer */
	for (irq = 1; irq <= 32; ++irq)
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

	/* Set up soft modem interrupts. */
        irq_desc[BCM_LINUX_SOFT_MODEM_DMA_IRQS].status = IRQ_DISABLED;
        irq_desc[BCM_LINUX_SOFT_MODEM_DMA_IRQS].action = 0;
        irq_desc[BCM_LINUX_SOFT_MODEM_DMA_IRQS].depth = 1;
        irq_desc[BCM_LINUX_SOFT_MODEM_DMA_IRQS].handler = &brcm_intc_type;

        /* Set up smartcard interrupts. */
        irq_desc[BCM_LINUX_SCA_IRQ].status = IRQ_DISABLED;
        irq_desc[BCM_LINUX_SCA_IRQ].action = 0;
        irq_desc[BCM_LINUX_SCA_IRQ].depth = 1;
        irq_desc[BCM_LINUX_SCA_IRQ].handler = &brcm_intc2_type;
        
        irq_desc[BCM_LINUX_SCB_IRQ].status = IRQ_DISABLED;
        irq_desc[BCM_LINUX_SCB_IRQ].action = 0;
        irq_desc[BCM_LINUX_SCB_IRQ].depth = 1;
        irq_desc[BCM_LINUX_SCB_IRQ].handler = &brcm_intc2_type;

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
