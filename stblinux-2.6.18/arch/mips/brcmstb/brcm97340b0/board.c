/*
 * arch/mips/brcmstb/brcm9xxxxyy/board.c
 *
 * Copyright (C) 2004-2007 Broadcom Corporation
 *
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
 *
 * Board Specific routines for Broadcom eval boards
 *
 * when         who    what
 * ----         ---    ----
 * 03-31-2004   THT    Created
 * 09-21-2006	TDT	   Added to 2.6.18 kernel
 */

#include <linux/config.h>
// For module exports
#include <linux/module.h>
#include <asm/brcmstb/common/brcmstb.h>

unsigned long
get_RAM_size(void)
{
	return 256 << 20;
}

EXPORT_SYMBOL(get_RAM_size);
