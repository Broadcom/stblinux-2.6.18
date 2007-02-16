/***********************************************************************/
/*                                                                     */
/*   MODULE:  bcmtimer.h                                               */
/*   DATE:    96/12/19                                                 */
/*   PURPOSE: Definitions for Timer block                               */
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


#ifndef BCMTIMER_H
#define BCMTIMER_H

#ifdef _cplusplus
extern "C" {
#endif


#define TIME_INTC			0x00
#define TIMER_0_CTL			0x04
#define TIMER_1_CTL			0x08
#define TIMER_2_CTL			0x0c
#define TIMER_3_CTL			0x10
#define TIMER_0_STS			0x14
#define TIMER_1_STS			0x18
#define TIMER_2_STS			0x1c
#define TIMER_3_STS			0x20
#define WATCHDOG_TIMEOUT	0x24
#define WATCHDOG_CMD		0x28
#define WATCHDOG_CNT		0x2c

#define TIMER_MASK      	2
#define TIMER_INTS      	3


/********************************************************************
   Include "bcmtimer_struct.h" to define register structures.
 ********************************************************************/
#include "bcmtimer_struct.h"


#ifdef _cplusplus
}
#endif

#endif
