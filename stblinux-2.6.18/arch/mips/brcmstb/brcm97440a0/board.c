/*
 * arch/mips/brcmstb/brcm97400a0/board.c
 *
 * Copyright (C) 2004-2006 Broadcom Corporation
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
#endif

#define NUM_DDR 4


#define SUN_TOP_CTRL_STRAP_VALUE 0xb040401c

#define STRAP_PCI_MEMWIN_SIZE_SHIFT 7
#define STRAP_PCI_MEMWIN_SIZE_MASK 0x00000018	/* Bit 7 & 8 */

#define STRAP_DDR2_0_CONFIGURATION_SHIFT 	13
#define STRAP_DDR2_0_CONFIGURATION_MASK  0x00006000  /* Bits 13-14 */

#define STRAP_DDR2_1_CONFIGURATION_SHIFT 	15
#define STRAP_DDR2_1_CONFIGURATION_MASK  0x00018000  /* Bits 15-16 */


static unsigned long
board_init_once(void)
{
	int i;
	unsigned long regval;
	unsigned long memSize0, memSize1;
	unsigned long ddr0, ddr1;
	unsigned long pci_memwin_size;
	
	regval = *((volatile unsigned long *) SUN_TOP_CTRL_STRAP_VALUE) ;

	/* Bit 12-13: 	DDR configuration for DDR0
	 * 			0 =  256MB using 16Mx16 parts
	 *       		1 =  512MB using 32Mx16 parts
	 *			2 = 1024MB using 64Mx16 parts
	 *			3 = 1024MB using 128Mx16 parts
	 */

	ddr0 = (regval & STRAP_DDR2_0_CONFIGURATION_MASK) >> STRAP_DDR2_0_CONFIGURATION_SHIFT;
	ddr1 = (regval & STRAP_DDR2_1_CONFIGURATION_MASK) >> STRAP_DDR2_1_CONFIGURATION_SHIFT;
	pci_memwin_size = (regval & STRAP_PCI_MEMWIN_SIZE_MASK) >> STRAP_PCI_MEMWIN_SIZE_SHIFT;
printk("board_init_once: regval=%08x, ddr0_strap=%x, , ddr1_strap=%x, pci_size=%x\n", regval, ddr0, ddr1, pci_memwin_size);

	switch (ddr0 & 3) {
	case 0:
		memSize0 = 256 << 20; 
		break;
	case 1:
		memSize0 = 512 << 20; 
		break;
	case 2:
		memSize0 = 1024 << 20; 
		break;
	case 3:
		memSize0 = 1024 << 20; 
		break;
	}

	switch (ddr1 & 3) {
	case 0:
		memSize1 = 256 << 20; 
		break;
	case 1:
		memSize1 = 512 << 20; 
		break;
	case 2:
		memSize1 = 1024 << 20; 
		break;
	case 3:
		memSize1 = 1024 << 20; 
		break;
	}
	
	printk("Detected %d & %d MB on board, using only 256MB\n", memSize0 >> 20, memSize1 >> 20);
	return 256 << 20;
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
			printk("Board strapped at %ld MB, default is %d MB\n", (dramSize>>20), (DRAM_SIZE>>20));
		}
	}
    if (dramSize)
	    return dramSize;
    else
        return DRAM_SIZE;
}


EXPORT_SYMBOL(get_RAM_size);

