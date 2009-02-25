/*
 * linux/arch/mips/brcmstb/common/discontig.c
 *
 * Discontiguous memory support.
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
 * when   who what
 * 060501 tht Initial codings for 97438 NUMA
 */
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/bootmem.h>
#include <linux/mmzone.h>

int numnodes = 1;

/*
 * Our node_data structure for discontiguous memory.
 */

static bootmem_data_t node_bootmem_data[MAX_NR_NODES];

pg_data_t discontig_node_data[MAX_NR_NODES] = {
  { bdata: &node_bootmem_data[0] },
  { bdata: &node_bootmem_data[1] },
#if MAX_NR_NODES >= 3
  { bdata: &node_bootmem_data[2] },
#endif
};

//struct page  *node_mem_maps[MAX_NR_NODES];

int node_sizes[MAX_NR_NODES];

EXPORT_SYMBOL(numnodes);
//EXPORT_SYMBOL(node_mem_maps);
EXPORT_SYMBOL(node_sizes);
EXPORT_SYMBOL(discontig_node_data);

unsigned long get_RAM_size(void);

void __init brcm_numa_init(void) 
{
	unsigned int ram_size;

#if defined ( CONFIG_MIPS_BCM3563C0 )
	numnodes = NR_NODES;
#else
	/*
	 * first 256Mb is in 'node' 0.
	 * second 256Mb is in 'node' 1.
	 */
	ram_size = get_RAM_size();

	if (ram_size <= (256 << 20)) 
		numnodes = 1;
	else
		numnodes = NR_NODES;

#if defined (CONFIG_MIPS_BCM7440) || defined (CONFIG_MIPS_BCM7601)
	int i;
	for (i = 0; i < MAX_NR_NODES; i++) {
		if (i < numnodes)
			discontig_node_data[i].bdata = &node_bootmem_data[i];
		else
			discontig_node_data[i].bdata = (pg_data_t *)0;
	}
#endif
#endif
}

#ifdef __DEBUG_DISCONTIGMEM
/* Enable this block for debugging */

struct page* __virt_to_page(unsigned long kaddr) 
{
	
	if (kaddr >= 0x10000000) printk("virt_to_page(kaddr=%08x)\n", kaddr);
	if (IS_KADDR_LOWER_RAM(kaddr)) {
		if (kaddr >= 0x10000000) printk("returning lower ram\n");
		return (struct page*) (NODE_MEM_MAP(0) + (__pa((unsigned long )kaddr) >> PAGE_SHIFT));
	} 
	else if (IS_KADDR_UPPER_RAM((unsigned long)kaddr))
	{
		if (kaddr >= 0x10000000) printk("virt_to_page: returning Node 1, NODE_MEM_MAP(1)=%08x, pa=%08x, U_R_B=%08x\n", 
			NODE_MEM_MAP(1), __pa((unsigned long )kaddr), UPPER_RAM_BASE);
		return (struct page*) (NODE_MEM_MAP(1) + ((__pa((unsigned long )kaddr) \
			  - UPPER_RAM_BASE) >> PAGE_SHIFT));
	}
	else {
			printk("virt_to_page: Invalid address\n");
			return (struct page*) 0;
	}
}


EXPORT_SYMBOL(__virt_to_page);
#endif

