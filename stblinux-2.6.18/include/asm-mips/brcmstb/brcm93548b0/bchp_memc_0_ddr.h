/***************************************************************************
 *     Copyright (c) 1999-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_memc_0_ddr.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/17/08 8:04p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Jul 17 17:19:55 2008
 *                 MD5 Checksum         9220767de76c9afe7a462b5b55e5d2ff
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/3548/rdb/b0/bchp_memc_0_ddr.h $
 * 
 * Hydra_Software_Devel/1   7/17/08 8:04p maivu
 * PR 44921: Initial B0 files
 *
 ***************************************************************************/

#ifndef BCHP_MEMC_0_DDR_H__
#define BCHP_MEMC_0_DDR_H__

/***************************************************************************
 *MEMC_0_DDR - 64-bit Memory Controller DDR IOBUF Registers
 ***************************************************************************/
#define BCHP_MEMC_0_DDR_CNTRLR_CONFIG            0x00162000 /* Memory Controller Mode-Configuration Register. */
#define BCHP_MEMC_0_DDR_DRAM_MODE                0x00162004 /* DDR Mode Register. */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0            0x00162008 /* DDR-SDRAM Timing Register. */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_1            0x0016200c /* DDR-SDRAM Timing Register. */
#define BCHP_MEMC_0_DDR_READ_WRITE_TIMING        0x00162010 /* Read to Write & write to read timing register */
#define BCHP_MEMC_0_DDR_POWER_DOWN_MODE          0x00162014 /* Memory Controller Auto-Refresh Power Down Control Register. */
#define BCHP_MEMC_0_DDR_CNTRLR_START_SEQ         0x00162018 /* Memory Controller Sequencer Enable */
#define BCHP_MEMC_0_DDR_CNTRLR_SM_TIMEOUT        0x0016201c /* Memory Controller , state machine timeout register. */
#define BCHP_MEMC_0_DDR_POWER_DOWN_STATUS        0x00162020 /* Memory Controller , status register. */
#define BCHP_MEMC_0_DDR_BANK_STATUS              0x00162024 /* Memory Controller, Bank Status Register. */
#define BCHP_MEMC_0_DDR_WCL_FIFO_STATUS          0x00162028 /* Memory Controller, Write latency comp fifo Status Register. */

/***************************************************************************
 *CNTRLR_CONFIG - Memory Controller Mode-Configuration Register.
 ***************************************************************************/
/* MEMC_0_DDR :: CNTRLR_CONFIG :: reserved0 [31:02] */
#define BCHP_MEMC_0_DDR_CNTRLR_CONFIG_reserved0_MASK               0xfffffffc
#define BCHP_MEMC_0_DDR_CNTRLR_CONFIG_reserved0_SHIFT              2

/* MEMC_0_DDR :: CNTRLR_CONFIG :: DEVICE_TECH [01:00] */
#define BCHP_MEMC_0_DDR_CNTRLR_CONFIG_DEVICE_TECH_MASK             0x00000003
#define BCHP_MEMC_0_DDR_CNTRLR_CONFIG_DEVICE_TECH_SHIFT            0

/***************************************************************************
 *DRAM_MODE - DDR Mode Register.
 ***************************************************************************/
/* MEMC_0_DDR :: DRAM_MODE :: reserved0 [31:24] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_reserved0_MASK                   0xff000000
#define BCHP_MEMC_0_DDR_DRAM_MODE_reserved0_SHIFT                  24

/* MEMC_0_DDR :: DRAM_MODE :: QOFF [23:23] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_QOFF_MASK                        0x00800000
#define BCHP_MEMC_0_DDR_DRAM_MODE_QOFF_SHIFT                       23

/* MEMC_0_DDR :: DRAM_MODE :: RDQS [22:22] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_RDQS_MASK                        0x00400000
#define BCHP_MEMC_0_DDR_DRAM_MODE_RDQS_SHIFT                       22

/* MEMC_0_DDR :: DRAM_MODE :: DQS_N [21:21] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_DQS_N_MASK                       0x00200000
#define BCHP_MEMC_0_DDR_DRAM_MODE_DQS_N_SHIFT                      21

/* MEMC_0_DDR :: DRAM_MODE :: PD [20:20] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_PD_MASK                          0x00100000
#define BCHP_MEMC_0_DDR_DRAM_MODE_PD_SHIFT                         20

/* MEMC_0_DDR :: DRAM_MODE :: reserved1 [19:18] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_reserved1_MASK                   0x000c0000
#define BCHP_MEMC_0_DDR_DRAM_MODE_reserved1_SHIFT                  18

/* MEMC_0_DDR :: DRAM_MODE :: ODT [17:16] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_ODT_MASK                         0x00030000
#define BCHP_MEMC_0_DDR_DRAM_MODE_ODT_SHIFT                        16

/* MEMC_0_DDR :: DRAM_MODE :: reserved2 [15:15] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_reserved2_MASK                   0x00008000
#define BCHP_MEMC_0_DDR_DRAM_MODE_reserved2_SHIFT                  15

/* MEMC_0_DDR :: DRAM_MODE :: AL [14:12] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_AL_MASK                          0x00007000
#define BCHP_MEMC_0_DDR_DRAM_MODE_AL_SHIFT                         12

/* MEMC_0_DDR :: DRAM_MODE :: reserved3 [11:11] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_reserved3_MASK                   0x00000800
#define BCHP_MEMC_0_DDR_DRAM_MODE_reserved3_SHIFT                  11

/* MEMC_0_DDR :: DRAM_MODE :: WR [10:08] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_WR_MASK                          0x00000700
#define BCHP_MEMC_0_DDR_DRAM_MODE_WR_SHIFT                         8

/* MEMC_0_DDR :: DRAM_MODE :: reserved4 [07:06] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_reserved4_MASK                   0x000000c0
#define BCHP_MEMC_0_DDR_DRAM_MODE_reserved4_SHIFT                  6

/* MEMC_0_DDR :: DRAM_MODE :: DS [05:05] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_DS_MASK                          0x00000020
#define BCHP_MEMC_0_DDR_DRAM_MODE_DS_SHIFT                         5

/* MEMC_0_DDR :: DRAM_MODE :: DLL [04:04] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_DLL_MASK                         0x00000010
#define BCHP_MEMC_0_DDR_DRAM_MODE_DLL_SHIFT                        4

/* MEMC_0_DDR :: DRAM_MODE :: reserved5 [03:03] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_reserved5_MASK                   0x00000008
#define BCHP_MEMC_0_DDR_DRAM_MODE_reserved5_SHIFT                  3

/* MEMC_0_DDR :: DRAM_MODE :: CL [02:00] */
#define BCHP_MEMC_0_DDR_DRAM_MODE_CL_MASK                          0x00000007
#define BCHP_MEMC_0_DDR_DRAM_MODE_CL_SHIFT                         0

/***************************************************************************
 *DRAM_TIMING_0 - DDR-SDRAM Timing Register.
 ***************************************************************************/
/* MEMC_0_DDR :: DRAM_TIMING_0 :: reserved0 [31:31] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_reserved0_MASK               0x80000000
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_reserved0_SHIFT              31

/* MEMC_0_DDR :: DRAM_TIMING_0 :: TRRD_NOP [30:28] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_TRRD_NOP_MASK                0x70000000
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_TRRD_NOP_SHIFT               28

/* MEMC_0_DDR :: DRAM_TIMING_0 :: reserved1 [27:27] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_reserved1_MASK               0x08000000
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_reserved1_SHIFT              27

/* MEMC_0_DDR :: DRAM_TIMING_0 :: TMRD_NOP [26:24] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_TMRD_NOP_MASK                0x07000000
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_TMRD_NOP_SHIFT               24

/* MEMC_0_DDR :: DRAM_TIMING_0 :: reserved2 [23:23] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_reserved2_MASK               0x00800000
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_reserved2_SHIFT              23

/* MEMC_0_DDR :: DRAM_TIMING_0 :: TRCD_NOP [22:20] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_TRCD_NOP_MASK                0x00700000
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_TRCD_NOP_SHIFT               20

/* MEMC_0_DDR :: DRAM_TIMING_0 :: TRP_NOP [19:17] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_TRP_NOP_MASK                 0x000e0000
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_TRP_NOP_SHIFT                17

/* MEMC_0_DDR :: DRAM_TIMING_0 :: TWR_NOP [16:13] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_TWR_NOP_MASK                 0x0001e000
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_TWR_NOP_SHIFT                13

/* MEMC_0_DDR :: DRAM_TIMING_0 :: TRAS_NOP [12:08] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_TRAS_NOP_MASK                0x00001f00
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_TRAS_NOP_SHIFT               8

/* MEMC_0_DDR :: DRAM_TIMING_0 :: reserved3 [07:07] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_reserved3_MASK               0x00000080
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_reserved3_SHIFT              7

/* MEMC_0_DDR :: DRAM_TIMING_0 :: TRFC_NOP [06:00] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_TRFC_NOP_MASK                0x0000007f
#define BCHP_MEMC_0_DDR_DRAM_TIMING_0_TRFC_NOP_SHIFT               0

/***************************************************************************
 *DRAM_TIMING_1 - DDR-SDRAM Timing Register.
 ***************************************************************************/
/* MEMC_0_DDR :: DRAM_TIMING_1 :: reserved0 [31:09] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_1_reserved0_MASK               0xfffffe00
#define BCHP_MEMC_0_DDR_DRAM_TIMING_1_reserved0_SHIFT              9

/* MEMC_0_DDR :: DRAM_TIMING_1 :: TFAW_NOP [08:04] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_1_TFAW_NOP_MASK                0x000001f0
#define BCHP_MEMC_0_DDR_DRAM_TIMING_1_TFAW_NOP_SHIFT               4

/* MEMC_0_DDR :: DRAM_TIMING_1 :: reserved1 [03:03] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_1_reserved1_MASK               0x00000008
#define BCHP_MEMC_0_DDR_DRAM_TIMING_1_reserved1_SHIFT              3

/* MEMC_0_DDR :: DRAM_TIMING_1 :: TRTP_NOP [02:00] */
#define BCHP_MEMC_0_DDR_DRAM_TIMING_1_TRTP_NOP_MASK                0x00000007
#define BCHP_MEMC_0_DDR_DRAM_TIMING_1_TRTP_NOP_SHIFT               0

/***************************************************************************
 *READ_WRITE_TIMING - Read to Write & write to read timing register
 ***************************************************************************/
/* MEMC_0_DDR :: READ_WRITE_TIMING :: reserved0 [31:08] */
#define BCHP_MEMC_0_DDR_READ_WRITE_TIMING_reserved0_MASK           0xffffff00
#define BCHP_MEMC_0_DDR_READ_WRITE_TIMING_reserved0_SHIFT          8

/* MEMC_0_DDR :: READ_WRITE_TIMING :: WR2RD_NOP [07:04] */
#define BCHP_MEMC_0_DDR_READ_WRITE_TIMING_WR2RD_NOP_MASK           0x000000f0
#define BCHP_MEMC_0_DDR_READ_WRITE_TIMING_WR2RD_NOP_SHIFT          4

/* MEMC_0_DDR :: READ_WRITE_TIMING :: RD2WR_NOP [03:00] */
#define BCHP_MEMC_0_DDR_READ_WRITE_TIMING_RD2WR_NOP_MASK           0x0000000f
#define BCHP_MEMC_0_DDR_READ_WRITE_TIMING_RD2WR_NOP_SHIFT          0

/***************************************************************************
 *POWER_DOWN_MODE - Memory Controller Auto-Refresh Power Down Control Register.
 ***************************************************************************/
/* MEMC_0_DDR :: POWER_DOWN_MODE :: reserved0 [31:14] */
#define BCHP_MEMC_0_DDR_POWER_DOWN_MODE_reserved0_MASK             0xffffc000
#define BCHP_MEMC_0_DDR_POWER_DOWN_MODE_reserved0_SHIFT            14

/* MEMC_0_DDR :: POWER_DOWN_MODE :: PDN_EN [13:13] */
#define BCHP_MEMC_0_DDR_POWER_DOWN_MODE_PDN_EN_MASK                0x00002000
#define BCHP_MEMC_0_DDR_POWER_DOWN_MODE_PDN_EN_SHIFT               13

/* MEMC_0_DDR :: POWER_DOWN_MODE :: PDN_MODE [12:12] */
#define BCHP_MEMC_0_DDR_POWER_DOWN_MODE_PDN_MODE_MASK              0x00001000
#define BCHP_MEMC_0_DDR_POWER_DOWN_MODE_PDN_MODE_SHIFT             12

/* MEMC_0_DDR :: POWER_DOWN_MODE :: INACT_CNT [11:00] */
#define BCHP_MEMC_0_DDR_POWER_DOWN_MODE_INACT_CNT_MASK             0x00000fff
#define BCHP_MEMC_0_DDR_POWER_DOWN_MODE_INACT_CNT_SHIFT            0

/***************************************************************************
 *CNTRLR_START_SEQ - Memory Controller Sequencer Enable
 ***************************************************************************/
/* MEMC_0_DDR :: CNTRLR_START_SEQ :: reserved0 [31:01] */
#define BCHP_MEMC_0_DDR_CNTRLR_START_SEQ_reserved0_MASK            0xfffffffe
#define BCHP_MEMC_0_DDR_CNTRLR_START_SEQ_reserved0_SHIFT           1

/* MEMC_0_DDR :: CNTRLR_START_SEQ :: START_SEQ [00:00] */
#define BCHP_MEMC_0_DDR_CNTRLR_START_SEQ_START_SEQ_MASK            0x00000001
#define BCHP_MEMC_0_DDR_CNTRLR_START_SEQ_START_SEQ_SHIFT           0

/***************************************************************************
 *CNTRLR_SM_TIMEOUT - Memory Controller , state machine timeout register.
 ***************************************************************************/
/* MEMC_0_DDR :: CNTRLR_SM_TIMEOUT :: reserved0 [31:17] */
#define BCHP_MEMC_0_DDR_CNTRLR_SM_TIMEOUT_reserved0_MASK           0xfffe0000
#define BCHP_MEMC_0_DDR_CNTRLR_SM_TIMEOUT_reserved0_SHIFT          17

/* MEMC_0_DDR :: CNTRLR_SM_TIMEOUT :: ENABLE [16:16] */
#define BCHP_MEMC_0_DDR_CNTRLR_SM_TIMEOUT_ENABLE_MASK              0x00010000
#define BCHP_MEMC_0_DDR_CNTRLR_SM_TIMEOUT_ENABLE_SHIFT             16

/* MEMC_0_DDR :: CNTRLR_SM_TIMEOUT :: COUNT [15:00] */
#define BCHP_MEMC_0_DDR_CNTRLR_SM_TIMEOUT_COUNT_MASK               0x0000ffff
#define BCHP_MEMC_0_DDR_CNTRLR_SM_TIMEOUT_COUNT_SHIFT              0

/***************************************************************************
 *POWER_DOWN_STATUS - Memory Controller , status register.
 ***************************************************************************/
/* MEMC_0_DDR :: POWER_DOWN_STATUS :: reserved0 [31:02] */
#define BCHP_MEMC_0_DDR_POWER_DOWN_STATUS_reserved0_MASK           0xfffffffc
#define BCHP_MEMC_0_DDR_POWER_DOWN_STATUS_reserved0_SHIFT          2

/* MEMC_0_DDR :: POWER_DOWN_STATUS :: STATUS [01:00] */
#define BCHP_MEMC_0_DDR_POWER_DOWN_STATUS_STATUS_MASK              0x00000003
#define BCHP_MEMC_0_DDR_POWER_DOWN_STATUS_STATUS_SHIFT             0

/***************************************************************************
 *BANK_STATUS - Memory Controller, Bank Status Register.
 ***************************************************************************/
/* MEMC_0_DDR :: BANK_STATUS :: reserved0 [31:08] */
#define BCHP_MEMC_0_DDR_BANK_STATUS_reserved0_MASK                 0xffffff00
#define BCHP_MEMC_0_DDR_BANK_STATUS_reserved0_SHIFT                8

/* MEMC_0_DDR :: BANK_STATUS :: BANK7_STATUS [07:07] */
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK7_STATUS_MASK              0x00000080
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK7_STATUS_SHIFT             7

/* MEMC_0_DDR :: BANK_STATUS :: BANK6_STATUS [06:06] */
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK6_STATUS_MASK              0x00000040
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK6_STATUS_SHIFT             6

/* MEMC_0_DDR :: BANK_STATUS :: BANK5_STATUS [05:05] */
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK5_STATUS_MASK              0x00000020
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK5_STATUS_SHIFT             5

/* MEMC_0_DDR :: BANK_STATUS :: BANK4_STATUS [04:04] */
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK4_STATUS_MASK              0x00000010
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK4_STATUS_SHIFT             4

/* MEMC_0_DDR :: BANK_STATUS :: BANK3_STATUS [03:03] */
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK3_STATUS_MASK              0x00000008
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK3_STATUS_SHIFT             3

/* MEMC_0_DDR :: BANK_STATUS :: BANK2_STATUS [02:02] */
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK2_STATUS_MASK              0x00000004
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK2_STATUS_SHIFT             2

/* MEMC_0_DDR :: BANK_STATUS :: BANK1_STATUS [01:01] */
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK1_STATUS_MASK              0x00000002
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK1_STATUS_SHIFT             1

/* MEMC_0_DDR :: BANK_STATUS :: BANK0_STATUS [00:00] */
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK0_STATUS_MASK              0x00000001
#define BCHP_MEMC_0_DDR_BANK_STATUS_BANK0_STATUS_SHIFT             0

/***************************************************************************
 *WCL_FIFO_STATUS - Memory Controller, Write latency comp fifo Status Register.
 ***************************************************************************/
/* MEMC_0_DDR :: WCL_FIFO_STATUS :: reserved0 [31:02] */
#define BCHP_MEMC_0_DDR_WCL_FIFO_STATUS_reserved0_MASK             0xfffffffc
#define BCHP_MEMC_0_DDR_WCL_FIFO_STATUS_reserved0_SHIFT            2

/* MEMC_0_DDR :: WCL_FIFO_STATUS :: OVERFLOW_STATUS [01:01] */
#define BCHP_MEMC_0_DDR_WCL_FIFO_STATUS_OVERFLOW_STATUS_MASK       0x00000002
#define BCHP_MEMC_0_DDR_WCL_FIFO_STATUS_OVERFLOW_STATUS_SHIFT      1

/* MEMC_0_DDR :: WCL_FIFO_STATUS :: UNDERRUN_STATUS [00:00] */
#define BCHP_MEMC_0_DDR_WCL_FIFO_STATUS_UNDERRUN_STATUS_MASK       0x00000001
#define BCHP_MEMC_0_DDR_WCL_FIFO_STATUS_UNDERRUN_STATUS_SHIFT      0

#endif /* #ifndef BCHP_MEMC_0_DDR_H__ */

/* End of File */
