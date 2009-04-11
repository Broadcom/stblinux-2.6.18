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

struct erase_info_user64 {
	uint64_t start;
	uint32_t length;
};

struct mtd_oob_buf {
	uint32_t start;
	uint32_t length;
	unsigned char __user *ptr;
};

struct mtd_oob_buf64 {
	uint64_t start;
	uint32_t length;
	unsigned char __user *ptr;
};



#define MTD_ABSENT		0
#define MTD_RAM			1
#define MTD_ROM			2
#define MTD_NORFLASH		3
#define MTD_NANDFLASH		4
#define MTD_DATAFLASH		6

#define MTD_WRITEABLE		0x400	/* Device is writeable */
#define MTD_BIT_WRITEABLE	0x800	/* Single bits can be flipped */
#define MTD_NO_ERASE		0x1000	/* No erase necessary */
#define MTD_STUPID_LOCK	0x2000	/* Always locked after reset */
#define MTD_OOB_WRITEABLE	0x4000	/* Use Out-Of-Band area */


// Some common devices / combinations of capabilities
#define MTD_CAP_ROM		0
#define MTD_CAP_RAM		(MTD_WRITEABLE | MTD_BIT_WRITEABLE | MTD_NO_ERASE)
#define MTD_CAP_NORFLASH	(MTD_WRITEABLE | MTD_BIT_WRITEABLE)
#define MTD_CAP_NANDFLASH	(MTD_WRITEABLE | MTD_OOB_WRITEABLE)
#define MTD_CAP_MLC_NANDFLASH (MTD_WRITEABLE)

// High level MLC test, compared to low level defined in brcmnand.h
#define MTD_IS_MLC(mtd) ((((mtd)->flags & MTD_CAP_MLC_NANDFLASH) == MTD_CAP_MLC_NANDFLASH) &&\
			(((mtd)->flags & MTD_OOB_WRITEABLE) != MTD_OOB_WRITEABLE))


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
	uint32_t writesize;
	uint32_t oobsize;   // Amount of OOB data per block (e.g. 16)
	uint32_t ecctype;
	uint32_t eccsize;
};

struct mtd_info_user64 {
	uint8_t type;
	uint32_t flags;
	uint64_t size;  // Total blocks in MTD
	uint32_t erasesize;
	uint32_t writesize;
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

struct region_info_user64 {
	uint64_t offset;		/* At which this region starts,
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

#define MEMGETINFO		_IOR('M', 1, struct mtd_info_user)
#define MEMERASE		_IOW('M', 2, struct erase_info_user)
#define MEMWRITEOOB		_IOWR('M', 3, struct mtd_oob_buf)
#define MEMREADOOB		_IOWR('M', 4, struct mtd_oob_buf)
#define MEMLOCK			_IOW('M', 5, struct erase_info_user)
#define MEMUNLOCK		_IOW('M', 6, struct erase_info_user)
#define MEMGETREGIONCOUNT	_IOR('M', 7, int)
#define MEMGETREGIONINFO	_IOWR('M', 8, struct region_info_user)
#define MEMSETOOBSEL		_IOW('M', 9, struct nand_oobinfo32)
#define MEMGETOOBSEL		_IOR('M', 10, struct nand_oobinfo32)
#define MEMGETBADBLOCK		_IOW('M', 11, loff_t)
#define MEMSETBADBLOCK		_IOW('M', 12, loff_t)
#define OTPSELECT		_IOR('M', 13, int)
#define OTPGETREGIONCOUNT	_IOW('M', 14, int)
#define OTPGETREGIONINFO	_IOW('M', 15, struct otp_info)
#define OTPLOCK			_IOR('M', 16, struct otp_info)
#define ECCGETLAYOUT		_IOR('M', 17, struct nand_ecclayout)
#define ECCGETSTATS		_IOR('M', 18, struct mtd_ecc_stats)
#define MTDFILEMODE		_IO('M', 19)

#define MEMGETINFO64		_IOR('M', 20, struct mtd_info_user64)
#define MEMGETREGIONINFO64	_IOWR('M', 21, struct region_info_user64)
#define MEMUNLOCK64		_IOW('M', 22, struct erase_info_user64)
#define MEMERASE64		_IOW('M', 23, struct erase_info_user64)
#define MEMWRITEOOB64		_IOWR('M', 24, struct mtd_oob_buf64)
#define MEMREADOOB64		_IOWR('M', 25, struct mtd_oob_buf64)
#define MEMSETOOBSEL64		_IOW('M', 26, struct nand_oobinfo)
#define MEMGETOOBSEL64		_IOR('M', 27, struct nand_oobinfo)

/*
 * Obsolete legacy interface. Keep it in order not to break userspace
 * interfaces
 */
struct nand_oobinfo32 {
	uint32_t useecc;
	uint32_t eccbytes;
	uint32_t oobfree[8][2];
	uint32_t eccpos[32];
};

/*
 * The nand_oobinfo32 struct (above) was originally called nand_oobinfo
 * It exists only for backward compatibility with ioctl() calls. The new
 * struct was not named nand_oobinfo64 because the struct name 'nand_oobinfo'
 * is used in the kernel (yaffs2/brcmnand) 
 * Also, note that the old ioctl still works with a changed struct name because
 * the ioctl numbers are generated based on sizeof(struct ...) and not name
 * of the struct. 
 */
struct nand_oobinfo {
	uint32_t useecc;
	uint32_t eccbytes;
	uint32_t oobfree[8][2];
	uint32_t eccpos[64];
};
/* For consistency with other structs used in ioctl 64-bit versions */
typedef struct nand_oobinfo64 nand_oobinfo;

struct nand_oobfree {
	uint32_t offset;
	uint32_t length;
};

#define MTD_MAX_OOBFREE_ENTRIES	9
#define MTD_MAX_ECCPOS_ENTRIES	218	// Was 64 for SLC
/*
 * ECC layout control structure. Exported to userspace for
 * diagnosis and to allow creation of raw images
 */
struct nand_ecclayout {
	uint32_t eccbytes;
	uint32_t eccpos[MTD_MAX_ECCPOS_ENTRIES];
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
