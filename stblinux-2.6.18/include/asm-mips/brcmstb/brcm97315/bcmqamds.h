/***********************************************************************/
/*                                                                     */
/*   MODULE:  bcmqamds.h                                               */
/*   DATE:    02/08/02                                                 */
/*   PURPOSE: Definitions for QAM Downstream block                     */
/*                                                                     */
/***********************************************************************/

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
 */

#ifndef BCMQAMDS_H
#define BCMQAMDS_H

#ifdef _cplusplus
extern "C" {
#endif

/*----------------------------------------------*/
/* IN-BAND 										*/
/*----------------------------------------------*/
#define QAM_DS_REG_SIZE   0x42
#define QAM_DS_NUM_MB_REG 0xFF

/* SINGLE BYTE */
/* reset and freeze */
#define SB_RST1     (0X05)
#define SB_RST2     (0X06)
#define SB_FRZ1     (0X07)
#define SB_FRZ2     (0X08)

/* demodulator */
#define SB_MIX      (0X09)
#define SB_FILT     (0X0A)
#define SB_NTCH     (0X0B)
#define SB_NSW1     (0X0C)  /* BCM only */
#define SB_NSW2     (0X0D)  /* BCM only */
#define SB_NSW3     (0X0E)  /* BCM only */

/* equalizer */
#define SB_QAM      (0X0F)
#define SB_QAM_SYMM_M	0x07		/* Mask for SYMM field of SB_QAM */

#define SB_BLND     (0X10)
#define SB_FFE1     (0X11)
#define SB_FFE2     (0X12)
#define SB_FFE3     (0X13)
#define SB_DFE1     (0X14)
#define SB_DFE2     (0X15)
#define SB_DFE3     (0X16)
#define SB_RSVD     (0X17)  /* BCM only */

/* fec */
#define SB_IDEP     (0X18)

/* loops */
#define SB_AGI      (0X19)
#define SB_AGT      (0X1A)
#define SB_CFL      (0X1B)
#define SB_CPL1     (0X1C)
#define SB_CPL2     (0X1D)
#define SB_TL       (0X1E)
#define SB_BDC      (0X1F)

/* performance monitoring */
#define SB_SNR      (0X20)
#define SB_BER1     (0X21)
#define SB_BER2     (0X22)
#define SB_IRS1     (0X23)
#define SB_IRM1     (0X24)
#define SB_IRS2     (0X25)
#define SB_IRM2     (0X26)
#define SB_CLDT     (0X27)
#define SB_TLDT     (0X28)
#define SB_LDT      (0X27)
#define SB_LDS      (0X28)

/* output interface */
#define SB_PS       (0X29)
#define SB_OI       (0X2A)
#define SB_OIC      (0X2B)
#define SB_OID      (0X2C)
#define SB_OIM      (0X2D)
#define SB_OIS      (0X2E)  /* BCM only */

/* clocks */
#define SB_AFE      (0X2F)
#define SB_AFE1		(0X2F)
#define SB_AFE2		(0X30)
#define SB_AFE3		(0X31)
#define SB_AFE4		(0X32)
#define SB_AFE5		(0X33)
#define SB_AFE6		(0X34)
#define SB_AFE7		(0X35)
#define SB_AFE8		(0X36)

#define SB_PLL      (0X37)
#define SB_CLK      (0X38)

/* tuner */
#define SB_TUNS     (0X39)
#define SB_TUNC     (0X3A)
#define SB_TUN0     (0X3B)
#define SB_TUN1     (0X3C)

/* test and misc */
#define SB_TP       (0X3D)
#define SB_TP2      (0X3E)
#define SB_TRI      (0X3F)
#define SB_MSC1     (0X46)
#define SB_MSC2     (0X47)
#define SB_MSC3     (0X48)

#define SB_PGA		(0X4C)

/* MULTI-BYTE */
/* Registers for Indirect Access */
#define DS_MLONG	0x00
#define DS_MBYTE0	(0x00)
#define DS_MBYTE1   (0x01)
#define DS_MBYTE2   (0x02)
#define DS_MBYTE3   (0x03)
#define DS_MBYTEOP  (0x04)

/* notch filter */
#define MB_STNCH0   	0X00
#define MB_STNCH1   	0X01
#define MB_STNCH2     	0X02
#define MB_LDBBDC     	0X06
#define MB_LDNCH0     	0X07
#define MB_LDNCH1     	0X08
#define MB_LDNCH2     	0X09
#define MB_LDNSWP0    	0X0A  /* BCM only */
#define MB_LDNSWP1    	0X0B  /* BCM only */
#define MB_LDNSWP2    	0X0C  /* BCM only */
#define MB_LDNSWP3    	0X0D  /* BCM only */
#define MB_LDNSWP4    	0X0E  /* BCM only */
#define MB_LDNSWP5    	0X0F  /* BCM only */

/* equalizer */
#define MB_STF0         0x10
#define MB_STF1         0x11
#define MB_STF2         0x12
#define MB_STF3         0x13
#define MB_STF4         0x14
#define MB_STF5         0x15
#define MB_STF6         0x16
#define MB_STF7         0x17
#define MB_STF8         0x18
#define MB_STF9         0x19
#define MB_STF10        0x1A
#define MB_STF11        0x1B
#define MB_STF12        0x1C
#define MB_STF13        0x1D
#define MB_STF14        0x1E
#define MB_STF15        0x1F

#define MB_STD0         0x20
#define MB_STD1         0x21
#define MB_STD2         0x22
#define MB_STD3         0x23
#define MB_STD4         0x24
#define MB_STD5         0x25
#define MB_STD6         0x26
#define MB_STD7         0x27
#define MB_STD8         0x28
#define MB_STD9         0x29
#define MB_STD10        0x2A
#define MB_STD11        0x2B
#define MB_STD12        0x2C
#define MB_STD13        0x2D
#define MB_STD14        0x2E
#define MB_STD15        0x2F
#define MB_STD16        0x30
#define MB_STD17        0x31
#define MB_STD18        0x32
#define MB_STD19        0x33
#define MB_STD20        0x34
#define MB_STD21        0x35
#define MB_STD22        0x36
#define MB_STD23        0x37

#define MB_STCMA        0X38
#define MB_STRCA        0X39
#define MB_STFMTHR      0X3A
#define MB_STEQLK       0X3B

#define MB_LDF0         0x40
#define MB_LDF1         0x41
#define MB_LDF2         0x42
#define MB_LDF3         0x43
#define MB_LDF4         0x44
#define MB_LDF5         0x45
#define MB_LDF6         0x46
#define MB_LDF7         0x47
#define MB_LDF8         0x48
#define MB_LDF9         0x49
#define MB_LDF10        0x4A
#define MB_LDF11        0x4B
#define MB_LDF12        0x4C
#define MB_LDF13        0x4D
#define MB_LDF14        0x4E
#define MB_LDF15        0x4F

#define MB_LDD0         0x50
#define MB_LDD1         0x51
#define MB_LDD2         0x52
#define MB_LDD3         0x53
#define MB_LDD4         0x54
#define MB_LDD5         0x55
#define MB_LDD6         0x56
#define MB_LDD7         0x57
#define MB_LDD8         0x58
#define MB_LDD9         0x59
#define MB_LDD10        0x5A
#define MB_LDD11        0x5B
#define MB_LDD12        0x5C
#define MB_LDD13        0x5D
#define MB_LDD14        0x5E
#define MB_LDD15        0x5F
#define MB_LDD16        0x60
#define MB_LDD17        0x61
#define MB_LDD18        0x62
#define MB_LDD19        0x63
#define MB_LDD20        0x64
#define MB_LDD21        0x65
#define MB_LDD22        0x66
#define MB_LDD23        0x67

#define MB_LDCMA        0X68
#define MB_LDRCA        0X69
#define MB_LDFMTHR      0X6A
#define MB_LDSFT        0X6B
#define MB_LDEQLK       0X6C

/* fec */
#define MB_STFECH       0X70
#define MB_STFECM       0X71
#define MB_STFECL       0X72

#define MB_LDFECH		0X78
#define MB_LDFECM		0X79
#define MB_LDFECL		0X7A

#define MB_LDCERC       0X7B
#define MB_LDUERC       0X7C
#define MB_LDNBERC      0X7D
#define MB_LDCBERC      0X7E
#define MB_LDBMPG       0X7F
#define MB_LDFECN		0X80
#define MB_LDFECD		0X81

/* if agc loop */
#define MB_STATHR       0X82
#define MB_STABW        0X83
#define MB_STAII        0X84
#define MB_STAGI        0X85
#define MB_STPGA1       0X86
#define MB_STPGA2       0XFA
#define MB_STPGA3       0XFB
#define MB_LDATHR		0X88
#define MB_LDABW		0X89
#define MB_LDAII        0X8A
#define MB_LDADSI       0X8B
#define MB_LDAGI        0X8C
#define MB_LDPGA1		0X8D
#define MB_LDPGA2		0XFC
#define MB_LDPGA3		0XFD

/* tuner agc loop */
#define MB_STAIT        0X8E
#define MB_STAGT        0X8F
#define MB_LDAIT        0X90
#define MB_LDADST       0X91
#define MB_LDAGT        0X92

/* fine agc loop */
#define MB_STAFTHR      0X93
#define MB_STAIF        0X94
#define MB_LDAIF        0X96

/* carrier freq loop */
#define MB_STCFC        0X97
#define MB_STCFI        0X98
#define MB_STCFSP       0X99
#define MB_STCFPA       0X9A
#define MB_STCFFOS      0X9B
#define MB_LDCFFO       0X9C
#define MB_LDCFC		0X9D
#define MB_LDCFI        0X9E
#define MB_LDCFSP		0X9F
#define MB_LDCFPA       0XA0
#define MB_LDCFFOS		0XA1

/* carrier phase loop */
#define MB_STCPC        0XA2
#define MB_STCPI        0XA3
#define MB_LDCPFO       0XA6
#define MB_LDCPC        0XA7
#define MB_LDCPI        0XA8
#define MB_LDCPFD		0XAA

/* timing loop */
#define MB_STTC         0XAB
#define MB_STTI         0XAC
#define MB_STTFOS       0XAD
#define MB_LDTFO        0XAE
#define MB_LDTC         0XAF
#define MB_LDTI         0XB0
#define MB_LDTFOS		0XB1

/* if dc canceller */
#define MB_STIF         0XB4
#define MB_LDIF         0XB5

/* snr/ber estimators */
#define MB_STSNRHT      0XB6
#define MB_STSNRLT      0XB7
#define MB_STBER        0XB9
#define MB_LDSNRHT		0XBA
#define MB_LDSNRLT		0XBB
#define MB_LDSNRE       0XBC
#define MB_LDBER        0XBD

/* lock detectors */
#define MB_STCLDHT      0XBE
#define MB_STCLDLT      0XBF
#define MB_STCLDA       0XC0
#define MB_STCLDC       0XC1
#define MB_STCLCTR      0XC2
#define MB_STTLDHT      0XC3
#define MB_STTLDLT      0XC4
#define MB_STTLDA       0XC5
#define MB_STTLDC       0XC6
#define MB_STTLCTR		0XC7
#define MB_LDCLDHT		0XC8
#define MB_LDCLDLT		0XC9
#define MB_LDCLDA       0XCA
#define MB_LDCLDC		0XCB
#define MB_LDCLCTR		0XCC
#define MB_LDTLDHT		0XCD
#define MB_LDTLDLT		0XCE
#define MB_LDTLDA       0XCF
#define MB_LDTLDC       0XD0
#define MB_LDTLCTR      0XD1

/* output interface */
#define MB_STOIPN       0XD2
#define MB_STOIPD       0XD3
#define MB_STOIFO       0XD8
#define MB_STGDSI       0XDC
#define MB_LDADC        0XDF

/* misc */
#define MB_LDCLIP       0XE1

/****************************************************************************
 * Register bit definitions
 ****************************************************************************/
/* RST1 register */
#define DS_RST1_LDRST	0x01
#define DS_RST1_SNRRST	0x02
#define DS_RST1_NTCRST	0x04
#define DS_RST1_DFERST	0x08
#define DS_RST1_FFERST	0x10
#define DS_RST1_GDSRST	0x20
#define DS_RST1_FECRST	0x40
#define DS_RST1_SFTRST	0x80

/* IRM1, IRS1 registers */
#define DS_IRM1_SLL		0x01
#define DS_IRM1_FLL		0x02
#define DS_IRM1_SOL		0x04
#define DS_IRM1_FOL		0x08
#define DS_IRM1_COV		0x10
#define DS_IRM1_UOV		0x20
#define DS_IRM1_ERN		0x40
#define DS_IRM1_ERC		0x80

#define DS_IRS1_SLL		0x01
#define DS_IRS1_FLL		0x02
#define DS_IRS1_SOL		0x04
#define DS_IRS1_FOL		0x08
#define DS_IRS1_COV		0x10
#define DS_IRS1_UOV		0x20
#define DS_IRS1_ERN		0x40
#define DS_IRS1_ERC		0x80

/* IRM2, IRS2 registers */
#define DS_IRM2_FFE		0x01
#define DS_IRM2_AGF		0x02
#define DS_IRM2_FFOV	0x04
#define DS_IRM2_CL		0x08
#define DS_IRM2_BMPG	0x10
#define DS_IRM2_TL		0x20
#define DS_IRM2_OIFOV	0x40
#define DS_IRM2_OIFUN	0x80

#define DS_IRS2_FFE		0x01
#define DS_IRS2_AGF		0x02
#define DS_IRS2_FFOV	0x04
#define DS_IRS2_CL		0x08
#define DS_IRS2_BMPG	0x10
#define DS_IRS2_TL		0x20
#define DS_IRS2_OIFOV	0x40
#define DS_IRS2_OIFUN	0x80



#ifdef _cplusplus
}
#endif

#endif	/* BCMQAMDS_H */
