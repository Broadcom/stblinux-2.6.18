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
/*   DATE:    96/12/19                                                 */
/*   PURPOSE: Definitions for PBUS bridge block                        */
/*                                                                     */
/***********************************************************************/
#ifndef BCMBRIDGE_H
#define BCMBRIDGE_H

#ifdef _cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

typedef struct {
   unsigned long status;           /* 0x00 */
   unsigned long control;          /* 0x04 */
   unsigned long post_rd_addr;     /* 0x08 */
   unsigned long post_rd_data;     /* 0x0c */
   unsigned long post_rd2_status;  /* 0x10 */
   unsigned long post_rd2_addr;    /* 0x14 */
   unsigned long post_rd2_data;    /* 0x18 */
   unsigned long post_intr_mask;   /* 0x1c */
} Bridge;

#define BRIDGE ((volatile Bridge * const) BRIDGE_ADR_BASE)

#endif /* _ASMLANGUAGE */

#ifdef _cplusplus
}
#endif

#endif
