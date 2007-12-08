/*
 * arch/mips/brcmstb/brcm97038c0
 *
 * Copyright (C) 2005 Broadcom Corporation
 *                    Richard Y. Hsu<ryhsu@broadcom.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * 7/13/2005 Initial version by Richard Y. hsu
 * 12/01/2005 Enhanced bcmrac support by Richard Y. hsu
 *
 */

#include <linux/kernel.h>
#include <asm/brcmstb/common/brcmstb.h>

int rac_setting(int);
extern void bcm_inv_rac_all(void);

extern int par_val2;

int rac_setting(int value)
{
	int	rac_value;
	char 	msg[256];

	switch(value) {
		case 0:		/* RAC disabled */
			rac_value = 0x00;
			break;

		case 1:		/* I-RAC enabled */
			bcm_inv_rac_all();
			rac_value = 0x93;	/* 0x10010011 - prefetch on, instruction cache enabled(default) */	
			break;

		case 2:		/* D-RAC enabled */
			bcm_inv_rac_all();
			rac_value = 0xD3;	/* 0x11010011 - prefetch on, data cache enabled */
			break;

		case 3:		/* I/D-RAC enabled */
			bcm_inv_rac_all();
			rac_value = 0xD3;
			break;

		case 10:		/* D-RAC enabled + write buffer enabled */
			bcm_inv_rac_all();
			rac_value = 0xDB;	/* 0x11011011 - prefetch on, data cache enabled */
			break;

		case 11:		/* I/D-RAC enabled + write buffer enabled */
			bcm_inv_rac_all();
			rac_value = 0xDB;	/* 0x11011011 - prefetch on, data cache enabled */
			break;

		default:
			rac_value = 0x00;	/* unspecified value, set to default */		
			sprintf(msg, "Invalid input for 7038c0 RAC mode setting. Default setting(RAC disabled) applied.\n");
			uart_puts(msg);
			break;
	}

	*((volatile unsigned long *)0xb000040c) = 0x01;
	while( (*((volatile unsigned long *)0xb0000424) & 0xffff) != 0);
	*((volatile unsigned long *)0xb000040c) = 0x00;

	*((volatile unsigned long *)0xb0000408) = par_val2;		/* 0x08000000; 128M for 7038c0 */

	*((volatile unsigned long *)0xb0000404) = rac_value;

	sprintf(msg, "after init RAC 0x%08lx    0x%08lx\n",
		*((volatile unsigned long *)0xb0000404), 
		*((volatile unsigned long *)0xb0000408) );
	uart_puts(msg);

	return(0);
}
