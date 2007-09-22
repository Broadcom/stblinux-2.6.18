/*
 * arch/mips/brcmstb/brcm97405a0/bchip.c
 *
 * Copyright (C) 2004-2006 Broadcom Corporation
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
 *
 * Board Specific routines for Broadcom eval boards
 *
 * when       who    what
 * ----       ---    ----
 * 06-09-2006 RYH    Full implementation of 7405a0 RAC
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/mipsregs.h>
#include <asm/bug.h>

static __init int RAC_init(void);
void uart_puts(const char *);
extern asmlinkage void _RAC_init(void);

#if 1
/*
 * THT 2/09/06: Initializes the core address space, so that RAC __AND__ the flash will work.
 * We must call this very early, unless the CFE has already done it, even though we don't enable RAC
 * Failing to do so will result in a bus error when accessing the flash around 0x01a4_0000 offset.
 */
static int gRAC_init_called;
static __init int RAC_init(void)
{
	volatile unsigned long cba;

	if (gRAC_init_called)
		return 1;

	gRAC_init_called = 1;
	cba = __read_32bit_c0_register($22, 6);
	printk("======> Before RAC_init:$22s5=%08x, $22s6(CBA)=%08lx \n", __read_32bit_c0_register($22, 5), cba);
	if (cba == 0x11f0000c) {
		printk("@B1F0_001C=%08lx, @B1F0_0004=%08lx, @B1F0_0000=%08lx, @B1F0_0008=%08lx\n",
			*((volatile unsigned long*) 0xB1F0001C), 
			*((volatile unsigned long*) 0xB1F00004),
			*((volatile unsigned long*) 0xB1F00000),
			*((volatile unsigned long*) 0xB1F00008)
			);
	}
	else {
		printk("Invalid value for CBA. Please update your bootloader\n");
		//BUG(); _RAC_init will straighten things out.
	}

#if 0 // LLMB setting is inherited from CFE
    _RAC_init();
#endif
	
	return 0;
}

core_initcall(RAC_init);

#endif


extern unsigned int par_val2;
extern void bcm_inv_rac_all(void);

#define	RAC_CONFIGURATION0_REGISTER	0x00000000
#define	RAC_CONFIGURATION1_REGISTER	0x00000008
#define	RAC_ADDRESS_RANGE_REGISTER	0x00000004

#define DEFAULT_RAC_CONFIGURATION	0x00087000	/* WRV = '10'b, PGMSZ = '111'b */

extern unsigned long rac_config0, rac_config1, rac_address_range;

int rac_setting(int value)
{
	unsigned long	rac_value;
	char 	msg[256];
	unsigned long cba=__read_32bit_c0_register($22, 6);

	if (!gRAC_init_called) {
		RAC_init();
	}
	cba &= 0xFFFC0000;

	rac_config0 = cba + 0xA0000000 + RAC_CONFIGURATION0_REGISTER;
	rac_config1 = cba + 0xA0000000 + RAC_CONFIGURATION1_REGISTER;
	rac_address_range = cba + 0xA0000000 + RAC_ADDRESS_RANGE_REGISTER;

	printk("CBA = %08lx    VALUE = %d    PAR_VAL2 = %08x\n", cba, value, par_val2); 
	printk("RAC0 = %08lx    RAC1 = %lx    RAC_RANGE = %08lx\n", rac_config0, rac_config1, rac_address_range); 

	sprintf(msg, "before init RAC 0x%08lx    0x%08lx    0x%08lx\n", 
			*((volatile unsigned long *)(rac_config0)), 
			*((volatile unsigned long *)(rac_config1)), 
			*((volatile unsigned long *)(rac_address_range)));
	uart_puts(msg);


	switch(value) {
		case 0:		/* RAC disabled, PF_D='0'b, PF_I='0'b, RAC_D='0'b, RAC_I='0'b */
			rac_value = 0x00000000;		
			break;

		case 1:		/* I-RAC enabled, C_INV='1'b, PF_D='0'b, PF_I='1'b, RAC_D='0'b, RAC_I='1'b */
			bcm_inv_rac_all();
			rac_value = 0x00000015;
			break;

		case 2:		/* D-RAC enabled, C_INV='1'b, PF_D='1'b, PF_I='0'b, RAC_D='1'b, RAC_I='0'b */
			bcm_inv_rac_all();
			rac_value = 0x0000001A;
			break;

		case 3:		/* I/D-RAC enabled, PF_D='1'b, PF_I='1'b, RAC_D='1'b, RAC_I='1'b */
			bcm_inv_rac_all();
			rac_value = 0x0000000F;
			break;

		case 11:    /* I/D-RAC enabled, C_INV='1'b, PF_D='1'b, PF_I='1'b, RAC_D='1'b, RAC_I='1'b */
			bcm_inv_rac_all();
			rac_value = 0x0000001F;
			break;

		case 0xff:    /* leave CFE value intact */
			bcm_inv_rac_all();
			rac_value = 0x00000000;
			break;

		default:	/* unspecified value, set to default */
			bcm_inv_rac_all();
			rac_value = 0x0000001F;			
			sprintf(msg, "Invalid input for 7401a0 RAC mode setting. Default setting(RAC disabled) applied.\n");
			uart_puts(msg);
			break;
	}

	rac_value |= DEFAULT_RAC_CONFIGURATION;
	printk("RAC_VALUE = %08lx\n", rac_value);

	if(value != 0xff) {
		*((volatile unsigned long *)(rac_config0)) =
			(*((volatile unsigned long *)(rac_config0)) & ~0x1f) |
			rac_value;
		*((volatile unsigned long *)(rac_config1)) =
			(*((volatile unsigned long *)(rac_config1)) & ~0x1f) |
			rac_value;
		*((volatile unsigned long *)(rac_address_range)) = par_val2;  /* 0x04000000; 64M for 7401a0 */
	} else {
		printk("******* Using CFE setting for RAC\n");
	}

	// clear WRV (space reservation by way - set to unreserved)
	*((volatile unsigned long *)(rac_config0)) &= ~(3 << 18);
	// set DPF (directional prefetching)
	*((volatile unsigned long *)(rac_config0)) |= (1 << 6);

	sprintf(msg, "after init RAC 0x%08lx    0x%08lx    0x%08lx\n", 
			*((volatile unsigned long *)(rac_config0)), 
			*((volatile unsigned long *)(rac_config1)), 
			*((volatile unsigned long *)(rac_address_range)));

	uart_puts(msg);
	
	printk("******* $22s0=%08x,    $22s5=%08x,    $22s6=%08x\n", 
		__read_32bit_c0_register($22, 0),
		__read_32bit_c0_register($22, 5),
	       	__read_32bit_c0_register($22, 6));


	return 0;
}



