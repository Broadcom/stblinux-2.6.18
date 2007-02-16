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
 * $brcm_Workfile: bcmvideoreg.h $
 * $brcm_Revision: SanJose_Linux_Devel/1 $
 * $brcm_Date: 8/13/02 5:6p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /Linux/kernel/src/bcm97xxx_2418/include/asm/brcmstb/brcm97315/bcmvideoreg.h $
 * 
 * SanJose_Linux_Devel/1   8/13/02 5:6p ttruong
 * Specific includes for various platforms 7110,7115,7315,7320
 * 
 * SanJose_Linux_Devel\1   5/14/02 3:40p eddieshi
 * newly created for 7315
 * 
 ***************************************************************************/
#ifndef BCMVIDEOREG_H
#define BCMVIDEOREG_H

#ifdef _cplusplus
extern "C" {
#endif

/**********************************************************************
 * BCM7115_A0 -- VDE_Registers
 **********************************************************************/
#define VDE_H_LENGTH                     0x00000000 /* CRT Horizontal Length */
#define VDE_V_HEIGHT                     0x00000004 /* CRT Vertical Height */
#define VDE_HS_WIDTH                     0x00000008 /* CRT HSync Width */
#define VDE_VS_WIDTH                     0x0000000C /* CRT VSync Width */
#define VDE_AV_X_START                   0x00000010 /* CRT Active Video (Background Color) Horizontal Start */
#define VDE_AV_X_END                     0x00000014 /* CRT Active Video (Background Color) Horizontal End */
#define VDE_AV_Y_START                   0x00000018 /* CRT Active Video (Background Color) Vertical Start */
#define VDE_AV_Y_END                     0x0000001C /* CRT Active Video (Background Color) Vertical End */
#define VDE_VWIN1_X_START                0x00000020 /* Video Window1 Horizontal Start */
#define VDE_VWIN1_X_END                  0x00000024 /* Video Window1 Horizontal End */
#define VDE_VWIN1_Y_START                0x00000028 /* Video Window1 Vertical Start */
#define VDE_VWIN1_Y_END                  0x0000002C /* Video Window1 Vertical End */
#define VDE_VWIN2_X_START                0x00000030 /* Video Window2 Horizontal Start */
#define VDE_VWIN2_X_END                  0x00000034 /* Video Window2 Horizontal End */
#define VDE_VWIN2_Y_START                0x00000038 /* Video Window2 Vertical Start */
#define VDE_VWIN2_Y_END                  0x0000003C /* Video Window2 Vertical End */
#define VDE_GFX_X_START                  0x00000040 /* Graphics Window Horizontal Start */
#define VDE_GFX_X_END                    0x00000044 /* Graphics Window Horizontal End */
#define VDE_GFX_Y_START                  0x00000048 /* Graphics Window Vertical Start */
#define VDE_GFX_Y_END                    0x0000004C /* Graphics Window Vertical End */
#define VDE_ANALOG_IN_X_START            0x00000050 /* Analog Video Input Horizontal Start */
#define VDE_ANALOG_IN_Y_START            0x00000054 /* Analog Video Input Vertical Start */
#define VDE_CAP_SIZE                     0x00000058 /* Video Capture Window Size */
#define VDE_SCALE_SRC_SIZE               0x0000005C /* Scaler Source Video Size */
#define VDE_SCALE_X_RATIO                0x00000060 /* Scaler X Scaling Factor */
#define VDE_SCALE_Y_RATIO                0x00000064 /* Scaler Y Scaling Factor */
#define VDE_SCALE_X_INIT_PHASE           0x00000068 /* Scaler Horizontal DDA Initial Phase Control */
#define VDE_SCALE_Y_TF2TF_INIT_PHASE     0x00000070 /* Vertical Scaler DDA Top Field Source on Top Field Display Initial Phase Control */
#define VDE_SCALE_Y_BF2BF_INIT_PHASE     0x00000074 /* Vertical Scaler DDA Bottom Field Source On Bottom Field Display Initial Phase Control */
#define VDE_SCALE_Y_TF2BF_INIT_PHASE     0x00000078 /* Vertical Scaler DDA Top Field Source on Bottom Field Display Initial Phase Control */
#define VDE_SCALE_Y_BF2TF_INIT_PHASE     0x0000007C /* Vertical Scaler DDA Bottom Field Source On Top Field Display Initial Phase Control */
#define VDE_CAP_TF_MSTART                0x00000080 /* Video Capture Buffer Top Field Memory Starting Address */
#define VDE_CAP_BF_MSTART                0x00000084 /* Video Capture Buffer Bottom Field Memory Starting Address */
#define VDE_PLBK_TF_MSTART               0x00000088 /* Video Playback Buffer Top Field Memory Starting Address */
#define VDE_PLBK_BF_MSTART               0x0000008C /* Video Playback Buffer Bottom Field Memory Starting Address */
#define VDE_CAP_PITCH                    0x00000090 /* Video Capture Buffer Memory Pitch */
#define VDE_PLBK_PITCH                   0x00000094 /* Video Playback Buffer Memory Pitch */
#define VDE_TF_RASTER                    0x00000098 /* CRTC Top Field Raster Compare */
#define VDE_BF_RASTER                    0x0000009C /* CRTC Bottom Field Raster Compare */
#define VDE_BG_COLOR                     0x000000A0 /* Background Color */
#define VDE_VWIN_ALPHA                   0x000000A4 /* Front Video Window Alpha */
#define VDE_MPEG_H_LENGTH                0x000000A8 /* MPEG Source Horizontal Length */
#define VDE_CAP_MEND                     0x000000AC /* Video Capture Buffer Memory Ending Address */
#define VDE_VDE_CAP_CTRL                 0x000000B0 /* Video Capture Control */
#define VDE_VDE_SCALE_CTRL               0x000000B4 /* Video Scaler Control */
#define VDE_VDE_DISP_CTRL                0x000000B8 /* Video Display Control */
#define VDE_SCRATCH_REG1                 0x000000BC /* Scratch Register */
#define VDE_CRTC_STATUS                  0x000000C0 /* CRTC Status */
#define VDE_VDE_REVISION                 0x000000C4 /* VDE Revision register */
#define VDE_CURRENT_CAP_MSTART           0x000000C8 /* Video Capture Buffer Memory Starting Address In Use */
#define VDE_CURRENT_PLBK_MSTART          0x000000CC /* Video Playback Buffer Memory Starting Address In Use */
#define VDE_DEBUG_PIXEL_OFFSET           0x000000D0 /* X/Y position for composite window pixel read back */
#define VDE_DEBUG_READ_BACK              0x000000D4 /* YUV color from composite window pixel read back */
#define VDE_VDE_FIFO_STATUS              0x000000DC /* VDE FIFO Status */
#define VDE_VCOEFF_PHASE0_01             0x000000E0 /* Vertical Scaler Filter Phase 0 Tap 0&1 Coefficient Registers */
#define VDE_VCOEFF_PHASE0_23             0x000000E4 /* Vertical Scaler Filter Phase 0 Tap 2&3 Coefficient Registers */
#define VDE_VCOEFF_PHASE1_01             0x000000E8 /* Vertical Scaler Filter Phase 1 Tap 0&1 Coefficient Registers */
#define VDE_VCOEFF_PHASE1_23             0x000000EC /* Vertical Scaler Filter Phase 1 Tap 2&3 Coefficient Registers */
#define VDE_VCOEFF_PHASE2_01             0x000000F0 /* Vertical Scaler Filter Phase 2 Tap 0&1 Coefficient Registers */
#define VDE_VCOEFF_PHASE2_23             0x000000F4 /* Vertical Scaler Filter Phase 2 Tap 2&3 Coefficient Registers */
#define VDE_VCOEFF_PHASE3_01             0x000000F8 /* Vertical Scaler Filter Phase 3 Tap 0&1 Coefficient Registers */
#define VDE_VCOEFF_PHASE3_23             0x000000FC /* Vertical Scaler Filter Phase 3 Tap 2&3 Coefficient Registers */
#define VDE_HCOEFF_PHASE0_01             0x00000100 /* Horizontal Scaler Filter Phase 0 Tap 0&1 Coefficient Registers */
#define VDE_HCOEFF_PHASE0_23             0x00000104 /* Horizontal Scaler Filter Phase 0 Tap 2&3 Coefficient Registers */
#define VDE_HCOEFF_PHASE0_45             0x00000108 /* Horizontal Scaler Filter Phase 0 Tap 4&5 Coefficient Registers */
#define VDE_HCOEFF_PHASE0_67             0x0000010C /* Horizontal Scaler Filter Phase 0 Tap 6&7 Coefficient Registers */
#define VDE_HCOEFF_PHASE1_01             0x00000110 /* Horizontal Scaler Filter Phase 1 Tap 0&1 Coefficient Registers */
#define VDE_HCOEFF_PHASE1_23             0x00000114 /* Horizontal Scaler Filter Phase 1 Tap 2&3 Coefficient Registers */
#define VDE_HCOEFF_PHASE1_45             0x00000118 /* Horizontal Scaler Filter Phase 1 Tap 4&5 Coefficient Registers */
#define VDE_HCOEFF_PHASE1_67             0x0000011C /* Horizontal Scaler Filter Phase 1 Tap 6&7 Coefficient Registers */
#define VDE_HCOEFF_PHASE2_01             0x00000120 /* Horizontal Scaler Filter Phase 2 Tap 0&1 Coefficient Registers */
#define VDE_HCOEFF_PHASE2_23             0x00000124 /* Horizontal Scaler Filter Phase 2 Tap 2&3 Coefficient Registers */
#define VDE_HCOEFF_PHASE2_45             0x00000128 /* Horizontal Scaler Filter Phase 2 Tap 4&5 Coefficient Registers */
#define VDE_HCOEFF_PHASE2_67             0x0000012C /* Horizontal Scaler Filter Phase 2 Tap 6&7 Coefficient Registers */
#define VDE_HCOEFF_PHASE3_01             0x00000130 /* Horizontal Scaler Filter Phase 3 Tap 0&1 Coefficient Registers */
#define VDE_HCOEFF_PHASE3_23             0x00000134 /* Horizontal Scaler Filter Phase 3 Tap 2&3 Coefficient Registers */
#define VDE_HCOEFF_PHASE3_45             0x00000138 /* Horizontal Scaler Filter Phase 3 Tap 4&5 Coefficient Registers */
#define VDE_HCOEFF_PHASE3_67             0x0000013C /* Horizontal Scaler Filter Phase 3 Tap 6&7 Coefficient Registers */
#define VDE_HCOEFF_PHASE4_01             0x00000140 /* Horizontal Scaler Filter Phase 4 Tap 0&1 Coefficient Registers */
#define VDE_HCOEFF_PHASE4_23             0x00000144 /* Horizontal Scaler Filter Phase 4 Tap 2&3 Coefficient Registers */
#define VDE_HCOEFF_PHASE4_45             0x00000148 /* Horizontal Scaler Filter Phase 4 Tap 4&5 Coefficient Registers */
#define VDE_HCOEFF_PHASE4_67             0x0000014C /* Horizontal Scaler Filter Phase 4 Tap 6&7 Coefficient Registers */
#define VDE_HCOEFF_PHASE5_01             0x00000150 /* Horizontal Scaler Filter Phase 5 Tap 0&1 Coefficient Registers */
#define VDE_HCOEFF_PHASE5_23             0x00000154 /* Horizontal Scaler Filter Phase 5 Tap 2&3 Coefficient Registers */
#define VDE_HCOEFF_PHASE5_45             0x00000158 /* Horizontal Scaler Filter Phase 5 Tap 4&5 Coefficient Registers */
#define VDE_HCOEFF_PHASE5_67             0x0000015C /* Horizontal Scaler Filter Phase 5 Tap 6&7 Coefficient Registers */
#define VDE_HCOEFF_PHASE6_01             0x00000160 /* Horizontal Scaler Filter Phase 6 Tap 0&1 Coefficient Registers */
#define VDE_HCOEFF_PHASE6_23             0x00000164 /* Horizontal Scaler Filter Phase 6 Tap 2&3 Coefficient Registers */
#define VDE_HCOEFF_PHASE6_45             0x00000168 /* Horizontal Scaler Filter Phase 6 Tap 4&5 Coefficient Registers */
#define VDE_HCOEFF_PHASE6_67             0x0000016C /* Horizontal Scaler Filter Phase 6 Tap 6&7 Coefficient Registers */
#define VDE_HCOEFF_PHASE7_01             0x00000170 /* Horizontal Scaler Filter Phase 7 Tap 0&1 Coefficient Registers */
#define VDE_HCOEFF_PHASE7_23             0x00000174 /* Horizontal Scaler Filter Phase 7 Tap 2&3 Coefficient Registers */
#define VDE_HCOEFF_PHASE7_45             0x00000178 /* Horizontal Scaler Filter Phase 7 Tap 4&5 Coefficient Registers */
#define VDE_HCOEFF_PHASE7_67             0x0000017C /* Horizontal Scaler Filter Phase 7 Tap 6&7 Coefficient Registers */

/**********************************************************************
 * BCM7115_A0 -- GDE_Registers
 **********************************************************************/
#define GDE_GDE_DISP_CTRL                0x00000400 /* Graphics display control */
#define GDE_ARC_SCALE_X                  0x00000404 /* Graphics aspect ratio converter scaling factor */
#define GDE_ARC_DDA_INIT_PHS             0x00000408 /* Graphics ARC DDA Initial Phase */
#define GDE_RGB_CHROMA_KEY_MIN           0x00000410 /* Graphics RGB chroma key lower bound */
#define GDE_RGB_CHROMA_KEY_MAX           0x00000414 /* Graphics RGB chroma key upper bound */
#define GDE_YUV_CHROMA_KEY_MIN           0x00000418 /* Graphics YUV chroma key lower bound */
#define GDE_YUV_CHROMA_KEY_MAX           0x0000041C /* Graphics YUV chroma key upper bound */
#define GDE_IND_CHROMA_KEY               0x00000420 /* Graphics CLUT index chroma key */
#define GDE_AFF_VSTART                   0x00000424 /* Graphics AFF Vertical Start */
#define GDE_AFF_VCOEF                    0x00000428 /* Graphics AFF Vertical Coefficient */
#define GDE_WD_MSTART                    0x00000430 /* Graphics WD memory starting address */
#define GDE_CURT_WD_MSTART               0x00000434 /* Graphics Current WD Memory starting address (Read Only) */
#define GDE_FIFO_STATUS                  0x00000438 /* Graphics FIFO underflow status (Read Only) */
#define GDE_REVISION                     0x0000043C /* GDE Revision (Read Only) */
#define GDE_GDE_SCRH_REG                 0x00000440 /* GDE scratch registers */
#define GDE_LCR_CTRL                     0x00000480 /* LCR configuration register */
#define GDE_LCR_HSTART                   0x00000484 /* LCR horizontal start */
#define GDE_LCR_HEND                     0x00000488 /* LCR horizontal end */
#define GDE_LCR_VSTART                   0x0000048C /* LCR vertical start */
#define GDE_LCR_VEND                     0x00000490 /* LCR vertical end */
#define GDE_LCR_HSIZE                    0x00000494 /* LCR horizontal source size */
#define GDE_LCR_HBLANK                   0x00000498 /* LCR horizontal blank pixel count */
#define GDE_LCR_ALPHA                    0x0000049C /* LCR alpha value */
#define GDE_LCR_MBASE                    0x000004A0 /* LCR memory starting address */
#define GDE_LCR_CMBASE                   0x000004A4 /* LCR CLUT memory starting address */
#define GDE_LCR_BMMSB                    0x000004A8 /* LCR bit map memory starting address */
#define GDE_HARC_C10_P0                  0x00000600 /* Horizontal ARC COEF1 and COEF0 of Phase0 */
#define GDE_HARC_C32_P0                  0x00000604 /* Horizontal ARC COEF3 and COEF2 of Phase0 */
#define GDE_HARC_C4_P0                   0x00000608 /* Horizontal ARC COEF4 of Phase0 */
#define GDE_HARC_C10_P1                  0x0000060C /* Horizontal ARC COEF1 and COEF0 of Phase1 */
#define GDE_HARC_C32_P1                  0x00000610 /* Horizontal ARC COEF3 and COEF2 of Phase1 */
#define GDE_HARC_C4_P1                   0x00000614 /* Horizontal ARC COEF4 of Phase1 */
#define GDE_HARC_C10_P2                  0x00000618 /* Horizontal ARC COEF1 and COEF0 of Phase2 */
#define GDE_HARC_C32_P2                  0x0000061C /* Horizontal ARC COEF3 and COEF2 of Phase2 */
#define GDE_HARC_C4_P2                   0x00000620 /* Horizontal ARC COEF4 of Phase2 */
#define GDE_HARC_C10_P3                  0x00000624 /* Horizontal ARC COEF1 and COEF0 of Phase3 */
#define GDE_HARC_C32_P3                  0x00000628 /* Horizontal ARC COEF3 and COEF2 of Phase3 */
#define GDE_HARC_C4_P3                   0x0000062C /* Horizontal ARC COEF4 of Phase3 */
#define GDE_HARC_C10_P4                  0x00000630 /* Horizontal ARC COEF1 and COEF0 of Phase4 */
#define GDE_HARC_C32_P4                  0x00000634 /* Horizontal ARC COEF3 and COEF2 of Phase4 */
#define GDE_HARC_C4_P4                   0x00000638 /* Horizontal ARC COEF4 of Phase4 */
#define GDE_HARC_C10_P5                  0x0000063C /* Horizontal ARC COEF1 and COEF0 of Phase5 */
#define GDE_HARC_C32_P5                  0x00000640 /* Horizontal ARC COEF3 and COEF2 of Phase5 */
#define GDE_HARC_C4_P5                   0x00000644 /* Horizontal ARC COEF4 of Phase5 */
#define GDE_HARC_C10_P6                  0x00000648 /* Horizontal ARC COEF1 and COEF0 of Phase6 */
#define GDE_HARC_C32_P6                  0x0000064C /* Horizontal ARC COEF3 and COEF2 of Phase6 */
#define GDE_HARC_C4_P6                   0x00000650 /* Horizontal ARC COEF4 of Phase6 */
#define GDE_HARC_C10_P7                  0x00000654 /* Horizontal ARC COEF1 and COEF0 of Phase7 */
#define GDE_HARC_C32_P7                  0x00000658 /* Horizontal ARC COEF3 and COEF2 of Phase7 */
#define GDE_HARC_C4_P7                   0x0000065C /* Horizontal ARC COEF4 of Phase7 */
#define GDE_HARC_C10_P8                  0x00000660 /* Horizontal ARC COEF1 and COEF0 of Phase8 */
#define GDE_HARC_C32_P8                  0x00000664 /* Horizontal ARC COEF3 and COEF2 of Phase8 */
#define GDE_HARC_C4_P8                   0x00000668 /* Horizontal ARC COEF4 of Phase8 */
#define GDE_HARC_C10_P9                  0x0000066C /* Horizontal ARC COEF1 and COEF0 of Phase9 */
#define GDE_HARC_C32_P9                  0x00000670 /* Horizontal ARC COEF3 and COEF2 of Phase9 */
#define GDE_HARC_C4_P9                   0x00000674 /* Horizontal ARC COEF4 of Phase9 */
#define GDE_HARC_C10_P10                 0x00000678 /* Horizontal ARC COEF1 and COEF0 of Phase10 */
#define GDE_HARC_C32_P10                 0x0000067C /* Horizontal ARC COEF3 and COEF2 of Phase10 */
#define GDE_HARC_C4_P10                  0x00000680 /* Horizontal ARC COEF4 of Phase10 */

/**********************************************************************
 * BCM7115_A0 -- MEM_DMA_Registers
 **********************************************************************/
#define MEM_DMA_CTRL                     0x00000F00 /* MEM_DMA Control Register */
#define MEM_DMA_CUR_DESC                 0x00000F04 /* MEM_DMA Current Descriptor Pointer Register */
#define MEM_DMA_CUR_BYTE                 0x00000F08 /* MEM_DMA Current Byte Count Register */
#define MEM_DMA_SCRATCH                  0x00000F0C /* MEM_DMA Scratch Register */
#define MEM_DMA_REVISION                 0x00000F10 /* MEM_DMA REVISION */

/**********************************************************************
 * BCM7115_A0 -- CCD_0_Registers
 **********************************************************************/
#define CCD_0_Control                    0x00001000 /* General Control Register */
#define CCD_0_Status                     0x00001004 /* Status Register */
#define CCD_0_Reset                      0x00001008 /* Soft Reset Register */
#define CCD_0_Top                        0x0000100C /* Top Field Control Register */
#define CCD_0_Bottom                     0x00001010 /* Bottom Field Control Register */
#define CCD_0_Data0                      0x00001014 /* Decoded Data Register 0 */
#define CCD_0_Data1                      0x00001018 /* Decoded Data Register 1 */
#define CCD_0_Data2                      0x0000101C /* Decoded Data Register 2 */
#define CCD_0_Data3                      0x00001020 /* Decoded Data Register 3 */
#define CCD_0_Data4                      0x00001024 /* Decoded Data Register 4 */
#define CCD_0_Data5                      0x00001028 /* Decoded Data Register 5 */
#define CCD_0_Lock                       0x0000102C /* Phase Lock Control Register */

/**********************************************************************
 * BCM7115_A0 -- CGMSAD_0_Registers
 **********************************************************************/
#define CGMSAD_0_Reset                   0x00001080 /* CGMS-A Caption Reset Register */
#define CGMSAD_0_Top_Control             0x00001084 /* CGMS-A Top-Field Control Register */
#define CGMSAD_0_Bot_Control             0x00001088 /* CGMS-A Bottom-Field Control Register */
#define CGMSAD_0_Top_Format              0x0000108C /* CGMS-A Top-Field Format Register */
#define CGMSAD_0_Bot_Format              0x00001090 /* CGMS-A Bottom-Field Format Register */
#define CGMSAD_0_Top_Data                0x00001094 /* CGMS-A Top-Field Data Register */
#define CGMSAD_0_Bot_Data                0x00001098 /* CGMS-A Bottom-Field Data Register */
#define CGMSAD_0_Status                  0x0000109C /* CGMS-A Status Register */

/**********************************************************************
 * BCM7115_A0 -- TTD_0_Registers
 **********************************************************************/
#define TTD_0_reset                      0x00001100 /* sw_reset Register */
#define TTD_0_status                     0x00001104 /* teletext_decoder_status Register */
#define TTD_0_signal_valid               0x00001108 /* teletext_decoder_status Register */
#define TTD_0_control                    0x0000110C /* teletext_decoder_control Register */
#define TTD_0_lock_control               0x00001110 /* teletext_decoder_lock_control Register */
#define TTD_0_shift_control              0x00001114 /* teletext_decoder_shift_control Register */
#define TTD_0_burst_control              0x00001118 /* teletext_decoder_shift_control Register */
#define TTD_0_write_address_top          0x0000111C /* Base Address of top Teletext Decoder buffer */
#define TTD_0_write_address_bottom       0x00001120 /* Base Address of bottom Teletext Decoder buffer */
#define TTD_0_subcarrier_freq            0x00001124 /* Subcarrier frequency register */
#define TTD_0_teletext_freq              0x00001128 /* Teletext frequency register */

/**********************************************************************
 * BCM7115_A0 -- WSSD_0_Registers
 **********************************************************************/
#define WSSD_0_Reset                     0x00001180 /* Wide Screen Signaling Decoder Reset Register */
#define WSSD_0_Control                   0x00001184 /* Wide Screen Signaling Decoder Control Register */
#define WSSD_0_Status                    0x00001188 /* Wide Screen Signaling Decoder Status Register */
#define WSSD_0_Threshold                 0x0000118C /* Wide Screen Signaling Decoder Threshold Register */

/**********************************************************************
 * BCM7115_A0 -- VDEC_0_Registers
 **********************************************************************/
#define VDEC_0_SW_Reset                  0x00001200 /* Software Reset register */
#define VDEC_0_Mode                      0x00001204 /* Video Decoder Mode register */
#define VDEC_0_Vertical_Blanking         0x00001208 /* vdec Vertical Blanking */
#define VDEC_0_Vertical_Clamping         0x0000120C /* vdec Vertical Clamping */
#define VDEC_0_Vertical_Sync             0x00001210 /* vdec Vertical Sync */
#define VDEC_0_Vertical_AGC              0x00001214 /* vdec Vertical Automatic Gain Control */
#define VDEC_0_Vertical_Misc1            0x00001218 /* vdec Vertical Miscellaneous 1 */
#define VDEC_0_Vertical_Misc2            0x0000121C /* vdec Vertical Miscellaneous 2 */
#define VDEC_0_Horizontal_Misc           0x00001220 /* vdec Horizontal Miscellaneous */
#define VDEC_0_SW_AGC                    0x00001224 /* vdec SW controlled AGC register */
#define VDEC_0_AUTO_AGC                  0x00001228 /* vdec Auto controlled AGC register */
#define VDEC_0_Sync_Luma_Blank           0x0000122C /* vdec sync, luma and blank levels read only register */
#define VDEC_0_Burst_Ampl                0x00001230 /* vdec color burst amplitude read only register */
#define VDEC_0_Clamp_Control             0x00001234 /* vdec clamp control register */
#define VDEC_0_Macrov_Setup              0x00001238 /* vdec macrovision setup register */
#define VDEC_0_YC_Separation             0x0000123C /* vdec yc separation register */
#define VDEC_0_Chroma_Suppression        0x00001240 /* vdec chroma suppression register */
#define VDEC_0_Picture_Control           0x00001244 /* vdec picture control register */
#define VDEC_0_Picture_Adjust            0x00001248 /* vdec picture adjust register */
#define VDEC_0_Subcarrier_Freq           0x0000124C /* vdec subcarrier frequency register */
#define VDEC_0_Luma_Filter_012           0x00001250 /* vdec luma filter of tap0, tap1, tap2 register */
#define VDEC_0_Luma_Filter_345           0x00001254 /* vdec luma filter of tap3, tap4, tap5 register */
#define VDEC_0_Luma_filter_6             0x00001258 /* vdec luma filter of tap6 register */
#define VDEC_0_Besrc_fifo                0x0000125C /* vdec backend sample rate converter fifo address register */
#define VDEC_0_Horizontal_Scaling        0x00001260 /* vdec horizontal scaling register */
#define VDEC_0_Acgc                      0x00001264 /* vdec automatic chroma gain control register */
#define VDEC_0_Chroma_lock               0x00001268 /* vdec chroma lock register */
#define VDEC_0_Line_lock                 0x0000126C /* vdec chroma lock register */
#define VDEC_0_Burst_Gate                0x00001270 /* color burst gate register */
#define VDEC_0_Vit_agc1                  0x00001274 /* vdec vertical interval agc 1 control register */
#define VDEC_0_Vit_agc2                  0x00001278 /* vdec vertical interval agc 2 control register */
#define VDEC_0_Trick_mode                0x0000127C /* vdec trick mode register */
#define VDEC_0_Vertical_Tilt             0x00001280 /* vdec trick mode register */
#define VDEC_0_Misc                      0x00001284 /* vdec misc. register */
#define VDEC_0_General_Control           0x00001288 /* vdec control register */
#define VDEC_0_General_Status            0x0000128C /* vdec status register */
#define VDEC_0_Anci656_Port_A            0x00001290 /* Ancillary 656 formatter Port_A register */
#define VDEC_0_Anci656_Port_B            0x00001294 /* Ancillary 656 formatter Port_B register */
#define VDEC_0_Anci656_Port_C            0x00001298 /* Ancillary 656 formatter Port_C register */
#define VDEC_0_Dither656                 0x0000129C /* Dithering 656 register */
#define VDEC_0_Test                      0x000012A0 /* TEST register */
#define VDEC_0_Revision_ID               0x000012A4 /* Video Decoder Mode register */
#define VDEC_0_VBI                       0x000012A8 /* Video Decoder Mode register */

/**********************************************************************
 * BCM7115_A0 -- CCE_Registers
 **********************************************************************/
#define CCE_Active_Lines                 0x00001400 /* Active Lines Register */
#define CCE_Control                      0x00001404 /* Control Register */
#define CCE_Gain_Delay                   0x00001408 /* Gain and Delay Register */
#define CCE_Soft_Reset                   0x0000140C /* Soft Reset Register */
#define CCE_Data0                        0x00001410 /* Data Register 0 */
#define CCE_Data1                        0x00001414 /* Data Register 1 */
#define CCE_Data2                        0x00001418 /* Data Register 2 */
#define CCE_Data3                        0x0000141C /* Data Register 3 */
#define CCE_Data4                        0x00001420 /* Data Register 4 */
#define CCE_Data5                        0x00001424 /* Data Register 5 */

/**********************************************************************
 * BCM7115_A0 -- CGMSAE_Registers
 **********************************************************************/
#define CGMSAE_Reset                     0x00001440 /* CGMS-A Caption Reset Register */
#define CGMSAE_Top_Control               0x00001444 /* CGMS-A Top-Field Control Register */
#define CGMSAE_Bot_Control               0x00001448 /* CGMS-A Bottom-Field Control Register */
#define CGMSAE_Top_Format                0x0000144C /* CGMS-A Top-Field Format Register */
#define CGMSAE_Bot_Format                0x00001450 /* CGMS-A Bottom-Field Format Register */
#define CGMSAE_Top_Data                  0x00001454 /* CGMS-A Top-Field Data Register */
#define CGMSAE_Bot_Data                  0x00001458 /* CGMS-A Bottom-Field Data Register */

/**********************************************************************
 * BCM7115_A0 -- VIDEO_ENC_Registers
 **********************************************************************/
#define VIDEO_ENC_RevID                  0x00001460 /* Video Encoder Revision ID */
#define VIDEO_ENC_Mode                   0x00001464 /* Video Encoder Glue Logic Data Output Modes */
#define VIDEO_ENC_PrimaryControl         0x00001468 /* VBI Control and Offset for Primary Data Output */
#define VIDEO_ENC_AuxillaryControl       0x0000146C /* VBI Control and Offset for Auxillary Data Output */

/**********************************************************************
 * BCM7115_A0 -- TTE_Registers
 **********************************************************************/
#define TTE_reset                        0x00001480 /* teletext_reset Register */
#define TTE_status                       0x00001484 /* teletext_status Register */
#define TTE_control                      0x00001488 /* teletext_control Register */
#define TTE_lines_active                 0x0000148C /* teletext_lines_active Register */
#define TTE_read_address_top             0x00001490 /* Base Address of top Teletext buffer */
#define TTE_read_address_bottom          0x00001494 /* Base Address of bottom Teletext buffer */
#define TTE_null_address                 0x00001498 /* Base Address of null buffer */
#define TTE_output_format                0x0000149C /* teletext_output_format Register */

/**********************************************************************
 * BCM7115_A0 -- CCIR656_ENC_Registers
 **********************************************************************/
#define CCIR656_ENC_SW_Reset             0x000014A0 /* Software Reset register */
#define CCIR656_ENC_Mode                 0x000014A4 /* ccir656_enc Mode register */
#define CCIR656_ENC_Vertical_Blanking    0x000014A8 /* ccir656_enc Vertical Blanking */
#define CCIR656_ENC_Vertical_Misc        0x000014AC /* ccir656_enc Vertical Misc */
#define CCIR656_ENC_Anci656_Port_A       0x000014B0 /* Ancillary 656 formatter Port_A register */
#define CCIR656_ENC_Anci656_Port_B       0x000014B4 /* Ancillary 656 formatter Port_B register */
#define CCIR656_ENC_Anci656_Port_C       0x000014B8 /* Ancillary 656 formatter Port_C register */

/**********************************************************************
 * BCM7115_A0 -- WSE_Registers
 **********************************************************************/
#define WSE_reset                        0x000014C0 /* WSS Reset Register */
#define WSE_control                      0x000014C4 /* WSS Control Register */
#define WSE_wss_data                     0x000014C8 /* WSS Data Register */

/**********************************************************************
 * BCM7115_A0 -- VEC_Registers
 **********************************************************************/
#define VEC_REVID                        0x00001500 /* VEC Macrocell Revision ID */
#define VEC_CONFIG0                      0x00001504 /* VEC Configuration 0 */
#define VEC_SCHPH                        0x00001508 /* VEC Color Subcarrier */
#define VEC_Soft_Reset                   0x0000150C /* VEC Soft Reset */
#define VEC_CPS01_CPS23                  0x00001520 /* Macrovision Control */
#define VEC_CPS45_CPS67                  0x00001524 /* Macrovision Control */
#define VEC_CPS89_CPS1011                0x00001528 /* Macrovision Control */
#define VEC_CPS1213_CPS1415              0x0000152C /* Macrovision Control */
#define VEC_CPS1617_CPS1819              0x00001530 /* Macrovision Control */
#define VEC_CPS2021_CPS2223              0x00001534 /* Macrovision Control */
#define VEC_CPS2425_CPS2627              0x00001538 /* Macrovision Control */
#define VEC_CPS2829_CPS3031              0x0000153C /* Macrovision Control */
#define VEC_CPS32_CPC                    0x00001540 /* Macrovision Control */
#define VEC_CLMP0_START                  0x00001544 /* Clamp0 Start Horizontal Count */
#define VEC_CLMP0_END                    0x00001548 /* Clamp0 End Horizontal Count */
#define VEC_CTRL27                       0x0000154C /* VCXO27 Control */
#define VEC_INT_COEF                     0x00001550 /* VCXO Loop Filter Integrating Coefficient */
#define VEC_LIN_COEF                     0x00001554 /* VCXO Loop Filter Linear Coefficient */
#define VEC_STCRI_SHADOW                 0x00001558 /* VCXO Loop Filter STCRI Shadow */
#define VEC_DELSIG_SHADOW                0x0000155C /* VCXO Loop Filter DELSIG Shadow */
#define VEC_STCRI                        0x00001560 /* VCXO Loop Filter Integrator Write Enable */
#define VEC_LDCRI                        0x00001564 /* VCXO Loop Filter Integrator Read Enable */
#define VEC_LDCRFO                       0x00001568 /* VCXO Loop Filter Frequency Offset Read Enable */
#define VEC_STDELSIG                     0x0000156C /* VCXO Loop Filter Sigma Delta Write Enable */
#define VEC_LDDELSIG                     0x00001570 /* VCXO Loop Filter Sigma Delta Read Enable */
#define VEC_TRANSFER_RD                  0x00001574 /* VCXO Loop Filter Snapshot Register Read */
#define VEC_LDPHASE                      0x00001578 /* VCXO Phase Detector Read Enable */
#define VEC_FREQ3_2                      0x00001580 /* Color Subcarrier Frequency MSB */
#define VEC_FREQ1_0                      0x00001584 /* Color Subcarrier Frequency LSB */
#define VEC_CONFIG1                      0x00001588 /* VEC Configuration 1 */
#define VEC_CONFIG2                      0x0000158C /* VEC Configuration 2 */
#define VEC_INTERRUPT_CONTROL            0x00001590 /* Control for Interrupt Signals */
#define VEC_INTERRUPT_STATUS             0x00001594 /* Status of Interrupt Signals */

/**********************************************************************
 *Enum: ANALOG_IN_SRC
 **********************************************************************/
#define VDE_ANALOG_IN_SRC_VDEC1                         0
#define VDE_ANALOG_IN_SRC_VDEC2                         1
#define VDE_ANALOG_IN_SRC_ITU_R656                      2

/**********************************************************************
 *Enum: ENABLE_DISABLE
 **********************************************************************/
#define VDE_ENABLE_DISABLE_ENABLE                       1
#define VDE_ENABLE_DISABLE_DISABLE                      0

/**********************************************************************
 *Enum: FIELD
 **********************************************************************/
#define VDE_FIELD_TOP                                   1
#define VDE_FIELD_BOTTOM                                0

/**********************************************************************
 *Enum: SD_VIDEO_FORMAT
 **********************************************************************/
#define VDE_SD_VIDEO_FORMAT_NTSC                        0
#define VDE_SD_VIDEO_FORMAT_PAL                         1

/**********************************************************************
 *Enum: VCXO_SRC
 **********************************************************************/
#define VDE_VCXO_SRC_VCXO1                              0
#define VDE_VCXO_SRC_VCXO2                              1

/**********************************************************************
 *Enum: VIDEO_CLOCK
 **********************************************************************/
#define VDE_VIDEO_CLOCK_X13P50MHz                       0
#define VDE_VIDEO_CLOCK_X27P00MHz                       1
#define VDE_VIDEO_CLOCK_X74P25MHz                       2

/**********************************************************************
 *Enum: VIDEO_FORMAT
 **********************************************************************/
#define VDE_VIDEO_FORMAT_D480I                          0
#define VDE_VIDEO_FORMAT_D480P                          1
#define VDE_VIDEO_FORMAT_D720P                          2
#define VDE_VIDEO_FORMAT_D1080I                         3

/**********************************************************************
 *Enum: VIDEO_IN_SRC
 **********************************************************************/
#define VDE_VIDEO_IN_SRC_SCALER                         0
#define VDE_VIDEO_IN_SRC_MPEG                           1
#define VDE_VIDEO_IN_SRC_ANALOG                         2
#define VDE_VIDEO_IN_SRC_PLAYBACK                       3

/**********************************************************************
 *Enum: VIDEO_SCAN_TYPE
 **********************************************************************/
#define VDE_VIDEO_SCAN_TYPE_INTERLACE                   0
#define VDE_VIDEO_SCAN_TYPE_PROGRESSIVE                 1

/**********************************************************************
 *Enum: YUV422_CHROMA_LOC
 **********************************************************************/
#define VDE_YUV422_CHROMA_LOC_CO_SITED_WITH_LUMA        0
#define VDE_YUV422_CHROMA_LOC_SHIFTED_BY_HALF_PIXEL     1

/**********************************************************************
 * VIDEO_ENC_RevID
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VIDEO_ENC_RevID_reserved0_MASK                  0xFFFF0000UL
#define VIDEO_ENC_RevID_reserved0_ALIGN                 0
#define VIDEO_ENC_RevID_reserved0_BITS                  16
#define VIDEO_ENC_RevID_reserved0_SHIFT                 16

/* REVID [15:00] - unsigned */
#define VIDEO_ENC_RevID_REVID_MASK                      0x0000FFFFUL
#define VIDEO_ENC_RevID_REVID_ALIGN                     0
#define VIDEO_ENC_RevID_REVID_BITS                      16
#define VIDEO_ENC_RevID_REVID_SHIFT                     0

/**********************************************************************
 * VIDEO_ENC_Mode
 **********************************************************************/
/* reserved0 [31:09] -  */
#define VIDEO_ENC_Mode_reserved0_MASK                   0xFFFFFE00UL
#define VIDEO_ENC_Mode_reserved0_ALIGN                  0
#define VIDEO_ENC_Mode_reserved0_BITS                   23
#define VIDEO_ENC_Mode_reserved0_SHIFT                  9

/* CSYNC [08:08] - boolean */
#define VIDEO_ENC_Mode_CSYNC_MASK                       0x00000100UL
#define VIDEO_ENC_Mode_CSYNC_ALIGN                      0
#define VIDEO_ENC_Mode_CSYNC_BITS                       1
#define VIDEO_ENC_Mode_CSYNC_SHIFT                      8
#define VIDEO_ENC_Mode_CSYNC_GPIO                       1
#define VIDEO_ENC_Mode_CSYNC_CCIR                       0

/* CCIR656 [07:07] - boolean */
#define VIDEO_ENC_Mode_CCIR656_MASK                     0x00000080UL
#define VIDEO_ENC_Mode_CCIR656_ALIGN                    0
#define VIDEO_ENC_Mode_CCIR656_BITS                     1
#define VIDEO_ENC_Mode_CCIR656_SHIFT                    7
#define VIDEO_ENC_Mode_CCIR656_WITH_GFX                 1
#define VIDEO_ENC_Mode_CCIR656_VIDEO_ONLY               0

/* AUXILLIARY [06:06] - boolean */
#define VIDEO_ENC_Mode_AUXILLIARY_MASK                  0x00000040UL
#define VIDEO_ENC_Mode_AUXILLIARY_ALIGN                 0
#define VIDEO_ENC_Mode_AUXILLIARY_BITS                  1
#define VIDEO_ENC_Mode_AUXILLIARY_SHIFT                 6
#define VIDEO_ENC_Mode_AUXILLIARY_WITH_GFX              1
#define VIDEO_ENC_Mode_AUXILLIARY_VIDEO_ONLY            0

/* PRIMARY [05:05] - boolean */
#define VIDEO_ENC_Mode_PRIMARY_MASK                     0x00000020UL
#define VIDEO_ENC_Mode_PRIMARY_ALIGN                    0
#define VIDEO_ENC_Mode_PRIMARY_BITS                     1
#define VIDEO_ENC_Mode_PRIMARY_SHIFT                    5
#define VIDEO_ENC_Mode_PRIMARY_WITH_GFX                 1
#define VIDEO_ENC_Mode_PRIMARY_VIDEO_ONLY               0

/* FILTER [04:03] - unsigned */
#define VIDEO_ENC_Mode_FILTER_MASK                      0x00000018UL
#define VIDEO_ENC_Mode_FILTER_ALIGN                     0
#define VIDEO_ENC_Mode_FILTER_BITS                      2
#define VIDEO_ENC_Mode_FILTER_SHIFT                     3
#define VIDEO_ENC_Mode_FILTER_F12221                    0
#define VIDEO_ENC_Mode_FILTER_F14641                    1
#define VIDEO_ENC_Mode_FILTER_DECIMATE                  2
#define VIDEO_ENC_Mode_FILTER_R                         3

/* ALIGN [02:01] - unsigned */
#define VIDEO_ENC_Mode_ALIGN_MASK                       0x00000006UL
#define VIDEO_ENC_Mode_ALIGN_ALIGN                      0
#define VIDEO_ENC_Mode_ALIGN_BITS                       2
#define VIDEO_ENC_Mode_ALIGN_SHIFT                      1
#define VIDEO_ENC_Mode_ALIGN_YCb0                       0
#define VIDEO_ENC_Mode_ALIGN_XCr0                       1
#define VIDEO_ENC_Mode_ALIGN_YCb1                       2
#define VIDEO_ENC_Mode_ALIGN_XCr1                       3

/* DAC_OUTPUT_PHASE [00:00] - boolean */
#define VIDEO_ENC_Mode_DAC_OUTPUT_PHASE_MASK            0x00000001UL
#define VIDEO_ENC_Mode_DAC_OUTPUT_PHASE_ALIGN           0
#define VIDEO_ENC_Mode_DAC_OUTPUT_PHASE_BITS            1
#define VIDEO_ENC_Mode_DAC_OUTPUT_PHASE_SHIFT           0
#define VIDEO_ENC_Mode_DAC_OUTPUT_PHASE_NEGATIVE_EDGE   1
#define VIDEO_ENC_Mode_DAC_OUTPUT_PHASE_POSITIVE_EDGE   0

/**********************************************************************
 * VIDEO_ENC_PrimaryControl
 **********************************************************************/
/* LUMA [31:24] - unsigned */
#define VIDEO_ENC_PrimaryControl_LUMA_MASK              0xFF000000UL
#define VIDEO_ENC_PrimaryControl_LUMA_ALIGN             0
#define VIDEO_ENC_PrimaryControl_LUMA_BITS              8
#define VIDEO_ENC_PrimaryControl_LUMA_SHIFT             24

/* COMP [23:16] - unsigned */
#define VIDEO_ENC_PrimaryControl_COMP_MASK              0x00FF0000UL
#define VIDEO_ENC_PrimaryControl_COMP_ALIGN             0
#define VIDEO_ENC_PrimaryControl_COMP_BITS              8
#define VIDEO_ENC_PrimaryControl_COMP_SHIFT             16

/* reserved0 [15:02] -  */
#define VIDEO_ENC_PrimaryControl_reserved0_MASK         0x0000FFFCUL
#define VIDEO_ENC_PrimaryControl_reserved0_ALIGN        0
#define VIDEO_ENC_PrimaryControl_reserved0_BITS         14
#define VIDEO_ENC_PrimaryControl_reserved0_SHIFT        2

/* VBI_DATA_ON_LUMA [01:01] - boolean */
#define VIDEO_ENC_PrimaryControl_VBI_DATA_ON_LUMA_MASK  0x00000002UL
#define VIDEO_ENC_PrimaryControl_VBI_DATA_ON_LUMA_ALIGN 0
#define VIDEO_ENC_PrimaryControl_VBI_DATA_ON_LUMA_BITS  1
#define VIDEO_ENC_PrimaryControl_VBI_DATA_ON_LUMA_SHIFT 1

/* VBI_DATA_ON_COMP [00:00] - boolean */
#define VIDEO_ENC_PrimaryControl_VBI_DATA_ON_COMP_MASK  0x00000001UL
#define VIDEO_ENC_PrimaryControl_VBI_DATA_ON_COMP_ALIGN 0
#define VIDEO_ENC_PrimaryControl_VBI_DATA_ON_COMP_BITS  1
#define VIDEO_ENC_PrimaryControl_VBI_DATA_ON_COMP_SHIFT 0

/* LUMA [31:24] - unsigned */
#define VIDEO_ENC_AuxillaryControl_LUMA_MASK            0xFF000000UL
#define VIDEO_ENC_AuxillaryControl_LUMA_ALIGN           0
#define VIDEO_ENC_AuxillaryControl_LUMA_BITS            8
#define VIDEO_ENC_AuxillaryControl_LUMA_SHIFT           24

/* COMP [23:16] - unsigned */
#define VIDEO_ENC_AuxillaryControl_COMP_MASK            0x00FF0000UL
#define VIDEO_ENC_AuxillaryControl_COMP_ALIGN           0
#define VIDEO_ENC_AuxillaryControl_COMP_BITS            8
#define VIDEO_ENC_AuxillaryControl_COMP_SHIFT           16

/* reserved1 [15:02] -  */
#define VIDEO_ENC_AuxillaryControl_reserved1_MASK       0x0000FFFCUL
#define VIDEO_ENC_AuxillaryControl_reserved1_ALIGN      0
#define VIDEO_ENC_AuxillaryControl_reserved1_BITS       14
#define VIDEO_ENC_AuxillaryControl_reserved1_SHIFT      2

/* VBI_DATA_ON_LUMA [01:01] - boolean */
#define VIDEO_ENC_AuxillaryControl_VBI_DATA_ON_LUMA_MASK 0x00000002UL
#define VIDEO_ENC_AuxillaryControl_VBI_DATA_ON_LUMA_ALIGN 0
#define VIDEO_ENC_AuxillaryControl_VBI_DATA_ON_LUMA_BITS 1
#define VIDEO_ENC_AuxillaryControl_VBI_DATA_ON_LUMA_SHIFT 1

/* VBI_DATA_ON_COMP [00:00] - boolean */
#define VIDEO_ENC_AuxillaryControl_VBI_DATA_ON_COMP_MASK 0x00000001UL
#define VIDEO_ENC_AuxillaryControl_VBI_DATA_ON_COMP_ALIGN 0
#define VIDEO_ENC_AuxillaryControl_VBI_DATA_ON_COMP_BITS 1
#define VIDEO_ENC_AuxillaryControl_VBI_DATA_ON_COMP_SHIFT 0

/**********************************************************************
 * VEC_REVID
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_REVID_reserved0_MASK                        0xFFFF0000UL
#define VEC_REVID_reserved0_ALIGN                       0
#define VEC_REVID_reserved0_BITS                        16
#define VEC_REVID_reserved0_SHIFT                       16

/* REVID [15:00] - unsigned */
#define VEC_REVID_REVID_MASK                            0x0000FFFFUL
#define VEC_REVID_REVID_ALIGN                           0
#define VEC_REVID_REVID_BITS                            16
#define VEC_REVID_REVID_SHIFT                           0

/**********************************************************************
 * VEC_CONFIG0
 **********************************************************************/
/* reserved0 [31:21] -  */
#define VEC_CONFIG0_reserved0_MASK                      0xFFE00000UL
#define VEC_CONFIG0_reserved0_ALIGN                     0
#define VEC_CONFIG0_reserved0_BITS                      11
#define VEC_CONFIG0_reserved0_SHIFT                     21

/* AUX_CHRBW1 [20:20] - unsigned */
#define VEC_CONFIG0_AUX_CHRBW1_MASK                     0x00100000UL
#define VEC_CONFIG0_AUX_CHRBW1_ALIGN                    0
#define VEC_CONFIG0_AUX_CHRBW1_BITS                     1
#define VEC_CONFIG0_AUX_CHRBW1_SHIFT                    20
#define VEC_CONFIG0_AUX_CHRBW1_BYPASS                   1
#define VEC_CONFIG0_AUX_CHRBW1_NO_BYPASS                0

/* AUX_CHRBW0 [19:19] - unsigned */
#define VEC_CONFIG0_AUX_CHRBW0_MASK                     0x00080000UL
#define VEC_CONFIG0_AUX_CHRBW0_ALIGN                    0
#define VEC_CONFIG0_AUX_CHRBW0_BITS                     1
#define VEC_CONFIG0_AUX_CHRBW0_SHIFT                    19
#define VEC_CONFIG0_AUX_CHRBW0_BW_0_65                  0
#define VEC_CONFIG0_AUX_CHRBW0_BW_1_30                  1

/* PBPR_FIL [18:18] - boolean */
#define VEC_CONFIG0_PBPR_FIL_MASK                       0x00040000UL
#define VEC_CONFIG0_PBPR_FIL_ALIGN                      0
#define VEC_CONFIG0_PBPR_FIL_BITS                       1
#define VEC_CONFIG0_PBPR_FIL_SHIFT                      18

/* CHROMA_GAIN [17:16] - unsigned */
#define VEC_CONFIG0_CHROMA_GAIN_MASK                    0x00030000UL
#define VEC_CONFIG0_CHROMA_GAIN_ALIGN                   0
#define VEC_CONFIG0_CHROMA_GAIN_BITS                    2
#define VEC_CONFIG0_CHROMA_GAIN_SHIFT                   16
#define VEC_CONFIG0_CHROMA_GAIN_UNITY_GAIN              0
#define VEC_CONFIG0_CHROMA_GAIN_one_plus_1_32           1
#define VEC_CONFIG0_CHROMA_GAIN_one_plus_1_16           2
#define VEC_CONFIG0_CHROMA_GAIN_one_plus_1_8            3

/* HB_SCAN [15:15] - boolean */
#define VEC_CONFIG0_HB_SCAN_MASK                        0x00008000UL
#define VEC_CONFIG0_HB_SCAN_ALIGN                       0
#define VEC_CONFIG0_HB_SCAN_BITS                        1
#define VEC_CONFIG0_HB_SCAN_SHIFT                       15

/* CBURST_GAIN [14:13] - unsigned */
#define VEC_CONFIG0_CBURST_GAIN_MASK                    0x00006000UL
#define VEC_CONFIG0_CBURST_GAIN_ALIGN                   0
#define VEC_CONFIG0_CBURST_GAIN_BITS                    2
#define VEC_CONFIG0_CBURST_GAIN_SHIFT                   13
#define VEC_CONFIG0_CBURST_GAIN_UNITY_GAIN              0
#define VEC_CONFIG0_CBURST_GAIN_one_plus_1_128          1
#define VEC_CONFIG0_CBURST_GAIN_one_plus_1_64           2
#define VEC_CONFIG0_CBURST_GAIN_one_plus_1_32           3

/* PEDEN1 [12:12] - boolean */
#define VEC_CONFIG0_PEDEN1_MASK                         0x00001000UL
#define VEC_CONFIG0_PEDEN1_ALIGN                        0
#define VEC_CONFIG0_PEDEN1_BITS                         1
#define VEC_CONFIG0_PEDEN1_SHIFT                        12

/* CHRBW1 [11:11] - unsigned */
#define VEC_CONFIG0_CHRBW1_MASK                         0x00000800UL
#define VEC_CONFIG0_CHRBW1_ALIGN                        0
#define VEC_CONFIG0_CHRBW1_BITS                         1
#define VEC_CONFIG0_CHRBW1_SHIFT                        11
#define VEC_CONFIG0_CHRBW1_BYPASS                       1
#define VEC_CONFIG0_CHRBW1_NO_BYPASS                    0

/* CHRBW0 [10:10] - unsigned */
#define VEC_CONFIG0_CHRBW0_MASK                         0x00000400UL
#define VEC_CONFIG0_CHRBW0_ALIGN                        0
#define VEC_CONFIG0_CHRBW0_BITS                         1
#define VEC_CONFIG0_CHRBW0_SHIFT                        10
#define VEC_CONFIG0_CHRBW0_BW_0_65                      0
#define VEC_CONFIG0_CHRBW0_BW_1_30                      1

/* SYNCDIS [09:09] - boolean */
#define VEC_CONFIG0_SYNCDIS_MASK                        0x00000200UL
#define VEC_CONFIG0_SYNCDIS_ALIGN                       0
#define VEC_CONFIG0_SYNCDIS_BITS                        1
#define VEC_CONFIG0_SYNCDIS_SHIFT                       9

/* BURDIS [08:08] - boolean */
#define VEC_CONFIG0_BURDIS_MASK                         0x00000100UL
#define VEC_CONFIG0_BURDIS_ALIGN                        0
#define VEC_CONFIG0_BURDIS_BITS                         1
#define VEC_CONFIG0_BURDIS_SHIFT                        8

/* CHRDIS [07:07] - boolean */
#define VEC_CONFIG0_CHRDIS_MASK                         0x00000080UL
#define VEC_CONFIG0_CHRDIS_ALIGN                        0
#define VEC_CONFIG0_CHRDIS_BITS                         1
#define VEC_CONFIG0_CHRDIS_SHIFT                        7

/* PEDEN [06:06] - boolean */
#define VEC_CONFIG0_PEDEN_MASK                          0x00000040UL
#define VEC_CONFIG0_PEDEN_ALIGN                         0
#define VEC_CONFIG0_PEDEN_BITS                          1
#define VEC_CONFIG0_PEDEN_SHIFT                         6

/* GENLKEN [05:05] - boolean */
#define VEC_CONFIG0_GENLKEN_MASK                        0x00000020UL
#define VEC_CONFIG0_GENLKEN_ALIGN                       0
#define VEC_CONFIG0_GENLKEN_BITS                        1
#define VEC_CONFIG0_GENLKEN_SHIFT                       5

/* YCDELAY [04:04] - boolean */
#define VEC_CONFIG0_YCDELAY_MASK                        0x00000010UL
#define VEC_CONFIG0_YCDELAY_ALIGN                       0
#define VEC_CONFIG0_YCDELAY_BITS                        1
#define VEC_CONFIG0_YCDELAY_SHIFT                       4

/* RAMPEN [03:03] - boolean */
#define VEC_CONFIG0_RAMPEN_MASK                         0x00000008UL
#define VEC_CONFIG0_RAMPEN_ALIGN                        0
#define VEC_CONFIG0_RAMPEN_BITS                         1
#define VEC_CONFIG0_RAMPEN_SHIFT                        3

/* YCDIS [02:02] - boolean */
#define VEC_CONFIG0_YCDIS_MASK                          0x00000004UL
#define VEC_CONFIG0_YCDIS_ALIGN                         0
#define VEC_CONFIG0_YCDIS_BITS                          1
#define VEC_CONFIG0_YCDIS_SHIFT                         2

/* STD [01:00] - unsigned */
#define VEC_CONFIG0_STD_MASK                            0x00000003UL
#define VEC_CONFIG0_STD_ALIGN                           0
#define VEC_CONFIG0_STD_BITS                            2
#define VEC_CONFIG0_STD_SHIFT                           0
#define VEC_CONFIG0_STD_NTSC                            0
#define VEC_CONFIG0_STD_PAL_BDGHI                       1
#define VEC_CONFIG0_STD_PAL_M                           2
#define VEC_CONFIG0_STD_PAL_N                           3

/**********************************************************************
 * VEC_SCHPH
 **********************************************************************/
/* reserved0 [31:08] -  */
#define VEC_SCHPH_reserved0_MASK                        0xFFFFFF00UL
#define VEC_SCHPH_reserved0_ALIGN                       0
#define VEC_SCHPH_reserved0_BITS                        24
#define VEC_SCHPH_reserved0_SHIFT                       8

/* SCHPH [07:00] - unsigned */
#define VEC_SCHPH_SCHPH_MASK                            0x000000FFUL
#define VEC_SCHPH_SCHPH_ALIGN                           0
#define VEC_SCHPH_SCHPH_BITS                            8
#define VEC_SCHPH_SCHPH_SHIFT                           0

/**********************************************************************
 * VEC_Soft_Reset
 **********************************************************************/
/* VEC_RESET [31:00] - unsigned */
#define VEC_Soft_Reset_VEC_RESET_MASK                   0xFFFFFFFFUL
#define VEC_Soft_Reset_VEC_RESET_ALIGN                  0
#define VEC_Soft_Reset_VEC_RESET_BITS                   32
#define VEC_Soft_Reset_VEC_RESET_SHIFT                  0

/**********************************************************************
 * VEC_CPS01_CPS23
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_CPS01_CPS23_reserved0_MASK                  0xFFFF0000UL
#define VEC_CPS01_CPS23_reserved0_ALIGN                 0
#define VEC_CPS01_CPS23_reserved0_BITS                  16
#define VEC_CPS01_CPS23_reserved0_SHIFT                 16

/* CPS0 [15:12] - unsigned */
#define VEC_CPS01_CPS23_CPS0_MASK                       0x0000F000UL
#define VEC_CPS01_CPS23_CPS0_ALIGN                      0
#define VEC_CPS01_CPS23_CPS0_BITS                       4
#define VEC_CPS01_CPS23_CPS0_SHIFT                      12

/* CPS1 [11:08] - unsigned */
#define VEC_CPS01_CPS23_CPS1_MASK                       0x00000F00UL
#define VEC_CPS01_CPS23_CPS1_ALIGN                      0
#define VEC_CPS01_CPS23_CPS1_BITS                       4
#define VEC_CPS01_CPS23_CPS1_SHIFT                      8

/* CPS2 [07:04] - unsigned */
#define VEC_CPS01_CPS23_CPS2_MASK                       0x000000F0UL
#define VEC_CPS01_CPS23_CPS2_ALIGN                      0
#define VEC_CPS01_CPS23_CPS2_BITS                       4
#define VEC_CPS01_CPS23_CPS2_SHIFT                      4

/* CPS3 [03:00] - unsigned */
#define VEC_CPS01_CPS23_CPS3_MASK                       0x0000000FUL
#define VEC_CPS01_CPS23_CPS3_ALIGN                      0
#define VEC_CPS01_CPS23_CPS3_BITS                       4
#define VEC_CPS01_CPS23_CPS3_SHIFT                      0

/**********************************************************************
 * VEC_CPS45_CPS67
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_CPS45_CPS67_reserved0_MASK                  0xFFFF0000UL
#define VEC_CPS45_CPS67_reserved0_ALIGN                 0
#define VEC_CPS45_CPS67_reserved0_BITS                  16
#define VEC_CPS45_CPS67_reserved0_SHIFT                 16

/* CPS4 [15:12] - unsigned */
#define VEC_CPS45_CPS67_CPS4_MASK                       0x0000F000UL
#define VEC_CPS45_CPS67_CPS4_ALIGN                      0
#define VEC_CPS45_CPS67_CPS4_BITS                       4
#define VEC_CPS45_CPS67_CPS4_SHIFT                      12

/* CPS5 [11:08] - unsigned */
#define VEC_CPS45_CPS67_CPS5_MASK                       0x00000F00UL
#define VEC_CPS45_CPS67_CPS5_ALIGN                      0
#define VEC_CPS45_CPS67_CPS5_BITS                       4
#define VEC_CPS45_CPS67_CPS5_SHIFT                      8

/* CPS6 [07:04] - unsigned */
#define VEC_CPS45_CPS67_CPS6_MASK                       0x000000F0UL
#define VEC_CPS45_CPS67_CPS6_ALIGN                      0
#define VEC_CPS45_CPS67_CPS6_BITS                       4
#define VEC_CPS45_CPS67_CPS6_SHIFT                      4

/* CPS7 [03:00] - unsigned */
#define VEC_CPS45_CPS67_CPS7_MASK                       0x0000000FUL
#define VEC_CPS45_CPS67_CPS7_ALIGN                      0
#define VEC_CPS45_CPS67_CPS7_BITS                       4
#define VEC_CPS45_CPS67_CPS7_SHIFT                      0

/**********************************************************************
 * VEC_CPS89_CPS1011
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_CPS89_CPS1011_reserved0_MASK                0xFFFF0000UL
#define VEC_CPS89_CPS1011_reserved0_ALIGN               0
#define VEC_CPS89_CPS1011_reserved0_BITS                16
#define VEC_CPS89_CPS1011_reserved0_SHIFT               16

/* CPS8 [15:12] - unsigned */
#define VEC_CPS89_CPS1011_CPS8_MASK                     0x0000F000UL
#define VEC_CPS89_CPS1011_CPS8_ALIGN                    0
#define VEC_CPS89_CPS1011_CPS8_BITS                     4
#define VEC_CPS89_CPS1011_CPS8_SHIFT                    12

/* CPS9 [11:08] - unsigned */
#define VEC_CPS89_CPS1011_CPS9_MASK                     0x00000F00UL
#define VEC_CPS89_CPS1011_CPS9_ALIGN                    0
#define VEC_CPS89_CPS1011_CPS9_BITS                     4
#define VEC_CPS89_CPS1011_CPS9_SHIFT                    8

/* CPS10 [07:04] - unsigned */
#define VEC_CPS89_CPS1011_CPS10_MASK                    0x000000F0UL
#define VEC_CPS89_CPS1011_CPS10_ALIGN                   0
#define VEC_CPS89_CPS1011_CPS10_BITS                    4
#define VEC_CPS89_CPS1011_CPS10_SHIFT                   4

/* CPS11 [03:00] - unsigned */
#define VEC_CPS89_CPS1011_CPS11_MASK                    0x0000000FUL
#define VEC_CPS89_CPS1011_CPS11_ALIGN                   0
#define VEC_CPS89_CPS1011_CPS11_BITS                    4
#define VEC_CPS89_CPS1011_CPS11_SHIFT                   0

/**********************************************************************
 * VEC_CPS1213_CPS1415
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_CPS1213_CPS1415_reserved0_MASK              0xFFFF0000UL
#define VEC_CPS1213_CPS1415_reserved0_ALIGN             0
#define VEC_CPS1213_CPS1415_reserved0_BITS              16
#define VEC_CPS1213_CPS1415_reserved0_SHIFT             16

/* CPS12 [15:12] - unsigned */
#define VEC_CPS1213_CPS1415_CPS12_MASK                  0x0000F000UL
#define VEC_CPS1213_CPS1415_CPS12_ALIGN                 0
#define VEC_CPS1213_CPS1415_CPS12_BITS                  4
#define VEC_CPS1213_CPS1415_CPS12_SHIFT                 12

/* CPS13 [11:08] - unsigned */
#define VEC_CPS1213_CPS1415_CPS13_MASK                  0x00000F00UL
#define VEC_CPS1213_CPS1415_CPS13_ALIGN                 0
#define VEC_CPS1213_CPS1415_CPS13_BITS                  4
#define VEC_CPS1213_CPS1415_CPS13_SHIFT                 8

/* CPS14 [07:04] - unsigned */
#define VEC_CPS1213_CPS1415_CPS14_MASK                  0x000000F0UL
#define VEC_CPS1213_CPS1415_CPS14_ALIGN                 0
#define VEC_CPS1213_CPS1415_CPS14_BITS                  4
#define VEC_CPS1213_CPS1415_CPS14_SHIFT                 4

/* CPS15 [03:00] - unsigned */
#define VEC_CPS1213_CPS1415_CPS15_MASK                  0x0000000FUL
#define VEC_CPS1213_CPS1415_CPS15_ALIGN                 0
#define VEC_CPS1213_CPS1415_CPS15_BITS                  4
#define VEC_CPS1213_CPS1415_CPS15_SHIFT                 0

/**********************************************************************
 * VEC_CPS1617_CPS1819
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_CPS1617_CPS1819_reserved0_MASK              0xFFFF0000UL
#define VEC_CPS1617_CPS1819_reserved0_ALIGN             0
#define VEC_CPS1617_CPS1819_reserved0_BITS              16
#define VEC_CPS1617_CPS1819_reserved0_SHIFT             16

/* CPS16 [15:12] - unsigned */
#define VEC_CPS1617_CPS1819_CPS16_MASK                  0x0000F000UL
#define VEC_CPS1617_CPS1819_CPS16_ALIGN                 0
#define VEC_CPS1617_CPS1819_CPS16_BITS                  4
#define VEC_CPS1617_CPS1819_CPS16_SHIFT                 12

/* CPS17 [11:08] - unsigned */
#define VEC_CPS1617_CPS1819_CPS17_MASK                  0x00000F00UL
#define VEC_CPS1617_CPS1819_CPS17_ALIGN                 0
#define VEC_CPS1617_CPS1819_CPS17_BITS                  4
#define VEC_CPS1617_CPS1819_CPS17_SHIFT                 8

/* CPS18 [07:04] - unsigned */
#define VEC_CPS1617_CPS1819_CPS18_MASK                  0x000000F0UL
#define VEC_CPS1617_CPS1819_CPS18_ALIGN                 0
#define VEC_CPS1617_CPS1819_CPS18_BITS                  4
#define VEC_CPS1617_CPS1819_CPS18_SHIFT                 4

/* CPS19 [03:00] - unsigned */
#define VEC_CPS1617_CPS1819_CPS19_MASK                  0x0000000FUL
#define VEC_CPS1617_CPS1819_CPS19_ALIGN                 0
#define VEC_CPS1617_CPS1819_CPS19_BITS                  4
#define VEC_CPS1617_CPS1819_CPS19_SHIFT                 0

/**********************************************************************
 * VEC_CPS2021_CPS2223
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_CPS2021_CPS2223_reserved0_MASK              0xFFFF0000UL
#define VEC_CPS2021_CPS2223_reserved0_ALIGN             0
#define VEC_CPS2021_CPS2223_reserved0_BITS              16
#define VEC_CPS2021_CPS2223_reserved0_SHIFT             16

/* CPS20 [15:12] - unsigned */
#define VEC_CPS2021_CPS2223_CPS20_MASK                  0x0000F000UL
#define VEC_CPS2021_CPS2223_CPS20_ALIGN                 0
#define VEC_CPS2021_CPS2223_CPS20_BITS                  4
#define VEC_CPS2021_CPS2223_CPS20_SHIFT                 12

/* CPS21 [11:08] - unsigned */
#define VEC_CPS2021_CPS2223_CPS21_MASK                  0x00000F00UL
#define VEC_CPS2021_CPS2223_CPS21_ALIGN                 0
#define VEC_CPS2021_CPS2223_CPS21_BITS                  4
#define VEC_CPS2021_CPS2223_CPS21_SHIFT                 8

/* CPS22 [07:04] - unsigned */
#define VEC_CPS2021_CPS2223_CPS22_MASK                  0x000000F0UL
#define VEC_CPS2021_CPS2223_CPS22_ALIGN                 0
#define VEC_CPS2021_CPS2223_CPS22_BITS                  4
#define VEC_CPS2021_CPS2223_CPS22_SHIFT                 4

/* CPS23 [03:00] - unsigned */
#define VEC_CPS2021_CPS2223_CPS23_MASK                  0x0000000FUL
#define VEC_CPS2021_CPS2223_CPS23_ALIGN                 0
#define VEC_CPS2021_CPS2223_CPS23_BITS                  4
#define VEC_CPS2021_CPS2223_CPS23_SHIFT                 0

/**********************************************************************
 * VEC_CPS2425_CPS2627
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_CPS2425_CPS2627_reserved0_MASK              0xFFFF0000UL
#define VEC_CPS2425_CPS2627_reserved0_ALIGN             0
#define VEC_CPS2425_CPS2627_reserved0_BITS              16
#define VEC_CPS2425_CPS2627_reserved0_SHIFT             16

/* CPS24 [15:12] - unsigned */
#define VEC_CPS2425_CPS2627_CPS24_MASK                  0x0000F000UL
#define VEC_CPS2425_CPS2627_CPS24_ALIGN                 0
#define VEC_CPS2425_CPS2627_CPS24_BITS                  4
#define VEC_CPS2425_CPS2627_CPS24_SHIFT                 12

/* CPS25 [11:08] - unsigned */
#define VEC_CPS2425_CPS2627_CPS25_MASK                  0x00000F00UL
#define VEC_CPS2425_CPS2627_CPS25_ALIGN                 0
#define VEC_CPS2425_CPS2627_CPS25_BITS                  4
#define VEC_CPS2425_CPS2627_CPS25_SHIFT                 8

/* CPS26 [07:04] - unsigned */
#define VEC_CPS2425_CPS2627_CPS26_MASK                  0x000000F0UL
#define VEC_CPS2425_CPS2627_CPS26_ALIGN                 0
#define VEC_CPS2425_CPS2627_CPS26_BITS                  4
#define VEC_CPS2425_CPS2627_CPS26_SHIFT                 4

/* CPS27 [03:00] - unsigned */
#define VEC_CPS2425_CPS2627_CPS27_MASK                  0x0000000FUL
#define VEC_CPS2425_CPS2627_CPS27_ALIGN                 0
#define VEC_CPS2425_CPS2627_CPS27_BITS                  4
#define VEC_CPS2425_CPS2627_CPS27_SHIFT                 0

/**********************************************************************
 * VEC_CPS2829_CPS3031
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_CPS2829_CPS3031_reserved0_MASK              0xFFFF0000UL
#define VEC_CPS2829_CPS3031_reserved0_ALIGN             0
#define VEC_CPS2829_CPS3031_reserved0_BITS              16
#define VEC_CPS2829_CPS3031_reserved0_SHIFT             16

/* CPS28 [15:12] - unsigned */
#define VEC_CPS2829_CPS3031_CPS28_MASK                  0x0000F000UL
#define VEC_CPS2829_CPS3031_CPS28_ALIGN                 0
#define VEC_CPS2829_CPS3031_CPS28_BITS                  4
#define VEC_CPS2829_CPS3031_CPS28_SHIFT                 12

/* CPS29 [11:08] - unsigned */
#define VEC_CPS2829_CPS3031_CPS29_MASK                  0x00000F00UL
#define VEC_CPS2829_CPS3031_CPS29_ALIGN                 0
#define VEC_CPS2829_CPS3031_CPS29_BITS                  4
#define VEC_CPS2829_CPS3031_CPS29_SHIFT                 8

/* CPS30 [07:04] - unsigned */
#define VEC_CPS2829_CPS3031_CPS30_MASK                  0x000000F0UL
#define VEC_CPS2829_CPS3031_CPS30_ALIGN                 0
#define VEC_CPS2829_CPS3031_CPS30_BITS                  4
#define VEC_CPS2829_CPS3031_CPS30_SHIFT                 4

/* CPS31 [03:00] - unsigned */
#define VEC_CPS2829_CPS3031_CPS31_MASK                  0x0000000FUL
#define VEC_CPS2829_CPS3031_CPS31_ALIGN                 0
#define VEC_CPS2829_CPS3031_CPS31_BITS                  4
#define VEC_CPS2829_CPS3031_CPS31_SHIFT                 0

/**********************************************************************
 * VEC_CPS32_CPC
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_CPS32_CPC_reserved0_MASK                    0xFFFF0000UL
#define VEC_CPS32_CPC_reserved0_ALIGN                   0
#define VEC_CPS32_CPC_reserved0_BITS                    16
#define VEC_CPS32_CPC_reserved0_SHIFT                   16

/* CPS32 [15:12] - unsigned */
#define VEC_CPS32_CPC_CPS32_MASK                        0x0000F000UL
#define VEC_CPS32_CPC_CPS32_ALIGN                       0
#define VEC_CPS32_CPC_CPS32_BITS                        4
#define VEC_CPS32_CPC_CPS32_SHIFT                       12

/* Unused [11:08] - unsigned */
#define VEC_CPS32_CPC_Unused_MASK                       0x00000F00UL
#define VEC_CPS32_CPC_Unused_ALIGN                      0
#define VEC_CPS32_CPC_Unused_BITS                       4
#define VEC_CPS32_CPC_Unused_SHIFT                      8

/* CPC0 [07:04] - unsigned */
#define VEC_CPS32_CPC_CPC0_MASK                         0x000000F0UL
#define VEC_CPS32_CPC_CPC0_ALIGN                        0
#define VEC_CPS32_CPC_CPC0_BITS                         4
#define VEC_CPS32_CPC_CPC0_SHIFT                        4

/* CPC1 [03:00] - unsigned */
#define VEC_CPS32_CPC_CPC1_MASK                         0x0000000FUL
#define VEC_CPS32_CPC_CPC1_ALIGN                        0
#define VEC_CPS32_CPC_CPC1_BITS                         4
#define VEC_CPS32_CPC_CPC1_SHIFT                        0

/**********************************************************************
 * VEC_CLMP0_START
 **********************************************************************/
/* reserved0 [31:11] -  */
#define VEC_CLMP0_START_reserved0_MASK                  0xFFFFF800UL
#define VEC_CLMP0_START_reserved0_ALIGN                 0
#define VEC_CLMP0_START_reserved0_BITS                  21
#define VEC_CLMP0_START_reserved0_SHIFT                 11

/* CLMP0_START [10:00] - unsigned */
#define VEC_CLMP0_START_CLMP0_START_MASK                0x000007FFUL
#define VEC_CLMP0_START_CLMP0_START_ALIGN               0
#define VEC_CLMP0_START_CLMP0_START_BITS                11
#define VEC_CLMP0_START_CLMP0_START_SHIFT               0

/**********************************************************************
 * VEC_CLMP0_END
 **********************************************************************/
/* reserved0 [31:11] -  */
#define VEC_CLMP0_END_reserved0_MASK                    0xFFFFF800UL
#define VEC_CLMP0_END_reserved0_ALIGN                   0
#define VEC_CLMP0_END_reserved0_BITS                    21
#define VEC_CLMP0_END_reserved0_SHIFT                   11

/* CLMP0_END [10:00] - unsigned */
#define VEC_CLMP0_END_CLMP0_END_MASK                    0x000007FFUL
#define VEC_CLMP0_END_CLMP0_END_ALIGN                   0
#define VEC_CLMP0_END_CLMP0_END_BITS                    11
#define VEC_CLMP0_END_CLMP0_END_SHIFT                   0

/**********************************************************************
 * VEC_CTRL27
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_CTRL27_reserved0_MASK                       0xFFFF0000UL
#define VEC_CTRL27_reserved0_ALIGN                      0
#define VEC_CTRL27_reserved0_BITS                       16
#define VEC_CTRL27_reserved0_SHIFT                      16

/* COMP_SYNC_SEL [15:15] - boolean */
#define VEC_CTRL27_COMP_SYNC_SEL_MASK                   0x00008000UL
#define VEC_CTRL27_COMP_SYNC_SEL_ALIGN                  0
#define VEC_CTRL27_COMP_SYNC_SEL_BITS                   1
#define VEC_CTRL27_COMP_SYNC_SEL_SHIFT                  15
#define VEC_CTRL27_COMP_SYNC_SEL_INTERNAL               1
#define VEC_CTRL27_COMP_SYNC_SEL_EXTERNAL               0

/* COMP_SYNC_INV [14:14] - boolean */
#define VEC_CTRL27_COMP_SYNC_INV_MASK                   0x00004000UL
#define VEC_CTRL27_COMP_SYNC_INV_ALIGN                  0
#define VEC_CTRL27_COMP_SYNC_INV_BITS                   1
#define VEC_CTRL27_COMP_SYNC_INV_SHIFT                  14
#define VEC_CTRL27_COMP_SYNC_INV_INVERT                 1
#define VEC_CTRL27_COMP_SYNC_INV_NO_INVERT              0

/* LP27_ENABLE [13:13] - boolean */
#define VEC_CTRL27_LP27_ENABLE_MASK                     0x00002000UL
#define VEC_CTRL27_LP27_ENABLE_ALIGN                    0
#define VEC_CTRL27_LP27_ENABLE_BITS                     1
#define VEC_CTRL27_LP27_ENABLE_SHIFT                    13
#define VEC_CTRL27_LP27_ENABLE_ENABLE                   1
#define VEC_CTRL27_LP27_ENABLE_DISABLE                  0

/* PAL_NTSC [12:12] - boolean */
#define VEC_CTRL27_PAL_NTSC_MASK                        0x00001000UL
#define VEC_CTRL27_PAL_NTSC_ALIGN                       0
#define VEC_CTRL27_PAL_NTSC_BITS                        1
#define VEC_CTRL27_PAL_NTSC_SHIFT                       12
#define VEC_CTRL27_PAL_NTSC_NTSC                        1
#define VEC_CTRL27_PAL_NTSC_PAL                         0

/* Unused [11:10] - unsigned */
#define VEC_CTRL27_Unused_MASK                          0x00000C00UL
#define VEC_CTRL27_Unused_ALIGN                         0
#define VEC_CTRL27_Unused_BITS                          2
#define VEC_CTRL27_Unused_SHIFT                         10

/* RESET_PD [09:09] - boolean */
#define VEC_CTRL27_RESET_PD_MASK                        0x00000200UL
#define VEC_CTRL27_RESET_PD_ALIGN                       0
#define VEC_CTRL27_RESET_PD_BITS                        1
#define VEC_CTRL27_RESET_PD_SHIFT                       9
#define VEC_CTRL27_RESET_PD_RESET                       1
#define VEC_CTRL27_RESET_PD_NORMAL                      0

/* SIGMA_DEL_INV [08:08] - boolean */
#define VEC_CTRL27_SIGMA_DEL_INV_MASK                   0x00000100UL
#define VEC_CTRL27_SIGMA_DEL_INV_ALIGN                  0
#define VEC_CTRL27_SIGMA_DEL_INV_BITS                   1
#define VEC_CTRL27_SIGMA_DEL_INV_SHIFT                  8
#define VEC_CTRL27_SIGMA_DEL_INV_INVERT                 1
#define VEC_CTRL27_SIGMA_DEL_INV_NO_INVERT              0

/* LP27_RST [07:07] - boolean */
#define VEC_CTRL27_LP27_RST_MASK                        0x00000080UL
#define VEC_CTRL27_LP27_RST_ALIGN                       0
#define VEC_CTRL27_LP27_RST_BITS                        1
#define VEC_CTRL27_LP27_RST_SHIFT                       7
#define VEC_CTRL27_LP27_RST_RESET                       1
#define VEC_CTRL27_LP27_RST_NORMAL                      0

/* LP27_SCALER [06:05] - unsigned */
#define VEC_CTRL27_LP27_SCALER_MASK                     0x00000060UL
#define VEC_CTRL27_LP27_SCALER_ALIGN                    0
#define VEC_CTRL27_LP27_SCALER_BITS                     2
#define VEC_CTRL27_LP27_SCALER_SHIFT                    5
#define VEC_CTRL27_LP27_SCALER_full_scale               0
#define VEC_CTRL27_LP27_SCALER_scale_1_by_4             1
#define VEC_CTRL27_LP27_SCALER_scale_1_by_8             2
#define VEC_CTRL27_LP27_SCALER_scale_1_by_2             3

/* LP27_TEST_PHASE [04:03] - unsigned */
#define VEC_CTRL27_LP27_TEST_PHASE_MASK                 0x00000018UL
#define VEC_CTRL27_LP27_TEST_PHASE_ALIGN                0
#define VEC_CTRL27_LP27_TEST_PHASE_BITS                 2
#define VEC_CTRL27_LP27_TEST_PHASE_SHIFT                3
#define VEC_CTRL27_LP27_TEST_PHASE_LEADING              0
#define VEC_CTRL27_LP27_TEST_PHASE_MID                  1
#define VEC_CTRL27_LP27_TEST_PHASE_LAGGING              2
#define VEC_CTRL27_LP27_TEST_PHASE_ZERO                 3

/* USE_TEST_PHASE [02:02] - boolean */
#define VEC_CTRL27_USE_TEST_PHASE_MASK                  0x00000004UL
#define VEC_CTRL27_USE_TEST_PHASE_ALIGN                 0
#define VEC_CTRL27_USE_TEST_PHASE_BITS                  1
#define VEC_CTRL27_USE_TEST_PHASE_SHIFT                 2
#define VEC_CTRL27_USE_TEST_PHASE_USE_TEST_PHASE        1
#define VEC_CTRL27_USE_TEST_PHASE_NORMAL                0

/* LP27_FREEZE [01:01] - boolean */
#define VEC_CTRL27_LP27_FREEZE_MASK                     0x00000002UL
#define VEC_CTRL27_LP27_FREEZE_ALIGN                    0
#define VEC_CTRL27_LP27_FREEZE_BITS                     1
#define VEC_CTRL27_LP27_FREEZE_SHIFT                    1
#define VEC_CTRL27_LP27_FREEZE_FREEZE                   1
#define VEC_CTRL27_LP27_FREEZE_NORMAL                   0

/* NEW_LOOP [00:00] - boolean */
#define VEC_CTRL27_NEW_LOOP_MASK                        0x00000001UL
#define VEC_CTRL27_NEW_LOOP_ALIGN                       0
#define VEC_CTRL27_NEW_LOOP_BITS                        1
#define VEC_CTRL27_NEW_LOOP_SHIFT                       0
#define VEC_CTRL27_NEW_LOOP_PHASE_9                     1
#define VEC_CTRL27_NEW_LOOP_PHASE_2                     0

/**********************************************************************
 * VEC_INT_COEF
 **********************************************************************/
/* reserved0 [31:20] -  */
#define VEC_INT_COEF_reserved0_MASK                     0xFFF00000UL
#define VEC_INT_COEF_reserved0_ALIGN                    0
#define VEC_INT_COEF_reserved0_BITS                     12
#define VEC_INT_COEF_reserved0_SHIFT                    20

/* INT_COEF [19:00] - unsigned */
#define VEC_INT_COEF_INT_COEF_MASK                      0x000FFFFFUL
#define VEC_INT_COEF_INT_COEF_ALIGN                     0
#define VEC_INT_COEF_INT_COEF_BITS                      20
#define VEC_INT_COEF_INT_COEF_SHIFT                     0

/**********************************************************************
 * VEC_LIN_COEF
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_LIN_COEF_reserved0_MASK                     0xFFFF0000UL
#define VEC_LIN_COEF_reserved0_ALIGN                    0
#define VEC_LIN_COEF_reserved0_BITS                     16
#define VEC_LIN_COEF_reserved0_SHIFT                    16

/* LIN_COEF [15:00] - unsigned */
#define VEC_LIN_COEF_LIN_COEF_MASK                      0x0000FFFFUL
#define VEC_LIN_COEF_LIN_COEF_ALIGN                     0
#define VEC_LIN_COEF_LIN_COEF_BITS                      16
#define VEC_LIN_COEF_LIN_COEF_SHIFT                     0

/**********************************************************************
 * VEC_STCRI_SHADOW
 **********************************************************************/
/* reserved0 [31:24] -  */
#define VEC_STCRI_SHADOW_reserved0_MASK                 0xFF000000UL
#define VEC_STCRI_SHADOW_reserved0_ALIGN                0
#define VEC_STCRI_SHADOW_reserved0_BITS                 8
#define VEC_STCRI_SHADOW_reserved0_SHIFT                24

/* STCRI_SHADOW [23:00] - unsigned */
#define VEC_STCRI_SHADOW_STCRI_SHADOW_MASK              0x00FFFFFFUL
#define VEC_STCRI_SHADOW_STCRI_SHADOW_ALIGN             0
#define VEC_STCRI_SHADOW_STCRI_SHADOW_BITS              24
#define VEC_STCRI_SHADOW_STCRI_SHADOW_SHIFT             0

/**********************************************************************
 * VEC_DELSIG_SHADOW
 **********************************************************************/
/* reserved0 [31:24] -  */
#define VEC_DELSIG_SHADOW_reserved0_MASK                0xFF000000UL
#define VEC_DELSIG_SHADOW_reserved0_ALIGN               0
#define VEC_DELSIG_SHADOW_reserved0_BITS                8
#define VEC_DELSIG_SHADOW_reserved0_SHIFT               24

/* DELSIG_SHADOW [23:00] - unsigned */
#define VEC_DELSIG_SHADOW_DELSIG_SHADOW_MASK            0x00FFFFFFUL
#define VEC_DELSIG_SHADOW_DELSIG_SHADOW_ALIGN           0
#define VEC_DELSIG_SHADOW_DELSIG_SHADOW_BITS            24
#define VEC_DELSIG_SHADOW_DELSIG_SHADOW_SHIFT           0

/**********************************************************************
 * VEC_STCRI
 **********************************************************************/
/* STCRI [31:00] - unsigned */
#define VEC_STCRI_STCRI_MASK                            0xFFFFFFFFUL
#define VEC_STCRI_STCRI_ALIGN                           0
#define VEC_STCRI_STCRI_BITS                            32
#define VEC_STCRI_STCRI_SHIFT                           0

/**********************************************************************
 * VEC_LDCRI
 **********************************************************************/
/* LDCRI [31:00] - unsigned */
#define VEC_LDCRI_LDCRI_MASK                            0xFFFFFFFFUL
#define VEC_LDCRI_LDCRI_ALIGN                           0
#define VEC_LDCRI_LDCRI_BITS                            32
#define VEC_LDCRI_LDCRI_SHIFT                           0

/**********************************************************************
 * VEC_LDCRFO
 **********************************************************************/
/* LDCRFO [31:00] - unsigned */
#define VEC_LDCRFO_LDCRFO_MASK                          0xFFFFFFFFUL
#define VEC_LDCRFO_LDCRFO_ALIGN                         0
#define VEC_LDCRFO_LDCRFO_BITS                          32
#define VEC_LDCRFO_LDCRFO_SHIFT                         0

/**********************************************************************
 * VEC_STDELSIG
 **********************************************************************/
/* STDELSIG [31:00] - unsigned */
#define VEC_STDELSIG_STDELSIG_MASK                      0xFFFFFFFFUL
#define VEC_STDELSIG_STDELSIG_ALIGN                     0
#define VEC_STDELSIG_STDELSIG_BITS                      32
#define VEC_STDELSIG_STDELSIG_SHIFT                     0

/**********************************************************************
 * VEC_LDDELSIG
 **********************************************************************/
/* LDDELSIG [31:00] - unsigned */
#define VEC_LDDELSIG_LDDELSIG_MASK                      0xFFFFFFFFUL
#define VEC_LDDELSIG_LDDELSIG_ALIGN                     0
#define VEC_LDDELSIG_LDDELSIG_BITS                      32
#define VEC_LDDELSIG_LDDELSIG_SHIFT                     0

/**********************************************************************
 * VEC_TRANSFER_RD
 **********************************************************************/
/* reserved0 [31:24] -  */
#define VEC_TRANSFER_RD_reserved0_MASK                  0xFF000000UL
#define VEC_TRANSFER_RD_reserved0_ALIGN                 0
#define VEC_TRANSFER_RD_reserved0_BITS                  8
#define VEC_TRANSFER_RD_reserved0_SHIFT                 24

/* TRANSFER [23:00] - unsigned */
#define VEC_TRANSFER_RD_TRANSFER_MASK                   0x00FFFFFFUL
#define VEC_TRANSFER_RD_TRANSFER_ALIGN                  0
#define VEC_TRANSFER_RD_TRANSFER_BITS                   24
#define VEC_TRANSFER_RD_TRANSFER_SHIFT                  0

/**********************************************************************
 * VEC_LDPHASE
 **********************************************************************/
/* reserved0 [31:09] -  */
#define VEC_LDPHASE_reserved0_MASK                      0xFFFFFE00UL
#define VEC_LDPHASE_reserved0_ALIGN                     0
#define VEC_LDPHASE_reserved0_BITS                      23
#define VEC_LDPHASE_reserved0_SHIFT                     9

/* LDPHASE [08:00] - unsigned */
#define VEC_LDPHASE_LDPHASE_MASK                        0x000001FFUL
#define VEC_LDPHASE_LDPHASE_ALIGN                       0
#define VEC_LDPHASE_LDPHASE_BITS                        9
#define VEC_LDPHASE_LDPHASE_SHIFT                       0

/**********************************************************************
 * VEC_FREQ3_2
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_FREQ3_2_reserved0_MASK                      0xFFFF0000UL
#define VEC_FREQ3_2_reserved0_ALIGN                     0
#define VEC_FREQ3_2_reserved0_BITS                      16
#define VEC_FREQ3_2_reserved0_SHIFT                     16

/* FREQ3_2 [15:00] - unsigned */
#define VEC_FREQ3_2_FREQ3_2_MASK                        0x0000FFFFUL
#define VEC_FREQ3_2_FREQ3_2_ALIGN                       0
#define VEC_FREQ3_2_FREQ3_2_BITS                        16
#define VEC_FREQ3_2_FREQ3_2_SHIFT                       0

/**********************************************************************
 * VEC_FREQ1_0
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_FREQ1_0_reserved0_MASK                      0xFFFF0000UL
#define VEC_FREQ1_0_reserved0_ALIGN                     0
#define VEC_FREQ1_0_reserved0_BITS                      16
#define VEC_FREQ1_0_reserved0_SHIFT                     16

/* FREQ1_0 [15:00] - unsigned */
#define VEC_FREQ1_0_FREQ1_0_MASK                        0x0000FFFFUL
#define VEC_FREQ1_0_FREQ1_0_ALIGN                       0
#define VEC_FREQ1_0_FREQ1_0_BITS                        16
#define VEC_FREQ1_0_FREQ1_0_SHIFT                       0

/**********************************************************************
 * VEC_CONFIG1
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_CONFIG1_reserved0_MASK                      0xFFFF0000UL
#define VEC_CONFIG1_reserved0_ALIGN                     0
#define VEC_CONFIG1_reserved0_BITS                      16
#define VEC_CONFIG1_reserved0_SHIFT                     16

/* TC_OBB [15:15] - boolean */
#define VEC_CONFIG1_TC_OBB_MASK                         0x00008000UL
#define VEC_CONFIG1_TC_OBB_ALIGN                        0
#define VEC_CONFIG1_TC_OBB_BITS                         1
#define VEC_CONFIG1_TC_OBB_SHIFT                        15
#define VEC_CONFIG1_TC_OBB_OFFSET_BINARY                0
#define VEC_CONFIG1_TC_OBB_TWOS_COMPLEMENT              1

/* RF_SEL [14:14] - boolean */
#define VEC_CONFIG1_RF_SEL_MASK                         0x00004000UL
#define VEC_CONFIG1_RF_SEL_ALIGN                        0
#define VEC_CONFIG1_RF_SEL_BITS                         1
#define VEC_CONFIG1_RF_SEL_SHIFT                        14
#define VEC_CONFIG1_RF_SEL_RF_MOD_AUX_PORT              0
#define VEC_CONFIG1_RF_SEL_RF_MOD_PRI_PORT              1

/* FOUR_DAC_SEL [13:13] - boolean */
#define VEC_CONFIG1_FOUR_DAC_SEL_MASK                   0x00002000UL
#define VEC_CONFIG1_FOUR_DAC_SEL_ALIGN                  0
#define VEC_CONFIG1_FOUR_DAC_SEL_BITS                   1
#define VEC_CONFIG1_FOUR_DAC_SEL_SHIFT                  13
#define VEC_CONFIG1_FOUR_DAC_SEL_CVBS_FOURTH_DAC        0
#define VEC_CONFIG1_FOUR_DAC_SEL_Y_C_FOURTH_DAC         1

/* OUTPUT_MODE [12:10] - unsigned */
#define VEC_CONFIG1_OUTPUT_MODE_MASK                    0x00001C00UL
#define VEC_CONFIG1_OUTPUT_MODE_ALIGN                   0
#define VEC_CONFIG1_OUTPUT_MODE_BITS                    3
#define VEC_CONFIG1_OUTPUT_MODE_SHIFT                   10
#define VEC_CONFIG1_OUTPUT_MODE_C_Y_CVBS_PRI_R1G1B1_AUX 0
#define VEC_CONFIG1_OUTPUT_MODE_C_Y_CVBS_PRI_V1Y1U1_AUX 1
#define VEC_CONFIG1_OUTPUT_MODE_V1Y1U1_PRI_R1G1B1_AUX   2
#define VEC_CONFIG1_OUTPUT_MODE_C_Y_CVBS_PRI_C1_Y1_CVBS1_AUX 3
#define VEC_CONFIG1_OUTPUT_MODE_C_Y_CVBS1_PRI_R1G1B1_AUX 4
#define VEC_CONFIG1_OUTPUT_MODE_CVBS_CVBS_Y1_PRI_C1_CVBS1_CVBS1_AUX 5
#define VEC_CONFIG1_OUTPUT_MODE_C_Y_Y1_PRI_C1_CVBS1_CVBS1_AUX 6
#define VEC_CONFIG1_OUTPUT_MODE_CVBS_CVBS_CVBS1_PRI_R1G1B1_AUX 7

/* DIS_CHR [09:09] - boolean */
#define VEC_CONFIG1_DIS_CHR_MASK                        0x00000200UL
#define VEC_CONFIG1_DIS_CHR_ALIGN                       0
#define VEC_CONFIG1_DIS_CHR_BITS                        1
#define VEC_CONFIG1_DIS_CHR_SHIFT                       9

/* DIS_LUMA [08:08] - boolean */
#define VEC_CONFIG1_DIS_LUMA_MASK                       0x00000100UL
#define VEC_CONFIG1_DIS_LUMA_ALIGN                      0
#define VEC_CONFIG1_DIS_LUMA_BITS                       1
#define VEC_CONFIG1_DIS_LUMA_SHIFT                      8

/* PH_IN [07:07] - boolean */
#define VEC_CONFIG1_PH_IN_MASK                          0x00000080UL
#define VEC_CONFIG1_PH_IN_ALIGN                         0
#define VEC_CONFIG1_PH_IN_BITS                          1
#define VEC_CONFIG1_PH_IN_SHIFT                         7
#define VEC_CONFIG1_PH_IN_RISING_EDGE                   0
#define VEC_CONFIG1_PH_IN_FALLING_EDGE                  1

/* PH_OUT [06:06] - boolean */
#define VEC_CONFIG1_PH_OUT_MASK                         0x00000040UL
#define VEC_CONFIG1_PH_OUT_ALIGN                        0
#define VEC_CONFIG1_PH_OUT_BITS                         1
#define VEC_CONFIG1_PH_OUT_SHIFT                        6
#define VEC_CONFIG1_PH_OUT_RISING_EDGE                  0
#define VEC_CONFIG1_PH_OUT_FALLING_EDGE                 1

/* DITHER_TYPE_SEL [05:05] - boolean */
#define VEC_CONFIG1_DITHER_TYPE_SEL_MASK                0x00000020UL
#define VEC_CONFIG1_DITHER_TYPE_SEL_ALIGN               0
#define VEC_CONFIG1_DITHER_TYPE_SEL_BITS                1
#define VEC_CONFIG1_DITHER_TYPE_SEL_SHIFT               5
#define VEC_CONFIG1_DITHER_TYPE_SEL_LFSR                0
#define VEC_CONFIG1_DITHER_TYPE_SEL_COUNTER             1

/* DITHER_ENABLE [04:04] - boolean */
#define VEC_CONFIG1_DITHER_ENABLE_MASK                  0x00000010UL
#define VEC_CONFIG1_DITHER_ENABLE_ALIGN                 0
#define VEC_CONFIG1_DITHER_ENABLE_BITS                  1
#define VEC_CONFIG1_DITHER_ENABLE_SHIFT                 4
#define VEC_CONFIG1_DITHER_ENABLE_DITHER_OFF            0
#define VEC_CONFIG1_DITHER_ENABLE_DITHER_ON             1

/* CYDELAY [03:03] - boolean */
#define VEC_CONFIG1_CYDELAY_MASK                        0x00000008UL
#define VEC_CONFIG1_CYDELAY_ALIGN                       0
#define VEC_CONFIG1_CYDELAY_BITS                        1
#define VEC_CONFIG1_CYDELAY_SHIFT                       3

/* LUMADIS [02:02] - boolean */
#define VEC_CONFIG1_LUMADIS_MASK                        0x00000004UL
#define VEC_CONFIG1_LUMADIS_ALIGN                       0
#define VEC_CONFIG1_LUMADIS_BITS                        1
#define VEC_CONFIG1_LUMADIS_SHIFT                       2

/* COMPDIS [01:01] - boolean */
#define VEC_CONFIG1_COMPDIS_MASK                        0x00000002UL
#define VEC_CONFIG1_COMPDIS_ALIGN                       0
#define VEC_CONFIG1_COMPDIS_BITS                        1
#define VEC_CONFIG1_COMPDIS_SHIFT                       1

/* STD2 [00:00] - boolean */
#define VEC_CONFIG1_STD2_MASK                           0x00000001UL
#define VEC_CONFIG1_STD2_ALIGN                          0
#define VEC_CONFIG1_STD2_BITS                           1
#define VEC_CONFIG1_STD2_SHIFT                          0
#define VEC_CONFIG1_STD2_STANDARD_VALUE                 0
#define VEC_CONFIG1_STD2_PROGRAMMED_VALUE               1

/**********************************************************************
 * VEC_CONFIG2
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VEC_CONFIG2_reserved0_MASK                      0xFFFF0000UL
#define VEC_CONFIG2_reserved0_ALIGN                     0
#define VEC_CONFIG2_reserved0_BITS                      16
#define VEC_CONFIG2_reserved0_SHIFT                     16

/* PROG_SCAN [15:15] - boolean */
#define VEC_CONFIG2_PROG_SCAN_MASK                      0x00008000UL
#define VEC_CONFIG2_PROG_SCAN_ALIGN                     0
#define VEC_CONFIG2_PROG_SCAN_BITS                      1
#define VEC_CONFIG2_PROG_SCAN_SHIFT                     15

/* SYNC_ADJ [14:12] - unsigned */
#define VEC_CONFIG2_SYNC_ADJ_MASK                       0x00007000UL
#define VEC_CONFIG2_SYNC_ADJ_ALIGN                      0
#define VEC_CONFIG2_SYNC_ADJ_BITS                       3
#define VEC_CONFIG2_SYNC_ADJ_SHIFT                      12

/* PBPR2_EN [11:11] - boolean */
#define VEC_CONFIG2_PBPR2_EN_MASK                       0x00000800UL
#define VEC_CONFIG2_PBPR2_EN_ALIGN                      0
#define VEC_CONFIG2_PBPR2_EN_BITS                       1
#define VEC_CONFIG2_PBPR2_EN_SHIFT                      11

/* PBPR_EN [10:10] - boolean */
#define VEC_CONFIG2_PBPR_EN_MASK                        0x00000400UL
#define VEC_CONFIG2_PBPR_EN_ALIGN                       0
#define VEC_CONFIG2_PBPR_EN_BITS                        1
#define VEC_CONFIG2_PBPR_EN_SHIFT                       10

/* VR_DAC_DIS [09:09] - boolean */
#define VEC_CONFIG2_VR_DAC_DIS_MASK                     0x00000200UL
#define VEC_CONFIG2_VR_DAC_DIS_ALIGN                    0
#define VEC_CONFIG2_VR_DAC_DIS_BITS                     1
#define VEC_CONFIG2_VR_DAC_DIS_SHIFT                    9

/* YG_DAC_DIS [08:08] - boolean */
#define VEC_CONFIG2_YG_DAC_DIS_MASK                     0x00000100UL
#define VEC_CONFIG2_YG_DAC_DIS_ALIGN                    0
#define VEC_CONFIG2_YG_DAC_DIS_BITS                     1
#define VEC_CONFIG2_YG_DAC_DIS_SHIFT                    8

/* UV_CB_EN [07:07] - boolean */
#define VEC_CONFIG2_UV_CB_EN_MASK                       0x00000080UL
#define VEC_CONFIG2_UV_CB_EN_ALIGN                      0
#define VEC_CONFIG2_UV_CB_EN_BITS                       1
#define VEC_CONFIG2_UV_CB_EN_SHIFT                      7

/* UV_DIG_DIS [06:06] - boolean */
#define VEC_CONFIG2_UV_DIG_DIS_MASK                     0x00000040UL
#define VEC_CONFIG2_UV_DIG_DIS_ALIGN                    0
#define VEC_CONFIG2_UV_DIG_DIS_BITS                     1
#define VEC_CONFIG2_UV_DIG_DIS_SHIFT                    6

/* RGB_DIG_DIS [05:05] - boolean */
#define VEC_CONFIG2_RGB_DIG_DIS_MASK                    0x00000020UL
#define VEC_CONFIG2_RGB_DIG_DIS_ALIGN                   0
#define VEC_CONFIG2_RGB_DIG_DIS_BITS                    1
#define VEC_CONFIG2_RGB_DIG_DIS_SHIFT                   5

/* UB_DAC_DIS [04:04] - boolean */
#define VEC_CONFIG2_UB_DAC_DIS_MASK                     0x00000010UL
#define VEC_CONFIG2_UB_DAC_DIS_ALIGN                    0
#define VEC_CONFIG2_UB_DAC_DIS_BITS                     1
#define VEC_CONFIG2_UB_DAC_DIS_SHIFT                    4

/* TMUX [03:02] - unsigned */
#define VEC_CONFIG2_TMUX_MASK                           0x0000000CUL
#define VEC_CONFIG2_TMUX_ALIGN                          0
#define VEC_CONFIG2_TMUX_BITS                           2
#define VEC_CONFIG2_TMUX_SHIFT                          2
#define VEC_CONFIG2_TMUX_DRIVE_ZERO                     0
#define VEC_CONFIG2_TMUX_RG_COMPOSITE                   1
#define VEC_CONFIG2_TMUX_UV_YC                          2
#define VEC_CONFIG2_TMUX_SYNC_YC                        3

/* RGB_SYNC_EN [01:01] - boolean */
#define VEC_CONFIG2_RGB_SYNC_EN_MASK                    0x00000002UL
#define VEC_CONFIG2_RGB_SYNC_EN_ALIGN                   0
#define VEC_CONFIG2_RGB_SYNC_EN_BITS                    1
#define VEC_CONFIG2_RGB_SYNC_EN_SHIFT                   1

/* GREEN_SYNC_EN [00:00] - boolean */
#define VEC_CONFIG2_GREEN_SYNC_EN_MASK                  0x00000001UL
#define VEC_CONFIG2_GREEN_SYNC_EN_ALIGN                 0
#define VEC_CONFIG2_GREEN_SYNC_EN_BITS                  1
#define VEC_CONFIG2_GREEN_SYNC_EN_SHIFT                 0

/**********************************************************************
 * VEC_INTERRUPT_CONTROL
 **********************************************************************/
/* reserved0 [31:02] -  */
#define VEC_INTERRUPT_CONTROL_reserved0_MASK            0xFFFFFFFCUL
#define VEC_INTERRUPT_CONTROL_reserved0_ALIGN           0
#define VEC_INTERRUPT_CONTROL_reserved0_BITS            30
#define VEC_INTERRUPT_CONTROL_reserved0_SHIFT           2

/* INTER_AFTER_BOT_VBI [01:01] - boolean */
#define VEC_INTERRUPT_CONTROL_INTER_AFTER_BOT_VBI_MASK  0x00000002UL
#define VEC_INTERRUPT_CONTROL_INTER_AFTER_BOT_VBI_ALIGN 0
#define VEC_INTERRUPT_CONTROL_INTER_AFTER_BOT_VBI_BITS  1
#define VEC_INTERRUPT_CONTROL_INTER_AFTER_BOT_VBI_SHIFT 1
#define VEC_INTERRUPT_CONTROL_INTER_AFTER_BOT_VBI_INACTIVE 0
#define VEC_INTERRUPT_CONTROL_INTER_AFTER_BOT_VBI_ACTIVE 1

/* INTER_AFTER_TOP_VBI [00:00] - boolean */
#define VEC_INTERRUPT_CONTROL_INTER_AFTER_TOP_VBI_MASK  0x00000001UL
#define VEC_INTERRUPT_CONTROL_INTER_AFTER_TOP_VBI_ALIGN 0
#define VEC_INTERRUPT_CONTROL_INTER_AFTER_TOP_VBI_BITS  1
#define VEC_INTERRUPT_CONTROL_INTER_AFTER_TOP_VBI_SHIFT 0
#define VEC_INTERRUPT_CONTROL_INTER_AFTER_TOP_VBI_INACTIVE 0
#define VEC_INTERRUPT_CONTROL_INTER_AFTER_TOP_VBI_ACTIVE 1

/**********************************************************************
 * VEC_INTERRUPT_STATUS
 **********************************************************************/
/* reserved0 [31:02] -  */
#define VEC_INTERRUPT_STATUS_reserved0_MASK             0xFFFFFFFCUL
#define VEC_INTERRUPT_STATUS_reserved0_ALIGN            0
#define VEC_INTERRUPT_STATUS_reserved0_BITS             30
#define VEC_INTERRUPT_STATUS_reserved0_SHIFT            2

/* BOT_INTERRUPT [01:01] - boolean */
#define VEC_INTERRUPT_STATUS_BOT_INTERRUPT_MASK         0x00000002UL
#define VEC_INTERRUPT_STATUS_BOT_INTERRUPT_ALIGN        0
#define VEC_INTERRUPT_STATUS_BOT_INTERRUPT_BITS         1
#define VEC_INTERRUPT_STATUS_BOT_INTERRUPT_SHIFT        1
#define VEC_INTERRUPT_STATUS_BOT_INTERRUPT_UNSET        0
#define VEC_INTERRUPT_STATUS_BOT_INTERRUPT_SET          1

/* TOP_INTERRUPT [00:00] - boolean */
#define VEC_INTERRUPT_STATUS_TOP_INTERRUPT_MASK         0x00000001UL
#define VEC_INTERRUPT_STATUS_TOP_INTERRUPT_ALIGN        0
#define VEC_INTERRUPT_STATUS_TOP_INTERRUPT_BITS         1
#define VEC_INTERRUPT_STATUS_TOP_INTERRUPT_SHIFT        0
#define VEC_INTERRUPT_STATUS_TOP_INTERRUPT_UNSET        0
#define VEC_INTERRUPT_STATUS_TOP_INTERRUPT_SET          1

/**********************************************************************
 * CCIR656_ENC_SW_Reset
 **********************************************************************/
/* RESET [31:00] - unsigned */
#define CCIR656_ENC_SW_Reset_RESET_MASK                 0xFFFFFFFFUL
#define CCIR656_ENC_SW_Reset_RESET_ALIGN                0
#define CCIR656_ENC_SW_Reset_RESET_BITS                 32
#define CCIR656_ENC_SW_Reset_RESET_SHIFT                0

/**********************************************************************
 * CCIR656_ENC_Mode
 **********************************************************************/
/* reserved0 [31:14] -  */
#define CCIR656_ENC_Mode_reserved0_MASK                 0xFFFFC000UL
#define CCIR656_ENC_Mode_reserved0_ALIGN                0
#define CCIR656_ENC_Mode_reserved0_BITS                 18
#define CCIR656_ENC_Mode_reserved0_SHIFT                14

/* FV_LATCH_REGION [13:13] - boolean */
#define CCIR656_ENC_Mode_FV_LATCH_REGION_MASK           0x00002000UL
#define CCIR656_ENC_Mode_FV_LATCH_REGION_ALIGN          0
#define CCIR656_ENC_Mode_FV_LATCH_REGION_BITS           1
#define CCIR656_ENC_Mode_FV_LATCH_REGION_SHIFT          13
#define CCIR656_ENC_Mode_FV_LATCH_REGION_ACTIVE         0
#define CCIR656_ENC_Mode_FV_LATCH_REGION_BLANKING       1

/* USE_LATCH_FV [12:12] - boolean */
#define CCIR656_ENC_Mode_USE_LATCH_FV_MASK              0x00001000UL
#define CCIR656_ENC_Mode_USE_LATCH_FV_ALIGN             0
#define CCIR656_ENC_Mode_USE_LATCH_FV_BITS              1
#define CCIR656_ENC_Mode_USE_LATCH_FV_SHIFT             12
#define CCIR656_ENC_Mode_USE_LATCH_FV_OFF               0
#define CCIR656_ENC_Mode_USE_LATCH_FV_ON                1

/* HSYNC_TO_FIRST_ACTIVE [11:01] - unsigned */
#define CCIR656_ENC_Mode_HSYNC_TO_FIRST_ACTIVE_MASK     0x00000FFEUL
#define CCIR656_ENC_Mode_HSYNC_TO_FIRST_ACTIVE_ALIGN    0
#define CCIR656_ENC_Mode_HSYNC_TO_FIRST_ACTIVE_BITS     11
#define CCIR656_ENC_Mode_HSYNC_TO_FIRST_ACTIVE_SHIFT    1

/* CHOOSE_FULL_RANGE [00:00] - boolean */
#define CCIR656_ENC_Mode_CHOOSE_FULL_RANGE_MASK         0x00000001UL
#define CCIR656_ENC_Mode_CHOOSE_FULL_RANGE_ALIGN        0
#define CCIR656_ENC_Mode_CHOOSE_FULL_RANGE_BITS         1
#define CCIR656_ENC_Mode_CHOOSE_FULL_RANGE_SHIFT        0
#define CCIR656_ENC_Mode_CHOOSE_FULL_RANGE_OFF          0
#define CCIR656_ENC_Mode_CHOOSE_FULL_RANGE_ON           1

/**********************************************************************
 * CCIR656_ENC_Vertical_Blanking
 **********************************************************************/
/* reserved0 [31:28] -  */
#define CCIR656_ENC_Vertical_Blanking_reserved0_MASK    0xF0000000UL
#define CCIR656_ENC_Vertical_Blanking_reserved0_ALIGN   0
#define CCIR656_ENC_Vertical_Blanking_reserved0_BITS    4
#define CCIR656_ENC_Vertical_Blanking_reserved0_SHIFT   28

/* FIELD1_END [27:21] - unsigned */
#define CCIR656_ENC_Vertical_Blanking_FIELD1_END_MASK   0x0FE00000UL
#define CCIR656_ENC_Vertical_Blanking_FIELD1_END_ALIGN  0
#define CCIR656_ENC_Vertical_Blanking_FIELD1_END_BITS   7
#define CCIR656_ENC_Vertical_Blanking_FIELD1_END_SHIFT  21

/* FIELD1_START [20:15] - unsigned */
#define CCIR656_ENC_Vertical_Blanking_FIELD1_START_MASK 0x001F8000UL
#define CCIR656_ENC_Vertical_Blanking_FIELD1_START_ALIGN 0
#define CCIR656_ENC_Vertical_Blanking_FIELD1_START_BITS 6
#define CCIR656_ENC_Vertical_Blanking_FIELD1_START_SHIFT 15

/* FIELD0_END [14:10] - unsigned */
#define CCIR656_ENC_Vertical_Blanking_FIELD0_END_MASK   0x00007C00UL
#define CCIR656_ENC_Vertical_Blanking_FIELD0_END_ALIGN  0
#define CCIR656_ENC_Vertical_Blanking_FIELD0_END_BITS   5
#define CCIR656_ENC_Vertical_Blanking_FIELD0_END_SHIFT  10

/* FIELD0_START [09:00] - unsigned */
#define CCIR656_ENC_Vertical_Blanking_FIELD0_START_MASK 0x000003FFUL
#define CCIR656_ENC_Vertical_Blanking_FIELD0_START_ALIGN 0
#define CCIR656_ENC_Vertical_Blanking_FIELD0_START_BITS 10
#define CCIR656_ENC_Vertical_Blanking_FIELD0_START_SHIFT 0

/**********************************************************************
 * CCIR656_ENC_Vertical_Misc
 **********************************************************************/
/* reserved0 [31:16] -  */
#define CCIR656_ENC_Vertical_Misc_reserved0_MASK        0xFFFF0000UL
#define CCIR656_ENC_Vertical_Misc_reserved0_ALIGN       0
#define CCIR656_ENC_Vertical_Misc_reserved0_BITS        16
#define CCIR656_ENC_Vertical_Misc_reserved0_SHIFT       16

/* FIELD0_END [15:10] - unsigned */
#define CCIR656_ENC_Vertical_Misc_FIELD0_END_MASK       0x0000FC00UL
#define CCIR656_ENC_Vertical_Misc_FIELD0_END_ALIGN      0
#define CCIR656_ENC_Vertical_Misc_FIELD0_END_BITS       6
#define CCIR656_ENC_Vertical_Misc_FIELD0_END_SHIFT      10

/* FIELD0_START [09:00] - unsigned */
#define CCIR656_ENC_Vertical_Misc_FIELD0_START_MASK     0x000003FFUL
#define CCIR656_ENC_Vertical_Misc_FIELD0_START_ALIGN    0
#define CCIR656_ENC_Vertical_Misc_FIELD0_START_BITS     10
#define CCIR656_ENC_Vertical_Misc_FIELD0_START_SHIFT    0

/**********************************************************************
 * CCIR656_ENC_Anci656_Port_A
 **********************************************************************/
/* reserved0 [31:27] -  */
#define CCIR656_ENC_Anci656_Port_A_reserved0_MASK       0xF8000000UL
#define CCIR656_ENC_Anci656_Port_A_reserved0_ALIGN      0
#define CCIR656_ENC_Anci656_Port_A_reserved0_BITS       5
#define CCIR656_ENC_Anci656_Port_A_reserved0_SHIFT      27

/* SDID [26:19] - unsigned */
#define CCIR656_ENC_Anci656_Port_A_SDID_MASK            0x07F80000UL
#define CCIR656_ENC_Anci656_Port_A_SDID_ALIGN           0
#define CCIR656_ENC_Anci656_Port_A_SDID_BITS            8
#define CCIR656_ENC_Anci656_Port_A_SDID_SHIFT           19

/* DWORD_COUNT [18:11] - unsigned */
#define CCIR656_ENC_Anci656_Port_A_DWORD_COUNT_MASK     0x0007F800UL
#define CCIR656_ENC_Anci656_Port_A_DWORD_COUNT_ALIGN    0
#define CCIR656_ENC_Anci656_Port_A_DWORD_COUNT_BITS     8
#define CCIR656_ENC_Anci656_Port_A_DWORD_COUNT_SHIFT    11

/* TYPE [10:03] - unsigned */
#define CCIR656_ENC_Anci656_Port_A_TYPE_MASK            0x000007F8UL
#define CCIR656_ENC_Anci656_Port_A_TYPE_ALIGN           0
#define CCIR656_ENC_Anci656_Port_A_TYPE_BITS            8
#define CCIR656_ENC_Anci656_Port_A_TYPE_SHIFT           3
#define CCIR656_ENC_Anci656_Port_A_TYPE_EURO_CLOSED_CAPTION 1
#define CCIR656_ENC_Anci656_Port_A_TYPE_WIDE_SCREEN     3
#define CCIR656_ENC_Anci656_Port_A_TYPE_US_TELETEXT     4
#define CCIR656_ENC_Anci656_Port_A_TYPE_US_CLOSED_CAPTION 5
#define CCIR656_ENC_Anci656_Port_A_TYPE_TELETEXT        8
#define CCIR656_ENC_Anci656_Port_A_TYPE_US_NABTS        12

/* FORMAT [02:01] - unsigned */
#define CCIR656_ENC_Anci656_Port_A_FORMAT_MASK          0x00000006UL
#define CCIR656_ENC_Anci656_Port_A_FORMAT_ALIGN         0
#define CCIR656_ENC_Anci656_Port_A_FORMAT_BITS          2
#define CCIR656_ENC_Anci656_Port_A_FORMAT_SHIFT         1
#define CCIR656_ENC_Anci656_Port_A_FORMAT_SAA7113H      0
#define CCIR656_ENC_Anci656_Port_A_FORMAT_MODIFIED_SAA7113H 1
#define CCIR656_ENC_Anci656_Port_A_FORMAT_SMPTE291M     2

/* ENABLE [00:00] - boolean */
#define CCIR656_ENC_Anci656_Port_A_ENABLE_MASK          0x00000001UL
#define CCIR656_ENC_Anci656_Port_A_ENABLE_ALIGN         0
#define CCIR656_ENC_Anci656_Port_A_ENABLE_BITS          1
#define CCIR656_ENC_Anci656_Port_A_ENABLE_SHIFT         0
#define CCIR656_ENC_Anci656_Port_A_ENABLE_OFF           0
#define CCIR656_ENC_Anci656_Port_A_ENABLE_ON            1

/* reserved1 [31:27] -  */
#define CCIR656_ENC_Anci656_Port_B_reserved1_MASK       0xF8000000UL
#define CCIR656_ENC_Anci656_Port_B_reserved1_ALIGN      0
#define CCIR656_ENC_Anci656_Port_B_reserved1_BITS       5
#define CCIR656_ENC_Anci656_Port_B_reserved1_SHIFT      27

/* SDID [26:19] - unsigned */
#define CCIR656_ENC_Anci656_Port_B_SDID_MASK            0x07F80000UL
#define CCIR656_ENC_Anci656_Port_B_SDID_ALIGN           0
#define CCIR656_ENC_Anci656_Port_B_SDID_BITS            8
#define CCIR656_ENC_Anci656_Port_B_SDID_SHIFT           19

/* DWORD_COUNT [18:11] - unsigned */
#define CCIR656_ENC_Anci656_Port_B_DWORD_COUNT_MASK     0x0007F800UL
#define CCIR656_ENC_Anci656_Port_B_DWORD_COUNT_ALIGN    0
#define CCIR656_ENC_Anci656_Port_B_DWORD_COUNT_BITS     8
#define CCIR656_ENC_Anci656_Port_B_DWORD_COUNT_SHIFT    11

/* TYPE [10:03] - unsigned */
#define CCIR656_ENC_Anci656_Port_B_TYPE_MASK            0x000007F8UL
#define CCIR656_ENC_Anci656_Port_B_TYPE_ALIGN           0
#define CCIR656_ENC_Anci656_Port_B_TYPE_BITS            8
#define CCIR656_ENC_Anci656_Port_B_TYPE_SHIFT           3
#define CCIR656_ENC_Anci656_Port_B_TYPE_EURO_CLOSED_CAPTION 1
#define CCIR656_ENC_Anci656_Port_B_TYPE_WIDE_SCREEN     3
#define CCIR656_ENC_Anci656_Port_B_TYPE_US_TELETEXT     4
#define CCIR656_ENC_Anci656_Port_B_TYPE_US_CLOSED_CAPTION 5
#define CCIR656_ENC_Anci656_Port_B_TYPE_TELETEXT        8
#define CCIR656_ENC_Anci656_Port_B_TYPE_US_NABTS        12

/* FORMAT [02:01] - unsigned */
#define CCIR656_ENC_Anci656_Port_B_FORMAT_MASK          0x00000006UL
#define CCIR656_ENC_Anci656_Port_B_FORMAT_ALIGN         0
#define CCIR656_ENC_Anci656_Port_B_FORMAT_BITS          2
#define CCIR656_ENC_Anci656_Port_B_FORMAT_SHIFT         1
#define CCIR656_ENC_Anci656_Port_B_FORMAT_SAA7113H      0
#define CCIR656_ENC_Anci656_Port_B_FORMAT_MODIFIED_SAA7113H 1
#define CCIR656_ENC_Anci656_Port_B_FORMAT_SMPTE291M     2

/* ENABLE [00:00] - boolean */
#define CCIR656_ENC_Anci656_Port_B_ENABLE_MASK          0x00000001UL
#define CCIR656_ENC_Anci656_Port_B_ENABLE_ALIGN         0
#define CCIR656_ENC_Anci656_Port_B_ENABLE_BITS          1
#define CCIR656_ENC_Anci656_Port_B_ENABLE_SHIFT         0
#define CCIR656_ENC_Anci656_Port_B_ENABLE_OFF           0
#define CCIR656_ENC_Anci656_Port_B_ENABLE_ON            1

/* reserved2 [31:27] -  */
#define CCIR656_ENC_Anci656_Port_C_reserved2_MASK       0xF8000000UL
#define CCIR656_ENC_Anci656_Port_C_reserved2_ALIGN      0
#define CCIR656_ENC_Anci656_Port_C_reserved2_BITS       5
#define CCIR656_ENC_Anci656_Port_C_reserved2_SHIFT      27

/* SDID [26:19] - unsigned */
#define CCIR656_ENC_Anci656_Port_C_SDID_MASK            0x07F80000UL
#define CCIR656_ENC_Anci656_Port_C_SDID_ALIGN           0
#define CCIR656_ENC_Anci656_Port_C_SDID_BITS            8
#define CCIR656_ENC_Anci656_Port_C_SDID_SHIFT           19

/* DWORD_COUNT [18:11] - unsigned */
#define CCIR656_ENC_Anci656_Port_C_DWORD_COUNT_MASK     0x0007F800UL
#define CCIR656_ENC_Anci656_Port_C_DWORD_COUNT_ALIGN    0
#define CCIR656_ENC_Anci656_Port_C_DWORD_COUNT_BITS     8
#define CCIR656_ENC_Anci656_Port_C_DWORD_COUNT_SHIFT    11

/* TYPE [10:03] - unsigned */
#define CCIR656_ENC_Anci656_Port_C_TYPE_MASK            0x000007F8UL
#define CCIR656_ENC_Anci656_Port_C_TYPE_ALIGN           0
#define CCIR656_ENC_Anci656_Port_C_TYPE_BITS            8
#define CCIR656_ENC_Anci656_Port_C_TYPE_SHIFT           3
#define CCIR656_ENC_Anci656_Port_C_TYPE_EURO_CLOSED_CAPTION 1
#define CCIR656_ENC_Anci656_Port_C_TYPE_WIDE_SCREEN     3
#define CCIR656_ENC_Anci656_Port_C_TYPE_US_TELETEXT     4
#define CCIR656_ENC_Anci656_Port_C_TYPE_US_CLOSED_CAPTION 5
#define CCIR656_ENC_Anci656_Port_C_TYPE_TELETEXT        8
#define CCIR656_ENC_Anci656_Port_C_TYPE_US_NABTS        12

/* FORMAT [02:01] - unsigned */
#define CCIR656_ENC_Anci656_Port_C_FORMAT_MASK          0x00000006UL
#define CCIR656_ENC_Anci656_Port_C_FORMAT_ALIGN         0
#define CCIR656_ENC_Anci656_Port_C_FORMAT_BITS          2
#define CCIR656_ENC_Anci656_Port_C_FORMAT_SHIFT         1
#define CCIR656_ENC_Anci656_Port_C_FORMAT_SAA7113H      0
#define CCIR656_ENC_Anci656_Port_C_FORMAT_MODIFIED_SAA7113H 1
#define CCIR656_ENC_Anci656_Port_C_FORMAT_SMPTE291M     2

/* ENABLE [00:00] - boolean */
#define CCIR656_ENC_Anci656_Port_C_ENABLE_MASK          0x00000001UL
#define CCIR656_ENC_Anci656_Port_C_ENABLE_ALIGN         0
#define CCIR656_ENC_Anci656_Port_C_ENABLE_BITS          1
#define CCIR656_ENC_Anci656_Port_C_ENABLE_SHIFT         0
#define CCIR656_ENC_Anci656_Port_C_ENABLE_OFF           0
#define CCIR656_ENC_Anci656_Port_C_ENABLE_ON            1

/**********************************************************************
 * GDE_GDE_DISP_CTRL
 **********************************************************************/
/* reserved0 [31:16] -  */
#define GDE_GDE_DISP_CTRL_reserved0_MASK                0xFFFF0000UL
#define GDE_GDE_DISP_CTRL_reserved0_ALIGN               0
#define GDE_GDE_DISP_CTRL_reserved0_BITS                16
#define GDE_GDE_DISP_CTRL_reserved0_SHIFT               16

/* CTRL_RSVD [15:08] - unsigned */
#define GDE_GDE_DISP_CTRL_CTRL_RSVD_MASK                0x0000FF00UL
#define GDE_GDE_DISP_CTRL_CTRL_RSVD_ALIGN               0
#define GDE_GDE_DISP_CTRL_CTRL_RSVD_BITS                8
#define GDE_GDE_DISP_CTRL_CTRL_RSVD_SHIFT               8

/* ARC_DDA_RESET_MODE [07:07] - boolean */
#define GDE_GDE_DISP_CTRL_ARC_DDA_RESET_MODE_MASK       0x00000080UL
#define GDE_GDE_DISP_CTRL_ARC_DDA_RESET_MODE_ALIGN      0
#define GDE_GDE_DISP_CTRL_ARC_DDA_RESET_MODE_BITS       1
#define GDE_GDE_DISP_CTRL_ARC_DDA_RESET_MODE_SHIFT      7

/* DISP_TF_ONLY [06:06] - boolean */
#define GDE_GDE_DISP_CTRL_DISP_TF_ONLY_MASK             0x00000040UL
#define GDE_GDE_DISP_CTRL_DISP_TF_ONLY_ALIGN            0
#define GDE_GDE_DISP_CTRL_DISP_TF_ONLY_BITS             1
#define GDE_GDE_DISP_CTRL_DISP_TF_ONLY_SHIFT            6

/* LITTLE_ENDIAN [05:05] - boolean */
#define GDE_GDE_DISP_CTRL_LITTLE_ENDIAN_MASK            0x00000020UL
#define GDE_GDE_DISP_CTRL_LITTLE_ENDIAN_ALIGN           0
#define GDE_GDE_DISP_CTRL_LITTLE_ENDIAN_BITS            1
#define GDE_GDE_DISP_CTRL_LITTLE_ENDIAN_SHIFT           5
#define GDE_GDE_DISP_CTRL_LITTLE_ENDIAN_BIG_ENDIAN      0
#define GDE_GDE_DISP_CTRL_LITTLE_ENDIAN_LITTLE_ENDIAN   1

/* FLTR_ENB_V [04:04] - boolean */
#define GDE_GDE_DISP_CTRL_FLTR_ENB_V_MASK               0x00000010UL
#define GDE_GDE_DISP_CTRL_FLTR_ENB_V_ALIGN              0
#define GDE_GDE_DISP_CTRL_FLTR_ENB_V_BITS               1
#define GDE_GDE_DISP_CTRL_FLTR_ENB_V_SHIFT              4

/* ARC_SCALE_DOWN [03:03] - boolean */
#define GDE_GDE_DISP_CTRL_ARC_SCALE_DOWN_MASK           0x00000008UL
#define GDE_GDE_DISP_CTRL_ARC_SCALE_DOWN_ALIGN          0
#define GDE_GDE_DISP_CTRL_ARC_SCALE_DOWN_BITS           1
#define GDE_GDE_DISP_CTRL_ARC_SCALE_DOWN_SHIFT          3

/* BURST_8GWORD [02:02] - boolean */
#define GDE_GDE_DISP_CTRL_BURST_8GWORD_MASK             0x00000004UL
#define GDE_GDE_DISP_CTRL_BURST_8GWORD_ALIGN            0
#define GDE_GDE_DISP_CTRL_BURST_8GWORD_BITS             1
#define GDE_GDE_DISP_CTRL_BURST_8GWORD_SHIFT            2

/* LCR_ENB [01:01] - boolean */
#define GDE_GDE_DISP_CTRL_LCR_ENB_MASK                  0x00000002UL
#define GDE_GDE_DISP_CTRL_LCR_ENB_ALIGN                 0
#define GDE_GDE_DISP_CTRL_LCR_ENB_BITS                  1
#define GDE_GDE_DISP_CTRL_LCR_ENB_SHIFT                 1

/* BGE_GFX_ENB [00:00] - boolean */
#define GDE_GDE_DISP_CTRL_BGE_GFX_ENB_MASK              0x00000001UL
#define GDE_GDE_DISP_CTRL_BGE_GFX_ENB_ALIGN             0
#define GDE_GDE_DISP_CTRL_BGE_GFX_ENB_BITS              1
#define GDE_GDE_DISP_CTRL_BGE_GFX_ENB_SHIFT             0

/**********************************************************************
 * GDE_ARC_SCALE_X
 **********************************************************************/
/* reserved0 [31:06] -  */
#define GDE_ARC_SCALE_X_reserved0_MASK                  0xFFFFFFC0UL
#define GDE_ARC_SCALE_X_reserved0_ALIGN                 0
#define GDE_ARC_SCALE_X_reserved0_BITS                  26
#define GDE_ARC_SCALE_X_reserved0_SHIFT                 6

/* ARC_SCALE_X [05:00] - unsigned */
#define GDE_ARC_SCALE_X_ARC_SCALE_X_MASK                0x0000003FUL
#define GDE_ARC_SCALE_X_ARC_SCALE_X_ALIGN               0
#define GDE_ARC_SCALE_X_ARC_SCALE_X_BITS                6
#define GDE_ARC_SCALE_X_ARC_SCALE_X_SHIFT               0

/**********************************************************************
 * GDE_ARC_DDA_INIT_PHS
 **********************************************************************/
/* reserved0 [31:06] -  */
#define GDE_ARC_DDA_INIT_PHS_reserved0_MASK             0xFFFFFFC0UL
#define GDE_ARC_DDA_INIT_PHS_reserved0_ALIGN            0
#define GDE_ARC_DDA_INIT_PHS_reserved0_BITS             26
#define GDE_ARC_DDA_INIT_PHS_reserved0_SHIFT            6

/* ARC_DDA_INIT_PHS [05:00] - unsigned */
#define GDE_ARC_DDA_INIT_PHS_ARC_DDA_INIT_PHS_MASK      0x0000003FUL
#define GDE_ARC_DDA_INIT_PHS_ARC_DDA_INIT_PHS_ALIGN     0
#define GDE_ARC_DDA_INIT_PHS_ARC_DDA_INIT_PHS_BITS      6
#define GDE_ARC_DDA_INIT_PHS_ARC_DDA_INIT_PHS_SHIFT     0

/**********************************************************************
 * GDE_RGB_CHROMA_KEY_MIN
 **********************************************************************/
/* reserved0 [31:24] -  */
#define GDE_RGB_CHROMA_KEY_MIN_reserved0_MASK           0xFF000000UL
#define GDE_RGB_CHROMA_KEY_MIN_reserved0_ALIGN          0
#define GDE_RGB_CHROMA_KEY_MIN_reserved0_BITS           8
#define GDE_RGB_CHROMA_KEY_MIN_reserved0_SHIFT          24

/* RGB_CHROMA_KEY_MIN [23:00] - unsigned */
#define GDE_RGB_CHROMA_KEY_MIN_RGB_CHROMA_KEY_MIN_MASK  0x00FFFFFFUL
#define GDE_RGB_CHROMA_KEY_MIN_RGB_CHROMA_KEY_MIN_ALIGN 0
#define GDE_RGB_CHROMA_KEY_MIN_RGB_CHROMA_KEY_MIN_BITS  24
#define GDE_RGB_CHROMA_KEY_MIN_RGB_CHROMA_KEY_MIN_SHIFT 0

/**********************************************************************
 * GDE_RGB_CHROMA_KEY_MAX
 **********************************************************************/
/* reserved0 [31:24] -  */
#define GDE_RGB_CHROMA_KEY_MAX_reserved0_MASK           0xFF000000UL
#define GDE_RGB_CHROMA_KEY_MAX_reserved0_ALIGN          0
#define GDE_RGB_CHROMA_KEY_MAX_reserved0_BITS           8
#define GDE_RGB_CHROMA_KEY_MAX_reserved0_SHIFT          24

/* RGB_CHROMA_KEY_MAX [23:00] - unsigned */
#define GDE_RGB_CHROMA_KEY_MAX_RGB_CHROMA_KEY_MAX_MASK  0x00FFFFFFUL
#define GDE_RGB_CHROMA_KEY_MAX_RGB_CHROMA_KEY_MAX_ALIGN 0
#define GDE_RGB_CHROMA_KEY_MAX_RGB_CHROMA_KEY_MAX_BITS  24
#define GDE_RGB_CHROMA_KEY_MAX_RGB_CHROMA_KEY_MAX_SHIFT 0

/**********************************************************************
 * GDE_YUV_CHROMA_KEY_MIN
 **********************************************************************/
/* reserved0 [31:24] -  */
#define GDE_YUV_CHROMA_KEY_MIN_reserved0_MASK           0xFF000000UL
#define GDE_YUV_CHROMA_KEY_MIN_reserved0_ALIGN          0
#define GDE_YUV_CHROMA_KEY_MIN_reserved0_BITS           8
#define GDE_YUV_CHROMA_KEY_MIN_reserved0_SHIFT          24

/* YUV_CHROMA_KEY_MIN [23:00] - unsigned */
#define GDE_YUV_CHROMA_KEY_MIN_YUV_CHROMA_KEY_MIN_MASK  0x00FFFFFFUL
#define GDE_YUV_CHROMA_KEY_MIN_YUV_CHROMA_KEY_MIN_ALIGN 0
#define GDE_YUV_CHROMA_KEY_MIN_YUV_CHROMA_KEY_MIN_BITS  24
#define GDE_YUV_CHROMA_KEY_MIN_YUV_CHROMA_KEY_MIN_SHIFT 0

/**********************************************************************
 * GDE_YUV_CHROMA_KEY_MAX
 **********************************************************************/
/* reserved0 [31:24] -  */
#define GDE_YUV_CHROMA_KEY_MAX_reserved0_MASK           0xFF000000UL
#define GDE_YUV_CHROMA_KEY_MAX_reserved0_ALIGN          0
#define GDE_YUV_CHROMA_KEY_MAX_reserved0_BITS           8
#define GDE_YUV_CHROMA_KEY_MAX_reserved0_SHIFT          24

/* YUV_CHROMA_KEY_MAX [23:00] - unsigned */
#define GDE_YUV_CHROMA_KEY_MAX_YUV_CHROMA_KEY_MAX_MASK  0x00FFFFFFUL
#define GDE_YUV_CHROMA_KEY_MAX_YUV_CHROMA_KEY_MAX_ALIGN 0
#define GDE_YUV_CHROMA_KEY_MAX_YUV_CHROMA_KEY_MAX_BITS  24
#define GDE_YUV_CHROMA_KEY_MAX_YUV_CHROMA_KEY_MAX_SHIFT 0

/**********************************************************************
 * GDE_IND_CHROMA_KEY
 **********************************************************************/
/* reserved0 [31:08] -  */
#define GDE_IND_CHROMA_KEY_reserved0_MASK               0xFFFFFF00UL
#define GDE_IND_CHROMA_KEY_reserved0_ALIGN              0
#define GDE_IND_CHROMA_KEY_reserved0_BITS               24
#define GDE_IND_CHROMA_KEY_reserved0_SHIFT              8

/* IND_CHROMA_KEY [07:00] - unsigned */
#define GDE_IND_CHROMA_KEY_IND_CHROMA_KEY_MASK          0x000000FFUL
#define GDE_IND_CHROMA_KEY_IND_CHROMA_KEY_ALIGN         0
#define GDE_IND_CHROMA_KEY_IND_CHROMA_KEY_BITS          8
#define GDE_IND_CHROMA_KEY_IND_CHROMA_KEY_SHIFT         0

/**********************************************************************
 * GDE_AFF_VSTART
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_AFF_VSTART_reserved0_MASK                   0xFFFFF800UL
#define GDE_AFF_VSTART_reserved0_ALIGN                  0
#define GDE_AFF_VSTART_reserved0_BITS                   21
#define GDE_AFF_VSTART_reserved0_SHIFT                  11

/* AFF_VSTART [10:00] - unsigned */
#define GDE_AFF_VSTART_AFF_VSTART_MASK                  0x000007FFUL
#define GDE_AFF_VSTART_AFF_VSTART_ALIGN                 0
#define GDE_AFF_VSTART_AFF_VSTART_BITS                  11
#define GDE_AFF_VSTART_AFF_VSTART_SHIFT                 0

/**********************************************************************
 * GDE_AFF_VCOEF
 **********************************************************************/
/* reserved0 [31:02] -  */
#define GDE_AFF_VCOEF_reserved0_MASK                    0xFFFFFFFCUL
#define GDE_AFF_VCOEF_reserved0_ALIGN                   0
#define GDE_AFF_VCOEF_reserved0_BITS                    30
#define GDE_AFF_VCOEF_reserved0_SHIFT                   2

/* AFF_VCOEF [01:00] - unsigned */
#define GDE_AFF_VCOEF_AFF_VCOEF_MASK                    0x00000003UL
#define GDE_AFF_VCOEF_AFF_VCOEF_ALIGN                   0
#define GDE_AFF_VCOEF_AFF_VCOEF_BITS                    2
#define GDE_AFF_VCOEF_AFF_VCOEF_SHIFT                   0

/**********************************************************************
 * GDE_WD_MSTART
 **********************************************************************/
/* reserved0 [31:28] -  */
#define GDE_WD_MSTART_reserved0_MASK                    0xF0000000UL
#define GDE_WD_MSTART_reserved0_ALIGN                   0
#define GDE_WD_MSTART_reserved0_BITS                    4
#define GDE_WD_MSTART_reserved0_SHIFT                   28

/* WD_MSTART [27:00] - unsigned */
#define GDE_WD_MSTART_WD_MSTART_MASK                    0x0FFFFFFFUL
#define GDE_WD_MSTART_WD_MSTART_ALIGN                   0
#define GDE_WD_MSTART_WD_MSTART_BITS                    28
#define GDE_WD_MSTART_WD_MSTART_SHIFT                   0

/**********************************************************************
 * GDE_CURT_WD_MSTART
 **********************************************************************/
/* reserved0 [31:28] -  */
#define GDE_CURT_WD_MSTART_reserved0_MASK               0xF0000000UL
#define GDE_CURT_WD_MSTART_reserved0_ALIGN              0
#define GDE_CURT_WD_MSTART_reserved0_BITS               4
#define GDE_CURT_WD_MSTART_reserved0_SHIFT              28

/* CURT_WD_MSTART [27:00] - unsigned */
#define GDE_CURT_WD_MSTART_CURT_WD_MSTART_MASK          0x0FFFFFFFUL
#define GDE_CURT_WD_MSTART_CURT_WD_MSTART_ALIGN         0
#define GDE_CURT_WD_MSTART_CURT_WD_MSTART_BITS          28
#define GDE_CURT_WD_MSTART_CURT_WD_MSTART_SHIFT         0

/**********************************************************************
 * GDE_FIFO_STATUS
 **********************************************************************/
/* reserved0 [31:05] -  */
#define GDE_FIFO_STATUS_reserved0_MASK                  0xFFFFFFE0UL
#define GDE_FIFO_STATUS_reserved0_ALIGN                 0
#define GDE_FIFO_STATUS_reserved0_BITS                  27
#define GDE_FIFO_STATUS_reserved0_SHIFT                 5

/* FIFO_STATUS [04:00] - unsigned */
#define GDE_FIFO_STATUS_FIFO_STATUS_MASK                0x0000001FUL
#define GDE_FIFO_STATUS_FIFO_STATUS_ALIGN               0
#define GDE_FIFO_STATUS_FIFO_STATUS_BITS                5
#define GDE_FIFO_STATUS_FIFO_STATUS_SHIFT               0

/**********************************************************************
 * GDE_REVISION
 **********************************************************************/
/* reserved0 [31:16] -  */
#define GDE_REVISION_reserved0_MASK                     0xFFFF0000UL
#define GDE_REVISION_reserved0_ALIGN                    0
#define GDE_REVISION_reserved0_BITS                     16
#define GDE_REVISION_reserved0_SHIFT                    16

/* MAJOR [15:08] - unsigned */
#define GDE_REVISION_MAJOR_MASK                         0x0000FF00UL
#define GDE_REVISION_MAJOR_ALIGN                        0
#define GDE_REVISION_MAJOR_BITS                         8
#define GDE_REVISION_MAJOR_SHIFT                        8

/* MINOR [07:00] - unsigned */
#define GDE_REVISION_MINOR_MASK                         0x000000FFUL
#define GDE_REVISION_MINOR_ALIGN                        0
#define GDE_REVISION_MINOR_BITS                         8
#define GDE_REVISION_MINOR_SHIFT                        0

/**********************************************************************
 * GDE_GDE_SCRH_REG
 **********************************************************************/
/* GDE_SCRH_REG [31:00] - unsigned */
#define GDE_GDE_SCRH_REG_GDE_SCRH_REG_MASK              0xFFFFFFFFUL
#define GDE_GDE_SCRH_REG_GDE_SCRH_REG_ALIGN             0
#define GDE_GDE_SCRH_REG_GDE_SCRH_REG_BITS              32
#define GDE_GDE_SCRH_REG_GDE_SCRH_REG_SHIFT             0

/**********************************************************************
 * GDE_LCR_CTRL
 **********************************************************************/
/* reserved0 [31:12] -  */
#define GDE_LCR_CTRL_reserved0_MASK                     0xFFFFF000UL
#define GDE_LCR_CTRL_reserved0_ALIGN                    0
#define GDE_LCR_CTRL_reserved0_BITS                     20
#define GDE_LCR_CTRL_reserved0_SHIFT                    12

/* LCR_CTRL_RSVD [11:09] - unsigned */
#define GDE_LCR_CTRL_LCR_CTRL_RSVD_MASK                 0x00000E00UL
#define GDE_LCR_CTRL_LCR_CTRL_RSVD_ALIGN                0
#define GDE_LCR_CTRL_LCR_CTRL_RSVD_BITS                 3
#define GDE_LCR_CTRL_LCR_CTRL_RSVD_SHIFT                9

/* LCR_CLUT_ACMP [08:08] - boolean */
#define GDE_LCR_CTRL_LCR_CLUT_ACMP_MASK                 0x00000100UL
#define GDE_LCR_CTRL_LCR_CLUT_ACMP_ALIGN                0
#define GDE_LCR_CTRL_LCR_CLUT_ACMP_BITS                 1
#define GDE_LCR_CTRL_LCR_CLUT_ACMP_SHIFT                8

/* LCR_HS_CLDENB [07:07] - boolean */
#define GDE_LCR_CTRL_LCR_HS_CLDENB_MASK                 0x00000080UL
#define GDE_LCR_CTRL_LCR_HS_CLDENB_ALIGN                0
#define GDE_LCR_CTRL_LCR_HS_CLDENB_BITS                 1
#define GDE_LCR_CTRL_LCR_HS_CLDENB_SHIFT                7

/* LCR_VS_CLDENB [06:06] - boolean */
#define GDE_LCR_CTRL_LCR_VS_CLDENB_MASK                 0x00000040UL
#define GDE_LCR_CTRL_LCR_VS_CLDENB_ALIGN                0
#define GDE_LCR_CTRL_LCR_VS_CLDENB_BITS                 1
#define GDE_LCR_CTRL_LCR_VS_CLDENB_SHIFT                6

/* LCR_ATYPE [05:04] - unsigned */
#define GDE_LCR_CTRL_LCR_ATYPE_MASK                     0x00000030UL
#define GDE_LCR_CTRL_LCR_ATYPE_ALIGN                    0
#define GDE_LCR_CTRL_LCR_ATYPE_BITS                     2
#define GDE_LCR_CTRL_LCR_ATYPE_SHIFT                    4
#define GDE_LCR_CTRL_LCR_ATYPE_ALPHA_FROM_SOURCE        1
#define GDE_LCR_CTRL_LCR_ATYPE_ALPHA_FROM_Y             2
#define GDE_LCR_CTRL_LCR_ATYPE_SINGLE_ALPHA             3

/* LCR_CKENB [03:03] - boolean */
#define GDE_LCR_CTRL_LCR_CKENB_MASK                     0x00000008UL
#define GDE_LCR_CTRL_LCR_CKENB_ALIGN                    0
#define GDE_LCR_CTRL_LCR_CKENB_BITS                     1
#define GDE_LCR_CTRL_LCR_CKENB_SHIFT                    3

/* LCR_ARCENB [02:02] - boolean */
#define GDE_LCR_CTRL_LCR_ARCENB_MASK                    0x00000004UL
#define GDE_LCR_CTRL_LCR_ARCENB_ALIGN                   0
#define GDE_LCR_CTRL_LCR_ARCENB_BITS                    1
#define GDE_LCR_CTRL_LCR_ARCENB_SHIFT                   2

/* LCR_16LB [01:01] - boolean */
#define GDE_LCR_CTRL_LCR_16LB_MASK                      0x00000002UL
#define GDE_LCR_CTRL_LCR_16LB_ALIGN                     0
#define GDE_LCR_CTRL_LCR_16LB_BITS                      1
#define GDE_LCR_CTRL_LCR_16LB_SHIFT                     1

/* LCR_8BPP [00:00] - boolean */
#define GDE_LCR_CTRL_LCR_8BPP_MASK                      0x00000001UL
#define GDE_LCR_CTRL_LCR_8BPP_ALIGN                     0
#define GDE_LCR_CTRL_LCR_8BPP_BITS                      1
#define GDE_LCR_CTRL_LCR_8BPP_SHIFT                     0

/**********************************************************************
 * GDE_LCR_HSTART
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_LCR_HSTART_reserved0_MASK                   0xFFFFF800UL
#define GDE_LCR_HSTART_reserved0_ALIGN                  0
#define GDE_LCR_HSTART_reserved0_BITS                   21
#define GDE_LCR_HSTART_reserved0_SHIFT                  11

/* LCR_HSTART [10:00] - unsigned */
#define GDE_LCR_HSTART_LCR_HSTART_MASK                  0x000007FFUL
#define GDE_LCR_HSTART_LCR_HSTART_ALIGN                 0
#define GDE_LCR_HSTART_LCR_HSTART_BITS                  11
#define GDE_LCR_HSTART_LCR_HSTART_SHIFT                 0

/**********************************************************************
 * GDE_LCR_HEND
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_LCR_HEND_reserved0_MASK                     0xFFFFF800UL
#define GDE_LCR_HEND_reserved0_ALIGN                    0
#define GDE_LCR_HEND_reserved0_BITS                     21
#define GDE_LCR_HEND_reserved0_SHIFT                    11

/* LCR_HEND [10:00] - unsigned */
#define GDE_LCR_HEND_LCR_HEND_MASK                      0x000007FFUL
#define GDE_LCR_HEND_LCR_HEND_ALIGN                     0
#define GDE_LCR_HEND_LCR_HEND_BITS                      11
#define GDE_LCR_HEND_LCR_HEND_SHIFT                     0

/**********************************************************************
 * GDE_LCR_VSTART
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_LCR_VSTART_reserved0_MASK                   0xFFFFF800UL
#define GDE_LCR_VSTART_reserved0_ALIGN                  0
#define GDE_LCR_VSTART_reserved0_BITS                   21
#define GDE_LCR_VSTART_reserved0_SHIFT                  11

/* LCR_VSTART [10:00] - unsigned */
#define GDE_LCR_VSTART_LCR_VSTART_MASK                  0x000007FFUL
#define GDE_LCR_VSTART_LCR_VSTART_ALIGN                 0
#define GDE_LCR_VSTART_LCR_VSTART_BITS                  11
#define GDE_LCR_VSTART_LCR_VSTART_SHIFT                 0

/**********************************************************************
 * GDE_LCR_VEND
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_LCR_VEND_reserved0_MASK                     0xFFFFF800UL
#define GDE_LCR_VEND_reserved0_ALIGN                    0
#define GDE_LCR_VEND_reserved0_BITS                     21
#define GDE_LCR_VEND_reserved0_SHIFT                    11

/* LCR_VEND [10:00] - unsigned */
#define GDE_LCR_VEND_LCR_VEND_MASK                      0x000007FFUL
#define GDE_LCR_VEND_LCR_VEND_ALIGN                     0
#define GDE_LCR_VEND_LCR_VEND_BITS                      11
#define GDE_LCR_VEND_LCR_VEND_SHIFT                     0

/**********************************************************************
 * GDE_LCR_HSIZE
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_LCR_HSIZE_reserved0_MASK                    0xFFFFF800UL
#define GDE_LCR_HSIZE_reserved0_ALIGN                   0
#define GDE_LCR_HSIZE_reserved0_BITS                    21
#define GDE_LCR_HSIZE_reserved0_SHIFT                   11

/* LCR_HSIZE [10:00] - unsigned */
#define GDE_LCR_HSIZE_LCR_HSIZE_MASK                    0x000007FFUL
#define GDE_LCR_HSIZE_LCR_HSIZE_ALIGN                   0
#define GDE_LCR_HSIZE_LCR_HSIZE_BITS                    11
#define GDE_LCR_HSIZE_LCR_HSIZE_SHIFT                   0

/**********************************************************************
 * GDE_LCR_HBLANK
 **********************************************************************/
/* reserved0 [31:02] -  */
#define GDE_LCR_HBLANK_reserved0_MASK                   0xFFFFFFFCUL
#define GDE_LCR_HBLANK_reserved0_ALIGN                  0
#define GDE_LCR_HBLANK_reserved0_BITS                   30
#define GDE_LCR_HBLANK_reserved0_SHIFT                  2

/* LCR_HBLANK [01:00] - unsigned */
#define GDE_LCR_HBLANK_LCR_HBLANK_MASK                  0x00000003UL
#define GDE_LCR_HBLANK_LCR_HBLANK_ALIGN                 0
#define GDE_LCR_HBLANK_LCR_HBLANK_BITS                  2
#define GDE_LCR_HBLANK_LCR_HBLANK_SHIFT                 0

/**********************************************************************
 * GDE_LCR_ALPHA
 **********************************************************************/
/* reserved0 [31:08] -  */
#define GDE_LCR_ALPHA_reserved0_MASK                    0xFFFFFF00UL
#define GDE_LCR_ALPHA_reserved0_ALIGN                   0
#define GDE_LCR_ALPHA_reserved0_BITS                    24
#define GDE_LCR_ALPHA_reserved0_SHIFT                   8

/* LCR_ALPHA [07:00] - unsigned */
#define GDE_LCR_ALPHA_LCR_ALPHA_MASK                    0x000000FFUL
#define GDE_LCR_ALPHA_LCR_ALPHA_ALIGN                   0
#define GDE_LCR_ALPHA_LCR_ALPHA_BITS                    8
#define GDE_LCR_ALPHA_LCR_ALPHA_SHIFT                   0

/**********************************************************************
 * GDE_LCR_MBASE
 **********************************************************************/
/* reserved0 [31:28] -  */
#define GDE_LCR_MBASE_reserved0_MASK                    0xF0000000UL
#define GDE_LCR_MBASE_reserved0_ALIGN                   0
#define GDE_LCR_MBASE_reserved0_BITS                    4
#define GDE_LCR_MBASE_reserved0_SHIFT                   28

/* LCR_MBASE [27:00] - unsigned */
#define GDE_LCR_MBASE_LCR_MBASE_MASK                    0x0FFFFFFFUL
#define GDE_LCR_MBASE_LCR_MBASE_ALIGN                   0
#define GDE_LCR_MBASE_LCR_MBASE_BITS                    28
#define GDE_LCR_MBASE_LCR_MBASE_SHIFT                   0

/**********************************************************************
 * GDE_LCR_CMBASE
 **********************************************************************/
/* reserved0 [31:18] -  */
#define GDE_LCR_CMBASE_reserved0_MASK                   0xFFFC0000UL
#define GDE_LCR_CMBASE_reserved0_ALIGN                  0
#define GDE_LCR_CMBASE_reserved0_BITS                   14
#define GDE_LCR_CMBASE_reserved0_SHIFT                  18

/* LCR_CMBASE [17:00] - unsigned */
#define GDE_LCR_CMBASE_LCR_CMBASE_MASK                  0x0003FFFFUL
#define GDE_LCR_CMBASE_LCR_CMBASE_ALIGN                 0
#define GDE_LCR_CMBASE_LCR_CMBASE_BITS                  18
#define GDE_LCR_CMBASE_LCR_CMBASE_SHIFT                 0

/**********************************************************************
 * GDE_LCR_BMMSB
 **********************************************************************/
/* reserved0 [31:18] -  */
#define GDE_LCR_BMMSB_reserved0_MASK                    0xFFFC0000UL
#define GDE_LCR_BMMSB_reserved0_ALIGN                   0
#define GDE_LCR_BMMSB_reserved0_BITS                    14
#define GDE_LCR_BMMSB_reserved0_SHIFT                   18

/* LCR_BMMSB [17:00] - unsigned */
#define GDE_LCR_BMMSB_LCR_BMMSB_MASK                    0x0003FFFFUL
#define GDE_LCR_BMMSB_LCR_BMMSB_ALIGN                   0
#define GDE_LCR_BMMSB_LCR_BMMSB_BITS                    18
#define GDE_LCR_BMMSB_LCR_BMMSB_SHIFT                   0

/**********************************************************************
 * GDE_HARC_C10_P0
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C10_P0_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C10_P0_reserved0_ALIGN                 0
#define GDE_HARC_C10_P0_reserved0_BITS                  5
#define GDE_HARC_C10_P0_reserved0_SHIFT                 27

/* HARC_C1_P0 [26:16] - signed */
#define GDE_HARC_C10_P0_HARC_C1_P0_MASK                 0x07FF0000UL
#define GDE_HARC_C10_P0_HARC_C1_P0_ALIGN                0
#define GDE_HARC_C10_P0_HARC_C1_P0_BITS                 11
#define GDE_HARC_C10_P0_HARC_C1_P0_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C10_P0_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C10_P0_reserved1_ALIGN                 0
#define GDE_HARC_C10_P0_reserved1_BITS                  5
#define GDE_HARC_C10_P0_reserved1_SHIFT                 11

/* HARC_C0_P0 [10:00] - signed */
#define GDE_HARC_C10_P0_HARC_C0_P0_MASK                 0x000007FFUL
#define GDE_HARC_C10_P0_HARC_C0_P0_ALIGN                0
#define GDE_HARC_C10_P0_HARC_C0_P0_BITS                 11
#define GDE_HARC_C10_P0_HARC_C0_P0_SHIFT                0

/**********************************************************************
 * GDE_HARC_C32_P0
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C32_P0_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C32_P0_reserved0_ALIGN                 0
#define GDE_HARC_C32_P0_reserved0_BITS                  5
#define GDE_HARC_C32_P0_reserved0_SHIFT                 27

/* HARC_C3_P0 [26:16] - signed */
#define GDE_HARC_C32_P0_HARC_C3_P0_MASK                 0x07FF0000UL
#define GDE_HARC_C32_P0_HARC_C3_P0_ALIGN                0
#define GDE_HARC_C32_P0_HARC_C3_P0_BITS                 11
#define GDE_HARC_C32_P0_HARC_C3_P0_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C32_P0_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C32_P0_reserved1_ALIGN                 0
#define GDE_HARC_C32_P0_reserved1_BITS                  5
#define GDE_HARC_C32_P0_reserved1_SHIFT                 11

/* HARC_C2_P0 [10:00] - signed */
#define GDE_HARC_C32_P0_HARC_C2_P0_MASK                 0x000007FFUL
#define GDE_HARC_C32_P0_HARC_C2_P0_ALIGN                0
#define GDE_HARC_C32_P0_HARC_C2_P0_BITS                 11
#define GDE_HARC_C32_P0_HARC_C2_P0_SHIFT                0

/**********************************************************************
 * GDE_HARC_C4_P0
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_HARC_C4_P0_reserved0_MASK                   0xFFFFF800UL
#define GDE_HARC_C4_P0_reserved0_ALIGN                  0
#define GDE_HARC_C4_P0_reserved0_BITS                   21
#define GDE_HARC_C4_P0_reserved0_SHIFT                  11

/* HARC_C4_P0 [10:00] - signed */
#define GDE_HARC_C4_P0_HARC_C4_P0_MASK                  0x000007FFUL
#define GDE_HARC_C4_P0_HARC_C4_P0_ALIGN                 0
#define GDE_HARC_C4_P0_HARC_C4_P0_BITS                  11
#define GDE_HARC_C4_P0_HARC_C4_P0_SHIFT                 0

/**********************************************************************
 * GDE_HARC_C10_P1
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C10_P1_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C10_P1_reserved0_ALIGN                 0
#define GDE_HARC_C10_P1_reserved0_BITS                  5
#define GDE_HARC_C10_P1_reserved0_SHIFT                 27

/* HARC_C1_P1 [26:16] - signed */
#define GDE_HARC_C10_P1_HARC_C1_P1_MASK                 0x07FF0000UL
#define GDE_HARC_C10_P1_HARC_C1_P1_ALIGN                0
#define GDE_HARC_C10_P1_HARC_C1_P1_BITS                 11
#define GDE_HARC_C10_P1_HARC_C1_P1_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C10_P1_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C10_P1_reserved1_ALIGN                 0
#define GDE_HARC_C10_P1_reserved1_BITS                  5
#define GDE_HARC_C10_P1_reserved1_SHIFT                 11

/* HARC_C0_P1 [10:00] - signed */
#define GDE_HARC_C10_P1_HARC_C0_P1_MASK                 0x000007FFUL
#define GDE_HARC_C10_P1_HARC_C0_P1_ALIGN                0
#define GDE_HARC_C10_P1_HARC_C0_P1_BITS                 11
#define GDE_HARC_C10_P1_HARC_C0_P1_SHIFT                0

/**********************************************************************
 * GDE_HARC_C32_P1
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C32_P1_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C32_P1_reserved0_ALIGN                 0
#define GDE_HARC_C32_P1_reserved0_BITS                  5
#define GDE_HARC_C32_P1_reserved0_SHIFT                 27

/* HARC_C3_P1 [26:16] - signed */
#define GDE_HARC_C32_P1_HARC_C3_P1_MASK                 0x07FF0000UL
#define GDE_HARC_C32_P1_HARC_C3_P1_ALIGN                0
#define GDE_HARC_C32_P1_HARC_C3_P1_BITS                 11
#define GDE_HARC_C32_P1_HARC_C3_P1_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C32_P1_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C32_P1_reserved1_ALIGN                 0
#define GDE_HARC_C32_P1_reserved1_BITS                  5
#define GDE_HARC_C32_P1_reserved1_SHIFT                 11

/* HARC_C2_P1 [10:00] - signed */
#define GDE_HARC_C32_P1_HARC_C2_P1_MASK                 0x000007FFUL
#define GDE_HARC_C32_P1_HARC_C2_P1_ALIGN                0
#define GDE_HARC_C32_P1_HARC_C2_P1_BITS                 11
#define GDE_HARC_C32_P1_HARC_C2_P1_SHIFT                0

/**********************************************************************
 * GDE_HARC_C4_P1
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_HARC_C4_P1_reserved0_MASK                   0xFFFFF800UL
#define GDE_HARC_C4_P1_reserved0_ALIGN                  0
#define GDE_HARC_C4_P1_reserved0_BITS                   21
#define GDE_HARC_C4_P1_reserved0_SHIFT                  11

/* HARC_C4_P1 [10:00] - signed */
#define GDE_HARC_C4_P1_HARC_C4_P1_MASK                  0x000007FFUL
#define GDE_HARC_C4_P1_HARC_C4_P1_ALIGN                 0
#define GDE_HARC_C4_P1_HARC_C4_P1_BITS                  11
#define GDE_HARC_C4_P1_HARC_C4_P1_SHIFT                 0

/**********************************************************************
 * GDE_HARC_C10_P2
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C10_P2_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C10_P2_reserved0_ALIGN                 0
#define GDE_HARC_C10_P2_reserved0_BITS                  5
#define GDE_HARC_C10_P2_reserved0_SHIFT                 27

/* HARC_C1_P2 [26:16] - signed */
#define GDE_HARC_C10_P2_HARC_C1_P2_MASK                 0x07FF0000UL
#define GDE_HARC_C10_P2_HARC_C1_P2_ALIGN                0
#define GDE_HARC_C10_P2_HARC_C1_P2_BITS                 11
#define GDE_HARC_C10_P2_HARC_C1_P2_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C10_P2_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C10_P2_reserved1_ALIGN                 0
#define GDE_HARC_C10_P2_reserved1_BITS                  5
#define GDE_HARC_C10_P2_reserved1_SHIFT                 11

/* HARC_C0_P2 [10:00] - signed */
#define GDE_HARC_C10_P2_HARC_C0_P2_MASK                 0x000007FFUL
#define GDE_HARC_C10_P2_HARC_C0_P2_ALIGN                0
#define GDE_HARC_C10_P2_HARC_C0_P2_BITS                 11
#define GDE_HARC_C10_P2_HARC_C0_P2_SHIFT                0

/**********************************************************************
 * GDE_HARC_C32_P2
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C32_P2_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C32_P2_reserved0_ALIGN                 0
#define GDE_HARC_C32_P2_reserved0_BITS                  5
#define GDE_HARC_C32_P2_reserved0_SHIFT                 27

/* HARC_C3_P2 [26:16] - signed */
#define GDE_HARC_C32_P2_HARC_C3_P2_MASK                 0x07FF0000UL
#define GDE_HARC_C32_P2_HARC_C3_P2_ALIGN                0
#define GDE_HARC_C32_P2_HARC_C3_P2_BITS                 11
#define GDE_HARC_C32_P2_HARC_C3_P2_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C32_P2_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C32_P2_reserved1_ALIGN                 0
#define GDE_HARC_C32_P2_reserved1_BITS                  5
#define GDE_HARC_C32_P2_reserved1_SHIFT                 11

/* HARC_C2_P2 [10:00] - signed */
#define GDE_HARC_C32_P2_HARC_C2_P2_MASK                 0x000007FFUL
#define GDE_HARC_C32_P2_HARC_C2_P2_ALIGN                0
#define GDE_HARC_C32_P2_HARC_C2_P2_BITS                 11
#define GDE_HARC_C32_P2_HARC_C2_P2_SHIFT                0

/**********************************************************************
 * GDE_HARC_C4_P2
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_HARC_C4_P2_reserved0_MASK                   0xFFFFF800UL
#define GDE_HARC_C4_P2_reserved0_ALIGN                  0
#define GDE_HARC_C4_P2_reserved0_BITS                   21
#define GDE_HARC_C4_P2_reserved0_SHIFT                  11

/* HARC_C4_P2 [10:00] - signed */
#define GDE_HARC_C4_P2_HARC_C4_P2_MASK                  0x000007FFUL
#define GDE_HARC_C4_P2_HARC_C4_P2_ALIGN                 0
#define GDE_HARC_C4_P2_HARC_C4_P2_BITS                  11
#define GDE_HARC_C4_P2_HARC_C4_P2_SHIFT                 0

/**********************************************************************
 * GDE_HARC_C10_P3
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C10_P3_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C10_P3_reserved0_ALIGN                 0
#define GDE_HARC_C10_P3_reserved0_BITS                  5
#define GDE_HARC_C10_P3_reserved0_SHIFT                 27

/* HARC_C1_P3 [26:16] - signed */
#define GDE_HARC_C10_P3_HARC_C1_P3_MASK                 0x07FF0000UL
#define GDE_HARC_C10_P3_HARC_C1_P3_ALIGN                0
#define GDE_HARC_C10_P3_HARC_C1_P3_BITS                 11
#define GDE_HARC_C10_P3_HARC_C1_P3_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C10_P3_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C10_P3_reserved1_ALIGN                 0
#define GDE_HARC_C10_P3_reserved1_BITS                  5
#define GDE_HARC_C10_P3_reserved1_SHIFT                 11

/* HARC_C0_P3 [10:00] - signed */
#define GDE_HARC_C10_P3_HARC_C0_P3_MASK                 0x000007FFUL
#define GDE_HARC_C10_P3_HARC_C0_P3_ALIGN                0
#define GDE_HARC_C10_P3_HARC_C0_P3_BITS                 11
#define GDE_HARC_C10_P3_HARC_C0_P3_SHIFT                0

/**********************************************************************
 * GDE_HARC_C32_P3
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C32_P3_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C32_P3_reserved0_ALIGN                 0
#define GDE_HARC_C32_P3_reserved0_BITS                  5
#define GDE_HARC_C32_P3_reserved0_SHIFT                 27

/* HARC_C3_P3 [26:16] - signed */
#define GDE_HARC_C32_P3_HARC_C3_P3_MASK                 0x07FF0000UL
#define GDE_HARC_C32_P3_HARC_C3_P3_ALIGN                0
#define GDE_HARC_C32_P3_HARC_C3_P3_BITS                 11
#define GDE_HARC_C32_P3_HARC_C3_P3_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C32_P3_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C32_P3_reserved1_ALIGN                 0
#define GDE_HARC_C32_P3_reserved1_BITS                  5
#define GDE_HARC_C32_P3_reserved1_SHIFT                 11

/* HARC_C2_P3 [10:00] - signed */
#define GDE_HARC_C32_P3_HARC_C2_P3_MASK                 0x000007FFUL
#define GDE_HARC_C32_P3_HARC_C2_P3_ALIGN                0
#define GDE_HARC_C32_P3_HARC_C2_P3_BITS                 11
#define GDE_HARC_C32_P3_HARC_C2_P3_SHIFT                0

/**********************************************************************
 * GDE_HARC_C4_P3
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_HARC_C4_P3_reserved0_MASK                   0xFFFFF800UL
#define GDE_HARC_C4_P3_reserved0_ALIGN                  0
#define GDE_HARC_C4_P3_reserved0_BITS                   21
#define GDE_HARC_C4_P3_reserved0_SHIFT                  11

/* HARC_C4_P3 [10:00] - signed */
#define GDE_HARC_C4_P3_HARC_C4_P3_MASK                  0x000007FFUL
#define GDE_HARC_C4_P3_HARC_C4_P3_ALIGN                 0
#define GDE_HARC_C4_P3_HARC_C4_P3_BITS                  11
#define GDE_HARC_C4_P3_HARC_C4_P3_SHIFT                 0

/**********************************************************************
 * GDE_HARC_C10_P4
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C10_P4_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C10_P4_reserved0_ALIGN                 0
#define GDE_HARC_C10_P4_reserved0_BITS                  5
#define GDE_HARC_C10_P4_reserved0_SHIFT                 27

/* HARC_C1_P4 [26:16] - signed */
#define GDE_HARC_C10_P4_HARC_C1_P4_MASK                 0x07FF0000UL
#define GDE_HARC_C10_P4_HARC_C1_P4_ALIGN                0
#define GDE_HARC_C10_P4_HARC_C1_P4_BITS                 11
#define GDE_HARC_C10_P4_HARC_C1_P4_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C10_P4_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C10_P4_reserved1_ALIGN                 0
#define GDE_HARC_C10_P4_reserved1_BITS                  5
#define GDE_HARC_C10_P4_reserved1_SHIFT                 11

/* HARC_C0_P4 [10:00] - signed */
#define GDE_HARC_C10_P4_HARC_C0_P4_MASK                 0x000007FFUL
#define GDE_HARC_C10_P4_HARC_C0_P4_ALIGN                0
#define GDE_HARC_C10_P4_HARC_C0_P4_BITS                 11
#define GDE_HARC_C10_P4_HARC_C0_P4_SHIFT                0

/**********************************************************************
 * GDE_HARC_C32_P4
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C32_P4_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C32_P4_reserved0_ALIGN                 0
#define GDE_HARC_C32_P4_reserved0_BITS                  5
#define GDE_HARC_C32_P4_reserved0_SHIFT                 27

/* HARC_C3_P4 [26:16] - signed */
#define GDE_HARC_C32_P4_HARC_C3_P4_MASK                 0x07FF0000UL
#define GDE_HARC_C32_P4_HARC_C3_P4_ALIGN                0
#define GDE_HARC_C32_P4_HARC_C3_P4_BITS                 11
#define GDE_HARC_C32_P4_HARC_C3_P4_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C32_P4_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C32_P4_reserved1_ALIGN                 0
#define GDE_HARC_C32_P4_reserved1_BITS                  5
#define GDE_HARC_C32_P4_reserved1_SHIFT                 11

/* HARC_C2_P4 [10:00] - signed */
#define GDE_HARC_C32_P4_HARC_C2_P4_MASK                 0x000007FFUL
#define GDE_HARC_C32_P4_HARC_C2_P4_ALIGN                0
#define GDE_HARC_C32_P4_HARC_C2_P4_BITS                 11
#define GDE_HARC_C32_P4_HARC_C2_P4_SHIFT                0

/**********************************************************************
 * GDE_HARC_C4_P4
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_HARC_C4_P4_reserved0_MASK                   0xFFFFF800UL
#define GDE_HARC_C4_P4_reserved0_ALIGN                  0
#define GDE_HARC_C4_P4_reserved0_BITS                   21
#define GDE_HARC_C4_P4_reserved0_SHIFT                  11

/* HARC_C4_P4 [10:00] - signed */
#define GDE_HARC_C4_P4_HARC_C4_P4_MASK                  0x000007FFUL
#define GDE_HARC_C4_P4_HARC_C4_P4_ALIGN                 0
#define GDE_HARC_C4_P4_HARC_C4_P4_BITS                  11
#define GDE_HARC_C4_P4_HARC_C4_P4_SHIFT                 0

/**********************************************************************
 * GDE_HARC_C10_P5
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C10_P5_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C10_P5_reserved0_ALIGN                 0
#define GDE_HARC_C10_P5_reserved0_BITS                  5
#define GDE_HARC_C10_P5_reserved0_SHIFT                 27

/* HARC_C1_P5 [26:16] - signed */
#define GDE_HARC_C10_P5_HARC_C1_P5_MASK                 0x07FF0000UL
#define GDE_HARC_C10_P5_HARC_C1_P5_ALIGN                0
#define GDE_HARC_C10_P5_HARC_C1_P5_BITS                 11
#define GDE_HARC_C10_P5_HARC_C1_P5_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C10_P5_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C10_P5_reserved1_ALIGN                 0
#define GDE_HARC_C10_P5_reserved1_BITS                  5
#define GDE_HARC_C10_P5_reserved1_SHIFT                 11

/* HARC_C0_P5 [10:00] - signed */
#define GDE_HARC_C10_P5_HARC_C0_P5_MASK                 0x000007FFUL
#define GDE_HARC_C10_P5_HARC_C0_P5_ALIGN                0
#define GDE_HARC_C10_P5_HARC_C0_P5_BITS                 11
#define GDE_HARC_C10_P5_HARC_C0_P5_SHIFT                0

/**********************************************************************
 * GDE_HARC_C32_P5
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C32_P5_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C32_P5_reserved0_ALIGN                 0
#define GDE_HARC_C32_P5_reserved0_BITS                  5
#define GDE_HARC_C32_P5_reserved0_SHIFT                 27

/* HARC_C3_P5 [26:16] - signed */
#define GDE_HARC_C32_P5_HARC_C3_P5_MASK                 0x07FF0000UL
#define GDE_HARC_C32_P5_HARC_C3_P5_ALIGN                0
#define GDE_HARC_C32_P5_HARC_C3_P5_BITS                 11
#define GDE_HARC_C32_P5_HARC_C3_P5_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C32_P5_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C32_P5_reserved1_ALIGN                 0
#define GDE_HARC_C32_P5_reserved1_BITS                  5
#define GDE_HARC_C32_P5_reserved1_SHIFT                 11

/* HARC_C2_P5 [10:00] - signed */
#define GDE_HARC_C32_P5_HARC_C2_P5_MASK                 0x000007FFUL
#define GDE_HARC_C32_P5_HARC_C2_P5_ALIGN                0
#define GDE_HARC_C32_P5_HARC_C2_P5_BITS                 11
#define GDE_HARC_C32_P5_HARC_C2_P5_SHIFT                0

/**********************************************************************
 * GDE_HARC_C4_P5
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_HARC_C4_P5_reserved0_MASK                   0xFFFFF800UL
#define GDE_HARC_C4_P5_reserved0_ALIGN                  0
#define GDE_HARC_C4_P5_reserved0_BITS                   21
#define GDE_HARC_C4_P5_reserved0_SHIFT                  11

/* HARC_C4_P5 [10:00] - signed */
#define GDE_HARC_C4_P5_HARC_C4_P5_MASK                  0x000007FFUL
#define GDE_HARC_C4_P5_HARC_C4_P5_ALIGN                 0
#define GDE_HARC_C4_P5_HARC_C4_P5_BITS                  11
#define GDE_HARC_C4_P5_HARC_C4_P5_SHIFT                 0

/**********************************************************************
 * GDE_HARC_C10_P6
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C10_P6_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C10_P6_reserved0_ALIGN                 0
#define GDE_HARC_C10_P6_reserved0_BITS                  5
#define GDE_HARC_C10_P6_reserved0_SHIFT                 27

/* HARC_C1_P6 [26:16] - signed */
#define GDE_HARC_C10_P6_HARC_C1_P6_MASK                 0x07FF0000UL
#define GDE_HARC_C10_P6_HARC_C1_P6_ALIGN                0
#define GDE_HARC_C10_P6_HARC_C1_P6_BITS                 11
#define GDE_HARC_C10_P6_HARC_C1_P6_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C10_P6_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C10_P6_reserved1_ALIGN                 0
#define GDE_HARC_C10_P6_reserved1_BITS                  5
#define GDE_HARC_C10_P6_reserved1_SHIFT                 11

/* HARC_C0_P6 [10:00] - signed */
#define GDE_HARC_C10_P6_HARC_C0_P6_MASK                 0x000007FFUL
#define GDE_HARC_C10_P6_HARC_C0_P6_ALIGN                0
#define GDE_HARC_C10_P6_HARC_C0_P6_BITS                 11
#define GDE_HARC_C10_P6_HARC_C0_P6_SHIFT                0

/**********************************************************************
 * GDE_HARC_C32_P6
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C32_P6_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C32_P6_reserved0_ALIGN                 0
#define GDE_HARC_C32_P6_reserved0_BITS                  5
#define GDE_HARC_C32_P6_reserved0_SHIFT                 27

/* HARC_C3_P6 [26:16] - signed */
#define GDE_HARC_C32_P6_HARC_C3_P6_MASK                 0x07FF0000UL
#define GDE_HARC_C32_P6_HARC_C3_P6_ALIGN                0
#define GDE_HARC_C32_P6_HARC_C3_P6_BITS                 11
#define GDE_HARC_C32_P6_HARC_C3_P6_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C32_P6_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C32_P6_reserved1_ALIGN                 0
#define GDE_HARC_C32_P6_reserved1_BITS                  5
#define GDE_HARC_C32_P6_reserved1_SHIFT                 11

/* HARC_C2_P6 [10:00] - signed */
#define GDE_HARC_C32_P6_HARC_C2_P6_MASK                 0x000007FFUL
#define GDE_HARC_C32_P6_HARC_C2_P6_ALIGN                0
#define GDE_HARC_C32_P6_HARC_C2_P6_BITS                 11
#define GDE_HARC_C32_P6_HARC_C2_P6_SHIFT                0

/**********************************************************************
 * GDE_HARC_C4_P6
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_HARC_C4_P6_reserved0_MASK                   0xFFFFF800UL
#define GDE_HARC_C4_P6_reserved0_ALIGN                  0
#define GDE_HARC_C4_P6_reserved0_BITS                   21
#define GDE_HARC_C4_P6_reserved0_SHIFT                  11

/* HARC_C4_P6 [10:00] - signed */
#define GDE_HARC_C4_P6_HARC_C4_P6_MASK                  0x000007FFUL
#define GDE_HARC_C4_P6_HARC_C4_P6_ALIGN                 0
#define GDE_HARC_C4_P6_HARC_C4_P6_BITS                  11
#define GDE_HARC_C4_P6_HARC_C4_P6_SHIFT                 0

/**********************************************************************
 * GDE_HARC_C10_P7
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C10_P7_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C10_P7_reserved0_ALIGN                 0
#define GDE_HARC_C10_P7_reserved0_BITS                  5
#define GDE_HARC_C10_P7_reserved0_SHIFT                 27

/* HARC_C1_P7 [26:16] - signed */
#define GDE_HARC_C10_P7_HARC_C1_P7_MASK                 0x07FF0000UL
#define GDE_HARC_C10_P7_HARC_C1_P7_ALIGN                0
#define GDE_HARC_C10_P7_HARC_C1_P7_BITS                 11
#define GDE_HARC_C10_P7_HARC_C1_P7_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C10_P7_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C10_P7_reserved1_ALIGN                 0
#define GDE_HARC_C10_P7_reserved1_BITS                  5
#define GDE_HARC_C10_P7_reserved1_SHIFT                 11

/* HARC_C0_P7 [10:00] - signed */
#define GDE_HARC_C10_P7_HARC_C0_P7_MASK                 0x000007FFUL
#define GDE_HARC_C10_P7_HARC_C0_P7_ALIGN                0
#define GDE_HARC_C10_P7_HARC_C0_P7_BITS                 11
#define GDE_HARC_C10_P7_HARC_C0_P7_SHIFT                0

/**********************************************************************
 * GDE_HARC_C32_P7
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C32_P7_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C32_P7_reserved0_ALIGN                 0
#define GDE_HARC_C32_P7_reserved0_BITS                  5
#define GDE_HARC_C32_P7_reserved0_SHIFT                 27

/* HARC_C3_P7 [26:16] - signed */
#define GDE_HARC_C32_P7_HARC_C3_P7_MASK                 0x07FF0000UL
#define GDE_HARC_C32_P7_HARC_C3_P7_ALIGN                0
#define GDE_HARC_C32_P7_HARC_C3_P7_BITS                 11
#define GDE_HARC_C32_P7_HARC_C3_P7_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C32_P7_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C32_P7_reserved1_ALIGN                 0
#define GDE_HARC_C32_P7_reserved1_BITS                  5
#define GDE_HARC_C32_P7_reserved1_SHIFT                 11

/* HARC_C2_P7 [10:00] - signed */
#define GDE_HARC_C32_P7_HARC_C2_P7_MASK                 0x000007FFUL
#define GDE_HARC_C32_P7_HARC_C2_P7_ALIGN                0
#define GDE_HARC_C32_P7_HARC_C2_P7_BITS                 11
#define GDE_HARC_C32_P7_HARC_C2_P7_SHIFT                0

/**********************************************************************
 * GDE_HARC_C4_P7
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_HARC_C4_P7_reserved0_MASK                   0xFFFFF800UL
#define GDE_HARC_C4_P7_reserved0_ALIGN                  0
#define GDE_HARC_C4_P7_reserved0_BITS                   21
#define GDE_HARC_C4_P7_reserved0_SHIFT                  11

/* HARC_C4_P7 [10:00] - signed */
#define GDE_HARC_C4_P7_HARC_C4_P7_MASK                  0x000007FFUL
#define GDE_HARC_C4_P7_HARC_C4_P7_ALIGN                 0
#define GDE_HARC_C4_P7_HARC_C4_P7_BITS                  11
#define GDE_HARC_C4_P7_HARC_C4_P7_SHIFT                 0

/**********************************************************************
 * GDE_HARC_C10_P8
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C10_P8_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C10_P8_reserved0_ALIGN                 0
#define GDE_HARC_C10_P8_reserved0_BITS                  5
#define GDE_HARC_C10_P8_reserved0_SHIFT                 27

/* HARC_C1_P8 [26:16] - signed */
#define GDE_HARC_C10_P8_HARC_C1_P8_MASK                 0x07FF0000UL
#define GDE_HARC_C10_P8_HARC_C1_P8_ALIGN                0
#define GDE_HARC_C10_P8_HARC_C1_P8_BITS                 11
#define GDE_HARC_C10_P8_HARC_C1_P8_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C10_P8_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C10_P8_reserved1_ALIGN                 0
#define GDE_HARC_C10_P8_reserved1_BITS                  5
#define GDE_HARC_C10_P8_reserved1_SHIFT                 11

/* HARC_C0_P8 [10:00] - signed */
#define GDE_HARC_C10_P8_HARC_C0_P8_MASK                 0x000007FFUL
#define GDE_HARC_C10_P8_HARC_C0_P8_ALIGN                0
#define GDE_HARC_C10_P8_HARC_C0_P8_BITS                 11
#define GDE_HARC_C10_P8_HARC_C0_P8_SHIFT                0

/**********************************************************************
 * GDE_HARC_C32_P8
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C32_P8_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C32_P8_reserved0_ALIGN                 0
#define GDE_HARC_C32_P8_reserved0_BITS                  5
#define GDE_HARC_C32_P8_reserved0_SHIFT                 27

/* HARC_C3_P8 [26:16] - signed */
#define GDE_HARC_C32_P8_HARC_C3_P8_MASK                 0x07FF0000UL
#define GDE_HARC_C32_P8_HARC_C3_P8_ALIGN                0
#define GDE_HARC_C32_P8_HARC_C3_P8_BITS                 11
#define GDE_HARC_C32_P8_HARC_C3_P8_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C32_P8_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C32_P8_reserved1_ALIGN                 0
#define GDE_HARC_C32_P8_reserved1_BITS                  5
#define GDE_HARC_C32_P8_reserved1_SHIFT                 11

/* HARC_C2_P8 [10:00] - signed */
#define GDE_HARC_C32_P8_HARC_C2_P8_MASK                 0x000007FFUL
#define GDE_HARC_C32_P8_HARC_C2_P8_ALIGN                0
#define GDE_HARC_C32_P8_HARC_C2_P8_BITS                 11
#define GDE_HARC_C32_P8_HARC_C2_P8_SHIFT                0

/**********************************************************************
 * GDE_HARC_C4_P8
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_HARC_C4_P8_reserved0_MASK                   0xFFFFF800UL
#define GDE_HARC_C4_P8_reserved0_ALIGN                  0
#define GDE_HARC_C4_P8_reserved0_BITS                   21
#define GDE_HARC_C4_P8_reserved0_SHIFT                  11

/* HARC_C4_P8 [10:00] - signed */
#define GDE_HARC_C4_P8_HARC_C4_P8_MASK                  0x000007FFUL
#define GDE_HARC_C4_P8_HARC_C4_P8_ALIGN                 0
#define GDE_HARC_C4_P8_HARC_C4_P8_BITS                  11
#define GDE_HARC_C4_P8_HARC_C4_P8_SHIFT                 0

/**********************************************************************
 * GDE_HARC_C10_P9
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C10_P9_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C10_P9_reserved0_ALIGN                 0
#define GDE_HARC_C10_P9_reserved0_BITS                  5
#define GDE_HARC_C10_P9_reserved0_SHIFT                 27

/* HARC_C1_P9 [26:16] - signed */
#define GDE_HARC_C10_P9_HARC_C1_P9_MASK                 0x07FF0000UL
#define GDE_HARC_C10_P9_HARC_C1_P9_ALIGN                0
#define GDE_HARC_C10_P9_HARC_C1_P9_BITS                 11
#define GDE_HARC_C10_P9_HARC_C1_P9_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C10_P9_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C10_P9_reserved1_ALIGN                 0
#define GDE_HARC_C10_P9_reserved1_BITS                  5
#define GDE_HARC_C10_P9_reserved1_SHIFT                 11

/* HARC_C0_P9 [10:00] - signed */
#define GDE_HARC_C10_P9_HARC_C0_P9_MASK                 0x000007FFUL
#define GDE_HARC_C10_P9_HARC_C0_P9_ALIGN                0
#define GDE_HARC_C10_P9_HARC_C0_P9_BITS                 11
#define GDE_HARC_C10_P9_HARC_C0_P9_SHIFT                0

/**********************************************************************
 * GDE_HARC_C32_P9
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C32_P9_reserved0_MASK                  0xF8000000UL
#define GDE_HARC_C32_P9_reserved0_ALIGN                 0
#define GDE_HARC_C32_P9_reserved0_BITS                  5
#define GDE_HARC_C32_P9_reserved0_SHIFT                 27

/* HARC_C3_P9 [26:16] - signed */
#define GDE_HARC_C32_P9_HARC_C3_P9_MASK                 0x07FF0000UL
#define GDE_HARC_C32_P9_HARC_C3_P9_ALIGN                0
#define GDE_HARC_C32_P9_HARC_C3_P9_BITS                 11
#define GDE_HARC_C32_P9_HARC_C3_P9_SHIFT                16

/* reserved1 [15:11] -  */
#define GDE_HARC_C32_P9_reserved1_MASK                  0x0000F800UL
#define GDE_HARC_C32_P9_reserved1_ALIGN                 0
#define GDE_HARC_C32_P9_reserved1_BITS                  5
#define GDE_HARC_C32_P9_reserved1_SHIFT                 11

/* HARC_C2_P9 [10:00] - signed */
#define GDE_HARC_C32_P9_HARC_C2_P9_MASK                 0x000007FFUL
#define GDE_HARC_C32_P9_HARC_C2_P9_ALIGN                0
#define GDE_HARC_C32_P9_HARC_C2_P9_BITS                 11
#define GDE_HARC_C32_P9_HARC_C2_P9_SHIFT                0

/**********************************************************************
 * GDE_HARC_C4_P9
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_HARC_C4_P9_reserved0_MASK                   0xFFFFF800UL
#define GDE_HARC_C4_P9_reserved0_ALIGN                  0
#define GDE_HARC_C4_P9_reserved0_BITS                   21
#define GDE_HARC_C4_P9_reserved0_SHIFT                  11

/* HARC_C4_P9 [10:00] - signed */
#define GDE_HARC_C4_P9_HARC_C4_P9_MASK                  0x000007FFUL
#define GDE_HARC_C4_P9_HARC_C4_P9_ALIGN                 0
#define GDE_HARC_C4_P9_HARC_C4_P9_BITS                  11
#define GDE_HARC_C4_P9_HARC_C4_P9_SHIFT                 0

/**********************************************************************
 * GDE_HARC_C10_P10
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C10_P10_reserved0_MASK                 0xF8000000UL
#define GDE_HARC_C10_P10_reserved0_ALIGN                0
#define GDE_HARC_C10_P10_reserved0_BITS                 5
#define GDE_HARC_C10_P10_reserved0_SHIFT                27

/* HARC_C1_P10 [26:16] - signed */
#define GDE_HARC_C10_P10_HARC_C1_P10_MASK               0x07FF0000UL
#define GDE_HARC_C10_P10_HARC_C1_P10_ALIGN              0
#define GDE_HARC_C10_P10_HARC_C1_P10_BITS               11
#define GDE_HARC_C10_P10_HARC_C1_P10_SHIFT              16

/* reserved1 [15:11] -  */
#define GDE_HARC_C10_P10_reserved1_MASK                 0x0000F800UL
#define GDE_HARC_C10_P10_reserved1_ALIGN                0
#define GDE_HARC_C10_P10_reserved1_BITS                 5
#define GDE_HARC_C10_P10_reserved1_SHIFT                11

/* HARC_C0_P10 [10:00] - signed */
#define GDE_HARC_C10_P10_HARC_C0_P10_MASK               0x000007FFUL
#define GDE_HARC_C10_P10_HARC_C0_P10_ALIGN              0
#define GDE_HARC_C10_P10_HARC_C0_P10_BITS               11
#define GDE_HARC_C10_P10_HARC_C0_P10_SHIFT              0

/**********************************************************************
 * GDE_HARC_C32_P10
 **********************************************************************/
/* reserved0 [31:27] -  */
#define GDE_HARC_C32_P10_reserved0_MASK                 0xF8000000UL
#define GDE_HARC_C32_P10_reserved0_ALIGN                0
#define GDE_HARC_C32_P10_reserved0_BITS                 5
#define GDE_HARC_C32_P10_reserved0_SHIFT                27

/* HARC_C3_P10 [26:16] - signed */
#define GDE_HARC_C32_P10_HARC_C3_P10_MASK               0x07FF0000UL
#define GDE_HARC_C32_P10_HARC_C3_P10_ALIGN              0
#define GDE_HARC_C32_P10_HARC_C3_P10_BITS               11
#define GDE_HARC_C32_P10_HARC_C3_P10_SHIFT              16

/* reserved1 [15:11] -  */
#define GDE_HARC_C32_P10_reserved1_MASK                 0x0000F800UL
#define GDE_HARC_C32_P10_reserved1_ALIGN                0
#define GDE_HARC_C32_P10_reserved1_BITS                 5
#define GDE_HARC_C32_P10_reserved1_SHIFT                11

/* HARC_C2_P10 [10:00] - signed */
#define GDE_HARC_C32_P10_HARC_C2_P10_MASK               0x000007FFUL
#define GDE_HARC_C32_P10_HARC_C2_P10_ALIGN              0
#define GDE_HARC_C32_P10_HARC_C2_P10_BITS               11
#define GDE_HARC_C32_P10_HARC_C2_P10_SHIFT              0

/**********************************************************************
 * GDE_HARC_C4_P10
 **********************************************************************/
/* reserved0 [31:11] -  */
#define GDE_HARC_C4_P10_reserved0_MASK                  0xFFFFF800UL
#define GDE_HARC_C4_P10_reserved0_ALIGN                 0
#define GDE_HARC_C4_P10_reserved0_BITS                  21
#define GDE_HARC_C4_P10_reserved0_SHIFT                 11

/* HARC_C4_P10 [10:00] - signed */
#define GDE_HARC_C4_P10_HARC_C4_P10_MASK                0x000007FFUL
#define GDE_HARC_C4_P10_HARC_C4_P10_ALIGN               0
#define GDE_HARC_C4_P10_HARC_C4_P10_BITS                11
#define GDE_HARC_C4_P10_HARC_C4_P10_SHIFT               0

/**********************************************************************
 * VDEC_SW_Reset
 **********************************************************************/
/* RESET [31:00] - unsigned */
#define VDEC_0_SW_Reset_RESET_MASK                      0xFFFFFFFFUL
#define VDEC_0_SW_Reset_RESET_ALIGN                     0
#define VDEC_0_SW_Reset_RESET_BITS                      32
#define VDEC_0_SW_Reset_RESET_SHIFT                     0

/**********************************************************************
 * VDEC_Mode
 **********************************************************************/
/* TRICK_MODE_STATUS [31:31] - boolean */
#define VDEC_0_Mode_TRICK_MODE_STATUS_MASK              0x80000000UL
#define VDEC_0_Mode_TRICK_MODE_STATUS_ALIGN             0
#define VDEC_0_Mode_TRICK_MODE_STATUS_BITS              1
#define VDEC_0_Mode_TRICK_MODE_STATUS_SHIFT             31

/* TV_SYSTEM_STATUS [30:30] - boolean */
#define VDEC_0_Mode_TV_SYSTEM_STATUS_MASK               0x40000000UL
#define VDEC_0_Mode_TV_SYSTEM_STATUS_ALIGN              0
#define VDEC_0_Mode_TV_SYSTEM_STATUS_BITS               1
#define VDEC_0_Mode_TV_SYSTEM_STATUS_SHIFT              30

/* VIDEO_STATUS [29:29] - boolean */
#define VDEC_0_Mode_VIDEO_STATUS_MASK                   0x20000000UL
#define VDEC_0_Mode_VIDEO_STATUS_ALIGN                  0
#define VDEC_0_Mode_VIDEO_STATUS_BITS                   1
#define VDEC_0_Mode_VIDEO_STATUS_SHIFT                  29

/* SW_COLOR_MODE_STATUS [28:28] - boolean */
#define VDEC_0_Mode_SW_COLOR_MODE_STATUS_MASK           0x10000000UL
#define VDEC_0_Mode_SW_COLOR_MODE_STATUS_ALIGN          0
#define VDEC_0_Mode_SW_COLOR_MODE_STATUS_BITS           1
#define VDEC_0_Mode_SW_COLOR_MODE_STATUS_SHIFT          28
#define VDEC_0_Mode_SW_COLOR_MODE_STATUS_BLACK_WHITE    0
#define VDEC_0_Mode_SW_COLOR_MODE_STATUS_COLOR          1

/* LINE_LOCK_ACCUMULATOR [27:27] - boolean */
#define VDEC_0_Mode_LINE_LOCK_ACCUMULATOR_MASK          0x08000000UL
#define VDEC_0_Mode_LINE_LOCK_ACCUMULATOR_ALIGN         0
#define VDEC_0_Mode_LINE_LOCK_ACCUMULATOR_BITS          1
#define VDEC_0_Mode_LINE_LOCK_ACCUMULATOR_SHIFT         27
#define VDEC_0_Mode_LINE_LOCK_ACCUMULATOR_ACTIVE        0
#define VDEC_0_Mode_LINE_LOCK_ACCUMULATOR_LOAD          1

/* CCIR656_FIXED_LENGTH [26:26] - boolean */
#define VDEC_0_Mode_CCIR656_FIXED_LENGTH_MASK           0x04000000UL
#define VDEC_0_Mode_CCIR656_FIXED_LENGTH_ALIGN          0
#define VDEC_0_Mode_CCIR656_FIXED_LENGTH_BITS           1
#define VDEC_0_Mode_CCIR656_FIXED_LENGTH_SHIFT          26

/* VBI_COMPOSITE [25:25] - boolean */
#define VDEC_0_Mode_VBI_COMPOSITE_MASK                  0x02000000UL
#define VDEC_0_Mode_VBI_COMPOSITE_ALIGN                 0
#define VDEC_0_Mode_VBI_COMPOSITE_BITS                  1
#define VDEC_0_Mode_VBI_COMPOSITE_SHIFT                 25
#define VDEC_0_Mode_VBI_COMPOSITE_BLOCK                 0
#define VDEC_0_Mode_VBI_COMPOSITE_PASS_THROUGH          1

/* CLAMP_PUMP [24:24] - boolean */
#define VDEC_0_Mode_CLAMP_PUMP_MASK                     0x01000000UL
#define VDEC_0_Mode_CLAMP_PUMP_ALIGN                    0
#define VDEC_0_Mode_CLAMP_PUMP_BITS                     1
#define VDEC_0_Mode_CLAMP_PUMP_SHIFT                    24
#define VDEC_0_Mode_CLAMP_PUMP_DISABLE                  0
#define VDEC_0_Mode_CLAMP_PUMP_ENABLE                   1

/* AGC_CLAMP_ORDER [23:23] - boolean */
#define VDEC_0_Mode_AGC_CLAMP_ORDER_MASK                0x00800000UL
#define VDEC_0_Mode_AGC_CLAMP_ORDER_ALIGN               0
#define VDEC_0_Mode_AGC_CLAMP_ORDER_BITS                1
#define VDEC_0_Mode_AGC_CLAMP_ORDER_SHIFT               23
#define VDEC_0_Mode_AGC_CLAMP_ORDER_AGC_CLAMP           0
#define VDEC_0_Mode_AGC_CLAMP_ORDER_CLAMP_AGC           1

/* INVERT_CLAMP_PULSE [22:22] - boolean */
#define VDEC_0_Mode_INVERT_CLAMP_PULSE_MASK             0x00400000UL
#define VDEC_0_Mode_INVERT_CLAMP_PULSE_ALIGN            0
#define VDEC_0_Mode_INVERT_CLAMP_PULSE_BITS             1
#define VDEC_0_Mode_INVERT_CLAMP_PULSE_SHIFT            22
#define VDEC_0_Mode_INVERT_CLAMP_PULSE_NORMAL           0
#define VDEC_0_Mode_INVERT_CLAMP_PULSE_INVERT           1

/* INVERTED_CLOCK [21:21] - boolean */
#define VDEC_0_Mode_INVERTED_CLOCK_MASK                 0x00200000UL
#define VDEC_0_Mode_INVERTED_CLOCK_ALIGN                0
#define VDEC_0_Mode_INVERTED_CLOCK_BITS                 1
#define VDEC_0_Mode_INVERTED_CLOCK_SHIFT                21

/* CONSTANT_OFFSET_MODE [20:20] - boolean */
#define VDEC_0_Mode_CONSTANT_OFFSET_MODE_MASK           0x00100000UL
#define VDEC_0_Mode_CONSTANT_OFFSET_MODE_ALIGN          0
#define VDEC_0_Mode_CONSTANT_OFFSET_MODE_BITS           1
#define VDEC_0_Mode_CONSTANT_OFFSET_MODE_SHIFT          20
#define VDEC_0_Mode_CONSTANT_OFFSET_MODE_OFF            0
#define VDEC_0_Mode_CONSTANT_OFFSET_MODE_ON             1

/* LINE_LOCK [19:19] - boolean */
#define VDEC_0_Mode_LINE_LOCK_MASK                      0x00080000UL
#define VDEC_0_Mode_LINE_LOCK_ALIGN                     0
#define VDEC_0_Mode_LINE_LOCK_BITS                      1
#define VDEC_0_Mode_LINE_LOCK_SHIFT                     19
#define VDEC_0_Mode_LINE_LOCK_OFF                       0
#define VDEC_0_Mode_LINE_LOCK_ON                        1

/* USE_COMB_THRESHOLD [18:17] - unsigned */
#define VDEC_0_Mode_USE_COMB_THRESHOLD_MASK             0x00060000UL
#define VDEC_0_Mode_USE_COMB_THRESHOLD_ALIGN            0
#define VDEC_0_Mode_USE_COMB_THRESHOLD_BITS             2
#define VDEC_0_Mode_USE_COMB_THRESHOLD_SHIFT            17

/* USE_COMB [16:16] - boolean */
#define VDEC_0_Mode_USE_COMB_MASK                       0x00010000UL
#define VDEC_0_Mode_USE_COMB_ALIGN                      0
#define VDEC_0_Mode_USE_COMB_BITS                       1
#define VDEC_0_Mode_USE_COMB_SHIFT                      16
#define VDEC_0_Mode_USE_COMB_MANUAL                     0
#define VDEC_0_Mode_USE_COMB_AUTO                       1

/* SW_USE_COMB [15:15] - boolean */
#define VDEC_0_Mode_SW_USE_COMB_MASK                    0x00008000UL
#define VDEC_0_Mode_SW_USE_COMB_ALIGN                   0
#define VDEC_0_Mode_SW_USE_COMB_BITS                    1
#define VDEC_0_Mode_SW_USE_COMB_SHIFT                   15

/* CCIR656_SRC [14:14] - boolean */
#define VDEC_0_Mode_CCIR656_SRC_MASK                    0x00004000UL
#define VDEC_0_Mode_CCIR656_SRC_ALIGN                   0
#define VDEC_0_Mode_CCIR656_SRC_BITS                    1
#define VDEC_0_Mode_CCIR656_SRC_SHIFT                   14
#define VDEC_0_Mode_CCIR656_SRC_INTERNAL                0
#define VDEC_0_Mode_CCIR656_SRC_EXTERNAL                1

/* PWM_INVERT [13:13] - boolean */
#define VDEC_0_Mode_PWM_INVERT_MASK                     0x00002000UL
#define VDEC_0_Mode_PWM_INVERT_ALIGN                    0
#define VDEC_0_Mode_PWM_INVERT_BITS                     1
#define VDEC_0_Mode_PWM_INVERT_SHIFT                    13

/* PWM_ENABLE [12:12] - boolean */
#define VDEC_0_Mode_PWM_ENABLE_MASK                     0x00001000UL
#define VDEC_0_Mode_PWM_ENABLE_ALIGN                    0
#define VDEC_0_Mode_PWM_ENABLE_BITS                     1
#define VDEC_0_Mode_PWM_ENABLE_SHIFT                    12

/* COLOR_MODE [11:11] - boolean */
#define VDEC_0_Mode_COLOR_MODE_MASK                     0x00000800UL
#define VDEC_0_Mode_COLOR_MODE_ALIGN                    0
#define VDEC_0_Mode_COLOR_MODE_BITS                     1
#define VDEC_0_Mode_COLOR_MODE_SHIFT                    11
#define VDEC_0_Mode_COLOR_MODE_MANUAL                   0
#define VDEC_0_Mode_COLOR_MODE_AUTO                     1

/* SW_COLOR_MODE [10:10] - boolean */
#define VDEC_0_Mode_SW_COLOR_MODE_MASK                  0x00000400UL
#define VDEC_0_Mode_SW_COLOR_MODE_ALIGN                 0
#define VDEC_0_Mode_SW_COLOR_MODE_BITS                  1
#define VDEC_0_Mode_SW_COLOR_MODE_SHIFT                 10
#define VDEC_0_Mode_SW_COLOR_MODE_BLACK_WHITE           0
#define VDEC_0_Mode_SW_COLOR_MODE_COLOR                 1

/* DC_RESTORE_TYPE [09:09] - boolean */
#define VDEC_0_Mode_DC_RESTORE_TYPE_MASK                0x00000200UL
#define VDEC_0_Mode_DC_RESTORE_TYPE_ALIGN               0
#define VDEC_0_Mode_DC_RESTORE_TYPE_BITS                1
#define VDEC_0_Mode_DC_RESTORE_TYPE_SHIFT               9
#define VDEC_0_Mode_DC_RESTORE_TYPE_PWM                 0
#define VDEC_0_Mode_DC_RESTORE_TYPE_PULSE               1

/* MAIN_AGC [08:07] - unsigned */
#define VDEC_0_Mode_MAIN_AGC_MASK                       0x00000180UL
#define VDEC_0_Mode_MAIN_AGC_ALIGN                      0
#define VDEC_0_Mode_MAIN_AGC_BITS                       2
#define VDEC_0_Mode_MAIN_AGC_SHIFT                      7
#define VDEC_0_Mode_MAIN_AGC_OFF                        0
#define VDEC_0_Mode_MAIN_AGC_SYNC                       1
#define VDEC_0_Mode_MAIN_AGC_PP                         2
#define VDEC_0_Mode_MAIN_AGC_SW                         3

/* COMB_2_LINES [06:06] - unsigned */
#define VDEC_0_Mode_COMB_2_LINES_MASK                   0x00000040UL
#define VDEC_0_Mode_COMB_2_LINES_ALIGN                  0
#define VDEC_0_Mode_COMB_2_LINES_BITS                   1
#define VDEC_0_Mode_COMB_2_LINES_SHIFT                  6
#define VDEC_0_Mode_COMB_2_LINES_OFF                    0
#define VDEC_0_Mode_COMB_2_LINES_ON                     1

/* LUMA_FILTER [05:04] - unsigned */
#define VDEC_0_Mode_LUMA_FILTER_MASK                    0x00000030UL
#define VDEC_0_Mode_LUMA_FILTER_ALIGN                   0
#define VDEC_0_Mode_LUMA_FILTER_BITS                    2
#define VDEC_0_Mode_LUMA_FILTER_SHIFT                   4
#define VDEC_0_Mode_LUMA_FILTER_RESERVED                0
#define VDEC_0_Mode_LUMA_FILTER_NOTCH                   1
#define VDEC_0_Mode_LUMA_FILTER_COMB                    2
#define VDEC_0_Mode_LUMA_FILTER_ADAPTIVE                3

/* CHROMA_FILTER [03:02] - unsigned */
#define VDEC_0_Mode_CHROMA_FILTER_MASK                  0x0000000CUL
#define VDEC_0_Mode_CHROMA_FILTER_ALIGN                 0
#define VDEC_0_Mode_CHROMA_FILTER_BITS                  2
#define VDEC_0_Mode_CHROMA_FILTER_SHIFT                 2
#define VDEC_0_Mode_CHROMA_FILTER_OFF                   0
#define VDEC_0_Mode_CHROMA_FILTER_HORIZONTAL            1
#define VDEC_0_Mode_CHROMA_FILTER_VERTICAL              2
#define VDEC_0_Mode_CHROMA_FILTER_ADAPTIVE              3

/* TV_SYSTEM [01:00] - unsigned */
#define VDEC_0_Mode_TV_SYSTEM_MASK                      0x00000003UL
#define VDEC_0_Mode_TV_SYSTEM_ALIGN                     0
#define VDEC_0_Mode_TV_SYSTEM_BITS                      2
#define VDEC_0_Mode_TV_SYSTEM_SHIFT                     0
#define VDEC_0_Mode_TV_SYSTEM_NTSC                      0
#define VDEC_0_Mode_TV_SYSTEM_PAL                       1
#define VDEC_0_Mode_TV_SYSTEM_PAL_N                     2
#define VDEC_0_Mode_TV_SYSTEM_PAL_M                     3

/**********************************************************************
 * VDEC_Vertical_Blanking
 **********************************************************************/
/* reserved0 [31:28] -  */
#define VDEC_0_Vertical_Blanking_reserved0_MASK         0xF0000000UL
#define VDEC_0_Vertical_Blanking_reserved0_ALIGN        0
#define VDEC_0_Vertical_Blanking_reserved0_BITS         4
#define VDEC_0_Vertical_Blanking_reserved0_SHIFT        28

/* FIELD1_END [27:21] - unsigned */
#define VDEC_0_Vertical_Blanking_FIELD1_END_MASK        0x0FE00000UL
#define VDEC_0_Vertical_Blanking_FIELD1_END_ALIGN       0
#define VDEC_0_Vertical_Blanking_FIELD1_END_BITS        7
#define VDEC_0_Vertical_Blanking_FIELD1_END_SHIFT       21

/* FIELD1_START [20:15] - unsigned */
#define VDEC_0_Vertical_Blanking_FIELD1_START_MASK      0x001F8000UL
#define VDEC_0_Vertical_Blanking_FIELD1_START_ALIGN     0
#define VDEC_0_Vertical_Blanking_FIELD1_START_BITS      6
#define VDEC_0_Vertical_Blanking_FIELD1_START_SHIFT     15

/* FIELD0_END [14:10] - unsigned */
#define VDEC_0_Vertical_Blanking_FIELD0_END_MASK        0x00007C00UL
#define VDEC_0_Vertical_Blanking_FIELD0_END_ALIGN       0
#define VDEC_0_Vertical_Blanking_FIELD0_END_BITS        5
#define VDEC_0_Vertical_Blanking_FIELD0_END_SHIFT       10

/* FIELD0_START [09:00] - unsigned */
#define VDEC_0_Vertical_Blanking_FIELD0_START_MASK      0x000003FFUL
#define VDEC_0_Vertical_Blanking_FIELD0_START_ALIGN     0
#define VDEC_0_Vertical_Blanking_FIELD0_START_BITS      10
#define VDEC_0_Vertical_Blanking_FIELD0_START_SHIFT     0

/**********************************************************************
 * VDEC_Vertical_Clamping
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDEC_0_Vertical_Clamping_reserved0_MASK         0xFC000000UL
#define VDEC_0_Vertical_Clamping_reserved0_ALIGN        0
#define VDEC_0_Vertical_Clamping_reserved0_BITS         6
#define VDEC_0_Vertical_Clamping_reserved0_SHIFT        26

/* FIELD1_END [25:19] - unsigned */
#define VDEC_0_Vertical_Clamping_FIELD1_END_MASK        0x03F80000UL
#define VDEC_0_Vertical_Clamping_FIELD1_END_ALIGN       0
#define VDEC_0_Vertical_Clamping_FIELD1_END_BITS        7
#define VDEC_0_Vertical_Clamping_FIELD1_END_SHIFT       19

/* FIELD1_START [18:13] - unsigned */
#define VDEC_0_Vertical_Clamping_FIELD1_START_MASK      0x0007E000UL
#define VDEC_0_Vertical_Clamping_FIELD1_START_ALIGN     0
#define VDEC_0_Vertical_Clamping_FIELD1_START_BITS      6
#define VDEC_0_Vertical_Clamping_FIELD1_START_SHIFT     13

/* FIELD0_END [12:08] - unsigned */
#define VDEC_0_Vertical_Clamping_FIELD0_END_MASK        0x00001F00UL
#define VDEC_0_Vertical_Clamping_FIELD0_END_ALIGN       0
#define VDEC_0_Vertical_Clamping_FIELD0_END_BITS        5
#define VDEC_0_Vertical_Clamping_FIELD0_END_SHIFT       8

/* FIELD0_START [07:00] - unsigned */
#define VDEC_0_Vertical_Clamping_FIELD0_START_MASK      0x000000FFUL
#define VDEC_0_Vertical_Clamping_FIELD0_START_ALIGN     0
#define VDEC_0_Vertical_Clamping_FIELD0_START_BITS      8
#define VDEC_0_Vertical_Clamping_FIELD0_START_SHIFT     0

/**********************************************************************
 * VDEC_Vertical_Sync
 **********************************************************************/
/* reserved0 [31:30] -  */
#define VDEC_0_Vertical_Sync_reserved0_MASK             0xC0000000UL
#define VDEC_0_Vertical_Sync_reserved0_ALIGN            0
#define VDEC_0_Vertical_Sync_reserved0_BITS             2
#define VDEC_0_Vertical_Sync_reserved0_SHIFT            30

/* CSYNC_SELECT [29:29] - boolean */
#define VDEC_0_Vertical_Sync_CSYNC_SELECT_MASK          0x20000000UL
#define VDEC_0_Vertical_Sync_CSYNC_SELECT_ALIGN         0
#define VDEC_0_Vertical_Sync_CSYNC_SELECT_BITS          1
#define VDEC_0_Vertical_Sync_CSYNC_SELECT_SHIFT         29
#define VDEC_0_Vertical_Sync_CSYNC_SELECT_WITH_SERRATION 0
#define VDEC_0_Vertical_Sync_CSYNC_SELECT_WITHOUT_SERRATION 1

/* INTERNAL_FIELD_ID [28:28] - boolean */
#define VDEC_0_Vertical_Sync_INTERNAL_FIELD_ID_MASK     0x10000000UL
#define VDEC_0_Vertical_Sync_INTERNAL_FIELD_ID_ALIGN    0
#define VDEC_0_Vertical_Sync_INTERNAL_FIELD_ID_BITS     1
#define VDEC_0_Vertical_Sync_INTERNAL_FIELD_ID_SHIFT    28

/* FIELD1_END [27:21] - unsigned */
#define VDEC_0_Vertical_Sync_FIELD1_END_MASK            0x0FE00000UL
#define VDEC_0_Vertical_Sync_FIELD1_END_ALIGN           0
#define VDEC_0_Vertical_Sync_FIELD1_END_BITS            7
#define VDEC_0_Vertical_Sync_FIELD1_END_SHIFT           21

/* FIELD1_START [20:15] - unsigned */
#define VDEC_0_Vertical_Sync_FIELD1_START_MASK          0x001F8000UL
#define VDEC_0_Vertical_Sync_FIELD1_START_ALIGN         0
#define VDEC_0_Vertical_Sync_FIELD1_START_BITS          6
#define VDEC_0_Vertical_Sync_FIELD1_START_SHIFT         15

/* FIELD0_END [14:10] - unsigned */
#define VDEC_0_Vertical_Sync_FIELD0_END_MASK            0x00007C00UL
#define VDEC_0_Vertical_Sync_FIELD0_END_ALIGN           0
#define VDEC_0_Vertical_Sync_FIELD0_END_BITS            5
#define VDEC_0_Vertical_Sync_FIELD0_END_SHIFT           10

/* FIELD0_START [09:00] - unsigned */
#define VDEC_0_Vertical_Sync_FIELD0_START_MASK          0x000003FFUL
#define VDEC_0_Vertical_Sync_FIELD0_START_ALIGN         0
#define VDEC_0_Vertical_Sync_FIELD0_START_BITS          10
#define VDEC_0_Vertical_Sync_FIELD0_START_SHIFT         0

/**********************************************************************
 * VDEC_Vertical_AGC
 **********************************************************************/
/* reserved0 [31:31] -  */
#define VDEC_0_Vertical_AGC_reserved0_MASK              0x80000000UL
#define VDEC_0_Vertical_AGC_reserved0_ALIGN             0
#define VDEC_0_Vertical_AGC_reserved0_BITS              1
#define VDEC_0_Vertical_AGC_reserved0_SHIFT             31

/* FIELD1_KILL_END [30:24] - unsigned */
#define VDEC_0_Vertical_AGC_FIELD1_KILL_END_MASK        0x7F000000UL
#define VDEC_0_Vertical_AGC_FIELD1_KILL_END_ALIGN       0
#define VDEC_0_Vertical_AGC_FIELD1_KILL_END_BITS        7
#define VDEC_0_Vertical_AGC_FIELD1_KILL_END_SHIFT       24

/* FIELD1_KILL_START [23:15] - unsigned */
#define VDEC_0_Vertical_AGC_FIELD1_KILL_START_MASK      0x00FF8000UL
#define VDEC_0_Vertical_AGC_FIELD1_KILL_START_ALIGN     0
#define VDEC_0_Vertical_AGC_FIELD1_KILL_START_BITS      9
#define VDEC_0_Vertical_AGC_FIELD1_KILL_START_SHIFT     15

/* FIELD0_KILL_END [14:10] - unsigned */
#define VDEC_0_Vertical_AGC_FIELD0_KILL_END_MASK        0x00007C00UL
#define VDEC_0_Vertical_AGC_FIELD0_KILL_END_ALIGN       0
#define VDEC_0_Vertical_AGC_FIELD0_KILL_END_BITS        5
#define VDEC_0_Vertical_AGC_FIELD0_KILL_END_SHIFT       10

/* FIELD0_KILL_START [09:00] - unsigned */
#define VDEC_0_Vertical_AGC_FIELD0_KILL_START_MASK      0x000003FFUL
#define VDEC_0_Vertical_AGC_FIELD0_KILL_START_ALIGN     0
#define VDEC_0_Vertical_AGC_FIELD0_KILL_START_BITS      10
#define VDEC_0_Vertical_AGC_FIELD0_KILL_START_SHIFT     0

/**********************************************************************
 * VDEC_Vertical_Misc1
 **********************************************************************/
/* reserved0 [31:31] -  */
#define VDEC_0_Vertical_Misc1_reserved0_MASK            0x80000000UL
#define VDEC_0_Vertical_Misc1_reserved0_ALIGN           0
#define VDEC_0_Vertical_Misc1_reserved0_BITS            1
#define VDEC_0_Vertical_Misc1_reserved0_SHIFT           31

/* VSYNC_THRESHOLD [30:29] - unsigned */
#define VDEC_0_Vertical_Misc1_VSYNC_THRESHOLD_MASK      0x60000000UL
#define VDEC_0_Vertical_Misc1_VSYNC_THRESHOLD_ALIGN     0
#define VDEC_0_Vertical_Misc1_VSYNC_THRESHOLD_BITS      2
#define VDEC_0_Vertical_Misc1_VSYNC_THRESHOLD_SHIFT     29

/* WMV_CSYNC_THRESHOLD [28:24] - unsigned */
#define VDEC_0_Vertical_Misc1_WMV_CSYNC_THRESHOLD_MASK  0x1F000000UL
#define VDEC_0_Vertical_Misc1_WMV_CSYNC_THRESHOLD_ALIGN 0
#define VDEC_0_Vertical_Misc1_WMV_CSYNC_THRESHOLD_BITS  5
#define VDEC_0_Vertical_Misc1_WMV_CSYNC_THRESHOLD_SHIFT 24

/* NMV_CSYNC_THRESHOLD [23:16] - unsigned */
#define VDEC_0_Vertical_Misc1_NMV_CSYNC_THRESHOLD_MASK  0x00FF0000UL
#define VDEC_0_Vertical_Misc1_NMV_CSYNC_THRESHOLD_ALIGN 0
#define VDEC_0_Vertical_Misc1_NMV_CSYNC_THRESHOLD_BITS  8
#define VDEC_0_Vertical_Misc1_NMV_CSYNC_THRESHOLD_SHIFT 16

/* FIELD0_END [15:10] - unsigned */
#define VDEC_0_Vertical_Misc1_FIELD0_END_MASK           0x0000FC00UL
#define VDEC_0_Vertical_Misc1_FIELD0_END_ALIGN          0
#define VDEC_0_Vertical_Misc1_FIELD0_END_BITS           6
#define VDEC_0_Vertical_Misc1_FIELD0_END_SHIFT          10

/* FIELD0_START [09:00] - unsigned */
#define VDEC_0_Vertical_Misc1_FIELD0_START_MASK         0x000003FFUL
#define VDEC_0_Vertical_Misc1_FIELD0_START_ALIGN        0
#define VDEC_0_Vertical_Misc1_FIELD0_START_BITS         10
#define VDEC_0_Vertical_Misc1_FIELD0_START_SHIFT        0

/**********************************************************************
 * VDEC_Vertical_Misc2
 **********************************************************************/
/* reserved0 [31:25] -  */
#define VDEC_0_Vertical_Misc2_reserved0_MASK            0xFE000000UL
#define VDEC_0_Vertical_Misc2_reserved0_ALIGN           0
#define VDEC_0_Vertical_Misc2_reserved0_BITS            7
#define VDEC_0_Vertical_Misc2_reserved0_SHIFT           25

/* FRAME_LINE_COUNT_READ_BACK [24:15] - unsigned */
#define VDEC_0_Vertical_Misc2_FRAME_LINE_COUNT_READ_BACK_MASK 0x01FF8000UL
#define VDEC_0_Vertical_Misc2_FRAME_LINE_COUNT_READ_BACK_ALIGN 0
#define VDEC_0_Vertical_Misc2_FRAME_LINE_COUNT_READ_BACK_BITS 10
#define VDEC_0_Vertical_Misc2_FRAME_LINE_COUNT_READ_BACK_SHIFT 15

/* MAX_LINE [14:08] - unsigned */
#define VDEC_0_Vertical_Misc2_MAX_LINE_MASK             0x00007F00UL
#define VDEC_0_Vertical_Misc2_MAX_LINE_ALIGN            0
#define VDEC_0_Vertical_Misc2_MAX_LINE_BITS             7
#define VDEC_0_Vertical_Misc2_MAX_LINE_SHIFT            8

/* FRAME_TOTAL_LINE [07:00] - unsigned */
#define VDEC_0_Vertical_Misc2_FRAME_TOTAL_LINE_MASK     0x000000FFUL
#define VDEC_0_Vertical_Misc2_FRAME_TOTAL_LINE_ALIGN    0
#define VDEC_0_Vertical_Misc2_FRAME_TOTAL_LINE_BITS     8
#define VDEC_0_Vertical_Misc2_FRAME_TOTAL_LINE_SHIFT    0

/**********************************************************************
 * VDEC_Horizontal_Misc
 **********************************************************************/
/* reserved0 [31:20] -  */
#define VDEC_0_Horizontal_Misc_reserved0_MASK           0xFFF00000UL
#define VDEC_0_Horizontal_Misc_reserved0_ALIGN          0
#define VDEC_0_Horizontal_Misc_reserved0_BITS           12
#define VDEC_0_Horizontal_Misc_reserved0_SHIFT          20

/* DELTA_LENGTH [19:14] - unsigned */
#define VDEC_0_Horizontal_Misc_DELTA_LENGTH_MASK        0x000FC000UL
#define VDEC_0_Horizontal_Misc_DELTA_LENGTH_ALIGN       0
#define VDEC_0_Horizontal_Misc_DELTA_LENGTH_BITS        6
#define VDEC_0_Horizontal_Misc_DELTA_LENGTH_SHIFT       14

/* HORIZONTAL_LENGTH [13:07] - unsigned */
#define VDEC_0_Horizontal_Misc_HORIZONTAL_LENGTH_MASK   0x00003F80UL
#define VDEC_0_Horizontal_Misc_HORIZONTAL_LENGTH_ALIGN  0
#define VDEC_0_Horizontal_Misc_HORIZONTAL_LENGTH_BITS   7
#define VDEC_0_Horizontal_Misc_HORIZONTAL_LENGTH_SHIFT  7

/* HSYNC_POSITION [06:00] - unsigned */
#define VDEC_0_Horizontal_Misc_HSYNC_POSITION_MASK      0x0000007FUL
#define VDEC_0_Horizontal_Misc_HSYNC_POSITION_ALIGN     0
#define VDEC_0_Horizontal_Misc_HSYNC_POSITION_BITS      7
#define VDEC_0_Horizontal_Misc_HSYNC_POSITION_SHIFT     0

/**********************************************************************
 * VDEC_SW_AGC
 **********************************************************************/
/* AGC_GAIN [31:00] - unsigned */
#define VDEC_0_SW_AGC_AGC_GAIN_MASK                     0xFFFFFFFFUL
#define VDEC_0_SW_AGC_AGC_GAIN_ALIGN                    0
#define VDEC_0_SW_AGC_AGC_GAIN_BITS                     32
#define VDEC_0_SW_AGC_AGC_GAIN_SHIFT                    0

/**********************************************************************
 * VDEC_AUTO_AGC
 **********************************************************************/
/* reserved0 [31:24] -  */
#define VDEC_0_AUTO_AGC_reserved0_MASK                  0xFF000000UL
#define VDEC_0_AUTO_AGC_reserved0_ALIGN                 0
#define VDEC_0_AUTO_AGC_reserved0_BITS                  8
#define VDEC_0_AUTO_AGC_reserved0_SHIFT                 24

/* CONSTANT_OFFSET [23:14] - unsigned */
#define VDEC_0_AUTO_AGC_CONSTANT_OFFSET_MASK            0x00FFC000UL
#define VDEC_0_AUTO_AGC_CONSTANT_OFFSET_ALIGN           0
#define VDEC_0_AUTO_AGC_CONSTANT_OFFSET_BITS            10
#define VDEC_0_AUTO_AGC_CONSTANT_OFFSET_SHIFT           14

/* AGC_TARGET [13:04] - unsigned */
#define VDEC_0_AUTO_AGC_AGC_TARGET_MASK                 0x00003FF0UL
#define VDEC_0_AUTO_AGC_AGC_TARGET_ALIGN                0
#define VDEC_0_AUTO_AGC_AGC_TARGET_BITS                 10
#define VDEC_0_AUTO_AGC_AGC_TARGET_SHIFT                4

/* AGC_GAIN [03:00] - unsigned */
#define VDEC_0_AUTO_AGC_AGC_GAIN_MASK                   0x0000000FUL
#define VDEC_0_AUTO_AGC_AGC_GAIN_ALIGN                  0
#define VDEC_0_AUTO_AGC_AGC_GAIN_BITS                   4
#define VDEC_0_AUTO_AGC_AGC_GAIN_SHIFT                  0

/**********************************************************************
 * VDEC_Sync_Luma_Blank
 **********************************************************************/
/* reserved0 [31:30] -  */
#define VDEC_0_Sync_Luma_Blank_reserved0_MASK           0xC0000000UL
#define VDEC_0_Sync_Luma_Blank_reserved0_ALIGN          0
#define VDEC_0_Sync_Luma_Blank_reserved0_BITS           2
#define VDEC_0_Sync_Luma_Blank_reserved0_SHIFT          30

/* BLANK_LEVEL [29:20] - unsigned */
#define VDEC_0_Sync_Luma_Blank_BLANK_LEVEL_MASK         0x3FF00000UL
#define VDEC_0_Sync_Luma_Blank_BLANK_LEVEL_ALIGN        0
#define VDEC_0_Sync_Luma_Blank_BLANK_LEVEL_BITS         10
#define VDEC_0_Sync_Luma_Blank_BLANK_LEVEL_SHIFT        20

/* LUMA_LEVEL [19:10] - unsigned */
#define VDEC_0_Sync_Luma_Blank_LUMA_LEVEL_MASK          0x000FFC00UL
#define VDEC_0_Sync_Luma_Blank_LUMA_LEVEL_ALIGN         0
#define VDEC_0_Sync_Luma_Blank_LUMA_LEVEL_BITS          10
#define VDEC_0_Sync_Luma_Blank_LUMA_LEVEL_SHIFT         10

/* SYNC_TIP_LEVEL [09:00] - unsigned */
#define VDEC_0_Sync_Luma_Blank_SYNC_TIP_LEVEL_MASK      0x000003FFUL
#define VDEC_0_Sync_Luma_Blank_SYNC_TIP_LEVEL_ALIGN     0
#define VDEC_0_Sync_Luma_Blank_SYNC_TIP_LEVEL_BITS      10
#define VDEC_0_Sync_Luma_Blank_SYNC_TIP_LEVEL_SHIFT     0

/**********************************************************************
 * VDEC_Burst_Ampl
 **********************************************************************/
/* reserved0 [31:30] -  */
#define VDEC_0_Burst_Ampl_reserved0_MASK                0xC0000000UL
#define VDEC_0_Burst_Ampl_reserved0_ALIGN               0
#define VDEC_0_Burst_Ampl_reserved0_BITS                2
#define VDEC_0_Burst_Ampl_reserved0_SHIFT               30

/* NOISE_LEVEL [29:20] - unsigned */
#define VDEC_0_Burst_Ampl_NOISE_LEVEL_MASK              0x3FF00000UL
#define VDEC_0_Burst_Ampl_NOISE_LEVEL_ALIGN             0
#define VDEC_0_Burst_Ampl_NOISE_LEVEL_BITS              10
#define VDEC_0_Burst_Ampl_NOISE_LEVEL_SHIFT             20

/* SLICE_LEVEL [19:10] - unsigned */
#define VDEC_0_Burst_Ampl_SLICE_LEVEL_MASK              0x000FFC00UL
#define VDEC_0_Burst_Ampl_SLICE_LEVEL_ALIGN             0
#define VDEC_0_Burst_Ampl_SLICE_LEVEL_BITS              10
#define VDEC_0_Burst_Ampl_SLICE_LEVEL_SHIFT             10

/* BURST_AMPL [09:00] - unsigned */
#define VDEC_0_Burst_Ampl_BURST_AMPL_MASK               0x000003FFUL
#define VDEC_0_Burst_Ampl_BURST_AMPL_ALIGN              0
#define VDEC_0_Burst_Ampl_BURST_AMPL_BITS               10
#define VDEC_0_Burst_Ampl_BURST_AMPL_SHIFT              0

/**********************************************************************
 * VDEC_Clamp_Control
 **********************************************************************/
/* reserved0 [31:19] -  */
#define VDEC_0_Clamp_Control_reserved0_MASK             0xFFF80000UL
#define VDEC_0_Clamp_Control_reserved0_ALIGN            0
#define VDEC_0_Clamp_Control_reserved0_BITS             13
#define VDEC_0_Clamp_Control_reserved0_SHIFT            19

/* USE_32_SAMPLES [18:18] - unsigned */
#define VDEC_0_Clamp_Control_USE_32_SAMPLES_MASK        0x00040000UL
#define VDEC_0_Clamp_Control_USE_32_SAMPLES_ALIGN       0
#define VDEC_0_Clamp_Control_USE_32_SAMPLES_BITS        1
#define VDEC_0_Clamp_Control_USE_32_SAMPLES_SHIFT       18

/* CLAMP_COMPARE [17:08] - unsigned */
#define VDEC_0_Clamp_Control_CLAMP_COMPARE_MASK         0x0003FF00UL
#define VDEC_0_Clamp_Control_CLAMP_COMPARE_ALIGN        0
#define VDEC_0_Clamp_Control_CLAMP_COMPARE_BITS         10
#define VDEC_0_Clamp_Control_CLAMP_COMPARE_SHIFT        8

/* CLAMP_START [07:00] - unsigned */
#define VDEC_0_Clamp_Control_CLAMP_START_MASK           0x000000FFUL
#define VDEC_0_Clamp_Control_CLAMP_START_ALIGN          0
#define VDEC_0_Clamp_Control_CLAMP_START_BITS           8
#define VDEC_0_Clamp_Control_CLAMP_START_SHIFT          0

/**********************************************************************
 * VDEC_Macrov_Setup
 **********************************************************************/
/* reserved0 [31:29] -  */
#define VDEC_0_Macrov_Setup_reserved0_MASK              0xE0000000UL
#define VDEC_0_Macrov_Setup_reserved0_ALIGN             0
#define VDEC_0_Macrov_Setup_reserved0_BITS              3
#define VDEC_0_Macrov_Setup_reserved0_SHIFT             29

/* MACROV_DETECT [28:28] - boolean */
#define VDEC_0_Macrov_Setup_MACROV_DETECT_MASK          0x10000000UL
#define VDEC_0_Macrov_Setup_MACROV_DETECT_ALIGN         0
#define VDEC_0_Macrov_Setup_MACROV_DETECT_BITS          1
#define VDEC_0_Macrov_Setup_MACROV_DETECT_SHIFT         28

/* CLAMP_GAIN [27:24] - unsigned */
#define VDEC_0_Macrov_Setup_CLAMP_GAIN_MASK             0x0F000000UL
#define VDEC_0_Macrov_Setup_CLAMP_GAIN_ALIGN            0
#define VDEC_0_Macrov_Setup_CLAMP_GAIN_BITS             4
#define VDEC_0_Macrov_Setup_CLAMP_GAIN_SHIFT            24

/* PSEUDO_THRESHOLD [23:20] - unsigned */
#define VDEC_0_Macrov_Setup_PSEUDO_THRESHOLD_MASK       0x00F00000UL
#define VDEC_0_Macrov_Setup_PSEUDO_THRESHOLD_ALIGN      0
#define VDEC_0_Macrov_Setup_PSEUDO_THRESHOLD_BITS       4
#define VDEC_0_Macrov_Setup_PSEUDO_THRESHOLD_SHIFT      20

/* PSEUDO_LINES [19:15] - unsigned */
#define VDEC_0_Macrov_Setup_PSEUDO_LINES_MASK           0x000F8000UL
#define VDEC_0_Macrov_Setup_PSEUDO_LINES_ALIGN          0
#define VDEC_0_Macrov_Setup_PSEUDO_LINES_BITS           5
#define VDEC_0_Macrov_Setup_PSEUDO_LINES_SHIFT          15

/* FIELD1_PSEUDO_START [14:05] - unsigned */
#define VDEC_0_Macrov_Setup_FIELD1_PSEUDO_START_MASK    0x00007FE0UL
#define VDEC_0_Macrov_Setup_FIELD1_PSEUDO_START_ALIGN   0
#define VDEC_0_Macrov_Setup_FIELD1_PSEUDO_START_BITS    10
#define VDEC_0_Macrov_Setup_FIELD1_PSEUDO_START_SHIFT   5

/* FIELD0_PSEUDO_START [04:00] - unsigned */
#define VDEC_0_Macrov_Setup_FIELD0_PSEUDO_START_MASK    0x0000001FUL
#define VDEC_0_Macrov_Setup_FIELD0_PSEUDO_START_ALIGN   0
#define VDEC_0_Macrov_Setup_FIELD0_PSEUDO_START_BITS    5
#define VDEC_0_Macrov_Setup_FIELD0_PSEUDO_START_SHIFT   0

/**********************************************************************
 * VDEC_YC_Separation
 **********************************************************************/
/* reserved0 [31:20] -  */
#define VDEC_0_YC_Separation_reserved0_MASK             0xFFF00000UL
#define VDEC_0_YC_Separation_reserved0_ALIGN            0
#define VDEC_0_YC_Separation_reserved0_BITS             12
#define VDEC_0_YC_Separation_reserved0_SHIFT            20

/* CHROMA_2_LINE_THRESHOLD [19:13] - unsigned */
#define VDEC_0_YC_Separation_CHROMA_2_LINE_THRESHOLD_MASK 0x000FE000UL
#define VDEC_0_YC_Separation_CHROMA_2_LINE_THRESHOLD_ALIGN 0
#define VDEC_0_YC_Separation_CHROMA_2_LINE_THRESHOLD_BITS 7
#define VDEC_0_YC_Separation_CHROMA_2_LINE_THRESHOLD_SHIFT 13

/* CHROMA_3_LINE_THRESHOLD [12:06] - unsigned */
#define VDEC_0_YC_Separation_CHROMA_3_LINE_THRESHOLD_MASK 0x00001FC0UL
#define VDEC_0_YC_Separation_CHROMA_3_LINE_THRESHOLD_ALIGN 0
#define VDEC_0_YC_Separation_CHROMA_3_LINE_THRESHOLD_BITS 7
#define VDEC_0_YC_Separation_CHROMA_3_LINE_THRESHOLD_SHIFT 6

/* LUMA_012DIFF_THRESHOLD [05:04] - unsigned */
#define VDEC_0_YC_Separation_LUMA_012DIFF_THRESHOLD_MASK 0x00000030UL
#define VDEC_0_YC_Separation_LUMA_012DIFF_THRESHOLD_ALIGN 0
#define VDEC_0_YC_Separation_LUMA_012DIFF_THRESHOLD_BITS 2
#define VDEC_0_YC_Separation_LUMA_012DIFF_THRESHOLD_SHIFT 4

/* LUMA_02DIFF_THRESHOLD [03:02] - unsigned */
#define VDEC_0_YC_Separation_LUMA_02DIFF_THRESHOLD_MASK 0x0000000CUL
#define VDEC_0_YC_Separation_LUMA_02DIFF_THRESHOLD_ALIGN 0
#define VDEC_0_YC_Separation_LUMA_02DIFF_THRESHOLD_BITS 2
#define VDEC_0_YC_Separation_LUMA_02DIFF_THRESHOLD_SHIFT 2

/* LUMA_SWITCH_VALUE [01:00] - unsigned */
#define VDEC_0_YC_Separation_LUMA_SWITCH_VALUE_MASK     0x00000003UL
#define VDEC_0_YC_Separation_LUMA_SWITCH_VALUE_ALIGN    0
#define VDEC_0_YC_Separation_LUMA_SWITCH_VALUE_BITS     2
#define VDEC_0_YC_Separation_LUMA_SWITCH_VALUE_SHIFT    0

/**********************************************************************
 * VDEC_Chroma_Suppression
 **********************************************************************/
/* reserved0 [31:19] -  */
#define VDEC_0_Chroma_Suppression_reserved0_MASK        0xFFF80000UL
#define VDEC_0_Chroma_Suppression_reserved0_ALIGN       0
#define VDEC_0_Chroma_Suppression_reserved0_BITS        13
#define VDEC_0_Chroma_Suppression_reserved0_SHIFT       19

/* CHROMA_SUPPRESS_HORIZONTAL [18:17] - unsigned */
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_HORIZONTAL_MASK 0x00060000UL
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_HORIZONTAL_ALIGN 0
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_HORIZONTAL_BITS 2
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_HORIZONTAL_SHIFT 17
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_HORIZONTAL_NEVER 0
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_HORIZONTAL_ALWAYS 1
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_HORIZONTAL_NO_COLOR 2

/* CHROMA_SUPPRESS_COEFFICIENT_4 [16:15] - unsigned */
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_4_MASK 0x00018000UL
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_4_ALIGN 0
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_4_BITS 2
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_4_SHIFT 15

/* CHROMA_SUPPRESS_COEFFICIENT_3 [14:13] - unsigned */
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_3_MASK 0x00006000UL
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_3_ALIGN 0
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_3_BITS 2
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_3_SHIFT 13

/* CHROMA_SUPPRESS_COEFFICIENT_2 [12:11] - unsigned */
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_2_MASK 0x00001800UL
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_2_ALIGN 0
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_2_BITS 2
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_2_SHIFT 11

/* CHROMA_SUPPRESS_COEFFICIENT_1 [10:09] - unsigned */
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_1_MASK 0x00000600UL
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_1_ALIGN 0
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_1_BITS 2
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_1_SHIFT 9

/* CHROMA_SUPPRESS_COEFFICIENT_0 [08:07] - unsigned */
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_0_MASK 0x00000180UL
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_0_ALIGN 0
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_0_BITS 2
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_COEFFICIENT_0_SHIFT 7

/* CHROMA_SUPPRESS_STRONG_COLOR_THRESHOLD [06:05] - unsigned */
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_STRONG_COLOR_THRESHOLD_MASK 0x00000060UL
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_STRONG_COLOR_THRESHOLD_ALIGN 0
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_STRONG_COLOR_THRESHOLD_BITS 2
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_STRONG_COLOR_THRESHOLD_SHIFT 5

/* CHROMA_SUPPRESS_CORRELATION_THRESHOLD [04:03] - unsigned */
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_CORRELATION_THRESHOLD_MASK 0x00000018UL
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_CORRELATION_THRESHOLD_ALIGN 0
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_CORRELATION_THRESHOLD_BITS 2
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_CORRELATION_THRESHOLD_SHIFT 3

/* CHROMA_SUPPRESS_TRANSITION_THRESHOLD [02:01] - unsigned */
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_TRANSITION_THRESHOLD_MASK 0x00000006UL
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_TRANSITION_THRESHOLD_ALIGN 0
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_TRANSITION_THRESHOLD_BITS 2
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESS_TRANSITION_THRESHOLD_SHIFT 1

/* CHROMA_SUPPRESSION [00:00] - unsigned */
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESSION_MASK 0x00000001UL
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESSION_ALIGN 0
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESSION_BITS 1
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESSION_SHIFT 0
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESSION_OFF 0
#define VDEC_0_Chroma_Suppression_CHROMA_SUPPRESSION_ON 1

/**********************************************************************
 * VDEC_Picture_Control
 **********************************************************************/
/* SATURATION_VALUE [31:25] - unsigned */
#define VDEC_0_Picture_Control_SATURATION_VALUE_MASK    0xFE000000UL
#define VDEC_0_Picture_Control_SATURATION_VALUE_ALIGN   0
#define VDEC_0_Picture_Control_SATURATION_VALUE_BITS    7
#define VDEC_0_Picture_Control_SATURATION_VALUE_SHIFT   25

/* TINT_VALUE [24:17] - unsigned */
#define VDEC_0_Picture_Control_TINT_VALUE_MASK          0x01FE0000UL
#define VDEC_0_Picture_Control_TINT_VALUE_ALIGN         0
#define VDEC_0_Picture_Control_TINT_VALUE_BITS          8
#define VDEC_0_Picture_Control_TINT_VALUE_SHIFT         17

/* CONTRAST_VALUE [16:08] - unsigned */
#define VDEC_0_Picture_Control_CONTRAST_VALUE_MASK      0x0001FF00UL
#define VDEC_0_Picture_Control_CONTRAST_VALUE_ALIGN     0
#define VDEC_0_Picture_Control_CONTRAST_VALUE_BITS      9
#define VDEC_0_Picture_Control_CONTRAST_VALUE_SHIFT     8

/* BRIGHTNESS_VALUE [07:00] - unsigned */
#define VDEC_0_Picture_Control_BRIGHTNESS_VALUE_MASK    0x000000FFUL
#define VDEC_0_Picture_Control_BRIGHTNESS_VALUE_ALIGN   0
#define VDEC_0_Picture_Control_BRIGHTNESS_VALUE_BITS    8
#define VDEC_0_Picture_Control_BRIGHTNESS_VALUE_SHIFT   0

/**********************************************************************
 * VDEC_Picture_Adjust
 **********************************************************************/
/* reserved0 [31:19] -  */
#define VDEC_0_Picture_Adjust_reserved0_MASK            0xFFF80000UL
#define VDEC_0_Picture_Adjust_reserved0_ALIGN           0
#define VDEC_0_Picture_Adjust_reserved0_BITS            13
#define VDEC_0_Picture_Adjust_reserved0_SHIFT           19

/* AUTO_COLOR_DETECT_THRESHOLD [18:11] - unsigned */
#define VDEC_0_Picture_Adjust_AUTO_COLOR_DETECT_THRESHOLD_MASK 0x0007F800UL
#define VDEC_0_Picture_Adjust_AUTO_COLOR_DETECT_THRESHOLD_ALIGN 0
#define VDEC_0_Picture_Adjust_AUTO_COLOR_DETECT_THRESHOLD_BITS 8
#define VDEC_0_Picture_Adjust_AUTO_COLOR_DETECT_THRESHOLD_SHIFT 11

/* CORING_MAX_X [10:07] - unsigned */
#define VDEC_0_Picture_Adjust_CORING_MAX_X_MASK         0x00000780UL
#define VDEC_0_Picture_Adjust_CORING_MAX_X_ALIGN        0
#define VDEC_0_Picture_Adjust_CORING_MAX_X_BITS         4
#define VDEC_0_Picture_Adjust_CORING_MAX_X_SHIFT        7

/* CORING_MAX_Y [06:01] - unsigned */
#define VDEC_0_Picture_Adjust_CORING_MAX_Y_MASK         0x0000007EUL
#define VDEC_0_Picture_Adjust_CORING_MAX_Y_ALIGN        0
#define VDEC_0_Picture_Adjust_CORING_MAX_Y_BITS         6
#define VDEC_0_Picture_Adjust_CORING_MAX_Y_SHIFT        1

/* CORING_DETAIL [00:00] - unsigned */
#define VDEC_0_Picture_Adjust_CORING_DETAIL_MASK        0x00000001UL
#define VDEC_0_Picture_Adjust_CORING_DETAIL_ALIGN       0
#define VDEC_0_Picture_Adjust_CORING_DETAIL_BITS        1
#define VDEC_0_Picture_Adjust_CORING_DETAIL_SHIFT       0

/**********************************************************************
 * VDEC_Subcarrier_Freq
 **********************************************************************/
/* FREQUENCY [31:00] - unsigned */
#define VDEC_0_Subcarrier_Freq_FREQUENCY_MASK           0xFFFFFFFFUL
#define VDEC_0_Subcarrier_Freq_FREQUENCY_ALIGN          0
#define VDEC_0_Subcarrier_Freq_FREQUENCY_BITS           32
#define VDEC_0_Subcarrier_Freq_FREQUENCY_SHIFT          0

/**********************************************************************
 * VDEC_Luma_Filter_012
 **********************************************************************/
/* reserved0 [31:27] -  */
#define VDEC_0_Luma_Filter_012_reserved0_MASK           0xF8000000UL
#define VDEC_0_Luma_Filter_012_reserved0_ALIGN          0
#define VDEC_0_Luma_Filter_012_reserved0_BITS           5
#define VDEC_0_Luma_Filter_012_reserved0_SHIFT          27

/* TAP2 [26:18] - unsigned */
#define VDEC_0_Luma_Filter_012_TAP2_MASK                0x07FC0000UL
#define VDEC_0_Luma_Filter_012_TAP2_ALIGN               0
#define VDEC_0_Luma_Filter_012_TAP2_BITS                9
#define VDEC_0_Luma_Filter_012_TAP2_SHIFT               18

/* TAP1 [17:09] - unsigned */
#define VDEC_0_Luma_Filter_012_TAP1_MASK                0x0003FE00UL
#define VDEC_0_Luma_Filter_012_TAP1_ALIGN               0
#define VDEC_0_Luma_Filter_012_TAP1_BITS                9
#define VDEC_0_Luma_Filter_012_TAP1_SHIFT               9

/* TAP0 [08:00] - unsigned */
#define VDEC_0_Luma_Filter_012_TAP0_MASK                0x000001FFUL
#define VDEC_0_Luma_Filter_012_TAP0_ALIGN               0
#define VDEC_0_Luma_Filter_012_TAP0_BITS                9
#define VDEC_0_Luma_Filter_012_TAP0_SHIFT               0

/**********************************************************************
 * VDEC_Luma_Filter_345
 **********************************************************************/
/* reserved0 [31:27] -  */
#define VDEC_0_Luma_Filter_345_reserved0_MASK           0xF8000000UL
#define VDEC_0_Luma_Filter_345_reserved0_ALIGN          0
#define VDEC_0_Luma_Filter_345_reserved0_BITS           5
#define VDEC_0_Luma_Filter_345_reserved0_SHIFT          27

/* TAP5 [26:18] - unsigned */
#define VDEC_0_Luma_Filter_345_TAP5_MASK                0x07FC0000UL
#define VDEC_0_Luma_Filter_345_TAP5_ALIGN               0
#define VDEC_0_Luma_Filter_345_TAP5_BITS                9
#define VDEC_0_Luma_Filter_345_TAP5_SHIFT               18

/* TAP4 [17:09] - unsigned */
#define VDEC_0_Luma_Filter_345_TAP4_MASK                0x0003FE00UL
#define VDEC_0_Luma_Filter_345_TAP4_ALIGN               0
#define VDEC_0_Luma_Filter_345_TAP4_BITS                9
#define VDEC_0_Luma_Filter_345_TAP4_SHIFT               9

/* TAP3 [08:00] - unsigned */
#define VDEC_0_Luma_Filter_345_TAP3_MASK                0x000001FFUL
#define VDEC_0_Luma_Filter_345_TAP3_ALIGN               0
#define VDEC_0_Luma_Filter_345_TAP3_BITS                9
#define VDEC_0_Luma_Filter_345_TAP3_SHIFT               0

/**********************************************************************
 * VDEC_Luma_filter_6
 **********************************************************************/
/* reserved0 [31:11] -  */
#define VDEC_0_Luma_filter_6_reserved0_MASK             0xFFFFF800UL
#define VDEC_0_Luma_filter_6_reserved0_ALIGN            0
#define VDEC_0_Luma_filter_6_reserved0_BITS             21
#define VDEC_0_Luma_filter_6_reserved0_SHIFT            11

/* MODE [10:10] - boolean */
#define VDEC_0_Luma_filter_6_MODE_MASK                  0x00000400UL
#define VDEC_0_Luma_filter_6_MODE_ALIGN                 0
#define VDEC_0_Luma_filter_6_MODE_BITS                  1
#define VDEC_0_Luma_filter_6_MODE_SHIFT                 10
#define VDEC_0_Luma_filter_6_MODE_GENERIC               0
#define VDEC_0_Luma_filter_6_MODE_ANTIALIAS             1

/* BYPASS [09:09] - boolean */
#define VDEC_0_Luma_filter_6_BYPASS_MASK                0x00000200UL
#define VDEC_0_Luma_filter_6_BYPASS_ALIGN               0
#define VDEC_0_Luma_filter_6_BYPASS_BITS                1
#define VDEC_0_Luma_filter_6_BYPASS_SHIFT               9

/* TAP6 [08:00] - unsigned */
#define VDEC_0_Luma_filter_6_TAP6_MASK                  0x000001FFUL
#define VDEC_0_Luma_filter_6_TAP6_ALIGN                 0
#define VDEC_0_Luma_filter_6_TAP6_BITS                  9
#define VDEC_0_Luma_filter_6_TAP6_SHIFT                 0

/**********************************************************************
 * VDEC_Besrc_fifo
 **********************************************************************/
/* DISABLE_SRC [31:31] - boolean */
#define VDEC_0_Besrc_fifo_DISABLE_SRC_MASK              0x80000000UL
#define VDEC_0_Besrc_fifo_DISABLE_SRC_ALIGN             0
#define VDEC_0_Besrc_fifo_DISABLE_SRC_BITS              1
#define VDEC_0_Besrc_fifo_DISABLE_SRC_SHIFT             31

/* CARRIER_CORING [30:30] - unsigned */
#define VDEC_0_Besrc_fifo_CARRIER_CORING_MASK           0x40000000UL
#define VDEC_0_Besrc_fifo_CARRIER_CORING_ALIGN          0
#define VDEC_0_Besrc_fifo_CARRIER_CORING_BITS           1
#define VDEC_0_Besrc_fifo_CARRIER_CORING_SHIFT          30
#define VDEC_0_Besrc_fifo_CARRIER_CORING_OFF            0
#define VDEC_0_Besrc_fifo_CARRIER_CORING_ON             1

/* BESRC_START [29:28] - unsigned */
#define VDEC_0_Besrc_fifo_BESRC_START_MASK              0x30000000UL
#define VDEC_0_Besrc_fifo_BESRC_START_ALIGN             0
#define VDEC_0_Besrc_fifo_BESRC_START_BITS              2
#define VDEC_0_Besrc_fifo_BESRC_START_SHIFT             28

/* CWRITE_START [27:19] - unsigned */
#define VDEC_0_Besrc_fifo_CWRITE_START_MASK             0x0FF80000UL
#define VDEC_0_Besrc_fifo_CWRITE_START_ALIGN            0
#define VDEC_0_Besrc_fifo_CWRITE_START_BITS             9
#define VDEC_0_Besrc_fifo_CWRITE_START_SHIFT            19

/* YWRITE_START [18:10] - unsigned */
#define VDEC_0_Besrc_fifo_YWRITE_START_MASK             0x0007FC00UL
#define VDEC_0_Besrc_fifo_YWRITE_START_ALIGN            0
#define VDEC_0_Besrc_fifo_YWRITE_START_BITS             9
#define VDEC_0_Besrc_fifo_YWRITE_START_SHIFT            10

/* READ_START [09:00] - unsigned */
#define VDEC_0_Besrc_fifo_READ_START_MASK               0x000003FFUL
#define VDEC_0_Besrc_fifo_READ_START_ALIGN              0
#define VDEC_0_Besrc_fifo_READ_START_BITS               10
#define VDEC_0_Besrc_fifo_READ_START_SHIFT              0

/**********************************************************************
 * VDEC_Horizontal_Scaling
 **********************************************************************/
/* reserved0 [31:28] -  */
#define VDEC_0_Horizontal_Scaling_reserved0_MASK        0xF0000000UL
#define VDEC_0_Horizontal_Scaling_reserved0_ALIGN       0
#define VDEC_0_Horizontal_Scaling_reserved0_BITS        4
#define VDEC_0_Horizontal_Scaling_reserved0_SHIFT       28

/* PIXEL_OFFSET [27:20] - unsigned */
#define VDEC_0_Horizontal_Scaling_PIXEL_OFFSET_MASK     0x0FF00000UL
#define VDEC_0_Horizontal_Scaling_PIXEL_OFFSET_ALIGN    0
#define VDEC_0_Horizontal_Scaling_PIXEL_OFFSET_BITS     8
#define VDEC_0_Horizontal_Scaling_PIXEL_OFFSET_SHIFT    20

/* OUTPUT_SIZE [19:10] - unsigned */
#define VDEC_0_Horizontal_Scaling_OUTPUT_SIZE_MASK      0x000FFC00UL
#define VDEC_0_Horizontal_Scaling_OUTPUT_SIZE_ALIGN     0
#define VDEC_0_Horizontal_Scaling_OUTPUT_SIZE_BITS      10
#define VDEC_0_Horizontal_Scaling_OUTPUT_SIZE_SHIFT     10

/* RATIO [09:00] - unsigned */
#define VDEC_0_Horizontal_Scaling_RATIO_MASK            0x000003FFUL
#define VDEC_0_Horizontal_Scaling_RATIO_ALIGN           0
#define VDEC_0_Horizontal_Scaling_RATIO_BITS            10
#define VDEC_0_Horizontal_Scaling_RATIO_SHIFT           0

/**********************************************************************
 * VDEC_Acgc
 **********************************************************************/
/* reserved0 [31:29] -  */
#define VDEC_0_Acgc_reserved0_MASK                      0xE0000000UL
#define VDEC_0_Acgc_reserved0_ALIGN                     0
#define VDEC_0_Acgc_reserved0_BITS                      3
#define VDEC_0_Acgc_reserved0_SHIFT                     29

/* LOCK_STATUS [28:28] - boolean */
#define VDEC_0_Acgc_LOCK_STATUS_MASK                    0x10000000UL
#define VDEC_0_Acgc_LOCK_STATUS_ALIGN                   0
#define VDEC_0_Acgc_LOCK_STATUS_BITS                    1
#define VDEC_0_Acgc_LOCK_STATUS_SHIFT                   28

/* FILTER_GAIN [27:25] - unsigned */
#define VDEC_0_Acgc_FILTER_GAIN_MASK                    0x0E000000UL
#define VDEC_0_Acgc_FILTER_GAIN_ALIGN                   0
#define VDEC_0_Acgc_FILTER_GAIN_BITS                    3
#define VDEC_0_Acgc_FILTER_GAIN_SHIFT                   25

/* GAIN_SELECT [24:23] - unsigned */
#define VDEC_0_Acgc_GAIN_SELECT_MASK                    0x01800000UL
#define VDEC_0_Acgc_GAIN_SELECT_ALIGN                   0
#define VDEC_0_Acgc_GAIN_SELECT_BITS                    2
#define VDEC_0_Acgc_GAIN_SELECT_SHIFT                   23

/* ACC_ENABLE [22:22] - boolean */
#define VDEC_0_Acgc_ACC_ENABLE_MASK                     0x00400000UL
#define VDEC_0_Acgc_ACC_ENABLE_ALIGN                    0
#define VDEC_0_Acgc_ACC_ENABLE_BITS                     1
#define VDEC_0_Acgc_ACC_ENABLE_SHIFT                    22

/* THRESHOLD [21:18] - unsigned */
#define VDEC_0_Acgc_THRESHOLD_MASK                      0x003C0000UL
#define VDEC_0_Acgc_THRESHOLD_ALIGN                     0
#define VDEC_0_Acgc_THRESHOLD_BITS                      4
#define VDEC_0_Acgc_THRESHOLD_SHIFT                     18

/* REFERENCE [17:10] - unsigned */
#define VDEC_0_Acgc_REFERENCE_MASK                      0x0003FC00UL
#define VDEC_0_Acgc_REFERENCE_ALIGN                     0
#define VDEC_0_Acgc_REFERENCE_BITS                      8
#define VDEC_0_Acgc_REFERENCE_SHIFT                     10

/* TYPE [09:09] - boolean */
#define VDEC_0_Acgc_TYPE_MASK                           0x00000200UL
#define VDEC_0_Acgc_TYPE_ALIGN                          0
#define VDEC_0_Acgc_TYPE_BITS                           1
#define VDEC_0_Acgc_TYPE_SHIFT                          9
#define VDEC_0_Acgc_TYPE_MANUAL                         0
#define VDEC_0_Acgc_TYPE_AUTO                           1

/* SW_GAIN [08:00] - unsigned */
#define VDEC_0_Acgc_SW_GAIN_MASK                        0x000001FFUL
#define VDEC_0_Acgc_SW_GAIN_ALIGN                       0
#define VDEC_0_Acgc_SW_GAIN_BITS                        9
#define VDEC_0_Acgc_SW_GAIN_SHIFT                       0

/**********************************************************************
 * VDEC_Chroma_lock
 **********************************************************************/
/* ACCUMULATOR_VALUE [31:16] - unsigned */
#define VDEC_0_Chroma_lock_ACCUMULATOR_VALUE_MASK       0xFFFF0000UL
#define VDEC_0_Chroma_lock_ACCUMULATOR_VALUE_ALIGN      0
#define VDEC_0_Chroma_lock_ACCUMULATOR_VALUE_BITS       16
#define VDEC_0_Chroma_lock_ACCUMULATOR_VALUE_SHIFT      16

/* STATUS [15:15] - boolean */
#define VDEC_0_Chroma_lock_STATUS_MASK                  0x00008000UL
#define VDEC_0_Chroma_lock_STATUS_ALIGN                 0
#define VDEC_0_Chroma_lock_STATUS_BITS                  1
#define VDEC_0_Chroma_lock_STATUS_SHIFT                 15

/* FILTER_GAIN [14:11] - unsigned */
#define VDEC_0_Chroma_lock_FILTER_GAIN_MASK             0x00007800UL
#define VDEC_0_Chroma_lock_FILTER_GAIN_ALIGN            0
#define VDEC_0_Chroma_lock_FILTER_GAIN_BITS             4
#define VDEC_0_Chroma_lock_FILTER_GAIN_SHIFT            11

/* GAIN_SELECT [10:09] - unsigned */
#define VDEC_0_Chroma_lock_GAIN_SELECT_MASK             0x00000600UL
#define VDEC_0_Chroma_lock_GAIN_SELECT_ALIGN            0
#define VDEC_0_Chroma_lock_GAIN_SELECT_BITS             2
#define VDEC_0_Chroma_lock_GAIN_SELECT_SHIFT            9

/* ACC_ENABLE [08:08] - boolean */
#define VDEC_0_Chroma_lock_ACC_ENABLE_MASK              0x00000100UL
#define VDEC_0_Chroma_lock_ACC_ENABLE_ALIGN             0
#define VDEC_0_Chroma_lock_ACC_ENABLE_BITS              1
#define VDEC_0_Chroma_lock_ACC_ENABLE_SHIFT             8

/* THRESHOLD [07:04] - unsigned */
#define VDEC_0_Chroma_lock_THRESHOLD_MASK               0x000000F0UL
#define VDEC_0_Chroma_lock_THRESHOLD_ALIGN              0
#define VDEC_0_Chroma_lock_THRESHOLD_BITS               4
#define VDEC_0_Chroma_lock_THRESHOLD_SHIFT              4

/* MODE [03:03] - boolean */
#define VDEC_0_Chroma_lock_MODE_MASK                    0x00000008UL
#define VDEC_0_Chroma_lock_MODE_ALIGN                   0
#define VDEC_0_Chroma_lock_MODE_BITS                    1
#define VDEC_0_Chroma_lock_MODE_SHIFT                   3
#define VDEC_0_Chroma_lock_MODE_PLL                     0
#define VDEC_0_Chroma_lock_MODE_LOCK                    1

/* LOOP_GAIN [02:00] - unsigned */
#define VDEC_0_Chroma_lock_LOOP_GAIN_MASK               0x00000007UL
#define VDEC_0_Chroma_lock_LOOP_GAIN_ALIGN              0
#define VDEC_0_Chroma_lock_LOOP_GAIN_BITS               3
#define VDEC_0_Chroma_lock_LOOP_GAIN_SHIFT              0

/**********************************************************************
 * VDEC_Line_lock
 **********************************************************************/
/* ACCUMULATOR_VALUE [31:22] - unsigned */
#define VDEC_0_Line_lock_ACCUMULATOR_VALUE_MASK         0xFFC00000UL
#define VDEC_0_Line_lock_ACCUMULATOR_VALUE_ALIGN        0
#define VDEC_0_Line_lock_ACCUMULATOR_VALUE_BITS         10
#define VDEC_0_Line_lock_ACCUMULATOR_VALUE_SHIFT        22

/* STATUS [21:21] - boolean */
#define VDEC_0_Line_lock_STATUS_MASK                    0x00200000UL
#define VDEC_0_Line_lock_STATUS_ALIGN                   0
#define VDEC_0_Line_lock_STATUS_BITS                    1
#define VDEC_0_Line_lock_STATUS_SHIFT                   21

/* FILTER_GAIN [20:17] - unsigned */
#define VDEC_0_Line_lock_FILTER_GAIN_MASK               0x001E0000UL
#define VDEC_0_Line_lock_FILTER_GAIN_ALIGN              0
#define VDEC_0_Line_lock_FILTER_GAIN_BITS               4
#define VDEC_0_Line_lock_FILTER_GAIN_SHIFT              17

/* GAIN_SELECT [16:14] - unsigned */
#define VDEC_0_Line_lock_GAIN_SELECT_MASK               0x0001C000UL
#define VDEC_0_Line_lock_GAIN_SELECT_ALIGN              0
#define VDEC_0_Line_lock_GAIN_SELECT_BITS               3
#define VDEC_0_Line_lock_GAIN_SELECT_SHIFT              14

/* ACC_ENABLE [13:13] - boolean */
#define VDEC_0_Line_lock_ACC_ENABLE_MASK                0x00002000UL
#define VDEC_0_Line_lock_ACC_ENABLE_ALIGN               0
#define VDEC_0_Line_lock_ACC_ENABLE_BITS                1
#define VDEC_0_Line_lock_ACC_ENABLE_SHIFT               13

/* THRESHOLD [12:11] - unsigned */
#define VDEC_0_Line_lock_THRESHOLD_MASK                 0x00001800UL
#define VDEC_0_Line_lock_THRESHOLD_ALIGN                0
#define VDEC_0_Line_lock_THRESHOLD_BITS                 2
#define VDEC_0_Line_lock_THRESHOLD_SHIFT                11

/* GAIN [10:08] - unsigned */
#define VDEC_0_Line_lock_GAIN_MASK                      0x00000700UL
#define VDEC_0_Line_lock_GAIN_ALIGN                     0
#define VDEC_0_Line_lock_GAIN_BITS                      3
#define VDEC_0_Line_lock_GAIN_SHIFT                     8

/* SAMPLES [07:00] - unsigned */
#define VDEC_0_Line_lock_SAMPLES_MASK                   0x000000FFUL
#define VDEC_0_Line_lock_SAMPLES_ALIGN                  0
#define VDEC_0_Line_lock_SAMPLES_BITS                   8
#define VDEC_0_Line_lock_SAMPLES_SHIFT                  0

/**********************************************************************
 * VDEC_Burst_Gate
 **********************************************************************/
/* NARROW_END [31:24] - unsigned */
#define VDEC_0_Burst_Gate_NARROW_END_MASK               0xFF000000UL
#define VDEC_0_Burst_Gate_NARROW_END_ALIGN              0
#define VDEC_0_Burst_Gate_NARROW_END_BITS               8
#define VDEC_0_Burst_Gate_NARROW_END_SHIFT              24

/* NARROW_START [23:16] - unsigned */
#define VDEC_0_Burst_Gate_NARROW_START_MASK             0x00FF0000UL
#define VDEC_0_Burst_Gate_NARROW_START_ALIGN            0
#define VDEC_0_Burst_Gate_NARROW_START_BITS             8
#define VDEC_0_Burst_Gate_NARROW_START_SHIFT            16

/* WIDE_END [15:08] - unsigned */
#define VDEC_0_Burst_Gate_WIDE_END_MASK                 0x0000FF00UL
#define VDEC_0_Burst_Gate_WIDE_END_ALIGN                0
#define VDEC_0_Burst_Gate_WIDE_END_BITS                 8
#define VDEC_0_Burst_Gate_WIDE_END_SHIFT                8

/* WIDE_START [07:00] - unsigned */
#define VDEC_0_Burst_Gate_WIDE_START_MASK               0x000000FFUL
#define VDEC_0_Burst_Gate_WIDE_START_ALIGN              0
#define VDEC_0_Burst_Gate_WIDE_START_BITS               8
#define VDEC_0_Burst_Gate_WIDE_START_SHIFT              0

/**********************************************************************
 * VDEC_Vit_agc1
 **********************************************************************/
/* reserved0 [31:21] -  */
#define VDEC_0_Vit_agc1_reserved0_MASK                  0xFFE00000UL
#define VDEC_0_Vit_agc1_reserved0_ALIGN                 0
#define VDEC_0_Vit_agc1_reserved0_BITS                  11
#define VDEC_0_Vit_agc1_reserved0_SHIFT                 21

/* SWITCH [20:20] - boolean */
#define VDEC_0_Vit_agc1_SWITCH_MASK                     0x00100000UL
#define VDEC_0_Vit_agc1_SWITCH_ALIGN                    0
#define VDEC_0_Vit_agc1_SWITCH_BITS                     1
#define VDEC_0_Vit_agc1_SWITCH_SHIFT                    20

/* FIELD0_END [19:10] - unsigned */
#define VDEC_0_Vit_agc1_FIELD0_END_MASK                 0x000FFC00UL
#define VDEC_0_Vit_agc1_FIELD0_END_ALIGN                0
#define VDEC_0_Vit_agc1_FIELD0_END_BITS                 10
#define VDEC_0_Vit_agc1_FIELD0_END_SHIFT                10

/* FIELD0_START [09:00] - unsigned */
#define VDEC_0_Vit_agc1_FIELD0_START_MASK               0x000003FFUL
#define VDEC_0_Vit_agc1_FIELD0_START_ALIGN              0
#define VDEC_0_Vit_agc1_FIELD0_START_BITS               10
#define VDEC_0_Vit_agc1_FIELD0_START_SHIFT              0

/**********************************************************************
 * VDEC_Vit_agc2
 **********************************************************************/
/* reserved0 [31:20] -  */
#define VDEC_0_Vit_agc2_reserved0_MASK                  0xFFF00000UL
#define VDEC_0_Vit_agc2_reserved0_ALIGN                 0
#define VDEC_0_Vit_agc2_reserved0_BITS                  12
#define VDEC_0_Vit_agc2_reserved0_SHIFT                 20

/* FIELD1_END [19:10] - unsigned */
#define VDEC_0_Vit_agc2_FIELD1_END_MASK                 0x000FFC00UL
#define VDEC_0_Vit_agc2_FIELD1_END_ALIGN                0
#define VDEC_0_Vit_agc2_FIELD1_END_BITS                 10
#define VDEC_0_Vit_agc2_FIELD1_END_SHIFT                10

/* FIELD1_START [09:00] - unsigned */
#define VDEC_0_Vit_agc2_FIELD1_START_MASK               0x000003FFUL
#define VDEC_0_Vit_agc2_FIELD1_START_ALIGN              0
#define VDEC_0_Vit_agc2_FIELD1_START_BITS               10
#define VDEC_0_Vit_agc2_FIELD1_START_SHIFT              0

/**********************************************************************
 * VDEC_Trick_mode
 **********************************************************************/
/* PROGRESSIVE_FIELD_ID [31:31] - boolean */
#define VDEC_0_Trick_mode_PROGRESSIVE_FIELD_ID_MASK     0x80000000UL
#define VDEC_0_Trick_mode_PROGRESSIVE_FIELD_ID_ALIGN    0
#define VDEC_0_Trick_mode_PROGRESSIVE_FIELD_ID_BITS     1
#define VDEC_0_Trick_mode_PROGRESSIVE_FIELD_ID_SHIFT    31
#define VDEC_0_Trick_mode_PROGRESSIVE_FIELD_ID_TOGGLE   0
#define VDEC_0_Trick_mode_PROGRESSIVE_FIELD_ID_CONSTANT 1

/* VSYNC_WIDTH [30:26] - unsigned */
#define VDEC_0_Trick_mode_VSYNC_WIDTH_MASK              0x7C000000UL
#define VDEC_0_Trick_mode_VSYNC_WIDTH_ALIGN             0
#define VDEC_0_Trick_mode_VSYNC_WIDTH_BITS              5
#define VDEC_0_Trick_mode_VSYNC_WIDTH_SHIFT             26

/* VSYNC_START [25:16] - unsigned */
#define VDEC_0_Trick_mode_VSYNC_START_MASK              0x03FF0000UL
#define VDEC_0_Trick_mode_VSYNC_START_ALIGN             0
#define VDEC_0_Trick_mode_VSYNC_START_BITS              10
#define VDEC_0_Trick_mode_VSYNC_START_SHIFT             16

/* FIELD1_OFFSET [15:14] - unsigned */
#define VDEC_0_Trick_mode_FIELD1_OFFSET_MASK            0x0000C000UL
#define VDEC_0_Trick_mode_FIELD1_OFFSET_ALIGN           0
#define VDEC_0_Trick_mode_FIELD1_OFFSET_BITS            2
#define VDEC_0_Trick_mode_FIELD1_OFFSET_SHIFT           14

/* TOTAL_ACTIVE_LINES [13:05] - unsigned */
#define VDEC_0_Trick_mode_TOTAL_ACTIVE_LINES_MASK       0x00003FE0UL
#define VDEC_0_Trick_mode_TOTAL_ACTIVE_LINES_ALIGN      0
#define VDEC_0_Trick_mode_TOTAL_ACTIVE_LINES_BITS       9
#define VDEC_0_Trick_mode_TOTAL_ACTIVE_LINES_SHIFT      5

/* MASK_SELECT [04:03] - unsigned */
#define VDEC_0_Trick_mode_MASK_SELECT_MASK              0x00000018UL
#define VDEC_0_Trick_mode_MASK_SELECT_ALIGN             0
#define VDEC_0_Trick_mode_MASK_SELECT_BITS              2
#define VDEC_0_Trick_mode_MASK_SELECT_SHIFT             3

/* VBLANK_INVERT [02:02] - boolean */
#define VDEC_0_Trick_mode_VBLANK_INVERT_MASK            0x00000004UL
#define VDEC_0_Trick_mode_VBLANK_INVERT_ALIGN           0
#define VDEC_0_Trick_mode_VBLANK_INVERT_BITS            1
#define VDEC_0_Trick_mode_VBLANK_INVERT_SHIFT           2
#define VDEC_0_Trick_mode_VBLANK_INVERT_NORMAL          0
#define VDEC_0_Trick_mode_VBLANK_INVERT_INVERT          1

/* VSYNC_INVERT [01:01] - boolean */
#define VDEC_0_Trick_mode_VSYNC_INVERT_MASK             0x00000002UL
#define VDEC_0_Trick_mode_VSYNC_INVERT_ALIGN            0
#define VDEC_0_Trick_mode_VSYNC_INVERT_BITS             1
#define VDEC_0_Trick_mode_VSYNC_INVERT_SHIFT            1
#define VDEC_0_Trick_mode_VSYNC_INVERT_NORMAL           0
#define VDEC_0_Trick_mode_VSYNC_INVERT_INVERT           1

/* CONTROL [00:00] - boolean */
#define VDEC_0_Trick_mode_CONTROL_MASK                  0x00000001UL
#define VDEC_0_Trick_mode_CONTROL_ALIGN                 0
#define VDEC_0_Trick_mode_CONTROL_BITS                  1
#define VDEC_0_Trick_mode_CONTROL_SHIFT                 0
#define VDEC_0_Trick_mode_CONTROL_MANUAL                0
#define VDEC_0_Trick_mode_CONTROL_AUTO                  1

/**********************************************************************
 * VDEC_Vertical_Tilt
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDEC_0_Vertical_Tilt_reserved0_MASK             0xFC000000UL
#define VDEC_0_Vertical_Tilt_reserved0_ALIGN            0
#define VDEC_0_Vertical_Tilt_reserved0_BITS             6
#define VDEC_0_Vertical_Tilt_reserved0_SHIFT            26

/* INTERRUPT_FIELD1 [25:16] - unsigned */
#define VDEC_0_Vertical_Tilt_INTERRUPT_FIELD1_MASK      0x03FF0000UL
#define VDEC_0_Vertical_Tilt_INTERRUPT_FIELD1_ALIGN     0
#define VDEC_0_Vertical_Tilt_INTERRUPT_FIELD1_BITS      10
#define VDEC_0_Vertical_Tilt_INTERRUPT_FIELD1_SHIFT     16

/* INTERRUPT_FIELD0 [15:06] - unsigned */
#define VDEC_0_Vertical_Tilt_INTERRUPT_FIELD0_MASK      0x0000FFC0UL
#define VDEC_0_Vertical_Tilt_INTERRUPT_FIELD0_ALIGN     0
#define VDEC_0_Vertical_Tilt_INTERRUPT_FIELD0_BITS      10
#define VDEC_0_Vertical_Tilt_INTERRUPT_FIELD0_SHIFT     6

/* TRICKMODE_HYSTERESIS [05:02] - unsigned */
#define VDEC_0_Vertical_Tilt_TRICKMODE_HYSTERESIS_MASK  0x0000003CUL
#define VDEC_0_Vertical_Tilt_TRICKMODE_HYSTERESIS_ALIGN 0
#define VDEC_0_Vertical_Tilt_TRICKMODE_HYSTERESIS_BITS  4
#define VDEC_0_Vertical_Tilt_TRICKMODE_HYSTERESIS_SHIFT 2

/* HALF_SYNC_AMPLITUDE [01:01] - unsigned */
#define VDEC_0_Vertical_Tilt_HALF_SYNC_AMPLITUDE_MASK   0x00000002UL
#define VDEC_0_Vertical_Tilt_HALF_SYNC_AMPLITUDE_ALIGN  0
#define VDEC_0_Vertical_Tilt_HALF_SYNC_AMPLITUDE_BITS   1
#define VDEC_0_Vertical_Tilt_HALF_SYNC_AMPLITUDE_SHIFT  1
#define VDEC_0_Vertical_Tilt_HALF_SYNC_AMPLITUDE_CURRENT 0
#define VDEC_0_Vertical_Tilt_HALF_SYNC_AMPLITUDE_FILTERED 1

/* COARSE_SYNC [00:00] - boolean */
#define VDEC_0_Vertical_Tilt_COARSE_SYNC_MASK           0x00000001UL
#define VDEC_0_Vertical_Tilt_COARSE_SYNC_ALIGN          0
#define VDEC_0_Vertical_Tilt_COARSE_SYNC_BITS           1
#define VDEC_0_Vertical_Tilt_COARSE_SYNC_SHIFT          0
#define VDEC_0_Vertical_Tilt_COARSE_SYNC_NORMAL         0
#define VDEC_0_Vertical_Tilt_COARSE_SYNC_AVG64          1

/**********************************************************************
 * VDEC_Misc
 **********************************************************************/
/* reserved0 [31:28] -  */
#define VDEC_0_Misc_reserved0_MASK                      0xF0000000UL
#define VDEC_0_Misc_reserved0_ALIGN                     0
#define VDEC_0_Misc_reserved0_BITS                      4
#define VDEC_0_Misc_reserved0_SHIFT                     28

/* PREDICT_LINE_LENGTH [27:27] - unsigned */
#define VDEC_0_Misc_PREDICT_LINE_LENGTH_MASK            0x08000000UL
#define VDEC_0_Misc_PREDICT_LINE_LENGTH_ALIGN           0
#define VDEC_0_Misc_PREDICT_LINE_LENGTH_BITS            1
#define VDEC_0_Misc_PREDICT_LINE_LENGTH_SHIFT           27
#define VDEC_0_Misc_PREDICT_LINE_LENGTH_MEASURED        0
#define VDEC_0_Misc_PREDICT_LINE_LENGTH_STANDARD        1

/* HSYNC_PREDICT_OFFSET [26:20] - unsigned */
#define VDEC_0_Misc_HSYNC_PREDICT_OFFSET_MASK           0x07F00000UL
#define VDEC_0_Misc_HSYNC_PREDICT_OFFSET_ALIGN          0
#define VDEC_0_Misc_HSYNC_PREDICT_OFFSET_BITS           7
#define VDEC_0_Misc_HSYNC_PREDICT_OFFSET_SHIFT          20

/* MINIMUM_VIDEO [19:18] - unsigned */
#define VDEC_0_Misc_MINIMUM_VIDEO_MASK                  0x000C0000UL
#define VDEC_0_Misc_MINIMUM_VIDEO_ALIGN                 0
#define VDEC_0_Misc_MINIMUM_VIDEO_BITS                  2
#define VDEC_0_Misc_MINIMUM_VIDEO_SHIFT                 18

/* SLICE_SELECTION [17:16] - unsigned */
#define VDEC_0_Misc_SLICE_SELECTION_MASK                0x00030000UL
#define VDEC_0_Misc_SLICE_SELECTION_ALIGN               0
#define VDEC_0_Misc_SLICE_SELECTION_BITS                2
#define VDEC_0_Misc_SLICE_SELECTION_SHIFT               16

/* SLICE_LEVEL [15:06] - unsigned */
#define VDEC_0_Misc_SLICE_LEVEL_MASK                    0x0000FFC0UL
#define VDEC_0_Misc_SLICE_LEVEL_ALIGN                   0
#define VDEC_0_Misc_SLICE_LEVEL_BITS                    10
#define VDEC_0_Misc_SLICE_LEVEL_SHIFT                   6

/* BLACK_LEVEL [05:00] - unsigned */
#define VDEC_0_Misc_BLACK_LEVEL_MASK                    0x0000003FUL
#define VDEC_0_Misc_BLACK_LEVEL_ALIGN                   0
#define VDEC_0_Misc_BLACK_LEVEL_BITS                    6
#define VDEC_0_Misc_BLACK_LEVEL_SHIFT                   0

/**********************************************************************
 * VDEC_General_Control
 **********************************************************************/
/* reserved0 [31:02] -  */
#define VDEC_0_General_Control_reserved0_MASK           0xFFFFFFFCUL
#define VDEC_0_General_Control_reserved0_ALIGN          0
#define VDEC_0_General_Control_reserved0_BITS           30
#define VDEC_0_General_Control_reserved0_SHIFT          2

/* FIELD1_INTERRUPT_ENABLE [01:01] - boolean */
#define VDEC_0_General_Control_FIELD1_INTERRUPT_ENABLE_MASK 0x00000002UL
#define VDEC_0_General_Control_FIELD1_INTERRUPT_ENABLE_ALIGN 0
#define VDEC_0_General_Control_FIELD1_INTERRUPT_ENABLE_BITS 1
#define VDEC_0_General_Control_FIELD1_INTERRUPT_ENABLE_SHIFT 1
#define VDEC_0_General_Control_FIELD1_INTERRUPT_ENABLE_INACTIVE 0
#define VDEC_0_General_Control_FIELD1_INTERRUPT_ENABLE_ACTIVE 1

/* FIELD0_INTERRUPT_ENABLE [00:00] - boolean */
#define VDEC_0_General_Control_FIELD0_INTERRUPT_ENABLE_MASK 0x00000001UL
#define VDEC_0_General_Control_FIELD0_INTERRUPT_ENABLE_ALIGN 0
#define VDEC_0_General_Control_FIELD0_INTERRUPT_ENABLE_BITS 1
#define VDEC_0_General_Control_FIELD0_INTERRUPT_ENABLE_SHIFT 0
#define VDEC_0_General_Control_FIELD0_INTERRUPT_ENABLE_INACTIVE 0
#define VDEC_0_General_Control_FIELD0_INTERRUPT_ENABLE_ACTIVE 1

/**********************************************************************
 * VDEC_General_Status
 **********************************************************************/
/* reserved0 [31:02] -  */
#define VDEC_0_General_Status_reserved0_MASK            0xFFFFFFFCUL
#define VDEC_0_General_Status_reserved0_ALIGN           0
#define VDEC_0_General_Status_reserved0_BITS            30
#define VDEC_0_General_Status_reserved0_SHIFT           2

/* FIELD1_INTERRUPT_STATUS [01:01] - boolean */
#define VDEC_0_General_Status_FIELD1_INTERRUPT_STATUS_MASK 0x00000002UL
#define VDEC_0_General_Status_FIELD1_INTERRUPT_STATUS_ALIGN 0
#define VDEC_0_General_Status_FIELD1_INTERRUPT_STATUS_BITS 1
#define VDEC_0_General_Status_FIELD1_INTERRUPT_STATUS_SHIFT 1
#define VDEC_0_General_Status_FIELD1_INTERRUPT_STATUS_UNSET 0
#define VDEC_0_General_Status_FIELD1_INTERRUPT_STATUS_SET 1

/* FIELD0_INTERRUPT_STATUS [00:00] - boolean */
#define VDEC_0_General_Status_FIELD0_INTERRUPT_STATUS_MASK 0x00000001UL
#define VDEC_0_General_Status_FIELD0_INTERRUPT_STATUS_ALIGN 0
#define VDEC_0_General_Status_FIELD0_INTERRUPT_STATUS_BITS 1
#define VDEC_0_General_Status_FIELD0_INTERRUPT_STATUS_SHIFT 0
#define VDEC_0_General_Status_FIELD0_INTERRUPT_STATUS_UNSET 0
#define VDEC_0_General_Status_FIELD0_INTERRUPT_STATUS_SET 1

/**********************************************************************
 * VDEC_Anci656_Port_A
 **********************************************************************/
/* reserved0 [31:27] -  */
#define VDEC_0_Anci656_Port_A_reserved0_MASK            0xF8000000UL
#define VDEC_0_Anci656_Port_A_reserved0_ALIGN           0
#define VDEC_0_Anci656_Port_A_reserved0_BITS            5
#define VDEC_0_Anci656_Port_A_reserved0_SHIFT           27

/* SDID [26:19] - unsigned */
#define VDEC_0_Anci656_Port_A_SDID_MASK                 0x07F80000UL
#define VDEC_0_Anci656_Port_A_SDID_ALIGN                0
#define VDEC_0_Anci656_Port_A_SDID_BITS                 8
#define VDEC_0_Anci656_Port_A_SDID_SHIFT                19

/* DWORD_COUNT [18:11] - unsigned */
#define VDEC_0_Anci656_Port_A_DWORD_COUNT_MASK          0x0007F800UL
#define VDEC_0_Anci656_Port_A_DWORD_COUNT_ALIGN         0
#define VDEC_0_Anci656_Port_A_DWORD_COUNT_BITS          8
#define VDEC_0_Anci656_Port_A_DWORD_COUNT_SHIFT         11

/* TYPE [10:03] - unsigned */
#define VDEC_0_Anci656_Port_A_TYPE_MASK                 0x000007F8UL
#define VDEC_0_Anci656_Port_A_TYPE_ALIGN                0
#define VDEC_0_Anci656_Port_A_TYPE_BITS                 8
#define VDEC_0_Anci656_Port_A_TYPE_SHIFT                3
#define VDEC_0_Anci656_Port_A_TYPE_EURO_CLOSED_CAPTION  1
#define VDEC_0_Anci656_Port_A_TYPE_WIDE_SCREEN          3
#define VDEC_0_Anci656_Port_A_TYPE_US_TELETEXT          4
#define VDEC_0_Anci656_Port_A_TYPE_US_CLOSED_CAPTION    5
#define VDEC_0_Anci656_Port_A_TYPE_TELETEXT             8
#define VDEC_0_Anci656_Port_A_TYPE_US_NABTS             12

/* FORMAT [02:01] - unsigned */
#define VDEC_0_Anci656_Port_A_FORMAT_MASK               0x00000006UL
#define VDEC_0_Anci656_Port_A_FORMAT_ALIGN              0
#define VDEC_0_Anci656_Port_A_FORMAT_BITS               2
#define VDEC_0_Anci656_Port_A_FORMAT_SHIFT              1
#define VDEC_0_Anci656_Port_A_FORMAT_SAA7113H           0
#define VDEC_0_Anci656_Port_A_FORMAT_MODIFIED_SAA7113H  1
#define VDEC_0_Anci656_Port_A_FORMAT_SMPTE291M          2

/* ENABLE [00:00] - boolean */
#define VDEC_0_Anci656_Port_A_ENABLE_MASK               0x00000001UL
#define VDEC_0_Anci656_Port_A_ENABLE_ALIGN              0
#define VDEC_0_Anci656_Port_A_ENABLE_BITS               1
#define VDEC_0_Anci656_Port_A_ENABLE_SHIFT              0
#define VDEC_0_Anci656_Port_A_ENABLE_OFF                0
#define VDEC_0_Anci656_Port_A_ENABLE_ON                 1

/* reserved1 [31:27] -  */
#define VDEC_0_Anci656_Port_B_reserved1_MASK            0xF8000000UL
#define VDEC_0_Anci656_Port_B_reserved1_ALIGN           0
#define VDEC_0_Anci656_Port_B_reserved1_BITS            5
#define VDEC_0_Anci656_Port_B_reserved1_SHIFT           27

/* SDID [26:19] - unsigned */
#define VDEC_0_Anci656_Port_B_SDID_MASK                 0x07F80000UL
#define VDEC_0_Anci656_Port_B_SDID_ALIGN                0
#define VDEC_0_Anci656_Port_B_SDID_BITS                 8
#define VDEC_0_Anci656_Port_B_SDID_SHIFT                19

/* DWORD_COUNT [18:11] - unsigned */
#define VDEC_0_Anci656_Port_B_DWORD_COUNT_MASK          0x0007F800UL
#define VDEC_0_Anci656_Port_B_DWORD_COUNT_ALIGN         0
#define VDEC_0_Anci656_Port_B_DWORD_COUNT_BITS          8
#define VDEC_0_Anci656_Port_B_DWORD_COUNT_SHIFT         11

/* TYPE [10:03] - unsigned */
#define VDEC_0_Anci656_Port_B_TYPE_MASK                 0x000007F8UL
#define VDEC_0_Anci656_Port_B_TYPE_ALIGN                0
#define VDEC_0_Anci656_Port_B_TYPE_BITS                 8
#define VDEC_0_Anci656_Port_B_TYPE_SHIFT                3
#define VDEC_0_Anci656_Port_B_TYPE_EURO_CLOSED_CAPTION  1
#define VDEC_0_Anci656_Port_B_TYPE_WIDE_SCREEN          3
#define VDEC_0_Anci656_Port_B_TYPE_US_TELETEXT          4
#define VDEC_0_Anci656_Port_B_TYPE_US_CLOSED_CAPTION    5
#define VDEC_0_Anci656_Port_B_TYPE_TELETEXT             8
#define VDEC_0_Anci656_Port_B_TYPE_US_NABTS             12

/* FORMAT [02:01] - unsigned */
#define VDEC_0_Anci656_Port_B_FORMAT_MASK               0x00000006UL
#define VDEC_0_Anci656_Port_B_FORMAT_ALIGN              0
#define VDEC_0_Anci656_Port_B_FORMAT_BITS               2
#define VDEC_0_Anci656_Port_B_FORMAT_SHIFT              1
#define VDEC_0_Anci656_Port_B_FORMAT_SAA7113H           0
#define VDEC_0_Anci656_Port_B_FORMAT_MODIFIED_SAA7113H  1
#define VDEC_0_Anci656_Port_B_FORMAT_SMPTE291M          2

/* ENABLE [00:00] - boolean */
#define VDEC_0_Anci656_Port_B_ENABLE_MASK               0x00000001UL
#define VDEC_0_Anci656_Port_B_ENABLE_ALIGN              0
#define VDEC_0_Anci656_Port_B_ENABLE_BITS               1
#define VDEC_0_Anci656_Port_B_ENABLE_SHIFT              0
#define VDEC_0_Anci656_Port_B_ENABLE_OFF                0
#define VDEC_0_Anci656_Port_B_ENABLE_ON                 1

/* reserved2 [31:27] -  */
#define VDEC_0_Anci656_Port_C_reserved2_MASK            0xF8000000UL
#define VDEC_0_Anci656_Port_C_reserved2_ALIGN           0
#define VDEC_0_Anci656_Port_C_reserved2_BITS            5
#define VDEC_0_Anci656_Port_C_reserved2_SHIFT           27

/* SDID [26:19] - unsigned */
#define VDEC_0_Anci656_Port_C_SDID_MASK                 0x07F80000UL
#define VDEC_0_Anci656_Port_C_SDID_ALIGN                0
#define VDEC_0_Anci656_Port_C_SDID_BITS                 8
#define VDEC_0_Anci656_Port_C_SDID_SHIFT                19

/* DWORD_COUNT [18:11] - unsigned */
#define VDEC_0_Anci656_Port_C_DWORD_COUNT_MASK          0x0007F800UL
#define VDEC_0_Anci656_Port_C_DWORD_COUNT_ALIGN         0
#define VDEC_0_Anci656_Port_C_DWORD_COUNT_BITS          8
#define VDEC_0_Anci656_Port_C_DWORD_COUNT_SHIFT         11

/* TYPE [10:03] - unsigned */
#define VDEC_0_Anci656_Port_C_TYPE_MASK                 0x000007F8UL
#define VDEC_0_Anci656_Port_C_TYPE_ALIGN                0
#define VDEC_0_Anci656_Port_C_TYPE_BITS                 8
#define VDEC_0_Anci656_Port_C_TYPE_SHIFT                3
#define VDEC_0_Anci656_Port_C_TYPE_EURO_CLOSED_CAPTION  1
#define VDEC_0_Anci656_Port_C_TYPE_WIDE_SCREEN          3
#define VDEC_0_Anci656_Port_C_TYPE_US_TELETEXT          4
#define VDEC_0_Anci656_Port_C_TYPE_US_CLOSED_CAPTION    5
#define VDEC_0_Anci656_Port_C_TYPE_TELETEXT             8
#define VDEC_0_Anci656_Port_C_TYPE_US_NABTS             12

/* FORMAT [02:01] - unsigned */
#define VDEC_0_Anci656_Port_C_FORMAT_MASK               0x00000006UL
#define VDEC_0_Anci656_Port_C_FORMAT_ALIGN              0
#define VDEC_0_Anci656_Port_C_FORMAT_BITS               2
#define VDEC_0_Anci656_Port_C_FORMAT_SHIFT              1
#define VDEC_0_Anci656_Port_C_FORMAT_SAA7113H           0
#define VDEC_0_Anci656_Port_C_FORMAT_MODIFIED_SAA7113H  1
#define VDEC_0_Anci656_Port_C_FORMAT_SMPTE291M          2

/* ENABLE [00:00] - boolean */
#define VDEC_0_Anci656_Port_C_ENABLE_MASK               0x00000001UL
#define VDEC_0_Anci656_Port_C_ENABLE_ALIGN              0
#define VDEC_0_Anci656_Port_C_ENABLE_BITS               1
#define VDEC_0_Anci656_Port_C_ENABLE_SHIFT              0
#define VDEC_0_Anci656_Port_C_ENABLE_OFF                0
#define VDEC_0_Anci656_Port_C_ENABLE_ON                 1

/**********************************************************************
 * VDEC_Dither656
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDEC_0_Dither656_reserved0_MASK                 0xFC000000UL
#define VDEC_0_Dither656_reserved0_ALIGN                0
#define VDEC_0_Dither656_reserved0_BITS                 6
#define VDEC_0_Dither656_reserved0_SHIFT                26

/* MODE [25:25] - boolean */
#define VDEC_0_Dither656_MODE_MASK                      0x02000000UL
#define VDEC_0_Dither656_MODE_ALIGN                     0
#define VDEC_0_Dither656_MODE_BITS                      1
#define VDEC_0_Dither656_MODE_SHIFT                     25
#define VDEC_0_Dither656_MODE_COUNTER                   0
#define VDEC_0_Dither656_MODE_LFSR                      1

/* VBLANK_DELAY [24:21] - unsigned */
#define VDEC_0_Dither656_VBLANK_DELAY_MASK              0x01E00000UL
#define VDEC_0_Dither656_VBLANK_DELAY_ALIGN             0
#define VDEC_0_Dither656_VBLANK_DELAY_BITS              4
#define VDEC_0_Dither656_VBLANK_DELAY_SHIFT             21

/* FV_LATCH_REGION [20:20] - boolean */
#define VDEC_0_Dither656_FV_LATCH_REGION_MASK           0x00100000UL
#define VDEC_0_Dither656_FV_LATCH_REGION_ALIGN          0
#define VDEC_0_Dither656_FV_LATCH_REGION_BITS           1
#define VDEC_0_Dither656_FV_LATCH_REGION_SHIFT          20
#define VDEC_0_Dither656_FV_LATCH_REGION_ACTIVE         0
#define VDEC_0_Dither656_FV_LATCH_REGION_BLANKING       1

/* USE_LATCHED_FV [19:19] - boolean */
#define VDEC_0_Dither656_USE_LATCHED_FV_MASK            0x00080000UL
#define VDEC_0_Dither656_USE_LATCHED_FV_ALIGN           0
#define VDEC_0_Dither656_USE_LATCHED_FV_BITS            1
#define VDEC_0_Dither656_USE_LATCHED_FV_SHIFT           19
#define VDEC_0_Dither656_USE_LATCHED_FV_OFF             0
#define VDEC_0_Dither656_USE_LATCHED_FV_ON              1

/* CHOOSE_FULL_RANGE [18:18] - boolean */
#define VDEC_0_Dither656_CHOOSE_FULL_RANGE_MASK         0x00040000UL
#define VDEC_0_Dither656_CHOOSE_FULL_RANGE_ALIGN        0
#define VDEC_0_Dither656_CHOOSE_FULL_RANGE_BITS         1
#define VDEC_0_Dither656_CHOOSE_FULL_RANGE_SHIFT        18
#define VDEC_0_Dither656_CHOOSE_FULL_RANGE_OFF          0
#define VDEC_0_Dither656_CHOOSE_FULL_RANGE_ON           1

/* CHROMA_ENABLE [17:17] - boolean */
#define VDEC_0_Dither656_CHROMA_ENABLE_MASK             0x00020000UL
#define VDEC_0_Dither656_CHROMA_ENABLE_ALIGN            0
#define VDEC_0_Dither656_CHROMA_ENABLE_BITS             1
#define VDEC_0_Dither656_CHROMA_ENABLE_SHIFT            17
#define VDEC_0_Dither656_CHROMA_ENABLE_OFF              0
#define VDEC_0_Dither656_CHROMA_ENABLE_ON               1

/* LUMA_ENABLE [16:16] - boolean */
#define VDEC_0_Dither656_LUMA_ENABLE_MASK               0x00010000UL
#define VDEC_0_Dither656_LUMA_ENABLE_ALIGN              0
#define VDEC_0_Dither656_LUMA_ENABLE_BITS               1
#define VDEC_0_Dither656_LUMA_ENABLE_SHIFT              16
#define VDEC_0_Dither656_LUMA_ENABLE_OFF                0
#define VDEC_0_Dither656_LUMA_ENABLE_ON                 1

/* ROW3 [15:12] - unsigned */
#define VDEC_0_Dither656_ROW3_MASK                      0x0000F000UL
#define VDEC_0_Dither656_ROW3_ALIGN                     0
#define VDEC_0_Dither656_ROW3_BITS                      4
#define VDEC_0_Dither656_ROW3_SHIFT                     12

/* ROW2 [11:08] - unsigned */
#define VDEC_0_Dither656_ROW2_MASK                      0x00000F00UL
#define VDEC_0_Dither656_ROW2_ALIGN                     0
#define VDEC_0_Dither656_ROW2_BITS                      4
#define VDEC_0_Dither656_ROW2_SHIFT                     8

/* ROW1 [07:04] - unsigned */
#define VDEC_0_Dither656_ROW1_MASK                      0x000000F0UL
#define VDEC_0_Dither656_ROW1_ALIGN                     0
#define VDEC_0_Dither656_ROW1_BITS                      4
#define VDEC_0_Dither656_ROW1_SHIFT                     4

/* ROW0 [03:00] - unsigned */
#define VDEC_0_Dither656_ROW0_MASK                      0x0000000FUL
#define VDEC_0_Dither656_ROW0_ALIGN                     0
#define VDEC_0_Dither656_ROW0_BITS                      4
#define VDEC_0_Dither656_ROW0_SHIFT                     0

/**********************************************************************
 * VDEC_Test
 **********************************************************************/
/* reserved0 [31:03] -  */
#define VDEC_0_Test_reserved0_MASK                      0xFFFFFFF8UL
#define VDEC_0_Test_reserved0_ALIGN                     0
#define VDEC_0_Test_reserved0_BITS                      29
#define VDEC_0_Test_reserved0_SHIFT                     3

/* TEST_SELECT [02:01] - unsigned */
#define VDEC_0_Test_TEST_SELECT_MASK                    0x00000006UL
#define VDEC_0_Test_TEST_SELECT_ALIGN                   0
#define VDEC_0_Test_TEST_SELECT_BITS                    2
#define VDEC_0_Test_TEST_SELECT_SHIFT                   1

/* TEST_MODE [00:00] - boolean */
#define VDEC_0_Test_TEST_MODE_MASK                      0x00000001UL
#define VDEC_0_Test_TEST_MODE_ALIGN                     0
#define VDEC_0_Test_TEST_MODE_BITS                      1
#define VDEC_0_Test_TEST_MODE_SHIFT                     0
#define VDEC_0_Test_TEST_MODE_OFF                       0
#define VDEC_0_Test_TEST_MODE_ON                        1

/**********************************************************************
 * VDEC_Revision_ID
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VDEC_0_Revision_ID_reserved0_MASK               0xFFFF0000UL
#define VDEC_0_Revision_ID_reserved0_ALIGN              0
#define VDEC_0_Revision_ID_reserved0_BITS               16
#define VDEC_0_Revision_ID_reserved0_SHIFT              16

/* REVISION_ID [15:00] - unsigned */
#define VDEC_0_Revision_ID_REVISION_ID_MASK             0x0000FFFFUL
#define VDEC_0_Revision_ID_REVISION_ID_ALIGN            0
#define VDEC_0_Revision_ID_REVISION_ID_BITS             16
#define VDEC_0_Revision_ID_REVISION_ID_SHIFT            0

/**********************************************************************
 * VDEC_VBI
 **********************************************************************/
/* FIELD1_END [31:23] - unsigned */
#define VDEC_0_VBI_FIELD1_END_MASK                      0xFF800000UL
#define VDEC_0_VBI_FIELD1_END_ALIGN                     0
#define VDEC_0_VBI_FIELD1_END_BITS                      9
#define VDEC_0_VBI_FIELD1_END_SHIFT                     23

/* FIELD1_START [22:14] - unsigned */
#define VDEC_0_VBI_FIELD1_START_MASK                    0x007FC000UL
#define VDEC_0_VBI_FIELD1_START_ALIGN                   0
#define VDEC_0_VBI_FIELD1_START_BITS                    9
#define VDEC_0_VBI_FIELD1_START_SHIFT                   14

/* FIELD0_END [13:07] - unsigned */
#define VDEC_0_VBI_FIELD0_END_MASK                      0x00003F80UL
#define VDEC_0_VBI_FIELD0_END_ALIGN                     0
#define VDEC_0_VBI_FIELD0_END_BITS                      7
#define VDEC_0_VBI_FIELD0_END_SHIFT                     7

/* FIELD0_START [06:00] - unsigned */
#define VDEC_0_VBI_FIELD0_START_MASK                    0x0000007FUL
#define VDEC_0_VBI_FIELD0_START_ALIGN                   0
#define VDEC_0_VBI_FIELD0_START_BITS                    7
#define VDEC_0_VBI_FIELD0_START_SHIFT                   0

/**********************************************************************
 * VDE_H_LENGTH
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_H_LENGTH_reserved0_MASK                     0xFFFFFC00UL
#define VDE_H_LENGTH_reserved0_ALIGN                    0
#define VDE_H_LENGTH_reserved0_BITS                     22
#define VDE_H_LENGTH_reserved0_SHIFT                    10

/* H_LENGTH [09:00] - unsigned */
#define VDE_H_LENGTH_H_LENGTH_MASK                      0x000003FFUL
#define VDE_H_LENGTH_H_LENGTH_ALIGN                     0
#define VDE_H_LENGTH_H_LENGTH_BITS                      10
#define VDE_H_LENGTH_H_LENGTH_SHIFT                     0

/**********************************************************************
 * VDE_V_HEIGHT
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_V_HEIGHT_reserved0_MASK                     0xFFFFFC00UL
#define VDE_V_HEIGHT_reserved0_ALIGN                    0
#define VDE_V_HEIGHT_reserved0_BITS                     22
#define VDE_V_HEIGHT_reserved0_SHIFT                    10

/* V_HEIGHT [09:00] - unsigned */
#define VDE_V_HEIGHT_V_HEIGHT_MASK                      0x000003FFUL
#define VDE_V_HEIGHT_V_HEIGHT_ALIGN                     0
#define VDE_V_HEIGHT_V_HEIGHT_BITS                      10
#define VDE_V_HEIGHT_V_HEIGHT_SHIFT                     0

/**********************************************************************
 * VDE_HS_WIDTH
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_HS_WIDTH_reserved0_MASK                     0xFFFFFC00UL
#define VDE_HS_WIDTH_reserved0_ALIGN                    0
#define VDE_HS_WIDTH_reserved0_BITS                     22
#define VDE_HS_WIDTH_reserved0_SHIFT                    10

/* HS_WIDTH [09:00] - unsigned */
#define VDE_HS_WIDTH_HS_WIDTH_MASK                      0x000003FFUL
#define VDE_HS_WIDTH_HS_WIDTH_ALIGN                     0
#define VDE_HS_WIDTH_HS_WIDTH_BITS                      10
#define VDE_HS_WIDTH_HS_WIDTH_SHIFT                     0

/**********************************************************************
 * VDE_VS_WIDTH
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_VS_WIDTH_reserved0_MASK                     0xFFFFFC00UL
#define VDE_VS_WIDTH_reserved0_ALIGN                    0
#define VDE_VS_WIDTH_reserved0_BITS                     22
#define VDE_VS_WIDTH_reserved0_SHIFT                    10

/* VS_WIDTH [09:00] - unsigned */
#define VDE_VS_WIDTH_VS_WIDTH_MASK                      0x000003FFUL
#define VDE_VS_WIDTH_VS_WIDTH_ALIGN                     0
#define VDE_VS_WIDTH_VS_WIDTH_BITS                      10
#define VDE_VS_WIDTH_VS_WIDTH_SHIFT                     0

/**********************************************************************
 * VDE_AV_X_START
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_AV_X_START_reserved0_MASK                   0xFFFFFC00UL
#define VDE_AV_X_START_reserved0_ALIGN                  0
#define VDE_AV_X_START_reserved0_BITS                   22
#define VDE_AV_X_START_reserved0_SHIFT                  10

/* X_START [09:00] - unsigned */
#define VDE_AV_X_START_X_START_MASK                     0x000003FFUL
#define VDE_AV_X_START_X_START_ALIGN                    0
#define VDE_AV_X_START_X_START_BITS                     10
#define VDE_AV_X_START_X_START_SHIFT                    0

/**********************************************************************
 * VDE_AV_X_END
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_AV_X_END_reserved0_MASK                     0xFFFFFC00UL
#define VDE_AV_X_END_reserved0_ALIGN                    0
#define VDE_AV_X_END_reserved0_BITS                     22
#define VDE_AV_X_END_reserved0_SHIFT                    10

/* X_END [09:00] - unsigned */
#define VDE_AV_X_END_X_END_MASK                         0x000003FFUL
#define VDE_AV_X_END_X_END_ALIGN                        0
#define VDE_AV_X_END_X_END_BITS                         10
#define VDE_AV_X_END_X_END_SHIFT                        0

/**********************************************************************
 * VDE_AV_Y_START
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_AV_Y_START_reserved0_MASK                   0xFFFFFC00UL
#define VDE_AV_Y_START_reserved0_ALIGN                  0
#define VDE_AV_Y_START_reserved0_BITS                   22
#define VDE_AV_Y_START_reserved0_SHIFT                  10

/* Y_START [09:00] - unsigned */
#define VDE_AV_Y_START_Y_START_MASK                     0x000003FFUL
#define VDE_AV_Y_START_Y_START_ALIGN                    0
#define VDE_AV_Y_START_Y_START_BITS                     10
#define VDE_AV_Y_START_Y_START_SHIFT                    0

/**********************************************************************
 * VDE_AV_Y_END
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_AV_Y_END_reserved0_MASK                     0xFFFFFC00UL
#define VDE_AV_Y_END_reserved0_ALIGN                    0
#define VDE_AV_Y_END_reserved0_BITS                     22
#define VDE_AV_Y_END_reserved0_SHIFT                    10

/* Y_END [09:00] - unsigned */
#define VDE_AV_Y_END_Y_END_MASK                         0x000003FFUL
#define VDE_AV_Y_END_Y_END_ALIGN                        0
#define VDE_AV_Y_END_Y_END_BITS                         10
#define VDE_AV_Y_END_Y_END_SHIFT                        0

/**********************************************************************
 * VDE_VWIN1_X_START
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_VWIN1_X_START_reserved0_MASK                0xFFFFFC00UL
#define VDE_VWIN1_X_START_reserved0_ALIGN               0
#define VDE_VWIN1_X_START_reserved0_BITS                22
#define VDE_VWIN1_X_START_reserved0_SHIFT               10

/* X_START [09:00] - unsigned */
#define VDE_VWIN1_X_START_X_START_MASK                  0x000003FFUL
#define VDE_VWIN1_X_START_X_START_ALIGN                 0
#define VDE_VWIN1_X_START_X_START_BITS                  10
#define VDE_VWIN1_X_START_X_START_SHIFT                 0

/**********************************************************************
 * VDE_VWIN1_X_END
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_VWIN1_X_END_reserved0_MASK                  0xFFFFFC00UL
#define VDE_VWIN1_X_END_reserved0_ALIGN                 0
#define VDE_VWIN1_X_END_reserved0_BITS                  22
#define VDE_VWIN1_X_END_reserved0_SHIFT                 10

/* X_END [09:00] - unsigned */
#define VDE_VWIN1_X_END_X_END_MASK                      0x000003FFUL
#define VDE_VWIN1_X_END_X_END_ALIGN                     0
#define VDE_VWIN1_X_END_X_END_BITS                      10
#define VDE_VWIN1_X_END_X_END_SHIFT                     0

/**********************************************************************
 * VDE_VWIN1_Y_START
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_VWIN1_Y_START_reserved0_MASK                0xFFFFFC00UL
#define VDE_VWIN1_Y_START_reserved0_ALIGN               0
#define VDE_VWIN1_Y_START_reserved0_BITS                22
#define VDE_VWIN1_Y_START_reserved0_SHIFT               10

/* Y_START [09:00] - unsigned */
#define VDE_VWIN1_Y_START_Y_START_MASK                  0x000003FFUL
#define VDE_VWIN1_Y_START_Y_START_ALIGN                 0
#define VDE_VWIN1_Y_START_Y_START_BITS                  10
#define VDE_VWIN1_Y_START_Y_START_SHIFT                 0

/**********************************************************************
 * VDE_VWIN1_Y_END
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_VWIN1_Y_END_reserved0_MASK                  0xFFFFFC00UL
#define VDE_VWIN1_Y_END_reserved0_ALIGN                 0
#define VDE_VWIN1_Y_END_reserved0_BITS                  22
#define VDE_VWIN1_Y_END_reserved0_SHIFT                 10

/* Y_END [09:00] - unsigned */
#define VDE_VWIN1_Y_END_Y_END_MASK                      0x000003FFUL
#define VDE_VWIN1_Y_END_Y_END_ALIGN                     0
#define VDE_VWIN1_Y_END_Y_END_BITS                      10
#define VDE_VWIN1_Y_END_Y_END_SHIFT                     0

/**********************************************************************
 * VDE_VWIN2_X_START
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_VWIN2_X_START_reserved0_MASK                0xFFFFFC00UL
#define VDE_VWIN2_X_START_reserved0_ALIGN               0
#define VDE_VWIN2_X_START_reserved0_BITS                22
#define VDE_VWIN2_X_START_reserved0_SHIFT               10

/* X_START [09:00] - unsigned */
#define VDE_VWIN2_X_START_X_START_MASK                  0x000003FFUL
#define VDE_VWIN2_X_START_X_START_ALIGN                 0
#define VDE_VWIN2_X_START_X_START_BITS                  10
#define VDE_VWIN2_X_START_X_START_SHIFT                 0

/**********************************************************************
 * VDE_VWIN2_X_END
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_VWIN2_X_END_reserved0_MASK                  0xFFFFFC00UL
#define VDE_VWIN2_X_END_reserved0_ALIGN                 0
#define VDE_VWIN2_X_END_reserved0_BITS                  22
#define VDE_VWIN2_X_END_reserved0_SHIFT                 10

/* X_END [09:00] - unsigned */
#define VDE_VWIN2_X_END_X_END_MASK                      0x000003FFUL
#define VDE_VWIN2_X_END_X_END_ALIGN                     0
#define VDE_VWIN2_X_END_X_END_BITS                      10
#define VDE_VWIN2_X_END_X_END_SHIFT                     0

/**********************************************************************
 * VDE_VWIN2_Y_START
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_VWIN2_Y_START_reserved0_MASK                0xFFFFFC00UL
#define VDE_VWIN2_Y_START_reserved0_ALIGN               0
#define VDE_VWIN2_Y_START_reserved0_BITS                22
#define VDE_VWIN2_Y_START_reserved0_SHIFT               10

/* Y_START [09:00] - unsigned */
#define VDE_VWIN2_Y_START_Y_START_MASK                  0x000003FFUL
#define VDE_VWIN2_Y_START_Y_START_ALIGN                 0
#define VDE_VWIN2_Y_START_Y_START_BITS                  10
#define VDE_VWIN2_Y_START_Y_START_SHIFT                 0

/**********************************************************************
 * VDE_VWIN2_Y_END
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_VWIN2_Y_END_reserved0_MASK                  0xFFFFFC00UL
#define VDE_VWIN2_Y_END_reserved0_ALIGN                 0
#define VDE_VWIN2_Y_END_reserved0_BITS                  22
#define VDE_VWIN2_Y_END_reserved0_SHIFT                 10

/* Y_END [09:00] - unsigned */
#define VDE_VWIN2_Y_END_Y_END_MASK                      0x000003FFUL
#define VDE_VWIN2_Y_END_Y_END_ALIGN                     0
#define VDE_VWIN2_Y_END_Y_END_BITS                      10
#define VDE_VWIN2_Y_END_Y_END_SHIFT                     0

/**********************************************************************
 * VDE_GFX_X_START
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_GFX_X_START_reserved0_MASK                  0xFFFFFC00UL
#define VDE_GFX_X_START_reserved0_ALIGN                 0
#define VDE_GFX_X_START_reserved0_BITS                  22
#define VDE_GFX_X_START_reserved0_SHIFT                 10

/* X_START [09:00] - unsigned */
#define VDE_GFX_X_START_X_START_MASK                    0x000003FFUL
#define VDE_GFX_X_START_X_START_ALIGN                   0
#define VDE_GFX_X_START_X_START_BITS                    10
#define VDE_GFX_X_START_X_START_SHIFT                   0

/**********************************************************************
 * VDE_GFX_X_END
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_GFX_X_END_reserved0_MASK                    0xFFFFFC00UL
#define VDE_GFX_X_END_reserved0_ALIGN                   0
#define VDE_GFX_X_END_reserved0_BITS                    22
#define VDE_GFX_X_END_reserved0_SHIFT                   10

/* X_END [09:00] - unsigned */
#define VDE_GFX_X_END_X_END_MASK                        0x000003FFUL
#define VDE_GFX_X_END_X_END_ALIGN                       0
#define VDE_GFX_X_END_X_END_BITS                        10
#define VDE_GFX_X_END_X_END_SHIFT                       0

/**********************************************************************
 * VDE_GFX_Y_START
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_GFX_Y_START_reserved0_MASK                  0xFFFFFC00UL
#define VDE_GFX_Y_START_reserved0_ALIGN                 0
#define VDE_GFX_Y_START_reserved0_BITS                  22
#define VDE_GFX_Y_START_reserved0_SHIFT                 10

/* Y_START [09:00] - unsigned */
#define VDE_GFX_Y_START_Y_START_MASK                    0x000003FFUL
#define VDE_GFX_Y_START_Y_START_ALIGN                   0
#define VDE_GFX_Y_START_Y_START_BITS                    10
#define VDE_GFX_Y_START_Y_START_SHIFT                   0

/**********************************************************************
 * VDE_GFX_Y_END
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_GFX_Y_END_reserved0_MASK                    0xFFFFFC00UL
#define VDE_GFX_Y_END_reserved0_ALIGN                   0
#define VDE_GFX_Y_END_reserved0_BITS                    22
#define VDE_GFX_Y_END_reserved0_SHIFT                   10

/* Y_END [09:00] - unsigned */
#define VDE_GFX_Y_END_Y_END_MASK                        0x000003FFUL
#define VDE_GFX_Y_END_Y_END_ALIGN                       0
#define VDE_GFX_Y_END_Y_END_BITS                        10
#define VDE_GFX_Y_END_Y_END_SHIFT                       0

/**********************************************************************
 * VDE_ANALOG_IN_X_START
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_ANALOG_IN_X_START_reserved0_MASK            0xFFFFFC00UL
#define VDE_ANALOG_IN_X_START_reserved0_ALIGN           0
#define VDE_ANALOG_IN_X_START_reserved0_BITS            22
#define VDE_ANALOG_IN_X_START_reserved0_SHIFT           10

/* X_START [09:00] - unsigned */
#define VDE_ANALOG_IN_X_START_X_START_MASK              0x000003FFUL
#define VDE_ANALOG_IN_X_START_X_START_ALIGN             0
#define VDE_ANALOG_IN_X_START_X_START_BITS              10
#define VDE_ANALOG_IN_X_START_X_START_SHIFT             0

/**********************************************************************
 * VDE_ANALOG_IN_Y_START
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_ANALOG_IN_Y_START_reserved0_MASK            0xFFFFFC00UL
#define VDE_ANALOG_IN_Y_START_reserved0_ALIGN           0
#define VDE_ANALOG_IN_Y_START_reserved0_BITS            22
#define VDE_ANALOG_IN_Y_START_reserved0_SHIFT           10

/* Y_START [09:00] - unsigned */
#define VDE_ANALOG_IN_Y_START_Y_START_MASK              0x000003FFUL
#define VDE_ANALOG_IN_Y_START_Y_START_ALIGN             0
#define VDE_ANALOG_IN_Y_START_Y_START_BITS              10
#define VDE_ANALOG_IN_Y_START_Y_START_SHIFT             0

/**********************************************************************
 * VDE_CAP_SIZE
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_CAP_SIZE_reserved0_MASK                     0xFC000000UL
#define VDE_CAP_SIZE_reserved0_ALIGN                    0
#define VDE_CAP_SIZE_reserved0_BITS                     6
#define VDE_CAP_SIZE_reserved0_SHIFT                    26

/* X_SIZE [25:16] - unsigned */
#define VDE_CAP_SIZE_X_SIZE_MASK                        0x03FF0000UL
#define VDE_CAP_SIZE_X_SIZE_ALIGN                       0
#define VDE_CAP_SIZE_X_SIZE_BITS                        10
#define VDE_CAP_SIZE_X_SIZE_SHIFT                       16

/* reserved1 [15:10] -  */
#define VDE_CAP_SIZE_reserved1_MASK                     0x0000FC00UL
#define VDE_CAP_SIZE_reserved1_ALIGN                    0
#define VDE_CAP_SIZE_reserved1_BITS                     6
#define VDE_CAP_SIZE_reserved1_SHIFT                    10

/* Y_SIZE [09:00] - unsigned */
#define VDE_CAP_SIZE_Y_SIZE_MASK                        0x000003FFUL
#define VDE_CAP_SIZE_Y_SIZE_ALIGN                       0
#define VDE_CAP_SIZE_Y_SIZE_BITS                        10
#define VDE_CAP_SIZE_Y_SIZE_SHIFT                       0

/* reserved2 [31:26] -  */
#define VDE_SCALE_SRC_SIZE_reserved2_MASK               0xFC000000UL
#define VDE_SCALE_SRC_SIZE_reserved2_ALIGN              0
#define VDE_SCALE_SRC_SIZE_reserved2_BITS               6
#define VDE_SCALE_SRC_SIZE_reserved2_SHIFT              26

/* X_SIZE [25:16] - unsigned */
#define VDE_SCALE_SRC_SIZE_X_SIZE_MASK                  0x03FF0000UL
#define VDE_SCALE_SRC_SIZE_X_SIZE_ALIGN                 0
#define VDE_SCALE_SRC_SIZE_X_SIZE_BITS                  10
#define VDE_SCALE_SRC_SIZE_X_SIZE_SHIFT                 16

/* reserved3 [15:10] -  */
#define VDE_SCALE_SRC_SIZE_reserved3_MASK               0x0000FC00UL
#define VDE_SCALE_SRC_SIZE_reserved3_ALIGN              0
#define VDE_SCALE_SRC_SIZE_reserved3_BITS               6
#define VDE_SCALE_SRC_SIZE_reserved3_SHIFT              10

/* Y_SIZE [09:00] - unsigned */
#define VDE_SCALE_SRC_SIZE_Y_SIZE_MASK                  0x000003FFUL
#define VDE_SCALE_SRC_SIZE_Y_SIZE_ALIGN                 0
#define VDE_SCALE_SRC_SIZE_Y_SIZE_BITS                  10
#define VDE_SCALE_SRC_SIZE_Y_SIZE_SHIFT                 0

/**********************************************************************
 * VDE_SCALE_X_RATIO
 **********************************************************************/
/* reserved0 [31:14] -  */
#define VDE_SCALE_X_RATIO_reserved0_MASK                0xFFFFC000UL
#define VDE_SCALE_X_RATIO_reserved0_ALIGN               0
#define VDE_SCALE_X_RATIO_reserved0_BITS                18
#define VDE_SCALE_X_RATIO_reserved0_SHIFT               14

/* SCALE_X_RATIO [13:00] - unsigned */
#define VDE_SCALE_X_RATIO_SCALE_X_RATIO_MASK            0x00003FFFUL
#define VDE_SCALE_X_RATIO_SCALE_X_RATIO_ALIGN           0
#define VDE_SCALE_X_RATIO_SCALE_X_RATIO_BITS            14
#define VDE_SCALE_X_RATIO_SCALE_X_RATIO_SHIFT           0

/**********************************************************************
 * VDE_SCALE_Y_RATIO
 **********************************************************************/
/* reserved0 [31:14] -  */
#define VDE_SCALE_Y_RATIO_reserved0_MASK                0xFFFFC000UL
#define VDE_SCALE_Y_RATIO_reserved0_ALIGN               0
#define VDE_SCALE_Y_RATIO_reserved0_BITS                18
#define VDE_SCALE_Y_RATIO_reserved0_SHIFT               14

/* SCALE_Y_RATIO [13:00] - unsigned */
#define VDE_SCALE_Y_RATIO_SCALE_Y_RATIO_MASK            0x00003FFFUL
#define VDE_SCALE_Y_RATIO_SCALE_Y_RATIO_ALIGN           0
#define VDE_SCALE_Y_RATIO_SCALE_Y_RATIO_BITS            14
#define VDE_SCALE_Y_RATIO_SCALE_Y_RATIO_SHIFT           0

/**********************************************************************
 * VDE_SCALE_X_INIT_PHASE
 **********************************************************************/
/* reserved0 [31:18] -  */
#define VDE_SCALE_X_INIT_PHASE_reserved0_MASK           0xFFFC0000UL
#define VDE_SCALE_X_INIT_PHASE_reserved0_ALIGN          0
#define VDE_SCALE_X_INIT_PHASE_reserved0_BITS           14
#define VDE_SCALE_X_INIT_PHASE_reserved0_SHIFT          18

/* SCALE_X_INIT_PHASE [17:00] - unsigned */
#define VDE_SCALE_X_INIT_PHASE_SCALE_X_INIT_PHASE_MASK  0x0003FFFFUL
#define VDE_SCALE_X_INIT_PHASE_SCALE_X_INIT_PHASE_ALIGN 0
#define VDE_SCALE_X_INIT_PHASE_SCALE_X_INIT_PHASE_BITS  18
#define VDE_SCALE_X_INIT_PHASE_SCALE_X_INIT_PHASE_SHIFT 0

/**********************************************************************
 * VDE_SCALE_Y_TF2TF_INIT_PHASE
 **********************************************************************/
/* reserved0 [31:18] -  */
#define VDE_SCALE_Y_TF2TF_INIT_PHASE_reserved0_MASK     0xFFFC0000UL
#define VDE_SCALE_Y_TF2TF_INIT_PHASE_reserved0_ALIGN    0
#define VDE_SCALE_Y_TF2TF_INIT_PHASE_reserved0_BITS     14
#define VDE_SCALE_Y_TF2TF_INIT_PHASE_reserved0_SHIFT    18

/* Y_INIT_PHASE [17:00] - signed */
#define VDE_SCALE_Y_TF2TF_INIT_PHASE_Y_INIT_PHASE_MASK  0x0003FFFFUL
#define VDE_SCALE_Y_TF2TF_INIT_PHASE_Y_INIT_PHASE_ALIGN 0
#define VDE_SCALE_Y_TF2TF_INIT_PHASE_Y_INIT_PHASE_BITS  18
#define VDE_SCALE_Y_TF2TF_INIT_PHASE_Y_INIT_PHASE_SHIFT 0

/* reserved1 [31:18] -  */
#define VDE_SCALE_Y_BF2BF_INIT_PHASE_reserved1_MASK     0xFFFC0000UL
#define VDE_SCALE_Y_BF2BF_INIT_PHASE_reserved1_ALIGN    0
#define VDE_SCALE_Y_BF2BF_INIT_PHASE_reserved1_BITS     14
#define VDE_SCALE_Y_BF2BF_INIT_PHASE_reserved1_SHIFT    18

/* Y_INIT_PHASE [17:00] - signed */
#define VDE_SCALE_Y_BF2BF_INIT_PHASE_Y_INIT_PHASE_MASK  0x0003FFFFUL
#define VDE_SCALE_Y_BF2BF_INIT_PHASE_Y_INIT_PHASE_ALIGN 0
#define VDE_SCALE_Y_BF2BF_INIT_PHASE_Y_INIT_PHASE_BITS  18
#define VDE_SCALE_Y_BF2BF_INIT_PHASE_Y_INIT_PHASE_SHIFT 0

/* reserved2 [31:18] -  */
#define VDE_SCALE_Y_TF2BF_INIT_PHASE_reserved2_MASK     0xFFFC0000UL
#define VDE_SCALE_Y_TF2BF_INIT_PHASE_reserved2_ALIGN    0
#define VDE_SCALE_Y_TF2BF_INIT_PHASE_reserved2_BITS     14
#define VDE_SCALE_Y_TF2BF_INIT_PHASE_reserved2_SHIFT    18

/* Y_INIT_PHASE [17:00] - signed */
#define VDE_SCALE_Y_TF2BF_INIT_PHASE_Y_INIT_PHASE_MASK  0x0003FFFFUL
#define VDE_SCALE_Y_TF2BF_INIT_PHASE_Y_INIT_PHASE_ALIGN 0
#define VDE_SCALE_Y_TF2BF_INIT_PHASE_Y_INIT_PHASE_BITS  18
#define VDE_SCALE_Y_TF2BF_INIT_PHASE_Y_INIT_PHASE_SHIFT 0

/* reserved3 [31:18] -  */
#define VDE_SCALE_Y_BF2TF_INIT_PHASE_reserved3_MASK     0xFFFC0000UL
#define VDE_SCALE_Y_BF2TF_INIT_PHASE_reserved3_ALIGN    0
#define VDE_SCALE_Y_BF2TF_INIT_PHASE_reserved3_BITS     14
#define VDE_SCALE_Y_BF2TF_INIT_PHASE_reserved3_SHIFT    18

/* Y_INIT_PHASE [17:00] - signed */
#define VDE_SCALE_Y_BF2TF_INIT_PHASE_Y_INIT_PHASE_MASK  0x0003FFFFUL
#define VDE_SCALE_Y_BF2TF_INIT_PHASE_Y_INIT_PHASE_ALIGN 0
#define VDE_SCALE_Y_BF2TF_INIT_PHASE_Y_INIT_PHASE_BITS  18
#define VDE_SCALE_Y_BF2TF_INIT_PHASE_Y_INIT_PHASE_SHIFT 0

/**********************************************************************
 * VDE_CAP_TF_MSTART
 **********************************************************************/
/* reserved0 [31:28] -  */
#define VDE_CAP_TF_MSTART_reserved0_MASK                0xF0000000UL
#define VDE_CAP_TF_MSTART_reserved0_ALIGN               0
#define VDE_CAP_TF_MSTART_reserved0_BITS                4
#define VDE_CAP_TF_MSTART_reserved0_SHIFT               28

/* MEM_ADDR [27:00] - unsigned */
#define VDE_CAP_TF_MSTART_MEM_ADDR_MASK                 0x0FFFFFFFUL
#define VDE_CAP_TF_MSTART_MEM_ADDR_ALIGN                0
#define VDE_CAP_TF_MSTART_MEM_ADDR_BITS                 28
#define VDE_CAP_TF_MSTART_MEM_ADDR_SHIFT                0

/* reserved1 [31:28] -  */
#define VDE_CAP_BF_MSTART_reserved1_MASK                0xF0000000UL
#define VDE_CAP_BF_MSTART_reserved1_ALIGN               0
#define VDE_CAP_BF_MSTART_reserved1_BITS                4
#define VDE_CAP_BF_MSTART_reserved1_SHIFT               28

/* MEM_ADDR [27:00] - unsigned */
#define VDE_CAP_BF_MSTART_MEM_ADDR_MASK                 0x0FFFFFFFUL
#define VDE_CAP_BF_MSTART_MEM_ADDR_ALIGN                0
#define VDE_CAP_BF_MSTART_MEM_ADDR_BITS                 28
#define VDE_CAP_BF_MSTART_MEM_ADDR_SHIFT                0

/**********************************************************************
 * VDE_PLBK_TF_MSTART
 **********************************************************************/
/* reserved0 [31:28] -  */
#define VDE_PLBK_TF_MSTART_reserved0_MASK               0xF0000000UL
#define VDE_PLBK_TF_MSTART_reserved0_ALIGN              0
#define VDE_PLBK_TF_MSTART_reserved0_BITS               4
#define VDE_PLBK_TF_MSTART_reserved0_SHIFT              28

/* MEM_ADDR [27:00] - unsigned */
#define VDE_PLBK_TF_MSTART_MEM_ADDR_MASK                0x0FFFFFFFUL
#define VDE_PLBK_TF_MSTART_MEM_ADDR_ALIGN               0
#define VDE_PLBK_TF_MSTART_MEM_ADDR_BITS                28
#define VDE_PLBK_TF_MSTART_MEM_ADDR_SHIFT               0

/* reserved1 [31:28] -  */
#define VDE_PLBK_BF_MSTART_reserved1_MASK               0xF0000000UL
#define VDE_PLBK_BF_MSTART_reserved1_ALIGN              0
#define VDE_PLBK_BF_MSTART_reserved1_BITS               4
#define VDE_PLBK_BF_MSTART_reserved1_SHIFT              28

/* MEM_ADDR [27:00] - unsigned */
#define VDE_PLBK_BF_MSTART_MEM_ADDR_MASK                0x0FFFFFFFUL
#define VDE_PLBK_BF_MSTART_MEM_ADDR_ALIGN               0
#define VDE_PLBK_BF_MSTART_MEM_ADDR_BITS                28
#define VDE_PLBK_BF_MSTART_MEM_ADDR_SHIFT               0

/**********************************************************************
 * VDE_CAP_PITCH
 **********************************************************************/
/* reserved0 [31:13] -  */
#define VDE_CAP_PITCH_reserved0_MASK                    0xFFFFE000UL
#define VDE_CAP_PITCH_reserved0_ALIGN                   0
#define VDE_CAP_PITCH_reserved0_BITS                    19
#define VDE_CAP_PITCH_reserved0_SHIFT                   13

/* PITCH [12:00] - unsigned */
#define VDE_CAP_PITCH_PITCH_MASK                        0x00001FFFUL
#define VDE_CAP_PITCH_PITCH_ALIGN                       0
#define VDE_CAP_PITCH_PITCH_BITS                        13
#define VDE_CAP_PITCH_PITCH_SHIFT                       0

/**********************************************************************
 * VDE_PLBK_PITCH
 **********************************************************************/
/* reserved0 [31:13] -  */
#define VDE_PLBK_PITCH_reserved0_MASK                   0xFFFFE000UL
#define VDE_PLBK_PITCH_reserved0_ALIGN                  0
#define VDE_PLBK_PITCH_reserved0_BITS                   19
#define VDE_PLBK_PITCH_reserved0_SHIFT                  13

/* PITCH [12:00] - unsigned */
#define VDE_PLBK_PITCH_PITCH_MASK                       0x00001FFFUL
#define VDE_PLBK_PITCH_PITCH_ALIGN                      0
#define VDE_PLBK_PITCH_PITCH_BITS                       13
#define VDE_PLBK_PITCH_PITCH_SHIFT                      0

/**********************************************************************
 * VDE_TF_RASTER
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_TF_RASTER_reserved0_MASK                    0xFFFFFC00UL
#define VDE_TF_RASTER_reserved0_ALIGN                   0
#define VDE_TF_RASTER_reserved0_BITS                    22
#define VDE_TF_RASTER_reserved0_SHIFT                   10

/* LINE_COUNT [09:00] - unsigned */
#define VDE_TF_RASTER_LINE_COUNT_MASK                   0x000003FFUL
#define VDE_TF_RASTER_LINE_COUNT_ALIGN                  0
#define VDE_TF_RASTER_LINE_COUNT_BITS                   10
#define VDE_TF_RASTER_LINE_COUNT_SHIFT                  0

/* reserved1 [31:10] -  */
#define VDE_BF_RASTER_reserved1_MASK                    0xFFFFFC00UL
#define VDE_BF_RASTER_reserved1_ALIGN                   0
#define VDE_BF_RASTER_reserved1_BITS                    22
#define VDE_BF_RASTER_reserved1_SHIFT                   10

/* LINE_COUNT [09:00] - unsigned */
#define VDE_BF_RASTER_LINE_COUNT_MASK                   0x000003FFUL
#define VDE_BF_RASTER_LINE_COUNT_ALIGN                  0
#define VDE_BF_RASTER_LINE_COUNT_BITS                   10
#define VDE_BF_RASTER_LINE_COUNT_SHIFT                  0

/**********************************************************************
 * VDE_BG_COLOR
 **********************************************************************/
/* reserved0 [31:24] -  */
#define VDE_BG_COLOR_reserved0_MASK                     0xFF000000UL
#define VDE_BG_COLOR_reserved0_ALIGN                    0
#define VDE_BG_COLOR_reserved0_BITS                     8
#define VDE_BG_COLOR_reserved0_SHIFT                    24

/* COLOR [23:00] - unsigned */
#define VDE_BG_COLOR_COLOR_MASK                         0x00FFFFFFUL
#define VDE_BG_COLOR_COLOR_ALIGN                        0
#define VDE_BG_COLOR_COLOR_BITS                         24
#define VDE_BG_COLOR_COLOR_SHIFT                        0

/**********************************************************************
 * VDE_VWIN_ALPHA
 **********************************************************************/
/* reserved0 [31:08] -  */
#define VDE_VWIN_ALPHA_reserved0_MASK                   0xFFFFFF00UL
#define VDE_VWIN_ALPHA_reserved0_ALIGN                  0
#define VDE_VWIN_ALPHA_reserved0_BITS                   24
#define VDE_VWIN_ALPHA_reserved0_SHIFT                  8

/* FRONT_VWIN_ALPHA [07:00] - unsigned */
#define VDE_VWIN_ALPHA_FRONT_VWIN_ALPHA_MASK            0x000000FFUL
#define VDE_VWIN_ALPHA_FRONT_VWIN_ALPHA_ALIGN           0
#define VDE_VWIN_ALPHA_FRONT_VWIN_ALPHA_BITS            8
#define VDE_VWIN_ALPHA_FRONT_VWIN_ALPHA_SHIFT           0

/**********************************************************************
 * VDE_MPEG_H_LENGTH
 **********************************************************************/
/* reserved0 [31:10] -  */
#define VDE_MPEG_H_LENGTH_reserved0_MASK                0xFFFFFC00UL
#define VDE_MPEG_H_LENGTH_reserved0_ALIGN               0
#define VDE_MPEG_H_LENGTH_reserved0_BITS                22
#define VDE_MPEG_H_LENGTH_reserved0_SHIFT               10

/* H_LENGTH [09:00] - unsigned */
#define VDE_MPEG_H_LENGTH_H_LENGTH_MASK                 0x000003FFUL
#define VDE_MPEG_H_LENGTH_H_LENGTH_ALIGN                0
#define VDE_MPEG_H_LENGTH_H_LENGTH_BITS                 10
#define VDE_MPEG_H_LENGTH_H_LENGTH_SHIFT                0

/**********************************************************************
 * VDE_CAP_MEND
 **********************************************************************/
/* reserved0 [31:28] -  */
#define VDE_CAP_MEND_reserved0_MASK                     0xF0000000UL
#define VDE_CAP_MEND_reserved0_ALIGN                    0
#define VDE_CAP_MEND_reserved0_BITS                     4
#define VDE_CAP_MEND_reserved0_SHIFT                    28

/* MEM_ADDR [27:00] - unsigned */
#define VDE_CAP_MEND_MEM_ADDR_MASK                      0x0FFFFFFFUL
#define VDE_CAP_MEND_MEM_ADDR_ALIGN                     0
#define VDE_CAP_MEND_MEM_ADDR_BITS                      28
#define VDE_CAP_MEND_MEM_ADDR_SHIFT                     0

/**********************************************************************
 * VDE_VDE_CAP_CTRL
 **********************************************************************/
/* reserved0 [31:12] -  */
#define VDE_VDE_CAP_CTRL_reserved0_MASK                 0xFFFFF000UL
#define VDE_VDE_CAP_CTRL_reserved0_ALIGN                0
#define VDE_VDE_CAP_CTRL_reserved0_BITS                 20
#define VDE_VDE_CAP_CTRL_reserved0_SHIFT                12

/* CAP_TF_SRC [11:11] - unsigned */
#define VDE_VDE_CAP_CTRL_CAP_TF_SRC_MASK                0x00000800UL
#define VDE_VDE_CAP_CTRL_CAP_TF_SRC_ALIGN               0
#define VDE_VDE_CAP_CTRL_CAP_TF_SRC_BITS                1
#define VDE_VDE_CAP_CTRL_CAP_TF_SRC_SHIFT               11
#define VDE_VDE_CAP_CTRL_CAP_TF_SRC_USE_CRT_TF          1
#define VDE_VDE_CAP_CTRL_CAP_TF_SRC_USE_SRC_TF          0

/* CAP_FIFO_RESET [10:10] - unsigned */
#define VDE_VDE_CAP_CTRL_CAP_FIFO_RESET_MASK            0x00000400UL
#define VDE_VDE_CAP_CTRL_CAP_FIFO_RESET_ALIGN           0
#define VDE_VDE_CAP_CTRL_CAP_FIFO_RESET_BITS            1
#define VDE_VDE_CAP_CTRL_CAP_FIFO_RESET_SHIFT           10
#define VDE_VDE_CAP_CTRL_CAP_FIFO_RESET_RESET_AT_HSYNC  1
#define VDE_VDE_CAP_CTRL_CAP_FIFO_RESET_RESET_AT_VSYNC  0

/* reserved1 [09:08] -  */
#define VDE_VDE_CAP_CTRL_reserved1_MASK                 0x00000300UL
#define VDE_VDE_CAP_CTRL_reserved1_ALIGN                0
#define VDE_VDE_CAP_CTRL_reserved1_BITS                 2
#define VDE_VDE_CAP_CTRL_reserved1_SHIFT                8

/* CLEAR_FIFO_STATUS [07:07] - unsigned */
#define VDE_VDE_CAP_CTRL_CLEAR_FIFO_STATUS_MASK         0x00000080UL
#define VDE_VDE_CAP_CTRL_CLEAR_FIFO_STATUS_ALIGN        0
#define VDE_VDE_CAP_CTRL_CLEAR_FIFO_STATUS_BITS         1
#define VDE_VDE_CAP_CTRL_CLEAR_FIFO_STATUS_SHIFT        7

/* CAP_MEM_BYTE_SWAP [06:06] - unsigned */
#define VDE_VDE_CAP_CTRL_CAP_MEM_BYTE_SWAP_MASK         0x00000040UL
#define VDE_VDE_CAP_CTRL_CAP_MEM_BYTE_SWAP_ALIGN        0
#define VDE_VDE_CAP_CTRL_CAP_MEM_BYTE_SWAP_BITS         1
#define VDE_VDE_CAP_CTRL_CAP_MEM_BYTE_SWAP_SHIFT        6

/* CAP_SRC [05:04] - unsigned */
#define VDE_VDE_CAP_CTRL_CAP_SRC_MASK                   0x00000030UL
#define VDE_VDE_CAP_CTRL_CAP_SRC_ALIGN                  0
#define VDE_VDE_CAP_CTRL_CAP_SRC_BITS                   2
#define VDE_VDE_CAP_CTRL_CAP_SRC_SHIFT                  4

/* MODE_SWITCH_CTRL [03:02] - unsigned */
#define VDE_VDE_CAP_CTRL_MODE_SWITCH_CTRL_MASK          0x0000000CUL
#define VDE_VDE_CAP_CTRL_MODE_SWITCH_CTRL_ALIGN         0
#define VDE_VDE_CAP_CTRL_MODE_SWITCH_CTRL_BITS          2
#define VDE_VDE_CAP_CTRL_MODE_SWITCH_CTRL_SHIFT         2
#define VDE_VDE_CAP_CTRL_MODE_SWITCH_CTRL_UPDATE_DISABLE 0
#define VDE_VDE_CAP_CTRL_MODE_SWITCH_CTRL_UPDATE_AT_VSYNC 1
#define VDE_VDE_CAP_CTRL_MODE_SWITCH_CTRL_UPDATE_BY_TITAN 2
#define VDE_VDE_CAP_CTRL_MODE_SWITCH_CTRL_UPDATE_NOW    3

/* CAP_CONT [01:01] - boolean */
#define VDE_VDE_CAP_CTRL_CAP_CONT_MASK                  0x00000002UL
#define VDE_VDE_CAP_CTRL_CAP_CONT_ALIGN                 0
#define VDE_VDE_CAP_CTRL_CAP_CONT_BITS                  1
#define VDE_VDE_CAP_CTRL_CAP_CONT_SHIFT                 1
#define VDE_VDE_CAP_CTRL_CAP_CONT_SINGLE_FRAME          0
#define VDE_VDE_CAP_CTRL_CAP_CONT_CONTINUOUS            1

/* CAP_EN [00:00] - boolean */
#define VDE_VDE_CAP_CTRL_CAP_EN_MASK                    0x00000001UL
#define VDE_VDE_CAP_CTRL_CAP_EN_ALIGN                   0
#define VDE_VDE_CAP_CTRL_CAP_EN_BITS                    1
#define VDE_VDE_CAP_CTRL_CAP_EN_SHIFT                   0

/**********************************************************************
 * VDE_VDE_SCALE_CTRL
 **********************************************************************/
/* reserved0 [31:08] -  */
#define VDE_VDE_SCALE_CTRL_reserved0_MASK               0xFFFFFF00UL
#define VDE_VDE_SCALE_CTRL_reserved0_ALIGN              0
#define VDE_VDE_SCALE_CTRL_reserved0_BITS               24
#define VDE_VDE_SCALE_CTRL_reserved0_SHIFT              8

/* reserved1 [07:06] -  */
#define VDE_VDE_SCALE_CTRL_reserved1_MASK               0x000000C0UL
#define VDE_VDE_SCALE_CTRL_reserved1_ALIGN              0
#define VDE_VDE_SCALE_CTRL_reserved1_BITS               2
#define VDE_VDE_SCALE_CTRL_reserved1_SHIFT              6

/* SCALER_SRC [05:04] - unsigned */
#define VDE_VDE_SCALE_CTRL_SCALER_SRC_MASK              0x00000030UL
#define VDE_VDE_SCALE_CTRL_SCALER_SRC_ALIGN             0
#define VDE_VDE_SCALE_CTRL_SCALER_SRC_BITS              2
#define VDE_VDE_SCALE_CTRL_SCALER_SRC_SHIFT             4

/* reserved2 [03:01] -  */
#define VDE_VDE_SCALE_CTRL_reserved2_MASK               0x0000000EUL
#define VDE_VDE_SCALE_CTRL_reserved2_ALIGN              0
#define VDE_VDE_SCALE_CTRL_reserved2_BITS               3
#define VDE_VDE_SCALE_CTRL_reserved2_SHIFT              1

/* SCALER_EN [00:00] - boolean */
#define VDE_VDE_SCALE_CTRL_SCALER_EN_MASK               0x00000001UL
#define VDE_VDE_SCALE_CTRL_SCALER_EN_ALIGN              0
#define VDE_VDE_SCALE_CTRL_SCALER_EN_BITS               1
#define VDE_VDE_SCALE_CTRL_SCALER_EN_SHIFT              0

/**********************************************************************
 * VDE_VDE_DISP_CTRL
 **********************************************************************/
/* reserved0 [31:30] -  */
#define VDE_VDE_DISP_CTRL_reserved0_MASK                0xC0000000UL
#define VDE_VDE_DISP_CTRL_reserved0_ALIGN               0
#define VDE_VDE_DISP_CTRL_reserved0_BITS                2
#define VDE_VDE_DISP_CTRL_reserved0_SHIFT               30

/* YUV_MAX_CLIP_ENABLE [29:29] - unsigned */
#define VDE_VDE_DISP_CTRL_YUV_MAX_CLIP_ENABLE_MASK      0x20000000UL
#define VDE_VDE_DISP_CTRL_YUV_MAX_CLIP_ENABLE_ALIGN     0
#define VDE_VDE_DISP_CTRL_YUV_MAX_CLIP_ENABLE_BITS      1
#define VDE_VDE_DISP_CTRL_YUV_MAX_CLIP_ENABLE_SHIFT     29

/* YUV_MIN_CLIP_ENABLE [28:28] - unsigned */
#define VDE_VDE_DISP_CTRL_YUV_MIN_CLIP_ENABLE_MASK      0x10000000UL
#define VDE_VDE_DISP_CTRL_YUV_MIN_CLIP_ENABLE_ALIGN     0
#define VDE_VDE_DISP_CTRL_YUV_MIN_CLIP_ENABLE_BITS      1
#define VDE_VDE_DISP_CTRL_YUV_MIN_CLIP_ENABLE_SHIFT     28

/* CROSS_VWIN [27:27] - unsigned */
#define VDE_VDE_DISP_CTRL_CROSS_VWIN_MASK               0x08000000UL
#define VDE_VDE_DISP_CTRL_CROSS_VWIN_ALIGN              0
#define VDE_VDE_DISP_CTRL_CROSS_VWIN_BITS               1
#define VDE_VDE_DISP_CTRL_CROSS_VWIN_SHIFT              27

/* CRTC_SLAVE [26:26] - unsigned */
#define VDE_VDE_DISP_CTRL_CRTC_SLAVE_MASK               0x04000000UL
#define VDE_VDE_DISP_CTRL_CRTC_SLAVE_ALIGN              0
#define VDE_VDE_DISP_CTRL_CRTC_SLAVE_BITS               1
#define VDE_VDE_DISP_CTRL_CRTC_SLAVE_SHIFT              26
#define VDE_VDE_DISP_CTRL_CRTC_SLAVE_FREE_RUN           0
#define VDE_VDE_DISP_CTRL_CRTC_SLAVE_SLAVE_MODE         1

/* DISP_CLK_SELECT [25:25] - unsigned */
#define VDE_VDE_DISP_CTRL_DISP_CLK_SELECT_MASK          0x02000000UL
#define VDE_VDE_DISP_CTRL_DISP_CLK_SELECT_ALIGN         0
#define VDE_VDE_DISP_CTRL_DISP_CLK_SELECT_BITS          1
#define VDE_VDE_DISP_CTRL_DISP_CLK_SELECT_SHIFT         25

/* ANALOG_IN_SCAN_TYPE [24:24] - unsigned */
#define VDE_VDE_DISP_CTRL_ANALOG_IN_SCAN_TYPE_MASK      0x01000000UL
#define VDE_VDE_DISP_CTRL_ANALOG_IN_SCAN_TYPE_ALIGN     0
#define VDE_VDE_DISP_CTRL_ANALOG_IN_SCAN_TYPE_BITS      1
#define VDE_VDE_DISP_CTRL_ANALOG_IN_SCAN_TYPE_SHIFT     24

/* MPEG_SRC_CHROMA_LOC [23:23] - unsigned */
#define VDE_VDE_DISP_CTRL_MPEG_SRC_CHROMA_LOC_MASK      0x00800000UL
#define VDE_VDE_DISP_CTRL_MPEG_SRC_CHROMA_LOC_ALIGN     0
#define VDE_VDE_DISP_CTRL_MPEG_SRC_CHROMA_LOC_BITS      1
#define VDE_VDE_DISP_CTRL_MPEG_SRC_CHROMA_LOC_SHIFT     23

/* ANALOG_SRC_CHROMA_LOC [22:22] - unsigned */
#define VDE_VDE_DISP_CTRL_ANALOG_SRC_CHROMA_LOC_MASK    0x00400000UL
#define VDE_VDE_DISP_CTRL_ANALOG_SRC_CHROMA_LOC_ALIGN   0
#define VDE_VDE_DISP_CTRL_ANALOG_SRC_CHROMA_LOC_BITS    1
#define VDE_VDE_DISP_CTRL_ANALOG_SRC_CHROMA_LOC_SHIFT   22

/* PLBK_SRC_CHROMA_LOC [21:21] - unsigned */
#define VDE_VDE_DISP_CTRL_PLBK_SRC_CHROMA_LOC_MASK      0x00200000UL
#define VDE_VDE_DISP_CTRL_PLBK_SRC_CHROMA_LOC_ALIGN     0
#define VDE_VDE_DISP_CTRL_PLBK_SRC_CHROMA_LOC_BITS      1
#define VDE_VDE_DISP_CTRL_PLBK_SRC_CHROMA_LOC_SHIFT     21

/* PLBK_MEM_BYTE_SWAP [20:20] - unsigned */
#define VDE_VDE_DISP_CTRL_PLBK_MEM_BYTE_SWAP_MASK       0x00100000UL
#define VDE_VDE_DISP_CTRL_PLBK_MEM_BYTE_SWAP_ALIGN      0
#define VDE_VDE_DISP_CTRL_PLBK_MEM_BYTE_SWAP_BITS       1
#define VDE_VDE_DISP_CTRL_PLBK_MEM_BYTE_SWAP_SHIFT      20

/* CROSS_VWIN_SRC [19:19] - unsigned */
#define VDE_VDE_DISP_CTRL_CROSS_VWIN_SRC_MASK           0x00080000UL
#define VDE_VDE_DISP_CTRL_CROSS_VWIN_SRC_ALIGN          0
#define VDE_VDE_DISP_CTRL_CROSS_VWIN_SRC_BITS           1
#define VDE_VDE_DISP_CTRL_CROSS_VWIN_SRC_SHIFT          19
#define VDE_VDE_DISP_CTRL_CROSS_VWIN_SRC_VWIN1          0
#define VDE_VDE_DISP_CTRL_CROSS_VWIN_SRC_VWIN2          1

/* reserved1 [18:12] -  */
#define VDE_VDE_DISP_CTRL_reserved1_MASK                0x0007F000UL
#define VDE_VDE_DISP_CTRL_reserved1_ALIGN               0
#define VDE_VDE_DISP_CTRL_reserved1_BITS                7
#define VDE_VDE_DISP_CTRL_reserved1_SHIFT               12

/* VWIN2_SRC [11:10] - unsigned */
#define VDE_VDE_DISP_CTRL_VWIN2_SRC_MASK                0x00000C00UL
#define VDE_VDE_DISP_CTRL_VWIN2_SRC_ALIGN               0
#define VDE_VDE_DISP_CTRL_VWIN2_SRC_BITS                2
#define VDE_VDE_DISP_CTRL_VWIN2_SRC_SHIFT               10

/* VWIN1_SRC [09:08] - unsigned */
#define VDE_VDE_DISP_CTRL_VWIN1_SRC_MASK                0x00000300UL
#define VDE_VDE_DISP_CTRL_VWIN1_SRC_ALIGN               0
#define VDE_VDE_DISP_CTRL_VWIN1_SRC_BITS                2
#define VDE_VDE_DISP_CTRL_VWIN1_SRC_SHIFT               8

/* VWIN_ORDER [07:07] - unsigned */
#define VDE_VDE_DISP_CTRL_VWIN_ORDER_MASK               0x00000080UL
#define VDE_VDE_DISP_CTRL_VWIN_ORDER_ALIGN              0
#define VDE_VDE_DISP_CTRL_VWIN_ORDER_BITS               1
#define VDE_VDE_DISP_CTRL_VWIN_ORDER_SHIFT              7
#define VDE_VDE_DISP_CTRL_VWIN_ORDER_VWIN1_ON_FRONT     0
#define VDE_VDE_DISP_CTRL_VWIN_ORDER_VWIN2_ON_FRONT     1

/* ANALOG_IN_TYPE [06:06] - unsigned */
#define VDE_VDE_DISP_CTRL_ANALOG_IN_TYPE_MASK           0x00000040UL
#define VDE_VDE_DISP_CTRL_ANALOG_IN_TYPE_ALIGN          0
#define VDE_VDE_DISP_CTRL_ANALOG_IN_TYPE_BITS           1
#define VDE_VDE_DISP_CTRL_ANALOG_IN_TYPE_SHIFT          6

/* ANALOG_IN_SRC [05:04] - unsigned */
#define VDE_VDE_DISP_CTRL_ANALOG_IN_SRC_MASK            0x00000030UL
#define VDE_VDE_DISP_CTRL_ANALOG_IN_SRC_ALIGN           0
#define VDE_VDE_DISP_CTRL_ANALOG_IN_SRC_BITS            2
#define VDE_VDE_DISP_CTRL_ANALOG_IN_SRC_SHIFT           4

/* PLBK_EN [03:03] - boolean */
#define VDE_VDE_DISP_CTRL_PLBK_EN_MASK                  0x00000008UL
#define VDE_VDE_DISP_CTRL_PLBK_EN_ALIGN                 0
#define VDE_VDE_DISP_CTRL_PLBK_EN_BITS                  1
#define VDE_VDE_DISP_CTRL_PLBK_EN_SHIFT                 3

/* VWIN2_EN [02:02] - boolean */
#define VDE_VDE_DISP_CTRL_VWIN2_EN_MASK                 0x00000004UL
#define VDE_VDE_DISP_CTRL_VWIN2_EN_ALIGN                0
#define VDE_VDE_DISP_CTRL_VWIN2_EN_BITS                 1
#define VDE_VDE_DISP_CTRL_VWIN2_EN_SHIFT                2

/* VWIN1_EN [01:01] - boolean */
#define VDE_VDE_DISP_CTRL_VWIN1_EN_MASK                 0x00000002UL
#define VDE_VDE_DISP_CTRL_VWIN1_EN_ALIGN                0
#define VDE_VDE_DISP_CTRL_VWIN1_EN_BITS                 1
#define VDE_VDE_DISP_CTRL_VWIN1_EN_SHIFT                1

/* CRTC_EN [00:00] - boolean */
#define VDE_VDE_DISP_CTRL_CRTC_EN_MASK                  0x00000001UL
#define VDE_VDE_DISP_CTRL_CRTC_EN_ALIGN                 0
#define VDE_VDE_DISP_CTRL_CRTC_EN_BITS                  1
#define VDE_VDE_DISP_CTRL_CRTC_EN_SHIFT                 0

/**********************************************************************
 * VDE_SCRATCH_REG1
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VDE_SCRATCH_REG1_reserved0_MASK                 0xFFFF0000UL
#define VDE_SCRATCH_REG1_reserved0_ALIGN                0
#define VDE_SCRATCH_REG1_reserved0_BITS                 16
#define VDE_SCRATCH_REG1_reserved0_SHIFT                16

/* SCRATCH_REGISTER [15:00] - unsigned */
#define VDE_SCRATCH_REG1_SCRATCH_REGISTER_MASK          0x0000FFFFUL
#define VDE_SCRATCH_REG1_SCRATCH_REGISTER_ALIGN         0
#define VDE_SCRATCH_REG1_SCRATCH_REGISTER_BITS          16
#define VDE_SCRATCH_REG1_SCRATCH_REGISTER_SHIFT         0

/**********************************************************************
 * VDE_CRTC_STATUS
 **********************************************************************/
/* CAP_FIELD_ID [31:31] - unsigned */
#define VDE_CRTC_STATUS_CAP_FIELD_ID_MASK               0x80000000UL
#define VDE_CRTC_STATUS_CAP_FIELD_ID_ALIGN              0
#define VDE_CRTC_STATUS_CAP_FIELD_ID_BITS               1
#define VDE_CRTC_STATUS_CAP_FIELD_ID_SHIFT              31

/* reserved0 [30:26] -  */
#define VDE_CRTC_STATUS_reserved0_MASK                  0x7C000000UL
#define VDE_CRTC_STATUS_reserved0_ALIGN                 0
#define VDE_CRTC_STATUS_reserved0_BITS                  5
#define VDE_CRTC_STATUS_reserved0_SHIFT                 26

/* CAP_V_COUNT [25:16] - unsigned */
#define VDE_CRTC_STATUS_CAP_V_COUNT_MASK                0x03FF0000UL
#define VDE_CRTC_STATUS_CAP_V_COUNT_ALIGN               0
#define VDE_CRTC_STATUS_CAP_V_COUNT_BITS                10
#define VDE_CRTC_STATUS_CAP_V_COUNT_SHIFT               16

/* CRT_FIELD_ID [15:15] - unsigned */
#define VDE_CRTC_STATUS_CRT_FIELD_ID_MASK               0x00008000UL
#define VDE_CRTC_STATUS_CRT_FIELD_ID_ALIGN              0
#define VDE_CRTC_STATUS_CRT_FIELD_ID_BITS               1
#define VDE_CRTC_STATUS_CRT_FIELD_ID_SHIFT              15

/* reserved1 [14:10] -  */
#define VDE_CRTC_STATUS_reserved1_MASK                  0x00007C00UL
#define VDE_CRTC_STATUS_reserved1_ALIGN                 0
#define VDE_CRTC_STATUS_reserved1_BITS                  5
#define VDE_CRTC_STATUS_reserved1_SHIFT                 10

/* CRT_V_COUNT [09:00] - unsigned */
#define VDE_CRTC_STATUS_CRT_V_COUNT_MASK                0x000003FFUL
#define VDE_CRTC_STATUS_CRT_V_COUNT_ALIGN               0
#define VDE_CRTC_STATUS_CRT_V_COUNT_BITS                10
#define VDE_CRTC_STATUS_CRT_V_COUNT_SHIFT               0

/**********************************************************************
 * VDE_VDE_REVISION
 **********************************************************************/
/* reserved0 [31:16] -  */
#define VDE_VDE_REVISION_reserved0_MASK                 0xFFFF0000UL
#define VDE_VDE_REVISION_reserved0_ALIGN                0
#define VDE_VDE_REVISION_reserved0_BITS                 16
#define VDE_VDE_REVISION_reserved0_SHIFT                16

/* MAJOR [15:08] - unsigned */
#define VDE_VDE_REVISION_MAJOR_MASK                     0x0000FF00UL
#define VDE_VDE_REVISION_MAJOR_ALIGN                    0
#define VDE_VDE_REVISION_MAJOR_BITS                     8
#define VDE_VDE_REVISION_MAJOR_SHIFT                    8

/* MINOR [07:00] - unsigned */
#define VDE_VDE_REVISION_MINOR_MASK                     0x000000FFUL
#define VDE_VDE_REVISION_MINOR_ALIGN                    0
#define VDE_VDE_REVISION_MINOR_BITS                     8
#define VDE_VDE_REVISION_MINOR_SHIFT                    0

/**********************************************************************
 * VDE_CURRENT_CAP_MSTART
 **********************************************************************/
/* reserved0 [31:28] -  */
#define VDE_CURRENT_CAP_MSTART_reserved0_MASK           0xF0000000UL
#define VDE_CURRENT_CAP_MSTART_reserved0_ALIGN          0
#define VDE_CURRENT_CAP_MSTART_reserved0_BITS           4
#define VDE_CURRENT_CAP_MSTART_reserved0_SHIFT          28

/* MEM_ADDR [27:00] - unsigned */
#define VDE_CURRENT_CAP_MSTART_MEM_ADDR_MASK            0x0FFFFFFFUL
#define VDE_CURRENT_CAP_MSTART_MEM_ADDR_ALIGN           0
#define VDE_CURRENT_CAP_MSTART_MEM_ADDR_BITS            28
#define VDE_CURRENT_CAP_MSTART_MEM_ADDR_SHIFT           0

/* reserved1 [31:28] -  */
#define VDE_CURRENT_PLBK_MSTART_reserved1_MASK          0xF0000000UL
#define VDE_CURRENT_PLBK_MSTART_reserved1_ALIGN         0
#define VDE_CURRENT_PLBK_MSTART_reserved1_BITS          4
#define VDE_CURRENT_PLBK_MSTART_reserved1_SHIFT         28

/* MEM_ADDR [27:00] - unsigned */
#define VDE_CURRENT_PLBK_MSTART_MEM_ADDR_MASK           0x0FFFFFFFUL
#define VDE_CURRENT_PLBK_MSTART_MEM_ADDR_ALIGN          0
#define VDE_CURRENT_PLBK_MSTART_MEM_ADDR_BITS           28
#define VDE_CURRENT_PLBK_MSTART_MEM_ADDR_SHIFT          0

/**********************************************************************
 * VDE_DEBUG_PIXEL_OFFSET
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_DEBUG_PIXEL_OFFSET_reserved0_MASK           0xFC000000UL
#define VDE_DEBUG_PIXEL_OFFSET_reserved0_ALIGN          0
#define VDE_DEBUG_PIXEL_OFFSET_reserved0_BITS           6
#define VDE_DEBUG_PIXEL_OFFSET_reserved0_SHIFT          26

/* X_OFFSET [25:16] - unsigned */
#define VDE_DEBUG_PIXEL_OFFSET_X_OFFSET_MASK            0x03FF0000UL
#define VDE_DEBUG_PIXEL_OFFSET_X_OFFSET_ALIGN           0
#define VDE_DEBUG_PIXEL_OFFSET_X_OFFSET_BITS            10
#define VDE_DEBUG_PIXEL_OFFSET_X_OFFSET_SHIFT           16

/* reserved1 [15:10] -  */
#define VDE_DEBUG_PIXEL_OFFSET_reserved1_MASK           0x0000FC00UL
#define VDE_DEBUG_PIXEL_OFFSET_reserved1_ALIGN          0
#define VDE_DEBUG_PIXEL_OFFSET_reserved1_BITS           6
#define VDE_DEBUG_PIXEL_OFFSET_reserved1_SHIFT          10

/* Y_OFFSET [09:00] - unsigned */
#define VDE_DEBUG_PIXEL_OFFSET_Y_OFFSET_MASK            0x000003FFUL
#define VDE_DEBUG_PIXEL_OFFSET_Y_OFFSET_ALIGN           0
#define VDE_DEBUG_PIXEL_OFFSET_Y_OFFSET_BITS            10
#define VDE_DEBUG_PIXEL_OFFSET_Y_OFFSET_SHIFT           0

/**********************************************************************
 * VDE_DEBUG_READ_BACK
 **********************************************************************/
/* reserved0 [31:24] -  */
#define VDE_DEBUG_READ_BACK_reserved0_MASK              0xFF000000UL
#define VDE_DEBUG_READ_BACK_reserved0_ALIGN             0
#define VDE_DEBUG_READ_BACK_reserved0_BITS              8
#define VDE_DEBUG_READ_BACK_reserved0_SHIFT             24

/* COLOR [23:00] - unsigned */
#define VDE_DEBUG_READ_BACK_COLOR_MASK                  0x00FFFFFFUL
#define VDE_DEBUG_READ_BACK_COLOR_ALIGN                 0
#define VDE_DEBUG_READ_BACK_COLOR_BITS                  24
#define VDE_DEBUG_READ_BACK_COLOR_SHIFT                 0

/**********************************************************************
 * VDE_VDE_FIFO_STATUS
 **********************************************************************/
/* reserved0 [31:09] -  */
#define VDE_VDE_FIFO_STATUS_reserved0_MASK              0xFFFFFE00UL
#define VDE_VDE_FIFO_STATUS_reserved0_ALIGN             0
#define VDE_VDE_FIFO_STATUS_reserved0_BITS              23
#define VDE_VDE_FIFO_STATUS_reserved0_SHIFT             9

/* TITAN_MODE_SWITCH_FLAG [08:08] - boolean */
#define VDE_VDE_FIFO_STATUS_TITAN_MODE_SWITCH_FLAG_MASK 0x00000100UL
#define VDE_VDE_FIFO_STATUS_TITAN_MODE_SWITCH_FLAG_ALIGN 0
#define VDE_VDE_FIFO_STATUS_TITAN_MODE_SWITCH_FLAG_BITS 1
#define VDE_VDE_FIFO_STATUS_TITAN_MODE_SWITCH_FLAG_SHIFT 8

/* PLAYBACK_FIFO_UNDERFLOW [07:07] - boolean */
#define VDE_VDE_FIFO_STATUS_PLAYBACK_FIFO_UNDERFLOW_MASK 0x00000080UL
#define VDE_VDE_FIFO_STATUS_PLAYBACK_FIFO_UNDERFLOW_ALIGN 0
#define VDE_VDE_FIFO_STATUS_PLAYBACK_FIFO_UNDERFLOW_BITS 1
#define VDE_VDE_FIFO_STATUS_PLAYBACK_FIFO_UNDERFLOW_SHIFT 7

/* PLAYBACK_FIFO_OVERFLOW [06:06] - boolean */
#define VDE_VDE_FIFO_STATUS_PLAYBACK_FIFO_OVERFLOW_MASK 0x00000040UL
#define VDE_VDE_FIFO_STATUS_PLAYBACK_FIFO_OVERFLOW_ALIGN 0
#define VDE_VDE_FIFO_STATUS_PLAYBACK_FIFO_OVERFLOW_BITS 1
#define VDE_VDE_FIFO_STATUS_PLAYBACK_FIFO_OVERFLOW_SHIFT 6

/* CAPTURE_FIFO_UNDERFLOW [05:05] - boolean */
#define VDE_VDE_FIFO_STATUS_CAPTURE_FIFO_UNDERFLOW_MASK 0x00000020UL
#define VDE_VDE_FIFO_STATUS_CAPTURE_FIFO_UNDERFLOW_ALIGN 0
#define VDE_VDE_FIFO_STATUS_CAPTURE_FIFO_UNDERFLOW_BITS 1
#define VDE_VDE_FIFO_STATUS_CAPTURE_FIFO_UNDERFLOW_SHIFT 5

/* CAPTURE_FIFO_OVERFLOW [04:04] - boolean */
#define VDE_VDE_FIFO_STATUS_CAPTURE_FIFO_OVERFLOW_MASK  0x00000010UL
#define VDE_VDE_FIFO_STATUS_CAPTURE_FIFO_OVERFLOW_ALIGN 0
#define VDE_VDE_FIFO_STATUS_CAPTURE_FIFO_OVERFLOW_BITS  1
#define VDE_VDE_FIFO_STATUS_CAPTURE_FIFO_OVERFLOW_SHIFT 4

/* VWIN2_FIFO_UNDERFLOW [03:03] - boolean */
#define VDE_VDE_FIFO_STATUS_VWIN2_FIFO_UNDERFLOW_MASK   0x00000008UL
#define VDE_VDE_FIFO_STATUS_VWIN2_FIFO_UNDERFLOW_ALIGN  0
#define VDE_VDE_FIFO_STATUS_VWIN2_FIFO_UNDERFLOW_BITS   1
#define VDE_VDE_FIFO_STATUS_VWIN2_FIFO_UNDERFLOW_SHIFT  3

/* VWIN2_FIFO_OVERFLOW [02:02] - boolean */
#define VDE_VDE_FIFO_STATUS_VWIN2_FIFO_OVERFLOW_MASK    0x00000004UL
#define VDE_VDE_FIFO_STATUS_VWIN2_FIFO_OVERFLOW_ALIGN   0
#define VDE_VDE_FIFO_STATUS_VWIN2_FIFO_OVERFLOW_BITS    1
#define VDE_VDE_FIFO_STATUS_VWIN2_FIFO_OVERFLOW_SHIFT   2

/* VWIN1_FIFO_UNDERFLOW [01:01] - boolean */
#define VDE_VDE_FIFO_STATUS_VWIN1_FIFO_UNDERFLOW_MASK   0x00000002UL
#define VDE_VDE_FIFO_STATUS_VWIN1_FIFO_UNDERFLOW_ALIGN  0
#define VDE_VDE_FIFO_STATUS_VWIN1_FIFO_UNDERFLOW_BITS   1
#define VDE_VDE_FIFO_STATUS_VWIN1_FIFO_UNDERFLOW_SHIFT  1

/* VWIN1_FIFO_OVERFLOW [00:00] - boolean */
#define VDE_VDE_FIFO_STATUS_VWIN1_FIFO_OVERFLOW_MASK    0x00000001UL
#define VDE_VDE_FIFO_STATUS_VWIN1_FIFO_OVERFLOW_ALIGN   0
#define VDE_VDE_FIFO_STATUS_VWIN1_FIFO_OVERFLOW_BITS    1
#define VDE_VDE_FIFO_STATUS_VWIN1_FIFO_OVERFLOW_SHIFT   0

/**********************************************************************
 * VDE_VCOEFF_PHASE0_01
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_VCOEFF_PHASE0_01_reserved0_MASK             0xFC000000UL
#define VDE_VCOEFF_PHASE0_01_reserved0_ALIGN            0
#define VDE_VCOEFF_PHASE0_01_reserved0_BITS             6
#define VDE_VCOEFF_PHASE0_01_reserved0_SHIFT            26

/* COEFF_1 [25:16] - unsigned */
#define VDE_VCOEFF_PHASE0_01_COEFF_1_MASK               0x03FF0000UL
#define VDE_VCOEFF_PHASE0_01_COEFF_1_ALIGN              0
#define VDE_VCOEFF_PHASE0_01_COEFF_1_BITS               10
#define VDE_VCOEFF_PHASE0_01_COEFF_1_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_VCOEFF_PHASE0_01_reserved1_MASK             0x0000FC00UL
#define VDE_VCOEFF_PHASE0_01_reserved1_ALIGN            0
#define VDE_VCOEFF_PHASE0_01_reserved1_BITS             6
#define VDE_VCOEFF_PHASE0_01_reserved1_SHIFT            10

/* COEFF_0 [09:00] - unsigned */
#define VDE_VCOEFF_PHASE0_01_COEFF_0_MASK               0x000003FFUL
#define VDE_VCOEFF_PHASE0_01_COEFF_0_ALIGN              0
#define VDE_VCOEFF_PHASE0_01_COEFF_0_BITS               10
#define VDE_VCOEFF_PHASE0_01_COEFF_0_SHIFT              0

/**********************************************************************
 * VDE_VCOEFF_PHASE0_23
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_VCOEFF_PHASE0_23_reserved0_MASK             0xFC000000UL
#define VDE_VCOEFF_PHASE0_23_reserved0_ALIGN            0
#define VDE_VCOEFF_PHASE0_23_reserved0_BITS             6
#define VDE_VCOEFF_PHASE0_23_reserved0_SHIFT            26

/* COEFF_3 [25:16] - unsigned */
#define VDE_VCOEFF_PHASE0_23_COEFF_3_MASK               0x03FF0000UL
#define VDE_VCOEFF_PHASE0_23_COEFF_3_ALIGN              0
#define VDE_VCOEFF_PHASE0_23_COEFF_3_BITS               10
#define VDE_VCOEFF_PHASE0_23_COEFF_3_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_VCOEFF_PHASE0_23_reserved1_MASK             0x0000FC00UL
#define VDE_VCOEFF_PHASE0_23_reserved1_ALIGN            0
#define VDE_VCOEFF_PHASE0_23_reserved1_BITS             6
#define VDE_VCOEFF_PHASE0_23_reserved1_SHIFT            10

/* COEFF_2 [09:00] - unsigned */
#define VDE_VCOEFF_PHASE0_23_COEFF_2_MASK               0x000003FFUL
#define VDE_VCOEFF_PHASE0_23_COEFF_2_ALIGN              0
#define VDE_VCOEFF_PHASE0_23_COEFF_2_BITS               10
#define VDE_VCOEFF_PHASE0_23_COEFF_2_SHIFT              0

/**********************************************************************
 * VDE_VCOEFF_PHASE1_01
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_VCOEFF_PHASE1_01_reserved0_MASK             0xFC000000UL
#define VDE_VCOEFF_PHASE1_01_reserved0_ALIGN            0
#define VDE_VCOEFF_PHASE1_01_reserved0_BITS             6
#define VDE_VCOEFF_PHASE1_01_reserved0_SHIFT            26

/* COEFF_1 [25:16] - unsigned */
#define VDE_VCOEFF_PHASE1_01_COEFF_1_MASK               0x03FF0000UL
#define VDE_VCOEFF_PHASE1_01_COEFF_1_ALIGN              0
#define VDE_VCOEFF_PHASE1_01_COEFF_1_BITS               10
#define VDE_VCOEFF_PHASE1_01_COEFF_1_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_VCOEFF_PHASE1_01_reserved1_MASK             0x0000FC00UL
#define VDE_VCOEFF_PHASE1_01_reserved1_ALIGN            0
#define VDE_VCOEFF_PHASE1_01_reserved1_BITS             6
#define VDE_VCOEFF_PHASE1_01_reserved1_SHIFT            10

/* COEFF_0 [09:00] - unsigned */
#define VDE_VCOEFF_PHASE1_01_COEFF_0_MASK               0x000003FFUL
#define VDE_VCOEFF_PHASE1_01_COEFF_0_ALIGN              0
#define VDE_VCOEFF_PHASE1_01_COEFF_0_BITS               10
#define VDE_VCOEFF_PHASE1_01_COEFF_0_SHIFT              0

/**********************************************************************
 * VDE_VCOEFF_PHASE1_23
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_VCOEFF_PHASE1_23_reserved0_MASK             0xFC000000UL
#define VDE_VCOEFF_PHASE1_23_reserved0_ALIGN            0
#define VDE_VCOEFF_PHASE1_23_reserved0_BITS             6
#define VDE_VCOEFF_PHASE1_23_reserved0_SHIFT            26

/* COEFF_3 [25:16] - unsigned */
#define VDE_VCOEFF_PHASE1_23_COEFF_3_MASK               0x03FF0000UL
#define VDE_VCOEFF_PHASE1_23_COEFF_3_ALIGN              0
#define VDE_VCOEFF_PHASE1_23_COEFF_3_BITS               10
#define VDE_VCOEFF_PHASE1_23_COEFF_3_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_VCOEFF_PHASE1_23_reserved1_MASK             0x0000FC00UL
#define VDE_VCOEFF_PHASE1_23_reserved1_ALIGN            0
#define VDE_VCOEFF_PHASE1_23_reserved1_BITS             6
#define VDE_VCOEFF_PHASE1_23_reserved1_SHIFT            10

/* COEFF_2 [09:00] - unsigned */
#define VDE_VCOEFF_PHASE1_23_COEFF_2_MASK               0x000003FFUL
#define VDE_VCOEFF_PHASE1_23_COEFF_2_ALIGN              0
#define VDE_VCOEFF_PHASE1_23_COEFF_2_BITS               10
#define VDE_VCOEFF_PHASE1_23_COEFF_2_SHIFT              0

/**********************************************************************
 * VDE_VCOEFF_PHASE2_01
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_VCOEFF_PHASE2_01_reserved0_MASK             0xFC000000UL
#define VDE_VCOEFF_PHASE2_01_reserved0_ALIGN            0
#define VDE_VCOEFF_PHASE2_01_reserved0_BITS             6
#define VDE_VCOEFF_PHASE2_01_reserved0_SHIFT            26

/* COEFF_1 [25:16] - unsigned */
#define VDE_VCOEFF_PHASE2_01_COEFF_1_MASK               0x03FF0000UL
#define VDE_VCOEFF_PHASE2_01_COEFF_1_ALIGN              0
#define VDE_VCOEFF_PHASE2_01_COEFF_1_BITS               10
#define VDE_VCOEFF_PHASE2_01_COEFF_1_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_VCOEFF_PHASE2_01_reserved1_MASK             0x0000FC00UL
#define VDE_VCOEFF_PHASE2_01_reserved1_ALIGN            0
#define VDE_VCOEFF_PHASE2_01_reserved1_BITS             6
#define VDE_VCOEFF_PHASE2_01_reserved1_SHIFT            10

/* COEFF_0 [09:00] - unsigned */
#define VDE_VCOEFF_PHASE2_01_COEFF_0_MASK               0x000003FFUL
#define VDE_VCOEFF_PHASE2_01_COEFF_0_ALIGN              0
#define VDE_VCOEFF_PHASE2_01_COEFF_0_BITS               10
#define VDE_VCOEFF_PHASE2_01_COEFF_0_SHIFT              0

/**********************************************************************
 * VDE_VCOEFF_PHASE2_23
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_VCOEFF_PHASE2_23_reserved0_MASK             0xFC000000UL
#define VDE_VCOEFF_PHASE2_23_reserved0_ALIGN            0
#define VDE_VCOEFF_PHASE2_23_reserved0_BITS             6
#define VDE_VCOEFF_PHASE2_23_reserved0_SHIFT            26

/* COEFF_3 [25:16] - unsigned */
#define VDE_VCOEFF_PHASE2_23_COEFF_3_MASK               0x03FF0000UL
#define VDE_VCOEFF_PHASE2_23_COEFF_3_ALIGN              0
#define VDE_VCOEFF_PHASE2_23_COEFF_3_BITS               10
#define VDE_VCOEFF_PHASE2_23_COEFF_3_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_VCOEFF_PHASE2_23_reserved1_MASK             0x0000FC00UL
#define VDE_VCOEFF_PHASE2_23_reserved1_ALIGN            0
#define VDE_VCOEFF_PHASE2_23_reserved1_BITS             6
#define VDE_VCOEFF_PHASE2_23_reserved1_SHIFT            10

/* COEFF_2 [09:00] - unsigned */
#define VDE_VCOEFF_PHASE2_23_COEFF_2_MASK               0x000003FFUL
#define VDE_VCOEFF_PHASE2_23_COEFF_2_ALIGN              0
#define VDE_VCOEFF_PHASE2_23_COEFF_2_BITS               10
#define VDE_VCOEFF_PHASE2_23_COEFF_2_SHIFT              0

/**********************************************************************
 * VDE_VCOEFF_PHASE3_01
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_VCOEFF_PHASE3_01_reserved0_MASK             0xFC000000UL
#define VDE_VCOEFF_PHASE3_01_reserved0_ALIGN            0
#define VDE_VCOEFF_PHASE3_01_reserved0_BITS             6
#define VDE_VCOEFF_PHASE3_01_reserved0_SHIFT            26

/* COEFF_1 [25:16] - unsigned */
#define VDE_VCOEFF_PHASE3_01_COEFF_1_MASK               0x03FF0000UL
#define VDE_VCOEFF_PHASE3_01_COEFF_1_ALIGN              0
#define VDE_VCOEFF_PHASE3_01_COEFF_1_BITS               10
#define VDE_VCOEFF_PHASE3_01_COEFF_1_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_VCOEFF_PHASE3_01_reserved1_MASK             0x0000FC00UL
#define VDE_VCOEFF_PHASE3_01_reserved1_ALIGN            0
#define VDE_VCOEFF_PHASE3_01_reserved1_BITS             6
#define VDE_VCOEFF_PHASE3_01_reserved1_SHIFT            10

/* COEFF_0 [09:00] - unsigned */
#define VDE_VCOEFF_PHASE3_01_COEFF_0_MASK               0x000003FFUL
#define VDE_VCOEFF_PHASE3_01_COEFF_0_ALIGN              0
#define VDE_VCOEFF_PHASE3_01_COEFF_0_BITS               10
#define VDE_VCOEFF_PHASE3_01_COEFF_0_SHIFT              0

/**********************************************************************
 * VDE_VCOEFF_PHASE3_23
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_VCOEFF_PHASE3_23_reserved0_MASK             0xFC000000UL
#define VDE_VCOEFF_PHASE3_23_reserved0_ALIGN            0
#define VDE_VCOEFF_PHASE3_23_reserved0_BITS             6
#define VDE_VCOEFF_PHASE3_23_reserved0_SHIFT            26

/* COEFF_3 [25:16] - unsigned */
#define VDE_VCOEFF_PHASE3_23_COEFF_3_MASK               0x03FF0000UL
#define VDE_VCOEFF_PHASE3_23_COEFF_3_ALIGN              0
#define VDE_VCOEFF_PHASE3_23_COEFF_3_BITS               10
#define VDE_VCOEFF_PHASE3_23_COEFF_3_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_VCOEFF_PHASE3_23_reserved1_MASK             0x0000FC00UL
#define VDE_VCOEFF_PHASE3_23_reserved1_ALIGN            0
#define VDE_VCOEFF_PHASE3_23_reserved1_BITS             6
#define VDE_VCOEFF_PHASE3_23_reserved1_SHIFT            10

/* COEFF_2 [09:00] - unsigned */
#define VDE_VCOEFF_PHASE3_23_COEFF_2_MASK               0x000003FFUL
#define VDE_VCOEFF_PHASE3_23_COEFF_2_ALIGN              0
#define VDE_VCOEFF_PHASE3_23_COEFF_2_BITS               10
#define VDE_VCOEFF_PHASE3_23_COEFF_2_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE0_01
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE0_01_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE0_01_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE0_01_reserved0_BITS             6
#define VDE_HCOEFF_PHASE0_01_reserved0_SHIFT            26

/* COEFF_1 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE0_01_COEFF_1_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE0_01_COEFF_1_ALIGN              0
#define VDE_HCOEFF_PHASE0_01_COEFF_1_BITS               10
#define VDE_HCOEFF_PHASE0_01_COEFF_1_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE0_01_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE0_01_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE0_01_reserved1_BITS             6
#define VDE_HCOEFF_PHASE0_01_reserved1_SHIFT            10

/* COEFF_0 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE0_01_COEFF_0_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE0_01_COEFF_0_ALIGN              0
#define VDE_HCOEFF_PHASE0_01_COEFF_0_BITS               10
#define VDE_HCOEFF_PHASE0_01_COEFF_0_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE0_23
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE0_23_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE0_23_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE0_23_reserved0_BITS             6
#define VDE_HCOEFF_PHASE0_23_reserved0_SHIFT            26

/* COEFF_3 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE0_23_COEFF_3_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE0_23_COEFF_3_ALIGN              0
#define VDE_HCOEFF_PHASE0_23_COEFF_3_BITS               10
#define VDE_HCOEFF_PHASE0_23_COEFF_3_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE0_23_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE0_23_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE0_23_reserved1_BITS             6
#define VDE_HCOEFF_PHASE0_23_reserved1_SHIFT            10

/* COEFF_2 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE0_23_COEFF_2_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE0_23_COEFF_2_ALIGN              0
#define VDE_HCOEFF_PHASE0_23_COEFF_2_BITS               10
#define VDE_HCOEFF_PHASE0_23_COEFF_2_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE0_45
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE0_45_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE0_45_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE0_45_reserved0_BITS             6
#define VDE_HCOEFF_PHASE0_45_reserved0_SHIFT            26

/* COEFF_5 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE0_45_COEFF_5_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE0_45_COEFF_5_ALIGN              0
#define VDE_HCOEFF_PHASE0_45_COEFF_5_BITS               10
#define VDE_HCOEFF_PHASE0_45_COEFF_5_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE0_45_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE0_45_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE0_45_reserved1_BITS             6
#define VDE_HCOEFF_PHASE0_45_reserved1_SHIFT            10

/* COEFF_4 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE0_45_COEFF_4_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE0_45_COEFF_4_ALIGN              0
#define VDE_HCOEFF_PHASE0_45_COEFF_4_BITS               10
#define VDE_HCOEFF_PHASE0_45_COEFF_4_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE0_67
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE0_67_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE0_67_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE0_67_reserved0_BITS             6
#define VDE_HCOEFF_PHASE0_67_reserved0_SHIFT            26

/* COEFF_7 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE0_67_COEFF_7_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE0_67_COEFF_7_ALIGN              0
#define VDE_HCOEFF_PHASE0_67_COEFF_7_BITS               10
#define VDE_HCOEFF_PHASE0_67_COEFF_7_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE0_67_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE0_67_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE0_67_reserved1_BITS             6
#define VDE_HCOEFF_PHASE0_67_reserved1_SHIFT            10

/* COEFF_6 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE0_67_COEFF_6_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE0_67_COEFF_6_ALIGN              0
#define VDE_HCOEFF_PHASE0_67_COEFF_6_BITS               10
#define VDE_HCOEFF_PHASE0_67_COEFF_6_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE1_01
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE1_01_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE1_01_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE1_01_reserved0_BITS             6
#define VDE_HCOEFF_PHASE1_01_reserved0_SHIFT            26

/* COEFF_1 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE1_01_COEFF_1_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE1_01_COEFF_1_ALIGN              0
#define VDE_HCOEFF_PHASE1_01_COEFF_1_BITS               10
#define VDE_HCOEFF_PHASE1_01_COEFF_1_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE1_01_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE1_01_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE1_01_reserved1_BITS             6
#define VDE_HCOEFF_PHASE1_01_reserved1_SHIFT            10

/* COEFF_0 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE1_01_COEFF_0_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE1_01_COEFF_0_ALIGN              0
#define VDE_HCOEFF_PHASE1_01_COEFF_0_BITS               10
#define VDE_HCOEFF_PHASE1_01_COEFF_0_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE1_23
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE1_23_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE1_23_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE1_23_reserved0_BITS             6
#define VDE_HCOEFF_PHASE1_23_reserved0_SHIFT            26

/* COEFF_3 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE1_23_COEFF_3_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE1_23_COEFF_3_ALIGN              0
#define VDE_HCOEFF_PHASE1_23_COEFF_3_BITS               10
#define VDE_HCOEFF_PHASE1_23_COEFF_3_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE1_23_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE1_23_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE1_23_reserved1_BITS             6
#define VDE_HCOEFF_PHASE1_23_reserved1_SHIFT            10

/* COEFF_2 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE1_23_COEFF_2_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE1_23_COEFF_2_ALIGN              0
#define VDE_HCOEFF_PHASE1_23_COEFF_2_BITS               10
#define VDE_HCOEFF_PHASE1_23_COEFF_2_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE1_45
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE1_45_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE1_45_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE1_45_reserved0_BITS             6
#define VDE_HCOEFF_PHASE1_45_reserved0_SHIFT            26

/* COEFF_5 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE1_45_COEFF_5_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE1_45_COEFF_5_ALIGN              0
#define VDE_HCOEFF_PHASE1_45_COEFF_5_BITS               10
#define VDE_HCOEFF_PHASE1_45_COEFF_5_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE1_45_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE1_45_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE1_45_reserved1_BITS             6
#define VDE_HCOEFF_PHASE1_45_reserved1_SHIFT            10

/* COEFF_4 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE1_45_COEFF_4_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE1_45_COEFF_4_ALIGN              0
#define VDE_HCOEFF_PHASE1_45_COEFF_4_BITS               10
#define VDE_HCOEFF_PHASE1_45_COEFF_4_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE1_67
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE1_67_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE1_67_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE1_67_reserved0_BITS             6
#define VDE_HCOEFF_PHASE1_67_reserved0_SHIFT            26

/* COEFF_7 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE1_67_COEFF_7_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE1_67_COEFF_7_ALIGN              0
#define VDE_HCOEFF_PHASE1_67_COEFF_7_BITS               10
#define VDE_HCOEFF_PHASE1_67_COEFF_7_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE1_67_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE1_67_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE1_67_reserved1_BITS             6
#define VDE_HCOEFF_PHASE1_67_reserved1_SHIFT            10

/* COEFF_6 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE1_67_COEFF_6_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE1_67_COEFF_6_ALIGN              0
#define VDE_HCOEFF_PHASE1_67_COEFF_6_BITS               10
#define VDE_HCOEFF_PHASE1_67_COEFF_6_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE2_01
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE2_01_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE2_01_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE2_01_reserved0_BITS             6
#define VDE_HCOEFF_PHASE2_01_reserved0_SHIFT            26

/* COEFF_1 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE2_01_COEFF_1_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE2_01_COEFF_1_ALIGN              0
#define VDE_HCOEFF_PHASE2_01_COEFF_1_BITS               10
#define VDE_HCOEFF_PHASE2_01_COEFF_1_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE2_01_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE2_01_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE2_01_reserved1_BITS             6
#define VDE_HCOEFF_PHASE2_01_reserved1_SHIFT            10

/* COEFF_0 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE2_01_COEFF_0_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE2_01_COEFF_0_ALIGN              0
#define VDE_HCOEFF_PHASE2_01_COEFF_0_BITS               10
#define VDE_HCOEFF_PHASE2_01_COEFF_0_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE2_23
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE2_23_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE2_23_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE2_23_reserved0_BITS             6
#define VDE_HCOEFF_PHASE2_23_reserved0_SHIFT            26

/* COEFF_3 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE2_23_COEFF_3_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE2_23_COEFF_3_ALIGN              0
#define VDE_HCOEFF_PHASE2_23_COEFF_3_BITS               10
#define VDE_HCOEFF_PHASE2_23_COEFF_3_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE2_23_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE2_23_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE2_23_reserved1_BITS             6
#define VDE_HCOEFF_PHASE2_23_reserved1_SHIFT            10

/* COEFF_2 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE2_23_COEFF_2_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE2_23_COEFF_2_ALIGN              0
#define VDE_HCOEFF_PHASE2_23_COEFF_2_BITS               10
#define VDE_HCOEFF_PHASE2_23_COEFF_2_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE2_45
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE2_45_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE2_45_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE2_45_reserved0_BITS             6
#define VDE_HCOEFF_PHASE2_45_reserved0_SHIFT            26

/* COEFF_5 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE2_45_COEFF_5_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE2_45_COEFF_5_ALIGN              0
#define VDE_HCOEFF_PHASE2_45_COEFF_5_BITS               10
#define VDE_HCOEFF_PHASE2_45_COEFF_5_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE2_45_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE2_45_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE2_45_reserved1_BITS             6
#define VDE_HCOEFF_PHASE2_45_reserved1_SHIFT            10

/* COEFF_4 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE2_45_COEFF_4_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE2_45_COEFF_4_ALIGN              0
#define VDE_HCOEFF_PHASE2_45_COEFF_4_BITS               10
#define VDE_HCOEFF_PHASE2_45_COEFF_4_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE2_67
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE2_67_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE2_67_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE2_67_reserved0_BITS             6
#define VDE_HCOEFF_PHASE2_67_reserved0_SHIFT            26

/* COEFF_7 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE2_67_COEFF_7_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE2_67_COEFF_7_ALIGN              0
#define VDE_HCOEFF_PHASE2_67_COEFF_7_BITS               10
#define VDE_HCOEFF_PHASE2_67_COEFF_7_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE2_67_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE2_67_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE2_67_reserved1_BITS             6
#define VDE_HCOEFF_PHASE2_67_reserved1_SHIFT            10

/* COEFF_6 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE2_67_COEFF_6_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE2_67_COEFF_6_ALIGN              0
#define VDE_HCOEFF_PHASE2_67_COEFF_6_BITS               10
#define VDE_HCOEFF_PHASE2_67_COEFF_6_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE3_01
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE3_01_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE3_01_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE3_01_reserved0_BITS             6
#define VDE_HCOEFF_PHASE3_01_reserved0_SHIFT            26

/* COEFF_1 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE3_01_COEFF_1_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE3_01_COEFF_1_ALIGN              0
#define VDE_HCOEFF_PHASE3_01_COEFF_1_BITS               10
#define VDE_HCOEFF_PHASE3_01_COEFF_1_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE3_01_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE3_01_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE3_01_reserved1_BITS             6
#define VDE_HCOEFF_PHASE3_01_reserved1_SHIFT            10

/* COEFF_0 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE3_01_COEFF_0_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE3_01_COEFF_0_ALIGN              0
#define VDE_HCOEFF_PHASE3_01_COEFF_0_BITS               10
#define VDE_HCOEFF_PHASE3_01_COEFF_0_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE3_23
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE3_23_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE3_23_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE3_23_reserved0_BITS             6
#define VDE_HCOEFF_PHASE3_23_reserved0_SHIFT            26

/* COEFF_3 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE3_23_COEFF_3_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE3_23_COEFF_3_ALIGN              0
#define VDE_HCOEFF_PHASE3_23_COEFF_3_BITS               10
#define VDE_HCOEFF_PHASE3_23_COEFF_3_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE3_23_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE3_23_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE3_23_reserved1_BITS             6
#define VDE_HCOEFF_PHASE3_23_reserved1_SHIFT            10

/* COEFF_2 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE3_23_COEFF_2_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE3_23_COEFF_2_ALIGN              0
#define VDE_HCOEFF_PHASE3_23_COEFF_2_BITS               10
#define VDE_HCOEFF_PHASE3_23_COEFF_2_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE3_45
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE3_45_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE3_45_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE3_45_reserved0_BITS             6
#define VDE_HCOEFF_PHASE3_45_reserved0_SHIFT            26

/* COEFF_5 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE3_45_COEFF_5_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE3_45_COEFF_5_ALIGN              0
#define VDE_HCOEFF_PHASE3_45_COEFF_5_BITS               10
#define VDE_HCOEFF_PHASE3_45_COEFF_5_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE3_45_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE3_45_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE3_45_reserved1_BITS             6
#define VDE_HCOEFF_PHASE3_45_reserved1_SHIFT            10

/* COEFF_4 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE3_45_COEFF_4_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE3_45_COEFF_4_ALIGN              0
#define VDE_HCOEFF_PHASE3_45_COEFF_4_BITS               10
#define VDE_HCOEFF_PHASE3_45_COEFF_4_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE3_67
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE3_67_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE3_67_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE3_67_reserved0_BITS             6
#define VDE_HCOEFF_PHASE3_67_reserved0_SHIFT            26

/* COEFF_7 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE3_67_COEFF_7_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE3_67_COEFF_7_ALIGN              0
#define VDE_HCOEFF_PHASE3_67_COEFF_7_BITS               10
#define VDE_HCOEFF_PHASE3_67_COEFF_7_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE3_67_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE3_67_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE3_67_reserved1_BITS             6
#define VDE_HCOEFF_PHASE3_67_reserved1_SHIFT            10

/* COEFF_6 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE3_67_COEFF_6_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE3_67_COEFF_6_ALIGN              0
#define VDE_HCOEFF_PHASE3_67_COEFF_6_BITS               10
#define VDE_HCOEFF_PHASE3_67_COEFF_6_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE4_01
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE4_01_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE4_01_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE4_01_reserved0_BITS             6
#define VDE_HCOEFF_PHASE4_01_reserved0_SHIFT            26

/* COEFF_1 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE4_01_COEFF_1_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE4_01_COEFF_1_ALIGN              0
#define VDE_HCOEFF_PHASE4_01_COEFF_1_BITS               10
#define VDE_HCOEFF_PHASE4_01_COEFF_1_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE4_01_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE4_01_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE4_01_reserved1_BITS             6
#define VDE_HCOEFF_PHASE4_01_reserved1_SHIFT            10

/* COEFF_0 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE4_01_COEFF_0_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE4_01_COEFF_0_ALIGN              0
#define VDE_HCOEFF_PHASE4_01_COEFF_0_BITS               10
#define VDE_HCOEFF_PHASE4_01_COEFF_0_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE4_23
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE4_23_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE4_23_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE4_23_reserved0_BITS             6
#define VDE_HCOEFF_PHASE4_23_reserved0_SHIFT            26

/* COEFF_3 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE4_23_COEFF_3_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE4_23_COEFF_3_ALIGN              0
#define VDE_HCOEFF_PHASE4_23_COEFF_3_BITS               10
#define VDE_HCOEFF_PHASE4_23_COEFF_3_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE4_23_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE4_23_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE4_23_reserved1_BITS             6
#define VDE_HCOEFF_PHASE4_23_reserved1_SHIFT            10

/* COEFF_2 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE4_23_COEFF_2_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE4_23_COEFF_2_ALIGN              0
#define VDE_HCOEFF_PHASE4_23_COEFF_2_BITS               10
#define VDE_HCOEFF_PHASE4_23_COEFF_2_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE4_45
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE4_45_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE4_45_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE4_45_reserved0_BITS             6
#define VDE_HCOEFF_PHASE4_45_reserved0_SHIFT            26

/* COEFF_5 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE4_45_COEFF_5_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE4_45_COEFF_5_ALIGN              0
#define VDE_HCOEFF_PHASE4_45_COEFF_5_BITS               10
#define VDE_HCOEFF_PHASE4_45_COEFF_5_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE4_45_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE4_45_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE4_45_reserved1_BITS             6
#define VDE_HCOEFF_PHASE4_45_reserved1_SHIFT            10

/* COEFF_4 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE4_45_COEFF_4_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE4_45_COEFF_4_ALIGN              0
#define VDE_HCOEFF_PHASE4_45_COEFF_4_BITS               10
#define VDE_HCOEFF_PHASE4_45_COEFF_4_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE4_67
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE4_67_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE4_67_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE4_67_reserved0_BITS             6
#define VDE_HCOEFF_PHASE4_67_reserved0_SHIFT            26

/* COEFF_7 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE4_67_COEFF_7_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE4_67_COEFF_7_ALIGN              0
#define VDE_HCOEFF_PHASE4_67_COEFF_7_BITS               10
#define VDE_HCOEFF_PHASE4_67_COEFF_7_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE4_67_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE4_67_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE4_67_reserved1_BITS             6
#define VDE_HCOEFF_PHASE4_67_reserved1_SHIFT            10

/* COEFF_6 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE4_67_COEFF_6_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE4_67_COEFF_6_ALIGN              0
#define VDE_HCOEFF_PHASE4_67_COEFF_6_BITS               10
#define VDE_HCOEFF_PHASE4_67_COEFF_6_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE5_01
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE5_01_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE5_01_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE5_01_reserved0_BITS             6
#define VDE_HCOEFF_PHASE5_01_reserved0_SHIFT            26

/* COEFF_1 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE5_01_COEFF_1_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE5_01_COEFF_1_ALIGN              0
#define VDE_HCOEFF_PHASE5_01_COEFF_1_BITS               10
#define VDE_HCOEFF_PHASE5_01_COEFF_1_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE5_01_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE5_01_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE5_01_reserved1_BITS             6
#define VDE_HCOEFF_PHASE5_01_reserved1_SHIFT            10

/* COEFF_0 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE5_01_COEFF_0_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE5_01_COEFF_0_ALIGN              0
#define VDE_HCOEFF_PHASE5_01_COEFF_0_BITS               10
#define VDE_HCOEFF_PHASE5_01_COEFF_0_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE5_23
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE5_23_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE5_23_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE5_23_reserved0_BITS             6
#define VDE_HCOEFF_PHASE5_23_reserved0_SHIFT            26

/* COEFF_3 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE5_23_COEFF_3_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE5_23_COEFF_3_ALIGN              0
#define VDE_HCOEFF_PHASE5_23_COEFF_3_BITS               10
#define VDE_HCOEFF_PHASE5_23_COEFF_3_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE5_23_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE5_23_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE5_23_reserved1_BITS             6
#define VDE_HCOEFF_PHASE5_23_reserved1_SHIFT            10

/* COEFF_2 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE5_23_COEFF_2_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE5_23_COEFF_2_ALIGN              0
#define VDE_HCOEFF_PHASE5_23_COEFF_2_BITS               10
#define VDE_HCOEFF_PHASE5_23_COEFF_2_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE5_45
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE5_45_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE5_45_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE5_45_reserved0_BITS             6
#define VDE_HCOEFF_PHASE5_45_reserved0_SHIFT            26

/* COEFF_5 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE5_45_COEFF_5_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE5_45_COEFF_5_ALIGN              0
#define VDE_HCOEFF_PHASE5_45_COEFF_5_BITS               10
#define VDE_HCOEFF_PHASE5_45_COEFF_5_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE5_45_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE5_45_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE5_45_reserved1_BITS             6
#define VDE_HCOEFF_PHASE5_45_reserved1_SHIFT            10

/* COEFF_4 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE5_45_COEFF_4_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE5_45_COEFF_4_ALIGN              0
#define VDE_HCOEFF_PHASE5_45_COEFF_4_BITS               10
#define VDE_HCOEFF_PHASE5_45_COEFF_4_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE5_67
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE5_67_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE5_67_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE5_67_reserved0_BITS             6
#define VDE_HCOEFF_PHASE5_67_reserved0_SHIFT            26

/* COEFF_7 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE5_67_COEFF_7_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE5_67_COEFF_7_ALIGN              0
#define VDE_HCOEFF_PHASE5_67_COEFF_7_BITS               10
#define VDE_HCOEFF_PHASE5_67_COEFF_7_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE5_67_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE5_67_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE5_67_reserved1_BITS             6
#define VDE_HCOEFF_PHASE5_67_reserved1_SHIFT            10

/* COEFF_6 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE5_67_COEFF_6_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE5_67_COEFF_6_ALIGN              0
#define VDE_HCOEFF_PHASE5_67_COEFF_6_BITS               10
#define VDE_HCOEFF_PHASE5_67_COEFF_6_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE6_01
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE6_01_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE6_01_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE6_01_reserved0_BITS             6
#define VDE_HCOEFF_PHASE6_01_reserved0_SHIFT            26

/* COEFF_1 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE6_01_COEFF_1_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE6_01_COEFF_1_ALIGN              0
#define VDE_HCOEFF_PHASE6_01_COEFF_1_BITS               10
#define VDE_HCOEFF_PHASE6_01_COEFF_1_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE6_01_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE6_01_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE6_01_reserved1_BITS             6
#define VDE_HCOEFF_PHASE6_01_reserved1_SHIFT            10

/* COEFF_0 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE6_01_COEFF_0_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE6_01_COEFF_0_ALIGN              0
#define VDE_HCOEFF_PHASE6_01_COEFF_0_BITS               10
#define VDE_HCOEFF_PHASE6_01_COEFF_0_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE6_23
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE6_23_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE6_23_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE6_23_reserved0_BITS             6
#define VDE_HCOEFF_PHASE6_23_reserved0_SHIFT            26

/* COEFF_3 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE6_23_COEFF_3_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE6_23_COEFF_3_ALIGN              0
#define VDE_HCOEFF_PHASE6_23_COEFF_3_BITS               10
#define VDE_HCOEFF_PHASE6_23_COEFF_3_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE6_23_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE6_23_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE6_23_reserved1_BITS             6
#define VDE_HCOEFF_PHASE6_23_reserved1_SHIFT            10

/* COEFF_2 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE6_23_COEFF_2_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE6_23_COEFF_2_ALIGN              0
#define VDE_HCOEFF_PHASE6_23_COEFF_2_BITS               10
#define VDE_HCOEFF_PHASE6_23_COEFF_2_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE6_45
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE6_45_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE6_45_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE6_45_reserved0_BITS             6
#define VDE_HCOEFF_PHASE6_45_reserved0_SHIFT            26

/* COEFF_5 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE6_45_COEFF_5_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE6_45_COEFF_5_ALIGN              0
#define VDE_HCOEFF_PHASE6_45_COEFF_5_BITS               10
#define VDE_HCOEFF_PHASE6_45_COEFF_5_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE6_45_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE6_45_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE6_45_reserved1_BITS             6
#define VDE_HCOEFF_PHASE6_45_reserved1_SHIFT            10

/* COEFF_4 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE6_45_COEFF_4_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE6_45_COEFF_4_ALIGN              0
#define VDE_HCOEFF_PHASE6_45_COEFF_4_BITS               10
#define VDE_HCOEFF_PHASE6_45_COEFF_4_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE6_67
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE6_67_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE6_67_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE6_67_reserved0_BITS             6
#define VDE_HCOEFF_PHASE6_67_reserved0_SHIFT            26

/* COEFF_7 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE6_67_COEFF_7_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE6_67_COEFF_7_ALIGN              0
#define VDE_HCOEFF_PHASE6_67_COEFF_7_BITS               10
#define VDE_HCOEFF_PHASE6_67_COEFF_7_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE6_67_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE6_67_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE6_67_reserved1_BITS             6
#define VDE_HCOEFF_PHASE6_67_reserved1_SHIFT            10

/* COEFF_6 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE6_67_COEFF_6_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE6_67_COEFF_6_ALIGN              0
#define VDE_HCOEFF_PHASE6_67_COEFF_6_BITS               10
#define VDE_HCOEFF_PHASE6_67_COEFF_6_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE7_01
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE7_01_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE7_01_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE7_01_reserved0_BITS             6
#define VDE_HCOEFF_PHASE7_01_reserved0_SHIFT            26

/* COEFF_1 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE7_01_COEFF_1_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE7_01_COEFF_1_ALIGN              0
#define VDE_HCOEFF_PHASE7_01_COEFF_1_BITS               10
#define VDE_HCOEFF_PHASE7_01_COEFF_1_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE7_01_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE7_01_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE7_01_reserved1_BITS             6
#define VDE_HCOEFF_PHASE7_01_reserved1_SHIFT            10

/* COEFF_0 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE7_01_COEFF_0_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE7_01_COEFF_0_ALIGN              0
#define VDE_HCOEFF_PHASE7_01_COEFF_0_BITS               10
#define VDE_HCOEFF_PHASE7_01_COEFF_0_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE7_23
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE7_23_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE7_23_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE7_23_reserved0_BITS             6
#define VDE_HCOEFF_PHASE7_23_reserved0_SHIFT            26

/* COEFF_3 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE7_23_COEFF_3_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE7_23_COEFF_3_ALIGN              0
#define VDE_HCOEFF_PHASE7_23_COEFF_3_BITS               10
#define VDE_HCOEFF_PHASE7_23_COEFF_3_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE7_23_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE7_23_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE7_23_reserved1_BITS             6
#define VDE_HCOEFF_PHASE7_23_reserved1_SHIFT            10

/* COEFF_2 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE7_23_COEFF_2_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE7_23_COEFF_2_ALIGN              0
#define VDE_HCOEFF_PHASE7_23_COEFF_2_BITS               10
#define VDE_HCOEFF_PHASE7_23_COEFF_2_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE7_45
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE7_45_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE7_45_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE7_45_reserved0_BITS             6
#define VDE_HCOEFF_PHASE7_45_reserved0_SHIFT            26

/* COEFF_5 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE7_45_COEFF_5_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE7_45_COEFF_5_ALIGN              0
#define VDE_HCOEFF_PHASE7_45_COEFF_5_BITS               10
#define VDE_HCOEFF_PHASE7_45_COEFF_5_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE7_45_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE7_45_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE7_45_reserved1_BITS             6
#define VDE_HCOEFF_PHASE7_45_reserved1_SHIFT            10

/* COEFF_4 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE7_45_COEFF_4_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE7_45_COEFF_4_ALIGN              0
#define VDE_HCOEFF_PHASE7_45_COEFF_4_BITS               10
#define VDE_HCOEFF_PHASE7_45_COEFF_4_SHIFT              0

/**********************************************************************
 * VDE_HCOEFF_PHASE7_67
 **********************************************************************/
/* reserved0 [31:26] -  */
#define VDE_HCOEFF_PHASE7_67_reserved0_MASK             0xFC000000UL
#define VDE_HCOEFF_PHASE7_67_reserved0_ALIGN            0
#define VDE_HCOEFF_PHASE7_67_reserved0_BITS             6
#define VDE_HCOEFF_PHASE7_67_reserved0_SHIFT            26

/* COEFF_7 [25:16] - unsigned */
#define VDE_HCOEFF_PHASE7_67_COEFF_7_MASK               0x03FF0000UL
#define VDE_HCOEFF_PHASE7_67_COEFF_7_ALIGN              0
#define VDE_HCOEFF_PHASE7_67_COEFF_7_BITS               10
#define VDE_HCOEFF_PHASE7_67_COEFF_7_SHIFT              16

/* reserved1 [15:10] -  */
#define VDE_HCOEFF_PHASE7_67_reserved1_MASK             0x0000FC00UL
#define VDE_HCOEFF_PHASE7_67_reserved1_ALIGN            0
#define VDE_HCOEFF_PHASE7_67_reserved1_BITS             6
#define VDE_HCOEFF_PHASE7_67_reserved1_SHIFT            10

/* COEFF_6 [09:00] - unsigned */
#define VDE_HCOEFF_PHASE7_67_COEFF_6_MASK               0x000003FFUL
#define VDE_HCOEFF_PHASE7_67_COEFF_6_ALIGN              0
#define VDE_HCOEFF_PHASE7_67_COEFF_6_BITS               10
#define VDE_HCOEFF_PHASE7_67_COEFF_6_SHIFT              0

/**********************************************************************
 * GDE_WD_WORD0
 **********************************************************************/
/* WINOP [31:30] - unsigned */
#define GDE_WD_WORD0_WINOP_MASK                         0xC0000000UL
#define GDE_WD_WORD0_WINOP_ALIGN                        0
#define GDE_WD_WORD0_WINOP_BITS                         2
#define GDE_WD_WORD0_WINOP_SHIFT                        30
#define GDE_WD_WORD0_WINOP_NORMAL_DISP                  0
#define GDE_WD_WORD0_WINOP_LOAD_CLUT                    1
#define GDE_WD_WORD0_WINOP_LAST_WD                      3

/* WINFMT [29:25] - unsigned */
#define GDE_WD_WORD0_WINFMT_MASK                        0x3E000000UL
#define GDE_WD_WORD0_WINFMT_ALIGN                       0
#define GDE_WD_WORD0_WINFMT_BITS                        5
#define GDE_WD_WORD0_WINFMT_SHIFT                       25
#define GDE_WD_WORD0_WINFMT_RGB16                       0
#define GDE_WD_WORD0_WINFMT_RGBA5551                    1
#define GDE_WD_WORD0_WINFMT_RGBA4444                    2
#define GDE_WD_WORD0_WINFMT_ARGB32                      3
#define GDE_WD_WORD0_WINFMT_CLUT2                       4
#define GDE_WD_WORD0_WINFMT_CLUT4                       5
#define GDE_WD_WORD0_WINFMT_CLUT8                       6
#define GDE_WD_WORD0_WINFMT_ACLUT16                     7
#define GDE_WD_WORD0_WINFMT_ALPHA0                      8
#define GDE_WD_WORD0_WINFMT_ALPHA2                      9
#define GDE_WD_WORD0_WINFMT_ALPHA4                      10
#define GDE_WD_WORD0_WINFMT_ALPHA8                      11
#define GDE_WD_WORD0_WINFMT_VYUY                        12
#define GDE_WD_WORD0_WINFMT_ARGB1555                    13
#define GDE_WD_WORD0_WINFMT_ALPHA1                      14
#define GDE_WD_WORD0_WINFMT_CURSOR                      15
#define GDE_WD_WORD0_WINFMT_TUNNEL                      16
#define GDE_WD_WORD0_WINFMT_ARGB17                      17
#define GDE_WD_WORD0_WINFMT_ARGB4444                    18
#define GDE_WD_WORD0_WINFMT_PMRGBA4444                  19
#define GDE_WD_WORD0_WINFMT_PMARGB32                    20
#define GDE_WD_WORD0_WINFMT_PMARGB4444                  21
#define GDE_WD_WORD0_WINFMT_YUY2                        22
#define GDE_WD_WORD0_WINFMT_PTARGB1555                  23

/* WMEM_ST [24:00] - unsigned */
#define GDE_WD_WORD0_WMEM_ST_MASK                       0x01FFFFFFUL
#define GDE_WD_WORD0_WMEM_ST_ALIGN                      0
#define GDE_WD_WORD0_WMEM_ST_BITS                       25
#define GDE_WD_WORD0_WMEM_ST_SHIFT                      0

/**********************************************************************
 * GDE_WD_WORD1
 **********************************************************************/
/* WLAY_2 [31:30] - unsigned */
#define GDE_WD_WORD1_WLAY_2_MASK                        0xC0000000UL
#define GDE_WD_WORD1_WLAY_2_ALIGN                       0
#define GDE_WD_WORD1_WLAY_2_BITS                        2
#define GDE_WD_WORD1_WLAY_2_SHIFT                       30

/* WLAY [29:28] - unsigned */
#define GDE_WD_WORD1_WLAY_MASK                          0x30000000UL
#define GDE_WD_WORD1_WLAY_ALIGN                         0
#define GDE_WD_WORD1_WLAY_BITS                          2
#define GDE_WD_WORD1_WLAY_SHIFT                         28

/* reserved0 [27:26] -  */
#define GDE_WD_WORD1_reserved0_MASK                     0x0C000000UL
#define GDE_WD_WORD1_reserved0_ALIGN                    0
#define GDE_WD_WORD1_reserved0_BITS                     2
#define GDE_WD_WORD1_reserved0_SHIFT                    26

/* WMEM_PIT [25:16] - unsigned */
#define GDE_WD_WORD1_WMEM_PIT_MASK                      0x03FF0000UL
#define GDE_WD_WORD1_WMEM_PIT_ALIGN                     0
#define GDE_WD_WORD1_WMEM_PIT_BITS                      10
#define GDE_WD_WORD1_WMEM_PIT_SHIFT                     16

/* SCIDX.reserved1 [15:06] -  */
#define GDE_WD_WORD1_SCIDX_reserved1_MASK               0x0000FFC0UL
#define GDE_WD_WORD1_SCIDX_reserved1_ALIGN              0
#define GDE_WD_WORD1_SCIDX_reserved1_BITS               10
#define GDE_WD_WORD1_SCIDX_reserved1_SHIFT              6

/* SCIDX.SCIDX [05:00] - unsigned */
#define GDE_WD_WORD1_SCIDX_SCIDX_MASK                   0x0000003FUL
#define GDE_WD_WORD1_SCIDX_SCIDX_ALIGN                  0
#define GDE_WD_WORD1_SCIDX_SCIDX_BITS                   6
#define GDE_WD_WORD1_SCIDX_SCIDX_SHIFT                  0

/* AL_2.reserved2 [15:08] -  */
#define GDE_WD_WORD1_AL_2_reserved2_MASK                0x0000FF00UL
#define GDE_WD_WORD1_AL_2_reserved2_ALIGN               0
#define GDE_WD_WORD1_AL_2_reserved2_BITS                8
#define GDE_WD_WORD1_AL_2_reserved2_SHIFT               8

/* AL_2.AL_2 [07:00] - unsigned */
#define GDE_WD_WORD1_AL_2_AL_2_MASK                     0x000000FFUL
#define GDE_WD_WORD1_AL_2_AL_2_ALIGN                    0
#define GDE_WD_WORD1_AL_2_AL_2_BITS                     8
#define GDE_WD_WORD1_AL_2_AL_2_SHIFT                    0

/* WCLR.WCLR [15:00] - unsigned */
#define GDE_WD_WORD1_WCLR_WCLR_MASK                     0x0000FFFFUL
#define GDE_WD_WORD1_WCLR_WCLR_ALIGN                    0
#define GDE_WD_WORD1_WCLR_WCLR_BITS                     16
#define GDE_WD_WORD1_WCLR_WCLR_SHIFT                    0

/**********************************************************************
 * GDE_WD_WORD2
 **********************************************************************/
/* ALTYP [31:30] - unsigned */
#define GDE_WD_WORD2_ALTYP_MASK                         0xC0000000UL
#define GDE_WD_WORD2_ALTYP_ALIGN                        0
#define GDE_WD_WORD2_ALTYP_BITS                         2
#define GDE_WD_WORD2_ALTYP_SHIFT                        30
#define GDE_WD_WORD2_ALTYP_ALPHA_FROM_SOURCE            1
#define GDE_WD_WORD2_ALTYP_ALPHA_FROM_Y                 2
#define GDE_WD_WORD2_ALTYP_SINGLE_ALPHA                 3

/* WINAL [29:22] - unsigned */
#define GDE_WD_WORD2_WINAL_MASK                         0x3FC00000UL
#define GDE_WD_WORD2_WINAL_ALIGN                        0
#define GDE_WD_WORD2_WINAL_BITS                         8
#define GDE_WD_WORD2_WINAL_SHIFT                        22

/* WYEND [21:11] - unsigned */
#define GDE_WD_WORD2_WYEND_MASK                         0x003FF800UL
#define GDE_WD_WORD2_WYEND_ALIGN                        0
#define GDE_WD_WORD2_WYEND_BITS                         11
#define GDE_WD_WORD2_WYEND_SHIFT                        11

/* WYST [10:00] - unsigned */
#define GDE_WD_WORD2_WYST_MASK                          0x000007FFUL
#define GDE_WD_WORD2_WYST_ALIGN                         0
#define GDE_WD_WORD2_WYST_BITS                          11
#define GDE_WD_WORD2_WYST_SHIFT                         0

/**********************************************************************
 * GDE_WD_WORD3
 **********************************************************************/
/* CKE [31:31] - boolean */
#define GDE_WD_WORD3_CKE_MASK                           0x80000000UL
#define GDE_WD_WORD3_CKE_ALIGN                          0
#define GDE_WD_WORD3_CKE_BITS                           1
#define GDE_WD_WORD3_CKE_SHIFT                          31

/* ARCEN [30:30] - boolean */
#define GDE_WD_WORD3_ARCEN_MASK                         0x40000000UL
#define GDE_WD_WORD3_ARCEN_ALIGN                        0
#define GDE_WD_WORD3_ARCEN_BITS                         1
#define GDE_WD_WORD3_ARCEN_SHIFT                        30

/* reserved0 [29:29] -  */
#define GDE_WD_WORD3_reserved0_MASK                     0x20000000UL
#define GDE_WD_WORD3_reserved0_ALIGN                    0
#define GDE_WD_WORD3_reserved0_BITS                     1
#define GDE_WD_WORD3_reserved0_SHIFT                    29

/* BLNKC [28:22] - unsigned */
#define GDE_WD_WORD3_BLNKC_MASK                         0x1FC00000UL
#define GDE_WD_WORD3_BLNKC_ALIGN                        0
#define GDE_WD_WORD3_BLNKC_BITS                         7
#define GDE_WD_WORD3_BLNKC_SHIFT                        22

/* WXEND [21:11] - unsigned */
#define GDE_WD_WORD3_WXEND_MASK                         0x003FF800UL
#define GDE_WD_WORD3_WXEND_ALIGN                        0
#define GDE_WD_WORD3_WXEND_BITS                         11
#define GDE_WD_WORD3_WXEND_SHIFT                        11

/* WXST [10:00] - unsigned */
#define GDE_WD_WORD3_WXST_MASK                          0x000007FFUL
#define GDE_WD_WORD3_WXST_ALIGN                         0
#define GDE_WD_WORD3_WXST_BITS                          11
#define GDE_WD_WORD3_WXST_SHIFT                         0

/**********************************************************************
 * GDE_WD_WORD4
 **********************************************************************/
/* RSV1 [31:30] - unsigned */
#define GDE_WD_WORD4_RSV1_MASK                          0xC0000000UL
#define GDE_WD_WORD4_RSV1_ALIGN                         0
#define GDE_WD_WORD4_RSV1_BITS                          2
#define GDE_WD_WORD4_RSV1_SHIFT                         30

/* reserved0 [29:11] -  */
#define GDE_WD_WORD4_reserved0_MASK                     0x3FFFF800UL
#define GDE_WD_WORD4_reserved0_ALIGN                    0
#define GDE_WD_WORD4_reserved0_BITS                     19
#define GDE_WD_WORD4_reserved0_SHIFT                    11

/* WXSIZE [10:00] - unsigned */
#define GDE_WD_WORD4_WXSIZE_MASK                        0x000007FFUL
#define GDE_WD_WORD4_WXSIZE_ALIGN                       0
#define GDE_WD_WORD4_WXSIZE_BITS                        11
#define GDE_WD_WORD4_WXSIZE_SHIFT                       0

/**********************************************************************
 * GDE_WD_WORD5
 **********************************************************************/
/* reserved0 [31:25] -  */
#define GDE_WD_WORD5_reserved0_MASK                     0xFE000000UL
#define GDE_WD_WORD5_reserved0_ALIGN                    0
#define GDE_WD_WORD5_reserved0_BITS                     7
#define GDE_WD_WORD5_reserved0_SHIFT                    25

/* WMEM_ST2 [24:00] - unsigned */
#define GDE_WD_WORD5_WMEM_ST2_MASK                      0x01FFFFFFUL
#define GDE_WD_WORD5_WMEM_ST2_ALIGN                     0
#define GDE_WD_WORD5_WMEM_ST2_BITS                      25
#define GDE_WD_WORD5_WMEM_ST2_SHIFT                     0

/**********************************************************************
 * GDE_WD_WORD6
 **********************************************************************/
/* reserved0 [31:10] -  */
#define GDE_WD_WORD6_reserved0_MASK                     0xFFFFFC00UL
#define GDE_WD_WORD6_reserved0_ALIGN                    0
#define GDE_WD_WORD6_reserved0_BITS                     22
#define GDE_WD_WORD6_reserved0_SHIFT                    10

/* WMEM_PIT2 [09:00] - unsigned */
#define GDE_WD_WORD6_WMEM_PIT2_MASK                     0x000003FFUL
#define GDE_WD_WORD6_WMEM_PIT2_ALIGN                    0
#define GDE_WD_WORD6_WMEM_PIT2_BITS                     10
#define GDE_WD_WORD6_WMEM_PIT2_SHIFT                    0

#ifdef _cplusplus
}
#endif

#endif	/* BCMVIDEOREG_H */
