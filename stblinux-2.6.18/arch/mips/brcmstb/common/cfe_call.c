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

extern unsigned int cfe_seal;

#define ETH_HWADDR_LEN 18	/* 12:45:78:ab:de:01\0 */

/*
 * Convert ch from a hex digit to an int
 */
static inline int hex(unsigned char ch)
{
	if (ch >= 'a' && ch <= 'f')
		return ch-'a'+10;
	if (ch >= '0' && ch <= '9')
		return ch-'0';
	if (ch >= 'A' && ch <= 'F')
		return ch-'A'+10;
	return -1;
}

int get_cfe_env_variable(cfe_xiocb_t *cfeparam,
				void * name_ptr, int name_length,
				void * val_ptr,  int val_length)
{
	int res = 0;

	cfeparam->xiocb_fcode  = CFE_CMD_ENV_GET;
	cfeparam->xiocb_status = 0;
	cfeparam->xiocb_handle = 0;
	cfeparam->xiocb_flags  = 0;
	cfeparam->xiocb_psize  = sizeof(xiocb_envbuf_t);
	cfeparam->plist.xiocb_envbuf.name_ptr    = (unsigned int)name_ptr;
	cfeparam->plist.xiocb_envbuf.name_length = name_length;
	cfeparam->plist.xiocb_envbuf.val_ptr     = (unsigned int)val_ptr;
	cfeparam->plist.xiocb_envbuf.val_length  = val_length;

	if (cfe_seal == CFE_SEAL) {
		res = cfe_call(cfeparam);
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

/* NOTE: do not put this on the stack.  It can exceed 3kB. */
static cfe_xiocb_t cfeparam;

/*
 * ethHwAddrs is an array of 16 uchar arrays, each of length 6, allocated by the caller
 * numAddrs are the actual number of HW addresses used by CFE.
 * For now we only use 1 MAC address for eth0
 */
int get_cfe_boot_parms( char bootParms[], int* numAddrs, unsigned char* ethHwAddrs[] )
{
	/*
	 * This string can be whatever you want, as long
	 * as it is * consistant both within CFE and here.
	 */
	const char* cfe_env = "BOOT_FLAGS";
#ifdef CONFIG_MTD_BRCMNAND
	const char* eth0HwAddr_env = "ETH0_HWADDR";
#endif
	int res;
	char msg[128];

{

    res = get_cfe_env_variable(&cfeparam,
				   (void *)cfe_env,   strlen(cfe_env),
				   (void *)bootParms, CFE_CMDLINE_BUFLEN);

	if (res){
		uart_puts( "No arguments presented to boot command\n" );
		res = -1;
	}
	else{
		/* The kernel only takes 256 bytes, but CFE buffer can get up to 1024 bytes */
		if (strlen(bootParms) >= COMMAND_LINE_SIZE) {
			int i;
			sprintf(msg, "Warnings, CFE boot params truncated to %d bytes\n",
				COMMAND_LINE_SIZE);
			uart_puts(msg);
			for (i=COMMAND_LINE_SIZE-1; i>=0; i--) {
				if (isspace(bootParms[i])) {
					bootParms[i] = '\0';
					break;
				}
			}
		}	
		res = 0;
	}
}

#ifdef CONFIG_MTD_BRCMNAND
if (ethHwAddrs != NULL) {
	unsigned char eth0HwAddr[ETH_HWADDR_LEN];
	int i, j, k;

	*numAddrs = 1;
	  res = get_cfe_env_variable(&cfeparam,
					 (void *)eth0HwAddr_env, strlen(eth0HwAddr_env),
					 (void *)eth0HwAddr,     ETH_HWADDR_LEN*(*numAddrs));
	  if (res)
		  res = -2;
	  else {
	}

	if (res){
		uart_puts( "Ethernet MAC address was not set in CFE\n" );
	
		res = -2;
	}
	else {
		if (strlen(eth0HwAddr) >= ETH_HWADDR_LEN*(*numAddrs)) {
			sprintf(msg, "Warnings, CFE boot params truncated to %d\n", ETH_HWADDR_LEN);
			uart_puts(msg);
			
			eth0HwAddr[ETH_HWADDR_LEN-1] = '\0';
		}	

		/*
		 * Convert to binary format
		 */
		for (k=0; k < *numAddrs; k++) {
			unsigned char* hwAddr = ethHwAddrs[k];
			int done = 0;
			for (i=0,j=0; i<ETH_HWADDR_LEN && !done; ) {
				switch (eth0HwAddr[i]) {
				case ':':
					i++;
					continue;
				
				case '\0':
					done = 1;
					break;

				default:
					hwAddr[j] = (unsigned char)
						((hex(eth0HwAddr[i]) << 4) | hex(eth0HwAddr[i+1]));
					j++;
					i +=2;
				}
			}
		}
		res = 0;
	}
}
#endif

	return res;
}



