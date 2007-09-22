/***************************************************************************
 *     Copyright (c) 1999-2006, Broadcom Corporation
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
 *   MODULE:  bcmuart.h                                                
 *   DATE:    02/15/02                                                 
 *   PURPOSE: Definitions for UART block                               
 *                                                                     
 *************************************************************************/
#ifndef _BCMUART_H
#define _BCMUART_H

#include "bcmmips.h"
#include "boardcfg.h"
#include "bchp_common.h"
#include "bchp_uarta.h"
#include "bchp_uartb.h"
#include "bchp_uartc.h"

#if !defined _ASMLANGUAGE
#ifdef __cplusplus
extern "C" {
#endif
#endif

/* UART register base addresses */
#define UARTA_ADR_BASE   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_UARTA_RBR)
#define UARTB_ADR_BASE   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_UARTB_RBR)
#define UARTC_ADR_BASE   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_UARTC_RBR)

#define UART_ADR_BASE	UARTA_ADR_BASE
#define console_out		_writeasm 

/* UART registers */
#define UART_SDW_RBR     	0x00
#define UART_SDW_THR     	0x00
#define UART_SDW_DLL     	0x00
#define UART_SDW_DLH     	0x04
#define UART_SDW_IER     	0x04
#define UART_SDW_IIR     	0x08
#define UART_SDW_FCR     	0x08
#define UART_SDW_LCR     	0x0c
#define UART_SDW_MCR     	0x10
#define UART_SDW_LSR     	0x14
#define UART_SDW_MSR     	0x18
#define UART_SDW_SCR     	0x1c

/* LCR bit definitions */
#define DLAB                            0x80
#define DLS_8BITS                       0x03

/* LSR bit definitions */
#define RFE                             0x80
#define TEMT                            0x40
#define THRE                            0x20
#define BI                              0x10
#define FE                              0x08
#define PE                              0x04
#define OE                              0x02
#define DR                              0x01

#if !defined _ASMLANGUAGE

/**********************************************************************
  Uart Register Structure
 **********************************************************************/
typedef struct UartChannel {
    volatile unsigned long sdw_rbr_thr_dll;	/* 0x00 */
    volatile unsigned long sdw_dlh_ier;	/* 0x04 */
    volatile unsigned long sdw_iir_fcr;	/* 0x08 */
    volatile unsigned long sdw_lcr;	/* 0x0c */
    volatile unsigned long sdw_mcr;	/* 0x10 */
    volatile unsigned long sdw_lsr;	/* 0x14 */
    volatile unsigned long sdw_msr;	/* 0x18 */
    volatile unsigned long sdw_scr;	/* 0x1c */
} UartChannel;

#define UARTA ((volatile UartChannel *) UARTA_ADR_BASE)
#define UARTB ((volatile UartChannel *) UARTB_ADR_BASE)
#define UARTC ((volatile UartChannel *) UARTC_ADR_BASE)

#define UART  ((volatile UartChannel *)	UART_ADR_BASE)

#endif /* _ASMLANGUAGE */

#if !defined _ASMLANGUAGE
#ifdef __cplusplus
}
#endif
#endif

#endif  /* _BCMUART_H */
