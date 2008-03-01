/*
 * arch/mips/brcmstb/brcm97325A0/board.c
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
#endif	// CONFIG_MIPS_BRCM_IKOS

#define NUM_DDR 4
#define SUN_TOP_CTRL_STRAP_VALUE 0xb040401c

#define STRAP_PCI_MEMWIN_SIZE_SHIFT	8
#define STRAP_PCI_MEMWIN_SIZE_MASK	0x00000300	/* Bit 8 & 9 */

#define STRAP_DDR2_0_CONFIGURATION_SHIFT 	23
#define STRAP_DDR2_0_CONFIGURATION_MASK		0x01800000  /* Bits 23-24 */

#define STRAP_DDR2_1_CONFIGURATION_SHIFT 	21
#define STRAP_DDR2_1_CONFIGURATION_MASK		0x00600000  /* Bits 21-22 */

static unsigned long board_init_once(void)
{
	int i;
	unsigned long regval;
	unsigned long memSize0, memSize1;
	unsigned long ddr0, ddr1;
	unsigned long pci_memwin_size;
	
	regval = *((volatile unsigned long *) SUN_TOP_CTRL_STRAP_VALUE) ;

	/* Bit 13-14: 	DDR configuration for DDR0
	 *		0 =  256MB using 16Mx16 parts
	 *     		1 =  512MB using 32Mx16 parts
	 *		2 = 1024MB using 64Mx16 parts
	 *		3 = 1024MB using 128Mx16 parts
	 */

	ddr0 = (regval & STRAP_DDR2_0_CONFIGURATION_MASK) >> STRAP_DDR2_0_CONFIGURATION_SHIFT;
	ddr1 = (regval & STRAP_DDR2_1_CONFIGURATION_MASK) >> STRAP_DDR2_1_CONFIGURATION_SHIFT;
	pci_memwin_size = (regval & STRAP_PCI_MEMWIN_SIZE_MASK) >> STRAP_PCI_MEMWIN_SIZE_SHIFT;
printk("board_init_once: regval=%08x, ddr0_strap=%x, , ddr1_strap=%x, pci_size=%x\n", regval, ddr0, ddr1, pci_memwin_size);
#ifdef CONFIG_MIPS_BRCM_IKOS
	return 32 << 20;
#else
	switch (ddr0 & 3) {
	case 0:
		memSize0 = 32 << 20; 
		break;
	case 1:
		memSize0 = 64 << 20; 
		break;
	case 2:
		memSize0 = 128 << 20; 
		break;
	case 3:
		memSize0 = 256 << 20; 
		break;
	}

	switch (ddr1 & 3) {
	case 0:
		memSize1 = 32 << 20; 
		break;
	case 1:
		memSize1 = 64 << 20; 
		break;
	case 2:
		memSize1 = 128 << 20; 
		break;
	case 3:
		memSize1 = 256 << 20; 
		break;
	}
	
	printk("Detected %d & %d MB on board, total %d MB\n", memSize0 >> 20, memSize1 >> 20, memSize0 >> 20 + memSize1 >> 20);
	return memSize0 + memSize1;
#endif
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

