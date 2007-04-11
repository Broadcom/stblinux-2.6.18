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
 *
 * $brcm_Workfile: bcmtmreg.h $
 * $brcm_Revision: Irvine_Diags_Devel/1 $
 * $brcm_Date: 4/15/03 4:51p $
 *
 * Module Description: Header file for TM registers
 *
 * Revision History:
 *
 * $brcm_Log: /SetTop/hardwareinterface/7328/include/bcmtmreg.h $
 * 
 * Irvine_Diags_Devel/1   4/15/03 4:51p brianlee
 * New version for 7328.
 * 
 * 
 ***************************************************************************/
#ifndef BCMTMREG_H
#define BCMTMREG_H

#ifdef _cplusplus
extern "C" {
#endif

/***************************************************************************/
/* These are "normal" register offsets within the TM module                */
/***************************************************************************/
#define TM_MISCRST				0x00
#define TM_PWMCTL				0x03
#define TM_PWMSEL				0x04
#define TM_SDSPHY_SEL			0x09
#define TM_QDAC_CTL				0x0A
#define TM_VECMISC				0x0E
#define TM_CLKMON				0x0F
#define TM_PWRDWN0				0x10
#define TM_PWRDWN1				0x11
#define TM_DISP_CLK_RFM_SEL		0x15
#define TM_DISPCKEN				0x1D
#define TM_WATCHDOG_CTL			0x21
#define TM_SFT_RST0				0x24
#define TM_SFT_RST1				0x25
#define TM_VCXO_BIAS_CNT0		0x36
#define TM_VCXO_BIAS_CNT1		0x37
#define TM_VCXO_CPPM_PLL		0x38
#define TM_VCXO_WDOG_CTRL		0x39
#define TM_EBI68K				0x40
#define TM_JDA_CTRL				0x41
#define TM_UPG_CTRL				0x42
#define TM_SCB_CTRL				0x43

/***************************************************************************/
/* These are the test register offsets within the TM module                */
/* These offsets are from the base of the TM (8000)						   */
/***************************************************************************/
#define TM_PINSTRAP2	0x82
#define TM_TMODE		0x83
#define TM_TPOUT0		0x84
#define TM_TPOUT1		0x85
#define TM_TPOUT2		0x86
#define TM_PINSTRAP		0x89
#define TM_MIPSBIST0	0x8A
#define TM_MIPSBIST1	0x8B
#define TM_QDAC_TEST 	0x8C
#define TM_TI2CCTL0		0x8D
#define TM_TI2CCTL1		0x8E
#define TM_RDBOND		0x92
#define TM_TPREG_EN		0x93
#define TM_OSCCTL		0x9B
#define TM_SCAN_TRI_ISB	0xA1
#define TM_SCAN_TRI_SCB	0xA2
#define TM_SCAN_TRI_RBUS	0xA3
#define TM_SCAN_TRI_PB	0xA4

/***************************************************************************/
/* Bit definitions														   */
/***************************************************************************/
/* TM_MISCRST */
#define MISCRST_FPRST	0x01
#define MISCRST_FPPOL	0x02
#define MISCRST_SC		0x04

/* TM_PWMSEL */
#define PWMSEL_A_MASK	0x07
#define PWMSEL_A_CVT	0x08
#define PWMSEL_B_MASK	0x70
#define PWMSEL_B_CVT	0x80
#define PWMSEL_B_SHIFT	0x04

/* TM_SDSPHY_SEL */
#define SDSPHY_BYP_PLL	0x10
#define SDSPHY_TP_IN	0x20
#define SDSPHY_AFEOUT	0x40

/* TM_TDAC_CTL */
#define QDAC_LEVEL		0x0f
#define QDAC_INPUT_012	0x10

/* TM_CLKMON */
#define CLKMON_BIT		0x01

/* TM_SCB_CTRL */
#define SCB_ENABLE		0x01

#ifdef _cplusplus
}
#endif

#endif
