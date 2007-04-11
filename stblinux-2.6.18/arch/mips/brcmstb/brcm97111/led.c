/*
 * arch/mips/brcmstb/brcm97111/led.c
 *
 * Copyright (C) 2001 Broadcom Corporation
 *                    Steven J. Hill <shill@broadcom.com>
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
 * LED interface for Broadcom eval boards
 *
 * 10-19-2001   SJH    Created
 */
#include <linux/module.h>
#include <asm/io.h>

/*
 * LED port offsets
 */
static unsigned long led_ioport[] = { 0xfffe0097, 0xfffe0096,
					0xfffe0099, 0xfffe0098 };

/*
 * LED segment display variables
 */
static char l4t[] = { 0xfe, 0xfd, 0xfb, 0xf7, 0xe7, 0xd7, 0xc7, 0xbf };
static int l4 = 0;

/*
 * Global LED access variable (0 = kernel, 1 = userspace)
 */
int led_access = 0;
EXPORT_SYMBOL(led_access);


inline void brcm_led_update(unsigned long digit)
{
        if(led_access)
		return;

	/* Check for stupid user */
	if(digit < 1 || digit > 4)
		return;

	outb(l4t[l4], led_ioport[digit-1]);
	l4 = (l4 + 1) & 7;
}

