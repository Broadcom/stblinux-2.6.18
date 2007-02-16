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
 ***********************************************************************/

/************************************************************************/
/*                                                                     	*/
/*   MODULE:  bcm97317.h                                   				*/
/*   DATE:    Feb 05, 2004                                            */
/*   PURPOSE: Define addresses of major hardware components of          */
/*            BCM97317                                                  */
/*                                                                      */
/************************************************************************/
#ifndef __BCM97317_MAP_H
#define __BCM97317_MAP_H


#ifdef _cplusplus
extern "C" {
#endif

#define BOARD_ID_STR					"BCM97317"

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
#ifndef CODE_IN_FLASH
#define PHYS_FLASH1_BASE		0x1E000000		/* CS1 */
#define PHYS_FLASH2_BASE		0x1E800000		/* CS2 */
#else
#define PHYS_FLASH1_BASE		0x1F000000		/* CS1 */
#define PHYS_FLASH2_BASE		0x1F800000		/* CS0 */
#endif
#define PHYS_FLASH_BASE			PHYS_FLASH1_BASE /* Flash Memory */

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

#define	DRAM_SIZE			(32*1024*1024)		/* in bytes */

/*************************************************************************/
/* Default Video Settings                                                */
/*************************************************************************/
#define DEFAULT_DISPLAY_OUTPUT			eSDOUTPUT_S_C		/* S-video and Composite */
#define	DEFAULT_DISPLAY_ASPECT_RATIO	eDISPLAY_AUTO		/* Auto mode */

/*************************************************************************/
/* 7315 register map                                       */
/*************************************************************************/
#include "bcm7317.h"


#ifdef _cplusplus
}
#endif


#endif /* __BCM97315_MAP_H */
