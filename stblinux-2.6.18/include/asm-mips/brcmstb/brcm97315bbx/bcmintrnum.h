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

#define	BCM_LINUX_TIMR_IRQ		32
#define	BCM_LINUX_SDS_IRQ		30
#define	BCM_LINUX_UART_IRQ		28
#define	BCM_LINUX_VEC_IRQ		27
#define	BCM_LINUX_ATP_AUD_IRQ		25
#define	BCM_LINUX_ADP_A_IRQ		24
#define	BCM_LINUX_ATP_A_IRQ		23
#define	BCM_LINUX_XPT_ICAM_IRQ		22
#define	BCM_LINUX_XPT_MSG_IRQ		21
#define	BCM_LINUX_XPT_OVFLOW_IRQ	20
#define	BCM_LINUX_XPT_STATUS_IRQ	19
#define	BCM_LINUX_MINITITAN_IRQ		18
#define	BCM_LINUX_BGE_IRQ		17
#define	BCM_LINUX_POSTED_XFER_IRQ	16
#define	BCM_LINUX_MIPS_IRQ		15
#define	BCM_LINUX_I2C_IRQ		12
#define	BCM_LINUX_SPI_IRQ		11
#define	BCM_LINUX_UPG_IRQ		10
#define	BCM_LINUX_MODEM_IRQ		9
#define	BCM_LINUX_USB_HOST_IRQ		8
#define	BCM_LINUX_ATP_C_IRQ		7
#define	BCM_LINUX_ADP_C_IRQ		6
#define	BCM_LINUX_IDE1_IRQ   		5
#define	BCM_LINUX_IDE0_IRQ		4
#define	BCM_LINUX_EBI_TX_IRQ		3
#define	BCM_LINUX_EBI_RX_IRQ		2
#define	BCM_LINUX_DMA_IRQ		1

/* Soft Modem */
#define BCM_LINUX_SOFT_MODEM_DMA_IRQS	35

/* 4413 ethernet */
#define BCM_LINUX_4413ENET_IRQ		36

/* 802.11 lan */
#define BCM_LINUX_4307WLAN_IRQ		37

/* Smart Card */
#define BCM_LINUX_SCA_IRQ		40
#define BCM_LINUX_SCB_IRQ		41


#define BCM_LINUX_SYSTIMER_IRQ		60

#ifdef __cplusplus
}
#endif

#endif


