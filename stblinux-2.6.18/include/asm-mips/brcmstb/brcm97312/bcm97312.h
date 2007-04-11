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


/************************************************************************/
/*                                                                     	*/
/*   MODULE:  bcm97312.h                                   				*/
/*   DATE:    August 4, 2004                                            */
/*   PURPOSE: Define addresses of major hardware components of          */
/*            BCM97312                                                  */
/*                                                                      */
/************************************************************************/
#ifndef __BCM97312_MAP_H
#define __BCM97312_MAP_H


#ifdef _cplusplus
extern "C" {
#endif

#define BOARD_ID_STR					"BCM97312"

/*************************************************************************/
/* MIPS Clock.                                                           */
/*************************************************************************/
#define CPU_CLOCK_RATE         			176000000		/* Hz */
#define XTALFREQ            			27000000

#ifdef IKOS
#define XTALFREQ1            			8333333
#else
#define XTALFREQ1            			27000000
#endif

/*****************************************************************************/
/*                    Physical Memory Map                                    */
/*****************************************************************************/

#define PHYS_DRAM_BASE			0x00000000		/* Dynamic RAM Base */
#define PHYS_ROM_BASE			0x1FC00000		/* ROM */
#define PHYS_CS0_DEV_BASE		0x1F000000		/* CS0 device, ROM or flash */
#define PHYS_CS1_DEV_BASE		0x1E000000		/* CS1 device, ROM or flash */

#define PHYS_FLASH_BASE			PHYS_CS1_DEV_BASE		/* CS1 */

/*****************************************************************************/
/* Note that the addresses above are physical addresses and that programs    */
/* have to use converted addresses defined below:                            */
/*****************************************************************************/
#define DRAM_BASE_CACHE		(0x80000000 | PHYS_DRAM_BASE)   /* cached DRAM */
#define DRAM_BASE_NOCACHE	(0xA0000000 | PHYS_DRAM_BASE)   /* uncached DRAM */
#define ROM_BASE_CACHE		(0x80000000 | PHYS_ROM_BASE)
#define ROM_BASE_NOCACHE	(0xA0000000 | PHYS_ROM_BASE)
#define FLASH_BASE_NOCACHE  (0xA0000000 | PHYS_FLASH_BASE)

#define	DRAM_SIZE			(32*1024*1024)		/* in bytes */

/*************************************************************************/
/* Default Video Settings                                                */
/*************************************************************************/
#define DEFAULT_DISPLAY_OUTPUT			eSDOUTPUT_S_C		/* S-video and Composite */
#define	DEFAULT_DISPLAY_ASPECT_RATIO	eDISPLAY_AUTO		/* Auto mode */

/*************************************************************************/
/* Diseqc Mode Setting                                                   */
/*************************************************************************/
#define DEFAULT_DISEQC_MODE				3

/*************************************************************************/
/* 7312 register map                                       */
/*************************************************************************/
#include "bcm7312.h"


#ifdef _cplusplus
}
#endif


#endif /* __BCM97312_MAP_H */
