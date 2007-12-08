/*
 * arch/mips/brcm/setup.c
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
 * Setup for Broadcom eval boards
 *
 * 10-23-2001   SJH    Created
 */
#include <linux/config.h>


// For module exports
#define EXPORT_SYMTAB
#include <linux/module.h>

#include <linux/console.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/addrspace.h>
#include <asm/irq.h>
#include <asm/reboot.h>
#include <asm/time.h>
#include <asm/delay.h>

#ifdef CONFIG_SMP
#include <linux/spinlock.h>
#include <asm/cpu-features.h>
#include <asm/war.h>

spinlock_t atomic_lock;
/* PR21653 */
EXPORT_SYMBOL(atomic_lock);
#endif

#if defined(CONFIG_BLK_DEV_IDE) || defined(CONFIG_BLK_DEV_IDE_MODULE)
#include <linux/ide.h>

#endif
#ifdef CONFIG_PC_KEYB
#include <asm/keyboard.h>
extern struct kbd_ops brcm_kbd_ops;
#endif

#include <asm/io.h>

#include "asm/brcmstb/common/brcmstb.h"

#ifdef CONFIG_MTD_BRCMNAND 
#include <linux/mtd/brcmnand.h>
#endif

extern void uart_puts(const char*);
extern void brcm_numa_init(void);

void __init bus_error_init(void) { /* nothing */ }


static void brcm_machine_restart(char *command)
{
	static void (*back_to_prom)(void) = (void (*)(void)) 0xbfc00000;


#if defined( CONFIG_MIPS_BCM7401 ) || defined( CONFIG_MIPS_BCM7400 ) \
	|| defined( CONFIG_MIPS_BCM3560 ) || defined( CONFIG_MIPS_BCM7038C0 ) \
	|| defined( CONFIG_MIPS_BCM7402 ) || defined( CONFIG_MIPS_BCM7402S ) \
	|| defined( CONFIG_MIPS_BCM7118 ) || defined( CONFIG_MIPS_BCM7440 )  \
        || defined( CONFIG_MIPS_BCM7403 ) || defined( CONFIG_MIPS_BCM7452 ) \
	|| defined( CONFIG_MIPS_BCM7405 )
#define SUN_TOP_CTRL_RESET_CTRL		0xb0404008
#define MASTER_RESET_ENABLE 		(1<<3)
  
  #if defined( CONFIG_MIPS_BCM7401A0 ) || defined( CONFIG_MIPS_BCM3560 ) \
 	|| defined( CONFIG_MIPS_BCM7038C0 ) || defined( CONFIG_MIPS_BCM7402S )
  #define SUN_TOP_CTRL_SW_RESET		0xb0404010
  
  #elif defined( CONFIG_MIPS_BCM7400 ) || defined( CONFIG_MIPS_BCM7401B0 ) \
  	|| defined( CONFIG_MIPS_BCM7402 ) || defined( CONFIG_MIPS_BCM7440 ) \
	|| defined(CONFIG_MIPS_BCM7401C0 ) || defined( CONFIG_MIPS_BCM7118A0 ) \
        || defined(CONFIG_MIPS_BCM7403A0) || defined( CONFIG_MIPS_BCM7452A0 ) \
	|| defined(CONFIG_MIPS_BCM7405A0)
  #define SUN_TOP_CTRL_SW_RESET		0xb0404014
  #endif
#define CHIP_MASTER_RESET 			(1<<31)

// jipeng - fixup for 3563
#if defined( CONFIG_MIPS_BCM3563 ) || defined(CONFIG_MIPS_BCM3563C0)
#define SUN_TOP_CTRL_SW_RESET		0xb0404014
#endif

	volatile unsigned long* ulp;

/* PR21527 - Fix SMP reboot problem */
#ifdef CONFIG_SMP
	smp_send_stop();
	udelay(10);
#endif

#ifdef CONFIG_MTD_BRCMNAND 
  	/*
  	 * THT: For version 1.0+ of the NAND controller, must revert to Direct Access in order to
  	 * do XIP from the flash
  	 */
  	brcmnand_prepare_reboot();
 #endif

	ulp = (volatile unsigned long*) SUN_TOP_CTRL_RESET_CTRL;
	*ulp |= MASTER_RESET_ENABLE;
	ulp = (volatile unsigned long*) SUN_TOP_CTRL_SW_RESET;
	*ulp |= CHIP_MASTER_RESET;
	udelay(10);

	/* NOTREACHED */
#endif

	/* Reboot */
	back_to_prom();

}

static void brcm_machine_halt(void)
{
	printk("Broadcom eval board halted.\n");
	while (1);
}

static void brcm_machine_power_off(void)
{
	printk("Broadcom eval board halted. Please turn off power.\n");
	while (1);
}

static __init void brcm_time_init(void)
{
	extern unsigned int mips_hpt_frequency;
	unsigned int GetMIPSFreq(void);
	volatile unsigned int countValue;
	unsigned int mipsFreq4Display;
	char msg[133];


	/* Set the counter frequency */
    	//mips_counter_frequency = CPU_CLOCK_RATE/2;

    	countValue = GetMIPSFreq(); // Taken over 1/8 sec.
    	mips_hpt_frequency = countValue * 8;
    	mipsFreq4Display = (mips_hpt_frequency/1000000) * 1000000;
    	sprintf(msg, "mips_counter_frequency = %d from Calibration, = %d from header(CPU_MHz/2)\n", 
		mipsFreq4Display, CPU_CLOCK_RATE/2);
	uart_puts(msg);

}

void __init  plat_timer_setup(struct irqaction *irq)
{
	unsigned int count;

	/* Connect the timer interrupt */
	irq->dev_id = (void *) irq;
	setup_irq(BCM_LINUX_SYSTIMER_IRQ, irq);

	/* Generate first timer interrupt */
	count = read_c0_count();
	write_c0_count(count + 1000);
}

void __init plat_mem_setup(void)
{
 	extern int rac_setting(int);
	extern int panic_timeout;

#ifdef CONFIG_MIPS_BRCM
#if defined( CONFIG_MIPS_BCM7038A0 )
	set_io_port_base(0xe0000000);  /* start of PCI IO space. */
#elif defined( CONFIG_MIPS_BCM7038B0 )  || defined( CONFIG_MIPS_BCM7038C0 ) \
	|| defined( CONFIG_MIPS_BCM7400 ) || defined( CONFIG_MIPS_BCM3560 ) \
	|| defined( CONFIG_MIPS_BCM7401 ) || defined( CONFIG_MIPS_BCM7402 ) \
	|| defined( CONFIG_MIPS_BCM7118 ) || defined( CONFIG_MIPS_BCM7440 ) \
        || defined( CONFIG_MIPS_BCM7403 ) || defined( CONFIG_MIPS_BCM7405 )
	
	set_io_port_base(0xf0000000);  /* start of PCI IO space. */
#elif defined( CONFIG_MIPS_BCM7329 )
	set_io_port_base(KSEG1ADDR(0x1af90000));
#elif defined ( CONFIG_BCM93730 )
	set_io_port_base(KSEG1ADDR(0x13000000));
#else
       
	set_io_port_base(0); 
#endif


#endif

	_machine_restart = brcm_machine_restart;
	_machine_halt = brcm_machine_halt;
	//_machine_power_off = brcm_machine_power_off;
	pm_power_off = brcm_machine_power_off;

	board_time_init = brcm_time_init;
 	panic_timeout = 180;

// Set RAC on 7400
#if defined( CONFIG_MIPS_BCM7400A0 )
	rac_setting(1);
#endif

#ifdef CONFIG_PC_KEYB
	kbd_ops = &brcm_kbd_ops;
#endif
#ifdef CONFIG_VT
	conswitchp = &dummy_con;
#endif

#ifdef CONFIG_DISCONTIGMEM
        brcm_numa_init();
#endif
}

unsigned long get_upper_membase(void)
{
	extern phys_t upper_memory;

	return (unsigned long) upper_memory;
}
EXPORT_SYMBOL(get_upper_membase);

//early_initcall(brcm_setup);
