/*
 * arch/mips/brcmstb/brcm97038b0
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
int rac_setting(int);
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
			rac_value = 0x93;	/* 0x10010011 - prefetch on, instruction cache enabled(default) */	
			break;

		case 2:		/* D-RAC enabled */
			rac_value = 0xD3;	/* 0x11010011 - prefetch on, data cache enabled */	
			break;

		case 3:		/* I/D-RAC enabled */
			rac_value = 0xD3;	
			break;

		case 10:		/* D-RAC enabled + write buffer enabled */
			rac_value = 0xDB;	/* 0x11011011 - prefetch on, data cache enabled */	
			break;

		case 11:		/* I/D-RAC enabled + write buffer enabled */
			rac_value = 0xDB;	/* 0x11011011 - prefetch on, data cache enabled */	
			break;

		default:
			rac_value = 0x00;	/* unspecified value, set to default */		
			sprintf(msg, "Invalid input for 7038b0 RAC mode setting. Default setting(RAC disabled) applied.\n");
			uart_puts(msg);
			break;
	}

	*((volatile unsigned long *)0xb000040c) = 0x01;
	while( (*((volatile unsigned long *)0xb0000424) & 0xffff) != 0);
	*((volatile unsigned long *)0xb000040c) = 0x00;

	*((volatile unsigned long *)0xb0000408) = par_val2;		/* 0x08000000; 128M for 7038c0 */

	*((volatile unsigned long *)0xb0000404) = rac_value;

	sprintf(msg, "after init RAC 0x%08x    0x%08x\n", *((volatile unsigned long *)0xb0000404), 
			*((volatile unsigned long *)0xb0000408) );
	uart_puts(msg);
}
