/*
 *  linux/drivers/mtd/onenand/generic.c
 *
 *  Copyright (c) 2005 Samsung Electronics
 *  Kyungmin Park <kyungmin.park@samsung.com>
 *
 *  $Id: generic.c,v 1.4 2006/01/18 15:07:51 lavinen Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  Overview:
 *   This is a device driver for the OneNAND flash for generic boards.
 */


#ifndef _BRCM_NAND_H_
#define _BRCM_NAND_H_

//ST NAND flashes
#ifndef FLASHTYPE_ST
    #define FLASHTYPE_ST            0x20
#endif
#define ST_NAND128W3A           0x73
#define ST_NAND256R3A           0x35
#define ST_NAND256W3A           0x75
#define ST_NAND256R4A           0x45
#define ST_NAND256W4A           0x55
#define ST_NAND512R3A           0x36    //Used on Bcm97400
#define ST_NAND512W3A           0x76
#define ST_NAND512R4A           0x46
#define ST_NAND512W4A           0x56
#define ST_NAND01GR3A           0x39
#define ST_NAND01GW3A           0x79
#define ST_NAND01GR4A           0x49
#define ST_NAND01GW4A           0x59
#define ST_NAND01GR3B           0xA1
#define ST_NAND01GW3B           0xF1
#define ST_NAND01GR4B           0xB1
#define ST_NAND01GW4B           0xC1
#define ST_NAND02GR3B           0xAA
#define ST_NAND02GW3B           0xDA
#define ST_NAND02GR4B           0xBA
#define ST_NAND02GW4B           0xCA
#define ST_NAND04GR3B           0xAC
#define ST_NAND04GW3B           0xDC
#define ST_NAND04GR4B           0xBC
#define ST_NAND04GW4B           0xCC
#define ST_NAND08GR3B           0xA3
#define ST_NAND08GW3B           0xD3
#define ST_NAND08GR4B           0xB3
#define ST_NAND08GW4B           0xC3

//Samsung NAND flash
#define FLASHTYPE_SAMSUNG       0xEC
#define SAMSUNG_K9F1G08R0A      0xA1
#define SAMSUNG_K9F1G08U0A      0xF1
#define SAMSUNG_K9F2G08U1A      0xF1

//Command Opcode
#define OP_PAGE_READ                0x01000000
#define OP_SPARE_AREA_READ          0x02000000
#define OP_STATUS_READ              0x03000000
#define OP_PROGRAM_PAGE             0x04000000
#define OP_PROGRAM_SPARE_AREA       0x05000000
#define OP_COPY_BACK                0x06000000
#define OP_DEVICE_ID_READ           0x07000000
#define OP_BLOCK_ERASE              0x08000000
#define OP_FLASH_RESET              0x09000000
#define OP_BLOCKS_LOCK              0x0A000000
#define OP_BLOCKS_LOCK_DOWN         0x0B000000
#define OP_BLOCKS_UNLOCK            0x0C000000
#define OP_READ_BLOCKS_LOCK_STATUS  0x0D000000

//NAND flash controller 
#define NFC_FLASHCACHE_SIZE     512

#define BRCMNAND_CTRL_VBASE	(KSEG1ADDR(0x10000000 + BCHP_NAND_REVISION))


/**
 * brcmnand_state_t - chip states
 * Enumeration for OneNAND flash chip state
 */
typedef enum {
	FL_READY,
	FL_READING,
	FL_WRITING,
	FL_ERASING,
	FL_SYNCING,
	FL_UNLOCKING,
	FL_LOCKING,
	FL_RESETING,
	FL_OTPING,
	FL_PM_SUSPENDED,
} brcmnand_state_t;

/**
 * struct brcmnand_cache - Brcm NAND BufferRAM Data
 * @param block		block # in BufferRAM
 * @param page		page # in BufferRAM
 * @param slide		slide # within page, if page size != NFC_FLASH_CACHE_SIZE
 * @param valid		valid flag
 */
struct brcmnand_bufferram {
	int block;	// Block #
	int page;	// Page # in block
	int slide;	// Slide # (of 512B cache) in page
	int valid;
};


/**
 * struct brcmnand_chip - OneNAND Private Flash Chip Data
 * @param base		[BOARDSPECIFIC] address to access Broadcom NAND controller
 * @param chipsize	[INTERN] the size of one chip for multichip arrays
 * @param device_id	[INTERN] device ID
 * @param verstion_id	[INTERN] version ID
 * @param options	[BOARDSPECIFIC] various chip options. They can partly be set to inform brcmnand_scan about
 * @param erase_shift	[INTERN] number of address bits in a block
 * @param page_shift	[INTERN] number of address bits in a page
 * @param ppb_shift	[INTERN] number of address bits in a pages per block
 * @param page_mask	[INTERN] a page per block mask
 * @param bufferam_index	[INTERN] BufferRAM index
 * @param bufferam	[INTERN] BufferRAM info
 * @param readw		[REPLACEABLE] hardware specific function for read short
 * @param writew	[REPLACEABLE] hardware specific function for write short
 * @param command	[REPLACEABLE] hardware specific function for writing commands to the chip
 * @param wait		[REPLACEABLE] hardware specific function for wait on ready
 * @param read_bufferram	[REPLACEABLE] hardware specific function for BufferRAM Area
 * @param write_bufferram	[REPLACEABLE] hardware specific function for BufferRAM Area
 * @param read_word	[REPLACEABLE] hardware specific function for read register of OneNAND
 * @param write_word	[REPLACEABLE] hardware specific function for write register of OneNAND
 * @param scan_bbt	[REPLACEALBE] hardware specific function for scaning Bad block Table
 * @param chip_lock	[INTERN] spinlock used to protect access to this structure and the chip
 * @param wq		[INTERN] wait queue to sleep on if a OneNAND operation is in progress
 * @param state		[INTERN] the current state of the OneNAND device
 * @param autooob	[REPLACEABLE] the default (auto)placement scheme
 * @param bbm		[REPLACEABLE] pointer to Bad Block Management
 * @param priv		[OPTIONAL] pointer to private chip date
 */
struct brcmnand_chip {
	void __iomem		*vbase; /* Virtual address of start of flash */
	unsigned int		chipSize;

	/* THT Added */
	unsigned int 		busWidth, pageSize, blockSize;
	
	unsigned int		device_id;
	//unsigned int		density_mask;
	unsigned int		options;

	unsigned int		erase_shift;
	unsigned int		page_shift;
	//unsigned int		ppb_shift;	/* Pages per block shift */
	unsigned int		page_mask;

	//THT: In BrcmNAND, the NAND controller  keeps track of the 512B Cache
	// so there is no need to manage the buffer ram.
	//unsigned int		bufferram_index;
	struct brcmnand_bufferram	bufferram;

	int (*command)(struct mtd_info *mtd, int cmd, loff_t address, size_t len);
	int (*wait)(struct mtd_info *mtd, int state, uint32_t* pStatus);
	int (*read_bufferram)(struct mtd_info *mtd,
			unsigned char *buffer, int offset, size_t count);
	int (*write_bufferram)(struct mtd_info *mtd,
			const unsigned char *buffer, int offset, size_t count);
	
	unsigned short (*read_word)(void __iomem *addr);
	void (*write_word)(unsigned short value, void __iomem *addr);

	// THT: Sync Burst Read, not supported yet.
	//void (*mmcontrol)(struct mtd_info *mtd, int sync_read);
	
	int (*block_markbad)(struct mtd_info *mtd, loff_t ofs);
	int (*scan_bbt)(struct mtd_info *mtd);

	uint32_t ctrl_read(uint32_t command);
	void ctrl_write(command, uint32_t val);

	spinlock_t			chip_lock;
	//atomic_t			semCount;
	wait_queue_head_t	wq;
	brcmnand_state_t	state;
	unsigned char		*page_buf;

	struct nand_oobinfo	*autooob;

	void 				*bbm;

	void				*priv;
};


#endif
