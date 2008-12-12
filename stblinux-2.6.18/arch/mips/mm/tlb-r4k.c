/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
 * Copyright (C) 1997, 1998, 1999, 2000 Ralf Baechle ralf@gnu.org
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2002 MIPS Technologies, Inc.  All rights reserved.
 * Copyright (C) 2004,2005 Broadcom Corp
 */
#include <linux/config.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/mmzone.h>
#include <linux/module.h> // For export symbol

#include <asm/cpu.h>
#include <asm/bootinfo.h>
#include <asm/mmu_context.h>
#include <asm/pgtable.h>
#include <asm/system.h>
#include <asm/brcmstb/common/brcmstb.h>

/*
 * Make sure all entries differ.  If they're not different
 * MIPS32 will take revenge ...
 */
#define UNIQUE_ENTRYHI(idx) (CKSEG0 + ((idx) << (PAGE_SHIFT + 1)))

/* CP0 hazard avoidance. */
#define BARRIER __asm__ __volatile__(".set noreorder\n\t" \
				     "nop; nop; nop; nop; nop; nop;\n\t" \
				     ".set reorder\n\t")

/* Atomicity and interruptability */
#ifdef CONFIG_MIPS_MT_SMTC

#include <asm/smtc.h>
#include <asm/mipsmtregs.h>

#define ENTER_CRITICAL(flags) \
	{ \
	unsigned int mvpflags; \
	local_irq_save(flags);\
	mvpflags = dvpe()
#define EXIT_CRITICAL(flags) \
	evpe(mvpflags); \
	local_irq_restore(flags); \
	}
#else

#define ENTER_CRITICAL(flags) local_irq_save(flags)
#define EXIT_CRITICAL(flags) local_irq_restore(flags)

#endif /* CONFIG_MIPS_MT_SMTC */

extern void build_tlb_refill_handler(void);

#ifdef DPRINTK
#undef DPRINTK
#endif
#if 0
#define DPRINTK printk
#else
#define DPRINTK(...) do { } while(0)
#endif

#ifdef CONFIG_MIPS_BRCM97XXX

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

#if	defined( CONFIG_MIPS_BRCM97XXX )
#if	defined( CONFIG_CPU_BIG_ENDIAN )
#define	CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN	2
#else
#define	CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN	0
#endif
#endif // defined( CONFIG_MIPS_BRCM97XXX )


/*
 * THT: For now, just use hard coded values.  Will clean up later
 */
static inline void brcm_setup_wired_discontig(void)
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

	/*
	 * THT: 6/27/08 for 2.6.18-5.1 PR43351
	 * Only the 16MB map use the new data structure, the 64MB map is left unchanged
	 */
	if ((bcm_pmemmap->tlb_mask & PM_64M) == PM_64M) {
		write_c0_pagemask(PM_64M);	/* each entry has 2x 64MB mappings */

#ifdef WIRED_PCI_MAPPING
		WR_TLB(PCI_MEM_WIN_BASE,
			PCI_MEM_START,
			PCI_MEM_START + OFFSET_64MBYTES,
			ATTR_UNCACHED);
		WR_TLB(PCI_MEM_WIN_BASE + OFFSET_128MBYTES,
			PCI_MEM_START + OFFSET_128MBYTES,
			PCI_MEM_START + OFFSET_128MBYTES + OFFSET_64MBYTES,
			ATTR_UNCACHED);
#endif
		WR_TLB(UPPER_RAM_VBASE,
			UPPER_RAM_BASE,
			UPPER_RAM_BASE + OFFSET_64MBYTES,
			ATTR_CACHED);
		WR_TLB(UPPER_RAM_VBASE + OFFSET_128MBYTES,
			UPPER_RAM_BASE + OFFSET_128MBYTES,
			UPPER_RAM_BASE + OFFSET_128MBYTES + OFFSET_64MBYTES,
			ATTR_CACHED);
	}
	else if ((bcm_pmemmap->tlb_mask & PM_16M) == PM_16M) {
		unsigned long va, pa0, pa1;
		
		write_c0_pagemask(PM_16M);	/* each entry has 2x 64MB mappings */

#ifdef WIRED_PCI_MAPPING
		/* Allocate 256MB PCI address space */
		for (va = bcm_pmemmap->pci_vAddr,
				pa0 = PCI_MEM_START,
				pa1 = PCI_MEM_START + OFFSET_16MBYTES;
			va < (bcm_pmemmap->pci_vAddr + bcm_pmemmap->pci_winSize);
			va += OFFSET_32MBYTES, pa0 += OFFSET_32MBYTES, pa1 += OFFSET_32MBYTES
			) 
		{
			WR_TLB(va, pa0, pa1, ATTR_UNCACHED);
		}
#endif

		/* Map VA to PA, PA and size given by bcm_pdiscontig_memmap */
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

	/* 8MB of mappings (PCI, but no PCIe) */
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
#endif
}

static inline void brcm_setup_wired_64(void)
{
	unsigned long flags;
	unsigned long old_ctx;
	unsigned long lo0, lo1, hi;
	volatile int entry, wired;

	write_c0_pagemask(PM_64M);  /* The next best thing we can have since 256MB does not work */

	local_irq_save(flags);
	/* Save old context and create impossible VPN2 value */
	old_ctx = (read_c0_entryhi() & 0xff);
	hi = (PCI_MEM_WIN_BASE&0xffffe000);
	lo0 = (((PCI_MEM_START>>(4+2))&0x3fffffc0)|0x17);
	lo1 = ((((PCI_MEM_START+OFFSET_64MBYTES)>>(4+2))&0x3fffffc0)|0x17);

	// Save the start entry presumably starting at 0, but we never know
	entry = wired = read_c0_wired();

#ifdef WIRED_PCI_MAPPING
//printk("Write first entry: hi=%08x, lo0=%08x, lo1=%08x, wired=%d\n", hi, lo0, lo1, entry);
	/* Blast 'em all away. */
	do {
		/*
		 * Make sure all entries differ.  If they're not different
		 * MIPS32 will take revenge ...
		 */
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
	} while(hi < PCI_IO_WIN_BASE);
#endif

//printk("Write end of first entry: hi=%08x, lo0=%08x, lo1=%08x, wired=%d\n", hi, lo0, lo1, entry);

	/* PCI/PCIe I/O - f000_0000 - f1ff_ffff */
	write_c0_pagemask(PM_16M);
	/* Adjust to 8MB offset */
	hi = PCI_IO_WIN_BASE;
	/* IO space starts at 0xf000_0000; VA = PA */
	lo0 = (((PCI_IO_WIN_BASE>>(4+2))&0x3fffffc0)|0x17);
	lo1 = ((((PCI_IO_WIN_BASE+OFFSET_16MBYTES)>>(4+2))&0x3fffffc0)|0x17);

	//printk("Write 2nd entry: hi=%08x, lo0=%08x, lo1=%08x, wired=%d\n", hi, lo0, lo1, entry);
	do {
		/*
		 * Make sure all entries differ.  If they're not different
		 * MIPS32 will take revenge ...
		 */
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

	BARRIER;
	write_c0_entryhi(old_ctx);
	// THT: Write the wired entries here, before releasing the lock
	write_c0_wired(entry); /* entry should be wired + 2 now */
	
	write_c0_pagemask(PM_4K);
	local_irq_restore(flags);
}

/* default: 16 entries * 16MB * 2 = 512MB = (0xf0000000 - 0xd0000000) */
#define PCI_MEM_TLB_ENTRIES	16

static inline void brcm_setup_wired_16(void)
{
	unsigned long flags;
	unsigned long old_ctx;
	unsigned long lo0, lo1, hi;
	volatile int entry, wired;

	/*
	 * NOTE:
	 *
	 * BMIPS3300 only supports 16MB TLB entries (1 hi/lo pair = 32MB).
	 *
	 * The reference kernel will map:
	 *
	 * d000_0000 - efff_ffff - PCI MEM (512MB) - 16 TLB entries
	 * f000_0000 - f1ff_ffff - PCI I/O ( 32MB) -  1 TLB entry
	 *
	 * It may be a good idea to shrink the PCI MEM region if your
	 * application does not need this much space, to free up some
	 * entries and help prevent TLB thrashing.
	 */
	write_c0_pagemask(PM_16M);

	local_irq_save(flags);
	/* Save old context and create impossible VPN2 value */
	old_ctx = (read_c0_entryhi() & 0xff);

	// Save the start entry presumably starting at 0, but we never know
	entry = wired = read_c0_wired();

	/* write entries for PCI MEM */
	hi = (PCI_MEM_WIN_BASE&0xffffe000);
	lo0 = (((PCI_MEM_WIN_BASE>>(4+2))&0x3fffffc0)|0x17);
	lo1 = ((((PCI_MEM_WIN_BASE+OFFSET_16MBYTES)>>(4+2))&0x3fffffc0)|0x17);

#ifdef WIRED_PCI_MAPPING
	/* Blast 'em all away. */
	while (entry < (PCI_MEM_TLB_ENTRIES+wired)) {
		/*
		 * Make sure all entries differ.  If they're not different
		 * MIPS32 will take revenge ...
		 */
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
	}
#endif

	/* write entry for PCI I/O */
	hi = (PCI_IO_WIN_BASE&0xffffe000);
	lo0 = (((PCI_IO_WIN_BASE>>(4+2))&0x3fffffc0)|0x17);
	lo1 = ((((PCI_IO_WIN_BASE+OFFSET_16MBYTES)>>(4+2))&0x3fffffc0)|0x17);

	do {
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

	BARRIER;
	write_c0_entryhi(old_ctx);

	// THT: Write it the wired entries here, before releasing the lock
	write_c0_wired(entry);

	write_c0_pagemask(PM_4K);
	local_irq_restore(flags);
}

static void brcm_setup_wired_entries(void)
{
#if defined(CONFIG_DISCONTIGMEM)

	/* Use 64MB pages with memory map supporting >256MB RAM */

	brcm_setup_wired_discontig();

#else

	if ((bcm_pmemmap->tlb_mask & PM_64M) == PM_64M) {
		/* Use 64MB pages with standard memory map */
		brcm_setup_wired_64();
	}
	else {
		/* Use 16MB pages with standard memory map */
		brcm_setup_wired_16();
	}
#endif
}

#define PCI_SATA_MEM_ENABLE			1
#define PCI_SATA_BUS_MASTER_ENABLE		2
#define PCI_SATA_PERR_ENABLE			0x10
#define PCI_SATA_SERR_ENABLE			0x20

static void brcm_setup_sata_bridge(void)
{
#if ! defined(CONFIG_BRCM_COMMON_PCI)
	/* For COMMON_PCI platforms, this moves into pci-brcmstb.c */
	if(brcm_sata_enabled == 0)
		return;

#if defined(BCHP_PCI_BRIDGE_PCI_CTRL) && defined(BRCM_SATA_SUPPORTED)
	/* Internal PCI SATA bridge setup for 7038, 7401, 7403, 7118, etc. */
	BDEV_SET(BCHP_PCI_BRIDGE_PCI_CTRL,
		(PCI_SATA_MEM_ENABLE|PCI_SATA_BUS_MASTER_ENABLE|
		 PCI_SATA_PERR_ENABLE|PCI_SATA_SERR_ENABLE));

	/* PCI slave window (SATA access to MIPS memory) */
	BDEV_WR(BCHP_PCI_BRIDGE_PCI_SLV_MEM_WIN_BASE,
		0 | CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);

	/* PCI master window (MIPS access to SATA BARs) */
	BDEV_WR(BCHP_PCI_BRIDGE_CPU_TO_SATA_MEM_WIN_BASE,
		PCI_SATA_MEM_START |
		CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);

	BDEV_WR(BCHP_PCI_BRIDGE_CPU_TO_SATA_IO_WIN_BASE,
		CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);

	/* do a PCI config read */
	BDEV_WR(BCHP_PCI_BRIDGE_SATA_CFG_INDEX, PCI_DEV_NUM_SATA);
	if(BDEV_RD(BCHP_PCI_BRIDGE_SATA_CFG_DATA) == 0xffffffff)
		printk(KERN_WARNING "Internal SATA is not responding\n");

#elif defined(BCHP_PCIX_BRIDGE_PCIX_CTRL) && defined(BRCM_SATA_SUPPORTED)

	/* Internal PCI-X SATA bridge setup for 7400, 7405, 7335 */

	BDEV_WR(BCHP_PCIX_BRIDGE_PCIX_CTRL,
		(PCI_SATA_MEM_ENABLE|PCI_SATA_BUS_MASTER_ENABLE|
		 PCI_SATA_PERR_ENABLE|PCI_SATA_SERR_ENABLE));
	BDEV_WR(BCHP_PCIX_BRIDGE_PCIX_SLV_MEM_WIN_BASE, 1);

	/* PCI slave window (SATA access to MIPS memory) */
	BDEV_WR(BCHP_PCIX_BRIDGE_PCIX_SLV_MEM_WIN_MODE,
		CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);

	/* PCI master window (MIPS access to SATA BARs) */
	BDEV_WR(BCHP_PCIX_BRIDGE_CPU_TO_SATA_MEM_WIN_BASE,
		PCI_SATA_MEM_START |
		CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);
	BDEV_WR(BCHP_PCIX_BRIDGE_CPU_TO_SATA_IO_WIN_BASE,
		CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);

	/* do a PCI config read */
	BDEV_WR(BCHP_PCIX_BRIDGE_SATA_CFG_INDEX, 0x80000000|PCI_DEV_NUM_SATA);
	if(BDEV_RD(BCHP_PCIX_BRIDGE_SATA_CFG_DATA) == 0xffffffff)
		printk(KERN_WARNING "Internal SATA is not responding\n");
#endif
#endif
}

static void brcm_setup_pci_bridge(void)
{
#if ! defined(CONFIG_BRCM_COMMON_PCI)
	/* For COMMON_PCI platforms, this moves into pci-brcmstb.c */
#if defined(BCHP_PCI_CFG_STATUS_COMMAND) && ! defined(CONFIG_BRCM_PCI_SLAVE)

	/* External PCI bridge setup (most chips) */

	BDEV_SET(BCHP_PCI_CFG_STATUS_COMMAND,
		PCI_BUS_MASTER|PCI_IO_ENABLE|PCI_MEM_ENABLE);

	BDEV_WR(BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN0, PCI_MEM_START);
	BDEV_WR(BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN1, PCI_MEM_START + 0x08000000);
	BDEV_WR(BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2, PCI_MEM_START + 0x10000000);
	BDEV_WR(BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3, PCI_MEM_START + 0x18000000);

	BDEV_WR(BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0,
		0x00000000 | CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);
	BDEV_WR(BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1,
		0x00200000 | CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);
	BDEV_WR(BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2,
		0x00400000 | CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);

	BDEV_WR(BCHP_PCI_CFG_GISB_BASE_W, 0x10000000);
	BDEV_WR(BCHP_PCI_CFG_MEMORY_BASE_W0, 0x00000000);
	BDEV_WR(BCHP_PCI_CFG_MEMORY_BASE_W1, 0x08000000);

#if ! defined(CONFIG_CPU_LITTLE_ENDIAN)
	// TDT - Swap memory base w0 when running big endian
	BDEV_UNSET(BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL,
		BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_MASK);
	BDEV_SET(BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL,
		2<<BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_SHIFT);
#endif

	/* do a PCI config read */
	DEV_WR(MIPS_PCI_XCFG_INDEX, PCI_DEV_NUM_EXT);
	DEV_RD(MIPS_PCI_XCFG_DATA);
#endif
#endif
}

#endif // All Broadcom STBs


void local_flush_tlb_all(void)
{
	unsigned long flags;
	unsigned long old_ctx;
	int entry;

#ifdef DEBUG_TLB
	printk("[tlball]");
#endif

	ENTER_CRITICAL(flags);
	/* Save old context and create impossible VPN2 value */
	old_ctx = read_c0_entryhi();
	write_c0_entrylo0(0);
	write_c0_entrylo1(0);

	entry = read_c0_wired();

	/* Blast 'em all away. */
	while (entry < current_cpu_data.tlbsize) {
		/* Make sure all entries differ. */
		write_c0_entryhi(UNIQUE_ENTRYHI(entry));
		write_c0_index(entry);
		mtc0_tlbw_hazard();
		tlb_write_indexed();
		entry++;
	}
	tlbw_use_hazard();
	write_c0_entryhi(old_ctx);
	EXIT_CRITICAL(flags);
}

/* All entries common to a mm share an asid.  To effectively flush
   these entries, we just bump the asid. */
void local_flush_tlb_mm(struct mm_struct *mm)
{
	int cpu;

	preempt_disable();

	cpu = smp_processor_id();

	if (cpu_context(cpu, mm) != 0) {
#ifdef DEBUG_TLB
		printk("[tlbmm<%d>]", cpu_context(cpu, mm));
#endif
		drop_mmu_context(mm, cpu);
	}

	preempt_enable();
}

void local_flush_tlb_range(struct vm_area_struct *vma, unsigned long start,
	unsigned long end)
{
	struct mm_struct *mm = vma->vm_mm;
	int cpu = smp_processor_id();

	if (cpu_context(cpu, mm) != 0) {
		unsigned long flags;
		int size;

#ifdef DEBUG_TLB
		printk("[tlbrange<%02x,%08lx,%08lx>]",
		       cpu_asid(cpu, mm), start, end);
#endif
		ENTER_CRITICAL(flags);
		size = (end - start + (PAGE_SIZE - 1)) >> PAGE_SHIFT;
		size = (size + 1) >> 1;
		if (size <= current_cpu_data.tlbsize/2) {
			int oldpid = read_c0_entryhi();
			int newpid = cpu_asid(cpu, mm);

			start &= (PAGE_MASK << 1);
			end += ((PAGE_SIZE << 1) - 1);
			end &= (PAGE_MASK << 1);
			while (start < end) {
				int idx;

				write_c0_entryhi(start | newpid);
				start += (PAGE_SIZE << 1);
				mtc0_tlbw_hazard();
				tlb_probe();
				BARRIER;
				idx = read_c0_index();
				write_c0_entrylo0(0);
				write_c0_entrylo1(0);
				if (idx < 0)
					continue;
				/* Make sure all entries differ. */
				write_c0_entryhi(UNIQUE_ENTRYHI(idx));
				mtc0_tlbw_hazard();
				tlb_write_indexed();
			}
			tlbw_use_hazard();
			write_c0_entryhi(oldpid);
		} else {
			drop_mmu_context(mm, cpu);
		}
		EXIT_CRITICAL(flags);
	}
}

void local_flush_tlb_kernel_range(unsigned long start, unsigned long end)
{
	unsigned long flags;
	int size;

	ENTER_CRITICAL(flags);
	size = (end - start + (PAGE_SIZE - 1)) >> PAGE_SHIFT;
	size = (size + 1) >> 1;
	if (size <= current_cpu_data.tlbsize / 2) {
		int pid = read_c0_entryhi();

		start &= (PAGE_MASK << 1);
		end += ((PAGE_SIZE << 1) - 1);
		end &= (PAGE_MASK << 1);

		while (start < end) {
			int idx;

			write_c0_entryhi(start);
			start += (PAGE_SIZE << 1);
			mtc0_tlbw_hazard();
			tlb_probe();
			BARRIER;
			idx = read_c0_index();
			write_c0_entrylo0(0);
			write_c0_entrylo1(0);
			if (idx < 0)
				continue;
			/* Make sure all entries differ. */
			write_c0_entryhi(UNIQUE_ENTRYHI(idx));
			mtc0_tlbw_hazard();
			tlb_write_indexed();
		}
		tlbw_use_hazard();
		write_c0_entryhi(pid);
	} else {
		local_flush_tlb_all();
	}
	EXIT_CRITICAL(flags);
}

void local_flush_tlb_page(struct vm_area_struct *vma, unsigned long page)
{
	int cpu = smp_processor_id();

	if (cpu_context(cpu, vma->vm_mm) != 0) {
		unsigned long flags;
		int oldpid, newpid, idx;

#ifdef DEBUG_TLB
		printk("[tlbpage<%d,%08lx>]", cpu_context(cpu, vma->vm_mm),
		       page);
#endif
		newpid = cpu_asid(cpu, vma->vm_mm);
		page &= (PAGE_MASK << 1);
		ENTER_CRITICAL(flags);
		oldpid = read_c0_entryhi();
		write_c0_entryhi(page | newpid);
		mtc0_tlbw_hazard();
		tlb_probe();
		BARRIER;
		idx = read_c0_index();
		write_c0_entrylo0(0);
		write_c0_entrylo1(0);
		if (idx < 0)
			goto finish;
		/* Make sure all entries differ. */
		write_c0_entryhi(UNIQUE_ENTRYHI(idx));
		mtc0_tlbw_hazard();
		tlb_write_indexed();
		tlbw_use_hazard();

	finish:
		write_c0_entryhi(oldpid);
		EXIT_CRITICAL(flags);
	}
}

/*
 * This one is only used for pages with the global bit set so we don't care
 * much about the ASID.
 */
void local_flush_tlb_one(unsigned long page)
{
	unsigned long flags;
	int oldpid, idx;

	ENTER_CRITICAL(flags);
	oldpid = read_c0_entryhi();
	page &= (PAGE_MASK << 1);
	write_c0_entryhi(page);
	mtc0_tlbw_hazard();
	tlb_probe();
	BARRIER;
	idx = read_c0_index();
	write_c0_entrylo0(0);
	write_c0_entrylo1(0);
	if (idx >= 0) {
		/* Make sure all entries differ. */
		write_c0_entryhi(UNIQUE_ENTRYHI(idx));
		mtc0_tlbw_hazard();
		tlb_write_indexed();
		tlbw_use_hazard();
	}
	write_c0_entryhi(oldpid);

	EXIT_CRITICAL(flags);
}

/*
 * We will need multiple versions of update_mmu_cache(), one that just
 * updates the TLB with the new pte(s), and another which also checks
 * for the R4k "end of page" hardware bug and does the needy.
 */
void __update_tlb(struct vm_area_struct * vma, unsigned long address, pte_t pte)
{
	unsigned long flags;
	pgd_t *pgdp;
	pud_t *pudp;
	pmd_t *pmdp;
	pte_t *ptep;
	int idx, pid;

	/*
	 * Handle debugger faulting in for debugee.
	 */
	if (current->active_mm != vma->vm_mm)
		return;

	ENTER_CRITICAL(flags);

	pid = read_c0_entryhi() & ASID_MASK;
	address &= (PAGE_MASK << 1);
	write_c0_entryhi(address | pid);
	pgdp = pgd_offset(vma->vm_mm, address);
	mtc0_tlbw_hazard();
	tlb_probe();
	BARRIER;
	pudp = pud_offset(pgdp, address);
	pmdp = pmd_offset(pudp, address);
	idx = read_c0_index();
	ptep = pte_offset_map(pmdp, address);

#if defined(CONFIG_64BIT_PHYS_ADDR) && defined(CONFIG_CPU_MIPS32_R1)
	write_c0_entrylo0(ptep->pte_high);
	ptep++;
	write_c0_entrylo1(ptep->pte_high);
#else
	write_c0_entrylo0(pte_val(*ptep++) >> 6);
	write_c0_entrylo1(pte_val(*ptep) >> 6);
#endif
	mtc0_tlbw_hazard();
	if (idx < 0)
		tlb_write_random();
	else
		tlb_write_indexed();
	tlbw_use_hazard();
	EXIT_CRITICAL(flags);
}

#if 0
static void r4k_update_mmu_cache_hwbug(struct vm_area_struct * vma,
				       unsigned long address, pte_t pte)
{
	unsigned long flags;
	unsigned int asid;
	pgd_t *pgdp;
	pmd_t *pmdp;
	pte_t *ptep;
	int idx;

	ENTER_CRITICAL(flags);
	address &= (PAGE_MASK << 1);
	asid = read_c0_entryhi() & ASID_MASK;
	write_c0_entryhi(address | asid);
	pgdp = pgd_offset(vma->vm_mm, address);
	mtc0_tlbw_hazard();
	tlb_probe();
	BARRIER;
	pmdp = pmd_offset(pgdp, address);
	idx = read_c0_index();
	ptep = pte_offset_map(pmdp, address);
	write_c0_entrylo0(pte_val(*ptep++) >> 6);
	write_c0_entrylo1(pte_val(*ptep) >> 6);
	mtc0_tlbw_hazard();
	if (idx < 0)
		tlb_write_random();
	else
		tlb_write_indexed();
	tlbw_use_hazard();
	EXIT_CRITICAL(flags);
}
#endif

void __init add_wired_entry(unsigned long entrylo0, unsigned long entrylo1,
	unsigned long entryhi, unsigned long pagemask)
{
	unsigned long flags;
	unsigned long wired;
	unsigned long old_pagemask;
	unsigned long old_ctx;

	ENTER_CRITICAL(flags);
	/* Save old context and create impossible VPN2 value */
	old_ctx = read_c0_entryhi();
	old_pagemask = read_c0_pagemask();
	wired = read_c0_wired();
	write_c0_wired(wired + 1);
	write_c0_index(wired);
	BARRIER;
	write_c0_pagemask(pagemask);
	write_c0_entryhi(entryhi);
	write_c0_entrylo0(entrylo0);
	write_c0_entrylo1(entrylo1);
	mtc0_tlbw_hazard();
	tlb_write_indexed();
	tlbw_use_hazard();

	write_c0_entryhi(old_ctx);
	BARRIER;
	write_c0_pagemask(old_pagemask);
	local_flush_tlb_all();
	EXIT_CRITICAL(flags);
}

/*
 * Used for loading TLB entries before trap_init() has started, when we
 * don't actually want to add a wired entry which remains throughout the
 * lifetime of the system
 */

static int temp_tlb_entry __initdata;

__init int add_temporary_entry(unsigned long entrylo0, unsigned long entrylo1,
			       unsigned long entryhi, unsigned long pagemask)
{
	int ret = 0;
	unsigned long flags;
	unsigned long wired;
	unsigned long old_pagemask;
	unsigned long old_ctx;

	ENTER_CRITICAL(flags);
	/* Save old context and create impossible VPN2 value */
	old_ctx = read_c0_entryhi();
	old_pagemask = read_c0_pagemask();
	wired = read_c0_wired();
	if (--temp_tlb_entry < wired) {
		printk(KERN_WARNING
		       "No TLB space left for add_temporary_entry\n");
		ret = -ENOSPC;
		goto out;
	}

	write_c0_index(temp_tlb_entry);
	write_c0_pagemask(pagemask);
	write_c0_entryhi(entryhi);
	write_c0_entrylo0(entrylo0);
	write_c0_entrylo1(entrylo1);
	mtc0_tlbw_hazard();
	tlb_write_indexed();
	tlbw_use_hazard();

	write_c0_entryhi(old_ctx);
	write_c0_pagemask(old_pagemask);
out:
	EXIT_CRITICAL(flags);
	return ret;
}

static void __init probe_tlb(unsigned long config)
{
	struct cpuinfo_mips *c = &current_cpu_data;
	unsigned int reg;

	/*
	 * If this isn't a MIPS32 / MIPS64 compliant CPU.  Config 1 register
	 * is not supported, we assume R4k style.  Cpu probing already figured
	 * out the number of tlb entries.
	 */
	if ((c->processor_id & 0xff0000) == PRID_COMP_LEGACY)
		return;
#ifdef CONFIG_MIPS_MT_SMTC
	/*
	 * If TLB is shared in SMTC system, total size already
	 * has been calculated and written into cpu_data tlbsize
	 */
	if((smtc_status & SMTC_TLB_SHARED) == SMTC_TLB_SHARED)
		return;
#endif /* CONFIG_MIPS_MT_SMTC */

	reg = read_c0_config1();
	if (!((config >> 7) & 3))
		panic("No TLB present");

	c->tlbsize = ((reg >> 25) & 0x3f) + 1;
}

static int __initdata ntlb = 0;
static int __init set_ntlb(char *str)
{
	get_option(&str, &ntlb);
	return 1;
}

__setup("ntlb=", set_ntlb);

#ifdef CONFIG_MIPS_BRCM97XXX

static bcm_memmap_t bcm_standard_memmap = {
#if defined(CONFIG_MTI_R34K) || defined(CONFIG_BMIPS4380) || \
	defined(CONFIG_BMIPS5000)
		// || def(7440b0) but 7440b0 defines its own bcm_memmap
	.tlb_mask =		PM_64M,
#else
	.tlb_mask =		PM_16M,
#endif
	.pci_vAddr =		PCI_MEM_WIN_BASE,	//0xd0000000, 						
	.pci_winSize =		PCI_MEM_WIN_SIZE, 	/* 128MB for 7440, 3563, 256MB for everybody else */
	.io_vAddr =		PCI_IO_WIN_BASE,		// 0xf0000000,			
	.io_winSize =		PCI_IO_WIN_SIZE,		//0x0060000b, 3563c0 & 7440b0 have 0x20_0000
	.nMemBanks =		1,
	.mem_vAddr =		{0,0xe0000000,0},		// Match array defined by bcm_pdiscontigmem->pAddr[1]
	.tlb_memmap = 	brcm_setup_wired_entries,
	.tlb_pci_bridge =	brcm_setup_pci_bridge,
	.tlb_sata_bridge =	brcm_setup_sata_bridge,
};

bcm_memmap_t* bcm_pmemmap = &bcm_standard_memmap;
EXPORT_SYMBOL(bcm_pmemmap);

#endif

void __init tlb_init(void)
{
	unsigned int config = read_c0_config();

	/*
	 * You should never change this register:
	 *   - On R4600 1.7 the tlbp never hits for pages smaller than
	 *     the value in the c0_pagemask register.
	 *   - The entire mm handling assumes the c0_pagemask register to
	 *     be set for 4kb pages.
	 */
	probe_tlb(config);
	write_c0_pagemask(PM_DEFAULT_MASK);
	write_c0_wired(0);
	write_c0_framemask(0);
	temp_tlb_entry = current_cpu_data.tlbsize - 1;
	local_flush_tlb_all();

#ifdef CONFIG_MIPS_BRCM97XXX
	if (bcm_pmemmap) {
		/* create wired entries in kseg2/kseg3 for PCI and discontig memory */
		bcm_pmemmap->tlb_memmap();

		/* program the PCI bridge registers */
		bcm_pmemmap->tlb_pci_bridge();
	
		if (bcm_pmemmap->tlb_sata_bridge)
		bcm_pmemmap->tlb_sata_bridge();
	}
#endif

	build_tlb_refill_handler();
}
