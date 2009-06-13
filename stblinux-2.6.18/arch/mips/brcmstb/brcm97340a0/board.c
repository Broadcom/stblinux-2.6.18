/*
 * arch/mips/brcmstb/brcm9xxxxyy/board.c
 *
 * Copyright (C) 2004-2007 Broadcom Corporation
 *
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
 * Board Specific routines for Broadcom eval boards
 *
 * when         who    what
 * ----         ---    ----
 * 03-31-2004   THT    Created
 * 09-21-2006	TDT	   Added to 2.6.18 kernel
 */

#include <linux/config.h>
// For module exports
#include <linux/module.h>

#include <asm/brcmstb/common/brcmstb.h>

#ifdef CONFIG_MIPS_BRCM_IKOS

#ifdef DRAM_SIZE
#undef DRAM_SIZE
#endif

// For Ikos
#define DRAM_SIZE	(32 << 20)
#else

/*
 * Smallest supported DRAM chip size in MB. e.g. if RDB says:
 *
 * 00: 16Mx16 bit device on MEMC0
 *
 * Then this should be 32MB (aka 256Mb)
 */
#define SMALLEST_MODULE	32

#define STRAP_PCI_MEMWIN_SIZE_SHIFT \
	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_pci_memwin_size_SHIFT
#define STRAP_PCI_MEMWIN_SIZE_MASK \
	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_pci_memwin_size_MASK


#ifndef BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_ddr0_device_config_SHIFT

/* traditional UMA configuration */

#define NUM_DDR 4

#define STRAP_DDR_CONFIGURATION_SHIFT \
	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_ddr_configuration_SHIFT
#define STRAP_DDR_CONFIGURATION_MASK \
	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_ddr_configuration_MASK

#else

/* 7405, 7335, 7336 and other chips with flexible DDR configurations */

#undef NUM_DDR

#define STRAP_DDR_SIZE_SHIFT \
	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_ddr0_device_config_SHIFT
#define STRAP_DDR_SIZE_MASK \
	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_ddr0_device_config_MASK

#define STRAP_DDR_CONFIGURATION_SHIFT \
	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_ddr_configuration_SHIFT
#define STRAP_DDR_CONFIGURATION_MASK \
	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_ddr_configuration_MASK

#endif

static unsigned long
board_init_once(void)
{
 	unsigned long regval;
	unsigned long memSize = 1<<4;
	unsigned long board_strap;
	unsigned long pci_memwin_size;
	
	regval = BDEV_RD(BCHP_SUN_TOP_CTRL_STRAP_VALUE_0);

	pci_memwin_size = (regval & STRAP_PCI_MEMWIN_SIZE_MASK) >>
		STRAP_PCI_MEMWIN_SIZE_SHIFT;
#ifdef NUM_DDR
	board_strap = (regval & STRAP_DDR_CONFIGURATION_MASK) >>
		STRAP_DDR_CONFIGURATION_SHIFT;
	memSize = (NUM_DDR * SMALLEST_MODULE << (board_strap & 3)) << 20;
#else
	board_strap = (regval & STRAP_DDR_SIZE_MASK) >>
		STRAP_DDR_SIZE_SHIFT;
	memSize = (SMALLEST_MODULE << (board_strap & 3)) << 20;

	board_strap = (regval & STRAP_DDR_CONFIGURATION_MASK) >>
		STRAP_DDR_CONFIGURATION_SHIFT;
	switch(board_strap) {
		case 0:
			memSize *= 4;	/* UMA64 */
			break;
		case 1:
			memSize *= 2;	/* UMA32 */
			break;
		case 2:
			memSize *= 1;	/* UMA16 */
			break;
		case 4:
			memSize *= 2;	/* 32 + 16 non-UMA */
			break;
		case 5:
			memSize *= 1;	/* 16 + 16 non-UMA */
			break;
		default:
			printk(KERN_WARNING "Unknown memory configuration %ld\n",
				board_strap);
	}
#endif
	
	printk("Detected %ld MB on MEMC0 (strap 0x%08lx)\n",
		(memSize >>20), regval);
	return memSize;
}

#endif //IKOS

#ifdef CONFIG_MIPS_BRCM_IKOS
unsigned long
get_RAM_size(void)
{
	return DRAM_SIZE;
}
#else
unsigned long
get_RAM_size(void)
{
	static int once;
	static unsigned long dramSize = 0;

	if (!once) {
		once++;
		dramSize = board_init_once();
		if (dramSize != DRAM_SIZE) {
			printk("Board strapped at %ld MB, default is %d MB\n",
				(dramSize>>20),
				(DRAM_SIZE>>20));
		}
#ifndef CONFIG_DISCONTIGMEM
		/* PR23504: without discontig support, we can only access 256MB */
		if (dramSize > 0x10000000) {
			dramSize = 0x10000000;
			printk("Extra RAM beyond 256MB ignored.  Please "
				"use a kernel that supports DISCONTIG.\n");
		}
#endif
	}
	if (dramSize)
		return dramSize;
	else
		return DRAM_SIZE;
}

#endif 
EXPORT_SYMBOL(get_RAM_size);
