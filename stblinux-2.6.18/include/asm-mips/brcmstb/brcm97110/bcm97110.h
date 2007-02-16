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
 *
 * $brcm_Workfile: bcm97110.h $
 * $brcm_Revision: Irvine_Diags_Devel/24 $
 * $brcm_Date: 1/9/03 4:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /SetTop/hardwareinterface/bcm97110/bcm97110.h $
 * 
 * Irvine_Diags_Devel/24   1/9/03 4:14p dlwin
 * Removed error message when build option.
 * 
 * Irvine_Diags_Devel/23   12/12/02 1:42p dlwin
 * Added better compiler error checking, as requested.
 * 
 * Irvine_Diags_Devel/22   11/19/02 1:1p garylin
 * remove compile warning
 * 
 * Irvine_Diags_Devel/21   11/18/02 9:49p garylin
 * change comment from // to ansi compliance
 * 
 * Irvine_Diags_Devel/20   11/8/02 9:52a dlwin
 * Changing the define for the location of Flash.
 * 
 * Irvine_Diags_Devel/19   11/1/02 3:43p dlwin
 * Set the default output display.
 * 
 * Irvine_Diags_Devel/18   10/24/02 5:39p dlwin
 * Added code to support Annex A and Annex B selection for Inband at run-
 * time.
 * 
 * Irvine_Diags_Devel/17   10/18/02 4:10p dlwin
 * Changed some comments.
 * 
 * Irvine_Diags_Devel/16   10/16/02 10:25a dlwin
 * Added a macro to define the type of display/video output port used for
 * this board.
 * 
 * Irvine_Diags_Devel/15   10/15/02 7:14p dlwin
 * Changed CS2 to 16MByte address space starting at 0x1C000000
 * 
 * Irvine_Diags_Devel/14   10/15/02 2:6p dlwin
 * Changed PCCARD to use 0x1B800000-0x1B9FFFFFF.
 * 
 * Irvine_Diags_Devel/13   9/25/02 3:40p dlwin
 * Corrected problem when building for Flash, jumped to invalid address.
 * 
 * Irvine_Diags_Devel/12   9/3/02 3:26p dlwin
 * Changed to have Flash device reside at address 0x1D000000.
 * 
 * Irvine_Diags_Devel/11   8/28/02 6:20p dlwin
 * Updated to support Flash boot.
 * 
 * Irvine_Diags_Devel/10   8/21/02 11:27a dlwin
 * Added OB support for both Annex A and DVS-178, allowing for
 * configuration using run-time options.
 * 
 * Irvine_Diags_Devel/9   8/19/02 5:34p dlwin
 * Added DRAM_SIZE definition.
 * 
 * Irvine_Diags_Devel/8   8/6/02 2:7p brianlee
 * Added BOARD_ID_STR.
 * 
 * Irvine_Diags_Devel/7   7/17/02 6:2p dlwin
 * Changed 7040 to 7041.
 * 
 * Irvine_Diags_Devel/6   7/16/02 4:4p dlwin
 * Changed speed for CPU to 220MHz.
 * 
 * Irvine_Diags_Devel\5   7/10/02 4:44p dlwin
 * Made OB default to be DVS178
 * 
 * Irvine_Diags_Devel\4   7/2/02 2:37p dlwin
 * Changed to allow the selection of Upstream mode of operation using a
 * define statement in 'bcm97110.h' similar to the selection for
 * Downstream and Out-Of-Band.
 * 
 * Irvine_Diags_Devel\3   6/28/02 6:29p dlwin
 * Added code support Annex A and Annex B for Qam downstream scripts.
 * 
 * Irvine_Diags_Devel\2   6/11/02 8:52p dlwin
 * Change to export USE_ANNEXA_MODE.
 * 
 * Irvine_Diags_Devel\1   5/31/02 5:20p dlwin
 * Initial version for bcm97110.
 * 
 ***************************************************************************/
#ifndef __BCM97110_MAP_H
#define __BCM97110_MAP_H


#ifdef _cplusplus
extern "C" {
#endif

#define BOARD_ID_STR					"BCM97110"

/*************************************************************************/
/* MIPS Clock.                                                           */
/*************************************************************************/
#define CPU_CLOCK_RATE         			220000000		/* Hz */
#define XTALFREQ            			27000000
#define XTALFREQ1            			27000000

/*****************************************************************************/
/*                    Physical Memory Map                                    */
/*****************************************************************************/
/* NOTE: modification of the address will require changing 'io_cs_init.s'	 */
/* The following is for ROM/FLASH base Bootup								 */
#ifdef CODE_IN_FLASH
#define PHYS_ROM_BASE			0x1F000000		/* CS1, Size=1MB */
#define PHYS_FLASH_BASE			0x1F000000		/* CS0, Size=16MB, CS1 is swapped with CS0 */
#elif defined(CODE_IN_ROM)
#define PHYS_ROM_BASE			0x1FC00000		/* CS0, Size=1MB */
#define PHYS_FLASH_BASE			0x1D000000		/* CS1, Size=16MB */
#elif defined(CODE_IN_SDRAM)
/* 
*  This assumes that CODE_IN_SDRAM are intended for targets where a 
*  bootloader is present and the bootloader is in Flash.  If the bootloader
*  is in Rom, then CODE_IN_ROM values should be used.
*/
#define PHYS_ROM_BASE			0x1F000000		/* CS1, Size=1MB */
#define PHYS_FLASH_BASE			0x1F000000		/* CS0, Size=16MB */
#else
/*
* Assume this is RAM build, CODE_IN_SDRAM
*/
#define PHYS_ROM_BASE			0x1F000000		/* CS1, Size=1MB */
#define PHYS_FLASH_BASE			0x1F000000		/* CS0, Size=16MB */
#endif
#define PHYS_DRAM_BASE			0x00000000		/* Size=64MB */
#define	PHYS_PCCARD_BASE		0x1C000000		/* CS2, Size=16MB */
#define	PHYS_TESTFPGA_BASE		0x1B100000		/* CS3, Size=1MB */
/* Keep POD1 and POD2 together */
#define	PHYS_POD1_BASE			0x1B200000		/* CS4, Size=1MB */
#define	PHYS_POD2_BASE			0x1B300000		/* CS6, Size=1MB */
#define	PHYS_POD_BASE			PHYS_POD1_BASE
#define	PHYS_BCM7041_BASE		0x1B400000		/* CS5, Size=64KB */
#define	PHYS_1394_BASE			0x1B500000		/* CS7, Size=1MB */
#define	PHYS_BCM44XX_BASE		0x1B600000		/* CS8, Size=8KB */

/*****************************************************************************/
/* Note that the addresses above are physical addresses and that programs    */
/* have to use converted addresses defined below:                            */
/*****************************************************************************/
#define DRAM_BASE_CACHE		(0x80000000 | PHYS_DRAM_BASE)   /* cached DRAM */
#define DRAM_BASE_NOCACHE	(0xA0000000 | PHYS_DRAM_BASE)   /* uncached DRAM */
#define ROM_BASE_CACHE		(0x80000000 | PHYS_ROM_BASE)
#define ROM_BASE_NOCACHE	(0xA0000000 | PHYS_ROM_BASE)
#define FLASH_BASE			(0xA0000000 | PHYS_FLASH_BASE)
#define FLASH_BASE_NOCACHE	(0xA0000000 | PHYS_FLASH_BASE)
#define FLASH_BASE_CACHE	(0x80000000 | PHYS_FLASH_BASE)
#define BOOT_CODE_BASE_CACHE (0x80000000 | 0x1FC00000)
#define BOOT_CODE_BASE_NOCACHE (0xA0000000 | 0x1FC00000)

#define PCCARD_BASE			(0xA0000000 | PHYS_PCCARD_BASE)
#define TESTFPGA_BASE		(0xA0000000 | PHYS_TESTFPGA_BASE)
#define POD_BASE			(0xA0000000 | PHYS_POD1_BASE)
#define POD1_BASE			(0xA0000000 | PHYS_POD1_BASE)
#define POD2_BASE			(0xA0000000 | PHYS_POD2_BASE)

#define BCM7041_BASE		(0xA0000000 | PHYS_BCM7041_BASE)
#define IEEE1394_BASE		(0xA0000000 | PHYS_1394_BASE)
#define BCM44XX_BASE		(0xA0000000 | PHYS_BCM44XX_BASE)

#define BCM44XX_ENET_BASE	(BCM44XX_BASE + 0x1800)

#define	DRAM_SIZE				(64*1024*1024)		/* in bytes */

/*************************************************************************/
/* symbol rates, QAM mode, and crystal frequency                         */
/*************************************************************************/
#define	ANNEXA_DS_IF_FREQ			36.125		/* Annex A Downstream IF Freq. */
#define	ANNEXB_DS_IF_FREQ			43.75		/* Annex B Downstream IF Freq. */
#define	OB_IF_FREQ					44.00		/* OB IF Freq in MHz */

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
/*  IB Annex A, for PAL/EuroDocsis, IB Davic is	not support	*/
/*  IB Annex B, for NTSC/Docsis								*/
/* To support PAL/EuroDocsis, define USE_IB_ANNEX_MODE_ONLY	*/
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
/* define USE_IB_ANNEXA_MODE_ONLY */
/* #define INCLUDE_IB_ALL_MODES */
#if defined(INCLUDE_IB_ALL_MODES) && !defined(USE_IB_ANNEXA_MODE_ONLY)
#error To support Annex A and Annex B Inband, the HW must be an Annex A HW.
#endif

/************************************************************/
/* Configure Upstream mode of operation						*/
/*  US Davic, for Davic										*/
/*  US DVS-178, for DVS-178									*/
/*  US Docsis, for Docsis									*/
/* Note, only one of the following three define statements	*/
/* can be defined per build.								*/
/* For US Davic, define USE_US_DAVIC_MODE_ONLY				*/
/* For US DVS-178, define USE_US_DVS178_MODE_ONLY			*/
/* For US Docsis, define USE_US_DOCSIS_MODE_ONLY			*/
/* To support Davic, DVS-178, and Docsis per build,			*/
/*  define INCLUDE_US_ALL_MODES								*/
/* If INCLUDE_US_ALL_MODES is defined, a run-time function	*/
/* is provided to configuration the US script.  If both		*/
/* USE_US_DAVIC_MODE_ONLY and INCLUDE_US_ALL_MODES are		*/
/* define then Davic will be the bootup default US mode		*/
/* but the US mode may be changed using a function call.	*/
/* If USE_US_DVS178_MODE_ONLY and INCLUDE_US_ALL_MODES are	*/
/* defined, then the default US mode will be DVS-178, but	*/
/* the US mode may be changed using a function call.		*/
/* If USE_US_DOCSIS_MODE_ONLY and INCLUDE_US_ALL_MODES are	*/
/* defined, then the default US mode will be Docis, but		*/
/* the US mode may be changed using a function call.		*/
/* If INCLUDE_US_ALL_MODES is not defined, then no run-time	*/
/* configuration is available.								*/
/************************************************************/
/* #define USE_US_DAVIC_MODE_ONLY */
/* #define USE_US_DVS178_MODE_ONLY */
#define USE_US_DOCSIS_MODE_ONLY
#define INCLUDE_US_ALL_MODES

#ifdef USE_IB_ANNEXA_MODE_ONLY
/* This is Annex A board */
#define	QAM_ANNEX_MODE				eAnnexA
#define DEFAULT_QAM64_SYM_RATE		6215000
#define DEFAULT_QAM256_SYM_RATE		6215000
#define	DS_IF_FREQ					ANNEXA_DS_IF_FREQ
#define XTAL_FREQ					28.0		/* Annex A Xtal Clk Freq. */
#define SYS_XTAL_FREQ				28.0		/* Annex A System Xtal Clk Freq. */
#define XTAL_FREQ_3125				28.0		/* Annex A Xtal Clk Freq. */
#define SYS_XTAL_FREQ_3125			28.0		/* Annex A System Xtal Clk Freq. */
#else
/* This is Annex B board */
#define	QAM_ANNEX_MODE				eAnnexB
#define DEFAULT_QAM64_SYM_RATE		5056900
#define DEFAULT_QAM256_SYM_RATE		5360500
#define	DS_IF_FREQ					ANNEXB_DS_IF_FREQ
#define	XTAL_FREQ					25.0		/* Annex B Xtal Clk Freq. */
#define	SYS_XTAL_FREQ				25.0		/* Annex B System Xtal Clk Freq. */
#define XTAL_FREQ_3125				25.0		/* Annex B Xtal Clk Freq. */
#define SYS_XTAL_FREQ_3125			25.0		/* Annex B System Xtal Clk Freq. */
#endif

/*************************************************************************/
/* Default Video Settings                                                */
/*************************************************************************/
#define DEFAULT_DISPLAY_OUTPUT			eSDOUTPUT_YPRPB_ONLY /* S-video, Composite, and RGB driven */
															/* although enum shows only YPRPB support */
#define	DEFAULT_DISPLAY_ASPECT_RATIO	eDISPLAY_AUTO		/* Auto mode */



/*************************************************************************/
/* 7110 register map                         				             */
/*************************************************************************/
#include "bcm7110.h"


#ifdef _cplusplus
}
#endif


#endif /* __BCM97110_MAP_H */
