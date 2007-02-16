/*
 * arch/mips/brcmstb/brcm97038/board.c
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
 * when         who    what
 * ----         ---    ----
 * 03-31-2004   THT    Created
 */

#include <linux/config.h>
// For module exports
#include <linux/module.h>

#include <asm/brcmstb/common/brcmstb.h>
#include <asm/brcmstb/brcm97038b0/bchp_pci_cfg.h>

#ifndef DRAM_SIZE
#define DRAM_SIZE	(128 << 20)
#endif


#if 0

// Find out how the memory size is strapped on the board
static unsigned long
board_init_once2(void)
{
	int i;
	unsigned long regval;
	unsigned long memSize = 1<<4;
	
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W0)) = 0xffffffff;
	regval = *((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W0)) ;


	for (i=4; i<32; i++) {
		if (regval & memSize) {
			break;
		}
		memSize <<= 1;
	}
//printk("board_init_once: regval=%08x, shift=%d, memSize=%x\n", regval, i, memSize);	
	
	printk("Detected %d MB on board\n", (memSize >>20));


// THT: DO the same thing for Win1
	return memSize;

	/* Restore value */
}
#endif

#define SUN_TOP_CTRL_STRAP_VALUE 0xb0404018

#define STRAP_DDR_CONFIGURATION_SHIFT 	21
#define STRAP_DDR_CONFIGURATION_MASK  	0x00E00000


static unsigned long
board_init_once(void)
{
	int i;
	unsigned long regval;
	unsigned long memSize = 1<<4;
	unsigned long board_strap, ddr_mode_shift;
	
	regval = *((volatile unsigned long *) SUN_TOP_CTRL_STRAP_VALUE) ;

	/* Bit 21-23
 	 * 0 = four 4Mx16  =  32MB
	 * 1 = four 8Mx16  =  64MB
	 * 2 = four 16Mx16 = 128MB
	 * 3 = four 32Mx16 = 256MB
	 * 4 = two  4Mx16  =  16MB
	 * 5 = two  8Mx16  =  32MB
	 * 6 = two  16Mx16 =  64MB
	 * 7 = two  32Mx16 = 128MB
	 */

	board_strap = (regval & STRAP_DDR_CONFIGURATION_MASK) >> STRAP_DDR_CONFIGURATION_SHIFT;
printk("board_init_once: regval=%08x, strap_option=%x\n", regval, board_strap);

	switch (board_strap) {
	case 0:
		memSize = 32 << 20; 
		break;
	case 1:
		memSize = 64 << 20; 
		break;
	case 2:
		memSize = 128 << 20; 
		break;
	case 3:
		memSize = 256 << 20; 
		break;
	case 4:
		memSize = 16 << 20; 
		break;
	case 5:
		memSize = 32 << 20; 
		break;
	case 6:
		memSize = 64 << 20; 
		break;
	case 7:
		memSize = 128 << 20; 
		break;
	default:
		memSize = 0;
		printk("board_init_once: Invalid strapping option read %08x\n", regval);
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

	if (!once) {
		once++;
		dramSize = board_init_once();
		if (dramSize != DRAM_SIZE) {
			printk("Board strapped at %d MB, default is %d MB\n", (dramSize>>20), (DRAM_SIZE>>20));
		}
	}
    if (dramSize)
	    return dramSize;
    else
        return DRAM_SIZE;
}

EXPORT_SYMBOL(get_RAM_size);

