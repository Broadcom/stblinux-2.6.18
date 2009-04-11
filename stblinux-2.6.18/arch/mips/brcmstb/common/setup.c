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
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/mtd/partitions.h>
#include <linux/bmoca.h>
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
#include <asm/brcmstb/common/brcmstb.h>

bcm_discontigMem_t* bcm_pdiscontig_memmap = (bcm_discontigMem_t*) NULL;

extern void uart_puts(const char*);
extern void brcm_numa_init(void);

void __init bus_error_init(void) { /* nothing */ }


static void brcm_machine_restart(char *command)
{
	static void (*back_to_prom)(void) = (void (*)(void)) 0xbfc00000;

/* PR21527 - Fix SMP reboot problem */
#ifdef CONFIG_SMP
	smp_send_stop();
	udelay(10);
#endif

	/*
	 * If NAND is on CS0, we need to revert to direct access in order to
	 * re-enable XIP so CFE can boot.  This was once done through a
	 * call to brcmnand_prepare_reboot() from this function, but is
	 * now handled through the FS/MTD notifier.
	 */

	/* enable chip reset, then do it */
	BDEV_WR_F(SUN_TOP_CTRL_RESET_CTRL, master_reset_en, 1);
	BDEV_RD(BCHP_SUN_TOP_CTRL_RESET_CTRL);

	BDEV_WR_F(SUN_TOP_CTRL_SW_RESET, chip_master_reset, 1);
	BDEV_RD(BCHP_SUN_TOP_CTRL_SW_RESET);

	udelay(10);

	/* NOTREACHED */

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

	/* Set the counter frequency */
    	countValue = GetMIPSFreq(); // Taken over 1/8 sec.
    	mips_hpt_frequency = countValue * 8;
    	printk("Found MIPS counter frequency: %d Mhz\n",
		(mips_hpt_frequency + 500000) / 1000000);

}

#ifdef	CONFIG_MIPS_MT_SMTC
irqreturn_t smtc_timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	int cpu = smp_processor_id();
	int vpflags;

	if (read_c0_cause() & (1 << 30)) {
		/* If timer interrupt, make it de-assert */
		write_c0_compare (read_c0_count() - 1);

                vpflags = dvpe();
                clear_c0_cause(0x100<<7);
                evpe(vpflags);

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
	extern int panic_timeout;

#ifdef CONFIG_MIPS_BRCM
#if defined( CONFIG_MIPS_BCM7038A0 )
	set_io_port_base(0xe0000000);  /* start of PCI IO space. */
#elif defined( CONFIG_MIPS_BCM7038B0 )  || defined( CONFIG_MIPS_BCM7038C0 ) \
	|| defined( CONFIG_MIPS_BCM7400 ) || defined( CONFIG_MIPS_BCM3560 ) \
	|| defined( CONFIG_MIPS_BCM7401 ) || defined( CONFIG_MIPS_BCM7402 ) \
	|| defined( CONFIG_MIPS_BCM7118 )  \
        || defined( CONFIG_MIPS_BCM7403 ) || defined( CONFIG_MIPS_BCM7405 ) \
	|| defined( CONFIG_MIPS_BCM7335 ) || defined( CONFIG_MIPS_BCM7325 ) \
	|| defined( CONFIG_MIPS_BCM3548 ) || defined( CONFIG_MIPS_BCM7420 ) \
	|| defined( CONFIG_MIPS_BCM7336 )
	
	set_io_port_base(0xf0000000);  /* start of PCI IO space. */
#elif defined( CONFIG_MIPS_BCM7329 )
	set_io_port_base(KSEG1ADDR(0x1af90000));
#elif defined ( CONFIG_BCM93730 )
	set_io_port_base(KSEG1ADDR(0x13000000));

#elif defined( CONFIG_MIPS_BCM7440 ) || defined (CONFIG_MIPS_BCM7601)
	set_io_port_base(PCI_IO_WIN_BASE);  /* 0xf8000000 in boardmap.h. */
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

#if defined( CONFIG_MIPS_BCM7440B0 ) || defined( CONFIG_MIPS_BCM7325B0 ) \
	|| defined( CONFIG_MIPS_BCM7443A0 ) || defined (CONFIG_MIPS_BCM7601)
	
    // Set externalize IO sync bit (CP0 $16, sel 7, bit 8)
	{
        uint32_t extIO = __read_32bit_c0_register($16, 7);
(16, 7, extIO | 0x100);
        extIO = __read_32bit_c0_register($16, 7);
	}

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

#if defined(CONFIG_SPI_BCM7XXX) || defined(CONFIG_SPI_BCM7XXX_MODULE)

/*
 * MTD support for serial flash on MSPI
 */

static struct resource bcmspi_resources[] = {
	[0] = {
		.start	= BPHYSADDR(BCHP_MSPI_REG_START),
		.end	= BPHYSADDR(BCHP_MSPI_REG_END),
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= BCM_LINUX_SPI_IRQ,
		.end	= BCM_LINUX_SPI_IRQ,
		.flags	= IORESOURCE_IRQ,
	},
#ifdef BCHP_BSPI_REG_START
	[2] = {
		.start	= BPHYSADDR(BCHP_BSPI_REG_START),
		.end	= BPHYSADDR(BCHP_BSPI_REG_END),
		.flags	= IORESOURCE_MEM,
	},
#endif
};

static struct platform_device bcmspi_dev = {
	.name		= "bcm7xxx-spi",
	.id		= 1,
	.resource	= bcmspi_resources,
	.num_resources	= ARRAY_SIZE(bcmspi_resources),
};

static struct spi_board_info spi_board_info[] __initdata = {
{
	.modalias	= "spidev",
	.bus_num	= 1,
	.chip_select	= 0,
	.mode		= SPI_MODE_3,
},
{
	.modalias	= "spidev",
	.bus_num	= 1,
	.chip_select	= 1,
	.mode		= SPI_MODE_3,
},
{
	.modalias	= "spidev",
	.bus_num	= 1,
	.chip_select	= 2,
	.mode		= SPI_MODE_3,
},
{
	.modalias	= "spidev",
	.bus_num	= 1,
	.chip_select	= 3,
	.mode		= SPI_MODE_3,
},
};

#ifdef BRCM_SPI_CHIP_SELECT
/* 16MB map copied from drivers/mtd/maps/bcm9xxxx-flash.c */
static struct mtd_partition spi_flash_map[] = {
	{ name: "rootfs",	offset: 0,		size: 12*1024*1024 },
	{ name: "cfe",	        offset: 0x00C00000,	size: 512*1024 },
	{ name: "vmlinux",	offset: 0x00C80000,	size: 3582*1024 },
	{ name: "config",	offset: 0x00FFF800,	size: 144 },
	{ name: "nvram",	offset: 0x00FFF890,	size: 1904 },
};

static struct flash_platform_data spi_flash_data = {
	.name		= NULL,
	.parts		= spi_flash_map,
	.nr_parts	= 5,
};
#endif

static int __init bcmspi_initcall(void)
{
#ifdef BRCM_SPI_CHIP_SELECT
	int i;

	/*
	 * by default, allow userland access through spidev to all devices
	 * except the serial flash
	 */
	for(i = 0; i < ARRAY_SIZE(spi_board_info); i++) {
		if(spi_board_info[i].chip_select == BRCM_SPI_CHIP_SELECT) {
			strcpy(spi_board_info[i].modalias, "m25p80");
			spi_board_info[i].platform_data = &spi_flash_data;
		}
	}
#endif /* BRCM_SPI_CHIP_SELECT */
	platform_device_register(&bcmspi_dev);
	spi_register_board_info(spi_board_info, ARRAY_SIZE(spi_board_info));

	return(0);
}

device_initcall(bcmspi_initcall);

#endif /* CONFIG_SPI_BCM7XXX || CONFIG_SPI_BCM7XXX_MODULE */

#if	defined(CONFIG_BCMUMAC) || defined(CONFIG_BCMUMAC_MODULE)
#ifdef BRCM_MOCA_SUPPORTED

static void moca_bogus_release(struct device *dev)
{
}

static struct moca_platform_data moca_data = {
	.macaddr_hi =		0x00000102,
	.macaddr_lo =		0x03040000,

	.enet_name =		"BCMUNIMAC",
	.enet_id =		1,

	.bcm3450_i2c_base =	BPHYSADDR(BCHP_BSCB_REG_START),
	.bcm3450_i2c_addr =	0x70,
};

static struct resource moca_resources[] = {
	[0] = {
		.start =	BPHYSADDR(BRCM_MOCA_REG_START),
		.end =		BPHYSADDR(BRCM_MOCA_REG_END),
		.flags =	IORESOURCE_MEM,
	},
	[1] = {
		.start =	BCM_LINUX_MOCA_IRQ,
		.end =		BCM_LINUX_MOCA_IRQ,
		.flags =	IORESOURCE_IRQ,
	},
};

static struct platform_device moca_plat_dev = {
	.name =			"bmoca",
	.id =			0,
	.num_resources =	ARRAY_SIZE(moca_resources),
	.resource =		moca_resources,
	.dev = {
		.platform_data = &moca_data,
		.release =	moca_bogus_release,
	},
};

#endif /* BRCM_MOCA_SUPPORTED */

#ifdef BRCM_UMAC_0_SUPPORTED
static void umac_0_bogus_release(struct device *dev)
{
}
#ifdef BRCM_UMAC_0_GPHY
static struct bcmumac_platform_data umac_0_data = {
	.phy_type = 	BRCM_PHY_TYPE_EXT_GMII,
	.phy_id = 		BRCM_PHY_ID_AUTO,
};
#else
static struct bcmumac_platform_data umac_0_data = {
	.phy_type =		BRCM_PHY_TYPE_INT,
	.phy_id =		1,
};
#endif /* BRCM_UMAC_0_GPHY */
static struct resource umac_0_resources[] = {
	[0] = {
		.start =	BPHYSADDR(BRCM_UMAC_0_REG_START),
		.end =		BPHYSADDR(BRCM_UMAC_0_REG_END),
		.flags =	IORESOURCE_MEM,
	},
	[1] = {
		.start =	BCM_LINUX_CPU_ENET_IRQ,
		.end =		BCM_LINUX_CPU_ENET_IRQ,
		.flags =	IORESOURCE_IRQ,
	},
};

static struct platform_device umac_0_plat_dev = {
	.name =			"bcmumac",
	.id =			0,
	.num_resources =	ARRAY_SIZE(umac_0_resources),
	.resource =		umac_0_resources,
	.dev = {
		.platform_data = &umac_0_data,
		.release =	umac_0_bogus_release,
	},
};

#endif /* BRCM_UMAC_0_SUPPORTED */

#ifdef BRCM_UMAC_1_SUPPORTED

static void umac_1_bogus_release(struct device *dev)
{
}

static struct bcmumac_platform_data umac_1_data = {
	.phy_type =		BRCM_PHY_TYPE_MOCA,
	.phy_id =		BRCM_PHY_ID_NONE,
};

static struct resource umac_1_resources[] = {
	[0] = {
		.start =	BPHYSADDR(BRCM_UMAC_1_REG_START),
		.end =		BPHYSADDR(BRCM_UMAC_1_REG_END),
		.flags =	IORESOURCE_MEM,
	},
	[1] = {
		.start =	BCM_LINUX_CPU_ENET_1_IRQ,
		.end =		BCM_LINUX_CPU_ENET_1_IRQ,
		.flags =	IORESOURCE_IRQ,
	},
};

static struct platform_device umac_1_plat_dev = {
	.name =			"bcmumac",
	.id =			1,
	.num_resources =	ARRAY_SIZE(umac_1_resources),
	.resource =		umac_1_resources,
	.dev = {
		.platform_data = &umac_1_data,
		.release =	umac_1_bogus_release,
	},
};

#endif /* BRCM_UMAC_1_SUPPORTED */

#if defined(BRCM_UMAC_0_SUPPORTED) || defined(BRCM_MOCA_SUPPORTED)

static int __init umac_moca_initcall(void)
{
	u8 mac[6];
	extern unsigned char *gHwAddrs[];
	unsigned long flags;

	spin_lock_irqsave(&g_magnum_spinlock, flags);
#if defined(BRCM_UMAC_0_SUPPORTED)
	BDEV_WR_F(SUN_TOP_CTRL_SW_RESET, enet_sw_reset, 0);
#endif
#if defined(BRCM_MOCA_SUPPORTED)
	BDEV_WR_F(SUN_TOP_CTRL_SW_RESET, moca_sw_reset, 0);
	BDEV_WR_F(MOCA_HOSTMISC_SW_RESET, moca_enet_reset, 0);
#endif
	BDEV_RD(BCHP_SUN_TOP_CTRL_SW_RESET);
	spin_unlock_irqrestore(&g_magnum_spinlock, flags);

	memcpy(mac, gHwAddrs[0], 6);

#if defined(BRCM_UMAC_0_SUPPORTED)
	memcpy(&umac_0_data.macaddr, mac, 6);
	platform_device_register(&umac_0_plat_dev);
	mac[4]++;
#endif

#if defined(BRCM_UMAC_1_SUPPORTED)
	memcpy(&umac_1_data.macaddr, mac, 6);
	platform_device_register(&umac_1_plat_dev);
	mac[4]++;
#endif

#if defined(BRCM_MOCA_SUPPORTED)
	mac_to_u32(&moca_data.macaddr_hi, &moca_data.macaddr_lo, mac);
	platform_device_register(&moca_plat_dev);
	mac[4]++;
#endif

	return(0);
}

device_initcall(umac_moca_initcall);

#endif /* defined(BRCM_UMAC_0_SUPPORTED) || defined(BRCM_MOCA_SUPPORTED) */
#endif	/* CONFIG_BCMUMAC */
