/*
 * arch/mips/brcm/prom.c
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
 * Set up kernel arguments and memory. Currently 32MB is reserved
 * for the 70XX and other Broadcom chip drivers.
 *
 * 11-29-2001   SJH    Created
 * 12-23-2005   RYH    Add bcmrac support
 * 05-22-2006   THT    Implement memory hole
 */
#include <linux/config.h>
#include <linux/ctype.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h> // SYM EXPORT */
#include <asm/bootinfo.h>
//#include <asm/mmzone.h>
#include <asm/brcmstb/common/brcmstb.h>


/* RYH */
unsigned int par_val = 0x00;	/* for RAC Mode setting, 0x00-Disabled, 0xD4-I&D Enabled, 0x94-I Only */
unsigned int par_val2 = 0x00;	/* for RAC Cacheable Space setting */

/* Enable SATA2 3Gbps, only works on 65nm chips (7400d0, 7405, 7335) no-op otherwise */
int gSata2_3Gbps = 0;
EXPORT_SYMBOL(gSata2_3Gbps);

/* 97118 normal or RNG */
int bcm7118_boardtype = 0;
EXPORT_SYMBOL(bcm7118_boardtype);

/* Customized flash size in MB */
unsigned int gFlashSize = 0;	/* Default size on 97438 is 64 */
unsigned int gFlashCode = 0; 	/* Special reset codes, 1 for writing 0xF0 to offset 55 for Spansion flash */

/* Clear NAND BBT */
int gClearBBT = 0;
EXPORT_SYMBOL(gClearBBT);

/* Enable Splash partition on flash */
#ifdef CONFIG_MTD_SPLASH_PARTITION
int gBcmSplash = 1;
#else
int gBcmSplash = 0;
#endif
EXPORT_SYMBOL(gBcmSplash);


/* The Chip Select [0..7] for the NAND chips from gNumNand above, only applicable to v1.0+ NAND controller */
#define NAND_MAX_CS    8
static int gNandCS_priv[NAND_MAX_CS+1]; // Num NAND stored in first entry
int* gNandCS;
EXPORT_SYMBOL(gNandCS);
/* Number of NAND chips, only applicable to v1.0+ NAND controller */
int gNumNand = 0;
EXPORT_SYMBOL(gNumNand);

/* SATA interpolation */
int gSataInterpolation = 0;
EXPORT_SYMBOL(gSataInterpolation);

/* 7401Cx revision to decide whether C0 workaround is needed or not */
int bcm7401Cx_rev = 0xFF;
EXPORT_SYMBOL(bcm7401Cx_rev);

/* 7118Ax revision to decide whether A0 workaround is needed or not */
int bcm7118Ax_rev = 0xFF;
EXPORT_SYMBOL(bcm7118Ax_rev);


/* 7403Ax revision to decide whether A0 workaround is needed or not */
int bcm7403Ax_rev = 0xFF;
EXPORT_SYMBOL(bcm7403Ax_rev);

#define MAX_HWADDR	16
#define HW_ADDR_LEN	6

int gNumHwAddrs = 0;
EXPORT_SYMBOL(gNumHwAddrs);

static unsigned char privHwAddrs[MAX_HWADDR][HW_ADDR_LEN];
unsigned char* gHwAddrs[MAX_HWADDR];
EXPORT_SYMBOL(gHwAddrs);

#define FLASH_MACADDR_ADDR 0xBFFFF824

unsigned long g_board_RAM_size = 0;	//Updated by get_RAM_size();
unsigned long g_board_discontig_RAM_size = 0; // updated by __get_discontig_RAM_size()

static unsigned long 
__default_get_discontig_RAM_size(void)
{
	return 0UL;
}

/*
 * 
 * By default will return 0.  It's up to individual boards to
 * override the default, by overloading the function pointer.
 * returns the amount of memory not accounted for by get_RAM_size();
 */
unsigned long (* __get_discontig_RAM_size) (void) = __default_get_discontig_RAM_size;
EXPORT_SYMBOL(__get_discontig_RAM_size);

#if defined( CONFIG_MIPS_BCM7400 ) || defined( CONFIG_MIPS_BCM7325 ) || \
	defined( CONFIG_MIPS_BCM7440 )
#define CONSOLE_KARGS " console=uart,mmio,0x10400b00,115200n8"

#else
#define CONSOLE_KARGS " console=ttyS0,115200"
#endif

#define RW_KARGS " rw"
#define DEFAULT_KARGS CONSOLE_KARGS RW_KARGS

#ifdef CONFIG_MIPS_BRCM97XXX
#include "asm/brcmstb/common/cfe_call.h"	/* CFE interface */

extern void uart_init(unsigned long uClock);
void uart_puts(const char *s);
extern void InitEbi (void);

/* RYH */
extern void set_debug_traps(void);
extern void breakpoint(void);
//cmdEntry_t rootEntry;

char cfeBootParms[CFE_CMDLINE_BUFLEN]; 
#endif
//char arcs_cmdline[COMMAND_LINE_SIZE];



/*
  * Munges cmdArg, and append the console= string if its not there
  */
static void
appendConsoleArg(char* cmdArg)
{
	char* pConsole = NULL;
	char* pRwRo = NULL;
	char c = ' ', *from = cmdArg;
	int len = 0;

	for (;;) {
		if (isspace(c) && (!memcmp(from, "ro", 2) || !memcmp(from, "rw", 2))) {
			pRwRo = from+2;
			/* If found ro or rw, and not part of a string */
			if (*pRwRo == '\0' || isspace(*pRwRo)) 
				break;
			pRwRo = NULL;
		}
		c = *(from++);
		if (!c)
			break;
		if (CL_SIZE <= ++len)
			break;
	}
	if (!pRwRo) {
		if ((strlen(cmdArg) + strlen(RW_KARGS)) < COMMAND_LINE_SIZE) {
			strcat(cmdArg, RW_KARGS);
		}
		else {
			uart_puts("***** WARNINGS: No rw appended to kernel args: args is too long");
		}
	}
	
	pConsole = strstr(cmdArg, "console=");
	if (!pConsole) {
		if ((strlen(cmdArg) + strlen(CONSOLE_KARGS)) < COMMAND_LINE_SIZE) {
			strcat(cmdArg, CONSOLE_KARGS);
		}
		else {
			uart_puts("***** WARNINGS: No console= appended to kernel args: args is too long");
		}
	}
	
	return;
}

static int
isRootSpecified(char* cmdArg)
{

	char c = ' ', *from = cmdArg;
	int len = 0;

	for (;;) {
		if (c == ' ' && !memcmp(from, "root=", 5)) {
			return 1;
		}
		c = *(from++);
		if (!c)
			break;
		if (CL_SIZE <= ++len)
			break;
	}
	
	return 0;
}

void __init prom_init(void)
{

#ifdef CONFIG_MIPS_BRCM97XXX
	int hasCfeParms = 0;
	int res = -1;
	char msg[COMMAND_LINE_SIZE];
	extern void determineBootFromFlashOrRom(void);
#endif

	uart_init(27000000);

	/* jipeng - mask out UPG L2 interrupt here */
	BDEV_WR(BCHP_IRQ0_IRQEN, 0);

	/* Fill in platform information */
	mips_machgroup = MACH_GROUP_BRCM;
	mips_machtype  = MACH_BRCM_STB;

#ifdef CONFIG_MIPS_BCM7118
	/* detect 7118RNG board */
	if( BDEV_RD(BCHP_CLKGEN_REG_START) == 0x1c )
		bcm7118_boardtype = 1;
#endif

#if defined( CONFIG_MIPS_BCM7118 ) || defined( CONFIG_MIPS_BCM7401C0 )	\
 || defined( CONFIG_MIPS_BCM7402C0 ) || defined( CONFIG_MIPS_BCM3563 ) \
 || defined (CONFIG_MIPS_BCM3563C0)
// jipeng - need set bus to async mode before enabling the following	
	if(!(read_c0_diag4() & 0x400000))
	{
		int	val=read_c0_diag4();
		write_c0_diag4(val | 0x400000);
		sprintf(msg, "CP0 reg 22 sel 0 to 5: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", read_c0_diag(), read_c0_diag1(), read_c0_diag2(), read_c0_diag3(), read_c0_diag4(), read_c0_diag5());
		uart_puts(msg);
	}

	// Enable write gathering (BCHP_MISB_BRIDGE_WG_MODE_N_TIMEOUT)
	BDEV_WR(0x0000040c, 0x264);
	// Enable Split Mode (BCHP_MISB_BRIDGE_MISB_SPLIT_MODE)
	BDEV_WR(0x00000410, 0x1);
#elif defined( CONFIG_MIPS_BCM7440A0 )
	if(!(read_c0_diag4() & 0x400000))
	{
		int	val=read_c0_diag4();
		write_c0_diag4(val | 0x400000);
		sprintf(msg, "CP0 reg 22 sel 0 to 5: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", read_c0_diag(), read_c0_diag1(), read_c0_diag2(), read_c0_diag3(), read_c0_diag4(), read_c0_diag5());
		uart_puts(msg);
	}
	
	// Enable write gathering (BCHP_MISB_BRIDGE_WG_MODE_N_TIMEOUT)
	BDEV_WR(0x0000040c, 0x2803);
#endif

	/* Kernel arguments */
#ifdef CONFIG_MIPS_BRCM97XXX
/* For the 97xxx series STB, process CFE boot parms */

  	{	
  		int i;

		for (i=0; i<MAX_HWADDR; i++) {
			gHwAddrs[i] = &privHwAddrs[i][0];
		}
  	}
  
	res = get_cfe_boot_parms(cfeBootParms, &gNumHwAddrs, gHwAddrs);
	if(gNumHwAddrs <= 0) {
#if !defined(CONFIG_BRCM_PCI_SLAVE)
		unsigned int i, mac = FLASH_MACADDR_ADDR, ok = 0;

		for(i = 0; i < 3; i++) {
			u16 word = readw((void *)mac);

			if(word != 0x0000 && word != 0xffff)
				ok = 1;

			gHwAddrs[0][(i << 1)] = word & 0xff;
			gHwAddrs[0][(i << 1) + 1] = word >> 8;
			mac += 2;
		}

		/* display warning for all 00's, all ff's, or multicast */
		if(! ok || (gHwAddrs[0][1] & 1)) {
			printk(KERN_WARNING
				"WARNING: read invalid MAC address "
				"%02x:%02x:%02x:%02x:%02x:%02x from flash @ 0x%08x\n",
				gHwAddrs[0][0], gHwAddrs[0][1], gHwAddrs[0][2],
				gHwAddrs[0][3], gHwAddrs[0][4], gHwAddrs[0][5],
				FLASH_MACADDR_ADDR);
		}
#else
		/* PCI slave mode - no EBI/flash available */
		u8 fixed_macaddr[] = { 0x00, 0xc0, 0xa8, 0x74, 0x3b, 0x51 };

		memcpy(&gHwAddrs[0][0], fixed_macaddr, sizeof(fixed_macaddr));
#endif
		gNumHwAddrs = 1;
	}
  //printk("get_cfe_boot_parms returns %d, strlen(cfeBootParms)=%d\n", res, strlen(cfeBootParms));
	hasCfeParms = ((res == 0 ) || (res == -2));
	// Make sure cfeBootParms is not empty or contains all white space
	if (hasCfeParms) {
		int i;
		
		hasCfeParms = 0;
		for (i=0; i < strlen(cfeBootParms); i++) {
			if (isspace(cfeBootParms[i])) {
				continue;
			}
			else if (cfeBootParms[i] == '\0') {
				break; // and leave hasCfeParms false
			}
			else {
				hasCfeParms = 1;
				break;
			}
		}
	}

	/* Platform specific Init */
#ifdef CONFIG_MIPS_BCM7440B0
	{
		extern int board_get_cfe_env(void);

		(void) board_get_cfe_env();
	}

#endif

	/* RYH - RAC */
	{
	  char	str1[32], str2[32] = "0x";
	  char *cp, *sp;

	  sprintf(msg, "cfeBootParms ===> %s\n", cfeBootParms);
	  uart_puts(msg);
	  cp = strstr( cfeBootParms, "bcmrac=" );
	  if( cp ) { 
		if ( strchr(cfeBootParms, ',') ) {
			for( cp+=strlen("bcmrac="),sp=str1; *cp != ','; *sp++=*cp++ );
			*sp = '\0';
			strcat( str2, ++cp );
			sprintf(msg, "STR1 : %s    STR2 : %s\n", str1, str2);
		        uart_puts(msg);
			sscanf( str1, "%u", &par_val );
			sscanf( str2, "%x", &par_val2 );
			if (par_val2 == 0x00) par_val2 = (get_RAM_size()-1) & 0xffff0000;
		} else {
			sprintf(msg, "RAC Cacheable Space is set to default...\n");
		        uart_puts(msg);
			sscanf( cp+=strlen("bcmrac="), "%i", &par_val );
			par_val2 = (get_RAM_size()-1) & 0xffff0000;
		}
	  }
	  else {
#if defined(CONFIG_MIPS_BCM7400D0) || defined(CONFIG_MIPS_BCM7405) \
    || defined(CONFIG_MIPS_BCM7335) || defined(CONFIG_MIPS_BCM3548)
		par_val = 0xff;		/* default: keep CFE setting */
#elif	!defined(CONFIG_MIPS_BCM7325A0)	/* no RAC in 7325A0 */
		par_val = 0x03;		/* set default to I/D RAC on */
#endif
		par_val2 = (get_RAM_size()-1) & 0xffff0000;
	  }
	}


	/* bcmsata2=1 */
	{
		char c = ' ', *from = cfeBootParms;
		int len = 0;

		for (;;) {
			if (c == ' ' && !memcmp(from, "bcmsata2=", 9)) {
				gSata2_3Gbps= memparse(from + 9, &from);
				break;
			}
			c = *(from++);
			if (!c)
				break;
			if (CL_SIZE <= ++len)
				break;
		}
	}

	/* bcmssc=1, turn on Interpolation for SSC drives, default 0 */
	{
		char c = ' ', *from = cfeBootParms;
		int len = 0;

		for (;;) {
			if (c == ' ' && !memcmp(from, "bcmssc=", 7)) {
				gSataInterpolation= memparse(from + 7, &from);
				break;
			}
			c = *(from++);
			if (!c)
				break;
			if (CL_SIZE <= ++len)
				break;
		}
	}


	/* flashsize=nnM */
	{
		char c = ' ', *from = cfeBootParms;
		int len = 0;

		for (;;) {
			if (c == ' ' && !memcmp(from, "flashsize=", 10)) {
				gFlashSize = memparse(from + 10, &from) >> 20;
				break;
			}
			c = *(from++);
			if (!c)
				break;
			if (CL_SIZE <= ++len)
				break;
		}
	}

	/* flashcode=1 : Write 0xF0 to reset Spansion flash */
	{
		char c = ' ', *from = cfeBootParms;
		int len = 0;

		for (;;) {
			if (c == ' ' && !memcmp(from, "flashcode=", 10)) {
				gFlashCode = memparse(from + 10, &from);
				break;
			}
			c = *(from++);
			if (!c)
				break;
			if (CL_SIZE <= ++len)
				break;
		}
	}

	/* bcmsplash=1 */
	{
		char c = ' ', *from = cfeBootParms;
		int len = 0;

		for (;;) {
			if (c == ' ' && !memcmp(from, "bcmsplash=", 10)) {
				gBcmSplash= memparse(from + 10, &from);
				break;
			}
			c = *(from++);
			if (!c)
				break;
			if (CL_SIZE <= ++len)
				break;
		}
	}

	/* brcmnand=
	 *	rescan: 	1. Rescan for bad blocks, and update existing BBT
	 *	showbbt:	2. Print out the contents of the BBT on boot up.
	 *
	 * The following commands are implemented but should be removed for production builds.  
	 * Use userspace flash_eraseall instead.
	 * These were intended for development debugging only.
	 * 	erase:	7. Erase entire flash, except CFE, and rescan for bad blocks 
	 *	eraseall:	8. Erase entire flash, and rescan for bad blocks
	 *	clearbbt:	9. Erase BBT and rescan for bad blocks.  (DANGEROUS, may lose Mfg's BIs).
	 */

	{
		char c = ' ', *from = cfeBootParms;
		int len = 0;

		for (;;) {
			if (c == ' ' && !memcmp(from, "brcmnand=", 9)) {
				if (!memcmp(from + 9, "rescan", 6)) {
					gClearBBT = 1; // Force BBT rescan, only BBT is erased and rebuilt
				}
				else if (!memcmp(from + 9, "showbbt", 7)) {
					gClearBBT = 2; // Force erase of entire NAND flash, before rescan
				}
#if 1
// Remove this in production build
				// Here we match the longer string first
				else if (!memcmp(from + 9, "eraseall", 8)) {
					gClearBBT = 8; // Force erase of entire NAND flash, before rescan
				}
				else if (!memcmp(from + 9, "erase", 5)) {
					gClearBBT = 7; // Force erase of NAND flash, except CFE, before rescan
				}

				else if (!memcmp(from + 9, "clearbbt", 8)) {
					gClearBBT = 9; // Force erase of BBT, DANGEROUS.
				}
#endif
				break;
			}
			c = *(from++);
			if (!c)
				break;
			if (CL_SIZE <= ++len)
				break;
		}
	}

       /* CS override for BrcmNAND controller */
	{
		char c = ' ', *from = cfeBootParms;
		int len = 0;
               int i;

               gNumNand = 0;
               gNandCS = NULL;
               for (i=0; i<ARRAY_SIZE(gNandCS_priv); i++) {
                       gNandCS_priv[i] = -1;
               }

		for (;;) {
                       if (c == ' ' && !memcmp(from, "nandcs=", 7)) {
                               get_options(from + 7, ARRAY_SIZE(gNandCS_priv), gNandCS_priv);
				break;
			}
			c = *(from++);
			if (!c)
				break;
			if (CL_SIZE <= ++len)
				break;
		}
               if (gNandCS_priv[0] != 0 && gNandCS_priv[0] != -1) {
                       gNumNand = gNandCS_priv[0]; // Num NAND stored in first entry
                       gNandCS = &gNandCS_priv[1];     // Real array starts at element #1
	}

               printk("Number of Nand Chips = %d\n", gNumNand);
       }



	/* Just accept whatever specified in BOOT_FLAGS as kernel options, unless root= is NOT specified */
	if (hasCfeParms && isRootSpecified(cfeBootParms)) {
//sprintf(msg, "after get_cfe_boot_parms, res=0, BootParmAddr=%08x,bootParms=%s\n",
//&cfeBootParms[0],cfeBootParms);
//uart_puts(msg);

		strcpy(arcs_cmdline, cfeBootParms);
		appendConsoleArg(arcs_cmdline);
	}
	else /* The old ways of doing it, as root is not specified on the command line */
		
#endif
	{
	/* Gets here when one of the following is true
	  * - CFE is not the boot loader, or
	  * - CFE is the boot loader, but no option is specified or
	  * - CFE option is specified, but does not say root=
	  */

#ifdef CONFIG_BLK_DEV_INITRD
		/*
		 * tht: Things like init=xxxx wants to be first on command line,
		 * kernel arg parse logic depends on that
		 */
		if (hasCfeParms) {
			strcpy(arcs_cmdline, cfeBootParms);
			strcat(arcs_cmdline, DEFAULT_KARGS);
			hasCfeParms = 0; // Suppress further processing
		}
		else {
			strcpy(arcs_cmdline, DEFAULT_KARGS);
		}

#elif defined(CONFIG_CMDLINE)
		char* p;
		int appendConsoleNeeded = 1;

#ifdef CONFIG_MIPS_BRCM97XXX

		/*
		  * if root= is not on the command line, but user specified something else, 
		  * tag it on.  Some command like init=xxx wants to be first, as kernel
		  * arg parse logic depends on that.
		  */
		if (hasCfeParms && !isRootSpecified(cfeBootParms)) {
			strcpy(arcs_cmdline, cfeBootParms);
			strcat(arcs_cmdline, " ");
			strcat(arcs_cmdline, CONFIG_CMDLINE);
			appendConsoleNeeded = 0;
		}
		else {
			strcpy(arcs_cmdline, CONFIG_CMDLINE);
			appendConsoleNeeded = 0;
		}
#else
		strcpy(arcs_cmdline, CONFIG_CMDLINE);
		appendConsoleNeeded = 0;
#endif
		
		uart_puts("Default command line = \n");
		uart_puts(CONFIG_CMDLINE);
		uart_puts("\n");
		p = &arcs_cmdline[0];
		while (p != NULL && *p != '\0') {
			if (!isspace(*p))
				break;
			p++;
		}
		if (p == NULL || *p == '\0') {
			uart_puts("Defaulting to boot from HD\n");
			/* Default is to boot from HD */
			strcpy(arcs_cmdline,
				"root=/dev/sda1" DEFAULT_KARGS);
		}
		else if (appendConsoleNeeded) {
			/* Make sure that the boot params specify a console */
			appendConsoleArg(arcs_cmdline);
		}
		
#else /* No CONFIG_CMDLINE, and not Initrd */
	/* Default is to boot from HD */
		strcpy(arcs_cmdline,
			"root=/dev/sda1" DEFAULT_KARGS);
#endif /* No CONFIG_CMDLINE */



	} /* End else no root= option is specified */

	uart_puts("Kernel boot options: ");
	uart_puts(arcs_cmdline);
	uart_puts("\r\n");

	// THT: PR21410 Implement memory hole in init_bootmem, now just record the memory size.
	{
		unsigned int ramSizeMB;
#ifdef CONFIG_DISCONTIGMEM
		unsigned long startOfDiscontig = 512<<20; /* 2nd half of DDR0 starts here, physically */
#endif

		g_board_RAM_size = get_RAM_size();
		ramSizeMB = g_board_RAM_size >> 20;


		if (ramSizeMB <= 256) {
			add_memory_region(0, g_board_RAM_size, BOOT_MEM_RAM);
		}
		else {
#if defined (CONFIG_MIPS_BCM7440B0)
			/*
			** On the 7440B0, Memory Region 0
			** is split into a DMA region sized
			** for IDE_WINDOW_END and a second
			** non-DMA mode for the rest of the
			** first memory controller range.
			*/
			add_memory_region(0x00000000, IDE_WINDOW_END, BOOT_MEM_RAM);
			if (bcm_pdiscontig_memmap->memSize[0] > IDE_WINDOW_END)
				add_memory_region(IDE_WINDOW_END, (bcm_pdiscontig_memmap->memSize[0] - IDE_WINDOW_END), BOOT_MEM_RAM);

#else
			/*
			** Limit Zero-based Memory Region 0 to 256MB such
			** that we do not overlap BCHP registers and EBI
			** space (0x10000000 - 0x1fffffff).
			*/
			add_memory_region(0, 256<<20, BOOT_MEM_RAM);
#endif

#if defined(CONFIG_DISCONTIGMEM)
			{
				int ddr = 0;
				unsigned long size;

				if(! bcm_pdiscontig_memmap) {
					add_memory_region(startOfDiscontig,
						(ramSizeMB - 256) << 20, BOOT_MEM_RAM);
				} else {
					/* ddr = 0 */
					size = bcm_pdiscontig_memmap->memSize[0] - (256<<20);
					if (size > 0) {
						add_memory_region(startOfDiscontig, size, BOOT_MEM_RAM);
						startOfDiscontig += size;
					}

					
					/* ddr = 1 or higher */
					for (ddr=1; ddr < bcm_pdiscontig_memmap->nDdrCtrls; ddr++) {
						/* The 2nd half of DDR0 starts at 512MB offset, but some DDR1 starts at 0x60000000 */
						startOfDiscontig = bcm_pdiscontig_memmap->physAddr[ddr];
						size = bcm_pdiscontig_memmap->memSize[ddr];
						add_memory_region(startOfDiscontig, size, BOOT_MEM_RAM);
						startOfDiscontig += size;
					}
				}
			}
#endif
		}
	}
	

#if defined (CONFIG_MIPS_BRCM97XXX) 
	(void) determineBootFromFlashOrRom();
#endif /* if BCM97xxx boards */
//	uart_puts("<--prom_init\r\n");

// jipeng - if it is not in sync mode, set it here
	switch (current_cpu_data.cputype) {
               	case CPU_BMIPS3300:
                    // RYH - BHTD patch 11/15/06
                    {
                        int cp022;

                        cp022 = __read_32bit_c0_register($22, 5);
                        sprintf(msg, "Initial CP0 22 value : 0x%08x\n", cp022);   
                        uart_puts(msg);

                        if ( cp022 & 0x00010000 ) {     // RYH - cp0 reg 22, sel 5, bit[16]
                                cp022 &= 0xfffeffff;
                                __write_32bit_c0_register($22, 5, cp022);

                                cp022 = __read_32bit_c0_register($22, 5);
                                sprintf(msg, "Updated CP0 22 value : 0x%08x\n", cp022);
                                uart_puts(msg);
                        }
                    }
                    if(read_c0_diag4() & 0x400000)
                    {
                        int     val=read_c0_diag4();
                        write_c0_diag4(val & ~0x400000);
                        sprintf(msg, "CP0 reg 22 sel 0 to 5: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
                                        read_c0_diag(),
                                        read_c0_diag1(),
                                        read_c0_diag2(),
                                        read_c0_diag3(),
                                        read_c0_diag4(),
                                        read_c0_diag5());
                        uart_puts(msg);
                    }

                    break;


	       	case CPU_BMIPS4350:
	       	case CPU_BMIPS4380:

                    // RYH - BHTD patch 01/08/07
                    {
                        int cp022;

                        cp022 = __read_32bit_c0_register($22, 0);
                        sprintf(msg, "Initial CP0 22 value : 0x%08x\n", cp022);   
                        uart_puts(msg);

                        if ( cp022 & 0x00200000 ) {     // RYH - cp0 reg22, sel 0, bit[21]
                                cp022 &= 0xffdfffff;
                                __write_32bit_c0_register($22, 0, cp022);

                                cp022 = __read_32bit_c0_register($22, 0);
                                sprintf(msg, "Updated CP0 22 value : 0x%08x\n", cp022);
                                uart_puts(msg);
                        }
                    }


		    if(read_c0_diag4() & 0x400000)
		    {
			int	val=read_c0_diag4();
			write_c0_diag4(val & ~0x400000);
			sprintf(msg, "CP0 reg 22 sel 0 to 5: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
					read_c0_diag(), 
					read_c0_diag1(), 
					read_c0_diag2(), 
					read_c0_diag3(), 
					read_c0_diag4(), 
					read_c0_diag5());
			uart_puts(msg);
		    }
               	    break;

		default:
		    break;
	}	

	if(bcm7401Cx_rev == 0xFF)
	{
		volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;
        	unsigned long chipId = (*pSundryRev) >> 16;		
		
		if(chipId == 0x7401)
		{
			bcm7401Cx_rev = (*pSundryRev) & 0xFF;
			sprintf(msg, "Sundry 0x%08x, chipId 0x%08lx, bcm7401Cx 0x%02x\n",  (int)pSundryRev, chipId, bcm7401Cx_rev);
			uart_puts(msg);
		}
		else if(chipId == 0x7118)
		{
			bcm7118Ax_rev = (*pSundryRev) & 0xFF;
			sprintf(msg, "Sundry 0x%08x, chipId 0x%08lx, bcm7118Ax 0x%02x\n",  (int)pSundryRev, chipId, bcm7118Ax_rev);
			uart_puts(msg);
		}
		else if(chipId == 0x7403)
		{
			bcm7403Ax_rev = (*pSundryRev) & 0xFF;
			sprintf(msg, "Sundry 0x%08x, chipId 0x%08lx, bcm7403Ax %02x\n",  (int)pSundryRev, chipId, bcm7403Ax_rev);
			uart_puts(msg);
		}

	}
}

const char *get_system_type(void)
{
        return "BCM97xxx Settop Platform";
}

void __init prom_free_prom_memory(void) {}
