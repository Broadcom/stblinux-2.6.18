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


#ifndef DRAM_SIZE
#define DRAM_SIZE	(256 << 20)
#endif

#define NUM_DDR 4


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



#define SUN_TOP_CTRL_STRAP_VALUE 0xb040401c

#define STRAP_PCI_MEMWIN_SIZE_SHIFT 7
#define STRAP_PCI_MEMWIN_SIZE_MASK 0x00000180	/* Bit 7 & 8 */

#define STRAP_DDR_CONFIGURATION_SHIFT 	13
#define STRAP_DDR_CONFIGURATION_MASK  	0x0000E000  /* Bits 13-15 */


static unsigned long
board_init_once(void)
{
	unsigned long regval;
	unsigned long memSize = 1<<4;
	unsigned long board_strap, ddr_mode_shift;
	unsigned long pci_memwin_size;
	
	regval = *((volatile unsigned long *) SUN_TOP_CTRL_STRAP_VALUE) ;

	/* Bit 15: 		0 = 64 bit DDR mode
	 *       		1 = 32 bit DDR mode
	 * Bit 14:13:	0 = 64Mx16bit part
	 *         		1 = 8Mx16bit part
	 *				2 = 16Mx16bit part
	 *				3 = 32Mx16bit part
	 */

	board_strap = (regval & STRAP_DDR_CONFIGURATION_MASK) >> STRAP_DDR_CONFIGURATION_SHIFT;
	pci_memwin_size = (regval & STRAP_PCI_MEMWIN_SIZE_MASK) >> STRAP_PCI_MEMWIN_SIZE_SHIFT;
printk("board_init_once: regval=%08lx, ddr_strap=%lx, %d chips, pci_size=%lx\n", regval, board_strap, NUM_DDR, pci_memwin_size);

	switch (board_strap & 4) {
	case 0:
		ddr_mode_shift = 2; // 64 bit
		break;
	case 4:
		ddr_mode_shift = 1; // 32 bit
		break;
	default:
        /* Should do assert here */
		ddr_mode_shift = 0;
		printk("board_init_once: Invalid strapping option read %08lx\n", regval);
		break;
	}

	/* The 7401 board has 4 chips */
	if (ddr_mode_shift > 0) {
		unsigned long bit14_13 = (board_strap & 3);
		unsigned long partSize;

     		switch(bit14_13) {
		case 0:
			partSize = (128 << 20);
			break;
		default:
 			partSize = (8 << 20) << bit14_13;  // 16 MB if strap value is 1
     		}

		memSize = NUM_DDR*partSize;

	}
    else {
        /* Should never get here */
			memSize = 0;
			printk("board_init_once: Invalid strapping option read %08lx\n", regval);
	}
	
	printk("Detected %ld MB on board\n", (memSize >>20));


#if 0
	/* Temp: Displays RAC infomation */
	regval = *((volatile unsigned long *) 0xff400000);
	printk("RAC configuration register @FF40_0000 = %08x\n", regval);

	regval = *((volatile unsigned long *) 0xff400004);
	printk("RAC address range register @FF40_0004 = %08x\n", regval);	
	
#endif
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
			printk("Board strapped at %ld MB, default is %d MB\n", (dramSize>>20), (DRAM_SIZE>>20));
		}
	}
#endif
    if (dramSize)
	    return dramSize;
    else
        return DRAM_SIZE;
}


EXPORT_SYMBOL(get_RAM_size);

