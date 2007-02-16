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

extern void uart_puts(const char*);

void __init bus_error_init(void) { /* nothing */ }


#if 0 //def CONFIG_MIPS_BCM7402S
#include <asm/brcmstb/brcm97401a0/bcmuart.h>

#define BCHP_SUN_TOP_CTRL_SW_RESET 	            		0x00404010
#define BCHP_SUN_TOP_CTRL_SEMAPHORE_8          		0x00404048
#define BCHP_SUN_GISB_ARB_TIMER                      		0x0040600c
#define BCHP_SUN_GISB_ARB_REQ_MASK               		0x00406008
#define BCHP_FMISC_SOFT_RESET                        		0x00104000 
#define BCHP_MEMC_0_DDR_PLL_FREQ_DIVIDER_RESET	0x0010683c
#define BCHP_MEMC_0_DDR_PLL_FREQ_CNTL            	0x00106818
#define BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5         	0x00404098
#define BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8         	0x004040a4 

#define printf(x) printk(x)

void WriteRegister(unsigned long offset, unsigned long value)
{
	(*(volatile unsigned long*) (0xb0000000+offset)) = value;
}

unsigned long ReadRegister(unsigned long offset)
{
	return (*((volatile unsigned long*) offset));
}

brcm_pci_slave_reset()
{
	{
	// remove chip sw reset 10404000+d16 +0xa00000000 need update to sundry/inc/top_ctrl.vh TOP_CTRL_SW_RESET value
	//li      t0,0xb0404010	// SUN_TOP_CTRL_SW_RESET
	//li      t1,0x00000000
	//sw      t1,0(t0)
		WriteRegister( BCHP_SUN_TOP_CTRL_SW_RESET, 0x03C50BD8 );
		// THT: Take out SATA Reset
		//WriteRegister( BCHP_SUN_TOP_CTRL_SW_RESET, 0x03C10BD8 );
printf("Done reset\n");
		udelay(100000);
printf("Write BCHP_SUN_TOP_CTRL_SW_RESET 2\n");
		WriteRegister( BCHP_SUN_TOP_CTRL_SW_RESET, 0x00000008 );

		// Disable EBI boot window
printf("Disable EBI boot window\n");
		WriteRegister( BCHP_EBI_CS_CONFIG_0, 0x00000000 );

	//
	//li      t0,0xb0404070	
	//li      t1,0xffffffff
	//sw      t1,0(t0)

printf("BCHP_SUN_TOP_CTRL_SEMAPHORE_8\n");
		WriteRegister( BCHP_SUN_TOP_CTRL_SEMAPHORE_8, 0xffffffff );

	//  program gisb timeout
	//li      t0,0xb040600c	
	//li      t1,0x0000ffff
	//sw      t1,0(t0)

printf("BCHP_SUN_GISB_ARB_TIMER\n");
		WriteRegister( BCHP_SUN_GISB_ARB_TIMER, 0x0000ffff );

	//  masking gisb req
	//li      t0,0xb0406008	
	//li      t1,0x00000000
	//sw      t1,0(t0)
printf("BCHP_SUN_GISB_ARB_REQ_MASK\n");
		WriteRegister( BCHP_SUN_GISB_ARB_REQ_MASK, 0x00000000 );

	//   RESET PFRI 
	//li      t0,0xb0104000	
	//li      t1,0x00700000
	//sw      t1,0(t0)

printf("BCHP_FMISC_SOFT_RESET\n");
		WriteRegister( BCHP_FMISC_SOFT_RESET, 0x00700000 );

	//   Unreset the rbus reset
	//li      t0,0xb0104000	
	//li      t1,0x00500000
	//sw      t1,0(t0)
printf("BCHP_FMISC_SOFT_RESET\n");
		WriteRegister( BCHP_FMISC_SOFT_RESET, 0x00500000 );

	//   
	//li      t0,0xb010683c	
	//li      t1,0x00000001
	//sw      t1,0(t0)
printf("BCHP_MEMC_0_DDR_PLL_FREQ_DIVIDER_RESET\n");
		WriteRegister( BCHP_MEMC_0_DDR_PLL_FREQ_DIVIDER_RESET, 0x00000001 );

	//  Temporary step to avoid a pll model issue 
	//li      t0,0xb0106818	
	//li      t1,0x00000115
	//sw      t1,0(t0)
printf("BCHP_MEMC_0_DDR_PLL_FREQ_CNTL\n");
		WriteRegister( BCHP_MEMC_0_DDR_PLL_FREQ_CNTL, 0x00000115 );

	//li      t0,0xb0106818	
	//li      t1,0x00000126
	//sw      t1,0(t0)
printf("BCHP_MEMC_0_DDR_PLL_FREQ_CNTL\n");
		WriteRegister( BCHP_MEMC_0_DDR_PLL_FREQ_CNTL, 0x00000126 );

	//li      t0,0xb010683c	
	//li      t1,0x00000000
	//sw      t1,0(t0)
printf("BCHP_MEMC_0_DDR_PLL_FREQ_DIVIDER_RESET\n");
		WriteRegister( BCHP_MEMC_0_DDR_PLL_FREQ_DIVIDER_RESET, 0x00000000 );

	//li      t0,0xb0104000	
	//li      t1,0x00000000
	//sw      t1,0(t0)
printf("BCHP_FMISC_SOFT_RESET\n");
		WriteRegister( BCHP_FMISC_SOFT_RESET, 0x00000000 );

	/* Set MUX to select UART1/B pin functions */
	//li   t0,BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5)
	//li   t3,0x00048000 /* 20:18 --> 1 for RXD1 17:15 --> 1 for TXD1	*/
	//sw   t3,0(t0)    

printf("Set MUX to select UART1/B pin functions\n");
		WriteRegister( BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, 0x00048000 );

	/* Set MUX to select UART2/C pin functions */
	//li   t0,BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8)
	//li   t3,0x00040200 /* 20:18 --> 1 for RXD2 11:09 --> 1 for TXD2	*/
	//sw   t3,0(t0)    
   
		WriteRegister( BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, 0x00040200 );

	/* set baud rate */
	//li		t0, UARTB_ADR_BASE
	//li		t3, BAUD_115200_LO
	//sw		t3, UART_BAUDLO(t0)

printf("Set BAUD rate\n");
		WriteRegister( BCHP_UARTB_BAUDLO, BAUD_115200_LO );

	//li		t3, BAUD_115200_HI 
	//sw		t3, UART_BAUDHI(t0)
   
		WriteRegister( BCHP_UARTB_BAUDHI, BAUD_115200_HI );

	/* 8 bits, 1 stop, no parity, enable tx/rx */
   	//move t1, zero
   	//sw   t1, UART_RXSTAT(t0)

		WriteRegister( BCHP_UARTB_RCVSTAT, 0 );

   	//sw   t1, UART_TXSTAT(t0)

		WriteRegister( BCHP_UARTB_XMTSTAT, 0 );

   	//li   t1, BITM8 | RXEN | TXEN
   	//sw   t1, UART_CONTROL(t0)   

	printf("Set Write 8N1 rate\n");
		WriteRegister( BCHP_UARTB_CONTROL, BITM8 | RXEN | TXEN );

		// Reset SDRAM and run SHMOO
		// ResetSDRam();
		
		// THT We have to rely on the host driver to reload the shmoo, and start over.

		/* LMT
		WriteRegister( BCHP_SUN_TOP_CTRL_SW_RESET, 0x3dff0 );
		WriteRegister( BCHP_SUN_TOP_CTRL_SW_RESET, 0x00 );
		WriteRegister( BCHP_SUN_TOP_CTRL_SW_RESET, 0x100000 );
		ResetSDRam();
	*/
	}
}

#endif

static void brcm_machine_restart(char *command)
{
	static void (*back_to_prom)(void) = (void (*)(void)) 0xbfc00000;

#if 0 //def CONFIG_BRCM_PCI_SLAVE
/* PCI slave style reset */
	brcm_pci_slave_reset();
	/* NOT REACHED */
	return;

#else
#if defined( CONFIG_MIPS_BCM7401 ) || defined( CONFIG_MIPS_BCM7400 ) \
	|| defined( CONFIG_MIPS_BCM3560 ) || defined( CONFIG_MIPS_BCM7038C0 ) \
	|| defined( CONFIG_MIPS_BCM7402 ) || defined( CONFIG_MIPS_BCM7402S ) \
	|| defined( CONFIG_MIPS_BCM7118 ) || defined( CONFIG_MIPS_BCM7440 ) 
#define SUN_TOP_CTRL_RESET_CTRL		0xb0404008
#define MASTER_RESET_ENABLE 		(1<<3)
  
  #if defined( CONFIG_MIPS_BCM7401A0 ) || defined( CONFIG_MIPS_BCM3560 ) \
 	|| defined( CONFIG_MIPS_BCM7038C0 ) || defined( CONFIG_MIPS_BCM7402S ) \
	|| defined( CONFIG_MIPS_BCM7118A0 )
  #define SUN_TOP_CTRL_SW_RESET		0xb0404010
  
  #elif defined( CONFIG_MIPS_BCM7400 ) || defined( CONFIG_MIPS_BCM7401B0 ) \
  	|| defined( CONFIG_MIPS_BCM7402 ) || defined( CONFIG_MIPS_BCM7440 ) \
	|| defined(CONFIG_MIPS_BCM7401C0 ) || defined( CONFIG_MIPS_BCM7118A0 )
  #define SUN_TOP_CTRL_SW_RESET		0xb0404014
  #endif
#define CHIP_MASTER_RESET 			(1<<31)

	volatile unsigned long* ulp;

/* PR21527 - Fix SMP reboot problem */
#ifdef CONFIG_SMP
	smp_send_stop();
	udelay(10);
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
#endif // Slave PCI reset
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
	|| defined( CONFIG_MIPS_BCM7118 ) || defined( CONFIG_MIPS_BCM7440 )
	
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
#if defined( CONFIG_MIPS_BCM7400 )	
	rac_setting(1);
#endif

#ifdef CONFIG_PC_KEYB
	kbd_ops = &brcm_kbd_ops;
#endif
#ifdef CONFIG_VT
	conswitchp = &dummy_con;
#endif
}

unsigned long get_upper_membase(void)
{
	extern phys_t upper_memory;

	return (unsigned long) upper_memory;
}
EXPORT_SYMBOL(get_upper_membase);

//early_initcall(brcm_setup);
