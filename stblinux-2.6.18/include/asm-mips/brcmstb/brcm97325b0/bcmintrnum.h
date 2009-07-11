/*     Copyright (c) 1999-2007, Broadcom Corporation
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
** File:		bcmintrnum.h
** Description: Linux system int number header file
**
** Created: 03/05/2002 by Qiang Ye
** Updated for SMP: 03/10/2006 by Troy Trammel
** Modifed for 7440B0: 01/15/07 by Troy Trammel
**
** REVISION:
**
** $Log: $
**
**
****************************************************************/
#ifndef BCMINTRNUM_H
#define BCMINTRNUM_H


#ifdef __cplusplus
extern "C" {
#endif

/*
 refer to arch/mips/brcmstb/brcm97325b0/mt_irq.c for SMTC kernel interrupt assignment,
 or, arch/mips/brcmstb/brcm97325b0/irq.c for UP kernel interrupt assignment
*/

#define	BCM_LINUX_UARTA_IRQ	(1+BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_UART0_CPU_INTR_SHIFT)
#define	BCM_LINUX_UARTB_IRQ	(1+32+32+BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_UPG_UART1_CPU_INTR_SHIFT)
#define	BCM_LINUX_UARTC_IRQ	(1+32+32+BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_UPG_UART2_CPU_INTR_SHIFT)

#define	BCM_LINUX_CPU_ENET_IRQ        	(1+BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_ENET_EMAC1_CPU_INTR_SHIFT)
#define POD_IRQ_NUM  			(1+32+BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_3_CPU_INTR_SHIFT )
#define BCM_LINUX_SATA_IRQ	  	(1+32+BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCI_SATA_CPU_INTR_SHIFT)     /* last one for intc level. */
#define	BCM_LINUX_IDE_IRQ 		(1+32+BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_IDE_CPU_INTR_SHIFT)
#define BCM_LINUX_1394_IRQ		(1+32+BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCI_INTA_1_CPU_INTR_SHIFT)
#define BCM_LINUX_EXT_PCI_IRQ		(1+32+BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCI_INTA_0_CPU_INTR_SHIFT)
#define BCM_LINUX_MINI_PCI_IRQ		(1+32+BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCI_INTA_2_CPU_INTR_SHIFT)
#define BCM_LINUX_EXPANSION_SLOT 	(BCM_LINUX_EXT_PCI_IRQ)

#define BCM_LINUX_USB_EHCI_CPU_INTR 	(1+32+BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_USB_EHCI_0_CPU_INTR_SHIFT)
#define BCM_LINUX_USB_EHCI_1_CPU_INTR 	(1+32+BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_USB_EHCI_1_CPU_INTR_SHIFT)

#define BCM_LINUX_USB_OHCI_0_CPU_INTR 	(1+32+BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_USB_OHCI_0_CPU_INTR_SHIFT)
#define BCM_LINUX_USB_OHCI_1_CPU_INTR 	(1+32+BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_USB_OHCI_1_CPU_INTR_SHIFT)

/* Auxiliary interrupts */
 
/* Some of the codes relies on these 3 interrupts to be consecutive */
#define BCM_LINUX_IRQ_BASE		(1+32+32+32)

#ifdef	CONFIG_MIPS_MT_SMTC		
#define BCM_LINUX_CPU_0			(BCM_LINUX_IRQ_BASE+3)
#define BCM_LINUX_CPU_1			(BCM_LINUX_IRQ_BASE+4)
#define BCM_LINUX_CPU_2			(BCM_LINUX_IRQ_BASE+5)
#define BCM_LINUX_CPU_3			(BCM_LINUX_IRQ_BASE+6)
#define BCM_LINUX_CPU_4			(BCM_LINUX_IRQ_BASE+7)
#define BCM_LINUX_CPU_5			(BCM_LINUX_IRQ_BASE+8)
#define BCM_LINUX_CPU_6			(BCM_LINUX_IRQ_BASE+9)
#define BCM_LINUX_CPU_7			(BCM_LINUX_IRQ_BASE+10)

#define BCM_LINUX_IPC_0_IRQ		BCM_LINUX_CPU_0		// 100
#define BCM_LINUX_IPC_1_IRQ		BCM_LINUX_CPU_1		// 101

#define BCM_LINUX_SYSTIMER_IRQ		BCM_LINUX_CPU_7		// 107
#define BCM_LINUX_SYSTIMER_1_IRQ 	(BCM_LINUX_CPU_7+1)	// 108

#define POD_DET_IRQ_NUM 		(BCM_LINUX_CPU_7+2)

#define BCM_LINUX_PERFCOUNT_IRQ		(BCM_LINUX_CPU_7+3)

#define BCM_LINUX_PATA_IRQ		(BCM_LINUX_CPU_7+4)

  // Alias for bcm71xx_ide
  #define BCM_LINUX_IDE0_IRQ		BCM_LINUX_PATA_IRQ

#define BCM_GR_BRIDGE_ERROR_IRQ	(BCM_LINUX_IRQ_BASE+10)
#else	// CONFIG_MIPS_MT_SMTC
#define BCM_LINUX_SYSTIMER_IRQ		(BCM_LINUX_IRQ_BASE+3)
#define BCM_LINUX_SYSTIMER_1_IRQ 	(BCM_LINUX_IRQ_BASE+4)

#define POD_DET_IRQ_NUM  		(BCM_LINUX_IRQ_BASE+5)

#define BCM_LINUX_IPC_0_IRQ		(BCM_LINUX_IRQ_BASE+6)
#define BCM_LINUX_IPC_1_IRQ		(BCM_LINUX_IRQ_BASE+7)

#define BCM_LINUX_PERFCOUNT_IRQ		(BCM_LINUX_IRQ_BASE+8)

#define BCM_LINUX_PATA_IRQ		(BCM_LINUX_IRQ_BASE+9)

  // Alias for bcm71xx_ide
  #define BCM_LINUX_IDE0_IRQ		BCM_LINUX_PATA_IRQ

#define BCM_GR_BRIDGE_ERROR_IRQ		(BCM_LINUX_IRQ_BASE+10)
#endif	// !CONFIG_MIPS_MT_SMTC

#ifdef __cplusplus
}
#endif

#endif


