/*
 *  drivers/mtd/brcmnand/brcm_nand.c
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

    File: brcm_nand.c

    Description: 
    NAND driver for Samsung K9F1G08U0A chip with Broadcom NAND controller.
    The main difference between the Broadcom controller and OneNAND is that the Broadcom
    NAND controller is only 512B cache (bufferram) regardless of the flash chip, 
    whereas OneNAND has multiple bufferram's to match the page size.
    This complicates this driver quite a bit, because, for large page flash (2K page)
    we have to read in all 4 slides before we know for sure whether a page is bad.

when	who what
-----	---	----
051011	tht	codings derived from brcmnand_base implementation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/jiffies.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>

#include <asm/brcmstb/brcm97400a0/bchp_nand.h>

#include "brcm_nand.h"


#define DRIVER_NAME	"brcmnand"

/*
 * MTD structure for Broadcom NAND
 */
static struct mtd_info *brcmnand_mtd = NULL;

#ifdef CONFIG_MTD_PARTITIONS
static const char *part_probes[] = { "cmdlinepart", NULL,  };
#endif

typedef struct brcmnand_chip_Id {
    uint8 mafId, chipId;
	char* chipIdStr;
} brcmnand_chip_Id;

/*
 * List of supported chip
 */
static brcmnand_chip brcmnand_chips[] = {
	{	.chipId: SAMSUNG_K9F1G08U0A,
		.mafId: FLASHTYPE_SAMSUNG,
		.chipIdStr: "Samsung K9F1G08U0A",
	}
};

#define BRCMNAND_MAX_CHIPS (sizeof(brcmnand_chips)/sizeof(brcmnand_chip))


/**
 * brcmnand_oob oob info for 2K page
 */
static struct nand_oobinfo brcmnand_oob_64 = {
	.useecc		= MTD_NANDECC_AUTOPLACE,
	.eccbytes	= 12,
	.eccpos		= {
		6,7,8,
		20,21,22,
		36,37,38,
		52,53,54
		},
	.oobfree	= {	{2, 3}, {9,7}, 
					{18, 3}, {25, 7},
					{34, 3}, {41, 7},
					{50, 3}, {57, 7},
					}
};

/**
 * brcmnand_oob oob info for 512 page
 */
static struct nand_oobinfo brcmnand_oob_16 = {
	.useecc		= MTD_NANDECC_AUTOPLACE,
	.eccbytes	= 3,
	.eccpos		= {
		6,7,8
		},
	.oobfree	= { {0, 5}, {9,7} }
};

static const unsigned char ffchars[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,	/* 16 */
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,	/* 32 */
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,	/* 48 */
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,	/* 64 */
};

static uint32_t brcmnand_ctrl_read(uint32_t nandCtrlReg) 
{
	volatile unsigned long* pReg = (volatile unsigned long*) (BRCMNAND_CTRL_VBASE + nandCtrlReg);

	if (nandCtrlReg < BCHP_NAND_REVISION || nandCtrlReg > BCHP_NAND_BLK_WR_PROTECT ||
		(nandCtrlReg & 0xfffffffc) != 0)) {
		printk("brcmnand_ctrl_read: Invalid register value %08x\n", nandCtrlReg);
	}
	return (uint32_t) (*pReg);
}


static void brcmnand_ctrl_write(uint32_t nandCtrlReg, uint32_t val) 
{
	volatile unsigned long* pReg = (volatile unsigned long*) (BRCMNAND_CTRL_VBASE + nandCtrlReg);

	if (nandCtrlReg < BCHP_NAND_REVISION || nandCtrlReg > BCHP_NAND_BLK_WR_PROTECT ||
		(nandCtrlReg & 0xfffffffc) != 0)) {
		printk("brcmnand_ctrl_read: Invalid register value %08x\n", nandCtrlReg);
	}
	*pReg = (volatile unsigned long) val;
}


#if 0

/**
 * brcmnand_readw - [BrcmNAND Interface] Read BrcmNAND register
 * @param addr		address to read
 *
 * Read BrcmNAND register
 */
static unsigned short brcmnand_readw(void __iomem *addr)
{
	return readw(addr);
}

/**
 * brcmnand_writew - [BrcmNAND Interface] Write BrcmNAND register with value
 * @param value		value to write
 * @param addr		address to write
 *
 * Write BrcmNAND register with value
 */
static void brcmnand_writew(unsigned short value, void __iomem *addr)
{
	writew(value, addr);
}



/**
 * brcmnand_block_address - [DEFAULT] Get block address
 * @param this		onenand chip data structure
 * @param block		the block
 * @return		translated block address if DDP, otherwise same
 *
 * Setup Start Address 1 Register (F100h)
 */
static int brcmnand_block_address(struct brcmnand_chip *this, int block)
{
	if (this->device_id & ONENAND_DEVICE_IS_DDP) {
		/* Device Flash Core select, NAND Flash Block Address */
		int dfs = 0;

		if (block & this->density_mask)
			dfs = 1;

		return (dfs << ONENAND_DDP_SHIFT) |
			(block & (this->density_mask - 1));
	}

	return block;
}

/**
 * brcmnand_bufferram_address - [DEFAULT] Get bufferram address
 * @param this		onenand chip data structure
 * @param block		the block
 * @return		set DBS value if DDP, otherwise 0
 *
 * Setup Start Address 2 Register (F101h) for DDP
 */
static int brcmnand_bufferram_address(struct brcmnand_chip *this, int block)
{
	if (this->device_id & ONENAND_DEVICE_IS_DDP) {
		/* Device BufferRAM Select */
		int dbs = 0;

		if (block & this->density_mask)
			dbs = 1;

		return (dbs << ONENAND_DDP_SHIFT);
	}

	return 0;
}

/**
 * brcmnand_page_address - [DEFAULT] Get page address
 * @param page		the page address
 * @param sector	the sector address
 * @return		combined page and sector address
 *
 * Setup Start Address 8 Register (F107h)
 */
static int brcmnand_page_address(int page, int sector)
{
	/* Flash Page Address, Flash Sector Address */
	int fpa, fsa;

	fpa = page & ONENAND_FPA_MASK;
	fsa = sector & ONENAND_FSA_MASK;

	return ((fpa << ONENAND_FPA_SHIFT) | fsa);
}

/**
 * brcmnand_buffer_address - [DEFAULT] Get buffer address
 * @param dataram1	DataRAM index
 * @param sectors	the sector address
 * @param count		the number of sectors
 * @return		the start buffer value
 *
 * Setup Start Buffer Register (F200h)
 */
static int brcmnand_buffer_address(int dataram1, int sectors, int count)
{
	int bsa, bsc;

	/* BufferRAM Sector Address */
	bsa = sectors & ONENAND_BSA_MASK;

	if (dataram1)
		bsa |= ONENAND_BSA_DATARAM1;	/* DataRAM1 */
	else
		bsa |= ONENAND_BSA_DATARAM0;	/* DataRAM0 */

	/* BufferRAM Sector Count */
	bsc = count & ONENAND_BSC_MASK;

	return ((bsa << ONENAND_BSA_SHIFT) | bsc);
}



/**
 * brcmnand_command - [DEFAULT] Send command to BrcmNAND device
 * @param mtd		MTD device structure
 * @param cmd		the command to be sent
 * @param addr		offset to read from or write to
 * @param len		number of bytes to read or write
 *
 * Send command to BrcmNAND device. This function is used for middle/large page
 * devices (1KB/2KB Bytes per page)
 */
static int brcmnand_command(struct mtd_info *mtd, int cmd, loff_t addr, size_t len)
{
	struct brcmnand_chip *this = mtd->priv;
	int value, readcmd = 0, block_cmd = 0;
	int block, page;
	/* Now we use page size operation */
	int sectors = 4, count = 4;

	/* Address translation */
	switch (cmd) {
	case ONENAND_CMD_UNLOCK:
	case ONENAND_CMD_LOCK:
	case ONENAND_CMD_LOCK_TIGHT:
		block = -1;
		page = -1;
		break;

	case ONENAND_CMD_ERASE:
	case ONENAND_CMD_BUFFERRAM:
	case ONENAND_CMD_OTP_ACCESS:
		block_cmd = 1;
		block = (int) (addr >> this->erase_shift);
		page = -1;
		break;

	default:
		block = (int) (addr >> this->erase_shift);
		page = (int) (addr >> this->page_shift);
		page &= this->page_mask;
		break;
	}

	/* NOTE: The setting order of the registers is very important! */
	if (cmd == ONENAND_CMD_BUFFERRAM) {
		/* Select DataRAM for DDP */
		value = brcmnand_bufferram_address(this, block);
		this->write_word(value, this->base + ONENAND_REG_START_ADDRESS2);

		/* Switch to the next data buffer */
		ONENAND_SET_NEXT_BUFFERRAM(this);

		return 0;
	}

	if (block != -1) {
		/* Write 'DFS, FBA' of Flash */
		value = brcmnand_block_address(this, block);
		this->write_word(value, this->base + ONENAND_REG_START_ADDRESS1);

		if (block_cmd) {
			/* Select DataRAM for DDP */
			value = brcmnand_bufferram_address(this, block);
			this->write_word(value, this->base + ONENAND_REG_START_ADDRESS2);
		}
	}

	if (page != -1) {
		int dataram;

		switch (cmd) {
		case ONENAND_CMD_READ:
		case ONENAND_CMD_READOOB:
			dataram = ONENAND_SET_NEXT_BUFFERRAM(this);
			readcmd = 1;
			break;

		default:
			dataram = ONENAND_CURRENT_BUFFERRAM(this);
			break;
		}

		/* Write 'FPA, FSA' of Flash */
		value = brcmnand_page_address(page, sectors);
		this->write_word(value, this->base + ONENAND_REG_START_ADDRESS8);

		/* Write 'BSA, BSC' of DataRAM */
		value = brcmnand_buffer_address(dataram, sectors, count);
		this->write_word(value, this->base + ONENAND_REG_START_BUFFER);

		if (readcmd) {
			/* Select DataRAM for DDP */
			value = brcmnand_bufferram_address(this, block);
			this->write_word(value, this->base + ONENAND_REG_START_ADDRESS2);
		}
	}

	/* Interrupt clear */
	this->write_word(ONENAND_INT_CLEAR, this->base + ONENAND_REG_INTERRUPT);

	/* Write command */
	this->write_word(cmd, this->base + ONENAND_REG_COMMAND);

	return 0;
}

#endif

/**
 * brcmnand_wait - [DEFAULT] wait until the command is done
 * @param mtd		MTD device structure
 * @param state		state to select the max. timeout value
 *
 * Wait for command done. This applies to all BrcmNAND command
 * Read can take up to 53, erase up to ?s and program up to 30 clk cycle ()
 * according to general BrcmNAND specs
 */
static int brcmnand_wait(struct mtd_info *mtd, int state, uint32_t* pStatus)
{
	struct brcmnand_chip * this = mtd->priv;
	unsigned long timeout;
	uint32_t ready;

	/* The 20 msec is enough */
	timeout = jiffies + msecs_to_jiffies(20);
	while (time_before(jiffies, timeout)) {
		ready = this->ctrl_read(BCHP_NAND_INTFC_STATUS);

		if (ready & 0x80000000) {
			*pStatus = ready;
			return 0;
		}

		if (state != FL_READING)
			cond_resched();
		//touch_softlockup_watchdog();
	}

	/*
	 * Get here on timeout
	 */
	return -ETIMEDOUT;
}

static int brcmnand_cache_is_valid(struct mtd_info* mtd,  int state) 
{
	struct brcmnand_chip * this = mtd->priv;
	unsigned long timeout;
	uint32_t ready;
	
	/* The 20 msec is enough */
	timeout = jiffies + msecs_to_jiffies(20);
	while (time_before(jiffies, timeout)) {
		ready = this->ctrl_read(BCHP_NAND_INTFC_STATUS);

		if ((ready & 0xa0000000) == 0xa0000000){
			return 1;
		}
		if (state != FL_READING)
			cond_resched();
	}
}

static int brcmnand_spare_is_valid(struct mtd_info* mtd,  int state) 
{
	struct brcmnand_chip * this = mtd->priv;
	unsigned long timeout;
	uint32_t ready;
	
	/* The 20 msec is enough */
	timeout = jiffies + msecs_to_jiffies(20);
	while (time_before(jiffies, timeout)) {
		ready = this->ctrl_read(BCHP_NAND_INTFC_STATUS);

		if ((ready & 0x90000000) == 0x90000000) {
			return 1;
		}
		if (state != FL_READING)
			cond_resched();
	}
}

#if 0
/**
 * brcmnand_bufferram_offset - [DEFAULT] BufferRAM offset
 * @param mtd		MTD data structure
 * @param area		BufferRAM area
 * @return		offset given area
 *
 * Return BufferRAM offset given area
 */
static inline int brcmnand_bufferram_offset(struct mtd_info *mtd, int area)
{
	struct brcmnand_chip *this = mtd->priv;

	if (ONENAND_CURRENT_BUFFERRAM(this)) {
		if (area == ONENAND_DATARAM)
			return mtd->oobblock;
		if (area == ONENAND_SPARERAM)
			return mtd->oobsize;
	}

	return 0;
}
#endif

#if 0


/**
 * brcmnand_read_bufferram - [BrcmNAND Interface] Read the bufferram area
 * @param mtd		MTD data structure
 * @param area		BufferRAM area
 * @param buffer	the databuffer to put/get data
 * @param offset	offset to read from or write to
 * @param count		number of bytes to read/write
 *
 * Read the BufferRAM area
 */
static int brcmnand_read_bufferram(struct mtd_info *mtd, /*int area, */
		unsigned char *buffer, int offset, size_t count)
{
	struct brcmnand_chip *this = mtd->priv;
	char __iomem *bufferram;

	/* Assuming count and offset have been validated prior to calling */
	bufferram = this->base;

	memcpy(buffer, bufferram + offset, count);

	return 0;
}


#if 0

/**
 * brcmnand_sync_read_bufferram - [BrcmNAND Interface] Read the bufferram area with Sync. Burst mode
 * @param mtd		MTD data structure
 * @param area		BufferRAM area
 * @param buffer	the databuffer to put/get data
 * @param offset	offset to read from or write to
 * @param count		number of bytes to read/write
 *
 * Read the BufferRAM area with Sync. Burst Mode
 */


static int brcmnand_sync_read_bufferram(struct mtd_info *mtd, int area,
		unsigned char *buffer, int offset, size_t count)
{
	struct brcmnand_chip *this = mtd->priv;
	void __iomem *bufferram;

	bufferram = this->base + area;

	bufferram += brcmnand_bufferram_offset(mtd, area);

	this->mmcontrol(mtd, ONENAND_SYS_CFG1_SYNC_READ);

	if (ONENAND_CHECK_BYTE_ACCESS(count)) {
		unsigned short word;

		/* Align with word(16-bit) size */
		count--;

		/* Read word and save byte */
		word = this->read_word(bufferram + offset + count);
		buffer[count] = (word & 0xff);
	}

	memcpy(buffer, bufferram + offset, count);

	this->mmcontrol(mtd, 0);

	return 0;
}

#endif

/**
 * brcmnand_write_bufferram - [BrcmNAND Interface] Write the bufferram area
 * @param mtd		MTD data structure
 * @param area		BufferRAM area
 * @param buffer	the databuffer to put/get data
 * @param offset	offset to read from or write to
 * @param count		number of bytes to read/write
 *
 * Write the BufferRAM area
 */
static int brcmnand_write_bufferram(struct mtd_info *mtd,/* int area, */
		const unsigned char *buffer, int offset, size_t count)
{
	struct brcmnand_chip *this = mtd->priv;
	char __iomem *bufferram;

	bufferram = this->base;

	memcpy(bufferram + offset, buffer, count);

	return 0;
}



/**
 * brcmnand_check_bufferram - [GENERIC] Check BufferRAM information
 * @param mtd		MTD data structure
 * @param addr		address to check
 * @return		1 if there are valid data, otherwise 0
 *
 * Check bufferram if there is data we required
 */
static int brcmnand_check_bufferram(struct mtd_info *mtd, loff_t addr)
{
	struct brcmnand_chip *this = mtd->priv;
	int block, page, slide = 0;
	int i;

	block = (int) (addr >> this->erase_shift);
	page = (int) (addr >> this->page_shift);
	page &= this->page_mask;
	if (this->pageSize != NFC_FLASHCACHE_SIZE)
		slide = (int) (page >> 9);
	}

	/* Is there valid data? */
	if (this->bufferram.block == block &&
	    this->bufferram.page == page &&
	    this->bufferram.slide == slide &&
	    this->bufferram.valid)
		return 1;

	return 0;
}

/**
 * brcmnand_update_bufferram - [GENERIC] Update BufferRAM information
 * @param mtd		MTD data structure
 * @param addr		address to update
 * @param valid		valid flag
 *
 * Update BufferRAM information
 */
static int brcmnand_update_bufferram(struct mtd_info *mtd, loff_t addr,
		int valid)
{
	struct brcmnand_chip *this = mtd->priv;
	int block, page, slide = 0;
	int i;

	block = (int) (addr >> this->erase_shift);
	page = (int) (addr >> this->page_shift);
	page &= this->page_mask;
	if (this->pageSize != NFC_FLASHCACHE_SIZE)
		slide = (int) (page >> 9);
	}

	/* Invalidate BufferRAM */
	for (i = 0; i < MAX_BUFFERRAM; i++) {
		if (this->bufferram.block == block &&
		    this->bufferram.page == page &&
		    this->bufferram.slide == slide)
		    
			this->bufferram.valid = 0;
	}

	
	this->bufferram.block = block;
	this->bufferram.page = page;
	this->bufferram.slide = slide;
	this->bufferram.valid = valid;

	return 0;
}

#endif



/**
 * brcmnand_get_device - [GENERIC] Get chip for selected access
 * @param mtd		MTD device structure
 * @param new_state	the state which is requested
 *
 * Get the device and lock it for exclusive access
 */
static int brcmnand_get_device(struct mtd_info *mtd, int new_state)
{
	struct brcmnand_chip *this = mtd->priv;
	DECLARE_WAITQUEUE(wait, current);

	/*
	 * Grab the lock and see if the device is available
	 */
	while (1) {
		spin_lock(&this->chip_lock);
#if 0
		if (atomic_dec(this->semCount)) == 0) {
			// Write 1 to Semaphore register TBD
			brcmnand_ctrl_write(BCHP_NAND_SEMAPHORE, 1);
		}
		else { /* In Use by another thread */
			atomic_inc(this->semCount);
			continue;
		}
#endif
		if (this->state == FL_READY) {
			this->state = new_state;
			spin_unlock(&this->chip_lock);
			break;
		}
		if (new_state == FL_PM_SUSPENDED) {
			spin_unlock(&this->chip_lock);
			return (this->state == FL_PM_SUSPENDED) ? 0 : -EAGAIN;
		}
		set_current_state(TASK_UNINTERRUPTIBLE);
		add_wait_queue(&this->wq, &wait);
		spin_unlock(&this->chip_lock);
		schedule();
		remove_wait_queue(&this->wq, &wait);
	}

	return 0;
}

/**
 * brcmnand_release_device - [GENERIC] release chip
 * @param mtd		MTD device structure
 *
 * Deselect, release chip lock and wake up anyone waiting on the device
 */
static void brcmnand_release_device(struct mtd_info *mtd)
{
	struct brcmnand_chip *this = mtd->priv;

	/* Release the chip */
	spin_lock(&this->chip_lock);
	this->state = FL_READY;
	wake_up(&this->wq);
	spin_unlock(&this->chip_lock);
}

/**
 * brcmnand_read_ecc - [MTD Interface] Read data with ECC
 * @param mtd		MTD device structure
 * @param from		offset to read from
 * @param len		number of bytes to read
 * @param retlen	pointer to variable to store the number of read bytes
 * @param buf		the databuffer to put data
 * @param oob_buf	filesystem supplied oob data buffer
 * @param oobsel	oob selection structure
 *
 * BrcmNAND read with ECC
 * returns 0 on success, -errno on error.
 */
static int brcmnand_read_ecc(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf,
	u_char *oob_buf, struct nand_oobinfo *oobsel)
{
	struct brcmnand_chip *this = mtd->priv;
	int read = 0, column;
	int thislen;
	int ret = 0;

	DEBUG(MTD_DEBUG_LEVEL3, "brcmnand_read_ecc: from = 0x%08x, len = %i\n", (unsigned int) from, (int) len);

	/* Do not allow reads past end of device */
	if (unlikely(from + len) > mtd->size) {
		DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_read_ecc: Attempt read beyond end of device\n");
		*retlen = 0;
		return -EINVAL;
	}

	/* Grab the lock and see if the device is available */
	brcmnand_get_device(mtd, FL_READING);

	/* TODO handling oob */

	while (read < len) {
		thislen = min_t(int, mtd->oobblock, len - read);

		column = from & (mtd->oobblock - 1);
		if (column + thislen > mtd->oobblock)
			thislen = mtd->oobblock - column;

#if 0
//THT: This is done in HW in our platform
		if (!brcmnand_check_bufferram(mtd, from)) {
			this->command(mtd, ONENAND_CMD_READ, from, mtd->oobblock);

			ret = this->wait(mtd, FL_READING);
			/* First copy data and check return value for ECC handling */
			brcmnand_update_bufferram(mtd, from, 1);
		}

		this->read_bufferram(mtd, ONENAND_DATARAM, buf, column, thislen);
#endif
		/* Immediate read till end of slice */
		/* THT Check buswidth here */
		memcpy(buf, &this->base[column], thislen);
		if (!brcmnand_cache_is_valid(this)) {
			DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_read_ecc: read failed = %d\n", ret);
			// Do BTT management here
			goto out;
		}
		read += thislen;

		if (read == len)
			break;

		if (ret) {
			DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_read_ecc: read failed = %d\n", ret);
			goto out;
		}

		from += thislen;
		buf += thislen;
	}

out:
	/* Deselect and wake up anyone waiting on the device */
	brcmnand_release_device(mtd);

	/*
	 * Return success, if no ECC failures, else -EBADMSG
	 * fs driver will take care of that, because
	 * retlen == desired len and result == -EBADMSG
	 */
	*retlen = read;
	return ret;
}

/**
 * brcmnand_read - [MTD Interface] MTD compability function for brcmnand_read_ecc
 * @param mtd		MTD device structure
 * @param from		offset to read from
 * @param len		number of bytes to read
 * @param retlen	pointer to variable to store the number of read bytes
 * @param buf		the databuffer to put data
 *
 * This function simply calls brcmnand_read_ecc with oob buffer and oobsel = NULL
*/
static int brcmnand_read(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf)
{
	return brcmnand_read_ecc(mtd, from, len, retlen, buf, NULL, NULL);
}

/**
 * brcmnand_read_oob - [MTD Interface] BrcmNAND read out-of-band
 * @param mtd		MTD device structure
 * @param from		offset to read from
 * @param len		number of bytes to read
 * @param retlen	pointer to variable to store the number of read bytes
 * @param buf		the databuffer to put data
 *
 * BrcmNAND read out-of-band data from the spare area
 */
static int brcmnand_read_oob(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf)
{
	struct brcmnand_chip *this = mtd->priv;
	int read = 0, thislen, column;
	int ret = 0;

	DEBUG(MTD_DEBUG_LEVEL3, "brcmnand_read_oob: from = 0x%08x, len = %i\n", (unsigned int) from, (int) len);

	/* Initialize return length value */
	*retlen = 0;

	/* Do not allow reads past end of device */
	if (unlikely((from + len) > mtd->size)) {
		DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_read_oob: Attempt read beyond end of device\n");
		return -EINVAL;
	}

	/* Grab the lock and see if the device is available */
	brcmnand_get_device(mtd, FL_READING);

	column = from & (mtd->oobsize - 1);

	while (read < len) {
		thislen = mtd->oobsize - column;
		thislen = min_t(int, thislen, len);

		this->command(mtd, ONENAND_CMD_READOOB, from, mtd->oobsize);

		brcmnand_update_bufferram(mtd, from, 0);

		ret = this->wait(mtd, FL_READING);
		/* First copy data and check return value for ECC handling */

		this->read_bufferram(mtd, ONENAND_SPARERAM, buf, column, thislen);

		read += thislen;

		if (read == len)
			break;

		if (ret) {
			DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_read_oob: read failed = %d\n", ret);
			goto out;
		}

		buf += thislen;

		/* Read more? */
		if (read < len) {
			/* Page size */
			from += mtd->oobblock;
			column = 0;
		}
	}

out:
	/* Deselect and wake up anyone waiting on the device */
	brcmnand_release_device(mtd);

	*retlen = read;
	return ret;
}

#ifdef CONFIG_MTD_BRCMNAND_VERIFY_WRITE
/**
 * brcmnand_verify_oob - [GENERIC] verify the oob contents after a write
 * @param mtd		MTD device structure
 * @param buf		the databuffer to verify
 * @param to		offset to read from
 * @param len		number of bytes to read and compare
 *
 */
static int brcmnand_verify_oob(struct mtd_info *mtd, const u_char *buf, loff_t to, int len)
{
	struct brcmnand_chip *this = mtd->priv;
	char *readp = this->page_buf;
	int column = to & (mtd->oobsize - 1);
	int status, i;

	this->command(mtd, ONENAND_CMD_READOOB, to, mtd->oobsize);
	brcmnand_update_bufferram(mtd, to, 0);
	status = this->wait(mtd, FL_READING);
	if (status)
		return status;
		
	this->read_bufferram(mtd, ONENAND_SPARERAM, readp, column, len);
	
	for(i = 0; i < len; i++)
		if (buf[i] != 0xFF && buf[i] != readp[i])
			return -EBADMSG;

	return 0;
}

/**
 * brcmnand_verify_page - [GENERIC] verify the chip contents after a write
 * @param mtd		MTD device structure
 * @param buf		the databuffer to verify
 *
 * Check DataRAM area directly
 */
static int brcmnand_verify_page(struct mtd_info *mtd, u_char *buf, loff_t addr)
{
	struct brcmnand_chip *this = mtd->priv;
	void __iomem *dataram0, *dataram1;
	int ret = 0;

	this->command(mtd, ONENAND_CMD_READ, addr, mtd->oobblock);

	ret = this->wait(mtd, FL_READING);
	if (ret)
		return ret;

	brcmnand_update_bufferram(mtd, addr, 1);

	/* Check, if the two dataram areas are same */
	dataram0 = this->base + ONENAND_DATARAM;
	dataram1 = dataram0 + mtd->oobblock;

	if (memcmp(dataram0, dataram1, mtd->oobblock))
		return -EBADMSG;

	return 0;
}
#else
#define brcmnand_verify_page(...)	(0)
#define brcmnand_verify_oob(...)		(0)
#endif

#define NOTALIGNED(x)	((x & (mtd->oobblock - 1)) != 0)

/**
 * brcmnand_write_ecc - [MTD Interface] BrcmNAND write with ECC
 * @param mtd		MTD device structure
 * @param to		offset to write to
 * @param len		number of bytes to write
 * @param retlen	pointer to variable to store the number of written bytes
 * @param buf		the data to write
 * @param eccbuf	filesystem supplied oob data buffer
 * @param oobsel	oob selection structure
 *
 * BrcmNAND write with ECC
 */
static int brcmnand_write_ecc(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf,
	u_char *eccbuf, struct nand_oobinfo *oobsel)
{
	struct brcmnand_chip *this = mtd->priv;
	int written = 0;
	int ret = 0;

	DEBUG(MTD_DEBUG_LEVEL3, "brcmnand_write_ecc: to = 0x%08x, len = %i\n", (unsigned int) to, (int) len);

	/* Initialize retlen, in case of early exit */
	*retlen = 0;

	/* Do not allow writes past end of device */
	if (unlikely((to + len) > mtd->size)) {
		DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_write_ecc: Attempt write to past end of device\n");
		return -EINVAL;
	}

	/* Reject writes, which are not page aligned */
        if (unlikely(NOTALIGNED(to)) || unlikely(NOTALIGNED(len))) {
                DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_write_ecc: Attempt to write not page aligned data\n");
                return -EINVAL;
        }

	/* Grab the lock and see if the device is available */
	brcmnand_get_device(mtd, FL_WRITING);

	/* Loop until all data write */
	while (written < len) {
		int thislen = min_t(int, mtd->oobblock, len - written);

		this->command(mtd, ONENAND_CMD_BUFFERRAM, to, mtd->oobblock);

		this->write_bufferram(mtd, ONENAND_DATARAM, buf, 0, thislen);
		this->write_bufferram(mtd, ONENAND_SPARERAM, ffchars, 0, mtd->oobsize);

		this->command(mtd, ONENAND_CMD_PROG, to, mtd->oobblock);

		brcmnand_update_bufferram(mtd, to, 1);

		ret = this->wait(mtd, FL_WRITING);
		if (ret) {
			DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_write_ecc: write filaed %d\n", ret);
			goto out;
		}

		written += thislen;

		/* Only check verify write turn on */
		ret = brcmnand_verify_page(mtd, (u_char *) buf, to);
		if (ret) {
			DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_write_ecc: verify failed %d\n", ret);
			goto out;
		}

		if (written == len)
			break;

		to += thislen;
		buf += thislen;
	}

out:
	/* Deselect and wake up anyone waiting on the device */
	brcmnand_release_device(mtd);

	*retlen = written;

	return ret;
}

/**
 * brcmnand_write - [MTD Interface] compability function for brcmnand_write_ecc
 * @param mtd		MTD device structure
 * @param to		offset to write to
 * @param len		number of bytes to write
 * @param retlen	pointer to variable to store the number of written bytes
 * @param buf		the data to write
 *
 * This function simply calls brcmnand_write_ecc
 * with oob buffer and oobsel = NULL
 */
static int brcmnand_write(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf)
{
	return brcmnand_write_ecc(mtd, to, len, retlen, buf, NULL, NULL);
}

/**
 * brcmnand_write_oob - [MTD Interface] BrcmNAND write out-of-band
 * @param mtd		MTD device structure
 * @param to		offset to write to
 * @param len		number of bytes to write
 * @param retlen	pointer to variable to store the number of written bytes
 * @param buf		the data to write
 *
 * BrcmNAND write out-of-band
 */
static int brcmnand_write_oob(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf)
{
	struct brcmnand_chip *this = mtd->priv;
	int column, ret = 0;
	int written = 0;

	DEBUG(MTD_DEBUG_LEVEL3, "brcmnand_write_oob: to = 0x%08x, len = %i\n", (unsigned int) to, (int) len);

	/* Initialize retlen, in case of early exit */
	*retlen = 0;

	/* Do not allow writes past end of device */
	if (unlikely((to + len) > mtd->size)) {
		DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_write_oob: Attempt write to past end of device\n");
		return -EINVAL;
	}

	/* Grab the lock and see if the device is available */
	brcmnand_get_device(mtd, FL_WRITING);

	/* Loop until all data write */
	while (written < len) {
		int thislen = min_t(int, mtd->oobsize, len - written);

		column = to & (mtd->oobsize - 1);

		this->command(mtd, ONENAND_CMD_BUFFERRAM, to, mtd->oobsize);

		/* We send data to spare ram with oobsize
		 * to prevent byte access */
		memset(this->page_buf, 0xff, mtd->oobsize);
		memcpy(this->page_buf + column, buf, thislen);
		this->write_bufferram(mtd, ONENAND_SPARERAM, this->page_buf, 0, mtd->oobsize);

		this->command(mtd, ONENAND_CMD_PROGOOB, to, mtd->oobsize);

		brcmnand_update_bufferram(mtd, to, 0);

		ret = this->wait(mtd, FL_WRITING);
		if (ret) {
			DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_write_oob: write filaed %d\n", ret);
			goto out;
		}

		ret = brcmnand_verify_oob(mtd, buf, to, thislen);
		if (ret) {
			DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_write_oob: verify failed %d\n", ret);
			goto out;
		}

		written += thislen;

		if (written == len)
			break;

		to += thislen;
		buf += thislen;
	}

out:
	/* Deselect and wake up anyone waiting on the device */
	brcmnand_release_device(mtd);

	*retlen = written;

	return ret;
}

/**
 * brcmnand_writev_ecc - [MTD Interface] write with iovec with ecc
 * @param mtd		MTD device structure
 * @param vecs		the iovectors to write
 * @param count		number of vectors
 * @param to		offset to write to
 * @param retlen	pointer to variable to store the number of written bytes
 * @param eccbuf	filesystem supplied oob data buffer
 * @param oobsel	oob selection structure
 *
 * BrcmNAND write with iovec with ecc
 */
static int brcmnand_writev_ecc(struct mtd_info *mtd, const struct kvec *vecs,
	unsigned long count, loff_t to, size_t *retlen,
	u_char *eccbuf, struct nand_oobinfo *oobsel)
{
	struct brcmnand_chip *this = mtd->priv;
	unsigned char *pbuf;
	size_t total_len, len;
	int i, written = 0;
	int ret = 0;

	/* Preset written len for early exit */
	*retlen = 0;

	/* Calculate total length of data */
	total_len = 0;
	for (i = 0; i < count; i++)
		total_len += vecs[i].iov_len;

	DEBUG(MTD_DEBUG_LEVEL3, "brcmnand_writev_ecc: to = 0x%08x, len = %i, count = %ld\n", (unsigned int) to, (unsigned int) total_len, count);

	/* Do not allow write past end of the device */
	if (unlikely((to + total_len) > mtd->size)) {
		DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_writev_ecc: Attempted write past end of device\n");
		return -EINVAL;
	}

	/* Reject writes, which are not page aligned */
        if (unlikely(NOTALIGNED(to)) || unlikely(NOTALIGNED(total_len))) {
                DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_writev_ecc: Attempt to write not page aligned data\n");
                return -EINVAL;
        }

	/* Grab the lock and see if the device is available */
	brcmnand_get_device(mtd, FL_WRITING);

	/* TODO handling oob */

	/* Loop until all keve's data has been written */
	len = 0;
	while (count) {
		pbuf = this->page_buf;
		/*
		 * If the given tuple is >= pagesize then
		 * write it out from the iov
		 */
		if ((vecs->iov_len - len) >= mtd->oobblock) {
			pbuf = vecs->iov_base + len;

			len += mtd->oobblock;

			/* Check, if we have to switch to the next tuple */
			if (len >= (int) vecs->iov_len) {
				vecs++;
				len = 0;
				count--;
			}
		} else {
			int cnt = 0, thislen;
			while (cnt < mtd->oobblock) {
				thislen = min_t(int, mtd->oobblock - cnt, vecs->iov_len - len);
				memcpy(this->page_buf + cnt, vecs->iov_base + len, thislen);
				cnt += thislen;
				len += thislen;

				/* Check, if we have to switch to the next tuple */
				if (len >= (int) vecs->iov_len) {
					vecs++;
					len = 0;
					count--;
				}
			}
		}

		this->command(mtd, ONENAND_CMD_BUFFERRAM, to, mtd->oobblock);

		this->write_bufferram(mtd, ONENAND_DATARAM, pbuf, 0, mtd->oobblock);
		this->write_bufferram(mtd, ONENAND_SPARERAM, ffchars, 0, mtd->oobsize);

		this->command(mtd, ONENAND_CMD_PROG, to, mtd->oobblock);

		brcmnand_update_bufferram(mtd, to, 1);

		ret = this->wait(mtd, FL_WRITING);
		if (ret) {
			DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_writev_ecc: write failed %d\n", ret);
			goto out;
		}


		/* Only check verify write turn on */
		ret = brcmnand_verify_page(mtd, (u_char *) pbuf, to);
		if (ret) {
			DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_writev_ecc: verify failed %d\n", ret);
			goto out;
		}

		written += mtd->oobblock;

		to += mtd->oobblock;
	}

out:
	/* Deselect and wakt up anyone waiting on the device */
	brcmnand_release_device(mtd);

	*retlen = written;

	return 0;
}

/**
 * brcmnand_writev - [MTD Interface] compabilty function for brcmnand_writev_ecc
 * @param mtd		MTD device structure
 * @param vecs		the iovectors to write
 * @param count		number of vectors
 * @param to		offset to write to
 * @param retlen	pointer to variable to store the number of written bytes
 *
 * BrcmNAND write with kvec. This just calls the ecc function
 */
static int brcmnand_writev(struct mtd_info *mtd, const struct kvec *vecs,
	unsigned long count, loff_t to, size_t *retlen)
{
	return brcmnand_writev_ecc(mtd, vecs, count, to, retlen, NULL, NULL);
}

/**
 * brcmnand_block_checkbad - [GENERIC] Check if a block is marked bad
 * @param mtd		MTD device structure
 * @param ofs		offset from device start
 * @param getchip	0, if the chip is already selected
 * @param allowbbt	1, if its allowed to access the bbt area
 *
 * Check, if the block is bad. Either by reading the bad block table or
 * calling of the scan function.
 */
static int brcmnand_block_checkbad(struct mtd_info *mtd, loff_t ofs, int getchip, int allowbbt)
{
	struct brcmnand_chip *this = mtd->priv;
	struct bbm_info *bbm = this->bbm;

	/* Return info from the table */
	return bbm->isbad_bbt(mtd, ofs, allowbbt);
}

/**
 * brcmnand_erase - [MTD Interface] erase block(s)
 * @param mtd		MTD device structure
 * @param instr		erase instruction
 *
 * Erase one ore more blocks
 */
static int brcmnand_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct brcmnand_chip *this = mtd->priv;
	unsigned int block_size;
	loff_t addr;
	int len;
	int ret = 0;

	DEBUG(MTD_DEBUG_LEVEL3, "brcmnand_erase: start = 0x%08x, len = %i\n", (unsigned int) instr->addr, (unsigned int) instr->len);

	block_size = (1 << this->erase_shift);

	/* Start address must align on block boundary */
	if (unlikely(instr->addr & (block_size - 1))) {
		DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_erase: Unaligned address\n");
		return -EINVAL;
	}

	/* Length must align on block boundary */
	if (unlikely(instr->len & (block_size - 1))) {
		DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_erase: Length not block aligned\n");
		return -EINVAL;
	}

	/* Do not allow erase past end of device */
	if (unlikely((instr->len + instr->addr) > mtd->size)) {
		DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_erase: Erase past end of device\n");
		return -EINVAL;
	}

	instr->fail_addr = 0xffffffff;

	/* Grab the lock and see if the device is available */
	brcmnand_get_device(mtd, FL_ERASING);

	/* Loop throught the pages */
	len = instr->len;
	addr = instr->addr;

	instr->state = MTD_ERASING;

	while (len) {

		/* Check if we have a bad block, we do not erase bad blocks */
		if (brcmnand_block_checkbad(mtd, addr, 0, 0)) {
			printk (KERN_WARNING "brcmnand_erase: attempt to erase a bad block at addr 0x%08x\n", (unsigned int) addr);
			instr->state = MTD_ERASE_FAILED;
			goto erase_exit;
		}

		this->command(mtd, ONENAND_CMD_ERASE, addr, block_size);

		ret = this->wait(mtd, FL_ERASING);
		/* Check, if it is write protected */
		if (ret) {
			if (ret == -EPERM)
				DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_erase: Device is write protected!!!\n");
			else
				DEBUG(MTD_DEBUG_LEVEL0, "brcmnand_erase: Failed erase, block %d\n", (unsigned) (addr >> this->erase_shift));
			instr->state = MTD_ERASE_FAILED;
			instr->fail_addr = addr;
			goto erase_exit;
		}

		len -= block_size;
		addr += block_size;
	}

	instr->state = MTD_ERASE_DONE;

erase_exit:

	ret = instr->state == MTD_ERASE_DONE ? 0 : -EIO;
	/* Do call back function */
	if (!ret)
		mtd_erase_callback(instr);

	/* Deselect and wake up anyone waiting on the device */
	brcmnand_release_device(mtd);

	return ret;
}

/**
 * brcmnand_sync - [MTD Interface] sync
 * @param mtd		MTD device structure
 *
 * Sync is actually a wait for chip ready function
 */
static void brcmnand_sync(struct mtd_info *mtd)
{
	DEBUG(MTD_DEBUG_LEVEL3, "brcmnand_sync: called\n");

	/* Grab the lock and see if the device is available */
	brcmnand_get_device(mtd, FL_SYNCING);

	/* Release it and go back */
	brcmnand_release_device(mtd);
}


/**
 * brcmnand_block_isbad - [MTD Interface] Check whether the block at the given offset is bad
 * @param mtd		MTD device structure
 * @param ofs		offset relative to mtd start
 *
 * Check whether the block is bad
 */
static int brcmnand_block_isbad(struct mtd_info *mtd, loff_t ofs)
{
	/* Check for invalid offset */
	if (ofs > mtd->size)
		return -EINVAL;

	return brcmnand_block_checkbad(mtd, ofs, 1, 0);
}

/**
 * brcmnand_default_block_markbad - [DEFAULT] mark a block bad
 * @param mtd		MTD device structure
 * @param ofs		offset from device start
 *
 * This is the default implementation, which can be overridden by
 * a hardware specific driver.
 */
static int brcmnand_default_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
	struct brcmnand_chip *this = mtd->priv;
	struct bbm_info *bbm = this->bbm;
	u_char buf[2] = {0, 0};
	size_t retlen;
	int block;

	/* Get block number */
	block = ((int) ofs) >> bbm->bbt_erase_shift;
        if (bbm->bbt)
                bbm->bbt[block >> 2] |= 0x01 << ((block & 0x03) << 1);

        /* We write two bytes, so we dont have to mess with 16 bit access */
        ofs += mtd->oobsize + (bbm->badblockpos & ~0x01);
        return mtd->write_oob(mtd, ofs , 2, &retlen, buf);
}

/**
 * brcmnand_block_markbad - [MTD Interface] Mark the block at the given offset as bad
 * @param mtd		MTD device structure
 * @param ofs		offset relative to mtd start
 *
 * Mark the block as bad
 */
static int brcmnand_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
	struct brcmnand_chip *this = mtd->priv;
	int ret;

	ret = brcmnand_block_isbad(mtd, ofs);
	if (ret) {
		/* If it was bad already, return success and do nothing */
		if (ret > 0)
			return 0;
		return ret;
	}

	return this->block_markbad(mtd, ofs);
}

/**
 * brcmnand_unlock - [MTD Interface] Unlock block(s)
 * @param mtd		MTD device structure
 * @param ofs		offset relative to mtd start
 * @param len		number of bytes to unlock
 *
 * Unlock one or more blocks
 */
static int brcmnand_unlock(struct mtd_info *mtd, loff_t ofs, size_t len)
{
	struct brcmnand_chip *this = mtd->priv;
	int start, end, block, value, status;

	start = ofs >> this->erase_shift;
	end = len >> this->erase_shift;

	/* Continuous lock scheme */
	if (this->options & ONENAND_CONT_LOCK) {
		/* Set start block address */
		this->write_word(start, this->base + ONENAND_REG_START_BLOCK_ADDRESS);
		/* Set end block address */
		this->write_word(end - 1, this->base + ONENAND_REG_END_BLOCK_ADDRESS);
		/* Write unlock command */
		this->command(mtd, ONENAND_CMD_UNLOCK, 0, 0);

		/* There's no return value */
		this->wait(mtd, FL_UNLOCKING);

		/* Sanity check */
		while (this->read_word(this->base + ONENAND_REG_CTRL_STATUS)
		    & ONENAND_CTRL_ONGO)
			continue;

		/* Check lock status */
		status = this->read_word(this->base + ONENAND_REG_WP_STATUS);
		if (!(status & ONENAND_WP_US))
			printk(KERN_ERR "wp status = 0x%x\n", status);

		return 0;
	}

	/* Block lock scheme */
	for (block = start; block < end; block++) {
		/* Set block address */
		value = brcmnand_block_address(this, block);
		this->write_word(value, this->base + ONENAND_REG_START_ADDRESS1);
		/* Select DataRAM for DDP */
		value = brcmnand_bufferram_address(this, block);
		this->write_word(value, this->base + ONENAND_REG_START_ADDRESS2);
		/* Set start block address */
		this->write_word(block, this->base + ONENAND_REG_START_BLOCK_ADDRESS);
		/* Write unlock command */
		this->command(mtd, ONENAND_CMD_UNLOCK, 0, 0);

		/* There's no return value */
		this->wait(mtd, FL_UNLOCKING);

		/* Sanity check */
		while (this->read_word(this->base + ONENAND_REG_CTRL_STATUS)
		    & ONENAND_CTRL_ONGO)
			continue;

		/* Check lock status */
		status = this->read_word(this->base + ONENAND_REG_WP_STATUS);
		if (!(status & ONENAND_WP_US))
			printk(KERN_ERR "block = %d, wp status = 0x%x\n", block, status);
	}

	return 0;
}


/**
 * brcmnand_print_device_info - Print device ID
 * @param device        device ID
 *
 * Print device ID
 */
static void brcmnand_print_device_info(brcmnand_chip* chip)
{
	printk(KERN_INFO "%sBrcmNAND%s %dMB\n",
                chip.chipIdStr, chip.flashSize);

}

#if 0

static const struct brcmnand_manufacturers brcmnand_manuf_ids[] = {
        {ONENAND_MFR_SAMSUNG, "Samsung"},
};

/**
 * brcmnand_check_maf - Check manufacturer ID
 * @param manuf         manufacturer ID
 *
 * Check manufacturer ID
 */
static int brcmnand_check_maf(int manuf)
{
	int size = ARRAY_SIZE(brcmnand_manuf_ids);
	char *name;
        int i;

        for (i = 0; i < size; i++)
                if (manuf == brcmnand_manuf_ids[i].id)
                        break;

	if (i < size)
		name = brcmnand_manuf_ids[i].name;
	else
		name = "Unknown";

        printk(KERN_DEBUG "BrcmNAND Manufacturer: %s (0x%0x)\n", name, manuf);

        return (i == size);
}

#endif

/*
 * bit 31: 	1 = OTP read-only
 * 30: 		Page Size: 0 = PG_SIZE_512, 1 = PG_SIZE_2KB
 * 28-29: 	Block size: 3=512K, 1 = 128K, 0 = 16K, 2 = 8K
 * 27:		Reserved
 * 24-26:	Device_Size
 *			0:	4MB
 *			1:	8MB
 *			2: 	16MB
 *			3:	32MB
 *			4:	64MB
 *			5:	128MB
 *			6: 	256MB
 * 23:		Dev_Width 0 = Byte8, 1 = Word16
 * 22-19: 	Reserved
 * 18:16:	Full Address Bytes
 * 15		Reserved
 * 14:12	Col_Adr_Bytes
 * 11:		Reserved
 * 10-08	Blk_Adr_Bytes
 * 07-00	Reserved
 */
 
void
brcmnand_decode_config(struct brcmnand_chip* chip, uint32_t nand_config)
{
	unsigned int chipSizeShift;
	
	chip->chipSize = (nand_config & 0x07000000) >> (24 - 20);

	switch ((nand_config & 0x30000000) >> 28) {
		case 3:
			chip->blockSize = 512 >> 10;
			break;
		case 1:
			chip->blockSize = 128 >> 10;
			break;
		case 1:	
			chip->blockSize = 128 >> 10;
			break;
		case 0:
			chip->blockSize = 8 >> 10;
			break;
	}
	chip->erase_shift = ffs(chip->blockSize);

	switch((nand_config & 0x40000000) >> 29) {
		case 0:
			chip->pageSize = 512;
			break;
		case 1:
			chip->pageSize = 2048;
			break;
	}
	chip->page_shift = ffs(chip->pageSize);
	chip->page_mask = (1 << chip->page_shift) - 1;

	chipSizeShift = (nand_config & 0x07000000) >> 24;
	chip->chipSize = (4 << 20) << chipSizeShift;

	printk("NAND Config: Reg=%08x, chipSize=%dMB, blockSize=%dK, pageSize=%dB\n",
		nand_config, chip->chipSize >> 20, chip->blockSize >> 10, chip->pageSize);

}


/**
 * brcmnand_probe - [BrcmNAND Interface] Probe the BrcmNAND device
 * @param mtd		MTD device structure
 *
 * BrcmNAND detection method:
 *   Compare the the values from command with ones from register
 */
static int brcmnand_probe(struct mtd_info *mtd)
{
	struct brcmnand_chip *this = mtd->priv;
	unsigned char brcmnand_maf_id, brcmnand_dev_id;
	uint32 dev_id;
	uint32_t nand_config;
	int version_id;
	//int density;

	/* Send the command for reading device ID from controller */
	dev_id = brcmnand_ctrl_read(BCHP_NAND_FLASH_DEVICE_ID);
	
#if 1
	/* Read manufacturer and device IDs from Controller */
	brcmnand_maf_id = (dev_id >> 24) & 0xff;
	brcmnand_dev_id = (dev_id >> 16) & 0xff;

	/* Check manufacturer ID */
	//if (brcmnand_check_maf(brcmnand_maf_id))
	//	return -ENXIO;
#endif

	/* Look up in our table for infos on device */
	for (i=0; i < BRCMNAND_MAX_CHIPS; i++) {
		if (brcmnand_dev_id == brcmnand_chips[i].chipId 
			&& brcmnand_maf_id == brcmnand_chips[i].mafId)
			break;
	}

	if (i >= BRCMNAND_MAX_CHIPS) {
		printk("DevId %08x not supported\n", dev_id);
		return (-EINVAL);
	}

	nand_config = brcmnand_ctrl_read(BCHP_NAND_CONFIG);

	brcmnand_decode_config(this, nand_config);


	/* Flash device information */
	brcmnand_print_device_info(&brcmnand_chips[i], flashSize);
	this->device_id = dev_id;

#if 0
	density = this->chipSize >> 20;
	this->chipsize = (16 << density) << 20;
	/* Set density mask. it is used for DDP */
	this->density_mask = (1 << (density + 6));
#endif

	/* BrcmNAND page size & block size */
	/* The data buffer size is equal to page size */
	mtd->oobblock = this->pageSize;
	mtd->oobsize = mtd->oobblock >> 5;
	mtd->erasesize = this->blockSize;

	/* Fix me: When we have both a NOR and NAND flash on board */

	mtd->size = this->chipSize;

	/* Set up base, based on flash size */
	if (this->chipSize == (256 << 20) {
		this->vbase = KSEG1ADDR(0x12000000);
	} else {
		/* We know that flash endAddr is 0x2000_0000 */
		this->vbase = KSEG1ADDR(0x20000000 - this->chipSize);
	}

	/* Version ID */
	version_id = brcmnand_ctrl_read(BCHP_NAND_REVISION);
	printk(KERN_DEBUG "BrcmNAND version = 0x%04x\n", version_id);

	return 0;
}

/**
 * brcmnand_suspend - [MTD Interface] Suspend the BrcmNAND flash
 * @param mtd		MTD device structure
 */
static int brcmnand_suspend(struct mtd_info *mtd)
{
	return brcmnand_get_device(mtd, FL_PM_SUSPENDED);
}

/**
 * brcmnand_resume - [MTD Interface] Resume the BrcmNAND flash
 * @param mtd		MTD device structure
 */
static void brcmnand_resume(struct mtd_info *mtd)
{
	struct brcmnand_chip *this = mtd->priv;

	if (this->state == FL_PM_SUSPENDED)
		brcmnand_release_device(mtd);
	else
		printk(KERN_ERR "resume() called for the chip which is not"
				"in suspended state\n");
}

/**
 * brcmnand_scan - [BrcmNAND Interface] Scan for the BrcmNAND device
 * @param mtd		MTD device structure
 * @param maxchips	Number of chips to scan for
 *
 * This fills out all the not initialized function pointers
 * with the defaults.
 * The flash ID is read and the mtd/chip structures are
 * filled with the appropriate values.
 *
 * THT: For now, machips should always be 1.
 */
int brcmnand_scan(struct mtd_info *mtd , int maxchips )
{
	struct nand_chip *this = mtd->priv;

	if (!this->ctrl_read)
		this->ctrl_read = brcmnand_ctrl_read;
	if (!this->ctrl_write)
		this->ctrl_write = brcmnand_ctrl_write;

	if (!this->command)
		this->command = brcmnand_command;
	if (!this->wait)
		this->wait = brcmnand_wait;

#if 0
	if (!this->read_bufferram)
		this->read_bufferram = brcmnand_read_bufferram;
	if (!this->write_bufferram)
		this->write_bufferram = brcmnand_write_bufferram;
#endif

	if (!this->block_markbad)
		this->block_markbad = nand_default_block_markbad;
	if (!this->scan_bbt)
		this->scan_bbt = brcmnand_default_bbt;

	if (brcmnand_probe(mtd))
		return -ENXIO;

#if 0
//THT: We don't have this feature
	/* Set Sync. Burst Read after probing */
	if (this->mmcontrol) {
		printk(KERN_INFO "BrcmNAND Sync. Burst Read support\n");
		this->read_bufferram = brcmnand_sync_read_bufferram;
	}
#endif

	/* Allocate buffers, if necessary */
	if (!this->page_buf) {
		size_t len;
		len = mtd->oobblock + mtd->oobsize;
		this->page_buf = kmalloc(len, GFP_KERNEL);
		if (!this->page_buf) {
			printk(KERN_ERR "brcmnand_scan(): Can't allocate page_buf\n");
			return -ENOMEM;
		}
		this->options |= ONENAND_PAGEBUF_ALLOC;
	}

	this->state = FL_READY;
	init_waitqueue_head(&this->wq);
	spin_lock_init(&this->chip_lock);

	switch (mtd->oobsize) {
	case 64:
		this->autooob = &brcmnand_oob_64;
		break;

	case 16:
		this->autooob = &brcmnand_oob_16;
		break;

	default:
		printk(KERN_WARNING "No OOB scheme defined for oobsize %d\n",
			mtd->oobsize);
		/* To prevent kernel oops */
		this->autooob = &brcmnand_oob_16;
		break;
	}

	memcpy(&mtd->oobinfo, this->autooob, sizeof(mtd->oobinfo));

	/* Fill in remaining MTD driver data */
	mtd->type = MTD_NANDFLASH;
	mtd->flags = MTD_CAP_NANDFLASH | MTD_ECC;
	mtd->ecctype = MTD_ECC_SW;
	mtd->erase = brcmnand_erase;
	mtd->point = NULL;
	mtd->unpoint = NULL;
	mtd->read = brcmnand_read;
	mtd->write = brcmnand_write;
	mtd->read_ecc = brcmnand_read_ecc;
	mtd->write_ecc = brcmnand_write_ecc;
	mtd->read_oob = brcmnand_read_oob;
	mtd->write_oob = brcmnand_write_oob;

	mtd->readv = NULL;
	mtd->readv_ecc = NULL;
	mtd->writev = brcmnand_writev;
	mtd->writev_ecc = brcmnand_writev_ecc;
	mtd->sync = brcmnand_sync;
	mtd->lock = NULL;
	mtd->unlock = brcmnand_unlock;
	mtd->suspend = brcmnand_suspend;
	mtd->resume = brcmnand_resume;
	mtd->block_isbad = brcmnand_block_isbad;
	mtd->block_markbad = brcmnand_block_markbad;
	mtd->owner = THIS_MODULE;

	/* Unlock whole block */
	mtd->unlock(mtd, 0x0, this->chipsize);

	return this->scan_bbt(mtd);
}

/**
 * brcmnand_release - [BrcmNAND Interface] Free resources held by the BrcmNAND device
 * @param mtd		MTD device structure
 */
void brcmnand_release(struct mtd_info *mtd)
{
	struct brcmnand_chip *this = mtd->priv;

#ifdef CONFIG_MTD_PARTITIONS
	/* Deregister partitions */
	del_mtd_partitions (mtd);
#endif
	/* Deregister the device */
	del_mtd_device (mtd);

	/* Free bad block table memory, if allocated */
	if (this->bbm)
		kfree(this->bbm);

#if 0
	/* Buffer allocated by brcmnand_scan */
	if (this->options & ONENAND_PAGEBUF_ALLOC)
		kfree(this->page_buf);
#endif
}



