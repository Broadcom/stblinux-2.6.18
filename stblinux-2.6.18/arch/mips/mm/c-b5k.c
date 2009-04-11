/*
 * c-b5k.c - variant of c-r4k.c for BMIPS5xxx
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
 * Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002 Ralf Baechle (ralf@gnu.org)
 * Copyright (C) 1999, 2000 Silicon Graphics, Inc.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/bitops.h>
#include <linux/module.h>
#include <linux/seq_file.h>

#include <asm/bcache.h>
#include <asm/bootinfo.h>
#include <asm/cache.h>
#include <asm/cacheops.h>
#include <asm/cpu.h>
#include <asm/cpu-features.h>
#include <asm/io.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/r4kcache.h>
#include <asm/system.h>
#include <asm/mmu_context.h>
#include <asm/war.h>
#include <asm/cacheflush.h> /* for run_uncached() */

static unsigned long icache_size __read_mostly;
static unsigned long dcache_size __read_mostly;
static unsigned long scache_size __read_mostly;

#ifdef CONFIG_CACHE_STATS
/*
 * NOTE: would be nice to use atomic64_t here, but it doesn't seem to be
 * available on MIPS32
 */
typedef unsigned long long cachestat_t;

static cachestat_t cache_all_count;		/* local_b5k___flush_cache_all */
static cachestat_t cache_all_cycles;
static cachestat_t cache_mm_count;		/* local_b5k_flush_cache_mm */
static cachestat_t cache_mm_cycles;
static cachestat_t cache_page_count;		/* local_b5k_flush_cache_page */
static cachestat_t cache_page_cycles;
static cachestat_t cache_ipage_count;		/* local_b5k_flush_icache_page */
static cachestat_t cache_ipage_cycles;
static cachestat_t cache_range_count;		/* local_b5k_flush_cache_range */
static cachestat_t cache_range_cycles;
static cachestat_t cache_brange_count;		/* brcm_r4k_flush_cache_range */
static cachestat_t cache_brange_cycles;
static cachestat_t cache_sigtramp_count;	/* local_b5k_flush_cache_sigtramp */
static cachestat_t cache_sigtramp_cycles;
static cachestat_t cache_iflush_count;		/* b5k_flush_icache_all */
static cachestat_t cache_iflush_cycles;
static cachestat_t cache_dpage_count;		/* local_b5k_flush_data_cache_page */
static cachestat_t cache_dpage_cycles;
static cachestat_t cache_irange_count;		/* local_b5k_flush_icache_range */
static cachestat_t cache_irange_cycles;
static cachestat_t cache_dmawb_count;		/* b5k_dma_cache_wback_inv */
static cachestat_t cache_dmawb_cycles;
static cachestat_t cache_dmainv_count;		/* b5k_dma_cache_inv */
static cachestat_t cache_dmainv_cycles;

void cache_printstats(struct seq_file *m)
{
	seq_printf(m, "local_b5k__flush_cache_all      : %-12llu (%llu)\n",
		cache_all_count, cache_all_cycles);
	seq_printf(m, "local_b5k_flush_cache_mm        : %-12llu (%llu)\n",
		cache_mm_count, cache_mm_cycles);
	seq_printf(m, "local_b5k_flush_cache_page      : %-12llu (%llu)\n",
		cache_page_count, cache_page_cycles);
	seq_printf(m, "local_b5k_flush_icache_page     : %-12llu (%llu)\n",
		cache_ipage_count, cache_ipage_cycles);
	seq_printf(m, "local_b5k_flush_cache_range     : %-12llu (%llu)\n",
		cache_range_count, cache_range_cycles);
	seq_printf(m, "brcm_r4k_flush_cache_range      : %-12llu (%llu)\n",
		cache_brange_count, cache_brange_cycles);
	seq_printf(m, "local_b5k_flush_cache_sigtramp  : %-12llu (%llu)\n",
		cache_sigtramp_count, cache_sigtramp_cycles);
	seq_printf(m, "b5k_flush_icache_all            : %-12llu (%llu)\n",
		cache_iflush_count, cache_iflush_cycles);
	seq_printf(m, "local_b5k_flush_data_cache_page : %-12llu (%llu)\n",
		cache_dpage_count, cache_dpage_cycles);
	seq_printf(m, "local_b5k_flush_icache_range    : %-12llu (%llu)\n",
		cache_irange_count, cache_irange_cycles);
	seq_printf(m, "b5k_dma_cache_wback_inv         : %-12llu (%llu)\n",
		cache_dmawb_count, cache_dmawb_cycles);
	seq_printf(m, "b5k_dma_cache_inv               : %-12llu (%llu)\n",
		cache_dmainv_count, cache_dmainv_cycles);
}

#define CACHE_ENTER(x) long long cache_time = read_c0_count()

#define CACHE_EXIT(x)  do { \
	cache_time = read_c0_count() - cache_time; \
	if(cache_time < 0) \
		cache_time += 0x100000000LL; \
	cache_##x##_count++; \
	cache_##x##_cycles += cache_time; \
	} while(0)

#else

#define CACHE_ENTER(x) do { } while(0)
#define CACHE_EXIT(x)  do { } while(0)
	
#endif

static void (* b5k_blast_scache)(void);

/*
 * Dummy cache handling routines for machines without boardcaches
 */
static void cache_noop(void) {}

void brcm_inv_prefetch(unsigned long addr, unsigned long size)
{
	bc_flush_prefetch(addr, size);
}
EXPORT_SYMBOL(brcm_inv_prefetch);

static struct bcache_ops no_sc_ops = {
	.bc_enable = (void *)cache_noop,
	.bc_disable = (void *)cache_noop,
	.bc_wback_inv = (void *)cache_noop,
	.bc_inv = (void *)cache_noop,
	.bc_flush_prefetch = (void *)cache_noop,
};

struct bcache_ops *bcops = &no_sc_ops;

static void (*b5k_blast_dcache_page)(unsigned long addr);

static inline void b5k_blast_dcache_page_dc32(unsigned long addr)
{
	blast_dcache32_page(addr);
}

static inline void b5k_blast_dcache_page_dc64(unsigned long addr)
{
	blast_dcache64_page(addr);
}

static inline void b5k_blast_dcache_page_setup(void)
{
	unsigned long  dc_lsize = cpu_dcache_line_size();

	if (dc_lsize == 0)
		b5k_blast_dcache_page = (void *)cache_noop;
	else if (dc_lsize == 16)
		b5k_blast_dcache_page = blast_dcache16_page;
	else if (dc_lsize == 32)
		b5k_blast_dcache_page = b5k_blast_dcache_page_dc32;
	else if (dc_lsize == 64)
		b5k_blast_dcache_page = b5k_blast_dcache_page_dc64;
}

static void (* b5k_blast_dcache_page_indexed)(unsigned long addr);

static inline void b5k_blast_dcache_page_indexed_setup(void)
{
	unsigned long dc_lsize = cpu_dcache_line_size();

	if (dc_lsize == 0)
		b5k_blast_dcache_page_indexed = (void *)cache_noop;
	else if (dc_lsize == 16)
		b5k_blast_dcache_page_indexed = blast_dcache16_page_indexed;
	else if (dc_lsize == 32)
		b5k_blast_dcache_page_indexed = blast_dcache32_page_indexed;
	else if (dc_lsize == 64)
		b5k_blast_dcache_page_indexed = blast_dcache64_page_indexed;
}

static void (* b5k_blast_dcache)(void);

static inline void b5k_blast_dcache_setup(void)
{
	unsigned long dc_lsize = cpu_dcache_line_size();

	if (dc_lsize == 0)
		b5k_blast_dcache = (void *)cache_noop;
	else if (dc_lsize == 16)
		b5k_blast_dcache = blast_dcache16;
	else if (dc_lsize == 32)
		b5k_blast_dcache = blast_dcache32;
	else if (dc_lsize == 64)
		b5k_blast_dcache = blast_dcache64;
}

static void (* b5k_blast_icache_page)(unsigned long addr);

static inline void b5k_blast_icache_page_setup(void)
{
	unsigned long ic_lsize = cpu_icache_line_size();

	if (ic_lsize == 0)
		b5k_blast_icache_page = (void *)cache_noop;
	else if (ic_lsize == 16)
		b5k_blast_icache_page = blast_icache16_page;
	else if (ic_lsize == 32)
		b5k_blast_icache_page = blast_icache32_page;
	else if (ic_lsize == 64)
		b5k_blast_icache_page = blast_icache64_page;
}


static void (* b5k_blast_icache_page_indexed)(unsigned long addr);

static inline void b5k_blast_icache_page_indexed_setup(void)
{
	unsigned long ic_lsize = cpu_icache_line_size();

	if (ic_lsize == 0)
		b5k_blast_icache_page_indexed = (void *)cache_noop;
	else if (ic_lsize == 16)
		b5k_blast_icache_page_indexed = blast_icache16_page_indexed;
	else if (ic_lsize == 32)
		b5k_blast_icache_page_indexed = blast_icache32_page_indexed;
	else if (ic_lsize == 64)
		b5k_blast_icache_page_indexed = blast_icache64_page_indexed;
}

static void (* b5k_blast_icache)(void);

static inline void b5k_blast_icache_setup(void)
{
	unsigned long ic_lsize = cpu_icache_line_size();

	if (ic_lsize == 0)
		b5k_blast_icache = (void *)cache_noop;
	else if (ic_lsize == 16)
		b5k_blast_icache = blast_icache16;
	else if (ic_lsize == 32)
		b5k_blast_icache = blast_icache32;
	else if (ic_lsize == 64)
		b5k_blast_icache = blast_icache64;
}

static void (* b5k_blast_scache_page)(unsigned long addr);

static inline void b5k_blast_scache_page_setup(void)
{
	unsigned long sc_lsize = cpu_scache_line_size();

	if (scache_size == 0)
		b5k_blast_scache_page = (void *)cache_noop;
	else if (sc_lsize == 16)
		b5k_blast_scache_page = blast_scache16_page;
	else if (sc_lsize == 32)
		b5k_blast_scache_page = blast_scache32_page;
	else if (sc_lsize == 64)
		b5k_blast_scache_page = blast_scache64_page;
	else if (sc_lsize == 128)
		b5k_blast_scache_page = blast_scache128_page;
}

static void (* b5k_blast_scache_page_indexed)(unsigned long addr);

static inline void b5k_blast_scache_page_indexed_setup(void)
{
	unsigned long sc_lsize = cpu_scache_line_size();

	if (scache_size == 0)
		b5k_blast_scache_page_indexed = (void *)cache_noop;
	else if (sc_lsize == 16)
		b5k_blast_scache_page_indexed = blast_scache16_page_indexed;
	else if (sc_lsize == 32)
		b5k_blast_scache_page_indexed = blast_scache32_page_indexed;
	else if (sc_lsize == 64)
		b5k_blast_scache_page_indexed = blast_scache64_page_indexed;
	else if (sc_lsize == 128)
		b5k_blast_scache_page_indexed = blast_scache128_page_indexed;
}

static inline void b5k_blast_scache_setup(void)
{
	unsigned long sc_lsize = cpu_scache_line_size();

	if (scache_size == 0)
		b5k_blast_scache = (void *)cache_noop;
	else if (sc_lsize == 16)
		b5k_blast_scache = blast_scache16;
	else if (sc_lsize == 32)
		b5k_blast_scache = blast_scache32;
	else if (sc_lsize == 64)
		b5k_blast_scache = blast_scache64;
	else if (sc_lsize == 128)
		b5k_blast_scache = blast_scache128;
}

/*
 * This is former mm's flush_cache_all() which really should be
 * flush_cache_vunmap these days ...
 */
static void b5k_flush_cache_all(void)
{
	/* NOTE: this is normally a NOP when !cpu_has_dc_aliases */
	b5k_blast_scache();
}

static void b5k___flush_cache_all(void)
{
	/* L2 flush implicitly flushes L1 */
	CACHE_ENTER(all);
	b5k_blast_scache();
	CACHE_EXIT(all);
}

static void b5k_flush_cache_range(struct vm_area_struct *vma,
	unsigned long start, unsigned long end)
{
	/* I$ and D$ are coherent; b5k has no aliases */
	CACHE_ENTER(range);
	CACHE_EXIT(range);
}


#ifdef CONFIG_MIPS_BRCM97XXX
void brcm_r4k_flush_cache_range(struct vm_area_struct *vma, unsigned long start, unsigned long end)
{
	struct mm_struct *mm = vma->vm_mm;
	unsigned long flags;
	CACHE_ENTER(brange);

#ifndef CONFIG_SMP
	if(mm->context == 0) {
		CACHE_EXIT(brange);
		return;
	}

	start &= PAGE_MASK;
#endif

	if (vma && mm) {		
#ifdef CONFIG_SMP
		if (!(cpu_context(smp_processor_id(), mm))) {
			CACHE_EXIT(brange);
			return;
		}

		if(mm->context == 0) {
			CACHE_EXIT(brange);
			return;
		}

		start &= PAGE_MASK;
#endif
		if((mm->context != current->mm->context) ||
			((end - start) > scache_size)) {
			b5k_blast_scache();
		} else {
			pgd_t *pgd;
			pud_t *pud;
			pmd_t *pmd;
			pte_t *pte;

			local_irq_save(flags);
			while(start < end) {
				pgd = pgd_offset(mm,start);
				pud = pud_offset(pgd, start);
				pmd = pmd_offset(pud, start);
				pte = pte_offset(pmd, start);

				if(pte_val(*pte) & _PAGE_VALID)
					b5k_blast_scache_page(start);
				start += PAGE_SIZE;
			}
			local_irq_restore(flags);
		}
	}

	CACHE_EXIT(brange);
}
#endif // Brcm imple of flush_cache_range

static void b5k_flush_cache_mm(struct mm_struct *mm)
{
	/* No cache aliases - not needed */
	CACHE_ENTER(mm);
	CACHE_EXIT(mm);
}

static void b5k_flush_cache_page(struct vm_area_struct *vma,
	unsigned long addr, unsigned long pfn)
{
	/*
	 * Caches are all coherent between TP0/TP1
	 * I$/D$ are coherent
	 * No aliases
	 */
	CACHE_ENTER(page);
	CACHE_EXIT(page);
	instruction_hazard();
}

static inline void local_b5k_flush_data_cache_page(void * addr)
{
	CACHE_ENTER(dpage);
	CACHE_EXIT(dpage);
}

static void b5k_flush_data_cache_page(unsigned long addr)
{
	local_b5k_flush_data_cache_page(NULL);
}

static void b5k_flush_icache_range(unsigned long start, unsigned long end)
{
	CACHE_ENTER(irange);
	CACHE_EXIT(irange);
	instruction_hazard();
}

static void b5k_flush_icache_page(struct vm_area_struct *vma,
	struct page *page)
{
	CACHE_ENTER(ipage);
	CACHE_EXIT(ipage);
	instruction_hazard();
}


#ifdef CONFIG_DMA_NONCOHERENT

static void b5k_dma_cache_wback_inv(unsigned long addr, unsigned long size)
{
	CACHE_ENTER(dmawb);
	/* Catch bad driver code */
	BUG_ON(size == 0);

	__sync();

	/* L2 flush implicitly flushes L1 */
	if (size >= scache_size)
		b5k_blast_scache();
	else
		bc_wback_inv(addr, size);
	CACHE_EXIT(dmawb);
}

static void b5k_dma_cache_inv(unsigned long addr, unsigned long size)
{
	b5k_dma_cache_wback_inv(addr, size);
}
#endif /* CONFIG_DMA_NONCOHERENT */

static void b5k_flush_cache_sigtramp(unsigned long addr)
{
	CACHE_ENTER(sigtramp);
	CACHE_EXIT(sigtramp);
}

static void b5k_flush_icache_all(void)
{
	CACHE_ENTER(iflush);
	CACHE_EXIT(iflush);
}

static char *way_string[] __initdata = { NULL, "direct mapped", "2-way",
	"3-way", "4-way", "5-way", "6-way", "7-way", "8-way"
};

static void __init probe_pcache(void)
{
	struct cpuinfo_mips *c = &current_cpu_data;
	unsigned int config = read_c0_config();
	unsigned long config1;
	unsigned int lsize;

	switch (c->cputype) {
	default:
		if (!(config & MIPS_CONF_M))
			panic("Don't know how to probe P-caches on this cpu.");

		/*
		 * So we seem to be a MIPS32 or MIPS64 CPU
		 * So let's probe the I-cache ...
		 */
		config1 = read_c0_config1();

		if ((lsize = ((config1 >> 19) & 7)))
			c->icache.linesz = 2 << lsize;
		else
			c->icache.linesz = lsize;
		c->icache.sets = 64 << ((config1 >> 22) & 7);
		c->icache.ways = 1 + ((config1 >> 16) & 7);

		icache_size = c->icache.sets *
		              c->icache.ways *
		              c->icache.linesz;
		c->icache.waybit = __ffs(icache_size/c->icache.ways);

		if (config & 0x8)		/* VI bit */
			c->icache.flags |= MIPS_CACHE_VTAG;

		/*
		 * Now probe the MIPS32 / MIPS64 data cache.
		 */
		c->dcache.flags = 0;

		if ((lsize = ((config1 >> 10) & 7)))
			c->dcache.linesz = 2 << lsize;
		else
			c->dcache.linesz= lsize;
		c->dcache.sets = 64 << ((config1 >> 13) & 7);
		c->dcache.ways = 1 + ((config1 >> 7) & 7);

		dcache_size = c->dcache.sets *
		              c->dcache.ways *
		              c->dcache.linesz;
		c->dcache.waybit = __ffs(dcache_size/c->dcache.ways);

		c->options |= MIPS_CPU_PREFETCH;
		break;
	}

	/* compute a couple of other cache variables */
	c->icache.waysize = icache_size / c->icache.ways;
	c->dcache.waysize = dcache_size / c->dcache.ways;

	c->icache.sets = c->icache.linesz ?
		icache_size / (c->icache.linesz * c->icache.ways) : 0;
	c->dcache.sets = c->dcache.linesz ?
		dcache_size / (c->dcache.linesz * c->dcache.ways) : 0;

	printk("Primary instruction cache %ldkB, %s, %s, linesize %d bytes.\n",
	       icache_size >> 10,
	       cpu_has_vtag_icache ? "virtually tagged" : "physically tagged",
	       way_string[c->icache.ways], c->icache.linesz);

	printk("Primary data cache %ldkB, %s, linesize %d bytes.\n",
	       dcache_size >> 10, way_string[c->dcache.ways], c->dcache.linesz);
}

extern int mips_sc_init(void);

static void __init setup_scache(void)
{
	struct cpuinfo_mips *c = &current_cpu_data;

	if (mips_sc_init ()) {
		scache_size = c->scache.ways * c->scache.sets * c->scache.linesz;
		printk("MIPS secondary cache %ldkB, %s, linesize %d bytes.\n",
		       scache_size >> 10,
		       way_string[c->scache.ways], c->scache.linesz);
	} else {
		panic("c-b5k cache functions require L2 enabled");
	}
	return;
}

void __init b5k_cache_init(void)
{
	extern void build_clear_page(void);
	extern void build_copy_page(void);
	extern char except_vec2_generic;

	/* Default cache error handler for R4000 and R5000 family */
	set_uncached_handler (0x100, &except_vec2_generic, 0x80);

	probe_pcache();
	setup_scache();

	b5k_blast_dcache_page_setup();
	b5k_blast_dcache_page_indexed_setup();
	b5k_blast_dcache_setup();
	b5k_blast_icache_page_setup();
	b5k_blast_icache_page_indexed_setup();
	b5k_blast_icache_setup();
	b5k_blast_scache_page_setup();
	b5k_blast_scache_page_indexed_setup();
	b5k_blast_scache_setup();

	shm_align_mask = PAGE_SIZE-1;

	flush_cache_all		= b5k_flush_cache_all;
	__flush_cache_all	= b5k___flush_cache_all;
	flush_cache_mm		= b5k_flush_cache_mm;
	flush_cache_page	= b5k_flush_cache_page;
	__flush_icache_page	= b5k_flush_icache_page;
	flush_cache_range	= b5k_flush_cache_range;

	flush_cache_sigtramp	= b5k_flush_cache_sigtramp;
	flush_icache_all	= b5k_flush_icache_all;
	local_flush_data_cache_page	= local_b5k_flush_data_cache_page;
	flush_data_cache_page	= b5k_flush_data_cache_page;
	flush_icache_range	= b5k_flush_icache_range;

#ifdef CONFIG_DMA_NONCOHERENT
	_dma_cache_wback_inv	= b5k_dma_cache_wback_inv;
	_dma_cache_wback	= b5k_dma_cache_wback_inv;
	_dma_cache_inv		= b5k_dma_cache_inv;
#endif

	build_clear_page();
	build_copy_page();
	b5k___flush_cache_all();
}
