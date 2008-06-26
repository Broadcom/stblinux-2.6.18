 /**************************************************************************
 *     Copyright (c) 2002-06 Broadcom Corporation
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
 * $brcm_Workfile: bcm71xx_ide.h $
 * $brcm_Revision: SanJose_Linux_Devel/2 $
 * $brcm_Date: 9/18/02 6:53p $
 *
 * Module Description:
 *    Header for Broadcom IDE Controller routines for Linux IDE driver.
 *
 * Revision History:
 *    03/20/02 jli Initial revision. Version 1.0
 *
 * $brcm_Log: /Linux/kernel/src/bcm97xxx_2418/drivers/ide/bcm71xx_ide.h $
 * 
 * SanJose_Linux_Devel/2   9/18/02 6:53p ttruong
 * Fixed based address -jli
 * 
 * SanJose_Linux_Devel\2   5/14/02 9:17a eddieshi
 * updated for 7320
 ***************************************************************************/

#ifndef BCM7110_IDE_H__
#define BCM7110_IDE_H__

#include <linux/config.h>
#include <linux/version.h>
#include <linux/ioport.h>
#include <linux/ide.h>

#ifndef ide_ioreg_t
#define ide_ioreg_t unsigned long
#endif

#if defined (CONFIG_MIPS_BCM7115)	|| defined (CONFIG_MIPS_BCM7112)						   
#define IDE_BASE_7xxx		0xFFFB0000
#elif defined (CONFIG_MIPS_BCM7110)
#define IDE_BASE_7xxx		0xFFFB0000
#elif defined (CONFIG_MIPS_BCM7314)
#define IDE_BASE_7xxx		0xFFFB0000
#elif defined (CONFIG_MIPS_BCM7315)
#define IDE_BASE_7xxx		0xFFFB0000
#elif defined (CONFIG_MIPS_BCM7317)
#define IDE_BASE_7xxx		0xFFFB0000
#elif defined (CONFIG_MIPS_BCM7318)
#define IDE_BASE_7xxx		0xFFFB0000
#elif defined (CONFIG_MIPS_BCM7320)
#define IDE_BASE_7xxx		0xBAFB0000
#elif defined(CONFIG_MIPS_BCM7440)
#define IDE_BASE_7xxx		0xB1700000
#undef GET_FIELD
#undef SET_FIELD
#else
#error "unknown BCM STB chip!!!"
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
/* 2.4.x */
#ifndef hwif_check_region
extern int CheckRegion(ide_ioreg_t from, unsigned int extent);
#define hwif_check_region(addr, num) CheckRegion(addr, num)
#endif


#ifndef hwif_request_region
extern void RequestRegion(ide_ioreg_t from, unsigned int extent, const char *name);
// Its a No-Op for BRCM IDE
#define hwif_request_region(addr, num, name) RequestRegion(addr, num, name)
#define ide_request_region(addr, num, name) RequestRegion(addr, num, name)
#endif

#else
/* 2.6.x */
#ifdef request_region
#undef request_region
#endif
extern struct resource* RequestRegion(ide_ioreg_t from, unsigned int extent, const char *name);
// Its a No-Op for BRCM IDE
//#define hwif_request_region(addr, num, name) RequestRegion(addr, num, name)
#define request_region(addr, num, name) RequestRegion(addr, num, name)


#ifdef release_region
#undef release_region
#endif

extern void ReleaseRegion(ide_ioreg_t from, unsigned int extent);
// Its a No-Op for BRCM IDE
#define release_region(addr, num) ReleaseRegion(addr, num)
#endif /* 2.6.x */
							   

#ifndef HIGH_4
#define HIGH_4(H)		((H)=(H>>4))
#endif
#ifndef LOW_4
#define LOW_4(L)		((L)=(L-((L>>4)<<4)))
#endif
#ifndef SPLIT_BYTE
#define SPLIT_BYTE(B,H,L)	((H)=(B>>4), (L)=(B-((B>>4)<<4)))
#endif
#ifndef MAKE_WORD
#define MAKE_WORD(W,HB,LB)	((W)=((HB<<8)+LB))
#endif


#define IDE_READ8(addr)			inb(addr + IDE_BASE_ADDR)   /* (*((volatile u8 *)(addr + IDE_BASE_ADDR))) */
#define IDE_READ16(addr)		inw(addr + IDE_BASE_ADDR)   /* (*((volatile u16 *)(addr + IDE_BASE_ADDR))) */
#define IDE_READ32(addr)		inl(addr + IDE_BASE_ADDR)   /* (*((volatile u32 *)(addr + IDE_BASE_ADDR))) */

#define IDE_WRITE8(addr, val)	outb(val, addr + IDE_BASE_ADDR) /* (IDE_READ8(addr) = (u8)val) */
#define IDE_WRITE16(addr, val)	outw(val, addr + IDE_BASE_ADDR) /* (IDE_READ16(addr) = (u16)val) */
#define IDE_WRITE32(addr, val)	outl(val, addr + IDE_BASE_ADDR) /* (IDE_READ32(addr) = (u32)val) */ 

/**********************************************************************
 *generic GET_FIELD & SET_FIELD
 **********************************************************************/
/*
 * m = memory, c = core, r = register, f = field, d = data.
 */
 
#if !defined(GET_FIELD) && !defined(SET_FIELD)
#define BRCM_ALIGN(c,r,f)   c##_##r##_##f##_ALIGN
#define BRCM_BITS(c,r,f)    c##_##r##_##f##_BITS
#define BRCM_MASK(c,r,f)    c##_##r##_##f##_MASK
#define BRCM_SHIFT(c,r,f)   c##_##r##_##f##_SHIFT

#define GET_FIELD(m,c,r,f) ((((m) & BRCM_MASK(c,r,f)) >> BRCM_SHIFT(c,r,f)) << BRCM_ALIGN(c,r,f))

#define SET_FIELD(m,c,r,f,d) ((m) = (((m) & ~BRCM_MASK(c,r,f)) | ((((d) >> BRCM_ALIGN(c,r,f)) << BRCM_SHIFT(c,r,f)) & BRCM_MASK(c,r,f))) )

#define SET_TYPE_FIELD(m,c,r,f,d) SET_FIELD(m,c,r,f,c##_##d)

#define SET_NAME_FIELD(m,c,r,f,d) SET_FIELD(m,c,r,f,c##_##r##_##f##_##d)

#define SET_VALUE_FIELD(m,c,r,f,d) SET_FIELD(m,c,r,f,d)

#endif /* GET & SET */


/* ...enums go here ... */


/**********************************************************************
 *BCM7110 -- IDE_Registers
 **********************************************************************/
#define IDE_CMD                          0x00000004 /* IDE Command Register */
#define IDE_PROG_IF                      0x00000008 /* IDE Program Interface Register */
#define IDE_PRI_CS0_ADD                  0x00000010 /* IDE Primary CS0 Base Address Register */
#define IDE_PRI_CS1_ADD                  0x00000014 /* IDE Primary CS1 Base Address Register */
#define IDE_BM_ADDR                      0x00000020 /* IDE BMIDE Base Address Register */
#define IDE_PIO_TIME                     0x00000040 /* IDE PIO Timing Register */
#define IDE_DMA_TIME                     0x00000044 /* IDE DMA Timing Register */
#define IDE_PIO_CNTL_MODE                0x00000048 /* IDE PIO Control/Mode Register */
#define IDE_UDMA_CNTL_STAT               0x00000054 /* IDE UDMA Control/Status Register */
#define IDE_UDMA_MODE                    0x00000058 /* IDE UDMA Mode Register */
#define IDE_HARD_RESET                   0x00000060 /* IDE Hard Reset Register */
#define IDE_IF_CNTL_STAT                 0x00000064 /* IDE Interface Control/Status Register */
#define IDE_MBIST_CNTL_STAT              0x00000068 /* IDE MBIST Control/Status Register */
#define IDE_PRI_DTE_A                    0x00000070 /* IDE Descriptor Table Entry A Register */
#define IDE_PRI_DTE_B                    0x00000074 /* IDE Descriptor Table Entry B Register */
#define IDE_PRI_INT_MSK                  0x00000080 /* IDE Interrupt Mask Register */
#define IDE_PRI_INT                      0x00000084 /* IDE Interrupt Register */

/* In Compatibility mode the address is 0x01F0.  In Native mode, address is 0x0200 */
#define IDE_PRI_DATA                     0x00000200 /* IDE ATA Data Register */
#define IDE_PRI_FEAT_ERR                 0x00000201 /* IDE ATA Features/Error Register */
#define IDE_PRI_SEC_CNT                  0x00000202 /* IDE ATA Sector Count Register */
#define IDE_PRI_SEC_NUM                  0x00000203 /* IDE ATA Sector Number Register */
#define IDE_PRI_CYL_LOW                  0x00000204 /* IDE ATA Cylinder Low Register */
#define IDE_PRI_CYL_HI                   0x00000205 /* IDE ATA Cylinder Hi Register */
#define IDE_PRI_DEV_HD                   0x00000206 /* IDE ATA Device/Head Register */
#define IDE_PRI_CMD_STAT                 0x00000207 /* IDE ATA Command/Status Register */

#define IDE_PRI_DEV_C_STAT               0x00000242 /* IDE ATA Device Control/Alternate Status Register */
#define IDE_PRI_BM_CMD_STAT              0x00000300 /* IDE Bus Master Command/Status Register */
#define IDE_PRI_DT_PT                    0x00000304 /* IDE Descriptor Table Pointer Register */



/**********************************************************************
 *IDE_CMD
 **********************************************************************/
/* IO_ACCESS [00:00] - boolean */
#define IDE_CMD_IO_ACCESS_ALIGN                         0
#define IDE_CMD_IO_ACCESS_BITS                          1
#define IDE_CMD_IO_ACCESS_MASK                          0x00000001UL
#define IDE_CMD_IO_ACCESS_SHIFT                         0
#define IDE_CMD_IO_ACCESS_ENABLE                        1
#define IDE_CMD_IO_ACCESS_DISABLE                       0

/* MASTER_ENA [02:02] - boolean */
#define IDE_CMD_MASTER_ENA_ALIGN                        0
#define IDE_CMD_MASTER_ENA_BITS                         1
#define IDE_CMD_MASTER_ENA_MASK                         0x00000004UL
#define IDE_CMD_MASTER_ENA_SHIFT                        2
#define IDE_CMD_MASTER_ENA_ENABLE                       1
#define IDE_CMD_MASTER_ENA_DISABLE                      0

/**********************************************************************
 *IDE_PROG_IF
 **********************************************************************/
/* PRI_NATIVE [08:08] - boolean */
#define IDE_PROG_IF_PRI_NATIVE_ALIGN                    0
#define IDE_PROG_IF_PRI_NATIVE_BITS                     1
#define IDE_PROG_IF_PRI_NATIVE_MASK                     0x00000100UL
#define IDE_PROG_IF_PRI_NATIVE_SHIFT                    8
#define IDE_PROG_IF_PRI_NATIVE_COMPAT_MODE              0
#define IDE_PROG_IF_PRI_NATIVE_NATIVE_MODE              1

/* PRI_DISABLE_REQ [09:09] - boolean */
#define IDE_PROG_IF_PRI_DISABLE_REQ_ALIGN               0
#define IDE_PROG_IF_PRI_DISABLE_REQ_BITS                1
#define IDE_PROG_IF_PRI_DISABLE_REQ_MASK                0x00000200UL
#define IDE_PROG_IF_PRI_DISABLE_REQ_SHIFT               9
#define IDE_PROG_IF_PRI_DISABLE_REQ_REQUEST             1

/* SEC_NATIVE [10:10] - boolean */
#define IDE_PROG_IF_SEC_NATIVE_ALIGN                    0
#define IDE_PROG_IF_SEC_NATIVE_BITS                     1
#define IDE_PROG_IF_SEC_NATIVE_MASK                     0x00000400UL
#define IDE_PROG_IF_SEC_NATIVE_SHIFT                    10
#define IDE_PROG_IF_SEC_NATIVE_COMPAT_MODE              0
#define IDE_PROG_IF_SEC_NATIVE_NATIVE_MODE              1

/* SEC_DISABLE_REQ [09:09] - boolean */
#define IDE_PROG_IF_SEC_DISABLE_REQ_ALIGN               0
#define IDE_PROG_IF_SEC_DISABLE_REQ_BITS                1
#define IDE_PROG_IF_SEC_DISABLE_REQ_MASK                0x00000800UL
#define IDE_PROG_IF_SEC_DISABLE_REQ_SHIFT               11
#define IDE_PROG_IF_SEC_DISABLE_REQ_REQUEST             1

/* MODE_SUPPORT [15:15] - boolean */
#define IDE_PROG_IF_MODE_SUPPORT_ALIGN                  0
#define IDE_PROG_IF_MODE_SUPPORT_BITS                   1
#define IDE_PROG_IF_MODE_SUPPORT_MASK                   0x00008000UL
#define IDE_PROG_IF_MODE_SUPPORT_SHIFT                  15
#define IDE_PROG_IF_MODE_SUPPORT_PIO_ONLY               0
#define IDE_PROG_IF_MODE_SUPPORT_DMA_SUPPORT            1

/**********************************************************************
 *IDE_PRI_CS0_ADD
 **********************************************************************/
/* P_CS0_BASE_ADDR [15:00] - unsigned */
#define IDE_PRI_CS0_ADD_P_CS0_BASE_ADDR_ALIGN           0
#define IDE_PRI_CS0_ADD_P_CS0_BASE_ADDR_BITS            16
#define IDE_PRI_CS0_ADD_P_CS0_BASE_ADDR_MASK            0x0000FFFFUL
#define IDE_PRI_CS0_ADD_P_CS0_BASE_ADDR_SHIFT           0

/**********************************************************************
 *IDE_PRI_CS1_ADD
 **********************************************************************/
/* P_CS1_BASE_ADDR [15:00] - unsigned */
#define IDE_PRI_CS1_ADD_P_CS1_BASE_ADDR_ALIGN           0
#define IDE_PRI_CS1_ADD_P_CS1_BASE_ADDR_BITS            16
#define IDE_PRI_CS1_ADD_P_CS1_BASE_ADDR_MASK            0x0000FFFFUL
#define IDE_PRI_CS1_ADD_P_CS1_BASE_ADDR_SHIFT           0

/**********************************************************************
 *IDE_SEC_CS0_ADD
 **********************************************************************/
/* S_CS0_BASE_ADDR [15:00] - unsigned */

/**********************************************************************
 *IDE_SEC_CS1_ADD
 **********************************************************************/
/* S_CS1_BASE_ADDR [15:00] - unsigned */

/**********************************************************************
 *IDE_BM_ADDR
 **********************************************************************/
/* IDE_BM_BASE_ADDR [15:00] - unsigned */
#define IDE_BM_ADDR_IDE_BM_BASE_ADDR_ALIGN              0
#define IDE_BM_ADDR_IDE_BM_BASE_ADDR_BITS               16
#define IDE_BM_ADDR_IDE_BM_BASE_ADDR_MASK               0x0000FFFFUL
#define IDE_BM_ADDR_IDE_BM_BASE_ADDR_SHIFT              0

/**********************************************************************
 *IDE_PIO_TIME
 **********************************************************************/
/* PS_PIO_CMD_REC [03:00] - unsigned */
#define IDE_PIO_TIME_PS_PIO_CMD_REC_ALIGN               0
#define IDE_PIO_TIME_PS_PIO_CMD_REC_BITS                4
#define IDE_PIO_TIME_PS_PIO_CMD_REC_MASK                0x0000000FUL
#define IDE_PIO_TIME_PS_PIO_CMD_REC_SHIFT               0

/* PS_PIO_CMD [07:04] - unsigned */
#define IDE_PIO_TIME_PS_PIO_CMD_ALIGN                   0
#define IDE_PIO_TIME_PS_PIO_CMD_BITS                    4
#define IDE_PIO_TIME_PS_PIO_CMD_MASK                    0x000000F0UL
#define IDE_PIO_TIME_PS_PIO_CMD_SHIFT                   4

/* PM_PIO_CMD_REC [11:08] - unsigned */
#define IDE_PIO_TIME_PM_PIO_CMD_REC_ALIGN               0
#define IDE_PIO_TIME_PM_PIO_CMD_REC_BITS                4
#define IDE_PIO_TIME_PM_PIO_CMD_REC_MASK                0x00000F00UL
#define IDE_PIO_TIME_PM_PIO_CMD_REC_SHIFT               8

/* PM_PIO_CMD [15:12] - unsigned */
#define IDE_PIO_TIME_PM_PIO_CMD_ALIGN                   0
#define IDE_PIO_TIME_PM_PIO_CMD_BITS                    4
#define IDE_PIO_TIME_PM_PIO_CMD_MASK                    0x0000F000UL
#define IDE_PIO_TIME_PM_PIO_CMD_SHIFT                   12


/* SS_PIO_CMD_REC [19:16] - unsigned */
#define IDE_PIO_TIME_SS_PIO_CMD_REC_ALIGN               0
#define IDE_PIO_TIME_SS_PIO_CMD_REC_BITS                4
#define IDE_PIO_TIME_SS_PIO_CMD_REC_MASK                0x000F0000UL
#define IDE_PIO_TIME_SS_PIO_CMD_REC_SHIFT               16

/* SS_PIO_CMD [23:20] - unsigned */
#define IDE_PIO_TIME_SS_PIO_CMD_ALIGN                   0
#define IDE_PIO_TIME_SS_PIO_CMD_BITS                    4
#define IDE_PIO_TIME_SS_PIO_CMD_MASK                    0x00F00000UL
#define IDE_PIO_TIME_SS_PIO_CMD_SHIFT                   20

/* SM_PIO_CMD_REC [27:24] - unsigned */
#define IDE_PIO_TIME_SM_PIO_CMD_REC_ALIGN               0
#define IDE_PIO_TIME_SM_PIO_CMD_REC_BITS                4
#define IDE_PIO_TIME_SM_PIO_CMD_REC_MASK                0x0F000000UL
#define IDE_PIO_TIME_SM_PIO_CMD_REC_SHIFT               24

/* SM_PIO_CMD [31:28] - unsigned */
#define IDE_PIO_TIME_SM_PIO_CMD_ALIGN                   0
#define IDE_PIO_TIME_SM_PIO_CMD_BITS                    4
#define IDE_PIO_TIME_SM_PIO_CMD_MASK                    0xF0000000UL
#define IDE_PIO_TIME_SM_PIO_CMD_SHIFT                   28

/**********************************************************************
 *IDE_DMA_TIME
 **********************************************************************/
/* PS_DMA_CMD_REC [03:00] - unsigned */
#define IDE_DMA_TIME_PS_DMA_CMD_REC_ALIGN               0
#define IDE_DMA_TIME_PS_DMA_CMD_REC_BITS                4
#define IDE_DMA_TIME_PS_DMA_CMD_REC_MASK                0x0000000FUL
#define IDE_DMA_TIME_PS_DMA_CMD_REC_SHIFT               0

/* PS_DMA_CMD [07:04] - unsigned */
#define IDE_DMA_TIME_PS_DMA_CMD_ALIGN                   0
#define IDE_DMA_TIME_PS_DMA_CMD_BITS                    4
#define IDE_DMA_TIME_PS_DMA_CMD_MASK                    0x000000F0UL
#define IDE_DMA_TIME_PS_DMA_CMD_SHIFT                   4

/* PM_DMA_CMD_REC [11:08] - unsigned */
#define IDE_DMA_TIME_PM_DMA_CMD_REC_ALIGN               0
#define IDE_DMA_TIME_PM_DMA_CMD_REC_BITS                4
#define IDE_DMA_TIME_PM_DMA_CMD_REC_MASK                0x00000F00UL
#define IDE_DMA_TIME_PM_DMA_CMD_REC_SHIFT               8

/* PM_DMA_CMD [15:12] - unsigned */
#define IDE_DMA_TIME_PM_DMA_CMD_ALIGN                   0
#define IDE_DMA_TIME_PM_DMA_CMD_BITS                    4
#define IDE_DMA_TIME_PM_DMA_CMD_MASK                    0x0000F000UL
#define IDE_DMA_TIME_PM_DMA_CMD_SHIFT                   12


/* SS_DMA_CMD_REC [19:16] - unsigned */
#define IDE_DMA_TIME_SS_DMA_CMD_REC_ALIGN               0
#define IDE_DMA_TIME_SS_DMA_CMD_REC_BITS                4
#define IDE_DMA_TIME_SS_DMA_CMD_REC_MASK                0x000F0000UL
#define IDE_DMA_TIME_SS_DMA_CMD_REC_SHIFT               16

/* SS_DMA_CMD [23:20] - unsigned */
#define IDE_DMA_TIME_SS_DMA_CMD_ALIGN                   0
#define IDE_DMA_TIME_SS_DMA_CMD_BITS                    4
#define IDE_DMA_TIME_SS_DMA_CMD_MASK                    0x00F00000UL
#define IDE_DMA_TIME_SS_DMA_CMD_SHIFT                   20

/* SM_DMA_CMD_REC [27:24] - unsigned */
#define IDE_DMA_TIME_SM_DMA_CMD_REC_ALIGN               0
#define IDE_DMA_TIME_SM_DMA_CMD_REC_BITS                4
#define IDE_DMA_TIME_SM_DMA_CMD_REC_MASK                0x0F000000UL
#define IDE_DMA_TIME_SM_DMA_CMD_REC_SHIFT               24

/* SM_DMA_CMD [31:28] - unsigned */
#define IDE_DMA_TIME_SM_DMA_CMD_ALIGN                   0
#define IDE_DMA_TIME_SM_DMA_CMD_BITS                    4
#define IDE_DMA_TIME_SM_DMA_CMD_MASK                    0xF0000000UL
#define IDE_DMA_TIME_SM_DMA_CMD_SHIFT                   28

/**********************************************************************
 *IDE_PIO_CNTL_MODE
 **********************************************************************/
/* DIS_PRI [00:00] - boolean */
#define IDE_PIO_CNTL_MODE_DIS_PRI_ALIGN                 0
#define IDE_PIO_CNTL_MODE_DIS_PRI_BITS                  1
#define IDE_PIO_CNTL_MODE_DIS_PRI_MASK                  0x00000001UL
#define IDE_PIO_CNTL_MODE_DIS_PRI_SHIFT                 0

/* PS_PST_WR_ENA [04:04] - boolean */
#define IDE_PIO_CNTL_MODE_PS_PST_WR_ENA_ALIGN           0
#define IDE_PIO_CNTL_MODE_PS_PST_WR_ENA_BITS            1
#define IDE_PIO_CNTL_MODE_PS_PST_WR_ENA_MASK            0x00000010UL
#define IDE_PIO_CNTL_MODE_PS_PST_WR_ENA_SHIFT           4

/* PM_PST_WR_ENA [05:05] - boolean */
#define IDE_PIO_CNTL_MODE_PM_PST_WR_ENA_ALIGN           0
#define IDE_PIO_CNTL_MODE_PM_PST_WR_ENA_BITS            1
#define IDE_PIO_CNTL_MODE_PM_PST_WR_ENA_MASK            0x00000020UL
#define IDE_PIO_CNTL_MODE_PM_PST_WR_ENA_SHIFT           5

/* PS_PRF_RD_ENA [06:06] - boolean */
#define IDE_PIO_CNTL_MODE_PS_PRF_RD_ENA_ALIGN           0
#define IDE_PIO_CNTL_MODE_PS_PRF_RD_ENA_BITS            1
#define IDE_PIO_CNTL_MODE_PS_PRF_RD_ENA_MASK            0x00000040UL
#define IDE_PIO_CNTL_MODE_PS_PRF_RD_ENA_SHIFT           6

/* PM_PRF_RD_ENA [07:07] - boolean */
#define IDE_PIO_CNTL_MODE_PM_PRF_RD_ENA_ALIGN           0
#define IDE_PIO_CNTL_MODE_PM_PRF_RD_ENA_BITS            1
#define IDE_PIO_CNTL_MODE_PM_PRF_RD_ENA_MASK            0x00000080UL
#define IDE_PIO_CNTL_MODE_PM_PRF_RD_ENA_SHIFT           7

/* DIS_SEC [08:08] - boolean */
#define IDE_PIO_CNTL_MODE_DIS_SEC_ALIGN                 0
#define IDE_PIO_CNTL_MODE_DIS_SEC_BITS                  1
#define IDE_PIO_CNTL_MODE_DIS_SEC_MASK                  0x00000100UL
#define IDE_PIO_CNTL_MODE_DIS_SEC_SHIFT                 8

/* SS_PST_WR_ENA [12:12] - boolean */
#define IDE_PIO_CNTL_MODE_SS_PST_WR_ENA_ALIGN           0
#define IDE_PIO_CNTL_MODE_SS_PST_WR_ENA_BITS            1
#define IDE_PIO_CNTL_MODE_SS_PST_WR_ENA_MASK            0x00001000UL
#define IDE_PIO_CNTL_MODE_SS_PST_WR_ENA_SHIFT           12

/* SM_PST_WR_ENA [13:13] - boolean */
#define IDE_PIO_CNTL_MODE_SM_PST_WR_ENA_ALIGN           0
#define IDE_PIO_CNTL_MODE_SM_PST_WR_ENA_BITS            1
#define IDE_PIO_CNTL_MODE_SM_PST_WR_ENA_MASK            0x00002000UL
#define IDE_PIO_CNTL_MODE_SM_PST_WR_ENA_SHIFT           13

/* SS_PRF_RD_ENA [14:14] - boolean */
#define IDE_PIO_CNTL_MODE_SS_PRF_RD_ENA_ALIGN           0
#define IDE_PIO_CNTL_MODE_SS_PRF_RD_ENA_BITS            1
#define IDE_PIO_CNTL_MODE_SS_PRF_RD_ENA_MASK            0x00004000UL
#define IDE_PIO_CNTL_MODE_SS_PRF_RD_ENA_SHIFT           14

/* SM_PRF_RD_ENA [15:15] - boolean */
#define IDE_PIO_CNTL_MODE_SM_PRF_RD_ENA_ALIGN           0
#define IDE_PIO_CNTL_MODE_SM_PRF_RD_ENA_BITS            1
#define IDE_PIO_CNTL_MODE_SM_PRF_RD_ENA_MASK            0x00008000UL
#define IDE_PIO_CNTL_MODE_SM_PRF_RD_ENA_SHIFT           15

/* PM_PIO_ACC_MD [18:16] - unsigned */
#define IDE_PIO_CNTL_MODE_PM_PIO_ACC_MD_ALIGN           0
#define IDE_PIO_CNTL_MODE_PM_PIO_ACC_MD_BITS            3
#define IDE_PIO_CNTL_MODE_PM_PIO_ACC_MD_MASK            0x00070000UL
#define IDE_PIO_CNTL_MODE_PM_PIO_ACC_MD_SHIFT           16

/* PS_PIO_ACC_MD [22:20] - unsigned */
#define IDE_PIO_CNTL_MODE_PS_PIO_ACC_MD_ALIGN           0
#define IDE_PIO_CNTL_MODE_PS_PIO_ACC_MD_BITS            3
#define IDE_PIO_CNTL_MODE_PS_PIO_ACC_MD_MASK            0x00700000UL
#define IDE_PIO_CNTL_MODE_PS_PIO_ACC_MD_SHIFT           20

/* SM_PIO_ACC_MD [26:24] - unsigned */
#define IDE_PIO_CNTL_MODE_SM_PIO_ACC_MD_ALIGN           0
#define IDE_PIO_CNTL_MODE_SM_PIO_ACC_MD_BITS            3
#define IDE_PIO_CNTL_MODE_SM_PIO_ACC_MD_MASK            0x07000000UL
#define IDE_PIO_CNTL_MODE_SM_PIO_ACC_MD_SHIFT           24

/* SS_PIO_ACC_MD [30:28] - unsigned */
#define IDE_PIO_CNTL_MODE_SS_PIO_ACC_MD_ALIGN           0
#define IDE_PIO_CNTL_MODE_SS_PIO_ACC_MD_BITS            3
#define IDE_PIO_CNTL_MODE_SS_PIO_ACC_MD_MASK            0x70000000UL
#define IDE_PIO_CNTL_MODE_SS_PIO_ACC_MD_SHIFT           28

/**********************************************************************
 *IDE_UDMA_CNTL_STAT
 **********************************************************************/
/* PM_UDMA [00:00] - boolean */
#define IDE_UDMA_CNTL_STAT_PM_UDMA_ALIGN                0
#define IDE_UDMA_CNTL_STAT_PM_UDMA_BITS                 1
#define IDE_UDMA_CNTL_STAT_PM_UDMA_MASK                 0x00000001UL
#define IDE_UDMA_CNTL_STAT_PM_UDMA_SHIFT                0
#define IDE_UDMA_CNTL_STAT_PM_UDMA_DISABLE              0
#define IDE_UDMA_CNTL_STAT_PM_UDMA_ENABLE               1

/* PS_UDMA [01:01] - boolean */
#define IDE_UDMA_CNTL_STAT_PS_UDMA_ALIGN                0
#define IDE_UDMA_CNTL_STAT_PS_UDMA_BITS                 1
#define IDE_UDMA_CNTL_STAT_PS_UDMA_MASK                 0x00000002UL
#define IDE_UDMA_CNTL_STAT_PS_UDMA_SHIFT                1
#define IDE_UDMA_CNTL_STAT_PS_UDMA_DISABLE              0
#define IDE_UDMA_CNTL_STAT_PS_UDMA_ENABLE               1

/* SM_UDMA [02:02] - boolean */
#define IDE_UDMA_CNTL_STAT_SM_UDMA_ALIGN                0
#define IDE_UDMA_CNTL_STAT_SM_UDMA_BITS                 1
#define IDE_UDMA_CNTL_STAT_SM_UDMA_MASK                 0x00000004UL
#define IDE_UDMA_CNTL_STAT_SM_UDMA_SHIFT                2
#define IDE_UDMA_CNTL_STAT_SM_UDMA_DISABLE              0
#define IDE_UDMA_CNTL_STAT_SM_UDMA_ENABLE               1

/* SS_UDMA [03:03] - boolean */
#define IDE_UDMA_CNTL_STAT_SS_UDMA_ALIGN                0
#define IDE_UDMA_CNTL_STAT_SS_UDMA_BITS                 1
#define IDE_UDMA_CNTL_STAT_SS_UDMA_MASK                 0x00000008UL
#define IDE_UDMA_CNTL_STAT_SS_UDMA_SHIFT                3
#define IDE_UDMA_CNTL_STAT_SS_UDMA_DISABLE              0
#define IDE_UDMA_CNTL_STAT_SS_UDMA_ENABLE               1

/* REPRT_MD [07:07] - boolean */
#define IDE_UDMA_CNTL_STAT_REPRT_MD_ALIGN               0
#define IDE_UDMA_CNTL_STAT_REPRT_MD_BITS                1
#define IDE_UDMA_CNTL_STAT_REPRT_MD_MASK                0x00000080UL
#define IDE_UDMA_CNTL_STAT_REPRT_MD_SHIFT               7
						 
/* EXTRA_DATA [08:08] - boolean */
#define IDE_UDMA_CNTL_STAT_PRI_EXTRA_ALIGN              0
#define IDE_UDMA_CNTL_STAT_PRI_EXTRA_BITS               1
#define IDE_UDMA_CNTL_STAT_PRI_EXTRA_MASK               0x00000100UL
#define IDE_UDMA_CNTL_STAT_PRI_EXTRA_SHIFT              8

/* EXTRA_DATA [12:12] - boolean */
#define IDE_UDMA_CNTL_STAT_SEC_EXTRA_ALIGN              0
#define IDE_UDMA_CNTL_STAT_SEC_EXTRA_BITS               1
#define IDE_UDMA_CNTL_STAT_SEC_EXTRA_MASK               0x00001000UL
#define IDE_UDMA_CNTL_STAT_SEC_EXTRA_SHIFT              12

/* PM_UDMA_MODE [18:16] - unsigned */
#define IDE_UDMA_CNTL_STAT_PM_UDMA_MODE_ALIGN           0
#define IDE_UDMA_CNTL_STAT_PM_UDMA_MODE_BITS            3
#define IDE_UDMA_CNTL_STAT_PM_UDMA_MODE_MASK            0x00070000UL
#define IDE_UDMA_CNTL_STAT_PM_UDMA_MODE_SHIFT           16
						  
/* PS_UDMA_MODE [22:20] - unsigned */
#define IDE_UDMA_CNTL_STAT_PS_UDMA_MODE_ALIGN           0
#define IDE_UDMA_CNTL_STAT_PS_UDMA_MODE_BITS            3
#define IDE_UDMA_CNTL_STAT_PS_UDMA_MODE_MASK            0x00700000UL
#define IDE_UDMA_CNTL_STAT_PS_UDMA_MODE_SHIFT           20
						 
/* SM_UDMA_MODE [26:24] - unsigned */
#define IDE_UDMA_CNTL_STAT_SM_UDMA_MODE_ALIGN           0
#define IDE_UDMA_CNTL_STAT_SM_UDMA_MODE_BITS            3
#define IDE_UDMA_CNTL_STAT_SM_UDMA_MODE_MASK            0x07000000UL
#define IDE_UDMA_CNTL_STAT_SM_UDMA_MODE_SHIFT           24

/* SS_UDMA_MODE [30:28] - unsigned */
#define IDE_UDMA_CNTL_STAT_SS_UDMA_MODE_ALIGN           0
#define IDE_UDMA_CNTL_STAT_SS_UDMA_MODE_BITS            3
#define IDE_UDMA_CNTL_STAT_SS_UDMA_MODE_MASK            0x70000000UL
#define IDE_UDMA_CNTL_STAT_SS_UDMA_MODE_SHIFT           28

/**********************************************************************
 *IDE_UDMA_MODE
 **********************************************************************/
/* MAX_PIO_BURST [07:00] - unsigned */
#define IDE_UDMA_MODE_MAX_PIO_BURST_ALIGN               0
#define IDE_UDMA_MODE_MAX_PIO_BURST_BITS                8
#define IDE_UDMA_MODE_MAX_PIO_BURST_MASK                0x000000FFUL
#define IDE_UDMA_MODE_MAX_PIO_BURST_SHIFT               0
						 
/* MIN_DMA_BURST [08:16] - unsigned */
#define IDE_UDMA_MODE_MIN_DMA_BURST_ALIGN               0
#define IDE_UDMA_MODE_MIN_DMA_BURST_BITS                8
#define IDE_UDMA_MODE_MIN_DMA_BURST_MASK                0x0000FF00UL
#define IDE_UDMA_MODE_MIN_DMA_BURST_SHIFT               8

/**********************************************************************
 *IDE_IF_CNTL_STAT
 **********************************************************************/
/* LB_PST_WR_ENA [16:16] - boolean */
#define IDE_IF_CNTL_STAT_LB_PST_WR_ENA_ALIGN            0
#define IDE_IF_CNTL_STAT_LB_PST_WR_ENA_BITS             1
#define IDE_IF_CNTL_STAT_LB_PST_WR_ENA_MASK             0x00010000UL
#define IDE_IF_CNTL_STAT_LB_PST_WR_ENA_SHIFT            16

/* DT_BYTE_SWP_ENA [17:17] - boolean */
#define IDE_IF_CNTL_STAT_DT_BYTE_SWP_ENA_ALIGN          0
#define IDE_IF_CNTL_STAT_DT_BYTE_SWP_ENA_BITS           1
#define IDE_IF_CNTL_STAT_DT_BYTE_SWP_ENA_MASK           0x00020000UL
#define IDE_IF_CNTL_STAT_DT_BYTE_SWP_ENA_SHIFT          17

/* DATA_SWP_TYPE [19:18] - unsigned */
#define IDE_IF_CNTL_STAT_DATA_SWP_TYPE_ALIGN            0
#define IDE_IF_CNTL_STAT_DATA_SWP_TYPE_BITS             2
#define IDE_IF_CNTL_STAT_DATA_SWP_TYPE_MASK             0x000C0000UL
#define IDE_IF_CNTL_STAT_DATA_SWP_TYPE_SHIFT            18

/* PIO_WR_WAIT [24:24] - boolean */
#define IDE_IF_CNTL_STAT_PIO_WR_WAIT_ALIGN              0
#define IDE_IF_CNTL_STAT_PIO_WR_WAIT_BITS               1
#define IDE_IF_CNTL_STAT_PIO_WR_WAIT_MASK               0x01000000UL
#define IDE_IF_CNTL_STAT_PIO_WR_WAIT_SHIFT              24

/* PIO_RD_WAIT [25:25] - boolean */
#define IDE_IF_CNTL_STAT_PIO_RD_WAIT_ALIGN              0
#define IDE_IF_CNTL_STAT_PIO_RD_WAIT_BITS               1
#define IDE_IF_CNTL_STAT_PIO_RD_WAIT_MASK               0x02000000UL
#define IDE_IF_CNTL_STAT_PIO_RD_WAIT_SHIFT              25


/**********************************************************************
 *IDE_MBIST_CNTL_STAT
 **********************************************************************/
/* MBIST_ENA_RD_FIFO [00:00] - boolean */
#define IDE_MBIST_CNTL_STAT_MBIST_ENA_RD_FIFO_ALIGN     0
#define IDE_MBIST_CNTL_STAT_MBIST_ENA_RD_FIFO_BITS      1
#define IDE_MBIST_CNTL_STAT_MBIST_ENA_RD_FIFO_MASK      0x00000001UL
#define IDE_MBIST_CNTL_STAT_MBIST_ENA_RD_FIFO_SHIFT     0

/* MBIST_HOLD_RD_FIFO [01:01] - boolean */
#define IDE_MBIST_CNTL_STAT_MBIST_HOLD_RD_FIFO_ALIGN    0
#define IDE_MBIST_CNTL_STAT_MBIST_HOLD_RD_FIFO_BITS     1
#define IDE_MBIST_CNTL_STAT_MBIST_HOLD_RD_FIFO_MASK     0x00000002UL
#define IDE_MBIST_CNTL_STAT_MBIST_HOLD_RD_FIFO_SHIFT    1

/* MBIST_DBG_ENA_RD_FIFO [02:02] - boolean */
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_ENA_RD_FIFO_ALIGN 0
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_ENA_RD_FIFO_BITS  1
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_ENA_RD_FIFO_MASK  0x00000004UL
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_ENA_RD_FIFO_SHIFT 2

/* MBIST_DMP_RD_FIFO [03:03] - boolean */
#define IDE_MBIST_CNTL_STAT_MBIST_DMP_RD_FIFO_ALIGN     0
#define IDE_MBIST_CNTL_STAT_MBIST_DMP_RD_FIFO_BITS      1
#define IDE_MBIST_CNTL_STAT_MBIST_DMP_RD_FIFO_MASK      0x00000008UL
#define IDE_MBIST_CNTL_STAT_MBIST_DMP_RD_FIFO_SHIFT     3

/* MBIST_ENA_WR_FIFO [04:04] - boolean */
#define IDE_MBIST_CNTL_STAT_MBIST_ENA_WR_FIFO_ALIGN     0
#define IDE_MBIST_CNTL_STAT_MBIST_ENA_WR_FIFO_BITS      1
#define IDE_MBIST_CNTL_STAT_MBIST_ENA_WR_FIFO_MASK      0x00000010UL
#define IDE_MBIST_CNTL_STAT_MBIST_ENA_WR_FIFO_SHIFT     4

/* MBIST_HOLD_WR_FIFO [05:05] - boolean */
#define IDE_MBIST_CNTL_STAT_MBIST_HOLD_WR_FIFO_ALIGN    0
#define IDE_MBIST_CNTL_STAT_MBIST_HOLD_WR_FIFO_BITS     1
#define IDE_MBIST_CNTL_STAT_MBIST_HOLD_WR_FIFO_MASK     0x00000020UL
#define IDE_MBIST_CNTL_STAT_MBIST_HOLD_WR_FIFO_SHIFT    5

/* MBIST_DBG_ENA_WR_FIFO [06:06] - boolean */
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_ENA_WR_FIFO_ALIGN 0
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_ENA_WR_FIFO_BITS  1
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_ENA_WR_FIFO_MASK  0x00000040UL
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_ENA_WR_FIFO_SHIFT 6

/* MBIST_DMP_WR_FIFO [07:07] - boolean */
#define IDE_MBIST_CNTL_STAT_MBIST_DMP_WR_FIFO_ALIGN     0
#define IDE_MBIST_CNTL_STAT_MBIST_DMP_WR_FIFO_BITS      1
#define IDE_MBIST_CNTL_STAT_MBIST_DMP_WR_FIFO_MASK      0x00000080UL
#define IDE_MBIST_CNTL_STAT_MBIST_DMP_WR_FIFO_SHIFT     7

/* MBIST_DONE_RD_FIFO [08:08] - boolean */
#define IDE_MBIST_CNTL_STAT_MBIST_DONE_RD_FIFO_ALIGN    0
#define IDE_MBIST_CNTL_STAT_MBIST_DONE_RD_FIFO_BITS     1
#define IDE_MBIST_CNTL_STAT_MBIST_DONE_RD_FIFO_MASK     0x00000100UL
#define IDE_MBIST_CNTL_STAT_MBIST_DONE_RD_FIFO_SHIFT    8

/* MBIST_DBG_OUT_RD_FIFO [09:09] - boolean */
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_OUT_RD_FIFO_ALIGN 0
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_OUT_RD_FIFO_BITS  1
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_OUT_RD_FIFO_MASK  0x00000200UL
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_OUT_RD_FIFO_SHIFT 9

/* MBIST_FAIL_RD_FIFO [10:10] - boolean */
#define IDE_MBIST_CNTL_STAT_MBIST_FAIL_RD_FIFO_ALIGN    0
#define IDE_MBIST_CNTL_STAT_MBIST_FAIL_RD_FIFO_BITS     1
#define IDE_MBIST_CNTL_STAT_MBIST_FAIL_RD_FIFO_MASK     0x00000400UL
#define IDE_MBIST_CNTL_STAT_MBIST_FAIL_RD_FIFO_SHIFT    10

/* MBIST_DONE_WR_FIFO [12:12] - boolean */
#define IDE_MBIST_CNTL_STAT_MBIST_DONE_WR_FIFO_ALIGN    0
#define IDE_MBIST_CNTL_STAT_MBIST_DONE_WR_FIFO_BITS     1
#define IDE_MBIST_CNTL_STAT_MBIST_DONE_WR_FIFO_MASK     0x00001000UL
#define IDE_MBIST_CNTL_STAT_MBIST_DONE_WR_FIFO_SHIFT    12

/* MBIST_DBG_OUT_WR_FIFO [13:13] - boolean */
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_OUT_WR_FIFO_ALIGN 0
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_OUT_WR_FIFO_BITS  1
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_OUT_WR_FIFO_MASK  0x00002000UL
#define IDE_MBIST_CNTL_STAT_MBIST_DBG_OUT_WR_FIFO_SHIFT 13

/* MBIST_FAIL_WR_FIFO [14:14] - boolean */
#define IDE_MBIST_CNTL_STAT_MBIST_FAIL_WR_FIFO_ALIGN    0
#define IDE_MBIST_CNTL_STAT_MBIST_FAIL_WR_FIFO_BITS     1
#define IDE_MBIST_CNTL_STAT_MBIST_FAIL_WR_FIFO_MASK     0x00004000UL
#define IDE_MBIST_CNTL_STAT_MBIST_FAIL_WR_FIFO_SHIFT    14


/**********************************************************************
 *IDE_PRI_DTE_A
 **********************************************************************/
/* DT_START_ADDR [31:00] - unsigned */
#define IDE_PRI_DTE_A_DT_START_ADDR_ALIGN               0
#define IDE_PRI_DTE_A_DT_START_ADDR_BITS                32
#define IDE_PRI_DTE_A_DT_START_ADDR_MASK                0xFFFFFFFFUL
#define IDE_PRI_DTE_A_DT_START_ADDR_SHIFT               0


/**********************************************************************
 *IDE_PRI_DTE_B
 **********************************************************************/
/* DT_BYTE_CNT [15:00] - unsigned */
#define IDE_PRI_DTE_B_DT_BYTE_CNT_ALIGN                 0
#define IDE_PRI_DTE_B_DT_BYTE_CNT_BITS                  16
#define IDE_PRI_DTE_B_DT_BYTE_CNT_MASK                  0x0000FFFFUL
#define IDE_PRI_DTE_B_DT_BYTE_CNT_SHIFT                 0

/* EOT [31:31] - boolean */
#define IDE_PRI_DTE_B_EOT_ALIGN                         0
#define IDE_PRI_DTE_B_EOT_BITS                          1
#define IDE_PRI_DTE_B_EOT_MASK                          0x80000000UL
#define IDE_PRI_DTE_B_EOT_SHIFT                         31


/**********************************************************************
 *IDE_PRI_INT_MSK
 **********************************************************************/
/* IDE_MASK [00:00] - boolean */
#define IDE_PRI_INT_MSK_IDE_MASK_ALIGN                  0
#define IDE_PRI_INT_MSK_IDE_MASK_BITS                   1
#define IDE_PRI_INT_MSK_IDE_MASK_MASK                   0x00000001UL
#define IDE_PRI_INT_MSK_IDE_MASK_SHIFT                  0

/* EOT_MASK [01:01] - boolean */
#define IDE_PRI_INT_MSK_EOT_MASK_ALIGN                  0
#define IDE_PRI_INT_MSK_EOT_MASK_BITS                   1
#define IDE_PRI_INT_MSK_EOT_MASK_MASK                   0x00000002UL
#define IDE_PRI_INT_MSK_EOT_MASK_SHIFT                  1


/**********************************************************************
 *IDE_PRI_INT
 **********************************************************************/
/* EOT_INT [01:01] - boolean */
#define IDE_PRI_INT_EOT_INT_ALIGN                       0
#define IDE_PRI_INT_EOT_INT_BITS                        1
#define IDE_PRI_INT_EOT_INT_MASK                        0x00000002UL
#define IDE_PRI_INT_EOT_INT_SHIFT                       1

				   
/**********************************************************************
 *IDE_PRI_DATA
 **********************************************************************/
/* PIO_DATA [15:00] - unsigned */
#define IDE_PRI_DATA_PIO_DATA_ALIGN                     0
#define IDE_PRI_DATA_PIO_DATA_BITS                      16
#define IDE_PRI_DATA_PIO_DATA_MASK                      0x0000FFFFUL
#define IDE_PRI_DATA_PIO_DATA_SHIFT                     0
				   

/**********************************************************************
 *IDE_PRI_FEAT_ERR
 **********************************************************************/
/* FEAT_ERROR [07:00] - unsigned */
#define IDE_PRI_FEAT_ERR_FEAT_ERROR_ALIGN               0
#define IDE_PRI_FEAT_ERR_FEAT_ERROR_BITS                8
#define IDE_PRI_FEAT_ERR_FEAT_ERROR_MASK                0x000000FFUL
#define IDE_PRI_FEAT_ERR_FEAT_ERROR_SHIFT               0
				   

/**********************************************************************
 *IDE_PRI_SEC_CNT
 **********************************************************************/
/* SECTOR_CNT [07:00] - unsigned */
#define IDE_PRI_SEC_CNT_SECTOR_CNT_ALIGN                0
#define IDE_PRI_SEC_CNT_SECTOR_CNT_BITS                 8
#define IDE_PRI_SEC_CNT_SECTOR_CNT_MASK                 0x000000FFUL
#define IDE_PRI_SEC_CNT_SECTOR_CNT_SHIFT                0


/**********************************************************************
 *IDE_PRI_SEC_NUM
 **********************************************************************/
/* SECTOR_NUM [07:00] - unsigned */
#define IDE_PRI_SEC_NUM_SECTOR_NUM_ALIGN                0
#define IDE_PRI_SEC_NUM_SECTOR_NUM_BITS                 8
#define IDE_PRI_SEC_NUM_SECTOR_NUM_MASK                 0x000000FFUL
#define IDE_PRI_SEC_NUM_SECTOR_NUM_SHIFT                0


/**********************************************************************
 *IDE_PRI_CYL_LOW
 **********************************************************************/
/* CYL_LOW [07:00] - unsigned */
#define IDE_PRI_CYL_LOW_CYL_LOW_ALIGN                   0
#define IDE_PRI_CYL_LOW_CYL_LOW_BITS                    8
#define IDE_PRI_CYL_LOW_CYL_LOW_MASK                    0x000000FFUL
#define IDE_PRI_CYL_LOW_CYL_LOW_SHIFT                   0


/**********************************************************************
 *IDE_PRI_CYL_HI
 **********************************************************************/
/* CYL_HI [07:00] - unsigned */
#define IDE_PRI_CYL_HI_CYL_HI_ALIGN                     0
#define IDE_PRI_CYL_HI_CYL_HI_BITS                      8
#define IDE_PRI_CYL_HI_CYL_HI_MASK                      0x000000FFUL
#define IDE_PRI_CYL_HI_CYL_HI_SHIFT                     0


/**********************************************************************
 *IDE_PRI_DEV_HD
 **********************************************************************/
/* DEV_HD [07:00] - unsigned */
#define IDE_PRI_DEV_HD_DEV_HD_ALIGN                     0
#define IDE_PRI_DEV_HD_DEV_HD_BITS                      8
#define IDE_PRI_DEV_HD_DEV_HD_MASK                      0x000000FFUL
#define IDE_PRI_DEV_HD_DEV_HD_SHIFT                     0


/**********************************************************************
 *IDE_PRI_CMD_STAT
 **********************************************************************/
/* CMD_STAT [07:00] - unsigned */
#define IDE_PRI_CMD_STAT_CMD_STAT_ALIGN                 0
#define IDE_PRI_CMD_STAT_CMD_STAT_BITS                  8
#define IDE_PRI_CMD_STAT_CMD_STAT_MASK                  0x000000FFUL
#define IDE_PRI_CMD_STAT_CMD_STAT_SHIFT                 0
					
					  
/**********************************************************************
 *IDE_PRI_DEV_C_STAT
 **********************************************************************/
/* DEV_CNTL_ALT_STAT [07:00] - unsigned */
#define IDE_PRI_DEV_C_STAT_DEV_CNTL_ALT_STAT_ALIGN      0
#define IDE_PRI_DEV_C_STAT_DEV_CNTL_ALT_STAT_BITS       8
#define IDE_PRI_DEV_C_STAT_DEV_CNTL_ALT_STAT_MASK       0x000000FFUL
#define IDE_PRI_DEV_C_STAT_DEV_CNTL_ALT_STAT_SHIFT      0


/**********************************************************************
 *IDE_PRI_BM_CMD_STAT
 **********************************************************************/
/* START_STOP [00:00] - boolean */
#define IDE_PRI_BM_CMD_STAT_START_STOP_ALIGN            0
#define IDE_PRI_BM_CMD_STAT_START_STOP_BITS             1
#define IDE_PRI_BM_CMD_STAT_START_STOP_MASK             0x00000001UL
#define IDE_PRI_BM_CMD_STAT_START_STOP_SHIFT            0

/* RD_WR [03:03] - boolean */
#define IDE_PRI_BM_CMD_STAT_RD_WR_ALIGN                 0
#define IDE_PRI_BM_CMD_STAT_RD_WR_BITS                  1
#define IDE_PRI_BM_CMD_STAT_RD_WR_MASK                  0x00000008UL
#define IDE_PRI_BM_CMD_STAT_RD_WR_SHIFT                 3

/* ACTIVE [16:16] - boolean */
#define IDE_PRI_BM_CMD_STAT_ACTIVE_ALIGN                0
#define IDE_PRI_BM_CMD_STAT_ACTIVE_BITS                 1
#define IDE_PRI_BM_CMD_STAT_ACTIVE_MASK                 0x00010000UL
#define IDE_PRI_BM_CMD_STAT_ACTIVE_SHIFT                16

/* INTRQ [18:18] - boolean */
#define IDE_PRI_BM_CMD_STAT_INTRQ_ALIGN                 0
#define IDE_PRI_BM_CMD_STAT_INTRQ_BITS                  1
#define IDE_PRI_BM_CMD_STAT_INTRQ_MASK                  0x00040000UL
#define IDE_PRI_BM_CMD_STAT_INTRQ_SHIFT                 18

/* DRV0_DMA [21:21] - boolean */
#define IDE_PRI_BM_CMD_STAT_DRV0_DMA_ALIGN              0
#define IDE_PRI_BM_CMD_STAT_DRV0_DMA_BITS               1
#define IDE_PRI_BM_CMD_STAT_DRV0_DMA_MASK               0x00200000UL
#define IDE_PRI_BM_CMD_STAT_DRV0_DMA_SHIFT              21     

/* DRV1_DMA [22:22] - boolean */
#define IDE_PRI_BM_CMD_STAT_DRV1_DMA_ALIGN              0
#define IDE_PRI_BM_CMD_STAT_DRV1_DMA_BITS               1
#define IDE_PRI_BM_CMD_STAT_DRV1_DMA_MASK               0x00400000UL
#define IDE_PRI_BM_CMD_STAT_DRV1_DMA_SHIFT              22

/* SIMPLEX [23:23] - boolean */
#define IDE_PRI_BM_CMD_STAT_SIMPLEX_ALIGN               0
#define IDE_PRI_BM_CMD_STAT_SIMPLEX_BITS                1
#define IDE_PRI_BM_CMD_STAT_SIMPLEX_MASK                0x00800000UL
#define IDE_PRI_BM_CMD_STAT_SIMPLEX_SHIFT               23


/**********************************************************************
 *IDE_PRI_DT_PT
 **********************************************************************/
/* DT_PTR [31:00] - unsigned */
#define IDE_PRI_DT_PT_DT_PTR_ALIGN                      0
#define IDE_PRI_DT_PT_DT_PTR_BITS                       32
#define IDE_PRI_DT_PT_DT_PTR_MASK                       0xFFFFFFFFUL
#define IDE_PRI_DT_PT_DT_PTR_SHIFT                      0

					

#endif /* #ifndef BCM7110_IDE_H__ */
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

/* End of File */

