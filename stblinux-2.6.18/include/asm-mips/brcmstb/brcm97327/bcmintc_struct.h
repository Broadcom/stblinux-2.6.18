/***********************************************************************/
/*                                                                     */
/*   MODULE:  bcmintc_struct.h                                                */
/*   DATE:    96/12/19                                                 */
/*   PURPOSE: Structures for Interrupt controller (INTC) block        */
/*                                                                     */
/***********************************************************************/

/***************************************************************************
 *     Copyright (c) 1999-2005, Broadcom Corporation
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
 */

#ifndef BCMINTC_STRUCT_H
#define BCMINTC_STRUCT_H

#ifdef _cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE


typedef struct IntControl {
  unsigned long        RevID;          /* (00) */
  unsigned long        testControl;    /* (04) */
  unsigned long        unused0;        /* (08) */
  unsigned long        IrqMask;        /* (0c) */
  unsigned long        IrqStatus;      /* (10) */
#define	TIMER_IRQ		0x80000000
#define	SDS_RX2_IRQ		0x40000000
#define	SDS_RX1_IRQ		0x20000000
#define	ADP_C_IRQ		0x10000000
#define	UART_IRQ		0x08000000
#define	ATP_C_IRQ		0x04000000
#define	ADP_B_IRQ		0x02000000
#define	ATP_B_IRQ		0x01000000
#define	ADP_A_IRQ		0x00800000
#define	ATP_A_IRQ		0x00400000
#define	RFM_IRQ			0x00200000
#define	XPT_MSG_IRQ		0x00100000
#define	XPT_OVFLOW_IRQ		0x00080000
#define	XPT_STATUS_IRQ		0x00040000
#define	MINITITAN_IRQ		0x00020000
#define	BGE_IRQ			0x00010000
#define	BGE_VEC1_IRQ		0x00008000
#define	BGE_VEC0_IRQ		0x00004000
#define	DLL_DRIFT_IRQ		0x00002000
#define	PERIPH_POST_DONE_IRQ	0x00001000
#define	I2C_IRQ			0x00000800
#define	SPI_IRQ			0x00000400
#define	UPG_IRQ			0x00000200
#define	MODEM_IRQ		0x00000100
#define	USB_HOST_IRQ		0x00000080
#define	PCI_IRQ			0x00000040
#define	SDS_HOST_IRQ		0x00000020
#define	SATA_IRQ   		0x00000010
#define	XPT_ICAM_IRQ		0x00000008
#define	EBI_TX_IRQ		0x00000004
#define	EBI_RX_IRQ		0x00000002
#define	DMA_IRQ			0x00000001
  unsigned char        extIrqConfig; /* (17) */
  unsigned char        extIrqStatus; /* (16) */
#define EXTIRQ_STS_MASK 0xf8
#define EXTIRQ5_STS     0x80
#define EXTIRQ4_STS     0x40
#define EXTIRQ3_STS     0x20
#define EXTIRQ2_STS     0x10
#define EXTIRQ1_STS     0x08
  unsigned char        extIrqClr; /* (15) */
#define EXTIRQ5_CLR     0x10
#define EXTIRQ4_CLR     0x08
#define EXTIRQ3_CLR     0x04
#define EXTIRQ2_CLR     0x02
#define EXTIRQ1_CLR     0x01
  unsigned char        extIrqMask; /* (14) */
#define EXTIRQ_EN_MASK  0x1f
#define EXTIRQ5_EN      0x10
#define EXTIRQ4_EN      0x08
#define EXTIRQ3_EN      0x04
#define EXTIRQ2_EN      0x02
#define EXTIRQ1_EN      0x01
} IntControl;                                

#define INTC ((IntControl * const)(INTC_ADR_BASE))


#endif /* _ASMLANGUAGE */

#ifdef _cplusplus
}
#endif

#endif
