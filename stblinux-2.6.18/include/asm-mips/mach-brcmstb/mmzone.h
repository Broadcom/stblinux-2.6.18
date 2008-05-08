/*
 * include/asm/brcmstb/common/discontig.h
 *
 * Copyright (C) 2006 Broadcom Corporation
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
 *defines for Broadcom eval boards with discontiguous memory model.
 *
 * 050106 THT Created
 * 60814  JWF 2.6 version
 */
/* JWF  this is specific to BRCM97438 with only 2 memory banks */

#ifndef __MACH_BRCMSTB_MMZONE_H
#define __MACH_BRCMSTB_MMZONE_H

#include <linux/mmzone.h>  

#define NR_NODES	2
#define MAX_NR_NODES	2

extern pg_data_t discontig_node_data[];
extern struct page* node_mem_maps[NR_NODES];
extern int node_sizes[NR_NODES];
extern int numnodes;

/* PR30707: Picked up wrong NODE_DATA define */
#ifdef NODE_DATA
#undef NODE_DATA
#endif
#define NODE_DATA(nid)		(&discontig_node_data[nid])

#ifdef NODE_MEM_MAP
#undef NODE_MEM_MAP
#endif
#define NODE_MEM_MAP(nid)	(NODE_DATA(nid)->node_mem_map)

#ifdef HIGHMEM_START
#undef HIGHMEM_START
#endif
#define HIGHMEM_START	0x80000000UL

#if defined ( CONFIG_MIPS_BCM3563C0 )

#define UPPER_RAM_BASE  0x60000000UL     // 1536MB
#define UPPER_RAM_SIZE  0x02000000UL     // 32MB
#define UPPER_RAM_END   (UPPER_RAM_BASE+UPPER_RAM_SIZE)
#define LOWER_RAM_SIZE  0x08000000UL    // 128MB
#define LOWER_RAM_END   0x08000000UL    // 128MB

#else

#define UPPER_RAM_BASE  0x20000000UL	// 512MB
#define UPPER_RAM_SIZE 	0x10000000UL	// 256MB
#define UPPER_RAM_END	(UPPER_RAM_BASE+UPPER_RAM_SIZE)
#define LOWER_RAM_SIZE 	0x10000000UL	// 256MB
#define LOWER_RAM_END	0x10000000UL	// 256MB

#endif

#if defined ( CONFIG_MIPS_BCM7440 )

#define UPPER_RAM_VBASE 0xd8000000UL
#define LOWER_RAM_VBASE PAGE_OFFSET

#else

#define UPPER_RAM_VBASE 0xe0000000UL
#define LOWER_RAM_VBASE	PAGE_OFFSET
#endif

#define IS_PA_UPPER_RAM(pa) (pa >= UPPER_RAM_BASE && pa < UPPER_RAM_END)
#define IS_KADDR_LOWER_RAM(kaddr) ((u32)kaddr >= LOWER_RAM_VBASE && \
				   (u32)kaddr < (LOWER_RAM_VBASE + LOWER_RAM_SIZE))
#define IS_KADDR_UPPER_RAM(kaddr) ((u32)kaddr >= UPPER_RAM_VBASE && \
				   (u32)kaddr < (UPPER_RAM_VBASE + UPPER_RAM_SIZE))

/* These functions fail for non-ram addresses */
static __inline__ unsigned long 
__pa(unsigned long x)
{

	if (x == (UPPER_RAM_VBASE + UPPER_RAM_SIZE)) // For high_memory
		return UPPER_RAM_END;

	else if (IS_KADDR_UPPER_RAM(x))
		return (UPPER_RAM_BASE + (x - UPPER_RAM_VBASE));

	else if (x == (LOWER_RAM_VBASE + LOWER_RAM_SIZE)) // For high_memory with only one node
		return (LOWER_RAM_SIZE);

	else if (IS_KADDR_LOWER_RAM(x))
		return (x - PAGE_OFFSET);
		
	printk(KERN_ERR "__pa invalid address 0x%08lx\n", x);
	BUG();
	return(0);
}

static __inline__ void* 
__va(unsigned long x)
{
	if (x >= UPPER_RAM_BASE && x <= UPPER_RAM_END) /* Use <= for high_memory */
		return ((void*) (UPPER_RAM_VBASE + x - UPPER_RAM_BASE));
	else
		return ((void *)((unsigned long) (x) + PAGE_OFFSET));
}

#define virt_to_page(kaddr) \
	((struct page*) (((IS_KADDR_LOWER_RAM(kaddr))  \
		? NODE_MEM_MAP(0) + (__pa((unsigned long )kaddr) >> PAGE_SHIFT) \
		:  (IS_KADDR_UPPER_RAM((unsigned long)kaddr) \
			? NODE_MEM_MAP(1) + ((__pa((unsigned long )kaddr) \
			  - UPPER_RAM_BASE) >> PAGE_SHIFT) \
			: 0))))

#define VALID_PAGE(page) \
	((unsigned long) (page) >= (unsigned long) NODE_MEM_MAP(0) && \
	 (unsigned long) (page) < (unsigned long) (NODE_MEM_MAP(0) + \
	                          NODE_DATA(0)->node_spanned_pages) \
		? 1 \
		: ((unsigned long) (page) >= (unsigned long) NODE_MEM_MAP(1) && \
		   (unsigned long) (page) < (unsigned long) (NODE_MEM_MAP(1) + \
		                            NODE_DATA(1)->node_spanned_pages) \
		   ? 1 : 0))

#define virt_addr_valid(kaddr) (pfn_valid(__pa((unsigned long)kaddr) >> PAGE_SHIFT))

#define nth_page(page,n) pfn_to_page(page_to_pfn((page)) + (n))

#define pa_to_nid(pa) ((((u32)(pa) >= UPPER_RAM_BASE) && \
		        ((u32)(pa) < UPPER_RAM_END)) ? 1 : 0)

#endif /* __MACH_BRCMSTB_MMZONE_H */
