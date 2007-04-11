/***********************************************************************/
/*                                                                     */
/*   MODULE:  bcmintc.h                                                */
/*   DATE:    96/12/19                                                 */
/*   PURPOSE: Definitions for Interrupt controller (INTC) block        */
/*                                                                     */
/***********************************************************************/

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

#ifndef BCMINTC_H
#define BCMINTC_H

#ifdef _cplusplus
extern "C" {
#endif


/* BCM7320 Interrupt Control register's offset.	*/
#define IRQ_INT_MASK		0x0c
#define IRQ_INT_STATUS		0x10
#define IRQ_EXT_MASK		0x14
#define IRQ_EXT_CLEAR		0x15
#define IRQ_EXT_STATUS		0x16
#define IRQ_EXT_CONFIG		0x17

#include "bcmintc_struct.h"


#ifdef _cplusplus
}
#endif

#endif
