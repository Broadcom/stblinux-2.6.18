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
#include "cfe_xiocb.h"
#include "asm/brcmstb/common/cfe_call.h"

int get_cfe_boot_parms( char bootParms[] )
{
	/*
	 * This string can be whatever you want, as long
	 * as it is * consistant both within CFE and here.
	 */
	const char* cfe_env = "BOOT_FLAGS";
	cfe_xiocb_t cfeparam;
	int res;
	extern unsigned int firmhandl;
	extern unsigned int firmentry;
	extern unsigned int cfe_seal;
	//extern unsigned int cfe_arg;

	cfeparam.xiocb_fcode  = CFE_CMD_ENV_GET;
	cfeparam.xiocb_status = 0;
	cfeparam.xiocb_handle = 0;
	cfeparam.xiocb_flags  = 0;
	cfeparam.xiocb_psize  = sizeof(xiocb_envbuf_t);
	cfeparam.plist.xiocb_envbuf.name_ptr    = (unsigned int)cfe_env;
	cfeparam.plist.xiocb_envbuf.name_length = strlen(cfe_env);
	cfeparam.plist.xiocb_envbuf.val_ptr     = (unsigned int)bootParms;
	cfeparam.plist.xiocb_envbuf.val_length  = CFE_CMDLINE_BUFLEN;

	//cfe_arg = &cfeparam;
{
char msg[CFE_CMDLINE_BUFLEN+12];
sprintf(msg, "Before: firmhandl=%08x, firmentry=%08x, seal=%08x,bootParmsAddr=%08x\n",
firmhandl, firmentry, cfe_seal, (unsigned int) cfeparam.plist.xiocb_envbuf.val_ptr);
uart_puts(msg);

	if (cfe_seal == CFE_SEAL) {
		res = cfe_call(&cfeparam);
	}
	else {
		uart_puts("Not called from CFE\n");
		res = -1;
	}

	if (res){
		uart_puts( "No arguments presented to boot command\n" );
		//sprintf(msg,"get_cfe_boot_parms: Failed retrieving boot parameters, ret=%d\n", res);
		//uart_puts(msg);
		//sprintf(msg,"After: firmhandl=%08x, firmentry=%08x\n", firmhandl, firmentry);
		//uart_puts(msg);
		res = -1;
	}
	else{
		/* The kernel only takes 256 bytes, but CFE buffer can get up to 1024 bytes */
		if (strlen(bootParms) >= COMMAND_LINE_SIZE) {
			int i;
			sprintf(msg, "Warnings, CFE boot params truncated to %d\n", COMMAND_LINE_SIZE);
			uart_puts(msg);
			for (i=COMMAND_LINE_SIZE-1; i>=0; i--) {
				if (isspace(bootParms[i])) {
					bootParms[i] = '\0';
					break;
				}
			}
		}	
		uart_puts("The cmdline args were:\n");
		sprintf(msg, "@%08x=%s\n", (unsigned int) bootParms,bootParms);
		uart_puts(msg);
		res = 0;
	}
}

	return res;
}



