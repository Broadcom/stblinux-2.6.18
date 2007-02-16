/*
 * Flash mapping for BCM7xxx boards
 *
 * Copyright (C) 2001 Broadcom Corporation
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
 * THT				10-23-2002
 * Steven J. Hill	09-25-2001
 * Mark Huang		09-15-2001
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/config.h>
#include <linux/init.h>

extern int gFlashSize;

#ifdef CONFIG_BCM93730
#define WINDOW_ADDR 0x1e000000

#elif defined(CONFIG_MIPS_BRCM97XXX)
extern unsigned long getPhysFlashBase(void);

#define WINDOW_ADDR getPhysFlashBase()

#else
#error ("bcm9xxxx-flash.c: Unsupported architecture\n")
#endif

#ifdef CONFIG_MIPS_BCM7315_BBX
/* 
 * The 7315BBX only has a 4MB flash
 */
#define WINDOW_SIZE 0x00400000	/* 4 MB flash */
#else
/* 
 * All other 97XXX boards.  May have 2 flash chips, but we only use 1.
 * and since they are of different sizes, no interleaving.
 */
#define WINDOW_SIZE (0x20000000	 - WINDOW_ADDR)

#endif


#define BUSWIDTH 2

static struct mtd_info *bcm9XXXX_mtd;

#ifdef CONFIG_MTD_COMPLEX_MAPPINGS
extern int bcm7401Cx_rev;
extern int bcm7118Ax_rev;

static inline void bcm9XXXX_map_copy_from_16bytes(void *to, unsigned long from, ssize_t len)
{
	static DEFINE_SPINLOCK(bcm9XXXX_lock);
	unsigned long flags;
	
	spin_lock_irqsave(&bcm9XXXX_lock, flags);
	*(volatile unsigned long*)0xbffff880 = 0xFFFF;
	*(volatile unsigned long*)0xbffff880 = 0xFFFF;
	*(volatile unsigned long*)0xbffff880 = 0xFFFF;
	*(volatile unsigned long*)0xbffff880 = 0xFFFF;
	*(volatile unsigned long*)0xbffff880 = 0xFFFF;
	*(volatile unsigned long*)0xbffff880 = 0xFFFF;
	*(volatile unsigned long*)0xbffff880 = 0xFFFF;
	*(volatile unsigned long*)0xbffff880 = 0xFFFF;
	memcpy_fromio(to, from, len);
	spin_unlock_irqrestore(&bcm9XXXX_lock, flags);
}

static map_word bcm9XXXX_map_read(struct map_info *map, unsigned long ofs)
{
	/* if it is 7401C0, then we need this workaround */
	if(bcm7401Cx_rev == 0x20 || bcm7118Ax_rev == 0x0)
	{	
		map_word r;
		static DEFINE_SPINLOCK(bcm9XXXX_lock);
		unsigned long flags;
	
		spin_lock_irqsave(&bcm9XXXX_lock, flags);
		*(volatile unsigned long*)0xb0400b1c = 0xFFFF;
		*(volatile unsigned long*)0xb0400b1c = 0xFFFF;
		*(volatile unsigned long*)0xb0400b1c = 0xFFFF;
		*(volatile unsigned long*)0xb0400b1c = 0xFFFF;
		*(volatile unsigned long*)0xb0400b1c = 0xFFFF;
		*(volatile unsigned long*)0xb0400b1c = 0xFFFF;
		*(volatile unsigned long*)0xb0400b1c = 0xFFFF;
		*(volatile unsigned long*)0xb0400b1c = 0xFFFF;
		r = inline_map_read(map, ofs);
		spin_unlock_irqrestore(&bcm9XXXX_lock, flags);
		return r;
	}
	else
		return inline_map_read(map, ofs);
}

static void bcm9XXXX_map_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	/* if it is 7401C0, then we need this workaround */
	if(bcm7401Cx_rev == 0x20 || bcm7118Ax_rev == 0x0)
	{
		if(len > 16)
		{
			while(len >= 16)
			{
				bcm9XXXX_map_copy_from_16bytes(to, map->virt + from, 16);
				(volatile unsigned long*)to += 4;
				(volatile unsigned long*)from += 4;
				len -= 16;
			}
		}
	
		if(len > 0)
		bcm9XXXX_map_copy_from_16bytes(to, map->virt + from, len);
	}	
	else	
		memcpy_fromio(to, map->virt + from, len);
}

static void bcm9XXXX_map_write(struct map_info *map, const map_word d, unsigned long ofs)
{
	inline_map_write(map, d, ofs);
}

static void bcm9XXXX_map_copy_to(struct map_info *map, unsigned long to, const void *from, ssize_t len)
{
	inline_map_copy_to(map, to, from, len);
}
#endif

struct map_info bcm9XXXX_map
= {
	name: "Broadcom 9xxxx mapped flash",
	// size: WINDOW_SIZE, // THT: Now a value to be determined at run-time.
	bankwidth: BUSWIDTH,

// jipeng - enable this for 7401C0 & 7118A0
#ifdef	CONFIG_MTD_COMPLEX_MAPPINGS
	read: bcm9XXXX_map_read,
	copy_from: bcm9XXXX_map_copy_from,
	write: bcm9XXXX_map_write,
	copy_to: bcm9XXXX_map_copy_to
#endif 
};

/*
 * Don't define DEFAULT_SIZE_MB if the platform does not support a standard partition table.
 * Defining it will allow the user to specify flashsize=nnM at boot time for non-standard flash size, however.
 */

static struct mtd_partition bcm9XXXX_parts[] = {
#ifdef CONFIG_BCM93730   /* BCM937xx STBs with PMON bootloader */
//#define DEFAULT_SIZE_MB 16
	{ name: "kernel",	offset: 0,		size: 2560*1024 }, 
			/* This size is 20*128K blocks. Although PMON use 128K blocks and
			 * will erase enough blocks to flash the kernel according to its size, 
			  * we must start rootfs on 256K boundary for mtd to work correctly 
			  */
	{ name: "rootfs",	offset: 2560*1024, 	size: 13696*1024 },
	{ name: "config",	offset: 16256*1024,	size: 128*1024 },

#elif defined( CONFIG_MIPS_BCM3560_WO_MIPS_INIT_FIX )
/* THT PR16701: 8/16/05 Obsoleted as the  MIPS init codes have been fixed.
 * This workaround is required for all 3560 CFE bootloaders built before this date:
 * Build Date: Mon Aug 15 14:34:08 PDT 2005 (rpereira@stb-irva-01)
 */
/* Temporarily created a skip zone to circumvent bad addr in flash addr space */
	{ name: "rootfs",	offset: 0,		    size: 10*1024*1024 },
	{ name: "skip", 	offset: 0x00A00000, size: 2*1024*1024 },
	{ name: "cfe",	    offset: 0x00C00000, size: 512*1024 },
	{ name: "vmlinux",	offset: 0x00C80000, size: 3582*1024 },
	{ name: "config",	offset: 0x00FFF800,	size: 144 },
	{ name: "nvram",	offset: 0x00FFF890,	size: 1904 },


#elif defined( CONFIG_MIPS_BCM7115 )  
/* PR9931: On the 7115 CFE boot-address is hardwired at 0 offset. */
//#define DEFAULT_SIZE_MB 16

	{ name: "cfe",	offset: 0, 		       size: 512*1024 },
	{ name: "rootfs",	offset: 0x00080000,     size: 12*1024*1024 },
	{ name: "vmlinux",	offset: 0x00C80000, size: 3582*1024 },
	{ name: "config",	offset: 0x00FFF800,	size: 144 },
	{ name: "nvram",	offset: 0x00FFF890,	size: 1904 },


#elif defined( CONFIG_MIPS_BCM7315_BBX )
#define DEFAULT_SIZE_MB 4
	{ name: "cfe",	    offset: 0, 			size: 512*1024 },
	{ name: "vmlinux",	offset: 0x00080000, size: 3582*1024 },
	{ name: "config",	offset: 0x003FF800,	size: 144 },
	{ name: "nvram",	offset: 0x003FF890,	size: 1904 },

#elif defined( CONFIG_MIPS_BCM7038 ) || defined( CONFIG_MIPS_BCM7400 ) || defined( CONFIG_MIPS_BCM7401 ) || \
	defined( CONFIG_MIPS_BCM7402 ) || defined( CONFIG_MIPS_BCM7118 ) 
	
#define DEFAULT_SIZE_MB 32 /* 32MB flash */
	{ name: "rootfs",	offset: 0,		    size: 28*1024*1024 },
	{ name: "cfe",	        offset: 0x01C00000, size: 512*1024 },
	{ name: "vmlinux",	offset: 0x01C80000, size: 3582*1024 },
	{ name: "config",	offset: 0x01FFF800,	size: 144 },
	{ name: "nvram",	offset: 0x01FFF890,	size: 1904 },

#elif defined( CONFIG_MIPS_BCM7440 )

#define DEFAULT_SIZE_MB 64 /* 64MB flash */

	{ name: "rootfs",		offset: 0,		size: 60*1024*1024 },
	{ name: "cfe",		offset: 0x03C00000, size: 512*1024 },
	{ name: "vmlinux",	offset: 0x03C80000, size: 3582*1024 },
	{ name: "config",		offset: 0x03FFF800,	size: 144 },
	{ name: "nvram",		offset: 0x03FFF890,	size: 1904 },

#elif defined(CONFIG_DP522) || defined(CONFIG_DP322)
        { name: "E*FlashFS",    offset: 0,              size:  2*1024*128},
        { name: "Application",  offset: 2*1024*128,     size: 29*1024*128},
        { name: "ATE",          offset: 31*1024*128,    size:  1*1024*128},
        { name: "BootLoader",   offset: 32*1024*128,    size:  1*1024*128},
        { name: "Kernel",       offset: 33*1024*128,    size:  7*1024*128},
        { name: "RootFS",       offset: 40*1024*128,    size: 10*1024*128},
        { name: "OpenTV",       offset: 50*1024*128,    size: 12*1024*128},
        { name: "BootRecovery", offset: 62*1024*128,    size:  2*1024*128}


#elif defined( CONFIG_MIPS_BRCM97XXX )
#define DEFAULT_SIZE_MB 16
	{ name: "rootfs",	offset: 0,		    size: 12*1024*1024 },
	{ name: "cfe",	        offset: 0x00C00000, size: 512*1024 },
	{ name: "vmlinux",	offset: 0x00C80000, size: 3582*1024 },
	{ name: "config",	offset: 0x00FFF800,	size: 144 },
	{ name: "nvram",	offset: 0x00FFF890,	size: 1904 },

#else
#error ("bcm7xxx-flash.c: Unknown/Unsupported platform\n")
#endif
};


int __init init_bcm9XXXX_map(void)
{
	printk(KERN_NOTICE "BCM97XXX flash device: 0x%8x at 0x%8x\n", WINDOW_SIZE, WINDOW_ADDR);
	bcm9XXXX_map.size = WINDOW_SIZE;
	/* Adjust partition table */
#ifdef DEFAULT_SIZE_MB
	if (WINDOW_SIZE != (DEFAULT_SIZE_MB << 20)) {
		int i;
		
		bcm9XXXX_parts[0].size += WINDOW_SIZE - (DEFAULT_SIZE_MB << 20);
//printk("Part[0] name=%s, size=%x, offset=%x\n", bcm9XXXX_parts[0].name, bcm9XXXX_parts[0].size, bcm9XXXX_parts[0].offset);
		for (i=1; i<ARRAY_SIZE(bcm9XXXX_parts); i++) {
			bcm9XXXX_parts[i].offset += WINDOW_SIZE - (DEFAULT_SIZE_MB << 20);
//printk("Part[%d] name=%s, size=%x, offset=%x\n", i, bcm9XXXX_parts[i].name, bcm9XXXX_parts[i].size, bcm9XXXX_parts[i].offset);
		}
	}
#endif
	bcm9XXXX_map.virt = (unsigned long)ioremap(WINDOW_ADDR, WINDOW_SIZE);

	if (!bcm9XXXX_map.virt) {
		printk("Failed to ioremap\n");
		return -EIO;
	}
	
	bcm9XXXX_mtd = do_map_probe("cfi_probe", &bcm9XXXX_map);
	if (!bcm9XXXX_mtd) {
		iounmap((void *)bcm9XXXX_map.virt);
		return -ENXIO;
	}
		
	add_mtd_partitions(bcm9XXXX_mtd, bcm9XXXX_parts, sizeof(bcm9XXXX_parts)/sizeof(bcm9XXXX_parts[0]));
	bcm9XXXX_mtd->owner = THIS_MODULE;
	return 0;
}

void __exit cleanup_bcm9XXXX_map(void)
{
	if (bcm9XXXX_mtd) {
		del_mtd_partitions(bcm9XXXX_mtd);
		map_destroy(bcm9XXXX_mtd);
	}
	if (bcm9XXXX_map.virt) {
		iounmap((void *)bcm9XXXX_map.virt);
		bcm9XXXX_map.virt = 0;
	}
}

module_init(init_bcm9XXXX_map);
module_exit(cleanup_bcm9XXXX_map);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Steven J. Hill <shill@broadcom.com>");
MODULE_DESCRIPTION("Broadcom 7xxx MTD map driver");
