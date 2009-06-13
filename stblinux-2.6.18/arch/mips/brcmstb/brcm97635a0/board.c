/*
 * arch/mips/brcmstb/brcm97635a0/board.c
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
 * 05-13-2009   jipeng Modified from 7440B0
 */

#include <linux/config.h>
#include <linux/ctype.h>
// For module exports
#include <linux/module.h>

#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/bootinfo.h>

#include "../common/cfe_xiocb.h"
#include <asm/brcmstb/common/cfe_call.h>
#include <asm/brcmstb/common/brcmstb.h>
#include "mmzone.h"	/* asm/mach-brcmstb */


/* CP0 hazard avoidance. */
#define BARRIER __asm__ __volatile__(".set noreorder\n\t" \
				     "nop; nop; nop; nop; nop; nop;\n\t" \
				     ".set reorder\n\t")

#define NUM_DDR 4
#define SUN_TOP_CTRL_STRAP_VALUE 0xb040401c

#define STRAP_PCI_MEMWIN_SIZE_SHIFT 7
#define STRAP_PCI_MEMWIN_SIZE_MASK 0x00000180	/* Bit 7 & 8 */

#define STRAP_DDR2_0_CONFIGURATION_SHIFT 	13
#define STRAP_DDR2_0_CONFIGURATION_MASK  0x00006000  /* Bits 13-14 */

#define STRAP_DDR2_1_CONFIGURATION_SHIFT 	15
#define STRAP_DDR2_1_CONFIGURATION_MASK  0x00018000  /* Bits 15-16 */

extern int get_cfe_hw_info(cfe_xiocb_t* cfe_boardinfo);
extern int get_cfe_env_variable(void * name_ptr,
				void * val_ptr,  int val_length);

/*
** Structure for return of CFE HW INFO
*/
cfe_xiocb_t cfe_boardinfo;

int cfe_hwinfo_called  = 0;
int cfe_hwinfo_stat    = -99;

/*
 * Board Name from CFE
 */
static char local_cfe_boardname[CFE_BOARDNAME_MAX_LEN];


unsigned int  mem_chip_cnt[4]  = {2,2,2,2};     // Number of memory chips indexed by controller. Default value is 2.
static bcm_discontigMem_t bcm7635a0_discontig_memmap;

/* Default to 512MB/768MB config data */
volatile unsigned int  NR_NODES       = 3;
volatile unsigned int  UPPER_RAM_NODE = 2;
volatile unsigned long UPPER_RAM_SIZE = 0x20000000;
volatile unsigned long LOWER_RAM_SIZE = 0x10000000;
volatile unsigned long LOWER_RAM_END  = 0x10000000;
EXPORT_SYMBOL(NR_NODES);
EXPORT_SYMBOL(UPPER_RAM_NODE);
EXPORT_SYMBOL(UPPER_RAM_SIZE);
EXPORT_SYMBOL(LOWER_RAM_SIZE);
EXPORT_SYMBOL(LOWER_RAM_END);
#define BCM7635A0_UPPER_RAM_BASE 0x20000000

/*
 * Various environment variables provided to user
 * for tuning knobs.
 */
char tmp_envbuf[128];

int ATAPI_ERRINFO = -1;
int SATA_RX_LVL   = -1;
int SATA_TX_LVL   = -1;
int SATA_TX_PRE   = -1;


static int
bcm_atoi(const char *s)
{
	int n;
	int neg = 0;

	n = 0;

	if (*s == '-') {
		neg = 1;
		s++;
	}

	while (isdigit(*s))
		n = (n * 10) + *s++ - '0';

	if (neg)
		n = 0 - n;

	return (n);
}



int __init board_get_cfe_env(void)
{
	/*
	 * Kernel tunables exported to the user
	 * via CFE Environment variables:
	 *
	 *  int ATAPI_ERRINFO = -1;   {0, 1}
	 *  int SATA_RX_LVL   = -1;   {0..3}
	 *  int SATA_TX_LVL   = -1;   {0..15}
	 *  int SATA_TX_PRE   = -1;   {0..7}
	 */
	const char* cfe_env = "ATAPI_ERRINFO";
	int res;
	
	tmp_envbuf[0] = 0;
	res = get_cfe_env_variable((void *)cfe_env,
				   (void *)tmp_envbuf, sizeof(tmp_envbuf));
	if (res == 0 && tmp_envbuf[0])
		ATAPI_ERRINFO = bcm_atoi(tmp_envbuf);
	else if (res)
		res = -3;

	cfe_env = "SATA_RX_LVL";
	tmp_envbuf[0] = 0;
	res = get_cfe_env_variable((void *)cfe_env,
				   (void *)tmp_envbuf, sizeof(tmp_envbuf));
	if (res == 0 && tmp_envbuf[0])
		SATA_RX_LVL = bcm_atoi(tmp_envbuf);
	else if (res)
		res = -3;

	cfe_env = "SATA_TX_LVL";
	tmp_envbuf[0] = 0;
	res = get_cfe_env_variable((void *)cfe_env,
				   (void *)tmp_envbuf, sizeof(tmp_envbuf));
	if (res == 0 && tmp_envbuf[0])
		SATA_TX_LVL = bcm_atoi(tmp_envbuf);
	else if (res)
		res = -3;

	cfe_env = "SATA_TX_PRE";
	tmp_envbuf[0] = 0;
	res = get_cfe_env_variable((void *)cfe_env,
				   (void *)tmp_envbuf, sizeof(tmp_envbuf));
	if (res == 0 && tmp_envbuf[0])
		SATA_TX_PRE = bcm_atoi(tmp_envbuf);
	else if (res)
		res = -3;

	cfe_hwinfo_called = 1;
	cfe_hwinfo_stat = get_cfe_hw_info(&cfe_boardinfo);

	/*
	 * Get the board name string.
	 */
	cfe_env = "CFE_BOARDNAME";
	res = get_cfe_env_variable((void *)cfe_env,
				   (void *)local_cfe_boardname,
				   CFE_BOARDNAME_MAX_LEN);
	if (res)
		res = -4;


	return res;
}

static void bcm7635a0_memmap_init(void);

extern int board_get_cfe_env(void);

static __init unsigned long 
board_init_once(void)
{
	/*
	** Dynamic structure initialization
	**
	** Process flash partition map from CFE HW INFO
	*/
	xiocb_boardinfo_t *boardinfo = (xiocb_boardinfo_t *)&cfe_boardinfo.plist.xiocb_boardinfo;
	cfe_xuint_t member;
	int i;
	int configured_from_cfe = 0;
	unsigned long total_mem_size;

	memset(&bcm7635a0_discontig_memmap, 0, sizeof(bcm_discontigMem_t));
	bcm_pdiscontig_memmap = &bcm7635a0_discontig_memmap;

        (void) board_get_cfe_env();

	if (cfe_min_rev(boardinfo->bi_ver_magic)) {

		printk("%s: CFE Board Name: %s\n", __FUNCTION__,
			local_cfe_boardname);
		printk("%s: Take memory configuration from CFE HWinfo\n", __FUNCTION__);

		if (CFE_NUM_DDR_CONTROLLERS > 4) {
			printk("%s: CFE_NUM_DDR_CONTROLLERS EXCEEDS LINUX LIMITATION OF 4 !!!!!\n", __FUNCTION__);
			BUG();
		}

		bcm_pdiscontig_memmap->physAddr[0] = 0UL;
		for (i = 0; i < CFE_NUM_DDR_CONTROLLERS; i++) {
			member = boardinfo->bi_ddr_bank_size[i];
			printk("-- DDR Bank %d: %d MB\n", i, (unsigned int)member);
			bcm_pdiscontig_memmap->memSize[i] = (unsigned long)member;
			bcm_pdiscontig_memmap->memSize[i] <<= 20;
			bcm_pdiscontig_memmap->nDdrCtrls++;
			if (i==1) {
				bcm_pdiscontig_memmap->physAddr[i] = 0x20000000;
			}
			else if (i > 1) {
				bcm_pdiscontig_memmap->physAddr[i] = bcm_pdiscontig_memmap->physAddr[i-1] +
					bcm_pdiscontig_memmap->memSize[i];
			}
		}
		configured_from_cfe = 1;
	}

	printk("Board Strap Settings: DDR_0 %d MB @%08x, DDR_1 %d MB @%08x\n",
		(unsigned int)(bcm_pdiscontig_memmap->memSize[0] >> 20),  
		(unsigned int)(bcm_pdiscontig_memmap->physAddr[0]),
		(unsigned int)(bcm_pdiscontig_memmap->memSize[1] >> 20),
		(unsigned int)(bcm_pdiscontig_memmap->physAddr[1]));
	/*
	** Init mem config globals
	**
	**  . NR_NODES = 3 if memSize[0] is GT 256M, 2 otherwise
	**  . UPPER_RAM_NODE = NR_NODES - 1;
	**  . UPPER_RAM_SIZE = memSize[1];
	**  . LOWER_RAM_SIZE and LOWER_RAM_END = memSize[0];
	*/
	/*
	 * THT: NR_NODES: How the memory is partitioned VIRTUALLY
	 * numDdrCtrls: How the memory is partitioned PHYSICALLY.
	 * They don't necessary match, because DDR0 can spans 2 nodes.
	 */
	total_mem_size = bcm_pdiscontig_memmap->memSize[0] + bcm_pdiscontig_memmap->memSize[1];
	if (bcm_pdiscontig_memmap->memSize[0] <= (256 << 20)) {
		//bcm_pdiscontig_memmap->numDdrCtrls = 2;
		NR_NODES = 2;
	}
	else {
		//bcm_pdiscontig_memmap->numDdrCtrls = 2;
		NR_NODES = 3;
	}
	UPPER_RAM_NODE = NR_NODES - 1;
	UPPER_RAM_SIZE = bcm_pdiscontig_memmap->memSize[1];
	LOWER_RAM_SIZE = LOWER_RAM_END = bcm_pdiscontig_memmap->memSize[0];

	(void) bcm7635a0_memmap_init();

	return (total_mem_size);
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

#define ATTR_UNCACHED	0x17
#define ATTR_CACHED	0x1f

static __init void 
bcm7635a0_tlb_memmap(void)
{
	unsigned long flags;
	unsigned long old_ctx;
	unsigned long lo0, lo1, hi, firstlim;
	volatile int entry, wired;

	
	//printk("bcm7635a0_pci_bridge\n");

	/*
	** PCI IO space 0xf8000000 - 0xfa000000 32MB mapping
	**   from physical address 0xf0000000 - 0xf2000000
	**
	**  PCI_IO_WIN_BASE = 0xf8000000
	**  CPU2PCI_CPU_PHYS_IO_WIN_BASE = 0xf0000000
	**  PCI_IO_WIN_SIZE = 0x02000000
	*/
	write_c0_pagemask(PM_16M);
	local_irq_save(flags);
	/* Save old context (ASID) */
	old_ctx = (read_c0_entryhi() & 0xff);

	// Save the start entry presumably starting at 0, but we never know
	entry = wired = read_c0_wired();
	/* Adjust to 8MB offset */
	hi = (PCI_IO_WIN_BASE & 0xffffe000);
	lo0 = (((CPU2PCI_CPU_PHYS_IO_WIN_BASE >> 6) & 0x3fffffc0) | ATTR_UNCACHED);
	lo1 = ((((CPU2PCI_CPU_PHYS_IO_WIN_BASE + OFFSET_16MBYTES) >> 6) &
												0x3fffffc0) | ATTR_UNCACHED);
	do {
		/*
		 * Make sure all entries differ.  If they're not different
		 * MIPS32 will take revenge ...
		 */
		printk("PCI IO Space entry: hi=%08x, lo0=%08x, lo1=%08x, wired=%d\n", hi, lo0, lo1, entry);
		write_c0_entrylo0(lo0);
		write_c0_entrylo1(lo1);
		BARRIER;
		write_c0_entryhi(hi);
		write_c0_index(entry);
		BARRIER;
		tlb_write_indexed();
		BARRIER;
		hi += OFFSET_32MBYTES;
		lo0 += TLBLO_OFFSET_32MBYTES;
		lo1 += TLBLO_OFFSET_32MBYTES;
		entry++;
	} while(0);

	/*
	**  NOTE - For both the 7440A0 and 7440B0:
	**
	**  PCI_MEM_WIN_BASE = 0xd0000000
	**  CPU2PCI_CPU_PHYS_MEM_WIN_BASE = 0xd0000000
	*/

	/*
	** Use 64MB page size mapping, 256MB page size does not work
	** Use 1 64MB entry to map the next 128 MB PCI Mem 
	*/
	write_c0_pagemask(PM_64M); 
	hi = PCI_MEM_WIN_BASE & 0xffffe000;
	firstlim = (PCI_MEM_WIN_BASE + PCI_MEM_WIN_SIZE) & 0xffffe000;
	lo0 = (((CPU2PCI_CPU_PHYS_MEM_WIN_BASE  >> 6) & 0x3fffffc0) | ATTR_UNCACHED);
	lo1 = ((((CPU2PCI_CPU_PHYS_MEM_WIN_BASE + OFFSET_64MBYTES) >> 6) & 0x3fffffc0) | ATTR_UNCACHED);

	do {
		printk("PCI Mem Window entry: hi=%08x, lo0=%08x, lo1=%08x, wired=%d\n", hi, lo0, lo1, entry);
		write_c0_entrylo0(lo0);
		write_c0_entrylo1(lo1);
		BARRIER;
		write_c0_entryhi(hi);
		write_c0_index(entry);
		BARRIER;
		tlb_write_indexed();
		BARRIER;
		hi += OFFSET_128MBYTES;
		lo0 += TLBLO_OFFSET_128MBYTES;
		lo1 += TLBLO_OFFSET_128MBYTES;
		entry++;
	}
	while (hi < firstlim);

	/*
	** NOTE:
	**
	**  For 7440A0 and 7440B0:
	**      UPPER_RAM_BASE  = 0x20000000
	**      UPPER_RAM_VBASE = 0xd8000000
	**      UPPER_RAM_SIZE  = 0x20000000
	*/

	/*
	** Use the next 4 64M entries to map DDR_1 area (512MB)
	** Change it to cacheable, no KSEG1 mapping (C=0)
	** For CMT platforms, this flag should be set to 0x1f (?)
	*/
	write_c0_pagemask(PM_64M); 
	hi = UPPER_RAM_VBASE & 0xffffe000;
	firstlim = (UPPER_RAM_VBASE + UPPER_RAM_SIZE) & 0xffffe000;
	/*  cache mode to cacheable (C = 3) */
	lo0 = (((UPPER_RAM_BASE >> 6) & 0x3fffffc0) | ATTR_CACHED);
	lo1 = ((((UPPER_RAM_BASE + OFFSET_64MBYTES) >> 6) & 0x3fffffc0) | ATTR_CACHED);
	do {
		/*
		** Make sure all entries differ.  If they're not different
		** MIPS32 will take revenge ...
		*/
		printk("Upper RAM entry: hi=%08x, lo0=%08x, lo1=%08x, wired=%d\n", hi, lo0, lo1, entry);
		write_c0_entrylo0(lo0);
		write_c0_entrylo1(lo1);
		BARRIER;
		write_c0_entryhi(hi);
		write_c0_index(entry);
		BARRIER;
		tlb_write_indexed();
		BARRIER;
		hi  += OFFSET_128MBYTES;
		lo0 += TLBLO_OFFSET_128MBYTES;
		lo1 += TLBLO_OFFSET_128MBYTES;
		entry++;
	} while (hi < firstlim);

    BARRIER;
	write_c0_entryhi(old_ctx);
	// THT: Write the wired entries here, before releasing the lock
	write_c0_wired(entry);

	write_c0_pagemask(PM_4K);
	local_irq_restore(flags);
}

	
static void 
bcm7635a0_pci_bridge(void)
{
// QY & THT: 11/13/03 This is here because CFE does not do it
	// first set up pci host.
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_STATUS_COMMAND)) |= (PCI_BUS_MASTER|PCI_IO_ENABLE|PCI_MEM_ENABLE);
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN0)) = CPU2PCI_PCI_PHYS_MEM_WIN0_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN1)) = CPU2PCI_PCI_PHYS_MEM_WIN1_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2)) = CPU2PCI_PCI_PHYS_MEM_WIN2_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3)) = CPU2PCI_PCI_PHYS_MEM_WIN3_BASE;

#ifdef CONFIG_CPU_LITTLE_ENDIAN
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0)) = 0x00000000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1)) = 0x00200000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2)) = 0x00400000;
#else
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0)) = 0x00000002;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1)) = 0x00200002;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2)) = 0x00400002;
#endif

	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_GISB_BASE_W)) = 0x10000000;

	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W0)) = 0x00000000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W1)) = 0x08000000;
	//*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W2)) = 0x04000000;

#ifndef CONFIG_CPU_LITTLE_ENDIAN
	// TDT - Swap memory base w0 when running big endian
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL))
		&= ~BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_MASK;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL))
		|= 2<<BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_SHIFT;
#endif

	//printk("about to touch pci config space\n");
	// do a pci config read.
	*((volatile unsigned long *)0xf8600004) = PCI_DEV_NUM_1394;
	//printk("$$$$$$$$$$ 1394 dev id %08x\n", *((volatile unsigned long *)0xf8600008));
	*((volatile unsigned long *)0xf8600004) = PCI_DEV_NUM_MINI;
	//printk("$$$$$$$$$$ mini slot dev id %08x\n", *((volatile unsigned long *)0xf8600008));
	*((volatile unsigned long *)0xf8600004) = PCI_DEV_NUM_EXT;
	//printk("$$$$$$$$$$ external dev id %08x\n", *((volatile unsigned long *)0xf8600008));

}

static void 
bcm7635a0_sata_bridge(void)
{
#ifndef CONFIG_MIPS_BRCM_SIM

/* 2nd PCI Bridge for SATA on 7038C0 */
/*
 * Set BCM7038 PCI Bus Bridge Command
 */

//#define PCI_BUS_MASTER  BCHP_PCI_CFG_STATUS_COMMAND_BUS_MASTER_MASK
//#define PCI_IO_ENABLE   BCHP_PCI_CFG_STATUS_COMMAND_MEMORY_SPACE_MASK
//#define PCI_MEM_ENABLE  BCHP_PCI_CFG_STATUS_COMMAND_IO_SPACE_MASK

#define BCHP_SATA_PHYSICAL_OFFSET         0x10500000
#define BCHP_SATA_KSEG1_OFFSET			KSEG1ADDR(BCHP_SATA_PHYSICAL_OFFSET)

#define BCHP_SATA_PCI_BRIDGE_PCI_CTRL     0x200
#define PCI_SATA_MEM_ENABLE               1
#define PCI_SATA_BUS_MASTER_ENABLE        2
#define PCI_SATA_PERR_ENABLE              0x10
#define PCI_SATA_SERR_ENABLE              0x20

	// first set up pci host.
	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_SATA_PCI_BRIDGE_PCI_CTRL)) |=
		(PCI_SATA_MEM_ENABLE|PCI_SATA_BUS_MASTER_ENABLE|PCI_SATA_PERR_ENABLE|PCI_SATA_SERR_ENABLE);

	 /************************************************
	  * Configure 7038C0 PCI config registers
	  ************************************************/

#define  BCHP_PCI_SATA_CFG_SLV_MEMORY_BASE_W0 0x20c

#ifdef CONFIG_CPU_LITTLE_ENDIAN
	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_SLV_MEMORY_BASE_W0)) 
		= PCI_7401_PHYS_MEM_WIN0_BASE;
#else
	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_SLV_MEMORY_BASE_W0)) 
		= PCI_7401_PHYS_MEM_WIN0_BASE | 2;
#endif

	/************************************************
    * Configure MIPS to PCI bridge.  
    ************************************************/
#define BCHP_PCI_SATA_CFG_CPU_2_PCI_MEM_WIN0 0x210
#define BCHP_PCI_SATA_CFG_CPU_2_PCI_IO_WIN0  0x214

#define CPU2PCI_PCI_SATA_PHYS_MEM_WIN0_BASE  0x10510000

// THT: For version 3 of the SATA bridge, the Win Base address must be the KSEG1 address.
#ifdef CONFIG_CPU_LITTLE_ENDIAN
 	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_CPU_2_PCI_MEM_WIN0)) 
 		= KSEG1ADDR(CPU2PCI_PCI_SATA_PHYS_MEM_WIN0_BASE);
 	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_CPU_2_PCI_IO_WIN0)) 
 		= 0;

#else

/* Using little endian byte order on disk */
	*((volatile unsigned long *) (BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_CPU_2_PCI_MEM_WIN0))
		= KSEG1ADDR(CPU2PCI_PCI_SATA_PHYS_MEM_WIN0_BASE | 0);
	


 /* DW swap */
	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_CPU_2_PCI_IO_WIN0))
		= 2;

#endif // ifdef Little Endian

		// do a pci config read.
	*((volatile unsigned long *)0xb0500204) = PCI_DEV_NUM_SATA;
	//printk("$$$$$$$$$$SATA dev id %08x\n", *((volatile unsigned long *)0xb0500208));
#endif	//!IKOS Simulation
}

/*
 * Board Specific TLB mappings for bcm7635ax
 */
static bcm_memmap_t bcm7635a0_memmap = {
	.tlb_mask =		PM_64M,
	.pci_vAddr =		PCI_MEM_WIN_BASE,		/* 0xd000_0000 */
	.pci_winSize =		PCI_MEM_WIN_SIZE,		/* 128MB for 7635 */
	.io_vAddr =		PCI_IO_WIN_BASE,			/* 0xf8000000 */
	.io_winSize =		PCI_IO_WIN_SIZE,			/* 0x02000000 */
	.nMemBanks =		1,
	.mem_vAddr =		{UPPER_RAM_VBASE,0},
	.tlb_memmap =	bcm7635a0_tlb_memmap,
	.tlb_pci_bridge =	bcm7635a0_pci_bridge,
	.tlb_sata_bridge =	bcm7635a0_sata_bridge
};

#if defined ( CONFIG_MIPS_BCM7635A0 )
#define LINUX_MIN_MEM   98  // MB

#else
#define LINUX_MIN_MEM   32  // MB
#endif
extern int linux_min_mem;
extern bcm_memnode_t brcm_bm;

extern int brcm_insert_ram_node(phys_t start, phys_t size, long type, bcm_memnode_t* bm);
extern void brcm_print_memory_map(void);

static __init void 
bcm7635a0_default_boot_mem(void)
{
	int ramSizeMB = get_RAM_size() >> 20;
	int size, total_size;
	char msg[80];
	
	if (ramSizeMB <= 32) {
#ifdef CONFIG_BLK_DEV_INITRD
		uart_puts("Using all RAM for STBs\n");
		size = ramSizeMB;

#else
		size = 20;

#endif
	}
#if defined ( CONFIG_MIPS_BCM7635A0 )
	else if (bcm_pdiscontig_memmap->memSize[0] < (256 << 20)) {
		size = linux_min_mem = 32;
		total_size = 94;
	}
#endif
	else {
		/* 
		  * Kernels on STBs with larger than 32MB, we only use 32MB RAM for the kernel
		  */
		size = linux_min_mem = LINUX_MIN_MEM;
#if defined ( CONFIG_MIPS_BCM7635A0 )
		total_size = 189;
#endif
	}

#if defined ( CONFIG_MIPS_BCM7635A0 )
	sprintf(msg, "Using %dMB for memory in %d regions, overwrite by passing mem=xx\n", total_size, numnodes);
#else
	total_size = size;
	sprintf(msg, "Using %dMB for memory, overwrite by passing mem=xx\n", total_size);
#endif
	uart_puts(msg);
	brcm_insert_ram_node(0, size<<20, BOOT_MEM_RAM, &brcm_bm);

#if defined ( CONFIG_MIPS_BCM7635A0 )
	if (bcm_pdiscontig_memmap->memSize[0] < (256 << 20)) {
		/*
		** 7635A0 Default Mem Map (384M memory)
		**
		**  User-defined physical RAM map:
		**  node [00000000, 01200000: RAM]
		**  node [01200000, 06e00000: RSVD]
		**  node [20000000, 04C00000: RAM]
		**  node [24C00000, 0b400000: RSVD]
		*/

		/* upper memory: 76M RAM, 180M RSVD */
		brcm_insert_ram_node(0x20000000, 76<<20, BOOT_MEM_RAM, &brcm_bm);
	}
	else if (bcm_pdiscontig_memmap->memSize[0] == (256 << 20)) {
		/*
		** 7635A0 Default Mem Map (512M memory)
		**
		**  User-defined physical RAM map:
		**  node [00000000, 06200000: RAM]
		**  node [06200000, 01e00000: RSVD]
		**  node [08000000, 01600000: RAM]
		**  node [09600000, 06a00000: RSVD]
		**  node [20000000, 04500000: RAM]
		**  node [24500000, 0bb00000: RSVD]
		*/

//printk("%s: Inserting 0x08000000, 22<<20, BOOT_MEM_RAM\n", __FUNCTION__);
		/* next 30M is RSVD, then 22M RAM, then 106M RSVD */
		brcm_insert_ram_node(0x08000000, 22<<20, BOOT_MEM_RAM, &brcm_bm);
//brcm_print_memory_map();


//printk("%s: Inserting 0x20000000, 69<<20, BOOT_MEM_RAM\n", __FUNCTION__);
		/* upper memory: 69M RAM, 187M RSVD */
		brcm_insert_ram_node(0x20000000, 69<<20, BOOT_MEM_RAM , &brcm_bm);
//brcm_print_memory_map();
	}
	else {
		/*
		** 7635A0 Default Mem Map (786M memory)
		**
		**  User-defined physical RAM map:
		**  node [00000000, 06200000: RAM]
		**  node [06200000, 01e00000: RSVD]
		**  node [08000000, 01600000: RAM]
		**  node [09600000, 06a00000: RSVD]
		**  node [20000000, 04500000: RAM]
		**  node [24500000, 0bb00000: RSVD]
		*/

//TBD
		/* next 30M is RSVD, then 22M RAM, then 106M RSVD */
		brcm_insert_ram_node(0x08000000, 22<<20, BOOT_MEM_RAM, &brcm_bm);

		/* upper memory: 69M RAM, 187M RSVD */
		brcm_insert_ram_node(0x20000000, 69<<20, BOOT_MEM_RAM , &brcm_bm);
	}
}
#endif

extern void (*brcm_default_boot_mem)(void);

static void 
bcm7635a0_memmap_init(void)
{
	/* Replace standard maps with bcm7635a0 specific maps */
	bcm_pmemmap = &bcm7635a0_memmap;

	// THT for now: 
	brcm_default_boot_mem = bcm7635a0_default_boot_mem;
}
