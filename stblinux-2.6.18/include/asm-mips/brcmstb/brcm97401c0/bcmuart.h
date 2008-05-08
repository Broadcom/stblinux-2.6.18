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
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 7/19/06 11:09a $
 *
 * Module Description:  Definitions for UART block
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/bsp/bcm97401/common/bcmuart.h $
 * 
 * Hydra_Software_Devel/7   7/19/06 11:09a jkim
 * PR14344: Use (BCHP_VER >= BCHP_VER_B0) instead of (BCHP_REV_B0==1)
 * 
 * Hydra_Software_Devel/6   6/27/06 2:02p garylin
 * PR14344: fix comment for vxWorks
 * 
 * Hydra_Software_Devel/5   6/21/06 9:53a jkim
 * PR14344: Add support for B0
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

#if !defined __ASSEMBLY__
#ifdef _cplusplus
extern "C" {
#endif
#endif

/* UART register base addresses */
#define UARTA_ADR_BASE   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_UARTA_RCVSTAT)
#define UARTB_ADR_BASE   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_UARTB_RCVSTAT)
/* B0 UARTC has changed to one like 7400 */
#define UARTC_ADR_BASE   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_UARTC_RBR)

/* Define console using UART_ADR_BASE here. Console could be UARTB or UARTC for 7401.
 * No other files need to be modified for SDE.
 */
#define UART_ADR_BASE	UARTB_ADR_BASE
#define console_out		uartb_out 

/* UART registers */
/* used for UARTC */
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

/* used for UARTA and UARTB */
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

#if !defined __ASSEMBLY__

/**********************************************************************
  Uart Register Structure
 **********************************************************************/
typedef struct UartChannelNew {
  volatile unsigned long sdw_rbr_thr_dll;	/* 0x00 */
  volatile unsigned long sdw_dlh_ier;	/* 0x04 */
  volatile unsigned long sdw_iir_fcr;	/* 0x08 */
  volatile unsigned long sdw_lcr;	/* 0x0c */
  volatile unsigned long sdw_mcr;	/* 0x10 */
  volatile unsigned long sdw_lsr;	/* 0x14 */
  volatile unsigned long sdw_msr;	/* 0x18 */
  volatile unsigned long sdw_scr;	/* 0x1c */
} UartChannelNew; /* Used for UARTC */

typedef struct UartChannel {
  volatile unsigned long rxstat;
  volatile unsigned long rxdata;
  volatile unsigned long unused;
  volatile unsigned long control;
  volatile unsigned long baudh;
    /* When divide SysClk/2/(1+baudword) we should get 32*bit-rate */
  volatile unsigned long baudl;
  volatile unsigned long txstat;
  volatile unsigned long txdata;
} UartChannel;

#define UARTA ((volatile UartChannel *) UARTA_ADR_BASE)
#define UARTB ((volatile UartChannel *) UARTB_ADR_BASE)
#define UARTC ((volatile UartChannel *) UARTC_ADR_BASE)

#define UART  ((volatile UartChannel *)	UART_ADR_BASE)


#endif /* __ASSEMBLY__ */

/******************************************************************
 * Baud Rate Table
 * XTALFREQ / baud rate / 16
 ******************************************************************/
#define BAUD_VAL(x)     (((XTALFREQ/8/(x) + 1)/2) - 1)
#define BAUD_VAL_HI(x)  ((BAUD_VAL(x) >> 8) & 0xff)
#define BAUD_VAL_LO(x)  (BAUD_VAL(x) & 0xff)

#define BAUD_1200_HI    BAUD_VAL_HI(1200)
#define BAUD_1200_LO    BAUD_VAL_LO(1200)
#define BAUD_2400_HI    BAUD_VAL_HI(2400)
#define BAUD_2400_LO    BAUD_VAL_LO(2400)
#define BAUD_4800_HI    BAUD_VAL_HI(4800)
#define BAUD_4800_LO    BAUD_VAL_LO(4800)
#define BAUD_9600_HI    BAUD_VAL_HI(9600)
#define BAUD_9600_LO    BAUD_VAL_LO(9600)
#define BAUD_19200_HI   BAUD_VAL_HI(19200)
#define BAUD_19200_LO   BAUD_VAL_LO(19200)
#define BAUD_38400_HI   BAUD_VAL_HI(38400)
#define BAUD_38400_LO   BAUD_VAL_LO(38400)
#define BAUD_57600_HI   BAUD_VAL_HI(57600)
#define BAUD_57600_LO   BAUD_VAL_LO(57600)
#define BAUD_115200_HI  BAUD_VAL_HI(115200)
#define BAUD_115200_LO  BAUD_VAL_LO(115200)

#if !defined __ASSEMBLY__
#ifdef __cplusplus
}
#endif
#endif

#endif  /* _BCMUART_H */
