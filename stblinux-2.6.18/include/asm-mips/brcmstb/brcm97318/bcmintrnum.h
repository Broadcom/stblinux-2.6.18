/*     Copyright (c) 1999-2005, Broadcom Corporation
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
 *        4                 *Unused*
 *        5                 *Unused*
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
 *	 18		     *Unused*
 *       60           7      R4k timer (used for master system time)
 *
 * Again, I cannot stress this enough, keep this table up to date!!!
 */

/* JPF Serial Code depends on a unique IRQ for each serial port */
#define BCM_LINUX_UARTA_IRQ		33
#define BCM_LINUX_UARTB_IRQ		34

/* JPF Each line in the INTC has an IRQ */
/* THT: Straight from the RDB */
#define INTERRUPT_ID_TIMER_IRQ		0x1f   
#define INTERRUPT_ID_ENET_IRQ		0x1c  
#define INTERRUPT_ID_UART_IRQ		0x1b
#define INTERRUPT_ID_BGE_VEC_IRQ	0x1a
#define INTERRUPT_ID_AUDIO_IRQ		0x18
#define INTERRUPT_ID_ADP_A_IRQ		0x17
#define INTERRUPT_ID_ATP_A_IRQ		0x16
#define INTERRUPT_ID_XPT_ICAM_IRQ	0x15
#define INTERRUPT_ID_XPT_MSG_IRQ	0x14
#define INTERRUPT_ID_XPT_OVFLOW_IRQ	0x13
#define INTERRUPT_ID_XPT_STATUS_IRQ	0x12
#define INTERRUPT_ID_MINITITAN_IRQ	0x11
#define INTERRUPT_ID_BGE_IRQ		0x10
#define INTERRUPT_ID_DAA_RING_IRQ	0x0d
#define INTERRUPT_ID_DAA_IRQ		0x0c
#define INTERRUPT_ID_I2C_IRQ		0x0b
#define INTERRUPT_ID_SPI_IRQ		0x0a
#define INTERRUPT_ID_UPG_IRQ		0x09
#define INTERRUPT_ID_SM_IRQ			0x08
#define INTERRUPT_ID_USB_HOST_IRQ	0x07
#define INTERRUPT_ID_ATP_C_IRQ		0x06
#define INTERRUPT_ID_ADP_C_IRQ		0x05
#define INTERRUPT_ID_PCM_IRQ		0x04
#define INTERRUPT_ID_IDE_IRQ		0x03
#define INTERRUPT_ID_EBI_TX_IRQ		0x02
#define INTERRUPT_ID_EBI_RX_IRQ		0x01
#define INTERRUPT_ID_DMA_IRQ		0x00

/* THT: Linux IRQ is 1 + the RDB bit position */
#define	BCM_LINUX_TIMR_IRQ			(1+INTERRUPT_ID_TIMER_IRQ)
#define BCM_LINUX_ENET_IRQ			(1+INTERRUPT_ID_ENET_IRQ)
#define	BCM_LINUX_UART_IRQ			(1+INTERRUPT_ID_UART_IRQ)
#define	BCM_LINUX_BGE_VEC_IRQ		(1+INTERRUPT_ID_BGE_VEC_IRQ)
#define	BCM_LINUX_ADP_AUDIO_IRQ		(1+INTERRUPT_ID_AUDIO_IRQ)
#define	BCM_LINUX_ADP_A_IRQ			(1+INTERRUPT_ID_ADP_A_IRQ)
#define	BCM_LINUX_ATP_A_IRQ			(1+INTERRUPT_ID_ATP_A_IRQ)
#define	BCM_LINUX_XPT_ICAM_IRQ		(1+INTERRUPT_ID_XPT_ICAM_IRQ)
#define	BCM_LINUX_XPT_MSG_IRQ		(1+INTERRUPT_ID_XPT_MSG_IRQ)
#define	BCM_LINUX_XPT_OVFLOW_IRQ	(1+INTERRUPT_ID_XPT_OVFLOW_IRQ)
#define	BCM_LINUX_XPT_STATUS_IRQ	(1+INTERRUPT_ID_XPT_STATUS_IRQ)
#define	BCM_LINUX_MINITITAN_IRQ		(1+INTERRUPT_ID_MINITITAN_IRQ)
#define	BCM_LINUX_BGE_CPU_IRQ		(1+INTERRUPT_ID_BGE_IRQ)
#define BCM_LINUX_DAA_RING_IRQ		(1+INTERRUPT_ID_DAA_RING_IRQ)
#define BCM_LINUX_DAA_IRQ			(1+INTERRUPT_ID_DAA_IRQ)
#define	BCM_LINUX_I2C_IRQ			(1+INTERRUPT_ID_I2C_IRQ)
#define	BCM_LINUX_SPI_IRQ			(1+INTERRUPT_ID_SPI_IRQ)
#define	BCM_LINUX_UPG_IRQ			(1+INTERRUPT_ID_UPG_IRQ)
#define	BCM_LINUX_SM_IRQ			(1+INTERRUPT_ID_SM_IRQ)
#define	BCM_LINUX_USB_HOST_IRQ		(1+INTERRUPT_ID_USB_HOST_IRQ)
#define	BCM_LINUX_ATP_C_IRQ			(1+INTERRUPT_ID_ATP_C_IRQ)
#define	BCM_LINUX_ADP_C_IRQ			(1+INTERRUPT_ID_ADP_C_IRQ)
#define	BCM_LINUX_BGE_PCM_IRQ		(1+INTERRUPT_ID_PCM_IRQ)
#define	BCM_LINUX_IDE_IRQ			(1+INTERRUPT_ID_IDE_IRQ)
#define	BCM_LINUX_EBI_TX_IRQ		(1+INTERRUPT_ID_EBI_TX_IRQ)
#define	BCM_LINUX_EBI_RX_IRQ		(1+INTERRUPT_ID_EBI_RX_IRQ)
#define	BCM_LINUX_DMA_IRQ			(1+INTERRUPT_ID_DMA_IRQ)


/* Soft Modem */
#define BCM_LINUX_SOFT_MODEM_DMA_IRQS	35

/* For naming convention compatibility across platforms */
#define BCM_LINUX_USB_IRQ BCM_LINUX_USB_HOST_IRQ
#define BCM_LINUX_MODEM_IRQ BCM_LINUX_SM_IRQ
#define BCM_LINUX_IDE0_IRQ BCM_LINUX_IDE_IRQ
#define BCM_LINUX_CPU_ENET_IRQ BCM_LINUX_ENET_IRQ

/* Smart Card */
#define BCM_LINUX_SCA_IRQ		40
#define BCM_LINUX_SCB_IRQ		41

#define BCM_LINUX_EXT_1_IRQ      51
#define BCM_LINUX_EXT_2_IRQ      52
#define BCM_LINUX_EXT_3_IRQ      53
#define BCM_LINUX_EXT_4_IRQ      54
#define BCM_LINUX_EXT_5_IRQ      55

#define BCM_LINUX_SYSTIMER_IRQ		60


#define BCM_PERFCOUNT_IRQ (BCM_LINUX_SYSTIMER_IRQ+1)

#ifdef __cplusplus
}
#endif

#endif


