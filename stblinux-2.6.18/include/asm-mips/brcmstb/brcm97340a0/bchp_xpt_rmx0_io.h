/***************************************************************************
 *     Copyright (c) 1999-2009, Broadcom Corporation
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
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Mar 20 12:59:20 2009
 *                 MD5 Checksum         4f0509cfa0b8fc4589050694b4a3e234
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7340/rdb/a0/bchp_xpt_rmx0_io.h $
 * 
 * Hydra_Software_Devel/1   3/22/09 9:36p yuxiaz
 * PR53430: Initial version of header files.
 *
 ***************************************************************************/

#ifndef BCHP_XPT_RMX0_IO_H__
#define BCHP_XPT_RMX0_IO_H__

/***************************************************************************
 *XPT_RMX0_IO - XPT RMX0 I/O Control Registers
 ***************************************************************************/
#define BCHP_XPT_RMX0_IO_FORMAT                  0x00383000 /* RMX Format Register */
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL            0x00383004 /* RMX Output Clk/Sync/Valid Delay Register */
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL            0x00383008 /* RMX Output Data Delay Register */
#define BCHP_XPT_RMX0_IO_PKT_DLY_CNT             0x0038300c /* RMX Packet Delay Control Register */
#define BCHP_XPT_RMX0_IO_TV_STATUS               0x00383020 /* TV Status */

/***************************************************************************
 *FORMAT - RMX Format Register
 ***************************************************************************/
/* XPT_RMX0_IO :: FORMAT :: reserved0 [31:17] */
#define BCHP_XPT_RMX0_IO_FORMAT_reserved0_MASK                     0xfffe0000
#define BCHP_XPT_RMX0_IO_FORMAT_reserved0_SHIFT                    17

/* XPT_RMX0_IO :: FORMAT :: RMX_PAUSE_EN [16:16] */
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_PAUSE_EN_MASK                  0x00010000
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_PAUSE_EN_SHIFT                 16

/* XPT_RMX0_IO :: FORMAT :: reserved1 [15:15] */
#define BCHP_XPT_RMX0_IO_FORMAT_reserved1_MASK                     0x00008000
#define BCHP_XPT_RMX0_IO_FORMAT_reserved1_SHIFT                    15

/* XPT_RMX0_IO :: FORMAT :: RMXP_ONLY_RATE [14:13] */
#define BCHP_XPT_RMX0_IO_FORMAT_RMXP_ONLY_RATE_MASK                0x00006000
#define BCHP_XPT_RMX0_IO_FORMAT_RMXP_ONLY_RATE_SHIFT               13

/* XPT_RMX0_IO :: FORMAT :: RMXP_ONLY_EN [12:12] */
#define BCHP_XPT_RMX0_IO_FORMAT_RMXP_ONLY_EN_MASK                  0x00001000
#define BCHP_XPT_RMX0_IO_FORMAT_RMXP_ONLY_EN_SHIFT                 12

/* XPT_RMX0_IO :: FORMAT :: RMXP_INVERT_VALID [11:11] */
#define BCHP_XPT_RMX0_IO_FORMAT_RMXP_INVERT_VALID_MASK             0x00000800
#define BCHP_XPT_RMX0_IO_FORMAT_RMXP_INVERT_VALID_SHIFT            11

/* XPT_RMX0_IO :: FORMAT :: RMXP_INVERT_SYNC [10:10] */
#define BCHP_XPT_RMX0_IO_FORMAT_RMXP_INVERT_SYNC_MASK              0x00000400
#define BCHP_XPT_RMX0_IO_FORMAT_RMXP_INVERT_SYNC_SHIFT             10

/* XPT_RMX0_IO :: FORMAT :: RMXP_INVERT_CLK [09:09] */
#define BCHP_XPT_RMX0_IO_FORMAT_RMXP_INVERT_CLK_MASK               0x00000200
#define BCHP_XPT_RMX0_IO_FORMAT_RMXP_INVERT_CLK_SHIFT              9

/* XPT_RMX0_IO :: FORMAT :: RMXP_ENABLE [08:08] */
#define BCHP_XPT_RMX0_IO_FORMAT_RMXP_ENABLE_MASK                   0x00000100
#define BCHP_XPT_RMX0_IO_FORMAT_RMXP_ENABLE_SHIFT                  8

/* XPT_RMX0_IO :: FORMAT :: reserved2 [07:07] */
#define BCHP_XPT_RMX0_IO_FORMAT_reserved2_MASK                     0x00000080
#define BCHP_XPT_RMX0_IO_FORMAT_reserved2_SHIFT                    7

/* XPT_RMX0_IO :: FORMAT :: RMX_TIMESTAMP_BF_SYNC [06:06] */
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_TIMESTAMP_BF_SYNC_MASK         0x00000040
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_TIMESTAMP_BF_SYNC_SHIFT        6

/* XPT_RMX0_IO :: FORMAT :: RMX_CLK_NRUN_EN [05:05] */
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_CLK_NRUN_EN_MASK               0x00000020
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_CLK_NRUN_EN_SHIFT              5

/* XPT_RMX0_IO :: FORMAT :: RMX_INVERT_VALID [04:04] */
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_INVERT_VALID_MASK              0x00000010
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_INVERT_VALID_SHIFT             4

/* XPT_RMX0_IO :: FORMAT :: RMX_INVERT_SYNC [03:03] */
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_INVERT_SYNC_MASK               0x00000008
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_INVERT_SYNC_SHIFT              3

/* XPT_RMX0_IO :: FORMAT :: RMX_INVERT_CLK [02:02] */
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_INVERT_CLK_MASK                0x00000004
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_INVERT_CLK_SHIFT               2

/* XPT_RMX0_IO :: FORMAT :: RMX_BYTE_SYNC [01:01] */
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_BYTE_SYNC_MASK                 0x00000002
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_BYTE_SYNC_SHIFT                1

/* XPT_RMX0_IO :: FORMAT :: RMX_MUTE [00:00] */
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_MUTE_MASK                      0x00000001
#define BCHP_XPT_RMX0_IO_FORMAT_RMX_MUTE_SHIFT                     0

/***************************************************************************
 *MISC_DLY_SEL - RMX Output Clk/Sync/Valid Delay Register
 ***************************************************************************/
/* XPT_RMX0_IO :: MISC_DLY_SEL :: reserved0 [31:28] */
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_reserved0_MASK               0xf0000000
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_reserved0_SHIFT              28

/* XPT_RMX0_IO :: MISC_DLY_SEL :: RMXP_VALID_DLY_SEL [27:24] */
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_RMXP_VALID_DLY_SEL_MASK      0x0f000000
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_RMXP_VALID_DLY_SEL_SHIFT     24

/* XPT_RMX0_IO :: MISC_DLY_SEL :: RMXP_SYNC_DLY_SEL [23:20] */
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_RMXP_SYNC_DLY_SEL_MASK       0x00f00000
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_RMXP_SYNC_DLY_SEL_SHIFT      20

/* XPT_RMX0_IO :: MISC_DLY_SEL :: RMXP_CLK_DLY_SEL [19:16] */
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_RMXP_CLK_DLY_SEL_MASK        0x000f0000
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_RMXP_CLK_DLY_SEL_SHIFT       16

/* XPT_RMX0_IO :: MISC_DLY_SEL :: RMX_VALID_DLY_SEL [15:12] */
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_RMX_VALID_DLY_SEL_MASK       0x0000f000
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_RMX_VALID_DLY_SEL_SHIFT      12

/* XPT_RMX0_IO :: MISC_DLY_SEL :: RMX_DATA_DLY_SEL [11:08] */
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_RMX_DATA_DLY_SEL_MASK        0x00000f00
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_RMX_DATA_DLY_SEL_SHIFT       8

/* XPT_RMX0_IO :: MISC_DLY_SEL :: RMX_SYNC_DLY_SEL [07:04] */
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_RMX_SYNC_DLY_SEL_MASK        0x000000f0
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_RMX_SYNC_DLY_SEL_SHIFT       4

/* XPT_RMX0_IO :: MISC_DLY_SEL :: RMX_CLK_DLY_SEL [03:00] */
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_RMX_CLK_DLY_SEL_MASK         0x0000000f
#define BCHP_XPT_RMX0_IO_MISC_DLY_SEL_RMX_CLK_DLY_SEL_SHIFT        0

/***************************************************************************
 *DATA_DLY_SEL - RMX Output Data Delay Register
 ***************************************************************************/
/* XPT_RMX0_IO :: DATA_DLY_SEL :: RMXP_DATA7_DLY_SEL [31:28] */
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA7_DLY_SEL_MASK      0xf0000000
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA7_DLY_SEL_SHIFT     28

/* XPT_RMX0_IO :: DATA_DLY_SEL :: RMXP_DATA6_DLY_SEL [27:24] */
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA6_DLY_SEL_MASK      0x0f000000
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA6_DLY_SEL_SHIFT     24

/* XPT_RMX0_IO :: DATA_DLY_SEL :: RMXP_DATA5_DLY_SEL [23:20] */
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA5_DLY_SEL_MASK      0x00f00000
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA5_DLY_SEL_SHIFT     20

/* XPT_RMX0_IO :: DATA_DLY_SEL :: RMXP_DATA4_DLY_SEL [19:16] */
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA4_DLY_SEL_MASK      0x000f0000
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA4_DLY_SEL_SHIFT     16

/* XPT_RMX0_IO :: DATA_DLY_SEL :: RMXP_DATA3_DLY_SEL [15:12] */
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA3_DLY_SEL_MASK      0x0000f000
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA3_DLY_SEL_SHIFT     12

/* XPT_RMX0_IO :: DATA_DLY_SEL :: RMXP_DATA2_DLY_SEL [11:08] */
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA2_DLY_SEL_MASK      0x00000f00
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA2_DLY_SEL_SHIFT     8

/* XPT_RMX0_IO :: DATA_DLY_SEL :: RMXP_DATA1_DLY_SEL [07:04] */
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA1_DLY_SEL_MASK      0x000000f0
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA1_DLY_SEL_SHIFT     4

/* XPT_RMX0_IO :: DATA_DLY_SEL :: RMXP_DATA0_DLY_SEL [03:00] */
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA0_DLY_SEL_MASK      0x0000000f
#define BCHP_XPT_RMX0_IO_DATA_DLY_SEL_RMXP_DATA0_DLY_SEL_SHIFT     0

/***************************************************************************
 *PKT_DLY_CNT - RMX Packet Delay Control Register
 ***************************************************************************/
/* XPT_RMX0_IO :: PKT_DLY_CNT :: reserved0 [31:09] */
#define BCHP_XPT_RMX0_IO_PKT_DLY_CNT_reserved0_MASK                0xfffffe00
#define BCHP_XPT_RMX0_IO_PKT_DLY_CNT_reserved0_SHIFT               9

/* XPT_RMX0_IO :: PKT_DLY_CNT :: RMX_PKT_DLY_CNT [08:00] */
#define BCHP_XPT_RMX0_IO_PKT_DLY_CNT_RMX_PKT_DLY_CNT_MASK          0x000001ff
#define BCHP_XPT_RMX0_IO_PKT_DLY_CNT_RMX_PKT_DLY_CNT_SHIFT         0

/***************************************************************************
 *TV_STATUS - TV Status
 ***************************************************************************/
/* XPT_RMX0_IO :: TV_STATUS :: reserved0 [31:09] */
#define BCHP_XPT_RMX0_IO_TV_STATUS_reserved0_MASK                  0xfffffe00
#define BCHP_XPT_RMX0_IO_TV_STATUS_reserved0_SHIFT                 9

/* XPT_RMX0_IO :: TV_STATUS :: ARM_CLK_EN [08:08] */
#define BCHP_XPT_RMX0_IO_TV_STATUS_ARM_CLK_EN_MASK                 0x00000100
#define BCHP_XPT_RMX0_IO_TV_STATUS_ARM_CLK_EN_SHIFT                8

/* XPT_RMX0_IO :: TV_STATUS :: reserved1 [07:00] */
#define BCHP_XPT_RMX0_IO_TV_STATUS_reserved1_MASK                  0x000000ff
#define BCHP_XPT_RMX0_IO_TV_STATUS_reserved1_SHIFT                 0

#endif /* #ifndef BCHP_XPT_RMX0_IO_H__ */

/* End of File */