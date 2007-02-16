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
/*   MODULE:  bcm97315bbx.h                                   				*/
/*   DATE:    April 13, 2002                                            */
/*   PURPOSE: Define addresses of major hardware components of          */
/*            BCM97315BBX                                                  */
/*                                                                      */
/************************************************************************/
#ifndef __BCM97315BBX_MAP_H
#define __BCM97315BBX_MAP_H


#ifdef _cplusplus
extern "C" {
#endif



#include <asm/brcmstb/brcm97315/bcm97315.h>

#undef BOARD_ID_STR
#define BOARD_ID_STR					"BCM97315BBX"

/*****************************************************************************/
/*                    Physical Memory Map                                    */

/*****************************************************************************/

#undef PHYS_FLASH1_BASE
#define PHYS_FLASH1_BASE 0x1FC00000
#undef PHYS_FLASH_BASE
#define PHYS_FLASH_BASE PHYS_FLASH1_BASE

#define PHYS_BCM44XX_BASE		0x1B800000		/* HPNA */

/*****************************************************************************/
/* Note that the addresses above are physical addresses and that programs    */
/* have to use converted addresses defined below:                            */
/*****************************************************************************/

#undef FLASH_BASE
#define FLASH_BASE			(0xA0000000 | PHYS_FLASH_BASE)

#undef FLASH1_BASE_NOCACHE
#define FLASH1_BASE_NOCACHE FLASH_BASE

#define BCM44XX_BASE		(0xA0000000 | PHYS_BCM44XX_BASE)
#define BCM44XX_ENET_BASE	(BCM44XX_BASE + 0x1800)



#ifdef _cplusplus
}
#endif


#endif /* __BCM97315BBX_MAP_H */
