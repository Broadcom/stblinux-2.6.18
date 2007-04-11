 /**************************************************************************
 *     Copyright (c) 2002-06 Broadcom Corporation
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
 * $brcm_Workfile: bcmuart.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 1/30/06 1:22p $
 *
 * Module Description:  Definitions for UART block
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/bsp/bcm97401/common/bcmuart.h $
 * 
 * Hydra_Software_Devel/4   1/30/06 1:22p agin
 * PR19313: Use Uart0 as default for BCM97400.
 * 
 * Hydra_Software_Devel/3   1/16/06 6:23p agin
 * PR19076: Support BCM7400.
 * 
 ***************************************************************************/
#ifndef _BCMUART_H
#define _BCMUART_H

#include "bcmmips.h"
#include "boardcfg.h"
#include "bchp_common.h"
#include "bchp_uarta.h"
#include "bchp_uartb.h"
#include "bchp_uartc.h"

#if !defined _ASMLANGUAGE
#ifdef _cplusplus
extern "C" {
#endif
#endif

/* UART register base addresses */

#define UARTA_ADR_BASE   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_UARTA_RCVSTAT)
#define UARTB_ADR_BASE   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_UARTB_RCVSTAT)
#define UARTC_ADR_BASE   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_UARTC_RBR)


/* Define console using UART_ADR_BASE here. Console could be UARTB or UARTC for 7401.
 * No other files need to be modified for SDE.
 */

#define UART_ADR_BASE	UARTB_ADR_BASE
#define console_out		uartb_out 

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


#define UART_RXSTAT     	0x00
#define UART_RXDATA     	0x04
#define UART_CONTROL    	0x0c
#define UART_BAUDHI     	0x10
#define UART_BAUDLO     	0x14
#define UART_TXSTAT     	0x18
#define UART_TXDATA     	0x1c



/* LCR bit definitions */
#define DLAB				0x80
#define DLS_8BITS			0x03

/* LSR bit definitions */
#define RFE 				0x80
#define TEMT 				0x40
#define THRE 				0x20
#define BI  				0x10
#define FE  				0x08
#define PE  				0x04
#define OE  				0x02
#define DR  				0x01


/* RXSTAT bit definitions */
#define PARERR				0x20
#define FRAMEERR			0x10
#define OVERRUNERR			0x08
#define RXDATARDY			0x04
#define RXINTEN				0x02
/* CONTROL bit definitions */
#define BITM8				0x10
#define	PAREN				0x08
#define	TXEN				0x04
#define	RXEN				0x02
#define	PODD				0x01
/* TXSTAT bit definitions */
#define	TXINTEN				0x04
#define	TXIDLE				0x02
#define	TXDREGEMT			0x01




#if !defined _ASMLANGUAGE
#ifdef _cplusplus
}
#endif
#endif

#endif  /* _BCMUART_H */
