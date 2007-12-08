/*
 * arch/mips/brcmstb/brcm97325a0/bchip.c
 *
 * Copyright (C) 2004-2006 Broadcom Corporation
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
 * when       who    what
 * ----       ---    ----
 * 20060830   THT  Created
 */

#include <linux/init.h>
#include <asm/mipsregs.h>
#include <asm/bug.h>


extern unsigned int par_val2;

#define CORE_REG_BASE_ADDR	0x11f00000

static inline void asm_write(unsigned long l, unsigned long addr)
{
	*((volatile unsigned long*) addr) = l;
}

// jipeng - no need to set up RAC since RAC was merged into L2 cache on 7325A0 (34K MIPS core)
int rac_setting(int value)
{
	return 0;
}

