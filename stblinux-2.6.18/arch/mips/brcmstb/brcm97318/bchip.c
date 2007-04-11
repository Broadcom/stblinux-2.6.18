/*
 * arch/mips/brcmstb/brcm97318
 *
 * Copyright (C) 2005 Broadcom Corporation
 *                    Richard Y. Hsu<ryhsu@broadcom.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * 7/13/2005 Initial version by Richard Y. hsu
 *
 */
int rac_setting(int);

int rac_setting(int value)
{
	int	rac_value;
	char 	msg[256];

	switch(value) {
		case 0:		/* RAC disabled */
			rac_value = 0x00;		
			break;

		case 1:		/* I-RAC enabled */
			rac_value = 0x94;	
			break;

		case 2:		/* D-RAC enabled */
			rac_value = 0x00;	/* no enabling D-RAC only for 7318. set to default */
			break;

		case 3:		/* I/D-RAC enabled */
			rac_value = 0xD4;	
			break;

		default:
			rac_value = 0x00;	/* unspecified value, set to default */		
			break;
	}

	*((volatile unsigned long *)0xffe0040c) = 0x01;
	while( (*((volatile unsigned long *)0xffe00424) & 0xffff) != 0);
	*((volatile unsigned long *)0xffe0040c) = 0x00;

	*((volatile unsigned long *)0xffe00408) = 0x08000000;

	*((volatile unsigned long *)0xffe00404) = rac_value;

	sprintf(msg, "after init RAC 0x%08x    \n", *((volatile unsigned long *)0xffe00404));
	uart_puts(msg);
}
