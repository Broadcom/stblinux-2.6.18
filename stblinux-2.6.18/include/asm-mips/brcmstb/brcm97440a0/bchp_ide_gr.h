/***************************************************************************
 *     Copyright (c) 1999-2006, Broadcom Corporation
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
 * $brcm_Workfile: bchp_ide_gr.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/10/06 5:50p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Feb 10 16:03:18 2006
 *                 MD5 Checksum         c4938591a1dffcbb175df566d9f5abc4
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.006
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7440/rdb/a0/bchp_ide_gr.h $
 * 
 * Hydra_Software_Devel/1   2/10/06 5:50p jasonh
 * PR 19602: Initial version.
 *
 ***************************************************************************/

#ifndef BCHP_IDE_GR_H__
#define BCHP_IDE_GR_H__

/***************************************************************************
 *IDE_GR - IDE GISB-to-RBUS Bridge Registers
 ***************************************************************************/
#define BCHP_IDE_GR_REVISION                     0x01710080 /* GR Bridge Revision */
#define BCHP_IDE_GR_CTRL                         0x01710084 /* GR Bridge Control Register */
#define BCHP_IDE_GR_SW_RESET_0                   0x01710088 /* GR Bridge Software Reset 0 Register */
#define BCHP_IDE_GR_SW_RESET_1                   0x0171008c /* GR Bridge Software Reset 1 Register */

/***************************************************************************
 *REVISION - GR Bridge Revision
 ***************************************************************************/
/* IDE_GR :: REVISION :: reserved0 [31:16] */
#define BCHP_IDE_GR_REVISION_reserved0_MASK                        0xffff0000
#define BCHP_IDE_GR_REVISION_reserved0_SHIFT                       16

/* IDE_GR :: REVISION :: MAJOR [15:08] */
#define BCHP_IDE_GR_REVISION_MAJOR_MASK                            0x0000ff00
#define BCHP_IDE_GR_REVISION_MAJOR_SHIFT                           8

/* IDE_GR :: REVISION :: MINOR [07:00] */
#define BCHP_IDE_GR_REVISION_MINOR_MASK                            0x000000ff
#define BCHP_IDE_GR_REVISION_MINOR_SHIFT                           0

/***************************************************************************
 *CTRL - GR Bridge Control Register
 ***************************************************************************/
/* IDE_GR :: CTRL :: reserved0 [31:01] */
#define BCHP_IDE_GR_CTRL_reserved0_MASK                            0xfffffffe
#define BCHP_IDE_GR_CTRL_reserved0_SHIFT                           1

/* IDE_GR :: CTRL :: gisb_error_intr [00:00] */
#define BCHP_IDE_GR_CTRL_gisb_error_intr_MASK                      0x00000001
#define BCHP_IDE_GR_CTRL_gisb_error_intr_SHIFT                     0
#define BCHP_IDE_GR_CTRL_gisb_error_intr_INTR_DISABLE              0
#define BCHP_IDE_GR_CTRL_gisb_error_intr_INTR_ENABLE               1

/***************************************************************************
 *SW_RESET_0 - GR Bridge Software Reset 0 Register
 ***************************************************************************/
/* IDE_GR :: SW_RESET_0 :: reserved0 [31:03] */
#define BCHP_IDE_GR_SW_RESET_0_reserved0_MASK                      0xfffffff8
#define BCHP_IDE_GR_SW_RESET_0_reserved0_SHIFT                     3

/* IDE_GR :: SW_RESET_0 :: YOUR_NAME02_SW_RESET [02:02] */
#define BCHP_IDE_GR_SW_RESET_0_YOUR_NAME02_SW_RESET_MASK           0x00000004
#define BCHP_IDE_GR_SW_RESET_0_YOUR_NAME02_SW_RESET_SHIFT          2
#define BCHP_IDE_GR_SW_RESET_0_YOUR_NAME02_SW_RESET_DEASSERT       0
#define BCHP_IDE_GR_SW_RESET_0_YOUR_NAME02_SW_RESET_ASSERT         1

/* IDE_GR :: SW_RESET_0 :: YOUR_NAME01_SW_RESET [01:01] */
#define BCHP_IDE_GR_SW_RESET_0_YOUR_NAME01_SW_RESET_MASK           0x00000002
#define BCHP_IDE_GR_SW_RESET_0_YOUR_NAME01_SW_RESET_SHIFT          1
#define BCHP_IDE_GR_SW_RESET_0_YOUR_NAME01_SW_RESET_DEASSERT       0
#define BCHP_IDE_GR_SW_RESET_0_YOUR_NAME01_SW_RESET_ASSERT         1

/* IDE_GR :: SW_RESET_0 :: YOUR_NAME00_SW_RESET [00:00] */
#define BCHP_IDE_GR_SW_RESET_0_YOUR_NAME00_SW_RESET_MASK           0x00000001
#define BCHP_IDE_GR_SW_RESET_0_YOUR_NAME00_SW_RESET_SHIFT          0
#define BCHP_IDE_GR_SW_RESET_0_YOUR_NAME00_SW_RESET_DEASSERT       0
#define BCHP_IDE_GR_SW_RESET_0_YOUR_NAME00_SW_RESET_ASSERT         1

/***************************************************************************
 *SW_RESET_1 - GR Bridge Software Reset 1 Register
 ***************************************************************************/
/* IDE_GR :: SW_RESET_1 :: reserved0 [31:03] */
#define BCHP_IDE_GR_SW_RESET_1_reserved0_MASK                      0xfffffff8
#define BCHP_IDE_GR_SW_RESET_1_reserved0_SHIFT                     3

/* IDE_GR :: SW_RESET_1 :: YOUR_NAME02_SW_RESET [02:02] */
#define BCHP_IDE_GR_SW_RESET_1_YOUR_NAME02_SW_RESET_MASK           0x00000004
#define BCHP_IDE_GR_SW_RESET_1_YOUR_NAME02_SW_RESET_SHIFT          2
#define BCHP_IDE_GR_SW_RESET_1_YOUR_NAME02_SW_RESET_DEASSERT       0
#define BCHP_IDE_GR_SW_RESET_1_YOUR_NAME02_SW_RESET_ASSERT         1

/* IDE_GR :: SW_RESET_1 :: YOUR_NAME01_SW_RESET [01:01] */
#define BCHP_IDE_GR_SW_RESET_1_YOUR_NAME01_SW_RESET_MASK           0x00000002
#define BCHP_IDE_GR_SW_RESET_1_YOUR_NAME01_SW_RESET_SHIFT          1
#define BCHP_IDE_GR_SW_RESET_1_YOUR_NAME01_SW_RESET_DEASSERT       0
#define BCHP_IDE_GR_SW_RESET_1_YOUR_NAME01_SW_RESET_ASSERT         1

/* IDE_GR :: SW_RESET_1 :: YOUR_NAME00_SW_RESET [00:00] */
#define BCHP_IDE_GR_SW_RESET_1_YOUR_NAME00_SW_RESET_MASK           0x00000001
#define BCHP_IDE_GR_SW_RESET_1_YOUR_NAME00_SW_RESET_SHIFT          0
#define BCHP_IDE_GR_SW_RESET_1_YOUR_NAME00_SW_RESET_DEASSERT       0
#define BCHP_IDE_GR_SW_RESET_1_YOUR_NAME00_SW_RESET_ASSERT         1

#endif /* #ifndef BCHP_IDE_GR_H__ */

/* End of File */
