/*---------------------------------------------------------------------------

    Copyright (c) 2008 Broadcom Corporation                      /\
                                                          _     /  \     _
    _____________________________________________________/ \   /    \   / \_
                                                            \_/      \_/  
    
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 as
 published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    File: bcmspi.h

    Description: 
    Headers for simplified "bcmspi" interface

    when        who         what
    -----       ---         ----
    20080620    cernekee    initial version
 ------------------------------------------------------------------------- */

#ifndef _BCMSPI_H
#define _BCMSPI_H

#include <linux/types.h>

struct bcmspi_parms {
	u32			speed_hz;
	u8			chip_select;
	u8			mode;
	u8			bits_per_word;
};

extern const struct bcmspi_parms bcmspi_default_parms_cs0;
extern const struct bcmspi_parms bcmspi_default_parms_cs1;

int bcmspi_simple_transaction(struct bcmspi_parms *xp,
	const void *tx_buf, int tx_len, void *rx_buf, int rx_len);

#endif /* _BCMSPI_H */
