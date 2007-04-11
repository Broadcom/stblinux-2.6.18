/***********************************************************************/
/*                                                                     */
/*   MODULE:  bcmqamob.h                                               */
/*   DATE:    02/08/02                                                 */
/*   PURPOSE: Definitions for QAM Out of Band block                    */
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

#ifndef BCMQAMOB_H
#define BCMQAMOB_H

#ifdef _cplusplus
extern "C" {
#endif

/*----------------------------------------------*/
/* OUT-OF-BAND 									*/
/*----------------------------------------------*/
/* SINGLE BYTE */
#define SB_OB_RSTC      (0x85)
#define SB_OB_FRZC      (0x86)
#define SB_OB_LMSC      (0x87)
#define SB_OB_BYPC      (0x88)
#define SB_OB_FMTC      (0x89)
#define SB_OB_FFEC      (0x8A)
#define SB_OB_RSTR      (0x8B)
#define SB_OB_IRQS      (0x8C)
#define SB_OB_IRQM      (0x8D)
#define SB_OB_BERC      (0x8E)
#define SB_OB_BERS      (0x8F)
#define SB_OB_DCCC      (0x90)
#define SB_OB_AGCC      (0x91)
#define SB_OB_USYN      (0x92)
#define SB_OB_DR2C      (0x93)
#define SB_OB_FLIC      (0x94)
#define SB_OB_RS2C      (0x95)
#define SB_OB_MISC      (0x96)
#define SB_OB_PGAC      (0x97)
#define SB_OB_POLS      (0x98)
#define SB_OB_ADBC      (0x99)

/* MULTI-BYTE */
#define	OB_MLONG		0x80
#define OB_MBYTE0       (0x80)
#define OB_MBYTE1       (0x81)
#define OB_MBYTE2       (0x82)
#define OB_MBYTE3       (0x83)
#define OB_MBYTEOP      (0x84)

#define MB_OB_STF0      0x00
#define MB_OB_STF1      0x01
#define MB_OB_STD0      0x08
#define MB_OB_STD1      0x09
#define MB_OB_STSNRT    0x12
#define MB_OB_STSNRE    0x13
#define MB_OB_STSNRC    0x14
#define MB_OB_STATHR    0x20
#define MB_OB_STABW     0x21
#define MB_OB_STAII     0x22
#define MB_OB_STCRVC    0x23
#define MB_OB_STAIT     0x24
#define MB_OB_STAGI     0x25
#define MB_OB_STAGT     0x26
#define MB_OB_STBRLC    0x28
#define MB_OB_STBRIC    0x29
#define MB_OB_STBRI     0x2A
#define MB_OB_STBFOS    0x2B
#define MB_OB_STBEN     0x2C
#define MB_OB_STBRDL    0x2D
//#define MB_OB_STLPST    0x2F
#define MB_OB_STDRLC    0x30
#define MB_OB_STDRIC    0x31
#define MB_OB_STDRI     0x32
#define MB_OB_STDRSP    0x33
#define MB_OB_STDRPA    0x34
#define MB_OB_STD2I     0x35
#define MB_OB_STD2IC    0x36
#define MB_OB_STD2LC    0x37
#define MB_OB_STD2PA    0x38
#define MB_OB_STFLA     0x3A
#define MB_OB_STFLTH    0x3B
#define MB_OB_STFECL    0x40
#define MB_OB_STFECH    0x41
#define MB_OB_STATMC    0x48
#define MB_OB_STRSB     0x49
#define MB_OB_STHEC     0x4A
#define MB_OB_STATML    0x4B
#define MB_OB_STSL      0x4C
#define MB_OB_STSF      0x4D
#define MB_OB_STRXC     0x4E
#define MB_OB_STPAR     0x4F
#define MB_OB_STBERTT   0x50
#define MB_OB_STDEL     0x54
#define MB_OB_STDCC     0x58
#define MB_OB_STCLP     0X59

#define MB_OB_LDF0      0x80
#define MB_OB_LDF1      0x81
#define MB_OB_LDD0      0x88
#define MB_OB_LDD1      0x89
#define MB_OB_LDSFT     0x94
#define MB_OB_LDSNRE    0x95
#define MB_OB_LDATMC    0xA8
#define MB_OB_LDRSB     0xA9
#define MB_OB_LDHEC     0xAA
#define MB_OB_LDATML    0xAB
#define MB_OB_LDSL      0xAC
#define MB_OB_LDSF      0xAD
#define MB_OB_LDRXC     0xAE
#define MB_OB_LDPAR     0xAF
#define MB_OB_LDADSI    0xB0
#define MB_OB_LDADST    0xB1
#define MB_OB_LDAII     0xB2
#define MB_OB_LDALI     0xB3
#define MB_OB_LDAIT     0xB4
#define MB_OB_LDAGI     0xB5
#define MB_OB_LDAGT     0xB6
#define MB_OB_LDBRFO    0xB8
#define MB_OB_LDBNCO    0xB9
#define MB_OB_LDBRI     0xBA
#define MB_OB_LDDRFO    0xBC
#define MB_OB_LDDRPA    0xBD
#define MB_OB_LDDRI     0xBE
#define MB_OB_LDBERTT   0xC0
#define MB_OB_LDBERTC   0xC1
#define MB_OB_LDDEL     0xC4
#define MB_OB_LDFLA     0xCA
#define MB_OB_LDDCC     0xCC
#define MB_OB_LDD2FO    0xCD
#define MB_OB_LDD2PA    0xCE
#define MB_OB_LDD2I     0xCF
#define MB_OB_LDCLP     0xD0
#define MB_OB_LDCRVC	0xD1

/****************************************************************************
 * Register bit definitions
 ****************************************************************************/
/* RSTC register */
#define OB_RSTC_AGLRST	0x01
#define OB_RSTC_BRLRST	0x02
#define OB_RSTC_DRLRST	0x04
#define OB_RSTC_SNRRST	0x08
#define OB_RSTC_DFRST	0x10
#define OB_RSTC_FFRRST	0x20
#define OB_RSTC_FECRST	0x40
#define OB_RSTC_SFTRST	0x80

/* IRQM registers */
#define OB_IRQM_SNR_LM	0x01
#define OB_IRQM_SNR_IM	0x02
#define OB_IRQM_FEC_LM	0x04
#define OB_IRQM_FEC_IM	0x08
#define OB_IRQM_FLI_LM	0x10
#define OB_IRQM_FLI_IM	0x20

/* IRQS registers */
#define OB_IRQS_SNR_LS	0x01
#define OB_IRQS_SNR_IS	0x02
#define OB_IRQS_FEC_LS	0x04
#define OB_IRQS_FEC_IS	0x08
#define OB_IRQS_FLI_LS	0x10
#define OB_IRQS_FLI_IS	0x20

#ifdef _cplusplus
}
#endif

#endif	/* BCMQAMOB_H */
