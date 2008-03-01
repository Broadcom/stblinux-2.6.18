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

//#if     defined(CONFIG_MTD_BRCMNAND) || defined(CONFIG_MTD_BRCMNAND_MODULE)
//#include <linux/mtd/brcmnand.h>
//#endif

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
        || defined( CONFIG_MIPS_BCM7403 ) || defined( CONFIG_MIPS_BCM7452 )
#define SUN_TOP_CTRL_RESET_CTRL		0xb0404008
#define MASTER_RESET_ENABLE 		(1<<3)
  
  /* NOTE: CONFIG_MIPS_BCM3563C0 implies CONFIG_MIPS_BCM3560 */
  #if defined( CONFIG_MIPS_BCM7400 ) || defined( CONFIG_MIPS_BCM7401B0 ) \
  	|| defined( CONFIG_MIPS_BCM7402 ) || defined( CONFIG_MIPS_BCM7440 ) \
	|| defined(CONFIG_MIPS_BCM7401C0 ) || defined( CONFIG_MIPS_BCM7118A0 ) \
	|| defined(CONFIG_MIPS_BCM7403A0) || defined( CONFIG_MIPS_BCM7452A0 ) \
	|| defined( CONFIG_MIPS_BCM3563 ) || defined(CONFIG_MIPS_BCM3563C0)
  #define SUN_TOP_CTRL_SW_RESET		0xb0404014

  #elif defined( CONFIG_MIPS_BCM7401A0 ) || defined( CONFIG_MIPS_BCM3560 ) \
 	|| defined( CONFIG_MIPS_BCM7038C0 ) || defined( CONFIG_MIPS_BCM7402S )
  #define SUN_TOP_CTRL_SW_RESET		0xb0404010
  
  #endif
#define CHIP_MASTER_RESET 			(1<<31)


#elif defined( CONFIG_MIPS_BCM7405 ) || defined( CONFIG_MIPS_BCM7335 ) \
	|| defined( CONFIG_MIPS_BCM7325 )

#define SUN_TOP_CTRL_SW_RESET		(BCHP_SUN_TOP_CTRL_SW_RESET | 0xb0000000)
#define SUN_TOP_CTRL_RESET_CTRL		(BCHP_SUN_TOP_CTRL_RESET_CTRL | 0xb0000000)
#define CHIP_MASTER_RESET		(1 << BCHP_SUN_TOP_CTRL_SW_RESET_chip_master_reset_SHIFT)
#define MASTER_RESET_ENABLE		(1 << BCHP_SUN_TOP_CTRL_RESET_CTRL_master_reset_en_SHIFT)

#endif

#ifdef SUN_TOP_CTRL_SW_RESET

	volatile unsigned long* ulp;

/* PR21527 - Fix SMP reboot problem */
#ifdef CONFIG_SMP
	smp_send_stop();
	udelay(10);
#endif

#if 0 //def CONFIG_MTD_BRCMNAND 
	// PR38914: 01-23-08 for 2.6.12-5.3 & 2.6.18-4.1: This is now called via the FS /MTD notifier.
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

#ifdef	CONFIG_MIPS_MT_SMTC
irqreturn_t smtc_timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	int cpu = smp_processor_id();
	int vpflags;

	if (read_c0_cause() & (1 << 30)) {
		/* If timer interrupt, make it de-assert */
		write_c0_compare (read_c0_count() - 1);

		/*
		 * There are things we only want to do once per tick
		 * in an "MP" system.   One TC of each VPE will take
		 * the actual timer interrupt.  The others will get
		 * timer broadcast IPIs. We use whoever it is that takes
		 * the tick on VPE 0 to run the full timer_interrupt().
		 */
		if (cpu_data[cpu].vpe_id == 0) {
				timer_interrupt(irq, NULL, regs);
				smtc_timer_broadcast(cpu_data[cpu].vpe_id);

		} else {
			write_c0_compare(read_c0_count() + (mips_hpt_frequency/HZ));
			local_timer_interrupt(irq, dev_id, regs);
			smtc_timer_broadcast(cpu_data[cpu].vpe_id);
		}
	}

	return IRQ_HANDLED;
}
#endif


void __init  plat_timer_setup(struct irqaction *irq)
{
	/* Connect the timer interrupt */
	irq->dev_id = (void *) irq;
	setup_irq(BCM_LINUX_SYSTIMER_IRQ, irq);

	printk("plat_timer_setup: status %08x cause %08x\n",
		read_c0_status(), read_c0_cause());

#ifdef	CONFIG_MIPS_MT_SMTC 
	irq->handler = smtc_timer_interrupt;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].status &= ~IRQ_DISABLED;
	irq_desc[BCM_LINUX_SYSTIMER_IRQ].status |= IRQ_PER_CPU;
#endif

	/* Generate first timer interrupt */
	write_c0_compare(read_c0_count() + (mips_hpt_frequency/HZ));
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
        || defined( CONFIG_MIPS_BCM7403 ) || defined( CONFIG_MIPS_BCM7405 ) \
	|| defined( CONFIG_MIPS_BCM7335 ) || defined( CONFIG_MIPS_BCM7325 )
	
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
