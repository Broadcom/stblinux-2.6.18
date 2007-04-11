/*     Copyright (c) 1999-2005, Broadcom Corporation
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
 * $brcm_Workfile: bcmpcmreg.h $
 * $brcm_Revision: SanJose_Linux_Devel\1 $
 * $brcm_Date: 5/14/02 3:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: \Linux\kernel\src\bcm97xxx\include\asm-mips\brcmstb\brcm97315\bcmpcmreg.h $
 * 
 * SanJose_Linux_Devel\1   5/14/02 3:42p eddieshi
 * newly created for 7315
 * 
 ***************************************************************************/
#ifndef BCMPCMREG_H
#define BCMPCMREG_H

#ifdef _cplusplus
extern "C" {
#endif

/**********************************************************************
 * BCM7115_A0 -- PCM_Registers
 **********************************************************************/
#define PCM_REVISION                     0x00000000 /* Revision */
#define PCM_GENERAL_CTRL                 0x00000004 /* PCM General Control */
#define PCM_CHANNEL_STATUS               0x00000008 /* Audio Channel Status */
#define PCM_RUN_CTRL                     0x0000000C /* Audio Run Control */
#define PCM_SW_RESET                     0x00000010 /* Soft Reset */
#define PCM_INTR_ENABLE                  0x00000014 /* Audio Interrupt Enable */
#define PCM_INTR_STATUS                  0x00000018 /* Audio Interrupt Status */
#define PCM_PB0_CTRL                     0x00000024 /* PB Channel 0 Control */
#define PCM_PB0_DESC_ADRS                0x00000028 /* PB Channel 0 descriptor address */
#define PCM_PB0_VOLUME                   0x0000002C /* PB Channel 0 Volume Control */
#define PCM_PB0_SRC_RATIO                0x00000030 /* PB Channel 0 SRC ratio */
#define PCM_PB0_CUR_BUF_ADRS             0x00000034 /* PB Channel 0 Current Buffer address */
#define PCM_PB1_CTRL                     0x00000038 /* PB Channel 1 Control */
#define PCM_PB1_DESC_ADRS                0x0000003C /* PB Channel 1 descriptor address */
#define PCM_PB1_VOLUME                   0x00000040 /* PB Channel 1 Volume Control */
#define PCM_PB1_SRC_RATIO                0x00000044 /* PB Channel 1 SRC ratio */
#define PCM_PB1_CUR_BUF_ADRS             0x00000048 /* PB Channel 1 Current Buffer address */
#define PCM_CAP0_CTRL                    0x00000060 /* CAP Channel 0 Control */
#define PCM_CAP0_DESC_ADRS               0x00000064 /* CAP Channel 0 descriptor address */
#define PCM_CAP0_VOLUME                  0x00000068 /* CAP Channel 0 Volume Control */
#define PCM_CAP0_SRC_RATIO               0x0000006C /* CAP Channel 0 SRC ratio */
#define PCM_CAP0_CUR_BUF_ADRS            0x00000070 /* CAP Channel 0 Current Buffer address */
#define PCM_CAP1_CTRL                    0x00000074 /* CAP Channel 1 Control */
#define PCM_CAP1_DESC_ADRS               0x00000078 /* CAP Channel 1 descriptor address */
#define PCM_CAP1_VOLUME                  0x0000007C /* CAP Channel 1 Volume Control */
#define PCM_CAP1_SRC_RATIO               0x00000080 /* CAP Channel 1 SRC ratio */
#define PCM_CAP1_CUR_BUF_ADRS            0x00000084 /* CAP Channel 1 Current Buffer address */

/**********************************************************************
 * PCM_REVISION
 **********************************************************************/
/* reserved0 [31:16] -  */
#define PCM_REVISION_reserved0_MASK                     0xFFFF0000UL
#define PCM_REVISION_reserved0_ALIGN                    0
#define PCM_REVISION_reserved0_BITS                     16
#define PCM_REVISION_reserved0_SHIFT                    16

/* MAJOR_REVISION [15:08] - unsigned */
#define PCM_REVISION_MAJOR_REVISION_MASK                0x0000FF00UL
#define PCM_REVISION_MAJOR_REVISION_ALIGN               0
#define PCM_REVISION_MAJOR_REVISION_BITS                8
#define PCM_REVISION_MAJOR_REVISION_SHIFT               8

/* MINOR_REVISION [07:00] - unsigned */
#define PCM_REVISION_MINOR_REVISION_MASK                0x000000FFUL
#define PCM_REVISION_MINOR_REVISION_ALIGN               0
#define PCM_REVISION_MINOR_REVISION_BITS                8
#define PCM_REVISION_MINOR_REVISION_SHIFT               0

/**********************************************************************
 * PCM_GENERAL_CTRL
 **********************************************************************/
/* reserved0 [31:01] -  */
#define PCM_GENERAL_CTRL_reserved0_MASK                 0xFFFFFFFEUL
#define PCM_GENERAL_CTRL_reserved0_ALIGN                0
#define PCM_GENERAL_CTRL_reserved0_BITS                 31
#define PCM_GENERAL_CTRL_reserved0_SHIFT                1

/* LTL_ENDIAN [00:00] - boolean */
#define PCM_GENERAL_CTRL_LTL_ENDIAN_MASK                0x00000001UL
#define PCM_GENERAL_CTRL_LTL_ENDIAN_ALIGN               0
#define PCM_GENERAL_CTRL_LTL_ENDIAN_BITS                1
#define PCM_GENERAL_CTRL_LTL_ENDIAN_SHIFT               0

/**********************************************************************
 * PCM_CHANNEL_STATUS
 **********************************************************************/
/* reserved0 [31:05] -  */
#define PCM_CHANNEL_STATUS_reserved0_MASK               0xFFFFFFE0UL
#define PCM_CHANNEL_STATUS_reserved0_ALIGN              0
#define PCM_CHANNEL_STATUS_reserved0_BITS               27
#define PCM_CHANNEL_STATUS_reserved0_SHIFT              5

/* CAP1_ACTIVE [04:04] - boolean */
#define PCM_CHANNEL_STATUS_CAP1_ACTIVE_MASK             0x00000010UL
#define PCM_CHANNEL_STATUS_CAP1_ACTIVE_ALIGN            0
#define PCM_CHANNEL_STATUS_CAP1_ACTIVE_BITS             1
#define PCM_CHANNEL_STATUS_CAP1_ACTIVE_SHIFT            4

/* CAP0_ACTIVE [03:03] - boolean */
#define PCM_CHANNEL_STATUS_CAP0_ACTIVE_MASK             0x00000008UL
#define PCM_CHANNEL_STATUS_CAP0_ACTIVE_ALIGN            0
#define PCM_CHANNEL_STATUS_CAP0_ACTIVE_BITS             1
#define PCM_CHANNEL_STATUS_CAP0_ACTIVE_SHIFT            3

/* reserved1 [02:02] -  */
#define PCM_CHANNEL_STATUS_reserved1_MASK               0x00000004UL
#define PCM_CHANNEL_STATUS_reserved1_ALIGN              0
#define PCM_CHANNEL_STATUS_reserved1_BITS               1
#define PCM_CHANNEL_STATUS_reserved1_SHIFT              2

/* PB1_ACTIVE [01:01] - boolean */
#define PCM_CHANNEL_STATUS_PB1_ACTIVE_MASK              0x00000002UL
#define PCM_CHANNEL_STATUS_PB1_ACTIVE_ALIGN             0
#define PCM_CHANNEL_STATUS_PB1_ACTIVE_BITS              1
#define PCM_CHANNEL_STATUS_PB1_ACTIVE_SHIFT             1

/* PB0_ACTIVE [00:00] - boolean */
#define PCM_CHANNEL_STATUS_PB0_ACTIVE_MASK              0x00000001UL
#define PCM_CHANNEL_STATUS_PB0_ACTIVE_ALIGN             0
#define PCM_CHANNEL_STATUS_PB0_ACTIVE_BITS              1
#define PCM_CHANNEL_STATUS_PB0_ACTIVE_SHIFT             0

/**********************************************************************
 * PCM_RUN_CTRL
 **********************************************************************/
/* reserved0 [31:15] -  */
#define PCM_RUN_CTRL_reserved0_MASK                     0xFFFF8000UL
#define PCM_RUN_CTRL_reserved0_ALIGN                    0
#define PCM_RUN_CTRL_reserved0_BITS                     17
#define PCM_RUN_CTRL_reserved0_SHIFT                    15

/* CAP1_STOP [14:14] - boolean */
#define PCM_RUN_CTRL_CAP1_STOP_MASK                     0x00004000UL
#define PCM_RUN_CTRL_CAP1_STOP_ALIGN                    0
#define PCM_RUN_CTRL_CAP1_STOP_BITS                     1
#define PCM_RUN_CTRL_CAP1_STOP_SHIFT                    14

/* reserved1 [13:13] -  */
#define PCM_RUN_CTRL_reserved1_MASK                     0x00002000UL
#define PCM_RUN_CTRL_reserved1_ALIGN                    0
#define PCM_RUN_CTRL_reserved1_BITS                     1
#define PCM_RUN_CTRL_reserved1_SHIFT                    13

/* CAP1_START [12:12] - boolean */
#define PCM_RUN_CTRL_CAP1_START_MASK                    0x00001000UL
#define PCM_RUN_CTRL_CAP1_START_ALIGN                   0
#define PCM_RUN_CTRL_CAP1_START_BITS                    1
#define PCM_RUN_CTRL_CAP1_START_SHIFT                   12

/* CAP0_STOP [11:11] - boolean */
#define PCM_RUN_CTRL_CAP0_STOP_MASK                     0x00000800UL
#define PCM_RUN_CTRL_CAP0_STOP_ALIGN                    0
#define PCM_RUN_CTRL_CAP0_STOP_BITS                     1
#define PCM_RUN_CTRL_CAP0_STOP_SHIFT                    11

/* reserved2 [10:10] -  */
#define PCM_RUN_CTRL_reserved2_MASK                     0x00000400UL
#define PCM_RUN_CTRL_reserved2_ALIGN                    0
#define PCM_RUN_CTRL_reserved2_BITS                     1
#define PCM_RUN_CTRL_reserved2_SHIFT                    10

/* CAP0_START [09:09] - boolean */
#define PCM_RUN_CTRL_CAP0_START_MASK                    0x00000200UL
#define PCM_RUN_CTRL_CAP0_START_ALIGN                   0
#define PCM_RUN_CTRL_CAP0_START_BITS                    1
#define PCM_RUN_CTRL_CAP0_START_SHIFT                   9

/* reserved3 [08:06] -  */
#define PCM_RUN_CTRL_reserved3_MASK                     0x000001C0UL
#define PCM_RUN_CTRL_reserved3_ALIGN                    0
#define PCM_RUN_CTRL_reserved3_BITS                     3
#define PCM_RUN_CTRL_reserved3_SHIFT                    6

/* PB1_STOP [05:05] - boolean */
#define PCM_RUN_CTRL_PB1_STOP_MASK                      0x00000020UL
#define PCM_RUN_CTRL_PB1_STOP_ALIGN                     0
#define PCM_RUN_CTRL_PB1_STOP_BITS                      1
#define PCM_RUN_CTRL_PB1_STOP_SHIFT                     5

/* PB1_RESTART [04:04] - boolean */
#define PCM_RUN_CTRL_PB1_RESTART_MASK                   0x00000010UL
#define PCM_RUN_CTRL_PB1_RESTART_ALIGN                  0
#define PCM_RUN_CTRL_PB1_RESTART_BITS                   1
#define PCM_RUN_CTRL_PB1_RESTART_SHIFT                  4

/* PB1_START [03:03] - boolean */
#define PCM_RUN_CTRL_PB1_START_MASK                     0x00000008UL
#define PCM_RUN_CTRL_PB1_START_ALIGN                    0
#define PCM_RUN_CTRL_PB1_START_BITS                     1
#define PCM_RUN_CTRL_PB1_START_SHIFT                    3

/* PB0_STOP [02:02] - boolean */
#define PCM_RUN_CTRL_PB0_STOP_MASK                      0x00000004UL
#define PCM_RUN_CTRL_PB0_STOP_ALIGN                     0
#define PCM_RUN_CTRL_PB0_STOP_BITS                      1
#define PCM_RUN_CTRL_PB0_STOP_SHIFT                     2

/* PB0_RESTART [01:01] - boolean */
#define PCM_RUN_CTRL_PB0_RESTART_MASK                   0x00000002UL
#define PCM_RUN_CTRL_PB0_RESTART_ALIGN                  0
#define PCM_RUN_CTRL_PB0_RESTART_BITS                   1
#define PCM_RUN_CTRL_PB0_RESTART_SHIFT                  1

/* PB0_START [00:00] - boolean */
#define PCM_RUN_CTRL_PB0_START_MASK                     0x00000001UL
#define PCM_RUN_CTRL_PB0_START_ALIGN                    0
#define PCM_RUN_CTRL_PB0_START_BITS                     1
#define PCM_RUN_CTRL_PB0_START_SHIFT                    0

/**********************************************************************
 * PCM_SW_RESET
 **********************************************************************/
/* reserved0 [31:04] -  */
#define PCM_SW_RESET_reserved0_MASK                     0xFFFFFFF0UL
#define PCM_SW_RESET_reserved0_ALIGN                    0
#define PCM_SW_RESET_reserved0_BITS                     28
#define PCM_SW_RESET_reserved0_SHIFT                    4

/* SYSCLK_SW_RESET [03:03] - boolean */
#define PCM_SW_RESET_SYSCLK_SW_RESET_MASK               0x00000008UL
#define PCM_SW_RESET_SYSCLK_SW_RESET_ALIGN              0
#define PCM_SW_RESET_SYSCLK_SW_RESET_BITS               1
#define PCM_SW_RESET_SYSCLK_SW_RESET_SHIFT              3

/* CAPCLK1_SW_RESET [02:02] - boolean */
#define PCM_SW_RESET_CAPCLK1_SW_RESET_MASK              0x00000004UL
#define PCM_SW_RESET_CAPCLK1_SW_RESET_ALIGN             0
#define PCM_SW_RESET_CAPCLK1_SW_RESET_BITS              1
#define PCM_SW_RESET_CAPCLK1_SW_RESET_SHIFT             2

/* CAPCLK0_SW_RESET [01:01] - boolean */
#define PCM_SW_RESET_CAPCLK0_SW_RESET_MASK              0x00000002UL
#define PCM_SW_RESET_CAPCLK0_SW_RESET_ALIGN             0
#define PCM_SW_RESET_CAPCLK0_SW_RESET_BITS              1
#define PCM_SW_RESET_CAPCLK0_SW_RESET_SHIFT             1

/* AUDCLK_SW_RESET [00:00] - boolean */
#define PCM_SW_RESET_AUDCLK_SW_RESET_MASK               0x00000001UL
#define PCM_SW_RESET_AUDCLK_SW_RESET_ALIGN              0
#define PCM_SW_RESET_AUDCLK_SW_RESET_BITS               1
#define PCM_SW_RESET_AUDCLK_SW_RESET_SHIFT              0

/**********************************************************************
 * PCM_INTR_ENABLE
 **********************************************************************/
/* reserved0 [31:05] -  */
#define PCM_INTR_ENABLE_reserved0_MASK                  0xFFFFFFE0UL
#define PCM_INTR_ENABLE_reserved0_ALIGN                 0
#define PCM_INTR_ENABLE_reserved0_BITS                  27
#define PCM_INTR_ENABLE_reserved0_SHIFT                 5

/* CAP1_INT_EN [04:04] - boolean */
#define PCM_INTR_ENABLE_CAP1_INT_EN_MASK                0x00000010UL
#define PCM_INTR_ENABLE_CAP1_INT_EN_ALIGN               0
#define PCM_INTR_ENABLE_CAP1_INT_EN_BITS                1
#define PCM_INTR_ENABLE_CAP1_INT_EN_SHIFT               4

/* CAP0_INT_EN [03:03] - boolean */
#define PCM_INTR_ENABLE_CAP0_INT_EN_MASK                0x00000008UL
#define PCM_INTR_ENABLE_CAP0_INT_EN_ALIGN               0
#define PCM_INTR_ENABLE_CAP0_INT_EN_BITS                1
#define PCM_INTR_ENABLE_CAP0_INT_EN_SHIFT               3

/* reserved1 [02:02] -  */
#define PCM_INTR_ENABLE_reserved1_MASK                  0x00000004UL
#define PCM_INTR_ENABLE_reserved1_ALIGN                 0
#define PCM_INTR_ENABLE_reserved1_BITS                  1
#define PCM_INTR_ENABLE_reserved1_SHIFT                 2

/* PB1_INT_EN [01:01] - boolean */
#define PCM_INTR_ENABLE_PB1_INT_EN_MASK                 0x00000002UL
#define PCM_INTR_ENABLE_PB1_INT_EN_ALIGN                0
#define PCM_INTR_ENABLE_PB1_INT_EN_BITS                 1
#define PCM_INTR_ENABLE_PB1_INT_EN_SHIFT                1

/* PB0_INT_EN [00:00] - boolean */
#define PCM_INTR_ENABLE_PB0_INT_EN_MASK                 0x00000001UL
#define PCM_INTR_ENABLE_PB0_INT_EN_ALIGN                0
#define PCM_INTR_ENABLE_PB0_INT_EN_BITS                 1
#define PCM_INTR_ENABLE_PB0_INT_EN_SHIFT                0

/**********************************************************************
 * PCM_INTR_STATUS
 **********************************************************************/
/* reserved0 [31:05] -  */
#define PCM_INTR_STATUS_reserved0_MASK                  0xFFFFFFE0UL
#define PCM_INTR_STATUS_reserved0_ALIGN                 0
#define PCM_INTR_STATUS_reserved0_BITS                  27
#define PCM_INTR_STATUS_reserved0_SHIFT                 5

/* CAP1_ACTIVE [04:04] - boolean */
#define PCM_INTR_STATUS_CAP1_ACTIVE_MASK                0x00000010UL
#define PCM_INTR_STATUS_CAP1_ACTIVE_ALIGN               0
#define PCM_INTR_STATUS_CAP1_ACTIVE_BITS                1
#define PCM_INTR_STATUS_CAP1_ACTIVE_SHIFT               4

/* CAP0_ACTIVE [03:03] - boolean */
#define PCM_INTR_STATUS_CAP0_ACTIVE_MASK                0x00000008UL
#define PCM_INTR_STATUS_CAP0_ACTIVE_ALIGN               0
#define PCM_INTR_STATUS_CAP0_ACTIVE_BITS                1
#define PCM_INTR_STATUS_CAP0_ACTIVE_SHIFT               3

/* reserved1 [02:02] -  */
#define PCM_INTR_STATUS_reserved1_MASK                  0x00000004UL
#define PCM_INTR_STATUS_reserved1_ALIGN                 0
#define PCM_INTR_STATUS_reserved1_BITS                  1
#define PCM_INTR_STATUS_reserved1_SHIFT                 2

/* PB1_ACTIVE [01:01] - boolean */
#define PCM_INTR_STATUS_PB1_ACTIVE_MASK                 0x00000002UL
#define PCM_INTR_STATUS_PB1_ACTIVE_ALIGN                0
#define PCM_INTR_STATUS_PB1_ACTIVE_BITS                 1
#define PCM_INTR_STATUS_PB1_ACTIVE_SHIFT                1

/* PB0_ACTIVE [00:00] - boolean */
#define PCM_INTR_STATUS_PB0_ACTIVE_MASK                 0x00000001UL
#define PCM_INTR_STATUS_PB0_ACTIVE_ALIGN                0
#define PCM_INTR_STATUS_PB0_ACTIVE_BITS                 1
#define PCM_INTR_STATUS_PB0_ACTIVE_SHIFT                0

/**********************************************************************
 * PCM_PB0_CTRL
 **********************************************************************/
/* reserved0 [31:02] -  */
#define PCM_PB0_CTRL_reserved0_MASK                     0xFFFFFFFCUL
#define PCM_PB0_CTRL_reserved0_ALIGN                    0
#define PCM_PB0_CTRL_reserved0_BITS                     30
#define PCM_PB0_CTRL_reserved0_SHIFT                    2

/* SIZE [01:01] - boolean */
#define PCM_PB0_CTRL_SIZE_MASK                          0x00000002UL
#define PCM_PB0_CTRL_SIZE_ALIGN                         0
#define PCM_PB0_CTRL_SIZE_BITS                          1
#define PCM_PB0_CTRL_SIZE_SHIFT                         1

/* MODE [00:00] - boolean */
#define PCM_PB0_CTRL_MODE_MASK                          0x00000001UL
#define PCM_PB0_CTRL_MODE_ALIGN                         0
#define PCM_PB0_CTRL_MODE_BITS                          1
#define PCM_PB0_CTRL_MODE_SHIFT                         0

/**********************************************************************
 * PCM_PB0_DESC_ADRS
 **********************************************************************/
/* reserved0 [31:28] -  */
#define PCM_PB0_DESC_ADRS_reserved0_MASK                0xF0000000UL
#define PCM_PB0_DESC_ADRS_reserved0_ALIGN               0
#define PCM_PB0_DESC_ADRS_reserved0_BITS                4
#define PCM_PB0_DESC_ADRS_reserved0_SHIFT               28

/* ADRS [27:04] - unsigned */
#define PCM_PB0_DESC_ADRS_ADRS_MASK                     0x0FFFFFF0UL
#define PCM_PB0_DESC_ADRS_ADRS_ALIGN                    0
#define PCM_PB0_DESC_ADRS_ADRS_BITS                     24
#define PCM_PB0_DESC_ADRS_ADRS_SHIFT                    4

/* reserved1 [03:00] -  */
#define PCM_PB0_DESC_ADRS_reserved1_MASK                0x0000000FUL
#define PCM_PB0_DESC_ADRS_reserved1_ALIGN               0
#define PCM_PB0_DESC_ADRS_reserved1_BITS                4
#define PCM_PB0_DESC_ADRS_reserved1_SHIFT               0

/**********************************************************************
 * PCM_PB0_VOLUME
 **********************************************************************/
/* reserved0 [31:29] -  */
#define PCM_PB0_VOLUME_reserved0_MASK                   0xE0000000UL
#define PCM_PB0_VOLUME_reserved0_ALIGN                  0
#define PCM_PB0_VOLUME_reserved0_BITS                   3
#define PCM_PB0_VOLUME_reserved0_SHIFT                  29

/* LEFT_VOLUME [28:16] - unsigned */
#define PCM_PB0_VOLUME_LEFT_VOLUME_MASK                 0x1FFF0000UL
#define PCM_PB0_VOLUME_LEFT_VOLUME_ALIGN                0
#define PCM_PB0_VOLUME_LEFT_VOLUME_BITS                 13
#define PCM_PB0_VOLUME_LEFT_VOLUME_SHIFT                16

/* reserved1 [15:13] -  */
#define PCM_PB0_VOLUME_reserved1_MASK                   0x0000E000UL
#define PCM_PB0_VOLUME_reserved1_ALIGN                  0
#define PCM_PB0_VOLUME_reserved1_BITS                   3
#define PCM_PB0_VOLUME_reserved1_SHIFT                  13

/* RIGHT_VOLUME [12:00] - unsigned */
#define PCM_PB0_VOLUME_RIGHT_VOLUME_MASK                0x00001FFFUL
#define PCM_PB0_VOLUME_RIGHT_VOLUME_ALIGN               0
#define PCM_PB0_VOLUME_RIGHT_VOLUME_BITS                13
#define PCM_PB0_VOLUME_RIGHT_VOLUME_SHIFT               0

/**********************************************************************
 * PCM_PB0_SRC_RATIO
 **********************************************************************/
/* reserved0 [31:10] -  */
#define PCM_PB0_SRC_RATIO_reserved0_MASK                0xFFFFFC00UL
#define PCM_PB0_SRC_RATIO_reserved0_ALIGN               0
#define PCM_PB0_SRC_RATIO_reserved0_BITS                22
#define PCM_PB0_SRC_RATIO_reserved0_SHIFT               10

/* IN_OUT_RATIO [09:00] - unsigned */
#define PCM_PB0_SRC_RATIO_IN_OUT_RATIO_MASK             0x000003FFUL
#define PCM_PB0_SRC_RATIO_IN_OUT_RATIO_ALIGN            0
#define PCM_PB0_SRC_RATIO_IN_OUT_RATIO_BITS             10
#define PCM_PB0_SRC_RATIO_IN_OUT_RATIO_SHIFT            0

/**********************************************************************
 * PCM_PB0_CUR_BUF_ADRS
 **********************************************************************/
/* reserved0 [31:28] -  */
#define PCM_PB0_CUR_BUF_ADRS_reserved0_MASK             0xF0000000UL
#define PCM_PB0_CUR_BUF_ADRS_reserved0_ALIGN            0
#define PCM_PB0_CUR_BUF_ADRS_reserved0_BITS             4
#define PCM_PB0_CUR_BUF_ADRS_reserved0_SHIFT            28

/* ADRS [27:00] - unsigned */
#define PCM_PB0_CUR_BUF_ADRS_ADRS_MASK                  0x0FFFFFFFUL
#define PCM_PB0_CUR_BUF_ADRS_ADRS_ALIGN                 0
#define PCM_PB0_CUR_BUF_ADRS_ADRS_BITS                  28
#define PCM_PB0_CUR_BUF_ADRS_ADRS_SHIFT                 0

/**********************************************************************
 * PCM_PB1_CTRL
 **********************************************************************/
/* reserved0 [31:02] -  */
#define PCM_PB1_CTRL_reserved0_MASK                     0xFFFFFFFCUL
#define PCM_PB1_CTRL_reserved0_ALIGN                    0
#define PCM_PB1_CTRL_reserved0_BITS                     30
#define PCM_PB1_CTRL_reserved0_SHIFT                    2

/* SIZE [01:01] - boolean */
#define PCM_PB1_CTRL_SIZE_MASK                          0x00000002UL
#define PCM_PB1_CTRL_SIZE_ALIGN                         0
#define PCM_PB1_CTRL_SIZE_BITS                          1
#define PCM_PB1_CTRL_SIZE_SHIFT                         1

/* MODE [00:00] - boolean */
#define PCM_PB1_CTRL_MODE_MASK                          0x00000001UL
#define PCM_PB1_CTRL_MODE_ALIGN                         0
#define PCM_PB1_CTRL_MODE_BITS                          1
#define PCM_PB1_CTRL_MODE_SHIFT                         0

/**********************************************************************
 * PCM_PB1_DESC_ADRS
 **********************************************************************/
/* reserved0 [31:28] -  */
#define PCM_PB1_DESC_ADRS_reserved0_MASK                0xF0000000UL
#define PCM_PB1_DESC_ADRS_reserved0_ALIGN               0
#define PCM_PB1_DESC_ADRS_reserved0_BITS                4
#define PCM_PB1_DESC_ADRS_reserved0_SHIFT               28

/* ADRS [27:04] - unsigned */
#define PCM_PB1_DESC_ADRS_ADRS_MASK                     0x0FFFFFF0UL
#define PCM_PB1_DESC_ADRS_ADRS_ALIGN                    0
#define PCM_PB1_DESC_ADRS_ADRS_BITS                     24
#define PCM_PB1_DESC_ADRS_ADRS_SHIFT                    4

/* reserved1 [03:00] -  */
#define PCM_PB1_DESC_ADRS_reserved1_MASK                0x0000000FUL
#define PCM_PB1_DESC_ADRS_reserved1_ALIGN               0
#define PCM_PB1_DESC_ADRS_reserved1_BITS                4
#define PCM_PB1_DESC_ADRS_reserved1_SHIFT               0

/**********************************************************************
 * PCM_PB1_VOLUME
 **********************************************************************/
/* reserved0 [31:29] -  */
#define PCM_PB1_VOLUME_reserved0_MASK                   0xE0000000UL
#define PCM_PB1_VOLUME_reserved0_ALIGN                  0
#define PCM_PB1_VOLUME_reserved0_BITS                   3
#define PCM_PB1_VOLUME_reserved0_SHIFT                  29

/* LEFT_VOLUME [28:16] - unsigned */
#define PCM_PB1_VOLUME_LEFT_VOLUME_MASK                 0x1FFF0000UL
#define PCM_PB1_VOLUME_LEFT_VOLUME_ALIGN                0
#define PCM_PB1_VOLUME_LEFT_VOLUME_BITS                 13
#define PCM_PB1_VOLUME_LEFT_VOLUME_SHIFT                16

/* reserved1 [15:13] -  */
#define PCM_PB1_VOLUME_reserved1_MASK                   0x0000E000UL
#define PCM_PB1_VOLUME_reserved1_ALIGN                  0
#define PCM_PB1_VOLUME_reserved1_BITS                   3
#define PCM_PB1_VOLUME_reserved1_SHIFT                  13

/* RIGHT_VOLUME [12:00] - unsigned */
#define PCM_PB1_VOLUME_RIGHT_VOLUME_MASK                0x00001FFFUL
#define PCM_PB1_VOLUME_RIGHT_VOLUME_ALIGN               0
#define PCM_PB1_VOLUME_RIGHT_VOLUME_BITS                13
#define PCM_PB1_VOLUME_RIGHT_VOLUME_SHIFT               0

/**********************************************************************
 * PCM_PB1_SRC_RATIO
 **********************************************************************/
/* reserved0 [31:10] -  */
#define PCM_PB1_SRC_RATIO_reserved0_MASK                0xFFFFFC00UL
#define PCM_PB1_SRC_RATIO_reserved0_ALIGN               0
#define PCM_PB1_SRC_RATIO_reserved0_BITS                22
#define PCM_PB1_SRC_RATIO_reserved0_SHIFT               10

/* IN_OUT_RATIO [09:00] - unsigned */
#define PCM_PB1_SRC_RATIO_IN_OUT_RATIO_MASK             0x000003FFUL
#define PCM_PB1_SRC_RATIO_IN_OUT_RATIO_ALIGN            0
#define PCM_PB1_SRC_RATIO_IN_OUT_RATIO_BITS             10
#define PCM_PB1_SRC_RATIO_IN_OUT_RATIO_SHIFT            0

/**********************************************************************
 * PCM_PB1_CUR_BUF_ADRS
 **********************************************************************/
/* reserved0 [31:28] -  */
#define PCM_PB1_CUR_BUF_ADRS_reserved0_MASK             0xF0000000UL
#define PCM_PB1_CUR_BUF_ADRS_reserved0_ALIGN            0
#define PCM_PB1_CUR_BUF_ADRS_reserved0_BITS             4
#define PCM_PB1_CUR_BUF_ADRS_reserved0_SHIFT            28

/* ADRS [27:00] - unsigned */
#define PCM_PB1_CUR_BUF_ADRS_ADRS_MASK                  0x0FFFFFFFUL
#define PCM_PB1_CUR_BUF_ADRS_ADRS_ALIGN                 0
#define PCM_PB1_CUR_BUF_ADRS_ADRS_BITS                  28
#define PCM_PB1_CUR_BUF_ADRS_ADRS_SHIFT                 0

/**********************************************************************
 * PCM_CAP0_CTRL
 **********************************************************************/
/* reserved0 [31:12] -  */
#define PCM_CAP0_CTRL_reserved0_MASK                    0xFFFFF000UL
#define PCM_CAP0_CTRL_reserved0_ALIGN                   0
#define PCM_CAP0_CTRL_reserved0_BITS                    20
#define PCM_CAP0_CTRL_reserved0_SHIFT                   12

/* THRESHOLD [11:09] - unsigned */
#define PCM_CAP0_CTRL_THRESHOLD_MASK                    0x00000E00UL
#define PCM_CAP0_CTRL_THRESHOLD_ALIGN                   0
#define PCM_CAP0_CTRL_THRESHOLD_BITS                    3
#define PCM_CAP0_CTRL_THRESHOLD_SHIFT                   9

/* ADAPT_FILT_EN [08:08] - boolean */
#define PCM_CAP0_CTRL_ADAPT_FILT_EN_MASK                0x00000100UL
#define PCM_CAP0_CTRL_ADAPT_FILT_EN_ALIGN               0
#define PCM_CAP0_CTRL_ADAPT_FILT_EN_BITS                1
#define PCM_CAP0_CTRL_ADAPT_FILT_EN_SHIFT               8

/* I2S_BIT_RESOLUTION [07:07] - boolean */
#define PCM_CAP0_CTRL_I2S_BIT_RESOLUTION_MASK           0x00000080UL
#define PCM_CAP0_CTRL_I2S_BIT_RESOLUTION_ALIGN          0
#define PCM_CAP0_CTRL_I2S_BIT_RESOLUTION_BITS           1
#define PCM_CAP0_CTRL_I2S_BIT_RESOLUTION_SHIFT          7

/* I2S_SOURCE [06:06] - boolean */
#define PCM_CAP0_CTRL_I2S_SOURCE_MASK                   0x00000040UL
#define PCM_CAP0_CTRL_I2S_SOURCE_ALIGN                  0
#define PCM_CAP0_CTRL_I2S_SOURCE_BITS                   1
#define PCM_CAP0_CTRL_I2S_SOURCE_SHIFT                  6

/* I2S_LRCLK_POLARITY [05:05] - boolean */
#define PCM_CAP0_CTRL_I2S_LRCLK_POLARITY_MASK           0x00000020UL
#define PCM_CAP0_CTRL_I2S_LRCLK_POLARITY_ALIGN          0
#define PCM_CAP0_CTRL_I2S_LRCLK_POLARITY_BITS           1
#define PCM_CAP0_CTRL_I2S_LRCLK_POLARITY_SHIFT          5

/* I2S_LRCLK_DLY [04:04] - boolean */
#define PCM_CAP0_CTRL_I2S_LRCLK_DLY_MASK                0x00000010UL
#define PCM_CAP0_CTRL_I2S_LRCLK_DLY_ALIGN               0
#define PCM_CAP0_CTRL_I2S_LRCLK_DLY_BITS                1
#define PCM_CAP0_CTRL_I2S_LRCLK_DLY_SHIFT               4

/* reserved1 [03:00] -  */
#define PCM_CAP0_CTRL_reserved1_MASK                    0x0000000FUL
#define PCM_CAP0_CTRL_reserved1_ALIGN                   0
#define PCM_CAP0_CTRL_reserved1_BITS                    4
#define PCM_CAP0_CTRL_reserved1_SHIFT                   0

/**********************************************************************
 * PCM_CAP0_DESC_ADRS
 **********************************************************************/
/* reserved0 [31:28] -  */
#define PCM_CAP0_DESC_ADRS_reserved0_MASK               0xF0000000UL
#define PCM_CAP0_DESC_ADRS_reserved0_ALIGN              0
#define PCM_CAP0_DESC_ADRS_reserved0_BITS               4
#define PCM_CAP0_DESC_ADRS_reserved0_SHIFT              28

/* ADRS [27:04] - unsigned */
#define PCM_CAP0_DESC_ADRS_ADRS_MASK                    0x0FFFFFF0UL
#define PCM_CAP0_DESC_ADRS_ADRS_ALIGN                   0
#define PCM_CAP0_DESC_ADRS_ADRS_BITS                    24
#define PCM_CAP0_DESC_ADRS_ADRS_SHIFT                   4

/* reserved1 [03:00] -  */
#define PCM_CAP0_DESC_ADRS_reserved1_MASK               0x0000000FUL
#define PCM_CAP0_DESC_ADRS_reserved1_ALIGN              0
#define PCM_CAP0_DESC_ADRS_reserved1_BITS               4
#define PCM_CAP0_DESC_ADRS_reserved1_SHIFT              0

/**********************************************************************
 * PCM_CAP0_VOLUME
 **********************************************************************/
/* reserved0 [31:00] -  */
#define PCM_CAP0_VOLUME_reserved0_MASK                  0xFFFFFFFFUL
#define PCM_CAP0_VOLUME_reserved0_ALIGN                 0
#define PCM_CAP0_VOLUME_reserved0_BITS                  32
#define PCM_CAP0_VOLUME_reserved0_SHIFT                 0

/**********************************************************************
 * PCM_CAP0_SRC_RATIO
 **********************************************************************/
/* reserved0 [31:00] -  */
#define PCM_CAP0_SRC_RATIO_reserved0_MASK               0xFFFFFFFFUL
#define PCM_CAP0_SRC_RATIO_reserved0_ALIGN              0
#define PCM_CAP0_SRC_RATIO_reserved0_BITS               32
#define PCM_CAP0_SRC_RATIO_reserved0_SHIFT              0

/**********************************************************************
 * PCM_CAP0_CUR_BUF_ADRS
 **********************************************************************/
/* reserved0 [31:28] -  */
#define PCM_CAP0_CUR_BUF_ADRS_reserved0_MASK            0xF0000000UL
#define PCM_CAP0_CUR_BUF_ADRS_reserved0_ALIGN           0
#define PCM_CAP0_CUR_BUF_ADRS_reserved0_BITS            4
#define PCM_CAP0_CUR_BUF_ADRS_reserved0_SHIFT           28

/* ADRS [27:00] - unsigned */
#define PCM_CAP0_CUR_BUF_ADRS_ADRS_MASK                 0x0FFFFFFFUL
#define PCM_CAP0_CUR_BUF_ADRS_ADRS_ALIGN                0
#define PCM_CAP0_CUR_BUF_ADRS_ADRS_BITS                 28
#define PCM_CAP0_CUR_BUF_ADRS_ADRS_SHIFT                0

/**********************************************************************
 * PCM_CAP1_CTRL
 **********************************************************************/
/* reserved0 [31:12] -  */
#define PCM_CAP1_CTRL_reserved0_MASK                    0xFFFFF000UL
#define PCM_CAP1_CTRL_reserved0_ALIGN                   0
#define PCM_CAP1_CTRL_reserved0_BITS                    20
#define PCM_CAP1_CTRL_reserved0_SHIFT                   12

/* THRESHOLD [11:09] - unsigned */
#define PCM_CAP1_CTRL_THRESHOLD_MASK                    0x00000E00UL
#define PCM_CAP1_CTRL_THRESHOLD_ALIGN                   0
#define PCM_CAP1_CTRL_THRESHOLD_BITS                    3
#define PCM_CAP1_CTRL_THRESHOLD_SHIFT                   9

/* ADAPT_FILT_EN [08:08] - boolean */
#define PCM_CAP1_CTRL_ADAPT_FILT_EN_MASK                0x00000100UL
#define PCM_CAP1_CTRL_ADAPT_FILT_EN_ALIGN               0
#define PCM_CAP1_CTRL_ADAPT_FILT_EN_BITS                1
#define PCM_CAP1_CTRL_ADAPT_FILT_EN_SHIFT               8

/* I2S_BIT_RESOLUTION [07:07] - boolean */
#define PCM_CAP1_CTRL_I2S_BIT_RESOLUTION_MASK           0x00000080UL
#define PCM_CAP1_CTRL_I2S_BIT_RESOLUTION_ALIGN          0
#define PCM_CAP1_CTRL_I2S_BIT_RESOLUTION_BITS           1
#define PCM_CAP1_CTRL_I2S_BIT_RESOLUTION_SHIFT          7

/* I2S_SOURCE [06:06] - boolean */
#define PCM_CAP1_CTRL_I2S_SOURCE_MASK                   0x00000040UL
#define PCM_CAP1_CTRL_I2S_SOURCE_ALIGN                  0
#define PCM_CAP1_CTRL_I2S_SOURCE_BITS                   1
#define PCM_CAP1_CTRL_I2S_SOURCE_SHIFT                  6

/* I2S_LRCLK_POLARITY [05:05] - boolean */
#define PCM_CAP1_CTRL_I2S_LRCLK_POLARITY_MASK           0x00000020UL
#define PCM_CAP1_CTRL_I2S_LRCLK_POLARITY_ALIGN          0
#define PCM_CAP1_CTRL_I2S_LRCLK_POLARITY_BITS           1
#define PCM_CAP1_CTRL_I2S_LRCLK_POLARITY_SHIFT          5

/* I2S_LRCLK_DLY [04:04] - boolean */
#define PCM_CAP1_CTRL_I2S_LRCLK_DLY_MASK                0x00000010UL
#define PCM_CAP1_CTRL_I2S_LRCLK_DLY_ALIGN               0
#define PCM_CAP1_CTRL_I2S_LRCLK_DLY_BITS                1
#define PCM_CAP1_CTRL_I2S_LRCLK_DLY_SHIFT               4

/* reserved1 [03:00] -  */
#define PCM_CAP1_CTRL_reserved1_MASK                    0x0000000FUL
#define PCM_CAP1_CTRL_reserved1_ALIGN                   0
#define PCM_CAP1_CTRL_reserved1_BITS                    4
#define PCM_CAP1_CTRL_reserved1_SHIFT                   0

/**********************************************************************
 * PCM_CAP1_DESC_ADRS
 **********************************************************************/
/* reserved0 [31:28] -  */
#define PCM_CAP1_DESC_ADRS_reserved0_MASK               0xF0000000UL
#define PCM_CAP1_DESC_ADRS_reserved0_ALIGN              0
#define PCM_CAP1_DESC_ADRS_reserved0_BITS               4
#define PCM_CAP1_DESC_ADRS_reserved0_SHIFT              28

/* ADRS [27:04] - unsigned */
#define PCM_CAP1_DESC_ADRS_ADRS_MASK                    0x0FFFFFF0UL
#define PCM_CAP1_DESC_ADRS_ADRS_ALIGN                   0
#define PCM_CAP1_DESC_ADRS_ADRS_BITS                    24
#define PCM_CAP1_DESC_ADRS_ADRS_SHIFT                   4

/* reserved1 [03:00] -  */
#define PCM_CAP1_DESC_ADRS_reserved1_MASK               0x0000000FUL
#define PCM_CAP1_DESC_ADRS_reserved1_ALIGN              0
#define PCM_CAP1_DESC_ADRS_reserved1_BITS               4
#define PCM_CAP1_DESC_ADRS_reserved1_SHIFT              0

/**********************************************************************
 * PCM_CAP1_VOLUME
 **********************************************************************/
/* reserved0 [31:00] -  */
#define PCM_CAP1_VOLUME_reserved0_MASK                  0xFFFFFFFFUL
#define PCM_CAP1_VOLUME_reserved0_ALIGN                 0
#define PCM_CAP1_VOLUME_reserved0_BITS                  32
#define PCM_CAP1_VOLUME_reserved0_SHIFT                 0

/**********************************************************************
 * PCM_CAP1_SRC_RATIO
 **********************************************************************/
/* reserved0 [31:00] -  */
#define PCM_CAP1_SRC_RATIO_reserved0_MASK               0xFFFFFFFFUL
#define PCM_CAP1_SRC_RATIO_reserved0_ALIGN              0
#define PCM_CAP1_SRC_RATIO_reserved0_BITS               32
#define PCM_CAP1_SRC_RATIO_reserved0_SHIFT              0

/**********************************************************************
 * PCM_CAP1_CUR_BUF_ADRS
 **********************************************************************/
/* reserved0 [31:28] -  */
#define PCM_CAP1_CUR_BUF_ADRS_reserved0_MASK            0xF0000000UL
#define PCM_CAP1_CUR_BUF_ADRS_reserved0_ALIGN           0
#define PCM_CAP1_CUR_BUF_ADRS_reserved0_BITS            4
#define PCM_CAP1_CUR_BUF_ADRS_reserved0_SHIFT           28

/* ADRS [27:00] - unsigned */
#define PCM_CAP1_CUR_BUF_ADRS_ADRS_MASK                 0x0FFFFFFFUL
#define PCM_CAP1_CUR_BUF_ADRS_ADRS_ALIGN                0
#define PCM_CAP1_CUR_BUF_ADRS_ADRS_BITS                 28
#define PCM_CAP1_CUR_BUF_ADRS_ADRS_SHIFT                0

/**********************************************************************
 *Enum: ACTIVE_INACTIVE
 **********************************************************************/
#define PCM_ACTIVE_INACTIVE_ACTIVE                      1
#define PCM_ACTIVE_INACTIVE_INACTIVE                    0

/**********************************************************************
 *Enum: ENABLE_DISABLE
 **********************************************************************/
#define PCM_ENABLE_DISABLE_ENABLE                       1
#define PCM_ENABLE_DISABLE_DISABLE                      0

/**********************************************************************
 *Enum: HIGH_LOW
 **********************************************************************/
#define PCM_HIGH_LOW_HIGH                               1
#define PCM_HIGH_LOW_LOW                                0

/**********************************************************************
 *Enum: I2S_BIT_RESOLUTION
 **********************************************************************/
#define PCM_I2S_BIT_RESOLUTION_RESOLUTION_18            1

/**********************************************************************
 *Enum: I2S_SOURCE_SEL
 **********************************************************************/
#define PCM_I2S_SOURCE_SEL_EXT_SOURCE                   1
#define PCM_I2S_SOURCE_SEL_INT_SOURCE                   0

/**********************************************************************
 *Enum: SIZE16_8
 **********************************************************************/
#define PCM_SIZE16_8_SIZE16                             1
#define PCM_SIZE16_8_SIZE8                              0

/**********************************************************************
 *Enum: SRC_RATIO
 **********************************************************************/
#define PCM_SRC_RATIO_NO_SRC                            256

/**********************************************************************
 *Enum: STEREO_MONO
 **********************************************************************/
#define PCM_STEREO_MONO_STEREO                          1
#define PCM_STEREO_MONO_MONO                            0

/**********************************************************************
 *Enum: VOLUME
 **********************************************************************/
#define PCM_VOLUME_FULL_VOL                             4096
#define PCM_VOLUME_MUTE                                 0

#ifdef _cplusplus
}
#endif

#endif	/* BCMPCMREG_H */
