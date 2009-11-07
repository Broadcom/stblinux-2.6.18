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
#include <linux/spinlock.h>
#include <linux/module.h> // SYM EXPORT */
#include <asm/bootinfo.h>
#include <asm/r4kcache.h>
//#include <asm/mmzone.h>
#include <asm/brcmstb/common/brcmstb.h>


/* RYH */
unsigned int par_val = 0x00;	/* for RAC Mode setting, 0x00-Disabled, 0xD4-I&D Enabled, 0x94-I Only */
unsigned int par_val2 = 0x00;	/* for RAC Cacheable Space setting */

/* to protect accesses to shared registers, e.g. CLKGEN, PIN_MUX */
DEFINE_SPINLOCK(g_magnum_spinlock);
EXPORT_SYMBOL(g_magnum_spinlock);

/* Enable SATA2 3Gbps, only works on 65nm chips (7400d0, 7405, 7335) no-op otherwise */
int gSata2_3Gbps = 0;
EXPORT_SYMBOL(gSata2_3Gbps);

/* SATA/ENET can be disabled through bond options */
int brcm_sata_enabled = 0;
EXPORT_SYMBOL(brcm_sata_enabled);

int brcm_enet_enabled = 0;
int brcm_enet_no_mdio = 0;
int brcm_emac_1_enabled = 0;
EXPORT_SYMBOL(brcm_enet_enabled);
EXPORT_SYMBOL(brcm_enet_no_mdio);
EXPORT_SYMBOL(brcm_emac_1_enabled);

int brcm_pci_enabled = 0;
EXPORT_SYMBOL(brcm_pci_enabled);

int brcm_smp_enabled = 0;
EXPORT_SYMBOL(brcm_smp_enabled);

/* DOCSIS platform (e.g. 97455) */
int brcm_docsis_platform = 0;
EXPORT_SYMBOL(brcm_docsis_platform);

/* EBI bug workaround */
int brcm_ebi_war = 0;
EXPORT_SYMBOL(brcm_ebi_war);

/* Customized flash size in MB */
unsigned int gFlashSize = 0;	/* Default size on 97438 is 64 */
unsigned int gFlashCode = 0; 	/* Special reset codes, 1 for writing 0xF0 to offset 55 for Spansion flash */

/* Clear NAND BBT */
int gClearBBT = 0;
EXPORT_SYMBOL(gClearBBT);

#ifdef CONFIG_MTD_BRCMNAND_CORRECTABLE_ERR_HANDLING
/* Argument for NAND CET */
char gClearCET = 0;
EXPORT_SYMBOL(gClearCET);
#endif /* CONFIG_MTD_BRCMNAND_CORRECTABLE_ERR_HANDLING */

/* Enable Splash partition on flash */
#ifdef CONFIG_MTD_SPLASH_PARTITION
int gBcmSplash = 1;
#else
int gBcmSplash = 0;
#endif
EXPORT_SYMBOL(gBcmSplash);

/*
 * Support for CFE defined env vars for NAND flash partition
 */

cfePartitions_t gCfePartitions;
EXPORT_SYMBOL(gCfePartitions);


/* The Chip Select [0..7] for the NAND chips from gNumNand above, only applicable to v1.0+ NAND controller */
#define NAND_MAX_CS    8
static int gNandCS_priv[NAND_MAX_CS+1]; // Num NAND stored in first entry
int* gNandCS;
EXPORT_SYMBOL(gNandCS);
/* Number of NAND chips, only applicable to v1.0+ NAND controller */
int gNumNand = 0;
EXPORT_SYMBOL(gNumNand);

/*
 * NAND controller timing parameters
 */
uint32_t gNandTiming1;
uint32_t gNandTiming2;
EXPORT_SYMBOL(gNandTiming1);
EXPORT_SYMBOL(gNandTiming2);

/* SATA interpolation */
int gSataInterpolation = 0;
EXPORT_SYMBOL(gSataInterpolation);

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

#define CONSOLE_KARGS " console=ttyS0,115200"

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

unsigned long brcm_dram0_size = 0;
unsigned long brcm_dram1_size = 0;
#endif
//char arcs_cmdline[COMMAND_LINE_SIZE];

#ifndef BRCM_MEMORY_STRAPS

#define MAGIC0		0xdeadbeef
#define MAGIC1		0xfeedcafe

static unsigned long
probe_memsize(void)
{
#if !defined(CONFIG_MIPS_BRCM_SIM)
	volatile uint32_t *addr = (volatile void *)KSEG1, *taddr;
	uint32_t olddata;
	long flags;
	unsigned int i, memsize = 256;

	printk("Probing system memory size... ");

	local_irq_save(flags);
	cache_op(Index_Writeback_Inv_D, KSEG0);
	olddata = *addr;

	/*
	 * Try to figure out where memory wraps around.  If it does not
	 * wrap, assume 256MB
 	*/
	for(i = 4; i <= 128; i <<= 1) {
		taddr = (volatile void *)(KSEG1 + (i * 1048576));
		*addr = MAGIC0;
		if(*taddr == MAGIC0) {
			*addr = MAGIC1;
			if(*taddr == MAGIC1) {
				memsize = i;
				break;
			}
		}
	}

	*addr = olddata;
	cache_op(Index_Writeback_Inv_D, KSEG0);
	local_irq_restore(flags);

	printk("found %u MB\n", memsize);

	return(memsize * 1048576);
#else
	/* Use fixed memory size (32MB) for simulation runs */
	return(32 * 1048576);
#endif
}

unsigned long
get_RAM_size(void)
{
	BUG_ON(! brcm_dram0_size);
	return(brcm_dram0_size);
}

EXPORT_SYMBOL(get_RAM_size);

#endif /* BRCM_MEMORY_STRAPS */

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

static void early_read_int(char *param, int *var)
{
	char *cp = strstr(cfeBootParms, param);

	if(cp == NULL)
		return;
	cp += strlen(param);
	*var = memparse(cp, &cp);
}

static void early_read_hex(char *param, uint32_t *var)
{
	char *cp = strstr(cfeBootParms, param);

	if(cp == NULL)
		return;
	cp += strlen(param);
	*var = simple_strtoul(cp, &cp, 16);
}

#define PINMUX(reg, field, val) do { \
	BDEV_WR(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_##reg, \
		(BDEV_RD(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_##reg) & \
		 ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_##reg##_##field##_MASK) | \
		((val) << \
		 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_##reg##_##field##_SHIFT)); \
	} while(0)

static void __init board_pinmux_setup(void)
{
#if ! defined(CONFIG_MIPS_BRCM_SIM)
#if   defined(CONFIG_MIPS_BCM7420)
	PINMUX(7, gpio_000, 1);		// ENET LEDs
	PINMUX(7, gpio_001, 1);

	PINMUX(21, sgpio_02, 1);	// MoCA I2C
	PINMUX(21, sgpio_03, 1);

	PINMUX(9, gpio_017, 1);		// MoCA LEDs
	PINMUX(9, gpio_019, 1);

	PINMUX(7, gpio_002, 1);		// RGMII
	PINMUX(7, gpio_003, 1);
	PINMUX(7, gpio_004, 1);
	PINMUX(7, gpio_005, 1);
	PINMUX(7, gpio_006, 1);
	PINMUX(7, gpio_007, 1);
	PINMUX(8, gpio_009, 1);
	PINMUX(8, gpio_010, 1);
	PINMUX(8, gpio_011, 1);
	PINMUX(8, gpio_012, 1);
	PINMUX(8, gpio_013, 1);
	PINMUX(8, gpio_014, 1);
	PINMUX(20, gpio_108, 4);	/*RGMII SDC/SDL */
	PINMUX(20, gpio_109, 5);

	/* set RGMII lines to 2.5V */
#ifdef CONFIG_MIPS_BCM7420A0
	BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, pad_mode_gpio_002, 1);
	BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, pad_mode_gpio_003, 1);
	BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, pad_mode_gpio_004, 1);
	BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, pad_mode_gpio_005, 1);
	BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, pad_mode_gpio_006, 1);
	BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, pad_mode_gpio_007, 1);
	BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, pad_mode_gpio_009, 1);
	BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, pad_mode_gpio_010, 1);
	BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, pad_mode_gpio_011, 1);
	BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, pad_mode_gpio_012, 1);
	BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, pad_mode_gpio_013, 1);
	BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, pad_mode_gpio_014, 1);
#else
	BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, rgmii_pad_mode, 1);
#endif /* CONFIG_MIPS_BCM7420A0 */
#endif /* CONFIG_MIPS_BCM7420 */
#endif /* ! CONFIG_MIPS_BRCM_SIM */
}


cfePartitions_t gCfePartitions; 

void __init prom_init(void)
{

#ifdef CONFIG_MIPS_BRCM97XXX
	int hasCfeParms = 0;
	int res = -1;
	extern void determineBootFromFlashOrRom(void);
#endif

	uart_init(27000000);

	/* jipeng - mask out UPG L2 interrupt here */
	BDEV_WR(BCHP_IRQ0_IRQEN, 0);

	board_pinmux_setup();

	/* Fill in platform information */
	mips_machgroup = MACH_GROUP_BRCM;
	mips_machtype  = MACH_BRCM_STB;

#ifdef BRCM_SATA_SUPPORTED
	brcm_sata_enabled = 1;
#endif

#ifdef BRCM_ENET_SUPPORTED
	brcm_enet_enabled = 1;
#endif

#ifdef BRCM_EMAC_1_SUPPORTED
	brcm_emac_1_enabled = 1;
#endif

#ifdef BRCM_PCI_SUPPORTED
	brcm_pci_enabled = 1;
#endif

#ifdef CONFIG_SMP
	brcm_smp_enabled = 1;
#endif

#ifdef CONFIG_MIPS_BCM7118
	/* detect 7118RNG board */
	if( BDEV_RD(BCHP_CLKGEN_REG_START) == 0x1c )
		brcm_sata_enabled = 0;
	/* onchip DOCSIS owns the ENET */
	brcm_enet_enabled = 0;
#endif

#ifdef CONFIG_MIPS_BCM7405
	/* detect 7406 */
	if(BDEV_RD(BCHP_SUN_TOP_CTRL_OTP_OPTION_STATUS) &
		BCHP_SUN_TOP_CTRL_OTP_OPTION_STATUS_otp_option_sata_disable_MASK)
		brcm_sata_enabled = 0;
	switch(BDEV_RD(BCHP_SUN_TOP_CTRL_OTP_OPTION_STATUS) & 0xf) {
		case 0x0:
			/* 7405/7406 */
			break;
		case 0x1:
			/* 7466 */
			brcm_pci_enabled = 0;
			brcm_emac_1_enabled = 0;
			break;
		case 0x3:
			/* 7106 */
			brcm_emac_1_enabled = 0;
			brcm_smp_enabled = 0;
			break;
		case 0x4:
		case 0x6:
			/* 7205/7213 */
			brcm_emac_1_enabled = 0;
			break;
	}
#endif
	
#if defined(CONFIG_BMIPS3300)
	// Set BIU to async mode
	set_c0_brcm_bus_pll(1 << 22);
	// Enable write gathering (BCHP_MISB_BRIDGE_WG_MODE_N_TIMEOUT)
	BDEV_WR(0x0000040c, 0x264);
	// Enable Split Mode (BCHP_MISB_BRIDGE_MISB_SPLIT_MODE)
	BDEV_WR(0x00000410, 0x1);
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
  
	res = get_cfe_boot_parms();
	hasCfeParms = (res == 0);

#ifdef BRCM_MEMORY_STRAPS
	get_RAM_size();
#else
	if(brcm_dram0_size == 0)
		brcm_dram0_size = probe_memsize();
#ifndef CONFIG_DISCONTIGMEM
	if(brcm_dram0_size > (256 << 20)) {
		printk("Extra RAM beyond 256MB ignored.  Please "
			"use a kernel that supports DISCONTIG.\n");
		brcm_dram0_size = 256 << 20;
	}
#endif /* CONFIG_DISCONTIGMEM */
#endif /* BRCM_MEMORY_STRAPS */

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
		if(! ok || (gHwAddrs[0][0] & 1)) {
			u8 fixed_macaddr[] = { 0x00,0x00,0xde,0xad,0xbe,0xef };
			printk(KERN_WARNING
				"WARNING: read invalid MAC address "
				"%02x:%02x:%02x:%02x:%02x:%02x from flash @ 0x%08x\n",
				gHwAddrs[0][0], gHwAddrs[0][1], gHwAddrs[0][2],
				gHwAddrs[0][3], gHwAddrs[0][4], gHwAddrs[0][5],
				FLASH_MACADDR_ADDR);
			memcpy(&gHwAddrs[0][0], fixed_macaddr,
				sizeof(fixed_macaddr));
		}
#else
		/* PCI slave mode - no EBI/flash available */
		u8 fixed_macaddr[] = { 0x00, 0xc0, 0xa8, 0x74, 0x3b, 0x51 };

		memcpy(&gHwAddrs[0][0], fixed_macaddr, sizeof(fixed_macaddr));
#endif
		gNumHwAddrs = 1;
	}
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

#else


	bcm_get_cfe_partition_env();



#endif

	/* RYH - RAC */
	{
	  char	str1[32], str2[32] = "0x";
	  char *cp, *sp;

	  cp = strstr( cfeBootParms, "bcmrac=" );
	  if( cp ) { 
		if ( strchr(cfeBootParms, ',') ) {
			for( cp+=strlen("bcmrac="),sp=str1; *cp != ','; *sp++=*cp++ );
			*sp = '\0';
			strcat( str2, ++cp );
			sscanf( str1, "%u", &par_val );
			sscanf( str2, "%x", &par_val2 );
			if (par_val2 == 0x00) par_val2 = (get_RAM_size()-1) & 0xffff0000;
		} else {
			sscanf( cp+=strlen("bcmrac="), "%i", &par_val );
			par_val2 = (get_RAM_size()-1) & 0xffff0000;
		}
	  }
	  else {
#if defined(CONFIG_BMIPS4380) || defined(CONFIG_BMIPS5000)
		par_val = 0xff;		/* default: keep CFE setting */
#elif !defined(CONFIG_MIPS_BCM7325B0)	/* no RAC in 7325B0 */
		par_val = 0x03;		/* set default to I/D RAC on */
#endif
		par_val2 = (get_RAM_size()-1) & 0xffff0000;
	  }
	}

	early_read_int("bcmsata2=", &gSata2_3Gbps);
	early_read_int("bcmssc=", &gSataInterpolation);
	early_read_int("flashsize=", &gFlashSize);
	early_read_int("flashcode=", &gFlashCode);
	early_read_int("bcmsplash=", &gBcmSplash);

	/* NAND Timings, in Hex */
	early_read_hex("nandTiming1=", &gNandTiming1);
	early_read_hex("nandTiming2=", &gNandTiming2);

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
	 *      showcet:       10. Display the correctable error count
	 *      resetcet:      11. Reset the correctable error count to 0 and table to all 0xff
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
#ifdef CONFIG_MTD_BRCMNAND_CORRECTABLE_ERR_HANDLING
				else if (!memcmp(from + 9, "showcet", 7)) {
					gClearCET = 1;
				}
				else if (!memcmp(from + 9, "resetcet", 8)) {
					gClearCET = 2;
				} 
				else if (!memcmp(from + 9, "disablecet", 10)) {
					gClearCET = 3;
				}
#endif /* CONFIG_MTD_BRCMNAND_CORRECTABLE_ERR_HANDLING */
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

               //printk("Number of Nand Chips = %d\n", gNumNand);
       }



	/* Just accept whatever specified in BOOT_FLAGS as kernel options, unless root= is NOT specified */
	if (hasCfeParms && isRootSpecified(cfeBootParms)) {
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

	//uart_puts("Kernel boot options: ");
	//uart_puts(arcs_cmdline);
	//uart_puts("\r\n");

	// THT: PR21410 Implement memory hole in init_bootmem, now just record the memory size.
	{
		unsigned int ramSizeMB;
#ifdef CONFIG_DISCONTIGMEM
		unsigned long startOfDiscontig = 512<<20; /* 2nd half of DDR0 starts here, physically */
#endif

		g_board_RAM_size = get_RAM_size();
		ramSizeMB = g_board_RAM_size >> 20;

printk("g_board_RAM_size=%dMB\n", ramSizeMB);

		if (ramSizeMB <= 256) {
			add_memory_region(0, g_board_RAM_size, BOOT_MEM_RAM);
		}
		else {
#if defined (CONFIG_MIPS_BCM7440B0) || defined(CONFIG_MIPS_BCM7601) || defined(CONFIG_MIPS_BCM7635)
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

#if defined(CONFIG_BMIPS3300)
	// clear BHTD to enable branch history table
	clear_c0_brcm_reset(1 << 16);

	// put the BIU back in sync mode
	clear_c0_brcm_bus_pll(1 << 22);
#elif defined(CONFIG_BMIPS4380)
	// clear BHTD to enable branch history table
	clear_c0_brcm_config_0(1 << 21);
#endif

	/* detect chips with EBI bug */
#if defined(CONFIG_MIPS_BCM7401) || defined(CONFIG_MIPS_BCM7403)
	if((BDEV_RD(BCHP_SUN_TOP_CTRL_PROD_REVISION) & 0xffff) == 0x20)
		brcm_ebi_war = 1;
#elif defined(CONFIG_MIPS_BCM7118)
	if((BDEV_RD(BCHP_SUN_TOP_CTRL_PROD_REVISION) & 0xffff) == 0x00)
		brcm_ebi_war = 1;
#endif

	printk("Options: sata=%d enet=%d emac_1=%d no_mdio=%d docsis=%d "
		"ebi_war=%d pci=%d smp=%d\n",
		brcm_sata_enabled, brcm_enet_enabled, brcm_emac_1_enabled,
		brcm_enet_no_mdio, brcm_docsis_platform, brcm_ebi_war,
		brcm_pci_enabled, brcm_smp_enabled);
}

const char *get_system_type(void)
{
        return "BCM97xxx Settop Platform";
}

void __init prom_free_prom_memory(void) {}
