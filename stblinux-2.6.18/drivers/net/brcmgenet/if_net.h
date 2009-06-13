/*
<:copyright-gpl 
 Copyright 2002 Broadcom Corp. All Rights Reserved. 
 
 This program is free software; you can distribute it and/or modify it 
 under the terms of the GNU General Public License (Version 2) as 
 published by the Free Software Foundation. 
 
 This program is distributed in the hope it will be useful, but WITHOUT 
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
 for more details. 
 
 You should have received a copy of the GNU General Public License along 
 with this program; if not, write to the Free Software Foundation, Inc., 
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA. 
:>
*/
/***********************************************************************/
/*                                                                     */
/*   MODULE:  if_net.h                                                 */
/*   DATE:    05/16/02                                                 */
/*   PURPOSE: network interface ioctl definition                       */
/*                                                                     */
/***********************************************************************/
#ifndef _IF_NET_H_
#define _IF_NET_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <linux/autoconf.h>
/*
 * Ioctl definitions.
 */

#define SIOCSACPISET		(SIOCDEVPRIVATE + 2)	/* go into ACPI WoL mode*/
#define SIOCSACPICANCEL		(SIOCDEVPRIVATE + 3)	/* cancel ACPI WoL Mode */
#define SIOCGPATTERN		(SIOCDEVPRIVATE + 4)	/* get ACPI pattern */
#define SIOCSPATTERN		(SIOCDEVPRIVATE + 5)	/* set ACPI pattern */

/* 
 * ACPI pattern (or generic HFB ) data
 */
typedef union _acpi_pattern{
	unsigned long value;
	struct {
#ifdef CONFIG_CPU_BIG_ENDIAN
		unsigned long unused : 12 ;
		unsigned long mask   : 4;
		unsigned long data   : 16;
#else
		unsigned long data   : 16 ;
		unsigned long mask   : 4;
		unsigned long unused : 12;
#endif
	}pattern;
}acpi_pattern_u;

/* used by ioctl call */
struct acpi_data {
	int fltr_index;				/* current filter index */
	acpi_pattern_u * p_data;	/* pointer to the pattern data.*/
	int count;					/* total count of the data */
};

#ifdef __cplusplus
}
#endif

#endif /* _IF_NET_H_ */
