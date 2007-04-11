/***********************************************************************/
/*                                                                     */
/*   MODULE:  bcmtimer_struct.h                                               */
/*   DATE:    96/12/19                                                 */
/*   PURPOSE: Structures for Timer block                               */
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

#ifndef BCMTIMER_STRUCT_H
#define BCMTIMER_STRUCT_H

#ifdef _cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE


typedef struct Timer {
  unsigned char  TimerInts;
#define TIMER0          0x01
#define TIMER1          0x02
#define TIMER2          0x04
#define TIMER3          0x08
#define WATCHDOG        0x10
  unsigned char  TimerMask;
#define TIMER0EN        0x01
#define TIMER1EN        0x02
#define TIMER2EN        0x04
#define TIMER3EN        0x08
  unsigned short unused0;			/* 0x00 */
  unsigned long  TimerCtl0;			/* 0x04 */
  unsigned long  TimerCtl1;			/* 0x08 */
  unsigned long  TimerCtl2;			/* 0x0c */
  unsigned long  TimerCtl3;			/* 0x10 */  
#define TIMERENABLE     0x80000000
#define RSTCNTCLR       0x40000000      
  unsigned long  TimerCnt0;			/* 0x14 */
  unsigned long  TimerCnt1;			/* 0x18 */
  unsigned long  TimerCnt2;			/* 0x1c */
  unsigned long  TimerCnt3;			/* 0x20 */
  unsigned long  WatchDogDefCount;	/* 0x24 */

  /* Write 0xff00 0x00ff to Start timer
   * Write 0xee00 0x00ee to Stop and re-load default count
   * Read from this register returns current watch dog count
   */
  unsigned long  WatchDogCtl;		/* 0x28 */

  /* Number of 40-MHz ticks for WD Reset pulse to last */
  unsigned long  WDResetCount;		/* 0x2c */
} Timer;

#define TIMER ((volatile Timer * const)(TIMR_ADR_BASE))


#endif /* _ASMLANGUAGE */

#ifdef _cplusplus
}
#endif

#endif
