/*
 * arch/mips/brcmstb/brcm97318/board.c
 *
 * Copyright (C) 2004-2005 Broadcom Corporation
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
 */

#include <linux/config.h>

// For module exports
#define EXPORT_SYMTAB
#include <linux/module.h>

#include <asm/brcmstb/common/brcmstb.h>

#ifndef DRAM_SIZE
#define DRAM_SIZE	(32 << 20)
#endif

unsigned long
get_RAM_size(void)
{
	return DRAM_SIZE;
}

EXPORT_SYMBOL(get_RAM_size);
