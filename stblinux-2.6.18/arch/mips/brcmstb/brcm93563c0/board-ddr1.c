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

#include <asm/bootinfo.h>

#include <asm/brcmstb/common/brcmstb.h>
#include <asm/brcmstb/brcm93563c0/bchp_pci_cfg.h>
#include "mmzone.h"	/* asm/mach-brcmstb */

/* CP0 hazard avoidance. */
#define BARRIER __asm__ __volatile__(".set noreorder\n\t" \
				     "nop; nop; nop; nop; nop; nop;\n\t" \
				     ".set reorder\n\t")



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

static bcm_discontigMem_t bcm3563c0_discontig_memmap;

volatile unsigned long LOWER_RAM_SIZE = 0x08000000; /* Default on Ref boards */
volatile unsigned long LOWER_RAM_END  = 0x08000000;
EXPORT_SYMBOL(LOWER_RAM_SIZE);
EXPORT_SYMBOL(LOWER_RAM_END);

static void bcm3563c0_memmap_init(void);

static unsigned long
board_init_once(void)
{
	int i;
	unsigned long regval;
	unsigned long memSize0, memSize1;
	unsigned long board_strap, ddr_mode_shift;
	unsigned long pci_memwin_size;

	memset(&bcm3563c0_discontig_memmap, 0, sizeof(bcm_discontigMem_t));
	bcm_pdiscontig_memmap = &bcm3563c0_discontig_memmap;

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
	printk("board_init_once: regval=%08x, ddr_0_strap=%x, %d chips, pci_size=%x\n", 
		(unsigned int) regval, (unsigned int) board_strap, NUM_DDR_0, (unsigned int) pci_memwin_size);

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
                	printk("!!!board_init_once: Invalid strapping option read %08x\n", (unsigned int) regval);
			break;
	}
	LOWER_RAM_SIZE = bcm_pdiscontig_memmap->memSize[0] = memSize0;
	LOWER_RAM_END = LOWER_RAM_SIZE;
	bcm_pdiscontig_memmap->physAddr[0] = 0UL;
	bcm_pdiscontig_memmap->nDdrCtrls = 1;

	regval = *((volatile unsigned long *) SUN_TOP_CTRL_STRAP_VALUE_1) ;

	/*
	 * Bit 01:00:	0 = not populated 
	 *         	1 = 8Mx16bit part
	 *		2 = 16Mx16bit part
	 *		3 = 32Mx16bit part
	 */

	board_strap = (regval & STRAP_DDR_1_CONFIGURATION_MASK) >> 
	                              STRAP_DDR_1_CONFIGURATION_SHIFT;
	printk("board_init_once: regval=%08x, ddr_1_strap=%x, %d chips\n", 
		(unsigned int) regval, (unsigned int) board_strap, NUM_DDR_1);

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
                	printk("!!!board_init_once: Invalid strapping option read %08x\n", (unsigned int) regval);
			break;
	}
	bcm_pdiscontig_memmap->memSize[1] = memSize1;
	bcm_pdiscontig_memmap->physAddr[1] = UPPER_RAM_BASE;
	if (memSize1 > 0) {
		bcm_pdiscontig_memmap->nDdrCtrls++;
	
	
		printk("Board Strap Settings: DDR_0 %d MB @%08x, DDR_1 %d MB @%08x\n",
			(unsigned int)(bcm_pdiscontig_memmap->memSize[0] >> 20),  
			(unsigned int)(bcm_pdiscontig_memmap->physAddr[0]),
			(unsigned int)(bcm_pdiscontig_memmap->memSize[1] >> 20),
			(unsigned int)(bcm_pdiscontig_memmap->physAddr[1]));	
	}
	else
		printk("Detected %d MB on board. DDR0 %dMB DDR1 %dMB\n", 
	         (unsigned int) ((memSize0+memSize1) >>20), (unsigned int) (memSize0 >> 20), (unsigned int) (memSize1 >> 20));

	(void) bcm3563c0_memmap_init();

	return (memSize0 + memSize1);
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

/******************************* TLB Setup **********************************/

/* 4MB page size */
#define OFFSET_4MBYTES        0x00400000
#define OFFSET_8MBYTES        0x00800000
#define TLBLO_OFFSET_8MBYTES 		(OFFSET_8MBYTES>>(4+2))

/* 16MB page size */
#define OFFSET_16MBYTES       0x01000000
#define OFFSET_32MBYTES       0x02000000
#define TLBLO_OFFSET_32MBYTES  		(OFFSET_32MBYTES>>(4+2))

/* 64MB page size */
#define OFFSET_64MBYTES       0x04000000
#define OFFSET_128MBYTES      0x08000000
#define TLBLO_OFFSET_128MBYTES 		(OFFSET_128MBYTES>>(4+2))

/* 256MB page size */
#define OFFSET_256MBYTES      0x10000000
#define OFFSET_512MBYTES      0x20000000
#define TLBLO_OFFSET_512MBYTES 		(OFFSET_512MBYTES>>(4+2))



#define PCI_BUS_MASTER  BCHP_PCI_CFG_STATUS_COMMAND_BUS_MASTER_MASK
#define PCI_IO_ENABLE   BCHP_PCI_CFG_STATUS_COMMAND_MEMORY_SPACE_MASK
#define PCI_MEM_ENABLE  BCHP_PCI_CFG_STATUS_COMMAND_IO_SPACE_MASK


#if	defined( CONFIG_CPU_BIG_ENDIAN )
#define	CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN	2
#else
#define	CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN	0
#endif


/*
 * THT: For now, just use hard coded values.  Will clean up later
 */
static inline void bcm3563c0_setup_wired_discontig(void)
{
#ifdef CONFIG_DISCONTIGMEM	/* uses some DISCONTIG-only macros */
	unsigned long flags;
	unsigned long old_ctx;
	unsigned long lo0, lo1, hi;
	volatile int entry, wired;

	/*
	 * 7400/7405/7335 with DISCONTIG:
	 *
	 * 0000_0000 - 7fff_ffff - USER
	 * 8000_0000 - 8fff_ffff - kseg0 - DDR_0 (256MB @ 0000_0000)
	 * 9000_0000 - 9fff_ffff - kseg0 - EBI/Registers (256MB)
	 * a000_0000 - afff_ffff - kseg1 - DDR_0 (256MB @ 0000_0000)
	 * b000_0000 - bfff_ffff - kseg1 - EBI/Registers (256MB)
	 * c000_0000 - cfff_ffff - vmalloc, kernel modules, etc. (256MB)
	 * d000_0000 - dfff_ffff - PCI MEM (256MB)
	 * e000_0000 - efff_ffff - DDR_0 (256MB @ 2000_0000)
	 * f000_0000 - f060_000b - PCI I/O (6MB)
	 * ff20_0000 - ff3f_ffff - EJTAG
	 * ff40_0000 - ffff_ffff - FIXMAP, etc.
	 */

	local_irq_save(flags);
	old_ctx = (read_c0_entryhi() & 0xff);
	entry = wired = 0;

#define ATTR_UNCACHED	0x17
#define ATTR_CACHED	0x1f
#define WR_TLB(va, pa0, pa1, attr) do \
	{ \
		hi = ((va) & 0xffffe000); \
		lo0 = (((pa0) >> (4+2)) & 0x3fffffc0) | (attr); \
		lo1 = (((pa1) >> (4+2)) & 0x3fffffc0) | (attr); \
		write_c0_entrylo0(lo0); \
		write_c0_entrylo1(lo1); \
		BARRIER; \
		write_c0_entryhi(hi); \
		write_c0_index(entry); \
		BARRIER; \
		tlb_write_indexed(); \
		BARRIER; \
		entry++; \
	} while(0)

	if ((bcm_pmemmap->tlb_mask & PM_16M) == PM_16M) {
		unsigned long va, pa0, pa1;
		
		write_c0_pagemask(PM_16M);	/* each entry has 2x 64MB mappings */

		/* Allocate 256MB PCI address space */
		for (va = bcm_pmemmap->pci_vAddr,
				pa0 = CPU2PCI_CPU_PHYS_MEM_WIN_BASE,
				pa1 = CPU2PCI_CPU_PHYS_MEM_WIN_BASE + OFFSET_16MBYTES;
			va < (bcm_pmemmap->pci_vAddr + bcm_pmemmap->pci_winSize);
			va += OFFSET_32MBYTES, pa0 += OFFSET_32MBYTES, pa1 += OFFSET_32MBYTES
			) 
		{
			WR_TLB(va, pa0, pa1, ATTR_UNCACHED);
		}

		for (va = bcm_pmemmap->mem_vAddr[1],
				pa0 = bcm_pdiscontig_memmap->physAddr[1],
				pa1 = pa0 + OFFSET_16MBYTES;
			pa1 < (bcm_pdiscontig_memmap->physAddr[1] + bcm_pdiscontig_memmap->memSize[1]);
			va += OFFSET_32MBYTES, pa0 += OFFSET_32MBYTES, pa1 += OFFSET_32MBYTES
			) 
		{
			WR_TLB(va, pa0, pa1, ATTR_CACHED);
		}
	}
	else {
		/* TBD */
		printk("TLB mapping for under 16MB needed\n");
		BUG();
	}

	/* For 3563, I/O win size is defined as 32MB, but we use the standard size 60000b */
	write_c0_pagemask(PM_4M);
	WR_TLB(PCI_IO_WIN_BASE,
		PCI_IO_WIN_BASE,
		PCI_IO_WIN_BASE + OFFSET_4MBYTES,
		ATTR_UNCACHED);

#undef WR_TLB
#undef ATTR_CACHED
#undef ATTR_UNCACHED

	/* restore original state; save new wired index */
	write_c0_pagemask(PM_4K);
	write_c0_entryhi(old_ctx);
	write_c0_wired(entry);

	local_irq_restore(flags);

	dump_tlb_wired();
#endif
}

extern bcm_memmap_t* bcm_pmemmap;
/*
 * Board Specific TLB mappings for bcm7440bx
 */
static bcm_memmap_t bcm3563c0_memmap = {
	.tlb_mask =		PM_16M,
	.pci_vAddr =		PCI_MEM_WIN_BASE,		/* 0xd000_0000 */
	.pci_winSize =		PCI_MEM_WIN_SIZE,		/* 128MB for 3563 */
	.io_vAddr =		PCI_IO_WIN_BASE,			/* 0xf0000000 */
	.io_winSize =		PCI_IO_WIN_SIZE,			/* 0x02000000 */
	.nMemBanks =		2,						/* Match number of phys mem banks */
	.mem_vAddr =		{0, UPPER_RAM_VBASE,0},
	.tlb_memmap =	bcm3563c0_setup_wired_discontig,
	.tlb_pci_bridge =	NULL, /* Will set up to use standard method */
	.tlb_sata_bridge =	NULL, /* No SATA on board */
};

extern void (*brcm_default_boot_mem)(void);
void (*__bcm_lowmem_default_boot_mem)(void); 


static __init void 
bcm3563c0_default_boot_mem(void)
{
/*
	 * Setup default bootmem for low memory
	 */
	 __bcm_lowmem_default_boot_mem();

	// Reserve all of DDR1 memory
	if (bcm_pdiscontig_memmap->memSize[1]) {
		brcm_insert_ram_node(
			bcm_pdiscontig_memmap->physAddr[1], 
			bcm_pdiscontig_memmap->memSize[1], BOOT_MEM_RESERVED, NULL);
	}
	
}

static void 
bcm3563c0_memmap_init(void)
{
	/* For PCI bridge, use default */
	bcm3563c0_memmap.tlb_pci_bridge = bcm_pmemmap->tlb_pci_bridge;

	/* Replace standard maps with bcm3563c0 specific maps */
	bcm_pmemmap = &bcm3563c0_memmap;

	__bcm_lowmem_default_boot_mem = brcm_default_boot_mem;
	brcm_default_boot_mem = bcm3563c0_default_boot_mem;
}
