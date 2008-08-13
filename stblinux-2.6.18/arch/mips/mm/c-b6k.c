/*
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

/*
 * Special Variant of smp_call_function for use by cache functions:
 *
 *  o No return value
 *  o collapses to normal function call on UP kernels
 *  o collapses to normal function call on systems with a single shared
 *    primary cache.
 */
static inline void r4k_on_each_cpu(void (*func) (void *info), void *info,
                                   int retry, int wait)
{
	preempt_disable();

#if !defined(CONFIG_MIPS_MT_SMP) && !defined(CONFIG_MIPS_MT_SMTC)
	smp_call_function(func, info, retry, wait);
#endif
	func(info);
	preempt_enable();
}

static unsigned long icache_size __read_mostly;
static unsigned long dcache_size __read_mostly;
static unsigned long scache_size __read_mostly;

#ifdef CONFIG_MIPS_BRCM97XXX
static unsigned long icache_size, dcache_size, scache_size;

/* RYH */
static int rac_set = 0;
extern int par_val;
extern char cfeBootParms[]; 
extern unsigned long rac_config0, rac_config1, rac_address_range;
#endif	// CONFIG_MIPS_BRCM97XXX

#if defined (CONFIG_SMP) && \
   (defined (CONFIG_MIPS_BCM7400) || defined (CONFIG_MIPS_BCM7440A0) || \
    defined (CONFIG_MIPS_BCM7405) || defined (CONFIG_MIPS_BCM7335) || \
    defined (CONFIG_MIPS_BCM3548))
/*
 * PR36773:
 * BRCM_CMT_CACHE_WAR_0 controls D$-only flush.  Should never be global.
 * BRCM_CMT_CACHE_WAR_1 controls mixed I$/D$ or I$ only.  Generally a good
 *   idea to make these global since I$ is not shared on BMIPS43xx.
 *
 * If defined, the flush is local.  If not defined, it is global.
 */
#define BRCM_CMT_CACHE_WAR_0
// #define BRCM_CMT_CACHE_WAR_1
#endif

#ifdef CONFIG_CACHE_STATS
/*
 * NOTE: would be nice to use atomic64_t here, but it doesn't seem to be
 * available on MIPS32
 */
typedef unsigned long long cachestat_t;

static cachestat_t cache_all_count;		/* local_r4k___flush_cache_all */
static cachestat_t cache_all_cycles;
static cachestat_t cache_mm_count;		/* local_r4k_flush_cache_mm */
static cachestat_t cache_mm_cycles;
static cachestat_t cache_page_count;		/* local_r4k_flush_cache_page */
static cachestat_t cache_page_cycles;
static cachestat_t cache_ipage_count;		/* local_r4k_flush_icache_page */
static cachestat_t cache_ipage_cycles;
static cachestat_t cache_range_count;		/* local_r4k_flush_cache_range */
static cachestat_t cache_range_cycles;
static cachestat_t cache_brange_count;		/* brcm_r4k_flush_cache_range */
static cachestat_t cache_brange_cycles;
static cachestat_t cache_sigtramp_count;	/* local_r4k_flush_cache_sigtramp */
static cachestat_t cache_sigtramp_cycles;
static cachestat_t cache_iflush_count;		/* r4k_flush_icache_all */
static cachestat_t cache_iflush_cycles;
static cachestat_t cache_dpage_count;		/* local_r4k_flush_data_cache_page */
static cachestat_t cache_dpage_cycles;
static cachestat_t cache_irange_count;		/* local_r4k_flush_icache_range */
static cachestat_t cache_irange_cycles;
static cachestat_t cache_dmawb_count;		/* r4k_dma_cache_wback_inv */
static cachestat_t cache_dmawb_cycles;
static cachestat_t cache_dmainv_count;		/* r4k_dma_cache_inv */
static cachestat_t cache_dmainv_cycles;

void cache_printstats(struct seq_file *m)
{
	seq_printf(m, "local_r4k__flush_cache_all      : %-12llu (%llu)\n",
		cache_all_count, cache_all_cycles);
	seq_printf(m, "local_r4k_flush_cache_mm        : %-12llu (%llu)\n",
		cache_mm_count, cache_mm_cycles);
	seq_printf(m, "local_r4k_flush_cache_page      : %-12llu (%llu)\n",
		cache_page_count, cache_page_cycles);
	seq_printf(m, "local_r4k_flush_icache_page     : %-12llu (%llu)\n",
		cache_ipage_count, cache_ipage_cycles);
	seq_printf(m, "local_r4k_flush_cache_range     : %-12llu (%llu)\n",
		cache_range_count, cache_range_cycles);
	seq_printf(m, "brcm_r4k_flush_cache_range      : %-12llu (%llu)\n",
		cache_brange_count, cache_brange_cycles);
	seq_printf(m, "local_r4k_flush_cache_sigtramp  : %-12llu (%llu)\n",
		cache_sigtramp_count, cache_sigtramp_cycles);
	seq_printf(m, "r4k_flush_icache_all            : %-12llu (%llu)\n",
		cache_iflush_count, cache_iflush_cycles);
	seq_printf(m, "local_r4k_flush_data_cache_page : %-12llu (%llu)\n",
		cache_dpage_count, cache_dpage_cycles);
	seq_printf(m, "local_r4k_flush_icache_range    : %-12llu (%llu)\n",
		cache_irange_count, cache_irange_cycles);
	seq_printf(m, "r4k_dma_cache_wback_inv         : %-12llu (%llu)\n",
		cache_dmawb_count, cache_dmawb_cycles);
	seq_printf(m, "r4k_dma_cache_inv               : %-12llu (%llu)\n",
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

static void (* r4k_blast_scache)(void);

#ifdef CONFIG_MIPS_BRCM97XXX

static inline
void bcm_local_inv_rac_all(void)
{

#if defined( CONFIG_MIPS_BCM7115 ) || defined( CONFIG_MIPS_BCM7112 ) ||\
      defined( CONFIG_MIPS_BCM7315 ) || defined( CONFIG_MIPS_BCM7317 ) ||\
      defined( CONFIG_MIPS_BCM7314 ) || defined( CONFIG_MIPS_BCM7318 ) ||\
      defined( CONFIG_MIPS_BCM7110 ) || defined( CONFIG_MIPS_BCM7111 )
	/* If RAC is Enabled (RAC_EN) */
	if (*((volatile unsigned long *)0xffe00404) & 0x08)
	{
		unsigned long flags;

		local_irq_save(flags);
		*((volatile unsigned long *)0xffe00400) |= 0x02;
		while ( (*((volatile unsigned long *)0xffe00400) & 0x02) == 0);
		local_irq_restore(flags);
	}
	/* else do nothing */
#elif defined (CONFIG_MIPS_BCM7320) || defined (CONFIG_MIPS_BCM7319) \
	|| defined(CONFIG_MIPS_BCM7328)
	if (*((volatile unsigned long *)0xbae00404) & 0x08)
	{
		unsigned long flags;

		local_irq_save(flags);
		*((volatile unsigned long *)0xbae00400) |= 0x02;
		while ( (*((volatile unsigned long *)0xbae00400) & 0x02) == 0);
		local_irq_restore(flags);
	}
	/* else do nothing */
#elif defined (CONFIG_MIPS_BCM3560) || defined (CONFIG_MIPS_BCM7401) \
	|| defined (CONFIG_MIPS_BCM7402) || defined(CONFIG_MIPS_BCM7118) \
	|| defined (CONFIG_MIPS_BCM7403)
	if (*((volatile unsigned long *)0xff400000) & 0x02)	/* RYH - check RAC_D bit in RAC Config Register */
	{
		unsigned long flags;

		local_irq_save(flags);
		*((volatile unsigned long *)0xff400000) |= 0x100;    /* RYH - set FLH bit in RAC Config Register */
		local_irq_restore(flags);
	}

#elif defined (CONFIG_MIPS_BCM7400) || defined (CONFIG_MIPS_BCM7440) \
	|| defined (CONFIG_MIPS_BCM7405) || defined(CONFIG_MIPS_BCM7335) \
	|| defined (CONFIG_MIPS_BCM3548)
#ifndef CONFIG_SMP
	if (*((volatile unsigned long *)rac_config0) & 0x02)	/* check RAC_D bit in RAC Config Register for TP0 */
#else
	if ((*((volatile unsigned long *)rac_config0) & 0x02) || (*((volatile unsigned long *)rac_config1) & 0x02))
#endif
	{
		unsigned long flags;

		local_irq_save(flags);
		*((volatile unsigned long *)rac_config0) |= 0x100;    /*  set FLH bit in RAC Config Register */
		local_irq_restore(flags);
	}

/* RAC not implemented in 7038A0 */
#elif defined(CONFIG_MIPS_BCM7038B0) || defined(CONFIG_MIPS_BCM7038C0)
	{
		unsigned long flags;

		if (*((volatile unsigned long *)0xb0000404) & 0x10)
		{
			local_irq_save(flags);
			*((volatile unsigned long *)0xb000040c) = 0x01;
			mb();
			while( (*((volatile unsigned long *)0xb0000424) & 0xffff) != 0);
			*((volatile unsigned long *)0xb000040c) = 0x00;
			local_irq_restore(flags);
		}

	}
#endif
}

void bcm_inv_rac_all(void)
{
#if defined(CONFIG_MIPS_BCM7325) || defined( CONFIG_MIPS_BCM7440B0 ) \
	|| defined( CONFIG_MIPS_BCM7443 )
	/* 7325 L2 supports prefetching */
#ifdef CONFIG_MIPS_BRCM_SCACHE
	r4k_blast_scache();
#endif
	__sync();
#else
	bcm_local_inv_rac_all();
#endif
}
EXPORT_SYMBOL(bcm_inv_rac_all);

#define MIPS_24K_CACHEOP_WAR_IMPL       __sync()

#if defined( CONFIG_MIPS_BCM7440B0 ) || defined( CONFIG_MIPS_BCM7325 ) \
	|| defined( CONFIG_MIPS_BCM7443A0 ) 
#define BCM_LOCAL_EXTRA_CACHEOP_WAR \
        MIPS_24K_CACHEOP_WAR_IMPL

        // TBD: Only certain platforms like the 7400, 7038Bx,Cx need an explicit
        // RAC cache flush to be done after a regular flush
#else
#define BCM_LOCAL_EXTRA_CACHEOP_WAR bcm_local_inv_rac_all()
#endif

#else // Non Broadcom chip

#define BCM_LOCAL_EXTRA_CACHEOP_WAR
void bcm_inv_rac_all(void) { }

#endif // CONFIG_MIPS_BRCM97XXX

unsigned long
get_dcache_size(void)
{
	return dcache_size;
}


/*
 * Dummy cache handling routines for machines without boardcaches
 */
static void cache_noop(void) {}

static struct bcache_ops no_sc_ops = {
	.bc_enable = (void *)cache_noop,
	.bc_disable = (void *)cache_noop,
	.bc_wback_inv = (void *)cache_noop,
	.bc_inv = (void *)cache_noop
};

struct bcache_ops *bcops = &no_sc_ops;

#define cpu_is_r4600_v1_x()	((read_c0_prid() & 0xfffffff0) == 0x00002010)
#define cpu_is_r4600_v2_x()	((read_c0_prid() & 0xfffffff0) == 0x00002020)

#define R4600_HIT_CACHEOP_WAR_IMPL					\
do {									\
	if (R4600_V2_HIT_CACHEOP_WAR && cpu_is_r4600_v2_x())		\
		*(volatile unsigned long *)CKSEG1;			\
	if (R4600_V1_HIT_CACHEOP_WAR)					\
		__asm__ __volatile__("nop;nop;nop;nop");		\
} while (0)

static void (*r4k_blast_dcache_page)(unsigned long addr);

static inline void r4k_blast_dcache_page_dc32(unsigned long addr)
{
	R4600_HIT_CACHEOP_WAR_IMPL;
	blast_dcache32_page(addr);
}

static inline void r4k_blast_dcache_page_dc64(unsigned long addr)
{
	R4600_HIT_CACHEOP_WAR_IMPL;
	blast_dcache64_page(addr);
}

static inline void r4k_blast_dcache_page_setup(void)
{
	unsigned long  dc_lsize = cpu_dcache_line_size();

	if (dc_lsize == 0)
		r4k_blast_dcache_page = (void *)cache_noop;
	else if (dc_lsize == 16)
		r4k_blast_dcache_page = blast_dcache16_page;
	else if (dc_lsize == 32)
		r4k_blast_dcache_page = r4k_blast_dcache_page_dc32;
	else if (dc_lsize == 64)
		r4k_blast_dcache_page = r4k_blast_dcache_page_dc64;
}

static void (* r4k_blast_dcache_page_indexed)(unsigned long addr);

static inline void r4k_blast_dcache_page_indexed_setup(void)
{
	unsigned long dc_lsize = cpu_dcache_line_size();

	if (dc_lsize == 0)
		r4k_blast_dcache_page_indexed = (void *)cache_noop;
	else if (dc_lsize == 16)
		r4k_blast_dcache_page_indexed = blast_dcache16_page_indexed;
	else if (dc_lsize == 32)
		r4k_blast_dcache_page_indexed = blast_dcache32_page_indexed;
	else if (dc_lsize == 64)
		r4k_blast_dcache_page_indexed = blast_dcache64_page_indexed;
}

static void (* r4k_blast_dcache)(void);

static inline void r4k_blast_dcache_setup(void)
{
	unsigned long dc_lsize = cpu_dcache_line_size();

	if (dc_lsize == 0)
		r4k_blast_dcache = (void *)cache_noop;
	else if (dc_lsize == 16)
		r4k_blast_dcache = blast_dcache16;
	else if (dc_lsize == 32)
		r4k_blast_dcache = blast_dcache32;
	else if (dc_lsize == 64)
		r4k_blast_dcache = blast_dcache64;
}

/* force code alignment (used for TX49XX_ICACHE_INDEX_INV_WAR) */
#define JUMP_TO_ALIGN(order) \
	__asm__ __volatile__( \
		"b\t1f\n\t" \
		".align\t" #order "\n\t" \
		"1:\n\t" \
		)
#define CACHE32_UNROLL32_ALIGN	JUMP_TO_ALIGN(10) /* 32 * 32 = 1024 */
#define CACHE32_UNROLL32_ALIGN2	JUMP_TO_ALIGN(11)

static inline void blast_r4600_v1_icache32(void)
{
	unsigned long flags;

	local_irq_save(flags);
	blast_icache32();
	local_irq_restore(flags);
}

static inline void tx49_blast_icache32(void)
{
	unsigned long start = INDEX_BASE;
	unsigned long end = start + current_cpu_data.icache.waysize;
	unsigned long ws_inc = 1UL << current_cpu_data.icache.waybit;
	unsigned long ws_end = current_cpu_data.icache.ways <<
	                       current_cpu_data.icache.waybit;
	unsigned long ws, addr;

	CACHE32_UNROLL32_ALIGN2;
	/* I'm in even chunk.  blast odd chunks */
	for (ws = 0; ws < ws_end; ws += ws_inc)
		for (addr = start + 0x400; addr < end; addr += 0x400 * 2)
			cache32_unroll32(addr|ws,Index_Invalidate_I);
	CACHE32_UNROLL32_ALIGN;
	/* I'm in odd chunk.  blast even chunks */
	for (ws = 0; ws < ws_end; ws += ws_inc)
		for (addr = start; addr < end; addr += 0x400 * 2)
			cache32_unroll32(addr|ws,Index_Invalidate_I);
}

static inline void blast_icache32_r4600_v1_page_indexed(unsigned long page)
{
	unsigned long flags;

	local_irq_save(flags);
	blast_icache32_page_indexed(page);
	local_irq_restore(flags);
}

static inline void tx49_blast_icache32_page_indexed(unsigned long page)
{
	unsigned long indexmask = current_cpu_data.icache.waysize - 1;
	unsigned long start = INDEX_BASE + (page & indexmask);
	unsigned long end = start + PAGE_SIZE;
	unsigned long ws_inc = 1UL << current_cpu_data.icache.waybit;
	unsigned long ws_end = current_cpu_data.icache.ways <<
	                       current_cpu_data.icache.waybit;
	unsigned long ws, addr;

	CACHE32_UNROLL32_ALIGN2;
	/* I'm in even chunk.  blast odd chunks */
	for (ws = 0; ws < ws_end; ws += ws_inc)
		for (addr = start + 0x400; addr < end; addr += 0x400 * 2)
			cache32_unroll32(addr|ws,Index_Invalidate_I);
	CACHE32_UNROLL32_ALIGN;
	/* I'm in odd chunk.  blast even chunks */
	for (ws = 0; ws < ws_end; ws += ws_inc)
		for (addr = start; addr < end; addr += 0x400 * 2)
			cache32_unroll32(addr|ws,Index_Invalidate_I);
}

static void (* r4k_blast_icache_page)(unsigned long addr);

static inline void r4k_blast_icache_page_setup(void)
{
	unsigned long ic_lsize = cpu_icache_line_size();

	if (ic_lsize == 0)
		r4k_blast_icache_page = (void *)cache_noop;
	else if (ic_lsize == 16)
		r4k_blast_icache_page = blast_icache16_page;
	else if (ic_lsize == 32)
		r4k_blast_icache_page = blast_icache32_page;
	else if (ic_lsize == 64)
		r4k_blast_icache_page = blast_icache64_page;
}


static void (* r4k_blast_icache_page_indexed)(unsigned long addr);

static inline void r4k_blast_icache_page_indexed_setup(void)
{
	unsigned long ic_lsize = cpu_icache_line_size();

	if (ic_lsize == 0)
		r4k_blast_icache_page_indexed = (void *)cache_noop;
	else if (ic_lsize == 16)
		r4k_blast_icache_page_indexed = blast_icache16_page_indexed;
	else if (ic_lsize == 32) {
		if (R4600_V1_INDEX_ICACHEOP_WAR && cpu_is_r4600_v1_x())
			r4k_blast_icache_page_indexed =
				blast_icache32_r4600_v1_page_indexed;
		else if (TX49XX_ICACHE_INDEX_INV_WAR)
			r4k_blast_icache_page_indexed =
				tx49_blast_icache32_page_indexed;
		else
			r4k_blast_icache_page_indexed =
				blast_icache32_page_indexed;
	} else if (ic_lsize == 64)
		r4k_blast_icache_page_indexed = blast_icache64_page_indexed;
}

static void (* r4k_blast_icache)(void);

static inline void r4k_blast_icache_setup(void)
{
	unsigned long ic_lsize = cpu_icache_line_size();

	if (ic_lsize == 0)
		r4k_blast_icache = (void *)cache_noop;
	else if (ic_lsize == 16)
		r4k_blast_icache = blast_icache16;
	else if (ic_lsize == 32) {
		if (R4600_V1_INDEX_ICACHEOP_WAR && cpu_is_r4600_v1_x())
			r4k_blast_icache = blast_r4600_v1_icache32;
		else if (TX49XX_ICACHE_INDEX_INV_WAR)
			r4k_blast_icache = tx49_blast_icache32;
		else
			r4k_blast_icache = blast_icache32;
	} else if (ic_lsize == 64)
		r4k_blast_icache = blast_icache64;
}

static void (* r4k_blast_scache_page)(unsigned long addr);

static inline void r4k_blast_scache_page_setup(void)
{
	unsigned long sc_lsize = cpu_scache_line_size();

	if (scache_size == 0)
		r4k_blast_scache_page = (void *)cache_noop;
	else if (sc_lsize == 16)
		r4k_blast_scache_page = blast_scache16_page;
	else if (sc_lsize == 32)
		r4k_blast_scache_page = blast_scache32_page;
	else if (sc_lsize == 64)
		r4k_blast_scache_page = blast_scache64_page;
	else if (sc_lsize == 128)
		r4k_blast_scache_page = blast_scache128_page;
}

static void (* r4k_blast_scache_page_indexed)(unsigned long addr);

static inline void r4k_blast_scache_page_indexed_setup(void)
{
	unsigned long sc_lsize = cpu_scache_line_size();

	if (scache_size == 0)
		r4k_blast_scache_page_indexed = (void *)cache_noop;
	else if (sc_lsize == 16)
		r4k_blast_scache_page_indexed = blast_scache16_page_indexed;
	else if (sc_lsize == 32)
		r4k_blast_scache_page_indexed = blast_scache32_page_indexed;
	else if (sc_lsize == 64)
		r4k_blast_scache_page_indexed = blast_scache64_page_indexed;
	else if (sc_lsize == 128)
		r4k_blast_scache_page_indexed = blast_scache128_page_indexed;
}

static inline void r4k_blast_scache_setup(void)
{
	unsigned long sc_lsize = cpu_scache_line_size();

	if (scache_size == 0)
		r4k_blast_scache = (void *)cache_noop;
	else if (sc_lsize == 16)
		r4k_blast_scache = blast_scache16;
	else if (sc_lsize == 32)
		r4k_blast_scache = blast_scache32;
	else if (sc_lsize == 64)
		r4k_blast_scache = blast_scache64;
	else if (sc_lsize == 128)
		r4k_blast_scache = blast_scache128;
}

/*
 * This is former mm's flush_cache_all() which really should be
 * flush_cache_vunmap these days ...
 */
static inline void local_r4k_flush_cache_all(void * args)
{
	r4k_blast_dcache();
	r4k_blast_icache();

	BCM_LOCAL_EXTRA_CACHEOP_WAR;
}

static void r4k_flush_cache_all(void)
{
	if (!cpu_has_dc_aliases)
		return;

#ifdef BRCM_CMT_CACHE_WAR_1
	local_r4k_flush_cache_all(NULL);
#else
	r4k_on_each_cpu(local_r4k_flush_cache_all, NULL, 1, 1);
#endif
}

static inline void local_r4k___flush_cache_all(void * args)
{
	CACHE_ENTER(all);
	r4k_blast_dcache();
	r4k_blast_icache();

	switch (current_cpu_data.cputype) {
	case CPU_R4000SC:
	case CPU_R4000MC:
	case CPU_R4400SC:
	case CPU_R4400MC:
	case CPU_R10000:
	case CPU_R12000:
	case CPU_R14000:
	case CPU_34K:
		r4k_blast_scache();
	}
	CACHE_EXIT(all);
}

static void r4k___flush_cache_all(void)
{
#ifdef BRCM_CMT_CACHE_WAR_1
	local_r4k___flush_cache_all(NULL);
#else
	r4k_on_each_cpu(local_r4k___flush_cache_all, NULL, 1, 1);
#endif
}

static inline void local_r4k_flush_cache_range(void * args)
{
	struct vm_area_struct *vma = args;
	int exec;
	CACHE_ENTER(range);

	if (!(cpu_context(smp_processor_id(), vma->vm_mm))) {
		CACHE_EXIT(range);
		return;
	}

	exec = vma->vm_flags & VM_EXEC;
	if (cpu_has_dc_aliases || exec)
		r4k_blast_dcache();
	if (exec)
		r4k_blast_icache();
	BCM_LOCAL_EXTRA_CACHEOP_WAR;
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
#ifdef DEBUG_CACHE
	printk("brcm_r4k_flush_cache_range[%d,%08lx,%08lx]\n", (int)mm->context, start, end);
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
		if(mm->context != current->mm->context) {
			r4k_flush_cache_all();
		} 
		else if ((end - start) > get_dcache_size()) {
			r4k_blast_dcache();
		}
		else {
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
					r4k_blast_dcache_page(start);
				start += PAGE_SIZE;
			}
			local_irq_restore(flags);
		}
	}
#if defined(CONFIG_MIPS_BRCM_SCACHE)
	r4k_blast_scache();
#endif
	BCM_LOCAL_EXTRA_CACHEOP_WAR;
	CACHE_EXIT(brange);
}
#endif // Brcm imple of flush_cache_range

static void r4k_flush_cache_range(struct vm_area_struct *vma,
	unsigned long start, unsigned long end)
{
#ifdef BRCM_CMT_CACHE_WAR_1
	local_r4k_flush_cache_range(vma);
#else
	r4k_on_each_cpu(local_r4k_flush_cache_range, vma, 1, 1);
#endif
}

static inline void local_r4k_flush_cache_mm(void * args)
{
	struct mm_struct *mm = args;
	CACHE_ENTER(mm);

	if (!cpu_context(smp_processor_id(), mm))
	{
		CACHE_EXIT(mm);
		return;
	}

	r4k_blast_dcache();
	r4k_blast_icache();

	/*
	 * Kludge alert.  For obscure reasons R4000SC and R4400SC go nuts if we
	 * only flush the primary caches but R10000 and R12000 behave sane ...
	 */
	if (current_cpu_data.cputype == CPU_R4000SC ||
	    current_cpu_data.cputype == CPU_R4000MC ||
	    current_cpu_data.cputype == CPU_R4400SC ||
	    current_cpu_data.cputype == CPU_R4400MC)
		r4k_blast_scache();

	BCM_LOCAL_EXTRA_CACHEOP_WAR;
	CACHE_EXIT(mm);
}

static void r4k_flush_cache_mm(struct mm_struct *mm)
{
	if (!cpu_has_dc_aliases)
		return;

#ifdef BRCM_CMT_CACHE_WAR_1
	local_r4k_flush_cache_mm(mm);
#else
	r4k_on_each_cpu(local_r4k_flush_cache_mm, mm, 1, 1);
#endif
}

struct flush_cache_page_args {
	struct vm_area_struct *vma;
	unsigned long addr;
	unsigned long pfn;
};

static inline void local_r4k_flush_cache_page(void *args)
{
	struct flush_cache_page_args *fcp_args = args;
	struct vm_area_struct *vma = fcp_args->vma;
	unsigned long addr = fcp_args->addr;
	unsigned long paddr = fcp_args->pfn << PAGE_SHIFT;
	int exec = vma->vm_flags & VM_EXEC;
	struct mm_struct *mm = vma->vm_mm;
	pgd_t *pgdp;
	pud_t *pudp;
	pmd_t *pmdp;
	pte_t *ptep;
	CACHE_ENTER(page);

	/*
	 * If ownes no valid ASID yet, cannot possibly have gotten
	 * this page into the cache.
	 */
	if (cpu_context(smp_processor_id(), mm) == 0) {
		CACHE_EXIT(page);
		return;
	}

	addr &= PAGE_MASK;
	pgdp = pgd_offset(mm, addr);
	pudp = pud_offset(pgdp, addr);
	pmdp = pmd_offset(pudp, addr);
	ptep = pte_offset(pmdp, addr);

	/*
	 * If the page isn't marked valid, the page cannot possibly be
	 * in the cache.
	 */
	if (!(pte_val(*ptep) & _PAGE_PRESENT)) {
		CACHE_EXIT(page);
		return;
	}

	/*
	 * Doing flushes for another ASID than the current one is
	 * too difficult since stupid R4k caches do a TLB translation
	 * for every cache flush operation.  So we do indexed flushes
	 * in that case, which doesn't overly flush the cache too much.
	 */
	if ((mm == current->active_mm) && (pte_val(*ptep) & _PAGE_VALID)) {
		if (cpu_has_dc_aliases || (exec && !cpu_has_ic_fills_f_dc)) {
			r4k_blast_dcache_page(addr);
			if (exec && !cpu_icache_snoops_remote_store)
				r4k_blast_scache_page(addr);
		}
		if (exec)
			r4k_blast_icache_page(addr);

		BCM_LOCAL_EXTRA_CACHEOP_WAR;
		CACHE_EXIT(page);
		return;
	}

	/*
	 * Do indexed flush, too much work to get the (possible) TLB refills
	 * to work correctly.
	 */
	if (cpu_has_dc_aliases || (exec && !cpu_has_ic_fills_f_dc)) {
		r4k_blast_dcache_page_indexed(cpu_has_pindexed_dcache ?
					      paddr : addr);
		if (exec && !cpu_icache_snoops_remote_store) {
			r4k_blast_scache_page_indexed(paddr);
		}
	}
	if (exec) {
		if (cpu_has_vtag_icache && mm == current->active_mm) {
			int cpu = smp_processor_id();

			if (cpu_context(cpu, mm) != 0)
				drop_mmu_context(mm, cpu);
		} else
			r4k_blast_icache_page_indexed(addr);
	}
	BCM_LOCAL_EXTRA_CACHEOP_WAR;
	CACHE_EXIT(page);
}

static void r4k_flush_cache_page(struct vm_area_struct *vma,
	unsigned long addr, unsigned long pfn)
{
	struct flush_cache_page_args args;

	args.vma = vma;
	args.addr = addr;
	args.pfn = pfn;


#ifdef BRCM_CMT_CACHE_WAR_1
	local_r4k_flush_cache_page(&args);
#else
	r4k_on_each_cpu(local_r4k_flush_cache_page, &args, 1, 1);
#endif
}

static inline void local_r4k_flush_data_cache_page(void * addr)
{
	CACHE_ENTER(dpage);
	r4k_blast_dcache_page((unsigned long) addr);
	CACHE_EXIT(dpage);
}

static void r4k_flush_data_cache_page(unsigned long addr)
{
#ifdef BRCM_CMT_CACHE_WAR_0
	local_r4k_flush_data_cache_page((void *) addr);
#else
	r4k_on_each_cpu(local_r4k_flush_data_cache_page, (void *) addr, 1, 1);
#endif
}

struct flush_icache_range_args {
	unsigned long start;
	unsigned long end;
};

static inline void local_r4k_flush_icache_range(void *args)
{
	struct flush_icache_range_args *fir_args = args;
	unsigned long start = fir_args->start;
	unsigned long end = fir_args->end;
	CACHE_ENTER(irange);

	if (!cpu_has_ic_fills_f_dc) {
		if (end - start >= dcache_size) {
			r4k_blast_dcache();
		} else {
			R4600_HIT_CACHEOP_WAR_IMPL;
			protected_blast_dcache_range(start, end);
		}

		if (!cpu_icache_snoops_remote_store && scache_size) {
			if (end - start > scache_size)
				r4k_blast_scache();
			else
				protected_blast_scache_range(start, end);
		}
	}

	if (end - start > icache_size)
		r4k_blast_icache();
	else
		protected_blast_icache_range(start, end);
	CACHE_EXIT(irange);
}

static void r4k_flush_icache_range(unsigned long start, unsigned long end)
{
	struct flush_icache_range_args args;

	args.start = start;
	args.end = end;

#ifdef BRCM_CMT_CACHE_WAR_1
	local_r4k_flush_icache_range(&args);
#else
	r4k_on_each_cpu(local_r4k_flush_icache_range, &args, 1, 1);
#endif
	instruction_hazard();

}

/*
 * Ok, this seriously sucks.  We use them to flush a user page but don't
 * know the virtual address, so we have to blast away the whole icache
 * which is significantly more expensive than the real thing.  Otoh we at
 * least know the kernel address of the page so we can flush it
 * selectivly.
 */

struct flush_icache_page_args {
	struct vm_area_struct *vma;
	struct page *page;
};

static inline void local_r4k_flush_icache_page(void *args)
{
	struct flush_icache_page_args *fip_args = args;
	struct vm_area_struct *vma = fip_args->vma;
	struct page *page = fip_args->page;
	CACHE_ENTER(ipage);

	/*
	 * Tricky ...  Because we don't know the virtual address we've got the
	 * choice of either invalidating the entire primary and secondary
	 * caches or invalidating the secondary caches also.  With the subset
	 * enforcment on R4000SC, R4400SC, R10000 and R12000 invalidating the
	 * secondary cache will result in any entries in the primary caches
	 * also getting invalidated which hopefully is a bit more economical.
	 */
	if (cpu_has_inclusive_pcaches) {
		unsigned long addr = (unsigned long) page_address(page);

		r4k_blast_scache_page(addr);
		ClearPageDcacheDirty(page);

		CACHE_EXIT(ipage);
		return;
	}

	if (!cpu_has_ic_fills_f_dc) {
		unsigned long addr = (unsigned long) page_address(page);
		r4k_blast_dcache_page(addr);
		if (!cpu_icache_snoops_remote_store)
			r4k_blast_scache_page(addr);
		ClearPageDcacheDirty(page);
	}

	/*
	 * We're not sure of the virtual address(es) involved here, so
	 * we have to flush the entire I-cache.
	 */
	if (cpu_has_vtag_icache && vma->vm_mm == current->active_mm) {
		int cpu = smp_processor_id();

		if (cpu_context(cpu, vma->vm_mm) != 0)
			drop_mmu_context(vma->vm_mm, cpu);
	} else
		r4k_blast_icache();
	CACHE_EXIT(ipage);
}

static void r4k_flush_icache_page(struct vm_area_struct *vma,
	struct page *page)
{
	struct flush_icache_page_args args;

	/*
	 * If there's no context yet, or the page isn't executable, no I-cache
	 * flush is needed.
	 */
	if (!(vma->vm_flags & VM_EXEC))
		return;

	args.vma = vma;
	args.page = page;

#ifdef BRCM_CMT_CACHE_WAR_1
	local_r4k_flush_icache_page(&args);
#else
	r4k_on_each_cpu(local_r4k_flush_icache_page, &args, 1, 1);
#endif
}


#ifdef CONFIG_DMA_NONCOHERENT

static void r4k_dma_cache_wback_inv(unsigned long addr, unsigned long size)
{
	CACHE_ENTER(dmawb);
	/* Catch bad driver code */
	BUG_ON(size == 0);

	if (cpu_has_inclusive_pcaches) {
		if (size >= scache_size)
			r4k_blast_scache();
		else
			blast_scache_range(addr, addr + size);
		CACHE_EXIT(dmawb);
		return;
	}

	/*
	 * Either no secondary cache or the available caches don't have the
	 * subset property so we have to flush the primary caches
	 * explicitly
	 */
	if (size >= dcache_size) {
		r4k_blast_dcache();
	} else {
		R4600_HIT_CACHEOP_WAR_IMPL;
		blast_dcache_range(addr, addr + size);
	}

	bc_wback_inv(addr, size);
	BCM_LOCAL_EXTRA_CACHEOP_WAR;
	CACHE_EXIT(dmawb);
}

static void r4k_dma_cache_inv(unsigned long addr, unsigned long size)
{
	CACHE_ENTER(dmainv);
	/* Catch bad driver code */
	BUG_ON(size == 0);

	if (cpu_has_inclusive_pcaches) {
		if (size >= scache_size)
			r4k_blast_scache();
		else
			blast_scache_range(addr, addr + size);
		CACHE_EXIT(dmainv);
		return;
	}

	if (size >= dcache_size) {
		r4k_blast_dcache();
	} else {
		R4600_HIT_CACHEOP_WAR_IMPL;
		blast_dcache_range(addr, addr + size);
	}

#if defined(CONFIG_MIPS_BCM7325)
	bc_wback_inv(addr, size);
#else
	bc_inv(addr, size);
#endif
	
	BCM_LOCAL_EXTRA_CACHEOP_WAR;
	CACHE_EXIT(dmainv);
}
#endif /* CONFIG_DMA_NONCOHERENT */

/*
 * While we're protected against bad userland addresses we don't care
 * very much about what happens in that case.  Usually a segmentation
 * fault will dump the process later on anyway ...
 */
static void local_r4k_flush_cache_sigtramp(void * arg)
{
	unsigned long ic_lsize = cpu_icache_line_size();
	unsigned long dc_lsize = cpu_dcache_line_size();
	unsigned long sc_lsize = cpu_scache_line_size();
	unsigned long addr = (unsigned long) arg;
	CACHE_ENTER(sigtramp);

	R4600_HIT_CACHEOP_WAR_IMPL;
	if (dc_lsize)
		protected_writeback_dcache_line(addr & ~(dc_lsize - 1));
	if (!cpu_icache_snoops_remote_store && scache_size)
		protected_writeback_scache_line(addr & ~(sc_lsize - 1));
	if (ic_lsize)
		protected_flush_icache_line(addr & ~(ic_lsize - 1));
	if (MIPS4K_ICACHE_REFILL_WAR) {
		__asm__ __volatile__ (
			".set push\n\t"
			".set noat\n\t"
			".set mips3\n\t"
#ifdef CONFIG_32BIT
			"la	$at,1f\n\t"
#endif
#ifdef CONFIG_64BIT
			"dla	$at,1f\n\t"
#endif
			"cache	%0,($at)\n\t"
			"nop; nop; nop\n"
			"1:\n\t"
			".set pop"
			:
			: "i" (Hit_Invalidate_I));
	}
	if (MIPS_CACHE_SYNC_WAR)
		__asm__ __volatile__ ("sync");
	CACHE_EXIT(sigtramp);
}

static void r4k_flush_cache_sigtramp(unsigned long addr)
{
#ifdef BRCM_CMT_CACHE_WAR_1
	local_r4k_flush_cache_sigtramp((void *) addr);
#else
	r4k_on_each_cpu(local_r4k_flush_cache_sigtramp, (void *) addr, 1, 1);
#endif
}

static void r4k_flush_icache_all(void)
{
	CACHE_ENTER(iflush);
	if (cpu_has_vtag_icache)
		r4k_blast_icache();
	CACHE_EXIT(iflush);
}

static inline void rm7k_erratum31(void)
{
	const unsigned long ic_lsize = 32;
	unsigned long addr;

	/* RM7000 erratum #31. The icache is screwed at startup. */
	write_c0_taglo(0);
	write_c0_taghi(0);

	for (addr = INDEX_BASE; addr <= INDEX_BASE + 4096; addr += ic_lsize) {
		__asm__ __volatile__ (
			".set push\n\t"
			".set noreorder\n\t"
			".set mips3\n\t"
			"cache\t%1, 0(%0)\n\t"
			"cache\t%1, 0x1000(%0)\n\t"
			"cache\t%1, 0x2000(%0)\n\t"
			"cache\t%1, 0x3000(%0)\n\t"
			"cache\t%2, 0(%0)\n\t"
			"cache\t%2, 0x1000(%0)\n\t"
			"cache\t%2, 0x2000(%0)\n\t"
			"cache\t%2, 0x3000(%0)\n\t"
			"cache\t%1, 0(%0)\n\t"
			"cache\t%1, 0x1000(%0)\n\t"
			"cache\t%1, 0x2000(%0)\n\t"
			"cache\t%1, 0x3000(%0)\n\t"
			".set pop\n"
			:
			: "r" (addr), "i" (Index_Store_Tag_I), "i" (Fill));
	}
}

static char *way_string[] __initdata = { NULL, "direct mapped", "2-way",
	"3-way", "4-way", "5-way", "6-way", "7-way", "8-way"
};

static void __init probe_pcache(void)
{
	struct cpuinfo_mips *c = &current_cpu_data;
	unsigned int config = read_c0_config();
	unsigned int prid = read_c0_prid();
	unsigned long config1;
	unsigned int lsize;

	switch (c->cputype) {
	case CPU_R4600:			/* QED style two way caches? */
	case CPU_R4700:
	case CPU_R5000:
	case CPU_NEVADA:
		icache_size = 1 << (12 + ((config & CONF_IC) >> 9));
		c->icache.linesz = 16 << ((config & CONF_IB) >> 5);
		c->icache.ways = 2;
		c->icache.waybit = __ffs(icache_size/2);

		dcache_size = 1 << (12 + ((config & CONF_DC) >> 6));
		c->dcache.linesz = 16 << ((config & CONF_DB) >> 4);
		c->dcache.ways = 2;
		c->dcache.waybit= __ffs(dcache_size/2);

		c->options |= MIPS_CPU_CACHE_CDEX_P;
		break;

	case CPU_R5432:
	case CPU_R5500:
		icache_size = 1 << (12 + ((config & CONF_IC) >> 9));
		c->icache.linesz = 16 << ((config & CONF_IB) >> 5);
		c->icache.ways = 2;
		c->icache.waybit= 0;

		dcache_size = 1 << (12 + ((config & CONF_DC) >> 6));
		c->dcache.linesz = 16 << ((config & CONF_DB) >> 4);
		c->dcache.ways = 2;
		c->dcache.waybit = 0;

		c->options |= MIPS_CPU_CACHE_CDEX_P;
		break;

	case CPU_TX49XX:
		icache_size = 1 << (12 + ((config & CONF_IC) >> 9));
		c->icache.linesz = 16 << ((config & CONF_IB) >> 5);
		c->icache.ways = 4;
		c->icache.waybit= 0;

		dcache_size = 1 << (12 + ((config & CONF_DC) >> 6));
		c->dcache.linesz = 16 << ((config & CONF_DB) >> 4);
		c->dcache.ways = 4;
		c->dcache.waybit = 0;

		c->options |= MIPS_CPU_CACHE_CDEX_P;
		c->options |= MIPS_CPU_PREFETCH;
		break;

	case CPU_R4000PC:
	case CPU_R4000SC:
	case CPU_R4000MC:
	case CPU_R4400PC:
	case CPU_R4400SC:
	case CPU_R4400MC:
	case CPU_R4300:
		icache_size = 1 << (12 + ((config & CONF_IC) >> 9));
		c->icache.linesz = 16 << ((config & CONF_IB) >> 5);
		c->icache.ways = 1;
		c->icache.waybit = 0; 	/* doesn't matter */

		dcache_size = 1 << (12 + ((config & CONF_DC) >> 6));
		c->dcache.linesz = 16 << ((config & CONF_DB) >> 4);
		c->dcache.ways = 1;
		c->dcache.waybit = 0;	/* does not matter */

		c->options |= MIPS_CPU_CACHE_CDEX_P;
		break;

	case CPU_R10000:
	case CPU_R12000:
	case CPU_R14000:
		icache_size = 1 << (12 + ((config & R10K_CONF_IC) >> 29));
		c->icache.linesz = 64;
		c->icache.ways = 2;
		c->icache.waybit = 0;

		dcache_size = 1 << (12 + ((config & R10K_CONF_DC) >> 26));
		c->dcache.linesz = 32;
		c->dcache.ways = 2;
		c->dcache.waybit = 0;

		c->options |= MIPS_CPU_PREFETCH;
		break;

	case CPU_VR4133:
		write_c0_config(config & ~VR41_CONF_P4K);
	case CPU_VR4131:
		/* Workaround for cache instruction bug of VR4131 */
		if (c->processor_id == 0x0c80U || c->processor_id == 0x0c81U ||
		    c->processor_id == 0x0c82U) {
			config |= 0x00400000U;
			if (c->processor_id == 0x0c80U)
				config |= VR41_CONF_BP;
			write_c0_config(config);
		} else
			c->options |= MIPS_CPU_CACHE_CDEX_P;

		icache_size = 1 << (10 + ((config & CONF_IC) >> 9));
		c->icache.linesz = 16 << ((config & CONF_IB) >> 5);
		c->icache.ways = 2;
		c->icache.waybit = __ffs(icache_size/2);

		dcache_size = 1 << (10 + ((config & CONF_DC) >> 6));
		c->dcache.linesz = 16 << ((config & CONF_DB) >> 4);
		c->dcache.ways = 2;
		c->dcache.waybit = __ffs(dcache_size/2);
		break;

	case CPU_VR41XX:
	case CPU_VR4111:
	case CPU_VR4121:
	case CPU_VR4122:
	case CPU_VR4181:
	case CPU_VR4181A:
		icache_size = 1 << (10 + ((config & CONF_IC) >> 9));
		c->icache.linesz = 16 << ((config & CONF_IB) >> 5);
		c->icache.ways = 1;
		c->icache.waybit = 0; 	/* doesn't matter */

		dcache_size = 1 << (10 + ((config & CONF_DC) >> 6));
		c->dcache.linesz = 16 << ((config & CONF_DB) >> 4);
		c->dcache.ways = 1;
		c->dcache.waybit = 0;	/* does not matter */

		c->options |= MIPS_CPU_CACHE_CDEX_P;
		break;

	case CPU_RM7000:
		rm7k_erratum31();

	case CPU_RM9000:
		icache_size = 1 << (12 + ((config & CONF_IC) >> 9));
		c->icache.linesz = 16 << ((config & CONF_IB) >> 5);
		c->icache.ways = 4;
		c->icache.waybit = __ffs(icache_size / c->icache.ways);

		dcache_size = 1 << (12 + ((config & CONF_DC) >> 6));
		c->dcache.linesz = 16 << ((config & CONF_DB) >> 4);
		c->dcache.ways = 4;
		c->dcache.waybit = __ffs(dcache_size / c->dcache.ways);

#if !defined(CONFIG_SMP) || !defined(RM9000_CDEX_SMP_WAR)
		c->options |= MIPS_CPU_CACHE_CDEX_P;
#endif
		c->options |= MIPS_CPU_PREFETCH;
		break;

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

	/*
	 * Processor configuration sanity check for the R4000SC erratum
	 * #5.  With page sizes larger than 32kB there is no possibility
	 * to get a VCE exception anymore so we don't care about this
	 * misconfiguration.  The case is rather theoretical anyway;
	 * presumably no vendor is shipping his hardware in the "bad"
	 * configuration.
	 */
	if ((prid & 0xff00) == PRID_IMP_R4000 && (prid & 0xff) < 0x40 &&
	    !(config & CONF_SC) && c->icache.linesz != 16 &&
	    PAGE_SIZE <= 0x8000)
		panic("Improper R4000SC processor configuration detected");

	/* compute a couple of other cache variables */
	c->icache.waysize = icache_size / c->icache.ways;
	c->dcache.waysize = dcache_size / c->dcache.ways;

	c->icache.sets = c->icache.linesz ?
		icache_size / (c->icache.linesz * c->icache.ways) : 0;
	c->dcache.sets = c->dcache.linesz ?
		dcache_size / (c->dcache.linesz * c->dcache.ways) : 0;

	/*
	 * R10000 and R12000 P-caches are odd in a positive way.  They're 32kB
	 * 2-way virtually indexed so normally would suffer from aliases.  So
	 * normally they'd suffer from aliases but magic in the hardware deals
	 * with that for us so we don't need to take care ourselves.
	 */
	switch (c->cputype) {
	case CPU_20KC:
	case CPU_25KF:
		c->dcache.flags |= MIPS_CACHE_PINDEX;
	case CPU_R10000:
	case CPU_R12000:
	case CPU_R14000:
	case CPU_SB1:
		break;
	case CPU_24K:
	case CPU_34K:
	case CPU_74K:
		if ((read_c0_config7() & (1 << 16))) {
			/* effectively physically indexed dcache,
			   thus no virtual aliases. */
			c->dcache.flags |= MIPS_CACHE_PINDEX;
			break;
		}
	default:
		if (c->dcache.waysize > PAGE_SIZE)
			c->dcache.flags |= MIPS_CACHE_ALIASES;
	}

	switch (c->cputype) {
	case CPU_20KC:
		/*
		 * Some older 20Kc chips doesn't have the 'VI' bit in
		 * the config register.
		 */
		c->icache.flags |= MIPS_CACHE_VTAG;
		break;

	case CPU_AU1000:
	case CPU_AU1500:
	case CPU_AU1100:
	case CPU_AU1550:
	case CPU_AU1200:
		c->icache.flags |= MIPS_CACHE_IC_F_DC;
		break;
	}

	printk("Primary instruction cache %ldkB, %s, %s, linesize %d bytes.\n",
	       icache_size >> 10,
	       cpu_has_vtag_icache ? "virtually tagged" : "physically tagged",
	       way_string[c->icache.ways], c->icache.linesz);

	printk("Primary data cache %ldkB, %s, linesize %d bytes.\n",
	       dcache_size >> 10, way_string[c->dcache.ways], c->dcache.linesz);
}

/*
 * If you even _breathe_ on this function, look at the gcc output and make sure
 * it does not pop things on and off the stack for the cache sizing loop that
 * executes in KSEG1 space or else you will crash and burn badly.  You have
 * been warned.
 */
static int __init probe_scache(void)
{
	extern unsigned long stext;
	unsigned long flags, addr, begin, end, pow2;
	unsigned int config = read_c0_config();
	struct cpuinfo_mips *c = &current_cpu_data;
	int tmp;

	if (config & CONF_SC)
		return 0;

	begin = (unsigned long) &stext;
	begin &= ~((4 * 1024 * 1024) - 1);
	end = begin + (4 * 1024 * 1024);

	/*
	 * This is such a bitch, you'd think they would make it easy to do
	 * this.  Away you daemons of stupidity!
	 */
	local_irq_save(flags);

	/* Fill each size-multiple cache line with a valid tag. */
	pow2 = (64 * 1024);
	for (addr = begin; addr < end; addr = (begin + pow2)) {
		unsigned long *p = (unsigned long *) addr;
		__asm__ __volatile__("nop" : : "r" (*p)); /* whee... */
		pow2 <<= 1;
	}

	/* Load first line with zero (therefore invalid) tag. */
	write_c0_taglo(0);
	write_c0_taghi(0);
	__asm__ __volatile__("nop; nop; nop; nop;"); /* avoid the hazard */
	cache_op(Index_Store_Tag_I, begin);
	cache_op(Index_Store_Tag_D, begin);
	cache_op(Index_Store_Tag_SD, begin);

	/* Now search for the wrap around point. */
	pow2 = (128 * 1024);
	tmp = 0;
	for (addr = begin + (128 * 1024); addr < end; addr = begin + pow2) {
		cache_op(Index_Load_Tag_SD, addr);
		__asm__ __volatile__("nop; nop; nop; nop;"); /* hazard... */
		if (!read_c0_taglo())
			break;
		pow2 <<= 1;
	}
	local_irq_restore(flags);
	addr -= begin;

	scache_size = addr;
	c->scache.linesz = 16 << ((config & R4K_CONF_SB) >> 22);
	c->scache.ways = 1;
	c->dcache.waybit = 0;		/* does not matter */

	return 1;
}

extern int r5k_sc_init(void);
extern int rm7k_sc_init(void);
extern int mips_sc_init(void);

static void __init setup_scache(void)
{
	struct cpuinfo_mips *c = &current_cpu_data;
	unsigned int config = read_c0_config();
	int sc_present = 0;

	/*
	 * Do the probing thing on R4000SC and R4400SC processors.  Other
	 * processors don't have a S-cache that would be relevant to the
	 * Linux memory managment.
	 */
	switch (c->cputype) {
	case CPU_R4000SC:
	case CPU_R4000MC:
	case CPU_R4400SC:
	case CPU_R4400MC:
		sc_present = run_uncached(probe_scache);
		if (sc_present)
			c->options |= MIPS_CPU_CACHE_CDEX_S;
		break;

	case CPU_R10000:
	case CPU_R12000:
	case CPU_R14000:
		scache_size = 0x80000 << ((config & R10K_CONF_SS) >> 16);
		c->scache.linesz = 64 << ((config >> 13) & 1);
		c->scache.ways = 2;
		c->scache.waybit= 0;
		sc_present = 1;
		break;

	case CPU_R5000:
	case CPU_NEVADA:
#ifdef CONFIG_R5000_CPU_SCACHE
		r5k_sc_init();
#endif
                return;

	case CPU_RM7000:
	case CPU_RM9000:
#ifdef CONFIG_RM7000_CPU_SCACHE
		rm7k_sc_init();
#endif
		return;

	default:
		if (c->isa_level == MIPS_CPU_ISA_M32R1 ||
		    c->isa_level == MIPS_CPU_ISA_M32R2 ||
		    c->isa_level == MIPS_CPU_ISA_M64R1 ||
		    c->isa_level == MIPS_CPU_ISA_M64R2) {
#ifdef CONFIG_MIPS_CPU_SCACHE
			if (mips_sc_init ()) {
				scache_size = c->scache.ways * c->scache.sets * c->scache.linesz;
				printk("MIPS secondary cache %ldkB, %s, linesize %d bytes.\n",
				       scache_size >> 10,
				       way_string[c->scache.ways], c->scache.linesz);
			}
#else
			if (!(c->scache.flags & MIPS_CACHE_NOT_PRESENT))
				panic("Dunno how to handle MIPS32 / MIPS64 second level cache");
#endif
			return;
		}
		sc_present = 0;
	}

	if (!sc_present)
		return;

	/* compute a couple of other cache variables */
	c->scache.waysize = scache_size / c->scache.ways;

	c->scache.sets = scache_size / (c->scache.linesz * c->scache.ways);

	printk("Unified secondary cache %ldkB %s, linesize %d bytes.\n",
	       scache_size >> 10, way_string[c->scache.ways], c->scache.linesz);

	c->options |= MIPS_CPU_INCLUSIVE_CACHES;
}

void au1x00_fixup_config_od(void)
{
	/*
	 * c0_config.od (bit 19) was write only (and read as 0)
	 * on the early revisions of Alchemy SOCs.  It disables the bus
	 * transaction overlapping and needs to be set to fix various errata.
	 */
	switch (read_c0_prid()) {
	case 0x00030100: /* Au1000 DA */
	case 0x00030201: /* Au1000 HA */
	case 0x00030202: /* Au1000 HB */
	case 0x01030200: /* Au1500 AB */
	/*
	 * Au1100 errata actually keeps silence about this bit, so we set it
	 * just in case for those revisions that require it to be set according
	 * to arch/mips/au1000/common/cputable.c
	 */
	case 0x02030200: /* Au1100 AB */
	case 0x02030201: /* Au1100 BA */
	case 0x02030202: /* Au1100 BC */
		set_c0_config(1 << 19);
		break;
	}
}

static inline void coherency_setup(void)
{
	change_c0_config(CONF_CM_CMASK, CONF_CM_DEFAULT);

	/*
	 * c0_status.cu=0 specifies that updates by the sc instruction use
	 * the coherency mode specified by the TLB; 1 means cachable
	 * coherent update on write will be used.  Not all processors have
	 * this bit and; some wire it to zero, others like Toshiba had the
	 * silly idea of putting something else there ...
	 */
	switch (current_cpu_data.cputype) {
	case CPU_R4000PC:
	case CPU_R4000SC:
	case CPU_R4000MC:
	case CPU_R4400PC:
	case CPU_R4400SC:
	case CPU_R4400MC:
		clear_c0_config(CONF_CU);
		break;
	/*
	 * We need to catch the early Alchemy SOCs with
	 * the write-only co_config.od bit and set it back to one...
	 */
	case CPU_AU1000: /* rev. DA, HA, HB */
	case CPU_AU1100: /* rev. AB, BA, BC ?? */
	case CPU_AU1500: /* rev. AB */
		au1x00_fixup_config_od();
		break;
	}
}

void __init r4k_cache_init(void)
{
	extern void build_clear_page(void);
	extern void build_copy_page(void);
	extern char except_vec2_generic;
	struct cpuinfo_mips *c = &current_cpu_data;

	/* Default cache error handler for R4000 and R5000 family */
	set_uncached_handler (0x100, &except_vec2_generic, 0x80);

#ifdef CONFIG_MIPS_BRCM97XXX
	{
		extern void uart_puts(const char *);
		extern int rac_setting(int);
    		extern void cacheALibInit(void);

#if defined(CONFIG_MIPS_BCM7320) || defined(CONFIG_MIPS_BCM7328) \
	|| defined(CONFIG_MIPS_BCM7319) || defined(CONFIG_MIPS_BCM7440B0) \
	|| defined (CONFIG_MIPS_BCM7443) ||  defined(CONFIG_MIPS_BCM7325) 
		/* Do nothing for BRCM MIPS-5k, 24k */
	printk("MTI Core cache probe\n");

#else
#define ICACHE_DCACHE_ENABLED 0xC0000000

/* RYH  5/19/06 */
        /* if ( strstr(cfeBootParms,"bcmrac") ) */
		if( !rac_set )
		{
			rac_set = 1;
			rac_setting(par_val);
		}

#if ! (defined(CONFIG_MIPS_BCM7325) || defined(CONFIG_MIPS_BCM7335) \
	|| defined(CONFIG_MIPS_BCM7405) || defined (CONFIG_MIPS_BCM3548))
		{
		unsigned int reg;

		/* Initialize and enable caches */
	    	reg = read_c0_diag();
    		if ( (reg& ICACHE_DCACHE_ENABLED) != ICACHE_DCACHE_ENABLED)
	    	{
    			uart_puts("$$$$$init cache \n");
        		cacheALibInit();
	        }
       
		reg = read_c0_diag();
		write_c0_diag(reg | ICACHE_DCACHE_ENABLED);

		}
#endif	// !defined(CONFIG_MIPS_BCM7325)
#endif	//  defined(CONFIG_MIPS_BCM7320) 
	}
#endif	//  CONFIG_MIPS_BRCM97XXX

	probe_pcache();
	setup_scache();

	/* THT: Turn on cache aliasing check, nasty cache aliasing problems if not */
	if (c->dcache.sets * c->dcache.ways > PAGE_SIZE)
		c->dcache.flags |= MIPS_CACHE_ALIASES;

	r4k_blast_dcache_page_setup();
	r4k_blast_dcache_page_indexed_setup();
	r4k_blast_dcache_setup();
	r4k_blast_icache_page_setup();
	r4k_blast_icache_page_indexed_setup();
	r4k_blast_icache_setup();
	r4k_blast_scache_page_setup();
	r4k_blast_scache_page_indexed_setup();
	r4k_blast_scache_setup();

	/*
	 * Some MIPS32 and MIPS64 processors have physically indexed caches.
	 * This code supports virtually indexed processors and will be
	 * unnecessarily inefficient on physically indexed processors.
	 */
	if (c->dcache.linesz)
		shm_align_mask = max_t( unsigned long,
					c->dcache.sets * c->dcache.linesz - 1,
					PAGE_SIZE - 1);
	else
		shm_align_mask = PAGE_SIZE-1;
	flush_cache_all		= r4k_flush_cache_all;
	__flush_cache_all	= r4k___flush_cache_all;
	flush_cache_mm		= r4k_flush_cache_mm;
	flush_cache_page	= r4k_flush_cache_page;
	__flush_icache_page	= r4k_flush_icache_page;
	flush_cache_range	= r4k_flush_cache_range;

	flush_cache_sigtramp	= r4k_flush_cache_sigtramp;
	flush_icache_all	= r4k_flush_icache_all;
	local_flush_data_cache_page	= local_r4k_flush_data_cache_page;
	flush_data_cache_page	= r4k_flush_data_cache_page;
	flush_icache_range	= r4k_flush_icache_range;

#ifdef CONFIG_DMA_NONCOHERENT
	_dma_cache_wback_inv	= r4k_dma_cache_wback_inv;
	_dma_cache_wback	= r4k_dma_cache_wback_inv;
	_dma_cache_inv		= r4k_dma_cache_inv;
#endif

	build_clear_page();
	build_copy_page();
	local_r4k___flush_cache_all(NULL);
	coherency_setup();
}
