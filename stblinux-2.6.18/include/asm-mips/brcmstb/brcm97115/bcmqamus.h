/***********************************************************************/
/*                                                                     */
/*   MODULE:  bcmqamus.h                                               */
/*   DATE:    02/08/02                                                 */
/*   PURPOSE: Definitions for QAM Upstream block                       */
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

#ifndef BCMQAMUS_H
#define BCMQAMUS_H

#ifdef _cplusplus
extern "C" {
#endif

/*----------------------------------------------*/
/* Upstream registers 							*/
/*----------------------------------------------*/
#define US_MISC2    	(0x00)
#define US_MISC1   		(0x01)
#define US_MISC0       	(0x02)
#define US_TEST_MODE  	(0x03)
#define US_TEST_INPUT 	(0x04)
#define US_TEST_MISC1 	(0x05)
#define US_TEST_MISC0 	(0x06)
#define US_RST        	(0x07)
#define US_DAC          (0x08)
#define US_POWER2     	(0x09)
#define US_POWER1     	(0x0a)
#define US_POWER0       (0x0b)
#define US_PORT       	(0x0c)
#define US_PLL        	(0x0d)
#define US_MAP        	(0x0e)
#define US_MOD        	(0x0f)
#define US_OEN_BDLY	    (0x10)
#define US_OEN_EDLY		(0x11)
#define US_PRBS_CFG2  	(0x12)
#define US_PRBS_CFG1  	(0x13)
#define US_PRBS_CFG0  	(0x14)
#define US_BAUD2      	(0x15)
#define US_BAUD1      	(0x16)
#define US_BAUD0      	(0x17)
#define US_IF2        	(0x18)
#define US_IF1        	(0x19)
#define US_IF0        	(0x1a)
#define US_BURST_LENGTH1	(0x1b)
#define US_BURST_LENGTH0	(0x1c)
#define US_CONFIG       (0x1d)

/* BURST CONFIG. BANK 0 */
#define US_PRBS2_B0   	(0x1e)
#define US_PRBS1_B0   	(0x1f)
#define US_PRBS0_B0   	(0x20)
#define US_RS1_B0     	(0x21)
#define US_RS0_B0     	(0x22)
#define US_FEC_B0     	(0x23)
#define US_QAM_B0     	(0x24)
#define US_PRELEN1_B0 	(0x25)
#define US_PRELEN0_B0 	(0x26)
#define US_PREOFF1_B0 	(0x27)
#define US_PREOFF0_B0 	(0x28)

/* BURST CONFIG. BANK 1 */
#define US_PRBS2_B1   	(0x29)
#define US_PRBS1_B1   	(0x2a)
#define US_PRBS0_B1   	(0x2b)
#define US_RS1_B1     	(0x2c)
#define US_RS0_B1     	(0x2d)
#define US_FEC_B1     	(0x2e)
#define US_QAM_B1     	(0x2f)
#define US_PRELEN1_B1 	(0x30)
#define US_PRELEN0_B1 	(0x31)
#define US_PREOFF1_B1 	(0x32)
#define US_PREOFF0_B1 	(0x33)

/* BURST CONFIG. BANK 2 */
#define US_PRBS2_B2   	(0x34)
#define US_PRBS1_B2   	(0x35)
#define US_PRBS0_B2   	(0x36)
#define US_RS1_B2     	(0x37)
#define US_RS0_B2     	(0x38)
#define US_FEC_B2     	(0x39)
#define US_QAM_B2     	(0x3a)
#define US_PRELEN1_B2 	(0x3b)
#define US_PRELEN0_B2 	(0x3c)
#define US_PREOFF1_B2 	(0x3d)
#define US_PREOFF0_B2 	(0x3e)

/* BURST CONFIG. BANK 3 */
#define US_PRBS2_B3   	(0x3f)
#define US_PRBS1_B3   	(0x40)
#define US_PRBS0_B3   	(0x41)
#define US_RS1_B3     	(0x42)
#define US_RS0_B3     	(0x43)
#define US_FEC_B3     	(0x44)
#define US_QAM_B3     	(0x45)
#define US_PRELEN1_B3 	(0x46)
#define US_PRELEN0_B3 	(0x47)
#define US_PREOFF1_B3 	(0x48)
#define US_PREOFF0_B3 	(0x49)

/* BURST CONFIG. BANK 4 */
#define US_PRBS2_B4   	(0x4a)
#define US_PRBS1_B4   	(0x4b)
#define US_PRBS0_B4   	(0x4c)
#define US_RS1_B4     	(0x4d)
#define US_RS0_B4     	(0x4e)
#define US_FEC_B4     	(0x4f)
#define US_QAM_B4     	(0x50)
#define US_PRELEN1_B4 	(0x51)
#define US_PRELEN0_B4 	(0x52)
#define US_PREOFF1_B4 	(0x53)
#define US_PREOFF0_B4 	(0x54)

/* BURST CONFIG. BANK 5 */
#define US_PRBS2_B5     (0x55)
#define US_PRBS1_B5   	(0x56)
#define US_PRBS0_B5   	(0x57)
#define US_RS1_B5     	(0x58)
#define US_RS0_B5     	(0x59)
#define US_FEC_B5     	(0x5a)
#define US_QAM_B5     	(0x5b)
#define US_PRELEN1_B5 	(0x5c)
#define US_PRELEN0_B5 	(0x5d)
#define US_PREOFF1_B5 	(0x5e)
#define US_PREOFF0_B5 	(0x5f)

/* EQUALIZER COEFFICIENTS BEGIN HERE */
#define EQ_COF15_MSB   	(0x60)
#define EQ_COF15_LSB   	(0x61)
#define EQ_COF14_MSB   	(0x62)
#define EQ_COF14_LSB   	(0x63)
#define EQ_COF13_MSB   	(0x64)
#define EQ_COF13_LSB   	(0x65)
#define EQ_COF12_MSB   	(0x66)
#define EQ_COF12_LSB   	(0x67)
#define EQ_COF11_MSB   	(0x68)
#define EQ_COF11_LSB   	(0x69)
#define EQ_COF10_MSB   	(0x6a)
#define EQ_COF10_LSB   	(0x6b)
#define EQ_COF09_MSB   	(0x6c)
#define EQ_COF09_LSB   	(0x6d)
#define EQ_COF08_MSB   	(0x6e)
#define EQ_COF08_LSB   	(0x6f)
#define EQ_COF07_MSB   	(0x70)
#define EQ_COF07_LSB   	(0x71)
#define EQ_COF06_MSB   	(0x72)
#define EQ_COF06_LSB   	(0x73)
#define EQ_COF05_MSB   	(0x74)
#define EQ_COF05_LSB   	(0x75)
#define EQ_COF04_MSB   	(0x76)
#define EQ_COF04_LSB   	(0x77)
#define EQ_COF03_MSB   	(0x78)
#define EQ_COF03_LSB   	(0x79)
#define EQ_COF02_MSB   	(0x7a)
#define EQ_COF02_LSB   	(0x7b)
#define EQ_COF01_MSB   	(0x7c)
#define EQ_COF01_LSB   	(0x7d)
#define EQ_COF00_MSB   	(0x7e)
#define EQ_COF00_LSB   	(0x7f)

#define US_PREBYTE    	(0x80)


/*set up burst bank data offset definitions */
#define PRBS2_OS        (0x00)
#define PRBS1_OS        (0x01)
#define PRBS0_OS        (0x02)
#define RS1_OS          (0x03)
#define RS0_OS          (0x04)
#define FEC_OS          (0x05)
#define QAM_OS          (0x06)
#define PRELEN1_OS      (0x07)
#define PRELEN0_OS      (0x08)
#define PREOFF1_OS      (0x09)
#define PREOFF0_OS      (0x0A)

#ifdef _cplusplus
}
#endif

#endif	/* BCMQAMUS_H */
