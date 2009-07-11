/*
 * arch/mips/brcmstb/common/brcmirq.h
 *
 * Copyright (C) 2009 Broadcom Corporation
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
 * macros for interrupt routines for Broadcom eval boards
 *
 * 05-05-2009   jipeng    Created
 */ 
#ifndef ARCH_MIPS_BRCMSTB_COMMON_BRCMIRQ_H	
#define ARCH_MIPS_BRCMSTB_COMMON_BRCMIRQ_H	

/* BUILD_MIPS_INT_*(intnum) can be used separately or inside
   BUILD_MIPS_INT_HANDLER(intnum)
*/

#define	BUILD_MIPS_INT_ENABLE(intnum)						\
static inline void brcm_mips_int##intnum##_enable(unsigned int irq)		\
{										\
	set_c0_status(STATUSF_IP##intnum);					\
}										\
									
#define	BUILD_MIPS_INT_DISABLE(intnum)						\
static inline void brcm_mips_int##intnum##_disable(unsigned int irq)		\
{										\
	clear_c0_status(STATUSF_IP##intnum);					\
}										\
									
#define	BUILD_MIPS_INT_ACK(intnum)						\
static inline void brcm_mips_int##intnum##_ack(unsigned int irq)		\
{										\
	/* Already done in brcm_irq_dispatch */					\
}										\
									
#define	BUILD_MIPS_INT_STARTUP(intnum)						\
static inline unsigned int brcm_mips_int##intnum##_startup(unsigned int irq)	\
{ 										\
	brcm_mips_int##intnum##_enable(irq);					\
	return 0; /* never anything pending */					\
}										\
									
#define	BUILD_MIPS_INT_END(intnum)						\
static inline void brcm_mips_int##intnum##_end(unsigned int irq)		\
{										\
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))		\
		brcm_mips_int##intnum##_enable(irq);				\
}										\
									
#define	BUILD_MIPS_INT_DISPATCH(intnum)						\
void inline brcm_mips_int##intnum##_dispatch(struct pt_regs *regs)		\
{										\
	brcm_mips_int##intnum##_disable(intnum);				\
}										\
									
#define	BUILD_MIPS_INT_TYPE(intnum)						\
static struct hw_interrupt_type brcm_mips_int##intnum##_type = {		\
	typename: "BCM MIPS INT##intnum",					\
	startup: brcm_mips_int##intnum##_startup,				\
	shutdown: brcm_mips_int##intnum##_disable,				\
	enable: brcm_mips_int##intnum##_enable,					\
	disable: brcm_mips_int##intnum##_disable,				\
	ack: brcm_mips_int##intnum##_ack,					\
	end: brcm_mips_int##intnum##_end,					\
	NULL									\
};										\

#define	BUILD_MIPS_INT_HANDLER(intnum)						\
	BUILD_MIPS_INT_ENABLE(intnum)						\
	BUILD_MIPS_INT_DISABLE(intnum)						\
	BUILD_MIPS_INT_ACK(intnum)						\
	BUILD_MIPS_INT_STARTUP(intnum)						\
	BUILD_MIPS_INT_END(intnum)						\
	BUILD_MIPS_INT_DISPATCH(intnum)						\
	BUILD_MIPS_INT_TYPE(intnum)						\

#ifndef	L1_IRQS
#if defined(BCHP_HIF_CPU_INTR1_INTR_W2_STATUS)
#define L1_IRQS         96
#elif defined(BCHP_HIF_CPU_INTR1_INTR_W1_STATUS)
#define L1_IRQS         64
#endif
#endif

/* 
  This section defines some common debugging functions that can help debug interrupts 
  DEBUG_UARTA_INTR			- debug L1 interrupt
  DEBUG_UARTA_INTR_FROM_INT2IRQ		- debug L1 interrupt from UART B
#endif
*/
//#define	DEBUG_UARTA_INTR
//#define	DEBUG_UARTA_INTR_FROM_INT2IRQ

#if	defined(DEBUG_UARTA_INTR) || defined(DEBUG_UARTA_INTR_FROM_INT2IRQ)
int gDebugPendingIrq0, gDebugPendingIrq1, gDebugPendingIrq2;
int gDebugMaskW0, gDebugMaskW1, gDebugMaskW2;
#define PRINTK(msg) 	uartB_puts(msg)
#else
#define PRINTK(msg)	 
#endif

#ifdef	DEBUG_UARTA_INTR_FROM_INT2IRQ
static inline void dump_INTC_regs(void)
{
	unsigned int status0, status1, status2;
	unsigned int pendingIrqs, pendingIrqs1, pendingIrqs2;
	unsigned int mask0, mask1, mask2;
	char msg[80];
	pendingIrqs2 = mask2 = 0;

	pendingIrqs = status0 = CPUINT1C->IntrW0Status;
	pendingIrqs &= ~(mask0 = CPUINT1C->IntrW0MaskStatus);

	pendingIrqs1 = status1 = CPUINT1C->IntrW1Status;
	pendingIrqs1 &= ~(mask1 = CPUINT1C->IntrW1MaskStatus);

#if	 L1_IRQS > 64
	pendingIrqs2 = status2 = CPUINT1C->IntrW2Status;
	pendingIrqs2 &= ~(mask2 = CPUINT1C->IntrW2MaskStatus);
#endif

	sprintf(msg, "\n");
	uartB_puts(msg);

	if((gDebugPendingIrq0 != pendingIrqs) || (gDebugPendingIrq1 != pendingIrqs1) || (gDebugPendingIrq2 != pendingIrqs2))
	{
		sprintf(msg, "Last pending0=%08x, pd1=%08x, pd2=%08x\n", 
			gDebugPendingIrq0, gDebugPendingIrq1, gDebugPendingIrq2);
		uartB_puts(msg);
	}

	if((gDebugMaskW0 != mask0) || (gDebugMaskW1 != mask1) || (gDebugMaskW2 != mask2))
	{
		sprintf(msg, "Last MaskW0=%08x, mW1=%08x, mW2=%08x\n", 
			gDebugMaskW0, gDebugMaskW1, gDebugMaskW2);
		uartB_puts(msg);
	}

	sprintf(msg, "Current pending0=%08x, pd1=%08x,  pd2=%08x\n", 
		pendingIrqs, pendingIrqs1, pendingIrqs2);
	uartB_puts(msg);

	sprintf(msg, "Current MaskW0=%08x, mW1=%08x, mW2=%08x\n", 
		mask0, mask1, mask2);
	uartB_puts(msg);

	if((status0 != pendingIrqs) || (status1 != pendingIrqs1) || (status2 != pendingIrqs2))
	{
		sprintf(msg, "Current status W0=%08x, W1=%08x, W2=%08x\n", 
			status0, status1, status2);
		uartB_puts(msg);
	}
}
#else
static inline void dump_INTC_regs(void)		{}
#endif

#endif	/* ARCH_MIPS_BRCMSTB_COMMON_BRCMIRQ_H */
