/*
 * arch/mips/brcmstb/brcm97118a0/board.c
 *
 * Copyright (C) 2004-2005 Broadcom Corporation
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
 */

#include <linux/config.h>
// For module exports
#include <linux/module.h>

#include <asm/brcmstb/common/brcmstb.h>
#include <asm/brcmstb/brcm97118a0/bchp_pci_cfg.h>


#ifndef DRAM_SIZE
#define DRAM_SIZE	(256 << 20)
#endif

#define MBYTES		(1<<20)
#define NUM_DDR 1

#define SUN_TOP_CTRL_STRAP_VALUE 0xb040401c

#define STRAP_PCI_MEMWIN_SIZE_SHIFT 7
#define STRAP_PCI_MEMWIN_SIZE_MASK 0x00000180	/* Bit 7 & 8 */

#define STRAP_DDR_CONFIGURATION_SHIFT 	13
#define STRAP_DDR_CONFIGURATION_MASK  	0x00006000  /* Bits 13-14 */


static unsigned long
board_init_once(void)
{
	int i;
	unsigned long regval;
	unsigned long memSize = 1<<4;
	unsigned long board_strap, ddr_mode_shift;
	unsigned long pci_memwin_size;
	
	regval = *((volatile unsigned long *) SUN_TOP_CTRL_STRAP_VALUE) ;

	/*
	 * Bit 14:13:	0 = 64Mx16bit part
	 *         		1 = 8Mx16bit part
	 *				2 = 16Mx16bit part
	 *				3 = 32Mx16bit part
	 */

	board_strap = (regval & STRAP_DDR_CONFIGURATION_MASK) >> STRAP_DDR_CONFIGURATION_SHIFT;
	pci_memwin_size = (regval & STRAP_PCI_MEMWIN_SIZE_MASK) >> STRAP_PCI_MEMWIN_SIZE_SHIFT;
	printk("board_init_once: regval=%08x, ddr_strap=%x, %d chips, pci_size=%x\n", regval, board_strap, NUM_DDR, pci_memwin_size);

	switch (board_strap) {
	case 0:
		memSize = NUM_DDR*256*MBYTES;
		break;
	case 1:
		memSize = NUM_DDR*32*MBYTES;
		break;
	case 2:
		memSize = NUM_DDR*64*MBYTES;
		break;
	case 3:
		memSize = NUM_DDR*128*MBYTES;
		break;
	}
	
	printk("Detected %d MB on board\n", (memSize >>20));
	
	return memSize;
	/* Restore value */
}


unsigned long
get_RAM_size(void)
{
	static int once;
	static unsigned long dramSize = 0;

#ifndef CONFIG_MIPS_BRCM_IKOS
	if (!once) {
		once++;
		dramSize = board_init_once();
		if (dramSize != DRAM_SIZE) {
			printk("Board strapped at %d MB, default is %d MB\n", (dramSize>>20), (DRAM_SIZE>>20));
		}
	}
#endif
    if (dramSize)
	    return dramSize;
    else
        return DRAM_SIZE;
}

EXPORT_SYMBOL(get_RAM_size);
