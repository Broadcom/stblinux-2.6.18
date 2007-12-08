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
#define EXPORT_SYMTAB
#include <linux/module.h>

#include <asm/brcmstb/common/brcmstb.h>
#include <asm/brcmstb/brcm97038c0/bchp_pci_cfg.h>

#ifdef DRAM_SIZE
#undef DRAM_SIZE
#endif

#define DRAM_SIZE	(64 << 20)

#define SUN_TOP_CTRL_STRAP_VALUE 0xb0404018

#define STRAP_PCI_MEMWIN_SIZE_SHIFT 7
#define STRAP_PCI_MEMWIN_SIZE_MASK 0x00000018	/* Bit 7 & 8 */

#define STRAP_DDR_CONFIGURATION_SHIFT 	13
#define STRAP_DDR_CONFIGURATION_MASK  	0x0000E000


static unsigned long
board_init_once(void)
{
	unsigned long regval;
	unsigned long memSize = 1<<4;
	unsigned long board_strap, partSize;
	unsigned long pci_memwin_size;
	
	volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;
	unsigned long chipId = (*pSundryRev) >> 16;
	
	regval = *((volatile unsigned long *) SUN_TOP_CTRL_STRAP_VALUE) ;

	/* For 7038 chip:
	 * Bit 15: 		0 = 64 bit DDR mode (4 chips)
	 *       		1 = 32 bit DDR mode (2 chips)
	 * Bit 14:13:		0 = Reserved
	 *			1 = 8Mx16bit part
	 *			2 = 16Mx16bit part
	 *			3 = 32Mx16bit part
	 * For 7438 chip:
	 * Bit 15:		1: 64 bit DDR 512Mbit X 8 chips
	 * Bit 14-13 are dont-care.
	 */

	board_strap = (regval & STRAP_DDR_CONFIGURATION_MASK) >> STRAP_DDR_CONFIGURATION_SHIFT;
	pci_memwin_size = (regval & STRAP_PCI_MEMWIN_SIZE_MASK) >> STRAP_PCI_MEMWIN_SIZE_SHIFT;
	partSize = (8 << 20) << (board_strap & 3);  // 16 MB if strap value is 1

	switch (chipId) {
	case 0x7438:
		if (board_strap & 4) {
			memSize = 512 << 20; // 512MB
			printk("Detected %d MB on board\n", (int)(memSize >>20));
			return memSize;
		}
		/* Else fall thru, using 16bitX parts as descripted above. */
	case 0x7038:
		if(! (board_strap & 3))
		{
			printk("warning: invalid strapping %08lx\n", regval);
			break;
		}
		memSize = ((board_strap & 4) ? 2 : 4) * partSize;
		printk("Detected %d MB on board\n", (int)(memSize >> 20));
		return(memSize);
	default:
		BUG();
		break;
	}
	return(DRAM_SIZE);
}


unsigned long
get_RAM_size(void)
{
	static int once;
	static unsigned long dramSize = 0;

	if (!once) {
		once++;
		dramSize = board_init_once();
	}
	return(dramSize);
}


EXPORT_SYMBOL(get_RAM_size);

