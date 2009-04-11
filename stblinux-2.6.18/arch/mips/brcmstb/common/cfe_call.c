/*
 * arch/mips/brcmstb/common/cfe_call.c
 *
 * Copyright (C) 2001-2004 Broadcom Corporation
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
 * Interface between CFE boot loader and Linux Kernel.
 *
 * 
 */
#include <linux/ctype.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <asm/bootinfo.h>

#include "../common/cfe_xiocb.h"
#include <asm/brcmstb/common/cfe_call.h>
#include <asm/brcmstb/common/brcmstb.h>

extern unsigned int cfe_seal;

/*
 * Convert ch from a hex digit to an int
 */
static inline int hex(char ch)
{
	if (ch >= 'a' && ch <= 'f')
		return ch-'a'+10;
	if (ch >= '0' && ch <= '9')
		return ch-'0';
	if (ch >= 'A' && ch <= 'F')
		return ch-'A'+10;
	return -1;
}

static int hex16(char *b)
{
	int d0, d1;

	d0 = hex(b[0]);
	d1 = hex(b[1]);
	if((d0 == -1) || (d1 == -1))
		return(-1);
	return((d0 << 4) | d1);
}

/* NOTE: do not put this on the stack.  It can exceed 3kB. */
static cfe_xiocb_t cfeparam;

int get_cfe_env_variable(char *name_ptr, char *val_ptr, int val_length)
{
	int res = 0;

	cfeparam.xiocb_fcode  = CFE_CMD_ENV_GET;
	cfeparam.xiocb_status = 0;
	cfeparam.xiocb_handle = 0;
	cfeparam.xiocb_flags  = 0;
	cfeparam.xiocb_psize  = sizeof(xiocb_envbuf_t);
	cfeparam.plist.xiocb_envbuf.name_ptr    = (unsigned int)name_ptr;
	cfeparam.plist.xiocb_envbuf.name_length = strlen(name_ptr);
	cfeparam.plist.xiocb_envbuf.val_ptr     = (unsigned int)val_ptr;
	cfeparam.plist.xiocb_envbuf.val_length  = val_length;

	if (cfe_seal == CFE_SEAL) {
		res = cfe_call(&cfeparam);
	}
	else
		res = -1;

	return (res);
}

int get_cfe_hw_info(cfe_xiocb_t* cfe_boardinfo)
{
	int res = -1;
	
	/*
	** Get CFE HW INFO
	*/
	memset(cfe_boardinfo, 0, sizeof(cfe_xiocb_t));
	cfe_boardinfo->xiocb_fcode  = CFE_CMD_GET_BOARD_INFO;
	cfe_boardinfo->xiocb_status = 0;
	cfe_boardinfo->xiocb_handle = 0;
	cfe_boardinfo->xiocb_flags  = 0;
	cfe_boardinfo->xiocb_psize  = sizeof(xiocb_boardinfo_t);

	if (cfe_seal == CFE_SEAL) {
		res = cfe_call(cfe_boardinfo);
	}
	return res;
}

static int parse_eth0_hwaddr(char *buf)
{
	int i, t;
	u8 addr[6];
	extern unsigned char *gHwAddrs[];
	extern int gNumHwAddrs;

	for(i = 0; i < 6; i++) {
		t = hex16(buf);
		if(t == -1)
			return(-1);
		addr[i] = t;
		buf += 3;
	}
	memcpy(&gHwAddrs[0][0], addr, 6);
	gNumHwAddrs = 1;

	return(0);
}

static int parse_dram0_size(char *buf)
{
	extern unsigned long brcm_dram0_size;
	char *endp;
	unsigned long tmp;

	tmp = simple_strtoul(buf, &endp, 0);
	if(*endp == 0) {
		brcm_dram0_size = tmp << 20;
		return(0);
	}
	return(-1);
}

static int parse_dram1_size(char *buf)
{
	extern unsigned long brcm_dram1_size;
	char *endp;
	unsigned long tmp;

	tmp = simple_strtoul(buf, &endp, 0);
	if(*endp == 0) {
		brcm_dram1_size = tmp << 20;
		return(0);
	}
	return(-1);
}

static int parse_boardname(char *buf)
{
#if defined(CONFIG_MIPS_BCM7401) || defined(CONFIG_MIPS_BCM7400) || \
	defined(CONFIG_MIPS_BCM7403) || defined(CONFIG_MIPS_BCM7405)
	/* autodetect 97455, 97456, 97458, 97459 DOCSIS boards */
	if(strncmp("BCM9745", buf, 7) == 0)
		brcm_docsis_platform = 1;
#endif
#if defined(CONFIG_MIPS_BCM7420)
	/* BCM97420CBMB */
	brcm_docsis_platform = 1;
#endif

#if defined(CONFIG_MIPS_BCM7405)
	/* autodetect 97405-MSG board (special MII configuration) */
	if(strstr(buf, "_MSG") != NULL)
		brcm_enet_no_mdio = 1;
#endif
	return(0);
}

static int parse_boot_flags(char *buf)
{
	extern char cfeBootParms[];

	strcpy(cfeBootParms, buf);
	return(0);
}

#define BUF_SIZE		COMMAND_LINE_SIZE
static char cfe_buf[BUF_SIZE];

int get_cfe_boot_parms(void)
{
	int ret;
	int e_ok = 1, d_ok = 1, b_ok = 1, c_ok = 1;

	printk("Fetching vars from bootloader... ");
	if (cfe_seal != CFE_SEAL) {
		printk("none present, using defaults.\n");
		return(-1);
	}

	ret = get_cfe_env_variable("ETH0_HWADDR", cfe_buf, BUF_SIZE);
	if((ret != 0) || (parse_eth0_hwaddr(cfe_buf) != 0))
		e_ok = 0;
	
	ret = get_cfe_env_variable("DRAM0_SIZE", cfe_buf, BUF_SIZE);
	if((ret != 0) || (parse_dram0_size(cfe_buf) != 0))
		d_ok = 0;
	
	ret = get_cfe_env_variable("DRAM1_SIZE", cfe_buf, BUF_SIZE);
	if((ret != 0) || (parse_dram1_size(cfe_buf) != 0))
		d_ok = 0;
	
	ret = get_cfe_env_variable("CFE_BOARDNAME", cfe_buf, BUF_SIZE);
	if((ret != 0) || (parse_boardname(cfe_buf) != 0))
		b_ok = 0;
	
	ret = get_cfe_env_variable("BOOT_FLAGS", cfe_buf, BUF_SIZE);
	if((ret != 0) || (parse_boot_flags(cfe_buf) != 0))
		c_ok = 0;

	if(e_ok || d_ok || b_ok || c_ok) {
		printk("OK (%c,%c,%c,%c)\n",
			e_ok ? 'E' : 'e',
			d_ok ? 'D' : 'd',
			b_ok ? 'B' : 'b',
			c_ok ? 'C' : 'c');
		return(0);
	} else {
		printk("FAILED\n");
		return(-1);
	}
}


cfeEnvVarPairs_t gCfeEnvVarPairs[] = {
	{ "LINUX_FFS_STARTAD", 		"LINUX_FFS_SIZE" },
	{ "SPLASH_PART_STARTAD", 	"SPLASH_PART_SIZE" },
	{ "LINUX_PART_STARTAD", 		"LINUX_PART_SIZE" },
	{ "OCAP_PART_STARTAD", 		"OCAP_PART_SIZE" },
/*
	{ "DRAM0_OFFSET", 			"DRAM0_SIZE" },
	{ "DRAM1_OFFSET", 			"DRAM1_SIZE" },
*/
	{ NULL, NULL },
};

static inline int bcm_atoi(const char *s, int base)
{
	int n;
	int neg = 0;

	n = 0;

	if (base == 10) {
		if (*s == '-') {
			neg = 1;
			s++;
		}
		while (isdigit(*s))
			n = (n * 10) + *s++ - '0';

		if (neg)
			n = 0 - n;
	}
	else if (base == 16) {
		while (*s) {
			int h = hex(*s);

			if (h >= 0) {
				n = (n*16) + h;
				s++;
			}
			else
				break;
		}
	}
	return (n);
}

void __init bcm_get_cfe_partition_env(void)
{
	char envval[128];
	int e, res;
	int numParts = 0;
	unsigned int size;

	for (e=0; gCfeEnvVarPairs[e].offset != NULL; e++) {
		int base = 16;
	
		envval[0] = '\0';

		res = get_cfe_env_variable((char*) gCfeEnvVarPairs[e].size,
				   envval, sizeof(envval));
		if (res == 0 && envval[0] != '\0') {
			/*
			if (e == DRAM0 || e == DRAM1)
				base = 10;
			*/
			
			size = bcm_atoi(envval, base);
			if (size == 0) /* Only size matters :-) */
				continue;
			gCfePartitions.parts[numParts].size = size;
			
			envval[0] =  '\0';
			res = get_cfe_env_variable((char*) gCfeEnvVarPairs[e].offset,
				   envval, sizeof(envval));
			if (res == 0 && envval[0] != '\0') {
				gCfePartitions.parts[numParts].offset = bcm_atoi(envval, 16);
	
			}
			else {
				/* Have size but no offset, its OK for DRAM, as only size matters */
				gCfePartitions.parts[numParts].offset = 0;
			}
			gCfePartitions.parts[numParts].part = e;
			gCfePartitions.numParts = ++numParts;
		}
	}

{
int i;
	for (i=0; i < gCfePartitions.numParts; i++) {
	int p = gCfePartitions.parts[i].part;
	
printk("CFE EnvVar: %s: %08x, %s: %08x\n", 
	gCfeEnvVarPairs[p].offset, gCfePartitions.parts[i].offset,
	gCfeEnvVarPairs[p].size, gCfePartitions.parts[i].size);
}
}
}

