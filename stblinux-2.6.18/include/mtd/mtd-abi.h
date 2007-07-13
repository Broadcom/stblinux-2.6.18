/*
 * $Id: mtd-abi.h,v 1.13 2005/11/07 11:14:56 gleixner Exp $
 *
 * Portions of MTD ABI definition which are shared by kernel and user space
 */

#ifndef __MTD_ABI_H__
#define __MTD_ABI_H__

#ifndef __KERNEL__ 
/* Urgh. The whole point of splitting this out into
   separate files was to avoid #ifdef __KERNEL__ */
#define __user
#endif

struct erase_info_user {
	uint32_t start;
	uint32_t length;
};

struct mtd_oob_buf {
	uint32_t start;
	uint32_t length;
	unsigned char __user *ptr;
};

#define MTD_ABSENT		0
#define MTD_RAM			1
#define MTD_ROM			2
#define MTD_NORFLASH		3
#define MTD_NANDFLASH		4
#define MTD_PEROM		5
#define MTD_DATAFLASH		6
#define MTD_BLOCK		7
#define MTD_OTHER		14
#define MTD_UNKNOWN		15

#define MTD_WRITEABLE		0x400	/* Device is writeable */
#define MTD_BIT_WRITEABLE	0x800	/* Single bits can be flipped */
#define MTD_NO_ERASE		0x1000	/* No erase necessary */
#define MTD_CLEAR_BITS		1       // Bits can be cleared (flash)
#define MTD_SET_BITS		2       // Bits can be set
#define MTD_ERASEABLE		4       // Has an erase function
#define MTD_WRITEB_WRITEABLE	8       // Direct IO is possible
#define MTD_VOLATILE		16      // Set for RAMs
#define MTD_XIP			32	// eXecute-In-Place possible
#define MTD_OOB			64	// Out-of-band data (NAND flash)
#define MTD_ECC			128	// Device capable of automatic ECC
#define MTD_NO_VIRTBLOCKS	256	// Virtual blocks not allowed
#define MTD_PROGRAM_REGIONS	512	// Configurable Programming Regions

// Some common devices / combinations of capabilities
#define MTD_CAP_ROM		0
#define MTD_CAP_RAM		(MTD_WRITEABLE | MTD_BIT_WRITEABLE | MTD_NO_ERASE)
#define MTD_CAP_NORFLASH	(MTD_WRITEABLE | MTD_BIT_WRITEABLE)
#define MTD_CAP_NANDFLASH	(MTD_WRITEABLE)


// Types of automatic ECC/Checksum available
#define MTD_ECC_NONE		0 	// No automatic ECC available
#define MTD_ECC_RS_DiskOnChip	1	// Automatic ECC on DiskOnChip
#define MTD_ECC_SW		2	// SW ECC for Toshiba & Samsung devices

/* ECC byte placement */
#define MTD_NANDECC_OFF		0	// Switch off ECC (Not recommended)
#define MTD_NANDECC_PLACE	1	// Use the given placement in the structure (YAFFS1 legacy mode)
#define MTD_NANDECC_AUTOPLACE	2	// Use the default placement scheme
#define MTD_NANDECC_PLACEONLY	3	// Use the given placement in the structure (Do not store ecc result on read)
#define MTD_NANDECC_AUTOPL_USR 	4	// Use the given autoplacement scheme rather than using the default

/* OTP mode selection */
#define MTD_OTP_OFF		0
#define MTD_OTP_FACTORY		1
#define MTD_OTP_USER		2

struct mtd_info_user {
	uint8_t type;
	uint32_t flags;
	uint32_t size;	 // Total size of the MTD
	uint32_t erasesize;
	uint32_t oobblock;  // Size of OOB blocks (e.g. 512)
	uint32_t oobsize;   // Amount of OOB data per block (e.g. 16)
	uint32_t ecctype;
	uint32_t eccsize;
};

struct region_info_user {
	uint32_t offset;		/* At which this region starts,
					 * from the beginning of the MTD */
	uint32_t erasesize;		/* For this region */
	uint32_t numblocks;		/* Number of blocks in this region */
	uint32_t regionindex;
};

struct otp_info {
	uint32_t start;
	uint32_t length;
	uint32_t locked;
};

#define MEMGETINFO              _IOR('M', 1, struct mtd_info_user)
#define MEMERASE                _IOW('M', 2, struct erase_info_user)
#define MEMWRITEOOB             _IOWR('M', 3, struct mtd_oob_buf)
#define MEMREADOOB              _IOWR('M', 4, struct mtd_oob_buf)
#define MEMLOCK                 _IOW('M', 5, struct erase_info_user)
#define MEMUNLOCK               _IOW('M', 6, struct erase_info_user)
#define MEMGETREGIONCOUNT	_IOR('M', 7, int)
#define MEMGETREGIONINFO	_IOWR('M', 8, struct region_info_user)
#define MEMSETOOBSEL		_IOW('M', 9, struct nand_oobinfo)
#define MEMGETOOBSEL		_IOR('M', 10, struct nand_oobinfo)
#define MEMGETBADBLOCK		_IOW('M', 11, loff_t)
#define MEMSETBADBLOCK		_IOW('M', 12, loff_t)
#define OTPSELECT		_IOR('M', 13, int)
#define OTPGETREGIONCOUNT	_IOW('M', 14, int)
#define OTPGETREGIONINFO	_IOW('M', 15, struct otp_info)
#define OTPLOCK		_IOR('M', 16, struct otp_info)
/* 2.6.12 ioctls 
#define MEMGETOOBAVAIL		_IOR('M', 17, uint32_t)
#define MEMWRITEOOBFREE         _IOWR('M', 18, struct mtd_oob_buf)
#define MEMREADOOBFREE          _IOWR('M', 19, struct mtd_oob_buf)
*/
/* 2.6.18.1 ioctls */
#define ECCGETLAYOUT		_IOR('M', 17, struct nand_ecclayout)
#define ECCGETSTATS		_IOR('M', 18, struct mtd_ecc_stats)
#define MTDFILEMODE		_IO('M', 19)

#define MTD_BRCMNAND_READNORFLASH _IOWR('M', 50, struct brcmnand_readNorFlash)
struct brcmnand_readNorFlash {
	void* buff;
	unsigned int offset;
	int len;
	int retVal;
};


/*
 * Obsolete legacy interface. Keep it in order not to break userspace
 * interfaces
 */
struct nand_oobinfo {
	uint32_t useecc;
	uint32_t eccbytes;
	uint32_t oobfree[8][2];
	uint32_t eccpos[32];
};

struct nand_oobfree {
	uint32_t offset;
	uint32_t length;
};

#define MTD_MAX_OOBFREE_ENTRIES	8
/*
 * ECC layout control structure. Exported to userspace for
 * diagnosis and to allow creation of raw images
 */
struct nand_ecclayout {
	uint32_t eccbytes;
	uint32_t eccpos[64];
	uint32_t oobavail;
	struct nand_oobfree oobfree[MTD_MAX_OOBFREE_ENTRIES];
};

/**
 * struct mtd_ecc_stats - error correction stats
 *
 * @corrected:	number of corrected bits
 * @failed:	number of uncorrectable errors
 * @badblocks:	number of bad blocks in this partition
 * @bbtblocks:	number of blocks reserved for bad block tables
 */
struct mtd_ecc_stats {
	uint32_t corrected;
	uint32_t failed;
	uint32_t badblocks;
	uint32_t bbtblocks;
};

/*
 * Read/write file modes for access to MTD
 */
enum mtd_file_modes {
	MTD_MODE_NORMAL = MTD_OTP_OFF,
	MTD_MODE_OTP_FACTORY = MTD_OTP_FACTORY,
	MTD_MODE_OTP_USER = MTD_OTP_USER,
	MTD_MODE_RAW,
};

#endif /* __MTD_ABI_H__ */
