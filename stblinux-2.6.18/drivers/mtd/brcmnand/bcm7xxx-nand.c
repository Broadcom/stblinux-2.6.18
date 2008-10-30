/*
 *
 *  drivers/mtd/brcmnand/bcm7xxx-nand.c
 *
    Copyright (c) 2005-2006 Broadcom Corporation                 
    
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 as
 published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    File: bcm7xxx-nand.c

    Description: 
    This is a device driver for the Broadcom NAND flash for bcm97xxx boards.
when	who what
-----	---	----
051011	tht	codings derived from OneNand generic.c implementation.
 */
 
#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/mtd/mtd.h>

#include <linux/mtd/partitions.h>


#include <asm/io.h>
//#include <asm/mach/flash.h>

#include <linux/mtd/brcmnand.h>
#include "brcmnand_priv.h"

#define PRINTK(...)
//#define PRINTK printk

#define DRIVER_NAME	"bcm7xxx-nand"
#define DRIVER_INFO "Broadcom STB NAND controller"

//#ifdef CONFIG_MTD_PARTITIONS
//static const char *part_probes[] = { "cmdlinepart", NULL,  };
//#endif

/* Size and offset are variable, depending on the size of the chip, but 
 * cfe_kernel always starts at 1FC0_0000 and is 4MB size.
 * Here is the layout of the NAND flash
 *
 *    Physical offset			Size			partition		Owner/comment
 *	<eof-4MB>	<eof>		4MB			BBT			BBT only if fsize > 512MB
 *	2000_0000	<eof - 1MB>	FlSize-256MB	data			Linux File System -- if fsize > 512MB
 *	1ff0_0000	1fff_ffff		1MB=8x128k 	BBT/or res	Linux RW -- if fsize <=512MB
 *	1fe0_0000	1fef_ffff		1MB=8x128k	nvm 		CFE, RO for Linux
 *	1fc0_0000	1fdf_ffff		2MB			CFE			CFE
 *
 *    [Optional] 1MB splash screen image partition carved out from the partition immediately 
 *	preceding the CFE partition when bcmsplash=1
 *    
 *	1f80_0000	1fbf_ffff		4MB			Linux Kernel	CFE
 *	start of flash	1f7f_ffff		flashSize-8MB	rootfs		Linux File System
 */
#define SMALLEST_FLASH_SIZE	(16<<20)
#define DEFAULT_RESERVED_SIZE 	(8<<20) 
#define DEFAULT_SPLASH_SIZE 	(1<<20)
#define DEFAULT_BBT0_SIZE_MB	(1)
#define DEFAULT_BBT1_SIZE_MB	(4)

#ifdef CONFIG_MTD_ECM_PARTITION
#define DEFAULT_OCAP_SIZE	(6<<20)
#define DEFAULT_AVAIL1_SIZE (32<<20)
#define DEFAULT_ECM_SIZE (DEFAULT_OCAP_SIZE+DEFAULT_AVAIL1_SIZE)
#define AVAIL1_PART	(1)
#define OCAP_PART	(2)
#else
#define DEFAULT_ECM_SIZE	(0)
#define DEFAULT_OCAP_SIZE	(0)
#define DEFAULT_AVAIL1_SIZE	(0)
#define AVAIL1_PART	(-1)
#define OCAP_PART	(-1)
#endif
#define DEFAULT_ROOTFS_SIZE (SMALLEST_FLASH_SIZE - DEFAULT_RESERVED_SIZE - DEFAULT_ECM_SIZE)

static struct mtd_partition bcm7XXX_nand_parts[] = 
{
#define ROOTFS_PART	(0)
	{ name: "rootfs",		offset: 0,					size: DEFAULT_ROOTFS_SIZE },	
#ifdef CONFIG_MTD_ECM_PARTITION
	{ name: "avail1",		offset: DEFAULT_ROOTFS_SIZE,	size: DEFAULT_AVAIL1_SIZE },
	{ name: "ocap",		offset: DEFAULT_ROOTFS_SIZE+DEFAULT_AVAIL1_SIZE,	size: DEFAULT_OCAP_SIZE },
#endif
	{ name: "kernel",	offset: 0x00800000,			size: 4<<20 },
	{ name: "cfe",		offset: 0x00C00000,			size: 2<<20 },
	{ name: "nvm",		offset: 0x00E00000,			size: 1<<20 },
	/* BBT 1MB not mountable by anyone */
	{ name: "data", 	offset: 0x20000000,		size: 0 },
/* Add 1 extra place-holder partition for splash, and a safety guard element */
	{name: NULL, offset: 0, size: 0},
	{name: NULL, offset: 0, size: 0}
};

struct brcmnand_info {
	struct mtd_info		mtd;
	struct mtd_partition*	parts;
	struct brcmnand_chip	brcmnand;
};
static struct brcmnand_info *info;

extern int gBcmSplash;

void* get_brcmnand_handle(void)
{
	void* handle = &info->brcmnand;
	return handle;
}
//EXPORT_SYMBOL(get_brcmnand_handle);


/* 
 * Size and offset are variable, depending on the size of the chip, but 
 * cfe_kernel always starts at 1FC0_0000 and is 4MB size.
 * The entire reserved area (kernel + CFE + BBT) occupies the last 8 MB of the flash.
 */
static void __devinit 
brcmnanddrv_setup_mtd_partitions(struct brcmnand_info* nandinfo, int *numParts)
{
	struct mtd_info* mtd = &nandinfo->mtd;
	unsigned long size; 
	int i = 0;
	unsigned int ecm_size = DEFAULT_ECM_SIZE;
#ifdef CONFIG_MTD_ECM_PARTITION
	unsigned int ocap_size = DEFAULT_OCAP_SIZE;
#endif
	unsigned int avail1_size = DEFAULT_AVAIL1_SIZE;

	if (device_size(mtd) <= (512<<20)) {
		size = (unsigned long) device_size(mtd);	// mtd->size may be different than nandinfo->size
		*numParts = ARRAY_SIZE(bcm7XXX_nand_parts) - 3; /* take into account the extra 2 parts
								   and the data partition */
	} else {
		size = 512 << 20;
		*numParts = ARRAY_SIZE(bcm7XXX_nand_parts) - 2; // take into account the extra 2 parts
	}

#ifdef CONFIG_MTD_ECM_PARTITION
	/* Do not generate AVAIL1 partition if usable flash size is less than 64MB */
	if (size < (64<<20)) {
		ecm_size = DEFAULT_OCAP_SIZE;
		bcm7XXX_nand_parts[AVAIL1_PART].size = avail1_size = 0;
		(*numParts)--;
	}
	else {
		int factor = size / (64 << 20); // Remember size is capped at 512MB
		
		bcm7XXX_nand_parts[OCAP_PART].size = ocap_size = factor*DEFAULT_OCAP_SIZE;
		bcm7XXX_nand_parts[AVAIL1_PART].size = avail1_size = factor*DEFAULT_AVAIL1_SIZE;
		ecm_size = ocap_size + avail1_size;
	}

#endif
	nandinfo->parts = bcm7XXX_nand_parts;
	bcm7XXX_nand_parts[0].size = size - DEFAULT_RESERVED_SIZE - ecm_size;
	bcm7XXX_nand_parts[0].ecclayout = mtd->ecclayout;
PRINTK("Part[%d] name=%s, size=%x, offset=%x\n", i, bcm7XXX_nand_parts[0].name, 
bcm7XXX_nand_parts[0].size, bcm7XXX_nand_parts[0].offset);

	for (i=1; i<(*numParts); i++) {
#ifdef CONFIG_MTD_ECM_PARTITION
		//if (0 == bcm7XXX_nand_parts[i].size)
		//	continue;
		/* Skip avail1 if size is less than 64 MB) */
 		if (0 == avail1_size && AVAIL1_PART == i) {
			bcm7XXX_nand_parts[i].offset = bcm7XXX_nand_parts[i-1].size + bcm7XXX_nand_parts[i-1].offset;
			continue;
		}
#endif

		bcm7XXX_nand_parts[i].offset = bcm7XXX_nand_parts[i-1].size + bcm7XXX_nand_parts[i-1].offset;
		// For now every partition uses the same oobinfo
		bcm7XXX_nand_parts[i].ecclayout = mtd->ecclayout;
PRINTK("Part[%d] name=%s, size=%x, offset=%x\n", i, bcm7XXX_nand_parts[i].name, 
bcm7XXX_nand_parts[i].size, bcm7XXX_nand_parts[i].offset);
	}

	
	if  (device_size(mtd) > (uint64_t) (512 << 20)) { // For total flash size > 512MB, we must split the rootfs into 2 partitions
		i = *numParts - 1;
		bcm7XXX_nand_parts[i].offset = 512 << 20;

		bcm7XXX_nand_parts[i].size = device_size(mtd) - ((512+DEFAULT_BBT1_SIZE_MB) << 20);
		bcm7XXX_nand_parts[i].ecclayout = mtd->ecclayout;
#ifdef CONFIG_MTD_ECM_PARTITION
PRINTK("Part[%d] name=%s, size=%x, offset=%x\n", avail1_size? i: i-1, bcm7XXX_nand_parts[i].name, 
bcm7XXX_nand_parts[i].size, bcm7XXX_nand_parts[i].offset);
#else
PRINTK("Part[%d] name=%s, size=%x, offset=%x\n", i, bcm7XXX_nand_parts[i].name, 
bcm7XXX_nand_parts[i].size, bcm7XXX_nand_parts[i].offset);
#endif

	}

#ifdef CONFIG_MTD_ECM_PARTITION
	/* Shift partitions 1 up if avail1_size is 0 */
	if (0 == avail1_size) {
		for (i=AVAIL1_PART; i < *numParts; i++) {
			bcm7XXX_nand_parts[i].offset = bcm7XXX_nand_parts[i+1].offset;
			bcm7XXX_nand_parts[i].size = bcm7XXX_nand_parts[i+1].size;
		}
		bcm7XXX_nand_parts[*numParts].offset = 0;
		bcm7XXX_nand_parts[*numParts].size = 0;
	}
#endif

#if 0
		/*
		 * Carve out 1MB for splash partition from previous partition,
		 * and inssert 1 partition of 1MB immediately above CFE.
		 */
		if (gBcmSplash && 0 == strcmp("cfe", bcm7XXX_nand_parts[i].name)) {
			int j;

			/* i now points to the CFE partition */
			bcm7XXX_nand_parts[i-1].size -= DEFAULT_SPLASH_SIZE;
			
			/* Move CFE and any partition that follows one down */
			for (j=ARRAY_SIZE(bcm7XXX_nand_parts) - 1; j >= i; j--) {
				bcm7XXX_nand_parts[j+1] = bcm7XXX_nand_parts[j];
			}	
			(*numParts)++;

			bcm7XXX_nand_parts[i].offset = bcm7XXX_nand_parts[i-1].size + bcm7XXX_nand_parts[i-1].offset;
			bcm7XXX_nand_parts[i].size = DEFAULT_SPLASH_SIZE;
			bcm7XXX_nand_parts[i].name = "splash";
			bcm7XXX_nand_parts[i].ecclayout = mtd->ecclayout;
			
			i++;
			/* i now points to the CFE partition which has been moved */
		}
#endif

	if (gBcmSplash) {		
PRINTK("In bcmSplash, numParts=%d\n", *numParts);
		for (i=0; i<*numParts; i++) {
	PRINTK("bcm7xxx-nand.c: i=%d\n", i);
	PRINTK("B4 Part[%d] name=%s, size=%x, offset=%x\n",  i, 
	bcm7XXX_nand_parts[i].name, bcm7XXX_nand_parts[i].size, bcm7XXX_nand_parts[i].offset);
			

			/* we will carve out 512KB from avail1 partition if it exists, or rootfs otherwise */
			if ((avail1_size && i == AVAIL1_PART) || (avail1_size == 0 && i == ROOTFS_PART)) {
				int j;

				/* i now points to the avail1 and/or rootfs partition */
				bcm7XXX_nand_parts[i].size -= DEFAULT_SPLASH_SIZE;
				if (i > 0) {
					bcm7XXX_nand_parts[i].offset = bcm7XXX_nand_parts[i-1].size + bcm7XXX_nand_parts[i-1].offset;
				}
				else {
					bcm7XXX_nand_parts[i].offset = 0;
				}
				
				/* Move all partitions that follow one down */
				for (j=*numParts - 1; j > i; j--) {
					bcm7XXX_nand_parts[j+1] = bcm7XXX_nand_parts[j];
	PRINTK("Moved partition[%d] down to [%d], name=%s\n", j, j+1, bcm7XXX_nand_parts[j+1].name);
				}	
				(*numParts)++;
				
	PRINTK("original: #parts=%d, Part[%d] name=%s, size=%x, offset=%x\n", *numParts,  i, 
bcm7XXX_nand_parts[i].name, bcm7XXX_nand_parts[i].size, bcm7XXX_nand_parts[i].offset);

				i++;
				/* i now points to the newly created splash partition */

				bcm7XXX_nand_parts[i].offset = bcm7XXX_nand_parts[i-1].size + bcm7XXX_nand_parts[i-1].offset;
				bcm7XXX_nand_parts[i].size = DEFAULT_SPLASH_SIZE;
				bcm7XXX_nand_parts[i].name = "splash";
	PRINTK("splash: #parts=%d, Part[%d] name=%s, size=%x, offset=%x\n", *numParts,  i, 
bcm7XXX_nand_parts[i].name, bcm7XXX_nand_parts[i].size, bcm7XXX_nand_parts[i].offset);
			}

		}
	}

}

static int __devinit brcmnanddrv_probe(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	//struct flash_platform_data *pdata = pdev->dev.platform_data;
	//struct resource *res = pdev->resource;
	//unsigned long size = res->end - res->start + 1;
	int err = 0;
	int numParts = 0;

	info = kmalloc(sizeof(struct brcmnand_info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	memset(info, 0, sizeof(struct brcmnand_info));

	info->brcmnand.buffers = kmalloc(sizeof(struct nand_buffers), GFP_DMA);
	if (!info->brcmnand.buffers) {
		kfree(info);
		return -ENOMEM;
	}

	memset(info->brcmnand.buffers, 0, sizeof(struct nand_buffers));

	info->brcmnand.numchips = 1; // For now, we only support 1 chip
	info->brcmnand.chip_shift = 0; // Only 1 chip
	//info->brcmnand.regs = pdev->resource[0].start;
	info->brcmnand.priv = &info->mtd;

	//info->brcmnand.mmcontrol = NULL;  // THT: Sync Burst Read TBD.  pdata->mmcontrol;

	info->mtd.name = pdev->dev.bus_id;
	info->mtd.priv = &info->brcmnand;
	info->mtd.owner = THIS_MODULE;

	/* Enable the following for a flash based bad block table */
	info->brcmnand.options |= NAND_USE_FLASH_BBT;
	

//printk("brcmnand_scan\n");
	if (brcmnand_scan(&info->mtd, MAX_NAND_CS)) {
		err = -ENXIO;
		goto out_free_info;
	}

//printk("	brcmnanddrv_setup_mtd_partitions\n");
	printk("	numchips=%d, size=%llx\n", info->brcmnand.numchips, device_size(&(info->mtd)));
	brcmnanddrv_setup_mtd_partitions(info, &numParts);
//printk("	add_mtd_partitions\n");
	add_mtd_partitions(&info->mtd, info->parts, numParts);
//printk("	dev_set_drvdata\n");	
	dev_set_drvdata(&pdev->dev, info);
//printk("<-- brcmnanddrv_probe\n");
	return 0;


out_free_info:
	kfree(info);

	return err;
}

static int __devexit brcmnanddrv_remove(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct brcmnand_info *info = dev_get_drvdata(&pdev->dev);
	//struct resource *res = pdev->resource;
	//unsigned long size = res->end - res->start + 1;

	dev_set_drvdata(&pdev->dev, NULL);

	if (info) {
		if (info->parts)
			del_mtd_partitions(&info->mtd);
		//else
			//del_mtd_device(&info->mtd);

		brcmnand_release(&info->mtd);
		//release_mem_region(res->start, size);
		//iounmap(info->brcmnand.base);
		kfree(info->brcmnand.buffers);
		kfree(info);
	}

	return 0;
}

static struct device_driver bcm7xxx_nand_driver = {
	.name		= DRIVER_NAME,
	.bus		= &platform_bus_type,
	.probe		= brcmnanddrv_probe,
	.remove		= __devexit_p(brcmnanddrv_remove),
};

MODULE_ALIAS(DRIVER_NAME);




MODULE_ALIAS(DRIVER_NAME);

static int __init brcmnanddrv_init(void)
{
	int err = 0;
	struct resource devRes[1];
	struct platform_device* pdev;
	unsigned long get_RAM_size(void);

#if 0 //def CONFIG_MIPS_BCM3548
	printk("brcmnand: not supported for 3548 in this release, exiting\n");
	return(-ENODEV);
#endif
	
	printk (DRIVER_INFO " (BrcmNand Controller)\n");

	memset(devRes, 0, sizeof(devRes));
	devRes[0].name = "brcmnand-base";
	devRes[0].start = BRCMNAND_CTRL_REGS;
	devRes[0].end = BRCMNAND_CTRL_REGS_END;
	devRes[0].flags = IORESOURCE_MEM;

	// Will need this whenever we use interrupt to look at the flash status
	//devRes[1].name ="brcmNand-irq";
	//devRes[1].start = tbd;
	//devRes[1].end = tbd;
	//devRes[1].flags = IORESOURCE_IRQ;

	// Before we register the driver, add a simple device matching our driver
	pdev = platform_device_register_simple(
		(char*) bcm7xxx_nand_driver.name,
		0, /* ID */
		devRes,
		ARRAY_SIZE(devRes));
	if (IS_ERR(pdev)) {
		printk("brcmnanddrv_init: device register failed, err=%d\n", err);
		return PTR_ERR(pdev);
	}

	// Set up dma_mask for our platform device
	// Overwrite whatever value it was set to.
	if (1 /*!pdev->dev.dma_mask*/) {
		//dma_set_mask(&pdev->dev, (u64) ((unsigned long) upper_memory - 1UL)); // default is 32MB 0x01ffffff;
		//dma_set_mask(&pdev->dev, 0x01ffffff);
		//pdev->dev.dma_mask = (u64*) 0x01ffffff;  
		pdev->dev.dma_mask = &pdev->dev.coherent_dma_mask; 
		pdev->dev.coherent_dma_mask = (u64)  ( get_RAM_size() - 1UL);
	}
		

	err = driver_register(&bcm7xxx_nand_driver);
	if (err) {
		printk("brcmnanddrv_init: driver_register failed, err=%d\n", err);
		return err;
	}
	return 0;
}

static void __exit brcmnanddrv_exit(void)
{
	driver_unregister(&bcm7xxx_nand_driver);
}

module_init(brcmnanddrv_init);
module_exit(brcmnanddrv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ton Truong <ttruong@broadcom.com>");
MODULE_DESCRIPTION("Broadcom NAND flash driver");

