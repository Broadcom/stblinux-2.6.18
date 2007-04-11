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
 *
 * $brcm_Workfile: bcm97115.h $
 * $brcm_Revision: Irvine_Diags_Devel/23 $
 * $brcm_Date: 11/11/02 5:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /SetTop/hardwareinterface/bcm97115/bcm97115.h $
 * 
 * Irvine_Diags_Devel/23   11/11/02 5:15p garylin
 * change comment to ansi compliance
 * 
 * Irvine_Diags_Devel/22   10/24/02 11:15a dlwin
 * Added code to support Annex A and Annex B selection for Inband at run-
 * time.
 * 
 * Irvine_Diags_Devel/21   10/16/02 10:44a dlwin
 * Used macros to define the default output video port and aspect ratio.
 * 
 * Irvine_Diags_Devel/20   9/19/02 4:16p dlwin
 * Added OB_IF_FREQ.
 * 
 * Irvine_Diags_Devel/19   8/28/02 6:2p dlwin
 * Added support for configuring Upstream mode of operation using run-time
 * configuration.
 * 
 * Irvine_Diags_Devel/18   8/27/02 2:26p garylin
 * change comment to ansi compliance
 * 
 * Irvine_Diags_Devel/17   8/21/02 10:43a dlwin
 * Added code to support OB for both Annex A and DVS-178 using run-time
 * configuration.
 * 
 * Irvine_Diags_Devel/16   8/19/02 5:26p dlwin
 * Added DRAM_SIZE define.
 * 
 * Irvine_Diags_Devel/15   8/6/02 2:6p brianlee
 * Added BOARD_ID_STR.
 * 
 * Irvine_Diags_Devel\14   7/8/02 11:6a dlwin
 * Changed to support configuring the Annex mode using defines in this
 * file.
 * 
 ***************************************************************************/
#ifndef __BCM97115_MAP_H
#define __BCM97115_MAP_H


#ifdef _cplusplus
extern "C" {
#endif

#define BOARD_ID_STR					"BCM97115"

/*************************************************************************/
/* MIPS Clock.                                                           */
/*************************************************************************/
#define CPU_CLOCK_RATE         			180000000		/* Hz */
#define XTALFREQ            			27000000
/*#define XTALFREQ1            			8333333 */ 
#define XTALFREQ1            			27000000

/*****************************************************************************/
/*                    Physical Memory Map                                    */
/*****************************************************************************/

#define PHYS_DRAM_BASE			0x00000000		/* Dynamic RAM Base */
#define PHYS_ROM_BASE			0x1FC00000		/* ROM */
#define PHYS_FLASH_BASE			0x1A000000		/* Flash Memory */
#define PHYS_FLASH1_BASE		PHYS_FLASH_BASE	/* Flash Memory */
#define PHYS_FLASH2_BASE		0x1B000000		/* Flash Memory */
#define PHYS_BCM44XX_BASE		0x1B800000		/* HPNA */
#define PHYS_SECURITY_BASE		0x1B900000		/* Security */
#define PHYS_BCM7040_BASE		0x1BA00000		/* 7040 */
#define PHYS_POD_BASE			0x1BB00000		/* pod */
#define PHYS_1394_BASE			0x1BC00000		/* 1394 */

#define PHYS_BOARD_REV      	0x1fa00000      /* Board Revision       */
#define PHYS_PLD_REV        	0x1fa00000      /* PLD Revision         */
#define PHYS_JUMPER_STAT    	0x1f800000      /* jumper setting */

/*****************************************************************************/
/* Note that the addresses above are physical addresses and that programs    */
/* have to use converted addresses defined below:                            */
/*****************************************************************************/
#define DRAM_BASE_CACHE		(0x80000000 | PHYS_DRAM_BASE)   /* cached DRAM */
#define DRAM_BASE_NOCACHE	(0xA0000000 | PHYS_DRAM_BASE)   /* uncached DRAM */
#define ROM_BASE_CACHE		(0x80000000 | PHYS_ROM_BASE)
#define ROM_BASE_NOCACHE	(0xA0000000 | PHYS_ROM_BASE)
#define FLASH_BASE			(0xA0000000 | PHYS_FLASH_BASE)
#define FLASH2_BASE			(0xA0000000 | PHYS_FLASH2_BASE)
#define FLASH1_BASE_NOCACHE FLASH_BASE
#define FLASH2_BASE_NOCACHE FLASH2_BASE
#define SECURITY_BASE		(0xA0000000 | PHYS_SECURITY_BASE)
#define BCM7040_BASE		(0xA0000000 | PHYS_BCM7040_BASE)
#define POD_BASE			(0xA0000000 | PHYS_POD_BASE)
#define IEEE1394_BASE		(0xA0000000 | PHYS_1394_BASE)
#define BCM44XX_BASE		(0xA0000000 | PHYS_BCM44XX_BASE)

#define BCM44XX_ENET_BASE	(BCM44XX_BASE + 0x1800)

#define	DRAM_SIZE				(64*1024*1024)		/* in bytes */

#define JUMPER_STAT_REV_NOCACHE			0xbf800000

/*************************************************************************/
/* symbol rates, QAM mode, and crystal frequency                         */
/*************************************************************************/
#define	ANNEXA_DS_IF_FREQ			36.125		/* Annex A Downstream IF Freq. in MHz */
#define	ANNEXB_DS_IF_FREQ			43.75		/* Annex B Downstream IF Freq. in MHz */
#define	OB_IF_FREQ					44.00		/* OB IF Freq. in MHz */

/************************************************************/
/* Configure Out-Of-Band mode of operation		 			*/
/*  OB Annex A, for Davic 									*/
/*  OB Annex B, for DVS-178									*/
/* To support Davic, define USE_OB_ANNEX_MODE_ONLY 			*/
/* To support both Davic and DVS-178 per build,				*/
/*  define INCLUDE_OB_ALL_MODES								*/
/* If INCLUDE_OB_ALL_MODES is defined, a run-time function	*/
/* is provided to configuration the OB script.  If both		*/
/* USE_OB_ANNEXA_MODE_ONLY and INCLUDE_OB_ALL_MODES are		*/
/* define then ANNEXA will be the bootup default OB mode	*/
/* but the OB mode may be changed using a function call.	*/
/* If INCLUDE_OB_ALL_MODES is defined, then the default		*/
/* OB mode will be DVS-178, but the OB mode may be changed	*/
/* using a function call.  If INCLUDE_OB_ALL_MODES is not	*/
/* defined, then no run-time configuration is available.	*/
/************************************************************/
/* #define USE_OB_ANNEXA_MODE_ONLY */
#define INCLUDE_OB_ALL_MODES

/************************************************************/
/* Configure In-Band mode of operation (downstream)			*/
/*  IB Annex A, for PAL, IB Davic is not support			*/
/*  IB Annex B, for NTSC									*/
/* To support PAL, define USE_IB_ANNEX_MODE_ONLY			*/
/* To support Annex A and Annex B per build,				*/
/*  define INCLUDE_IB_ALL_MODES								*/
/* If INCLUDE_IB_ALL_MODES is defined, then					*/
/*  USE_IB_ANNEXA_MODE_ONLY must be defined.  The reason is */
/*  that Annex B 6MHz channel can fit into an Annex A 8MHz	*/
/*  channel.  Annex A 8MHz channel can not fit into an		*/
/*  Annex B 6MHz channel.									*/
/*  NOTE: INCLUDE_IB_ALL_MODES will produce marginal		*/
/*  performance for Annex B applications.					*/
/************************************************************/
/* #define USE_IB_ANNEXA_MODE_ONLY */
/* #define INCLUDE_IB_ALL_MODES */
#if defined(INCLUDE_IB_ALL_MODES) && !defined(USE_IB_ANNEXA_MODE_ONLY)
#error To support Annex A and Annex B Inband, the HW must be an Annex A HW.
#endif

/************************************************************/
/* Configure Upstream mode of operation						*/
/*  US Davic, for Davic										*/
/*  US DVS-178, for DVS-178									*/
/* Note, only one of the following two define statements	*/
/* can be defined per build.								*/
/* For US Davic, define USE_US_DAVIC_MODE_ONLY				*/
/* For US DVS-178, define USE_US_DVS178_MODE_ONLY			*/
/* To support Davic and DVS-178 per build,					*/
/*  define INCLUDE_US_ALL_MODES								*/
/* If INCLUDE_US_ALL_MODES is defined, a run-time function	*/
/* is provided to configuration the US script.  If both		*/
/* USE_US_DAVIC_MODE_ONLY and INCLUDE_US_ALL_MODES are		*/
/* define then Davic will be the bootup default US mode		*/
/* but the US mode may be changed using a function call.	*/
/* If USE_US_DVS178_MODE_ONLY and INCLUDE_US_ALL_MODES are	*/
/* defined, then the default US mode will be DVS-178, but	*/
/* the US mode may be changed using a function call.		*/
/* If INCLUDE_US_ALL_MODES is not defined, then no run-time	*/
/* configuration is available.								*/
/************************************************************/
/* #define USE_US_DAVIC_MODE_ONLY */
#define USE_US_DVS178_MODE_ONLY
#define INCLUDE_US_ALL_MODES

#ifdef USE_IB_ANNEXA_MODE_ONLY
/* This is Annex A board */
#define	QAM_ANNEX_MODE				eAnnexA
#define DEFAULT_QAM64_SYM_RATE		6215000
#define DEFAULT_QAM256_SYM_RATE		6215000
#define	DS_IF_FREQ					ANNEXA_DS_IF_FREQ
#define XTAL_FREQ					28.0		/* Annex A Xtal Clk Freq. */
#define SYS_XTAL_FREQ				28.0		/* Annex A System Xtal Clk Freq. */
#else
/* This is Annex B board */
#define	QAM_ANNEX_MODE				eAnnexB
#define DEFAULT_QAM64_SYM_RATE		5056900
#define DEFAULT_QAM256_SYM_RATE		5360500
#define	DS_IF_FREQ					ANNEXB_DS_IF_FREQ
#define	XTAL_FREQ					25.0		/* Annex B Xtal Clk Freq. */
#define	SYS_XTAL_FREQ				25.0		/* Annex B System Xtal Clk Freq. */
#endif


/*************************************************************************/
/* Default Video Settings                                                */
/*************************************************************************/
#define DEFAULT_DISPLAY_OUTPUT			eSDOUTPUT_S_C		/* S-video and Composite */
#define	DEFAULT_DISPLAY_ASPECT_RATIO	eDISPLAY_AUTO		/* Auto mode */

/*************************************************************************/
/* 7115 register map                                                     */
/*************************************************************************/
#include "bcm7115.h"

/*************************************************************************/
/* Specific the default UART for startup 								 */
/*************************************************************************/
#define DEFAULT_STARTUP_UART_A			1			/* 1, use as default */
#define DEFAULT_STARTUP_UART_B			0			/* 1, use as default */

#ifdef _cplusplus
}
#endif


#endif /* __BCM97115_MAP_H */
