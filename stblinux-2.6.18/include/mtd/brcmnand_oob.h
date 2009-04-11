/*
 *  include/mtd/brcmnand_oob.h
 *
    Copyright (c) 2005-2007 Broadcom Corporation                 
    
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

    File: brcmnand_oob.c

    Description: 
    Export OOB structure to user space.

when	who what
-----	---	----
051011	tht	Moved OOB format here from brcmnand_base.c in order to share it with User space
 */

#ifndef __BRCMNAND_OOB_H
#define __BRCMNAND_OOB_H

#include <linux/version.h>

/*
 * Assuming proper include that precede this has the typedefs for struct nand_oobinfo
 */

/**
 * brcmnand_oob oob info for 2K page
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18))

static struct nand_ecclayout brcmnand_oob_64 = {
	.eccbytes	= 12,
	.eccpos		= {
		6,7,8,
		22,23,24,
		38,39,40,
		54,55,56
		},
	.oobfree	= { /* 0-1 used for BBT and/or manufacturer bad block marker, 
	                    * first slice loses 2 bytes for BBT */
				{.offset=2, .length=4}, 
				{.offset=9,.length=13}, 		/* First slice {9,7} 2nd slice {16,6}are combined */ 
									/* ST uses 6th byte (offset=5) as Bad Block Indicator, 
									  * in addition to the 1st byte, and will be adjusted at run time */
				{.offset=25, .length=13},				/* 2nd slice  */
				{.offset=41, .length=13},				/* 3rd slice */
				{.offset=57, .length=7},				/* 4th slice */
	            {.offset=0, .length=0}				/* End marker */
			}
};


/**
 * brcmnand_oob oob info for 512 page
 */
static struct nand_ecclayout brcmnand_oob_16 = {
	.eccbytes	= 3,
	.eccpos		= {
		6,7,8
		},
	.oobfree	= { {.offset=0, .length=5}, 
				{.offset=9,.length=7}, /* Byte 5 (6th byte) used for BI */
				{.offset=0, .length=0}		/* End marker */
			   }
			/* THT Bytes offset 4&5 are used by BBT.  Actually only byte 5 is used, but in order to accomodate
			 * for 16 bit bus width, byte 4 is also not used.  If we only use byte-width chip, (We did)
			 * then we can also use byte 4 as free bytes.
			 */
};

/* Small page with BCH-4 */
static struct nand_ecclayout brcmnand_oob_bch4_512 = {
	.eccbytes	= 7,
	.eccpos		= {
		9,10,11,12,13,14,15
		},
	.oobfree	= { 	{.offset=0, .length=5}, 
				{.offset=7,.length=2}, /* Byte 5 (6th byte) used for BI */
				{.offset=0, .length=0}		/* End marker */
			   }
};

/*
 * 4K page MLC with BCH-4 ECC, uses 7 ECC bytes per 512B ECC step
 */
static struct nand_ecclayout brcmnand_oob_bch4_4k = {
	.eccbytes	= 7*8,  /* 7*8 = 56 bytes */
	.eccpos		= { 
		9,10,11,12,13,14,15,
		25,26,27,28,29,30,31,
		41,42,43,44,45,46,47,
		57,58,59,60,61,62,63,
		73,74,75,76,77,78,79,
		89,90,91,92,93,94,95,
		105,106,107,108,109,110,111,
		121,122,123,124,125,126,127
		},
	.oobfree	= { /* 0  used for BBT and/or manufacturer bad block marker, 
	                    * first slice loses 1 byte for BBT */
				{.offset=1, .length=8}, 		/* 1st slice loses byte 0 */
				{.offset=16,.length=9}, 		/* 2nd slice  */
				{.offset=32, .length=9},		/* 3rd slice  */
				{.offset=48, .length=9},		/* 4th slice */
				{.offset=64, .length=9},		/* 5th slice */
				{.offset=80, .length=9},		/* 6th slice */
				{.offset=96, .length=9},		/* 7th slice */
				{.offset=112, .length=9},		/* 8th slice */
	            		{.offset=0, .length=0}			/* End marker */
			}
};

/*
 * 2K page SLC/MLC with BCH-4 ECC, uses 7 ECC bytes per 512B ECC step
 */
static struct nand_ecclayout brcmnand_oob_bch4_2k = {
	.eccbytes	= 7*8,  /* 7*8 = 56 bytes */
	.eccpos		= { 
		9,10,11,12,13,14,15,
		25,26,27,28,29,30,31,
		41,42,43,44,45,46,47,
		57,58,59,60,61,62,63
		},
	.oobfree	= { /* 0  used for BBT and/or manufacturer bad block marker, 
	                    * first slice loses 1 byte for BBT */
				{.offset=1, .length=8}, 		/* 1st slice loses byte 0 */
				{.offset=16,.length=9}, 		/* 2nd slice  */
				{.offset=32, .length=9},		/* 3rd slice  */
				{.offset=48, .length=9},		/* 4th slice */
	            		{.offset=0, .length=0}			/* End marker */
			}
};


/*
 * 4K page MLC with BCH-8 ECC, uses 13 ECC bytes per 512B ECC step, and requires OOB size of 27B+
 */
static struct nand_ecclayout brcmnand_oob_bch8_4k = {
	.eccbytes	= 13*8,  /* 13*8 = 56 bytes */
	.eccpos		= { 
		14,15,16,17,18,19,20,21,22,23,24,25,26,
		41,42,43,44,45,46,47,48,49,50,51,52,53,
		68,69,70,71,72,73,74,75,76,77,78,79,80,
		95,96,97,98,99,100,101,102,103,104,105,106,107,
		122,123,124,125,126,127,128,129,130,131,132,133,134,
		149,150,151,152,153,154,155,156,157,158,159,160,161,
		176,177,178,179,180,181,182,183,184,185,186,187,188,
		203,204,205,206,207,208,209,210,211,212,213,214,215
		},
	.oobfree	= { /* 0  used for BBT and/or manufacturer bad block marker, 
	                    * first slice loses 1 byte for BBT */
				{.offset=1, .length=13}, 		/* 1st slice loses byte 0 */
				{.offset=27,.length=14}, 		/* 2nd slice  */
				{.offset=54, .length=14},		/* 3rd slice  */
				{.offset=81, .length=14},		/* 4th slice */
				{.offset=108, .length=14},		/* 5th slice */
				{.offset=135, .length=14},		/* 6th slice */
				{.offset=162, .length=14},		/* 7th slice */
				{.offset=189, .length=14},		/* 8th slice */
	            		{.offset=0, .length=0}			/* End marker */
			}
};

#else
/* MLC not supported in 2.6.12 */

static struct nand_oobinfo brcmnand_oob_64 = {
	.useecc		= MTD_NANDECC_AUTOPLACE,
	.eccbytes	= 12,
	.eccpos		= {
		6,7,8,
		22,23,24,
		38,39,40,
		54,55,56
		},
	.oobfree	= { /* 0-1 used for BBT and/or manufacturer bad block marker, 
	                    * first slice loses 2 bytes for BBT */
				{2, 4}, {9,13}, 		/* First slice {9,7} 2nd slice {16,6}are combined */ 
									/* ST uses 6th byte (offset=5) as Bad Block Indicator, 
									  * in addition to the 1st byte, and will be adjusted at run time */
				{25, 13},				/* 2nd slice  */
				{41, 13},				/* 3rd slice */
				{57, 7},				/* 4th slice */
	                   {0, 0}				/* End marker */
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
	.oobfree	= { {0, 5}, {9,7}, /* Byte 5 (6th byte) used for BI */
				{0, 0}		/* End marker */
			   }
			/* THT Bytes offset 4&5 are used by BBT.  Actually only byte 5 is used, but in order to accomodate
			 * for 16 bit bus width, byte 4 is also not used.  If we only use byte-width chip, (We did)
			 * then we can also use byte 4 as free bytes.
			 */
};


#endif /* 2.6.17 or earlier */
#endif

