/*
 * Flash Base address for BCM97xxx boards
 *
 * Copyright (C) 2002,2003,2004,2005 Broadcom Corporation
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
 * THT 10/22/2002
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/config.h>
#include <asm/delay.h>
#include <asm/brcmstb/common/brcmstb.h>

/*
 * The way the file bcm97xxx.h are defined, it would require a compile
 * time switch in order to determine whether we boot from flash or ROM.
 * This simple QRY test can test for us, as we query the CFE boot address
 * to see if it responds to the CFI 'QRY' test.
 */
 
#ifndef CODE_IN_FLASH
#define CODE_IN_FLASH
#endif

#define WINDOW_ADDR (0x20000000 - BRCM_FLASH_SIZE)

static const unsigned long FLASH_FLASH_BASE = WINDOW_ADDR;
extern const unsigned long ROM_FLASH_BASE;

static unsigned long RT_PHYS_FLASH_BASE = WINDOW_ADDR;

unsigned long 
getPhysFlashBase(void)
{
	return RT_PHYS_FLASH_BASE;
}
EXPORT_SYMBOL(getPhysFlashBase);

#define BOOT_LOADER_ENTRY 0xbfc00000


/*
 * Determine whether CFE was booted from Flash or ROM 
 */
#if defined(CONFIG_BRCM_SKIP_CHECK_BOOTROM)
void determineBootFromFlashOrRom(void)
{
	char msg[64];
	extern int gFlashSize;

       	if (gFlashSize) 
     	RT_PHYS_FLASH_BASE = (0x20000000 - (gFlashSize << 20));

	sprintf(msg, "**********BOOTEDFROMFLASH, Base=%08lx\n", RT_PHYS_FLASH_BASE);
	uart_puts(msg);

	return;
}
#else	/* !CONFIG_BRCM_SKIP_CHECK_BOOTROM */
void determineBootFromFlashOrRom(void)
{
	char msg[128];
	extern int gFlashSize;
	extern int gFlashCode;
	unsigned short   query[3];
	volatile unsigned short * queryaddr;
	int bootFromFlash = 0;

	/* Reset for Spansion flash only */
	if (gFlashCode == 1) {
		*(volatile unsigned short *)(BOOT_LOADER_ENTRY | (0x55 << 1)) = 0xF0;
		udelay(10);
	}
	
	/* Enter query mode x16 */
	*(volatile unsigned short *)(BOOT_LOADER_ENTRY | (0x55 << 1)) = 0x98;
	queryaddr = (volatile unsigned short *)(BOOT_LOADER_ENTRY | (0x10 << 1));

	query[0] = *queryaddr++;
	query[1] = *queryaddr++;
	query[2] = *queryaddr;

	/* Go back to read-array mode */
	*(volatile unsigned short *)(BOOT_LOADER_ENTRY | (0x55 << 1)) = 0xFFFF;

#if (!defined( CONFIG_MIPS_BCM7110 ) || defined( CONFIG_MIPS_BCM7110_DSG))
	if( query[0] == 0x51 &&     /* Q */
	   	query[1] == 0x52 &&     /* R */
	   	query[2] == 0x59  )    /* Y */
	{
		bootFromFlash = 1;
	}
#else
	/*
	 * The 7110 (Not the 7110-DSG) has 2 8bit flash chips that are interleaved.  Rather than using the CFI_probe routine which
	 * does this test taking interleave into account, for all instances and purposes, this should
	 * be enough
	 */
	if( query[0] == 0x5151 &&     /* Q */
	   	query[1] == 0x5252 &&     /* R */
	   	query[2] == 0x5959  )    /* Y */
	{
	   	bootFromFlash = 1;
	}
#endif
	if (bootFromFlash) {	
        	if (!gFlashSize) {
#ifndef CONFIG_MIPS_BCM97438
		RT_PHYS_FLASH_BASE = FLASH_FLASH_BASE;
#else
			RT_PHYS_FLASH_BASE = (0x20000000 - (64 << 20));
#endif
        	}
       	else {
            		RT_PHYS_FLASH_BASE = (0x20000000 - (gFlashSize << 20));
        	}
		sprintf(msg, "**********BOOTEDFROMFLASH, Base=%08lx\n", RT_PHYS_FLASH_BASE);
		uart_puts(msg);
	
	} else {
		RT_PHYS_FLASH_BASE = ROM_FLASH_BASE;
		sprintf(msg, "**********BOOTEDFROMROM, Base=%08lx\n", RT_PHYS_FLASH_BASE);
		uart_puts(msg);
	}
}
#endif /* !CONFIG_BRCM_SKIP_CHECK_BOOTROM */
