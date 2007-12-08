/*
 * arch/mips/brcmstb/brcm93563/board.c
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
#include <asm/brcmstb/brcm93563c0/bchp_pci_cfg.h>

#ifndef DRAM_SIZE
#define DRAM_SIZE	(128 << 20)
#endif

#define MBYTES		(1<<20)
#define NUM_DDR_0	4 
#define NUM_DDR_1	2

#define SUN_TOP_CTRL_STRAP_VALUE_0	0xb040401c
#define SUN_TOP_CTRL_STRAP_VALUE_1	0xb0404020


#define STRAP_PCI_MEMWIN_SIZE_SHIFT	7
#define STRAP_PCI_MEMWIN_SIZE_MASK 	0x00000180	/* Bit 7 & 8 */

#define STRAP_DDR_0_CONFIGURATION_SHIFT	13
#define STRAP_DDR_0_CONFIGURATION_MASK	0x0000e000	/* Bits 13-15 */

#define STRAP_DDR_1_CONFIGURATION_SHIFT 0
#define STRAP_DDR_1_CONFIGURATION_MASK  0x00000003      /* Bits 0-1 */



static unsigned long
board_init_once(void)
{
	int i;
	unsigned long regval;
	unsigned long memSize0, memSize1;
	unsigned long board_strap, ddr_mode_shift;
	unsigned long pci_memwin_size;
	
	regval = *((volatile unsigned long *) SUN_TOP_CTRL_STRAP_VALUE_0) ;

	/*
	 * Bit 15:13:	0 = reserved 
	 *         	1 = 8Mx16bit part
	 *		2 = 16Mx16bit part
	 *		3 = 32Mx16bit part
	 */

	board_strap = (regval & STRAP_DDR_0_CONFIGURATION_MASK) >> 
	                              STRAP_DDR_0_CONFIGURATION_SHIFT;
	pci_memwin_size = (regval & STRAP_PCI_MEMWIN_SIZE_MASK) >> 
	                              STRAP_PCI_MEMWIN_SIZE_SHIFT;
	printk("board_init_once: regval=%08x, ddr_0_strap=%x, %d chips, pci_size=%x\n", regval, board_strap, NUM_DDR_0, pci_memwin_size);

	switch (board_strap) 
	{
		case 1:
			memSize0 = NUM_DDR_0*16*MBYTES;
			break;
		case 2:
			memSize0 = NUM_DDR_0*32*MBYTES;
			break;
		case 3:
			memSize0 = NUM_DDR_0*64*MBYTES;
			break;
		default:
			/* Should never get here */
       			memSize0 = 0;
                	printk("!!!board_init_once: Invalid strapping option read %08x\n", regval);
			break;
	}

	regval = *((volatile unsigned long *) SUN_TOP_CTRL_STRAP_VALUE_1) ;

	/*
	 * Bit 01:00:	0 = not populated 
	 *         	1 = 8Mx16bit part
	 *		2 = 16Mx16bit part
	 *		3 = 32Mx16bit part
	 */

	board_strap = (regval & STRAP_DDR_1_CONFIGURATION_MASK) >> 
	                              STRAP_DDR_1_CONFIGURATION_SHIFT;
	printk("board_init_once: regval=%08x, ddr_1_strap=%x, %d chips\n", regval, board_strap, NUM_DDR_1);

	switch (board_strap) 
	{
		case 0:
			memSize1 = 0;
			break;
		case 1:
			memSize1 = NUM_DDR_1*16*MBYTES;
			break;
		case 2:
			memSize1 = NUM_DDR_1*32*MBYTES;
			break;
		case 3:
			memSize1 = NUM_DDR_1*64*MBYTES;
			break;
		default:
			/* Should never get here */
       			memSize1 = 0;
                	printk("!!!board_init_once: Invalid strapping option read %08x\n", regval);
			break;
	}
	printk("Detected %d MB on board. DDR0 %dMB DDR1 %dMB\n", 
	         ((memSize0+memSize1) >>20), memSize0 >> 20, memSize1 >> 20);
	
	return (memSize0);
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
