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
 * $brcm_Workfile: bchp_uarta.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/21/05 1:53p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Oct 21 12:13:42 2005
 *                 MD5 Checksum         e787da23f53e88a7247b18abd494ff4a
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.006
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/3560/rdb/b0/bchp_uarta.h $
 * 
 * Hydra_Software_Devel/1   10/21/05 1:53p jasonh
 * PR 17692: Initial version
 *
 ***************************************************************************/

#ifndef BCHP_UARTA_H__
#define BCHP_UARTA_H__

/***************************************************************************
 *UARTA - UART A
 ***************************************************************************/
#define BCHP_UARTA_RCVSTAT                       0x00400180 /* RECEIVE STATUS REGISTER */
#define BCHP_UARTA_RCVDATA                       0x00400184 /* RECEIVE DATA REGISTER */
#define BCHP_UARTA_CONTROL                       0x0040018c /* CONTROL REGISTER */
#define BCHP_UARTA_BAUDHI                        0x00400190 /* BAUD RATE REGISTER HI */
#define BCHP_UARTA_BAUDLO                        0x00400194 /* BAUD RATE REGISTER LO */
#define BCHP_UARTA_XMTSTAT                       0x00400198 /* TRANSMIT STATUS REGISTER */
#define BCHP_UARTA_XMTDATA                       0x0040019c /* TRANSMIT DATA REGISTER */

/***************************************************************************
 *RCVSTAT - RECEIVE STATUS REGISTER
 ***************************************************************************/
/* UARTA :: RCVSTAT :: reserved0 [31:06] */
#define BCHP_UARTA_RCVSTAT_reserved0_MASK                          0xffffffc0
#define BCHP_UARTA_RCVSTAT_reserved0_SHIFT                         6

/* UARTA :: RCVSTAT :: PE [05:05] */
#define BCHP_UARTA_RCVSTAT_PE_MASK                                 0x00000020
#define BCHP_UARTA_RCVSTAT_PE_SHIFT                                5

/* UARTA :: RCVSTAT :: FE [04:04] */
#define BCHP_UARTA_RCVSTAT_FE_MASK                                 0x00000010
#define BCHP_UARTA_RCVSTAT_FE_SHIFT                                4

/* UARTA :: RCVSTAT :: OVRN [03:03] */
#define BCHP_UARTA_RCVSTAT_OVRN_MASK                               0x00000008
#define BCHP_UARTA_RCVSTAT_OVRN_SHIFT                              3

/* UARTA :: RCVSTAT :: RDA [02:02] */
#define BCHP_UARTA_RCVSTAT_RDA_MASK                                0x00000004
#define BCHP_UARTA_RCVSTAT_RDA_SHIFT                               2

/* UARTA :: RCVSTAT :: RIE [01:01] */
#define BCHP_UARTA_RCVSTAT_RIE_MASK                                0x00000002
#define BCHP_UARTA_RCVSTAT_RIE_SHIFT                               1

/* UARTA :: RCVSTAT :: reserved1 [00:00] */
#define BCHP_UARTA_RCVSTAT_reserved1_MASK                          0x00000001
#define BCHP_UARTA_RCVSTAT_reserved1_SHIFT                         0

/***************************************************************************
 *RCVDATA - RECEIVE DATA REGISTER
 ***************************************************************************/
/* UARTA :: RCVDATA :: reserved0 [31:08] */
#define BCHP_UARTA_RCVDATA_reserved0_MASK                          0xffffff00
#define BCHP_UARTA_RCVDATA_reserved0_SHIFT                         8

/* UARTA :: RCVDATA :: RDR [07:00] */
#define BCHP_UARTA_RCVDATA_RDR_MASK                                0x000000ff
#define BCHP_UARTA_RCVDATA_RDR_SHIFT                               0

/***************************************************************************
 *CONTROL - CONTROL REGISTER
 ***************************************************************************/
/* UARTA :: CONTROL :: reserved0 [31:07] */
#define BCHP_UARTA_CONTROL_reserved0_MASK                          0xffffff80
#define BCHP_UARTA_CONTROL_reserved0_SHIFT                         7

/* UARTA :: CONTROL :: STOP2 [06:06] */
#define BCHP_UARTA_CONTROL_STOP2_MASK                              0x00000040
#define BCHP_UARTA_CONTROL_STOP2_SHIFT                             6

/* UARTA :: CONTROL :: LOOP [05:05] */
#define BCHP_UARTA_CONTROL_LOOP_MASK                               0x00000020
#define BCHP_UARTA_CONTROL_LOOP_SHIFT                              5

/* UARTA :: CONTROL :: BITM8 [04:04] */
#define BCHP_UARTA_CONTROL_BITM8_MASK                              0x00000010
#define BCHP_UARTA_CONTROL_BITM8_SHIFT                             4

/* UARTA :: CONTROL :: PAREN [03:03] */
#define BCHP_UARTA_CONTROL_PAREN_MASK                              0x00000008
#define BCHP_UARTA_CONTROL_PAREN_SHIFT                             3

/* UARTA :: CONTROL :: TXE [02:02] */
#define BCHP_UARTA_CONTROL_TXE_MASK                                0x00000004
#define BCHP_UARTA_CONTROL_TXE_SHIFT                               2

/* UARTA :: CONTROL :: RXE [01:01] */
#define BCHP_UARTA_CONTROL_RXE_MASK                                0x00000002
#define BCHP_UARTA_CONTROL_RXE_SHIFT                               1

/* UARTA :: CONTROL :: PODD [00:00] */
#define BCHP_UARTA_CONTROL_PODD_MASK                               0x00000001
#define BCHP_UARTA_CONTROL_PODD_SHIFT                              0

/***************************************************************************
 *BAUDHI - BAUD RATE REGISTER HI
 ***************************************************************************/
/* UARTA :: BAUDHI :: reserved0 [31:08] */
#define BCHP_UARTA_BAUDHI_reserved0_MASK                           0xffffff00
#define BCHP_UARTA_BAUDHI_reserved0_SHIFT                          8

/* UARTA :: BAUDHI :: BRR_1 [07:07] */
#define BCHP_UARTA_BAUDHI_BRR_1_MASK                               0x00000080
#define BCHP_UARTA_BAUDHI_BRR_1_SHIFT                              7

/* UARTA :: BAUDHI :: reserved1 [06:06] */
#define BCHP_UARTA_BAUDHI_reserved1_MASK                           0x00000040
#define BCHP_UARTA_BAUDHI_reserved1_SHIFT                          6

/* UARTA :: BAUDHI :: BRR [05:00] */
#define BCHP_UARTA_BAUDHI_BRR_MASK                                 0x0000003f
#define BCHP_UARTA_BAUDHI_BRR_SHIFT                                0

/***************************************************************************
 *BAUDLO - BAUD RATE REGISTER LO
 ***************************************************************************/
/* UARTA :: BAUDLO :: reserved0 [31:08] */
#define BCHP_UARTA_BAUDLO_reserved0_MASK                           0xffffff00
#define BCHP_UARTA_BAUDLO_reserved0_SHIFT                          8

/* UARTA :: BAUDLO :: BRR [07:00] */
#define BCHP_UARTA_BAUDLO_BRR_MASK                                 0x000000ff
#define BCHP_UARTA_BAUDLO_BRR_SHIFT                                0

/***************************************************************************
 *XMTSTAT - TRANSMIT STATUS REGISTER
 ***************************************************************************/
/* UARTA :: XMTSTAT :: reserved0 [31:03] */
#define BCHP_UARTA_XMTSTAT_reserved0_MASK                          0xfffffff8
#define BCHP_UARTA_XMTSTAT_reserved0_SHIFT                         3

/* UARTA :: XMTSTAT :: TIE [02:02] */
#define BCHP_UARTA_XMTSTAT_TIE_MASK                                0x00000004
#define BCHP_UARTA_XMTSTAT_TIE_SHIFT                               2

/* UARTA :: XMTSTAT :: IDLE [01:01] */
#define BCHP_UARTA_XMTSTAT_IDLE_MASK                               0x00000002
#define BCHP_UARTA_XMTSTAT_IDLE_SHIFT                              1

/* UARTA :: XMTSTAT :: TDRE [00:00] */
#define BCHP_UARTA_XMTSTAT_TDRE_MASK                               0x00000001
#define BCHP_UARTA_XMTSTAT_TDRE_SHIFT                              0

/***************************************************************************
 *XMTDATA - TRANSMIT DATA REGISTER
 ***************************************************************************/
/* UARTA :: XMTDATA :: reserved0 [31:08] */
#define BCHP_UARTA_XMTDATA_reserved0_MASK                          0xffffff00
#define BCHP_UARTA_XMTDATA_reserved0_SHIFT                         8

/* UARTA :: XMTDATA :: TDR [07:00] */
#define BCHP_UARTA_XMTDATA_TDR_MASK                                0x000000ff
#define BCHP_UARTA_XMTDATA_TDR_SHIFT                               0

#endif /* #ifndef BCHP_UARTA_H__ */

/* End of File */
