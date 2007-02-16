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
#include <asm/brcmstb/brcm97400a0/bchp_pci_cfg.h>

#ifdef CONFIG_MIPS_BRCM_IKOS

#ifdef DRAM_SIZE
#undef DRAM_SIZE
#endif

// For Ikos
#define DRAM_SIZE	(32 << 20)
#endif

#define NUM_DDR 4


#define SUN_TOP_CTRL_STRAP_VALUE 0xb040401c

#define STRAP_PCI_MEMWIN_SIZE_SHIFT 7
#define STRAP_PCI_MEMWIN_SIZE_MASK 0x00000018	/* Bit 7 & 8 */

#define STRAP_DDR_CONFIGURATION_SHIFT 	12
#define STRAP_DDR_CONFIGURATION_MASK  	0x00003000  /* Bits 12-13, we will not look into Bits 14-15 */


static unsigned long
board_init_once(void)
{
	int i;
	unsigned long regval;
	unsigned long memSize = 1<<4;
	unsigned long board_strap;
	unsigned long pci_memwin_size;
	
	regval = *((volatile unsigned long *) SUN_TOP_CTRL_STRAP_VALUE) ;

	/* Bit 12-13: 	DDR configuration for DDR0
	 * 				0 = 8Mx16 bit parts (16MB each)
	 *       		1 = 16Mx16 bit parts (32MB each)
	 *				2 = 32Mx16 bit parts (64MB each)
	 *				3 = 64Mx16 bit parts (128MB each)
	 * Bit 14: indicates the configuration of DDR1	
     * Bit 15: indicates the configuration of DDR2
	 */

	board_strap = (regval & STRAP_DDR_CONFIGURATION_MASK) >> STRAP_DDR_CONFIGURATION_SHIFT;
	pci_memwin_size = (regval & STRAP_PCI_MEMWIN_SIZE_MASK) >> STRAP_PCI_MEMWIN_SIZE_SHIFT;
printk("board_init_once: regval=%08x, ddr_strap=%x, %d chips, pci_size=%x\n", regval, board_strap, NUM_DDR, pci_memwin_size);

	switch (board_strap & 3) {
	case 0:
		memSize = (16 << 20)*NUM_DDR; 
		break;
	case 1:
		memSize = (32 << 20)*NUM_DDR; 
		break;
	case 2:
		memSize = (64 << 20)*NUM_DDR; 
		break;
	case 3:
		memSize = (128 << 20)*NUM_DDR; 
		break;
	}
	
	printk("Detected %d MB on board\n", (memSize >>20));
	return memSize;
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

