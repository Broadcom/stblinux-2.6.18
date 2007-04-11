/*
 * Flash Base address for BCM97xxx boards
 *
  * Copyright (C) 2002,2003,2004,2005 Broadcom Corporation
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
 * THT 10/22/2002
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/config.h>

#ifdef CODE_IN_FLASH
#undef CODE_IN_FLASH
#endif

#ifndef CODE_IN_ROM
#define CODE_IN_ROM
#endif

#ifdef CONFIG_MIPS_BCM3560
#include <asm/brcmstb/common/brcmstb.h>
#define WINDOW_ADDR CPU_PHYS_FLASH_BASE

#elif defined( CONFIG_MIPS_BCM7319 )
#include <asm/brcmstb/brcm97319/bcm97319.h>
#define WINDOW_ADDR PHYS_FLASH_BASE

#elif defined( CONFIG_MIPS_BCM7328 )
#include <asm/brcmstb/brcm97328/bcm97328.h>
#define WINDOW_ADDR PHYS_FLASH_BASE

#elif defined(CONFIG_MIPS_BCM7320)
#include <asm/brcmstb/brcm97320/bcm97320.h>
/* Assume ROM daughter card present, 
 * will play with CS0 when it is not (boot from flash) 
 */
#define WINDOW_ADDR PHYS_FLASH2_CS0ROM_BASE	/*0x1e800000*/

#elif defined(CONFIG_MIPS_BCM7115)
#include <asm/brcmstb/brcm97115/bcm97115.h>
#define WINDOW_ADDR PHYS_FLASH_BASE			/*0x1a000000*/

#elif defined(CONFIG_MIPS_BCM7112)
#include <asm/brcmstb/brcm97112/bcm97112.h>
#define WINDOW_ADDR PHYS_FLASH_BASE			/*0x1a000000*/


#elif defined(CONFIG_MIPS_BCM7110)
#include <asm/brcmstb/brcm97110/bcm97110.h>
#define WINDOW_ADDR PHYS_FLASH_BASE			/*0x1d000000*/

#elif defined(CONFIG_MIPS_BCM7111)
#include <asm/brcmstb/brcm97111/bcm97111.h>
#define WINDOW_ADDR PHYS_FLASH_BASE			/*0x1d000000*/

#elif defined(CONFIG_MIPS_BCM7118)
#include <asm/brcmstb/common/brcmstb.h>
#define WINDOW_ADDR CPU_PHYS_FLASH_BASE

#elif defined(CONFIG_MIPS_BCM7312)
#include <asm/brcmstb/brcm97312/bcm97312.h>
#define WINDOW_ADDR PHYS_FLASH_BASE			/* 0x1e000000 */

#elif defined(CONFIG_MIPS_BCM7314)
#include <asm/brcmstb/brcm97314/bcm97314.h>
#define WINDOW_ADDR PHYS_FLASH1_BASE


#elif defined(CONFIG_MIPS_BCM7315)
#include <asm/brcmstb/brcm97315/bcm97315.h>
#define WINDOW_ADDR PHYS_FLASH1_BASE

#elif defined(CONFIG_MIPS_BCM7317)
#include <asm/brcmstb/brcm97317/bcm97317.h>
#define WINDOW_ADDR PHYS_FLASH1_BASE

#elif defined(CONFIG_MIPS_BCM7318)
#include <asm/brcmstb/brcm97318/bcm97318.h>
#define WINDOW_ADDR PHYS_FLASH_BASE			/* 0x1e000000 */

#elif defined(CONFIG_MIPS_BCM7038)
#include <asm/brcmstb/common/brcmstb.h>
#define WINDOW_ADDR CPU_PHYS_FLASH_BASE

#elif defined(CONFIG_MIPS_BCM7327)
#include <asm/brcmstb/common/brcmstb.h>
#define WINDOW_ADDR PHYS_FLASH_BASE

#elif defined(CONFIG_MIPS_BCM7329)
#include <asm/brcmstb/common/brcmstb.h>
#define WINDOW_ADDR PHYS_FLASH_BASE

#elif defined(CONFIG_MIPS_BCM7400)
/* Both A0 and B0 revs */
#include <asm/brcmstb/common/brcmstb.h>
#define WINDOW_ADDR CPU_PHYS_FLASH_BASE

#elif defined(CONFIG_MIPS_BCM7401) || defined(CONFIG_MIPS_BCM7402) || \
      defined(CONFIG_MIPS_BCM7403)
/* Both A0 and B0 revs */
#include <asm/brcmstb/common/brcmstb.h>
#define WINDOW_ADDR CPU_PHYS_FLASH_BASE

#elif defined(CONFIG_MIPS_BCM7440)
#include <asm/brcmstb/common/brcmstb.h>
#define WINDOW_ADDR CPU_PHYS_FLASH_BASE

#endif

const unsigned long ROM_FLASH_BASE = WINDOW_ADDR;
