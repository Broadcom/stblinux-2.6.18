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

    File: bcm7xxx_nand.c

    Description: 
    This is a device driver for the Broadcom NAND flash for bcm97xxx boards.
when	who what
-----	---	----
051011	tht	codings derived from OneNand generic.c implementation.
 */
 

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/brcmnand.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>
#include <asm/mach/flash.h>

#define DRIVER_NAME	"bcm7xxx-nand"

#ifdef CONFIG_MTD_PARTITIONS
static const char *part_probes[] = { "cmdlinepart", NULL,  };
#endif

/* Size and offset are variable, depending on the size of the chip, but 
 * cfe_kernel always starts at 1FC0_0000 and is 4MB size.
 */
static struct mtd_partition bcm7XXX_nand_parts[] = 
{
	{ name: "rootfs",		offset: 0,			size: 12*1024*1024 },
	{ name: "cfe_kernel",	offset: 0x00C00000,	size: 4*1024*1024 },
	//{ name: "vmlinux",	offset: 0x00C80000, size: 3582*1024 },
	//{ name: "config",	offset: 0x00FFF800,	size: 144 },
	//{ name: "nvram",	offset: 0x00FFF890,	size: 1904 },
};

struct brcmnand_info {
	struct mtd_info			mtd;
	struct mtd_partition*	parts;
	struct brcmnand_chip	brcmnand;
};

/* 
 * Size and offset are variable, depending on the size of the chip, but 
 * cfe_kernel always starts at 1FC0_0000 and is 4MB size.
 */
static void __devinit 
brcmnanddrv_setup_mtd_partitions(struct brcmnand_info* nandinfo)
{
	unsigned long size = nandinfo.brcmnand.chipsize;

	nandinfo->parts = bcm7XXX_nand_parts;
	nandinfo->parts[0].size = size - 4*1024*1024;
	nandinfo->parts[1].offset = nandinfo->parts[0].size;
}

static int __devinit brcmnanddrv_probe(struct device *dev)
{
	struct brcmnand_info *info;
	struct platform_device *pdev = to_platform_device(dev);
	struct flash_platform_data *pdata = pdev->dev.platform_data;
	struct resource *res = pdev->resource;
	unsigned long size = res->end - res->start + 1;
	int err;

	info = kmalloc(sizeof(struct brcmnand_info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	memset(info, 0, sizeof(struct brcmnand_info));

	info->brcmnand.base = BRCMNAND_CTRL_VBASE;

	//info->brcmnand.mmcontrol = NULL;  // THT: Sync Burst Read TBD.  pdata->mmcontrol;

	info->mtd.name = pdev->dev.bus_id;
	info->mtd.priv = &info->brcmnand;
	info->mtd.owner = THIS_MODULE;

	if (brcmnand_scan(&info->mtd, 1)) {
		err = -ENXIO;
		goto out_free_info;
	}

	
	brcmnanddrv_setup_mtd_partitions(&info);
	add_mtd_partitions(&info->mtd, info->parts, err);
	
	dev_set_drvdata(&pdev->dev, info);

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
	unsigned long size = res->end - res->start + 1;

	dev_set_drvdata(&pdev->dev, NULL);

	if (info) {
		if (info->parts)
			del_mtd_partitions(&info->mtd);
		//else
			//del_mtd_device(&info->mtd);

		brcmnand_release(&info->mtd);
		//release_mem_region(res->start, size);
		//iounmap(info->brcmnand.base);
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
	return driver_register(&bcm7xxx_nand_driver);
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

