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

#ifndef BCM97328_H
#define BCM97328_H

#ifndef LANGUAGE_ASSEMBLY
#ifdef _cplusplus
extern "C" {
#endif
#endif

#define BOARD_ID_STR					"BCM97328"

/*************************************************************************/
/* MIPS Clock.                                                           */
/*************************************************************************/
#define CPU_CLOCK_RATE         			248400000		/* Hz */
#define XTALFREQ            			27000000
#define XTALFREQ1						27000000


/*****************************************************************************/
/*                    Physical Memory Map                                    */
/*****************************************************************************/
#define PHYS_SDRAM_BASE		0x00000000	/* SDRAM Base */
#define PHYS_ROM_BASE		0x1FC00000	/* ROM */
#define PHYS_FLASH_BASE     0x1C000000
#define PHYS_BCM44XX_BASE   0x1B000000  /* BCM4413 */
#define DRAM_SIZE (32*1024*1024)


/*****************************************************************************/
/* Note that the addresses above are physical addresses and that programs    */
/* have to use converted addresses defined below:                            */
/*****************************************************************************/
#define DRAM_BASE_CACHE		(0x80000000 | PHYS_SDRAM_BASE)   /* cached DRAM */
#define DRAM_BASE_NOCACHE	(0xA0000000 | PHYS_SDRAM_BASE)   /* uncached DRAM */
#define ROM_BASE_CACHE		(0x80000000 | PHYS_ROM_BASE)
#define ROM_BASE_NOCACHE	(0xA0000000 | PHYS_ROM_BASE)
#define FLASH_BASE_NOCACHE  (0xA0000000 | PHYS_FLASH_BASE)
#define BCM44XX_BASE		(0xA0000000 | PHYS_BCM44XX_BASE)

#define BCM44XX_ENET_BASE	(BCM44XX_BASE + 0x1800)

/* 7230 register map */
#include "bcm7328.h"

#ifndef LANGUAGE_ASSEMBLY
#ifdef _cplusplus
}
#endif
#endif

#endif
