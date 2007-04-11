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
 * $brcm_Workfile: bchp_pci_cfg.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/8/06 10:35a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Feb  7 17:42:52 2006
 *                 MD5 Checksum         98ca6e78f720f3a055ccbe1c0496b04b
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.006
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7401/rdb/b0/bchp_pci_cfg.h $
 * 
 * Hydra_Software_Devel/1   2/8/06 10:35a jasonh
 * PR 18593: Initial version.
 *
 ***************************************************************************/

#ifndef BCHP_PCI_CFG_H__
#define BCHP_PCI_CFG_H__

/***************************************************************************
 *PCI_CFG - PCI Configuration Registers
 ***************************************************************************/
#define BCHP_PCI_CFG_DEVICE_VENDOR_ID            0x00000200 /* PCI CFG Device Vendor ID */
#define BCHP_PCI_CFG_STATUS_COMMAND              0x00000204 /* PCI CFG Status Command */
#define BCHP_PCI_CFG_CLASS_CODE_REV_ID           0x00000208 /* PCI CFG Class Code Rev ID */
#define BCHP_PCI_CFG_BIST_LATENCY_CACHELINE      0x0000020c /* PCI CFG BIST Latency Cacheline Size */
#define BCHP_PCI_CFG_GISB_BASE_W                 0x00000210 /* PCI CFG GISB Base Window */
#define BCHP_PCI_CFG_MEMORY_BASE_W0              0x00000214 /* PCI CFG Memory Base Window0 */
#define BCHP_PCI_CFG_MEMORY_BASE_W1              0x00000218 /* PCI CFG Memory Base Window1 */
#define BCHP_PCI_CFG_MEMORY_BASE_W2              0x0000021c /* PCI CFG Memory Base Window2 */
#define BCHP_PCI_CFG_SUBSYSTEM_ID                0x0000022c /* PCI CFG Subsystem ID */
#define BCHP_PCI_CFG_LATENCY_GRANT_INTR          0x0000023c /* Latency Min Grant Interrupt */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN0          0x00000250 /* PCI CFG CPU to PCI MEM WIN0 */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN1          0x00000254 /* PCI CFG CPU to PCI MEM WIN1 */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2          0x00000258 /* PCI CFG CPU to PCI MEM WIN2 */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3          0x0000025c /* PCI CFG CPU to PCI MEM WIN3 */
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0           0x00000260 /* PCI CFG CPU to PCI IO WIN0 */
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1           0x00000264 /* PCI CFG CPU to PCI IO WIN1 */
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2           0x00000268 /* PCI CFG CPU to PCI IO WIN2 */
#define BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL       0x0000026c /* PCI CFG PCI SDRAM Endian Control */

/***************************************************************************
 *DEVICE_VENDOR_ID - PCI CFG Device Vendor ID
 ***************************************************************************/
/* PCI_CFG :: DEVICE_VENDOR_ID :: DEV_ID [31:16] */
#define BCHP_PCI_CFG_DEVICE_VENDOR_ID_DEV_ID_MASK                  0xffff0000
#define BCHP_PCI_CFG_DEVICE_VENDOR_ID_DEV_ID_SHIFT                 16

/* PCI_CFG :: DEVICE_VENDOR_ID :: VEND_ID [15:00] */
#define BCHP_PCI_CFG_DEVICE_VENDOR_ID_VEND_ID_MASK                 0x0000ffff
#define BCHP_PCI_CFG_DEVICE_VENDOR_ID_VEND_ID_SHIFT                0

/***************************************************************************
 *STATUS_COMMAND - PCI CFG Status Command
 ***************************************************************************/
/* PCI_CFG :: STATUS_COMMAND :: DETECTED_PARITY_ERROR [31:31] */
#define BCHP_PCI_CFG_STATUS_COMMAND_DETECTED_PARITY_ERROR_MASK     0x80000000
#define BCHP_PCI_CFG_STATUS_COMMAND_DETECTED_PARITY_ERROR_SHIFT    31
#define BCHP_PCI_CFG_STATUS_COMMAND_DETECTED_PARITY_ERROR_ASSERTED 1
#define BCHP_PCI_CFG_STATUS_COMMAND_DETECTED_PARITY_ERROR_DEASSERTED 0

/* PCI_CFG :: STATUS_COMMAND :: SIGNALED_SYSTEM_ERROR [30:30] */
#define BCHP_PCI_CFG_STATUS_COMMAND_SIGNALED_SYSTEM_ERROR_MASK     0x40000000
#define BCHP_PCI_CFG_STATUS_COMMAND_SIGNALED_SYSTEM_ERROR_SHIFT    30
#define BCHP_PCI_CFG_STATUS_COMMAND_SIGNALED_SYSTEM_ERROR_ASSERTED 1
#define BCHP_PCI_CFG_STATUS_COMMAND_SIGNALED_SYSTEM_ERROR_DEASSERTED 0

/* PCI_CFG :: STATUS_COMMAND :: RECEIVED_MASTER_ABORT [29:29] */
#define BCHP_PCI_CFG_STATUS_COMMAND_RECEIVED_MASTER_ABORT_MASK     0x20000000
#define BCHP_PCI_CFG_STATUS_COMMAND_RECEIVED_MASTER_ABORT_SHIFT    29
#define BCHP_PCI_CFG_STATUS_COMMAND_RECEIVED_MASTER_ABORT_ASSERTED 1
#define BCHP_PCI_CFG_STATUS_COMMAND_RECEIVED_MASTER_ABORT_DEASSERTED 0

/* PCI_CFG :: STATUS_COMMAND :: RECEIVED_TARGET_ABORT [28:28] */
#define BCHP_PCI_CFG_STATUS_COMMAND_RECEIVED_TARGET_ABORT_MASK     0x10000000
#define BCHP_PCI_CFG_STATUS_COMMAND_RECEIVED_TARGET_ABORT_SHIFT    28
#define BCHP_PCI_CFG_STATUS_COMMAND_RECEIVED_TARGET_ABORT_ASSERTED 1
#define BCHP_PCI_CFG_STATUS_COMMAND_RECEIVED_TARGET_ABORT_DEASSERTED 0

/* PCI_CFG :: STATUS_COMMAND :: SIGNALED_TARGET_ABORT [27:27] */
#define BCHP_PCI_CFG_STATUS_COMMAND_SIGNALED_TARGET_ABORT_MASK     0x08000000
#define BCHP_PCI_CFG_STATUS_COMMAND_SIGNALED_TARGET_ABORT_SHIFT    27
#define BCHP_PCI_CFG_STATUS_COMMAND_SIGNALED_TARGET_ABORT_ASSERTED 1
#define BCHP_PCI_CFG_STATUS_COMMAND_SIGNALED_TARGET_ABORT_DEASSERTED 0

/* PCI_CFG :: STATUS_COMMAND :: DEVICE_SELECT_TIMING [26:25] */
#define BCHP_PCI_CFG_STATUS_COMMAND_DEVICE_SELECT_TIMING_MASK      0x06000000
#define BCHP_PCI_CFG_STATUS_COMMAND_DEVICE_SELECT_TIMING_SHIFT     25

/* PCI_CFG :: STATUS_COMMAND :: MASTER_DATA_PARITY_ERROR [24:24] */
#define BCHP_PCI_CFG_STATUS_COMMAND_MASTER_DATA_PARITY_ERROR_MASK  0x01000000
#define BCHP_PCI_CFG_STATUS_COMMAND_MASTER_DATA_PARITY_ERROR_SHIFT 24
#define BCHP_PCI_CFG_STATUS_COMMAND_MASTER_DATA_PARITY_ERROR_ASSERTED 1
#define BCHP_PCI_CFG_STATUS_COMMAND_MASTER_DATA_PARITY_ERROR_DEASSERTED 0

/* PCI_CFG :: STATUS_COMMAND :: FAST_BACK_TO_BACK_CAPABLE [23:23] */
#define BCHP_PCI_CFG_STATUS_COMMAND_FAST_BACK_TO_BACK_CAPABLE_MASK 0x00800000
#define BCHP_PCI_CFG_STATUS_COMMAND_FAST_BACK_TO_BACK_CAPABLE_SHIFT 23
#define BCHP_PCI_CFG_STATUS_COMMAND_FAST_BACK_TO_BACK_CAPABLE_ASSERTED 1
#define BCHP_PCI_CFG_STATUS_COMMAND_FAST_BACK_TO_BACK_CAPABLE_DEASSERTED 0

/* PCI_CFG :: STATUS_COMMAND :: reserved0 [22:22] */
#define BCHP_PCI_CFG_STATUS_COMMAND_reserved0_MASK                 0x00400000
#define BCHP_PCI_CFG_STATUS_COMMAND_reserved0_SHIFT                22

/* PCI_CFG :: STATUS_COMMAND :: CAPABLE_66MHZ [21:21] */
#define BCHP_PCI_CFG_STATUS_COMMAND_CAPABLE_66MHZ_MASK             0x00200000
#define BCHP_PCI_CFG_STATUS_COMMAND_CAPABLE_66MHZ_SHIFT            21
#define BCHP_PCI_CFG_STATUS_COMMAND_CAPABLE_66MHZ_ASSERTED         1
#define BCHP_PCI_CFG_STATUS_COMMAND_CAPABLE_66MHZ_DEASSERTED       0

/* PCI_CFG :: STATUS_COMMAND :: reserved1 [20:10] */
#define BCHP_PCI_CFG_STATUS_COMMAND_reserved1_MASK                 0x001ffc00
#define BCHP_PCI_CFG_STATUS_COMMAND_reserved1_SHIFT                10

/* PCI_CFG :: STATUS_COMMAND :: FAST_BACK_TO_BACK_ENABLE [09:09] */
#define BCHP_PCI_CFG_STATUS_COMMAND_FAST_BACK_TO_BACK_ENABLE_MASK  0x00000200
#define BCHP_PCI_CFG_STATUS_COMMAND_FAST_BACK_TO_BACK_ENABLE_SHIFT 9
#define BCHP_PCI_CFG_STATUS_COMMAND_FAST_BACK_TO_BACK_ENABLE_ASSERTED 1
#define BCHP_PCI_CFG_STATUS_COMMAND_FAST_BACK_TO_BACK_ENABLE_DEASSERTED 0

/* PCI_CFG :: STATUS_COMMAND :: SERR_ENABLE [08:08] */
#define BCHP_PCI_CFG_STATUS_COMMAND_SERR_ENABLE_MASK               0x00000100
#define BCHP_PCI_CFG_STATUS_COMMAND_SERR_ENABLE_SHIFT              8
#define BCHP_PCI_CFG_STATUS_COMMAND_SERR_ENABLE_ASSERTED           1
#define BCHP_PCI_CFG_STATUS_COMMAND_SERR_ENABLE_DEASSERTED         0

/* PCI_CFG :: STATUS_COMMAND :: STEPPING_CONTROL [07:07] */
#define BCHP_PCI_CFG_STATUS_COMMAND_STEPPING_CONTROL_MASK          0x00000080
#define BCHP_PCI_CFG_STATUS_COMMAND_STEPPING_CONTROL_SHIFT         7
#define BCHP_PCI_CFG_STATUS_COMMAND_STEPPING_CONTROL_ASSERTED      1
#define BCHP_PCI_CFG_STATUS_COMMAND_STEPPING_CONTROL_DEASSERTED    0

/* PCI_CFG :: STATUS_COMMAND :: PARITY_ERROR_RESPONSE [06:06] */
#define BCHP_PCI_CFG_STATUS_COMMAND_PARITY_ERROR_RESPONSE_MASK     0x00000040
#define BCHP_PCI_CFG_STATUS_COMMAND_PARITY_ERROR_RESPONSE_SHIFT    6
#define BCHP_PCI_CFG_STATUS_COMMAND_PARITY_ERROR_RESPONSE_ASSERTED 1
#define BCHP_PCI_CFG_STATUS_COMMAND_PARITY_ERROR_RESPONSE_DEASSERTED 0

/* PCI_CFG :: STATUS_COMMAND :: VGA_PALETTE_SNOOP [05:05] */
#define BCHP_PCI_CFG_STATUS_COMMAND_VGA_PALETTE_SNOOP_MASK         0x00000020
#define BCHP_PCI_CFG_STATUS_COMMAND_VGA_PALETTE_SNOOP_SHIFT        5
#define BCHP_PCI_CFG_STATUS_COMMAND_VGA_PALETTE_SNOOP_ASSERTED     1
#define BCHP_PCI_CFG_STATUS_COMMAND_VGA_PALETTE_SNOOP_DEASSERTED   0

/* PCI_CFG :: STATUS_COMMAND :: MEM_WRITE_INV_ENABLE [04:04] */
#define BCHP_PCI_CFG_STATUS_COMMAND_MEM_WRITE_INV_ENABLE_MASK      0x00000010
#define BCHP_PCI_CFG_STATUS_COMMAND_MEM_WRITE_INV_ENABLE_SHIFT     4
#define BCHP_PCI_CFG_STATUS_COMMAND_MEM_WRITE_INV_ENABLE_ASSERTED  1
#define BCHP_PCI_CFG_STATUS_COMMAND_MEM_WRITE_INV_ENABLE_DEASSERTED 0

/* PCI_CFG :: STATUS_COMMAND :: SPECIAL_CYCLES [03:03] */
#define BCHP_PCI_CFG_STATUS_COMMAND_SPECIAL_CYCLES_MASK            0x00000008
#define BCHP_PCI_CFG_STATUS_COMMAND_SPECIAL_CYCLES_SHIFT           3
#define BCHP_PCI_CFG_STATUS_COMMAND_SPECIAL_CYCLES_ASSERTED        1
#define BCHP_PCI_CFG_STATUS_COMMAND_SPECIAL_CYCLES_DEASSERTED      0

/* PCI_CFG :: STATUS_COMMAND :: BUS_MASTER [02:02] */
#define BCHP_PCI_CFG_STATUS_COMMAND_BUS_MASTER_MASK                0x00000004
#define BCHP_PCI_CFG_STATUS_COMMAND_BUS_MASTER_SHIFT               2
#define BCHP_PCI_CFG_STATUS_COMMAND_BUS_MASTER_ASSERTED            1
#define BCHP_PCI_CFG_STATUS_COMMAND_BUS_MASTER_DEASSERTED          0

/* PCI_CFG :: STATUS_COMMAND :: MEMORY_SPACE [01:01] */
#define BCHP_PCI_CFG_STATUS_COMMAND_MEMORY_SPACE_MASK              0x00000002
#define BCHP_PCI_CFG_STATUS_COMMAND_MEMORY_SPACE_SHIFT             1
#define BCHP_PCI_CFG_STATUS_COMMAND_MEMORY_SPACE_ASSERTED          1
#define BCHP_PCI_CFG_STATUS_COMMAND_MEMORY_SPACE_DEASSERTED        0

/* PCI_CFG :: STATUS_COMMAND :: IO_SPACE [00:00] */
#define BCHP_PCI_CFG_STATUS_COMMAND_IO_SPACE_MASK                  0x00000001
#define BCHP_PCI_CFG_STATUS_COMMAND_IO_SPACE_SHIFT                 0
#define BCHP_PCI_CFG_STATUS_COMMAND_IO_SPACE_ASSERTED              1
#define BCHP_PCI_CFG_STATUS_COMMAND_IO_SPACE_DEASSERTED            0

/***************************************************************************
 *CLASS_CODE_REV_ID - PCI CFG Class Code Rev ID
 ***************************************************************************/
/* PCI_CFG :: CLASS_CODE_REV_ID :: CLASS_CODE [31:08] */
#define BCHP_PCI_CFG_CLASS_CODE_REV_ID_CLASS_CODE_MASK             0xffffff00
#define BCHP_PCI_CFG_CLASS_CODE_REV_ID_CLASS_CODE_SHIFT            8

/* PCI_CFG :: CLASS_CODE_REV_ID :: REV_ID [07:00] */
#define BCHP_PCI_CFG_CLASS_CODE_REV_ID_REV_ID_MASK                 0x000000ff
#define BCHP_PCI_CFG_CLASS_CODE_REV_ID_REV_ID_SHIFT                0

/***************************************************************************
 *BIST_LATENCY_CACHELINE - PCI CFG BIST Latency Cacheline Size
 ***************************************************************************/
/* PCI_CFG :: BIST_LATENCY_CACHELINE :: BIST [31:24] */
#define BCHP_PCI_CFG_BIST_LATENCY_CACHELINE_BIST_MASK              0xff000000
#define BCHP_PCI_CFG_BIST_LATENCY_CACHELINE_BIST_SHIFT             24

/* PCI_CFG :: BIST_LATENCY_CACHELINE :: HEADER_TYPE [23:16] */
#define BCHP_PCI_CFG_BIST_LATENCY_CACHELINE_HEADER_TYPE_MASK       0x00ff0000
#define BCHP_PCI_CFG_BIST_LATENCY_CACHELINE_HEADER_TYPE_SHIFT      16

/* PCI_CFG :: BIST_LATENCY_CACHELINE :: LATENCY_TMR [15:08] */
#define BCHP_PCI_CFG_BIST_LATENCY_CACHELINE_LATENCY_TMR_MASK       0x0000ff00
#define BCHP_PCI_CFG_BIST_LATENCY_CACHELINE_LATENCY_TMR_SHIFT      8

/* PCI_CFG :: BIST_LATENCY_CACHELINE :: CACHELINE_SIZE [07:00] */
#define BCHP_PCI_CFG_BIST_LATENCY_CACHELINE_CACHELINE_SIZE_MASK    0x000000ff
#define BCHP_PCI_CFG_BIST_LATENCY_CACHELINE_CACHELINE_SIZE_SHIFT   0

/***************************************************************************
 *GISB_BASE_W - PCI CFG GISB Base Window
 ***************************************************************************/
/* PCI_CFG :: GISB_BASE_W :: BASE_ADDR [31:25] */
#define BCHP_PCI_CFG_GISB_BASE_W_BASE_ADDR_MASK                    0xfe000000
#define BCHP_PCI_CFG_GISB_BASE_W_BASE_ADDR_SHIFT                   25

/* PCI_CFG :: GISB_BASE_W :: reserved0 [24:00] */
#define BCHP_PCI_CFG_GISB_BASE_W_reserved0_MASK                    0x01ffffff
#define BCHP_PCI_CFG_GISB_BASE_W_reserved0_SHIFT                   0

/***************************************************************************
 *MEMORY_BASE_W0 - PCI CFG Memory Base Window0
 ***************************************************************************/
/* PCI_CFG :: MEMORY_BASE_W0 :: BASE_ADDR [31:25] */
#define BCHP_PCI_CFG_MEMORY_BASE_W0_BASE_ADDR_MASK                 0xfe000000
#define BCHP_PCI_CFG_MEMORY_BASE_W0_BASE_ADDR_SHIFT                25

/* PCI_CFG :: MEMORY_BASE_W0 :: reserved0 [24:04] */
#define BCHP_PCI_CFG_MEMORY_BASE_W0_reserved0_MASK                 0x01fffff0
#define BCHP_PCI_CFG_MEMORY_BASE_W0_reserved0_SHIFT                4

/* PCI_CFG :: MEMORY_BASE_W0 :: PREFETCH [03:03] */
#define BCHP_PCI_CFG_MEMORY_BASE_W0_PREFETCH_MASK                  0x00000008
#define BCHP_PCI_CFG_MEMORY_BASE_W0_PREFETCH_SHIFT                 3

/* PCI_CFG :: MEMORY_BASE_W0 :: reserved1 [02:00] */
#define BCHP_PCI_CFG_MEMORY_BASE_W0_reserved1_MASK                 0x00000007
#define BCHP_PCI_CFG_MEMORY_BASE_W0_reserved1_SHIFT                0

/***************************************************************************
 *MEMORY_BASE_W1 - PCI CFG Memory Base Window1
 ***************************************************************************/
/* PCI_CFG :: MEMORY_BASE_W1 :: BASE_ADDR [31:25] */
#define BCHP_PCI_CFG_MEMORY_BASE_W1_BASE_ADDR_MASK                 0xfe000000
#define BCHP_PCI_CFG_MEMORY_BASE_W1_BASE_ADDR_SHIFT                25

/* PCI_CFG :: MEMORY_BASE_W1 :: reserved0 [24:04] */
#define BCHP_PCI_CFG_MEMORY_BASE_W1_reserved0_MASK                 0x01fffff0
#define BCHP_PCI_CFG_MEMORY_BASE_W1_reserved0_SHIFT                4

/* PCI_CFG :: MEMORY_BASE_W1 :: PREFETCH [03:03] */
#define BCHP_PCI_CFG_MEMORY_BASE_W1_PREFETCH_MASK                  0x00000008
#define BCHP_PCI_CFG_MEMORY_BASE_W1_PREFETCH_SHIFT                 3

/* PCI_CFG :: MEMORY_BASE_W1 :: reserved1 [02:00] */
#define BCHP_PCI_CFG_MEMORY_BASE_W1_reserved1_MASK                 0x00000007
#define BCHP_PCI_CFG_MEMORY_BASE_W1_reserved1_SHIFT                0

/***************************************************************************
 *MEMORY_BASE_W2 - PCI CFG Memory Base Window2
 ***************************************************************************/
/* PCI_CFG :: MEMORY_BASE_W2 :: BASE_ADDR [31:25] */
#define BCHP_PCI_CFG_MEMORY_BASE_W2_BASE_ADDR_MASK                 0xfe000000
#define BCHP_PCI_CFG_MEMORY_BASE_W2_BASE_ADDR_SHIFT                25

/* PCI_CFG :: MEMORY_BASE_W2 :: reserved0 [24:04] */
#define BCHP_PCI_CFG_MEMORY_BASE_W2_reserved0_MASK                 0x01fffff0
#define BCHP_PCI_CFG_MEMORY_BASE_W2_reserved0_SHIFT                4

/* PCI_CFG :: MEMORY_BASE_W2 :: PREFETCH [03:03] */
#define BCHP_PCI_CFG_MEMORY_BASE_W2_PREFETCH_MASK                  0x00000008
#define BCHP_PCI_CFG_MEMORY_BASE_W2_PREFETCH_SHIFT                 3

/* PCI_CFG :: MEMORY_BASE_W2 :: reserved1 [02:00] */
#define BCHP_PCI_CFG_MEMORY_BASE_W2_reserved1_MASK                 0x00000007
#define BCHP_PCI_CFG_MEMORY_BASE_W2_reserved1_SHIFT                0

/***************************************************************************
 *SUBSYSTEM_ID - PCI CFG Subsystem ID
 ***************************************************************************/
/* PCI_CFG :: SUBSYSTEM_ID :: SUBSYSTEM_ID [31:16] */
#define BCHP_PCI_CFG_SUBSYSTEM_ID_SUBSYSTEM_ID_MASK                0xffff0000
#define BCHP_PCI_CFG_SUBSYSTEM_ID_SUBSYSTEM_ID_SHIFT               16

/* PCI_CFG :: SUBSYSTEM_ID :: SUBVENDOR_ID [15:00] */
#define BCHP_PCI_CFG_SUBSYSTEM_ID_SUBVENDOR_ID_MASK                0x0000ffff
#define BCHP_PCI_CFG_SUBSYSTEM_ID_SUBVENDOR_ID_SHIFT               0

/***************************************************************************
 *LATENCY_GRANT_INTR - Latency Min Grant Interrupt
 ***************************************************************************/
/* PCI_CFG :: LATENCY_GRANT_INTR :: MAX_LATENCY [31:24] */
#define BCHP_PCI_CFG_LATENCY_GRANT_INTR_MAX_LATENCY_MASK           0xff000000
#define BCHP_PCI_CFG_LATENCY_GRANT_INTR_MAX_LATENCY_SHIFT          24

/* PCI_CFG :: LATENCY_GRANT_INTR :: MIN_GRANT [23:16] */
#define BCHP_PCI_CFG_LATENCY_GRANT_INTR_MIN_GRANT_MASK             0x00ff0000
#define BCHP_PCI_CFG_LATENCY_GRANT_INTR_MIN_GRANT_SHIFT            16

/* PCI_CFG :: LATENCY_GRANT_INTR :: INTR_PIN [15:08] */
#define BCHP_PCI_CFG_LATENCY_GRANT_INTR_INTR_PIN_MASK              0x0000ff00
#define BCHP_PCI_CFG_LATENCY_GRANT_INTR_INTR_PIN_SHIFT             8

/* PCI_CFG :: LATENCY_GRANT_INTR :: INTR_LINE_REG [07:00] */
#define BCHP_PCI_CFG_LATENCY_GRANT_INTR_INTR_LINE_REG_MASK         0x000000ff
#define BCHP_PCI_CFG_LATENCY_GRANT_INTR_INTR_LINE_REG_SHIFT        0

/***************************************************************************
 *CPU_2_PCI_MEM_WIN0 - PCI CFG CPU to PCI MEM WIN0
 ***************************************************************************/
/* PCI_CFG :: CPU_2_PCI_MEM_WIN0 :: BASE_ADRS [31:27] */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN0_BASE_ADRS_MASK             0xf8000000
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN0_BASE_ADRS_SHIFT            27

/* PCI_CFG :: CPU_2_PCI_MEM_WIN0 :: reserved0 [26:02] */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN0_reserved0_MASK             0x07fffffc
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN0_reserved0_SHIFT            2

/* PCI_CFG :: CPU_2_PCI_MEM_WIN0 :: ENDIAN_MODE [01:00] */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN0_ENDIAN_MODE_MASK           0x00000003
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN0_ENDIAN_MODE_SHIFT          0

/***************************************************************************
 *CPU_2_PCI_MEM_WIN1 - PCI CFG CPU to PCI MEM WIN1
 ***************************************************************************/
/* PCI_CFG :: CPU_2_PCI_MEM_WIN1 :: BASE_ADRS [31:27] */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN1_BASE_ADRS_MASK             0xf8000000
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN1_BASE_ADRS_SHIFT            27

/* PCI_CFG :: CPU_2_PCI_MEM_WIN1 :: reserved0 [26:02] */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN1_reserved0_MASK             0x07fffffc
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN1_reserved0_SHIFT            2

/* PCI_CFG :: CPU_2_PCI_MEM_WIN1 :: ENDIAN_MODE [01:00] */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN1_ENDIAN_MODE_MASK           0x00000003
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN1_ENDIAN_MODE_SHIFT          0

/***************************************************************************
 *CPU_2_PCI_MEM_WIN2 - PCI CFG CPU to PCI MEM WIN2
 ***************************************************************************/
/* PCI_CFG :: CPU_2_PCI_MEM_WIN2 :: BASE_ADRS [31:27] */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2_BASE_ADRS_MASK             0xf8000000
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2_BASE_ADRS_SHIFT            27

/* PCI_CFG :: CPU_2_PCI_MEM_WIN2 :: reserved0 [26:02] */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2_reserved0_MASK             0x07fffffc
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2_reserved0_SHIFT            2

/* PCI_CFG :: CPU_2_PCI_MEM_WIN2 :: ENDIAN_MODE [01:00] */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2_ENDIAN_MODE_MASK           0x00000003
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2_ENDIAN_MODE_SHIFT          0

/***************************************************************************
 *CPU_2_PCI_MEM_WIN3 - PCI CFG CPU to PCI MEM WIN3
 ***************************************************************************/
/* PCI_CFG :: CPU_2_PCI_MEM_WIN3 :: BASE_ADRS [31:27] */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3_BASE_ADRS_MASK             0xf8000000
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3_BASE_ADRS_SHIFT            27

/* PCI_CFG :: CPU_2_PCI_MEM_WIN3 :: reserved0 [26:02] */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3_reserved0_MASK             0x07fffffc
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3_reserved0_SHIFT            2

/* PCI_CFG :: CPU_2_PCI_MEM_WIN3 :: ENDIAN_MODE [01:00] */
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3_ENDIAN_MODE_MASK           0x00000003
#define BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3_ENDIAN_MODE_SHIFT          0

/***************************************************************************
 *CPU_2_PCI_IO_WIN0 - PCI CFG CPU to PCI IO WIN0
 ***************************************************************************/
/* PCI_CFG :: CPU_2_PCI_IO_WIN0 :: BASE_ADRS [31:21] */
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0_BASE_ADRS_MASK              0xffe00000
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0_BASE_ADRS_SHIFT             21

/* PCI_CFG :: CPU_2_PCI_IO_WIN0 :: reserved0 [20:02] */
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0_reserved0_MASK              0x001ffffc
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0_reserved0_SHIFT             2

/* PCI_CFG :: CPU_2_PCI_IO_WIN0 :: ENDIAN_MODE [01:00] */
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0_ENDIAN_MODE_MASK            0x00000003
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0_ENDIAN_MODE_SHIFT           0

/***************************************************************************
 *CPU_2_PCI_IO_WIN1 - PCI CFG CPU to PCI IO WIN1
 ***************************************************************************/
/* PCI_CFG :: CPU_2_PCI_IO_WIN1 :: BASE_ADRS [31:21] */
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1_BASE_ADRS_MASK              0xffe00000
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1_BASE_ADRS_SHIFT             21

/* PCI_CFG :: CPU_2_PCI_IO_WIN1 :: reserved0 [20:02] */
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1_reserved0_MASK              0x001ffffc
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1_reserved0_SHIFT             2

/* PCI_CFG :: CPU_2_PCI_IO_WIN1 :: ENDIAN_MODE [01:00] */
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1_ENDIAN_MODE_MASK            0x00000003
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1_ENDIAN_MODE_SHIFT           0

/***************************************************************************
 *CPU_2_PCI_IO_WIN2 - PCI CFG CPU to PCI IO WIN2
 ***************************************************************************/
/* PCI_CFG :: CPU_2_PCI_IO_WIN2 :: BASE_ADRS [31:21] */
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2_BASE_ADRS_MASK              0xffe00000
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2_BASE_ADRS_SHIFT             21

/* PCI_CFG :: CPU_2_PCI_IO_WIN2 :: reserved0 [20:02] */
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2_reserved0_MASK              0x001ffffc
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2_reserved0_SHIFT             2

/* PCI_CFG :: CPU_2_PCI_IO_WIN2 :: ENDIAN_MODE [01:00] */
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2_ENDIAN_MODE_MASK            0x00000003
#define BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2_ENDIAN_MODE_SHIFT           0

/***************************************************************************
 *PCI_SDRAM_ENDIAN_CTRL - PCI CFG PCI SDRAM Endian Control
 ***************************************************************************/
/* PCI_CFG :: PCI_SDRAM_ENDIAN_CTRL :: reserved0 [31:08] */
#define BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_reserved0_MASK          0xffffff00
#define BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_reserved0_SHIFT         8

/* PCI_CFG :: PCI_SDRAM_ENDIAN_CTRL :: ENDIAN_MODE_GWIN [07:06] */
#define BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_GWIN_MASK   0x000000c0
#define BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_GWIN_SHIFT  6

/* PCI_CFG :: PCI_SDRAM_ENDIAN_CTRL :: ENDIAN_MODE_MWIN2 [05:04] */
#define BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN2_MASK  0x00000030
#define BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN2_SHIFT 4

/* PCI_CFG :: PCI_SDRAM_ENDIAN_CTRL :: ENDIAN_MODE_MWIN1 [03:02] */
#define BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN1_MASK  0x0000000c
#define BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN1_SHIFT 2

/* PCI_CFG :: PCI_SDRAM_ENDIAN_CTRL :: ENDIAN_MODE_MWIN0 [01:00] */
#define BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_MASK  0x00000003
#define BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_SHIFT 0

#endif /* #ifndef BCHP_PCI_CFG_H__ */

/* End of File */
