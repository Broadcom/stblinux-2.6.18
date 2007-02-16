/***************************************************************************
 *     Copyright (c) 1999-2005, Broadcom Corporation
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


/***********************************************************************/
/*                                                                     */
/*   MODULE:  bcmbridge.h                                              */
/*   DATE:    03/14/2002                                               */
/*   PURPOSE: Definitions for PBUS Bridge Registers                    */
/*                                                                     */
/***********************************************************************/
#ifndef BCMBRIDGE_H
#define BCMBRIDGE_H

#ifdef _cplusplus
extern "C" {
#endif


#ifndef _ASMLANGUAGE

typedef struct
{
   unsigned long       Status;
#define PST_BSY              0x80000000     
#define PST_RD_PND           0x40000000     
#define PST_RD_CMPLT         0x20000000     
#define PST_ERR              0x10000000     
#define PST_TEA              0x08000000     
#define DISABLE_TIMER        0x00008000
#define TRANS_TIMER_MASK     0x00007fff
    
   unsigned long       Control;
#define WR_POST_EN_RFMOD     0x00001000
#define WR_POST_EN_SOFTMODEM 0x00000800
#define WR_POST_EN_DS2       0x00000400
#define WR_POST_EN_USB       0x00000200
#define WR_POST_EN_DAVIC     0x00000100
#define WR_POST_EN_IDE       0x00000080
#define WR_POST_EN_TM_TOP    0x00000040
#define WR_POST_EN_TIMER     0x00000020
#define WR_POST_EN_INTC      0x00000010
#define WR_POST_EN_US        0x00000008
#define WR_POST_EN_DS1       0x00000004
#define WR_POST_EN_OOB       0x00000002
#define WR_POST_EN_UPG       0x00000001

   unsigned long       PostRdAddr;
   unsigned long       PostRdData;
} PbusControl;

#define PBUS    ((volatile PbusControl *) BRIDGE_ADR_BASE)


#else /* if _ASMLANGUAGE is defined */


/* Status register bit definitions */
#define PST_BSY              0x80000000     
#define PST_RD_PND           0x40000000     
#define PST_RD_CMPLT         0x20000000     
#define PST_ERR              0x10000000     
#define PST_TEA              0x08000000     
#define DISABLE_TIMER        0x00008000
#define TRANS_TIMER_MASK     0x00007fff
    
/* Control register bit definitions */
#define WR_POST_EN_RFMOD     0x00001000
#define WR_POST_EN_SOFTMODEM 0x00000800
#define WR_POST_EN_DS2       0x00000400
#define WR_POST_EN_USB       0x00000200
#define WR_POST_EN_DAVIC     0x00000100
#define WR_POST_EN_IDE       0x00000080
#define WR_POST_EN_TM_TOP    0x00000040
#define WR_POST_EN_TIMER     0x00000020
#define WR_POST_EN_INTC      0x00000010
#define WR_POST_EN_US        0x00000008
#define WR_POST_EN_DS1       0x00000004
#define WR_POST_EN_OOB       0x00000002
#define WR_POST_EN_UPG       0x00000001


#endif /* _ASMLANGUAGE */


#ifdef _cplusplus
}

#endif

#endif
