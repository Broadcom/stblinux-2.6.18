/*
 *  Readonly Block Device Layer Over MTD
 *
 *  (C) 2006 Baydarov Konstantin <kbaidarov@dev.rtsoft.ru>
 *           Pantelis Antoniou <panto@intracom.gr>
 *           David Woodhouse <dwmw2@infradead.org>
 *
 *  It allows to use any filesystem on this device in
 *  RO mode and thus gain faster mount times and better
 *  throughput rates.
 *
 */

#include <linux/init.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/blktrans.h>

struct romblock_map {
	struct mtd_blktrans_dev dev;
	/* block map for RO */
	int32_t *block_map;
	int32_t block_top;
	int32_t block_scantop;
};

static loff_t map_over_bad_blocks(struct mtd_blktrans_dev* dev, loff_t from)
{
	int i, block;
	struct mtd_info *mtd = dev->mtd;
	struct romblock_map* dev_cont = container_of(dev, struct romblock_map, dev);
	int32_t *block_map = dev_cont->block_map;
	int32_t block_top = dev_cont->block_top;
	int32_t block_scantop = dev_cont->block_scantop;

	/* if no bad block checking is possible bail out */
	if (mtd->block_isbad == NULL)
		return from;

	/* first time in */
	if (block_map == NULL) {
		block_top = (int32_t) device_size(mtd) / mtd->erasesize;
		//block_map = kmalloc(sizeof(*block_map) * block_top, GFP_KERNEL);
		block_map = vmalloc(sizeof(*block_map) * block_top);
		if (block_map == NULL) {
			printk (KERN_ERR "map_over_bad_blocks(): unable to allocate block map\n");
			return -ENOMEM;
		}

		/* THT/Sidc: Update global struct */
		dev_cont->block_map = block_map;
		dev_cont->block_top = block_top;
		
		for (i = 0; i < block_top; i++)
			block_map[i] = -1;

		for (i = 0; i < block_top; i++)
			if ((*mtd->block_isbad)(mtd, i * mtd->erasesize) == 0)
				break;

		if (i >= block_top) {
			printk (KERN_WARNING "map_over_bad_blocks(): all blocks bad!\n");
			return -EIO;
		}
		block_scantop = 0;
		block_map[0] = i;

		DEBUG(MTD_DEBUG_LEVEL0, "mtd: map %d -> %d\n", block_scantop, block_map[block_scantop]);
	}

	block = ((int)from / mtd->erasesize);
	if (block >= block_top)
		return (loff_t)-1;

	/* scan for bad block up to where we want */
	while (block >= block_scantop) {
		/* find a non bad block */
		for (i = block_map[block_scantop] + 1; i < block_top; i++)
			if ((*mtd->block_isbad)(mtd, i * mtd->erasesize) == 0)
				break;

		/* exhausted ? */
		if (i >= block_top) {
			printk (KERN_WARNING "map_over_bad_blocks(): no more good blocks!\n");
			return (loff_t)-1;
		}

		block_map[++block_scantop] = i;
		DEBUG(MTD_DEBUG_LEVEL0, "mtd: map %d -> %d\n", block_scantop, block_map[block_scantop]);
		
		/* THT/Sidc: Update global struct */
		dev_cont->block_scantop = block_scantop;
	}

	block = block_map[(int)from / mtd->erasesize];
	from = (block * mtd->erasesize) | ((int)from & (mtd->erasesize - 1));
	
	return from;
}

static int romblock_readsect(struct mtd_blktrans_dev *dev,
			      unsigned long block, char *buf)
{
	size_t retlen;
	unsigned long from;

	from = map_over_bad_blocks(dev, block<<9);

	if (dev->mtd->read(dev->mtd, from, 512, &retlen, buf))
		return 1;
	return 0;
}


static int romblock_writesect(struct mtd_blktrans_dev *dev,
			      unsigned long block, char *buf)
{
	size_t retlen;
#if 0
/*
 * THT: I believe original codes are wrong, as we also need to adjust the write offset,
 * but since we never call WR op on this device, the fixed codes won't be compiled in.
 */
	unsigned long from;

	from = map_over_bad_blocks(dev, block<<9);
	if (dev->mtd->write(dev->mtd, from, 512, &retlen, buf))
		return 1;
#else

	if (dev->mtd->write(dev->mtd, (block * 512), 512, &retlen, buf))
		return 1;
#endif
	return 0;
}

static void romblock_add_mtd(struct mtd_blktrans_ops *tr, struct mtd_info *mtd)
{
	struct romblock_map *dev_cont = kmalloc(sizeof(*dev_cont), GFP_KERNEL);

	if (!dev_cont)
		return;

	memset(dev_cont, 0, sizeof(*dev_cont));

	dev_cont->dev.mtd = mtd;
	dev_cont->dev.devnum = mtd->index;
	dev_cont->dev.blksize = 512;
	/* size is in 512 byte sectors so a int type is ok for upto 512 GB */
	dev_cont->dev.size = device_size(mtd) >> 9;
	dev_cont->dev.tr = tr;
	dev_cont->dev.readonly = 1;

	add_mtd_blktrans_dev(&(dev_cont->dev));
}

static void romblock_remove_dev(struct mtd_blktrans_dev *dev)
{
	struct romblock_map* dev_cont = container_of(dev, struct romblock_map, dev);
	
	del_mtd_blktrans_dev(dev);
	/* THT: Free the map */
	if (dev_cont->block_map) {
		vfree(dev_cont->block_map);
		dev_cont->block_map = NULL;
	}
	kfree(dev_cont);
}

static struct mtd_blktrans_ops romblock_tr = {
	.name		= "romblock",
	.major		= 253,
	.part_bits	= 0,
	.readsect	= romblock_readsect,
	.writesect	= romblock_writesect,
	.add_mtd	= romblock_add_mtd,
	.remove_dev	= romblock_remove_dev,
	.owner		= THIS_MODULE,
};

static int __init romblock_init(void)
{
	return register_mtd_blktrans(&romblock_tr);
}

static void __exit romblock_exit(void)
{
	deregister_mtd_blktrans(&romblock_tr);
}

module_init(romblock_init);
module_exit(romblock_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Baydarov Konstantin <kbaidarov@dev.rtsoft.ru>");
MODULE_DESCRIPTION("Readonly Block Device Layer Over MTD");
