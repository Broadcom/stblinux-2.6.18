/*---------------------------------------------------------------------------

    Copyright (c) 2001-2007 Broadcom Corporation                 /\
                                                          _     /  \     _
    _____________________________________________________/ \   /    \   / \_
                                                            \_/      \_/  
    
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 as
 published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    File: brcm-pm.c

    Description: 
    Power management for Broadcom STB/DTV peripherals

    when        who         what
    -----       ---         ----
    20071030    cernekee    initial version
    20071219    cernekee    add DDR self-refresh support; switch to sysfs
 ------------------------------------------------------------------------- */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/mii.h>
#include <linux/platform_device.h>
#include <asm/delay.h>
#include <asm/brcmstb/common/brcmstb.h>
#include <asm/brcmstb/common/brcm-pm.h>

#if defined(CONFIG_MIPS_BCM7325) || defined(CONFIG_MIPS_BCM7335) \
	|| defined(CONFIG_MIPS_BCM7336)
/* EREF clock also controls the frontend on these chips */
#define EREF_OK			0
#else
#define EREF_OK			1
#endif

static atomic_t usb_count = ATOMIC_INIT(1);
static atomic_t enet_count = ATOMIC_INIT(1);
static atomic_t sata_count = ATOMIC_INIT(1);

static int (*sata_off_cb)(void *) = NULL;
static int (*sata_on_cb)(void *) = NULL;
static void *sata_cb_arg;

static int (*enet_off_cb)(void *) = NULL;
static int (*enet_on_cb)(void *) = NULL;
static void *enet_cb_arg;

static int (*ehci_off_cb)(void *) = NULL;
static int (*ehci_on_cb)(void *) = NULL;
static void *ehci_cb_arg;

static int (*ohci_off_cb)(void *) = NULL;
static int (*ohci_on_cb)(void *) = NULL;
static void *ohci_cb_arg;

/*
 * LOW-LEVEL FUNCTIONS
 */

/*
 * usb, enet, sata are READ ONLY
 * 0  = no drivers present, device is powered off
 * >0 = driver(s) present, device is in use
 */

static void usb_enable(void)
{
	unsigned long flags;

	printk(KERN_INFO "brcm-pm: enabling power to USB block\n");

	spin_lock_irqsave(&g_magnum_spinlock, flags);

#if defined(BCHP_CLK_SYS_PLL_0_PLL_4_DIS_CH_MASK)
	BDEV_UNSET(BCHP_CLK_SYS_PLL_0_PLL_4, 
		BCHP_CLK_SYS_PLL_0_PLL_4_DIS_CH_MASK);
	BDEV_RD(BCHP_CLK_SYS_PLL_0_PLL_4);
#endif

#if defined(BCHP_CLK_USB_PM_CTRL_DIS_216M_CLK_MASK)
	BDEV_UNSET(BCHP_CLK_USB_PM_CTRL, 
		BCHP_CLK_USB_PM_CTRL_DIS_216M_CLK_MASK);
	BDEV_RD(BCHP_CLK_USB_PM_CTRL);
#endif

#if defined(BCHP_CLK_PM_CTRL_2_DIS_USB_216M_CLK_MASK)
	BDEV_UNSET(BCHP_CLK_PM_CTRL_2,
		BCHP_CLK_PM_CTRL_2_DIS_USB_216M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL_2);
#endif

#if defined(BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_USB_MASK)
	BDEV_UNSET(BCHP_CLKGEN_PWRDN_CTRL_0,
		BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_USB_MASK);
	BDEV_RD(BCHP_CLKGEN_PWRDN_CTRL_0);
#endif

#if defined(BCHP_CLK_USB_PM_CTRL_DIS_108M_CLK_MASK)
	BDEV_UNSET(BCHP_CLK_USB_PM_CTRL, 
		BCHP_CLK_USB_PM_CTRL_DIS_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_USB_PM_CTRL);
#endif

#if defined(BCHP_CLK_PM_CTRL_DIS_USB_108M_CLK_MASK)
	BDEV_UNSET(BCHP_CLK_PM_CTRL, 
		BCHP_CLK_PM_CTRL_DIS_USB_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL);
#endif

#if defined(BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_USB_MASK)
	BDEV_UNSET(BCHP_CLKGEN_PWRDN_CTRL_1,
		BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_USB_MASK);
	BDEV_RD(BCHP_CLKGEN_PWRDN_CTRL_1);
#endif

	BDEV_SET(BCHP_USB_CTRL_UTMI_CTL_1,
		BCHP_USB_CTRL_UTMI_CTL_1_PHY_PWDNB_MASK |
		BCHP_USB_CTRL_UTMI_CTL_1_PHY1_PWDNB_MASK);
	BDEV_RD(BCHP_USB_CTRL_UTMI_CTL_1);

	BDEV_SET(BCHP_USB_CTRL_PLL_CTL_1,
		BCHP_USB_CTRL_PLL_CTL_1_PLL_PWRDWNB_MASK);
	BDEV_RD(BCHP_USB_CTRL_PLL_CTL_1);

	BDEV_SET(BCHP_USB_CTRL_UTMI_CTL_1,
		BCHP_USB_CTRL_UTMI_CTL_1_UTMI_PWDNB_MASK |
		BCHP_USB_CTRL_UTMI_CTL_1_UTMI1_PWDNB_MASK);
	BDEV_RD(BCHP_USB_CTRL_PLL_CTL_1);

	spin_unlock_irqrestore(&g_magnum_spinlock, flags);
}

static void usb_disable(void)
{
	unsigned long flags;

	printk(KERN_INFO "brcm-pm: disabling power to USB block\n");

	spin_lock_irqsave(&g_magnum_spinlock, flags);

	BDEV_UNSET(BCHP_USB_CTRL_UTMI_CTL_1,
		BCHP_USB_CTRL_UTMI_CTL_1_UTMI_PWDNB_MASK |
		BCHP_USB_CTRL_UTMI_CTL_1_UTMI1_PWDNB_MASK);
	BDEV_RD(BCHP_USB_CTRL_UTMI_CTL_1);

	BDEV_UNSET(BCHP_USB_CTRL_PLL_CTL_1,
		BCHP_USB_CTRL_PLL_CTL_1_PLL_PWRDWNB_MASK |
		BCHP_USB_CTRL_PLL_CTL_1_XTAL_PWRDWNB_MASK);
	BDEV_RD(BCHP_USB_CTRL_PLL_CTL_1);

	BDEV_UNSET(BCHP_USB_CTRL_UTMI_CTL_1,
		BCHP_USB_CTRL_UTMI_CTL_1_PHY_PWDNB_MASK |
		BCHP_USB_CTRL_UTMI_CTL_1_PHY1_PWDNB_MASK);
	BDEV_RD(BCHP_USB_CTRL_UTMI_CTL_1);

#if defined(BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_USB_MASK)
	BDEV_SET(BCHP_CLKGEN_PWRDN_CTRL_1,
		BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_USB_MASK);
	BDEV_RD(BCHP_CLKGEN_PWRDN_CTRL_1);
#endif

#if defined(BCHP_CLK_PM_CTRL_DIS_USB_108M_CLK_MASK)
	BDEV_SET(BCHP_CLK_PM_CTRL, 
		BCHP_CLK_PM_CTRL_DIS_USB_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL);
#endif

#if defined(BCHP_CLK_USB_PM_CTRL_DIS_108M_CLK_MASK)
	BDEV_SET(BCHP_CLK_USB_PM_CTRL,
		BCHP_CLK_USB_PM_CTRL_DIS_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_USB_PM_CTRL);
#endif

#if defined(BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_USB_MASK)
	BDEV_SET(BCHP_CLKGEN_PWRDN_CTRL_0,
		BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_USB_MASK);
	BDEV_RD(BCHP_CLKGEN_PWRDN_CTRL_0);
#endif

#if defined(BCHP_CLK_PM_CTRL_2_DIS_USB_216M_CLK_MASK)
	BDEV_SET(BCHP_CLK_PM_CTRL_2,
		BCHP_CLK_PM_CTRL_2_DIS_USB_216M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL_2);
#endif

#if defined(BCHP_CLK_USB_PM_CTRL_DIS_216M_CLK_MASK)
	BDEV_SET(BCHP_CLK_USB_PM_CTRL, 
		BCHP_CLK_USB_PM_CTRL_DIS_216M_CLK_MASK);
	BDEV_RD(BCHP_CLK_USB_PM_CTRL);
#endif

#if defined(BCHP_CLK_SYS_PLL_0_PLL_4_DIS_CH_MASK)
	BDEV_SET(BCHP_CLK_SYS_PLL_0_PLL_4, 
		BCHP_CLK_SYS_PLL_0_PLL_4_DIS_CH_MASK);
	BDEV_RD(BCHP_CLK_SYS_PLL_0_PLL_4);
#endif

	spin_unlock_irqrestore(&g_magnum_spinlock, flags);
}

#if defined(BCHP_ENET_TOP_REG_START)
#define MII_S_C		(BCHP_ENET_TOP_REG_START + 0x10)
#define MII_DATA	(BCHP_ENET_TOP_REG_START + 0x14)
#define EMAC_INT	(BCHP_ENET_TOP_REG_START + 0x1c)
#define CONTROL		(BCHP_ENET_TOP_REG_START + 0x2c)
#define ENET_PM_SUPPORTED 1
#elif defined(BCHP_EMAC_0_REG_START)
#define MII_S_C		(BCHP_EMAC_0_REG_START + 0x10)
#define MII_DATA	(BCHP_EMAC_0_REG_START + 0x14)
#define EMAC_INT	(BCHP_EMAC_0_REG_START + 0x1c)
#define CONTROL		(BCHP_EMAC_0_REG_START + 0x2c)
#define ENET_PM_SUPPORTED 1
#endif

#ifdef ENET_PM_SUPPORTED

#define MDIO_WR		0x50020000
#define MDIO_PMD_SHIFT	23
#define MDIO_REG_SHIFT	18
#define EMAC_MDIO_INT	0x01

/* write to the internal PHY's registers */
static void brcm_mii_write(int addr, u16 data)
{
	int i;

	BDEV_WR(EMAC_INT, EMAC_MDIO_INT);
	BDEV_RD(EMAC_INT);

	BDEV_WR(MII_DATA, MDIO_WR | (1 << MDIO_PMD_SHIFT) |
		(addr << MDIO_REG_SHIFT) | data);
	for(i = 0; i < 1000; i++) {
		if(BDEV_RD(EMAC_INT) & EMAC_MDIO_INT)
			return;
		udelay(1);
	}
	printk(KERN_WARNING "brcm-pm: MII write timed out\n");
}

static void enet_enable(void)
{
	unsigned long flags;

	printk(KERN_INFO "brcm-pm: enabling power to ENET block\n");

	spin_lock_irqsave(&g_magnum_spinlock, flags);

#if EREF_OK && defined(BCHP_VCXO_CTL_MISC_EREF_CTRL_POWERDOWN_MASK)
	BDEV_UNSET(BCHP_VCXO_CTL_MISC_EREF_CTRL,
		BCHP_VCXO_CTL_MISC_EREF_CTRL_POWERDOWN_MASK);
	BDEV_RD(BCHP_VCXO_CTL_MISC_EREF_CTRL);
#endif

#if defined(BCHP_CLK_PM_CTRL_2_DIS_ENET_216M_CLK_MASK)
	BDEV_UNSET(BCHP_CLK_PM_CTRL_2,
		BCHP_CLK_PM_CTRL_2_DIS_ENET_216M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL_2);
#endif

#if defined(BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_ENET_MASK)
	BDEV_UNSET(BCHP_CLKGEN_PWRDN_CTRL_0,
		BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_ENET_MASK);
	BDEV_RD(BCHP_CLKGEN_PWRDN_CTRL_0);
#endif

#if defined(BCHP_CLK_PM_CTRL_DIS_ENET_108M_CLK_MASK)
	BDEV_UNSET(BCHP_CLK_PM_CTRL, BCHP_CLK_PM_CTRL_DIS_ENET_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL);
#endif

#if defined(BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_ENET_MASK)
	BDEV_UNSET(BCHP_CLKGEN_PWRDN_CTRL_1,
		BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_ENET_MASK);
	BDEV_RD(BCHP_CLKGEN_PWRDN_CTRL_1);
#endif

#if defined(BCHP_CLK_PM_CTRL_1_DIS_ENET_25M_CLK_MASK)
	BDEV_UNSET(BCHP_CLK_PM_CTRL_1,
		BCHP_CLK_PM_CTRL_1_DIS_ENET_25M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL_1);
#endif

#if defined(BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_25_CG_ENET_MASK)
	BDEV_UNSET(BCHP_CLKGEN_PWRDN_CTRL_2,
		BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_25_CG_ENET_MASK);
	BDEV_RD(BCHP_CLKGEN_PWRDN_CTRL_2);
#endif

#if defined(BCHP_CLKGEN_PWRDN_CTRL_3_PWRDN_CLOCK_25_CG_ENET_MASK)
	BDEV_UNSET(BCHP_CLKGEN_PWRDN_CTRL_3,
		BCHP_CLKGEN_PWRDN_CTRL_3_PWRDN_CLOCK_25_CG_ENET_MASK);
	BDEV_RD(BCHP_CLKGEN_PWRDN_CTRL_3);
#endif
	spin_unlock_irqrestore(&g_magnum_spinlock, flags);

	/* exit PHY IDDQ mode */
	if(! (BDEV_RD(CONTROL) & 0x08)) {
		brcm_mii_write(0x1f, 0x008b);
		brcm_mii_write(0x10, 0x0000);
		brcm_mii_write(0x14, 0x0000);
		brcm_mii_write(0x1f, 0x000f);
		brcm_mii_write(0x10, 0x00d0);
		brcm_mii_write(0x1f, 0x000b);
	}
}

static void enet_disable(void)
{
	unsigned long flags;

	printk(KERN_INFO "brcm-pm: disabling power to ENET block\n");

	/* enter PHY IDDQ mode */
	if(! (BDEV_RD(CONTROL) & 0x08)) {
		if((BDEV_RD(MII_S_C) & 0x3f) == 0)
			BDEV_WR(MII_S_C, 0x9f);
		brcm_mii_write(MII_BMCR, BMCR_RESET);
		brcm_mii_write(0x1f, 0x008b);
		brcm_mii_write(0x10, 0x01c0);
		brcm_mii_write(0x14, 0x7000);
		brcm_mii_write(0x1f, 0x000f);
		brcm_mii_write(0x10, 0x20d0);
		brcm_mii_write(0x1f, 0x000b);
	}

	spin_lock_irqsave(&g_magnum_spinlock, flags);

#if defined(BCHP_CLKGEN_PWRDN_CTRL_3_PWRDN_CLOCK_25_CG_ENET_MASK)
	BDEV_SET(BCHP_CLKGEN_PWRDN_CTRL_3,
		BCHP_CLKGEN_PWRDN_CTRL_3_PWRDN_CLOCK_25_CG_ENET_MASK);
	BDEV_RD(BCHP_CLKGEN_PWRDN_CTRL_3);
#endif

#if defined(BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_25_CG_ENET_MASK)
	BDEV_SET(BCHP_CLKGEN_PWRDN_CTRL_2,
		BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_25_CG_ENET_MASK);
	BDEV_RD(BCHP_CLKGEN_PWRDN_CTRL_2);
#endif

#if defined(BCHP_CLK_PM_CTRL_1_DIS_ENET_25M_CLK_MASK)
	BDEV_SET(BCHP_CLK_PM_CTRL_1,
		BCHP_CLK_PM_CTRL_1_DIS_ENET_25M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL_1);
#endif

#if defined(BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_ENET_MASK)
	BDEV_SET(BCHP_CLKGEN_PWRDN_CTRL_1,
		BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_ENET_MASK);
	BDEV_RD(BCHP_CLKGEN_PWRDN_CTRL_1);
#endif

#if defined(BCHP_CLK_PM_CTRL_DIS_ENET_108M_CLK_MASK)
	BDEV_SET(BCHP_CLK_PM_CTRL, BCHP_CLK_PM_CTRL_DIS_ENET_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL);
#endif

#if defined(BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_ENET_MASK)
	BDEV_SET(BCHP_CLKGEN_PWRDN_CTRL_0,
		BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_ENET_MASK);
	BDEV_RD(BCHP_CLKGEN_PWRDN_CTRL_0);
#endif

#if defined(BCHP_CLK_PM_CTRL_2_DIS_ENET_216M_CLK_MASK)
	BDEV_SET(BCHP_CLK_PM_CTRL_2,
		BCHP_CLK_PM_CTRL_2_DIS_ENET_216M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL_2);
#endif

#if EREF_OK && defined(BCHP_VCXO_CTL_MISC_EREF_CTRL_POWERDOWN_MASK)
	BDEV_SET(BCHP_VCXO_CTL_MISC_EREF_CTRL,
		BCHP_VCXO_CTL_MISC_EREF_CTRL_POWERDOWN_MASK);
	BDEV_RD(BCHP_VCXO_CTL_MISC_EREF_CTRL);
#endif
	spin_unlock_irqrestore(&g_magnum_spinlock, flags);
}

#else /* ENET_PM_SUPPORTED */


#endif

static void sata_enable(void)
{
#if defined(BRCM_SATA_SUPPORTED)
	unsigned long flags;

	printk(KERN_INFO "brcm-pm: enabling power to SATA block\n");

	spin_lock_irqsave(&g_magnum_spinlock, flags);

#if defined(BCHP_CLK_GENET_NETWORK_PLL_4_DIS_CH_MASK)
		BDEV_SET(BCHP_CLK_GENET_NETWORK_PLL_4,
		BCHP_CLK_GENET_NETWORK_PLL_4_CLOCK_ENA_MASK |
		BCHP_CLK_GENET_NETWORK_PLL_4_EN_CMLBUF_MASK);
	BDEV_UNSET(BCHP_CLK_GENET_NETWORK_PLL_4,
		BCHP_CLK_GENET_NETWORK_PLL_4_DIS_CH_MASK);
	BDEV_RD(BCHP_CLK_GENET_NETWORK_PLL_4);
#endif

#if defined(BCHP_CLK_PM_CTRL_2_DIS_SATA_PCI_CLK_MASK)
	BDEV_UNSET(BCHP_CLK_PM_CTRL_2,
		BCHP_CLK_PM_CTRL_2_DIS_SATA_PCI_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL_2);
#endif

#if defined(BCHP_CLK_PM_CTRL_2_DIS_SATA_216M_CLK_MASK)
	BDEV_UNSET(BCHP_CLK_PM_CTRL_2,
		BCHP_CLK_PM_CTRL_2_DIS_SATA_216M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL_2);
#endif

#if defined(BCHP_CLK_PM_CTRL_DIS_SATA_108M_CLK_MASK)
	BDEV_UNSET(BCHP_CLK_PM_CTRL, 
		BCHP_CLK_PM_CTRL_DIS_SATA_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL);
#endif

#if defined(BCHP_CLK_SATA_CLK_PM_CTRL_DIS_SATA_PCI_CLK_MASK)
	BDEV_UNSET(BCHP_CLK_SATA_CLK_PM_CTRL,
		BCHP_CLK_SATA_CLK_PM_CTRL_DIS_SATA_PCI_CLK_MASK);
	BDEV_RD(BCHP_CLK_SATA_CLK_PM_CTRL);
#endif

#if defined(BCHP_CLK_SATA_CLK_PM_CTRL_DIS_216M_CLK_MASK)
	BDEV_UNSET(BCHP_CLK_SATA_CLK_PM_CTRL,
		BCHP_CLK_SATA_CLK_PM_CTRL_DIS_216M_CLK_MASK);
	BDEV_RD(BCHP_CLK_SATA_CLK_PM_CTRL);
#endif

#if defined(BCHP_CLK_SATA_CLK_PM_CTRL_DIS_108M_CLK_MASK)
	BDEV_UNSET(BCHP_CLK_SATA_CLK_PM_CTRL, 
		BCHP_CLK_SATA_CLK_PM_CTRL_DIS_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_SATA_CLK_PM_CTRL);
#endif

#if defined(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1_sata_ana_pwrdn_MASK)
	BDEV_UNSET(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1,
		BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1_sata_ana_pwrdn_MASK);
	BDEV_RD(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1);
#endif
	spin_unlock_irqrestore(&g_magnum_spinlock, flags);
#endif /* BRCM_SATA_SUPPORTED */
}

static void sata_disable(void)
{
#if defined(BRCM_SATA_SUPPORTED)
	unsigned long flags;

	printk(KERN_INFO "brcm-pm: disabling power to SATA block\n");

	spin_lock_irqsave(&g_magnum_spinlock, flags);

#if defined(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1_sata_ana_pwrdn_MASK)
	BDEV_SET(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1,
		BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1_sata_ana_pwrdn_MASK);
	BDEV_RD(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1);
#endif

#if defined(BCHP_CLK_SATA_CLK_PM_CTRL_DIS_108M_CLK_MASK)
	BDEV_SET(BCHP_CLK_SATA_CLK_PM_CTRL, 
		BCHP_CLK_SATA_CLK_PM_CTRL_DIS_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_SATA_CLK_PM_CTRL);
#endif

#if defined(BCHP_CLK_SATA_CLK_PM_CTRL_DIS_216M_CLK_MASK)
	BDEV_SET(BCHP_CLK_SATA_CLK_PM_CTRL,
		BCHP_CLK_SATA_CLK_PM_CTRL_DIS_216M_CLK_MASK);
	BDEV_RD(BCHP_CLK_SATA_CLK_PM_CTRL);
#endif

#if defined(BCHP_CLK_SATA_CLK_PM_CTRL_DIS_SATA_PCI_CLK_MASK)
	BDEV_SET(BCHP_CLK_SATA_CLK_PM_CTRL,
		BCHP_CLK_SATA_CLK_PM_CTRL_DIS_SATA_PCI_CLK_MASK);
	BDEV_RD(BCHP_CLK_SATA_CLK_PM_CTRL);
#endif

#if defined(BCHP_CLK_PM_CTRL_DIS_SATA_108M_CLK_MASK)
	BDEV_SET(BCHP_CLK_PM_CTRL, 
		BCHP_CLK_PM_CTRL_DIS_SATA_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL);
#endif

#if defined(BCHP_CLK_PM_CTRL_2_DIS_SATA_PCI_CLK_MASK)
	BDEV_SET(BCHP_CLK_PM_CTRL_2,
		BCHP_CLK_PM_CTRL_2_DIS_SATA_PCI_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL_2);
#endif

#if defined(BCHP_CLK_PM_CTRL_2_DIS_SATA_216M_CLK_MASK)
	BDEV_SET(BCHP_CLK_PM_CTRL_2,
		BCHP_CLK_PM_CTRL_2_DIS_SATA_216M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL_2);
#endif

#if defined(BCHP_CLK_GENET_NETWORK_PLL_4_DIS_CH_MASK)
		BDEV_UNSET(BCHP_CLK_GENET_NETWORK_PLL_4,
		BCHP_CLK_GENET_NETWORK_PLL_4_CLOCK_ENA_MASK |
		BCHP_CLK_GENET_NETWORK_PLL_4_EN_CMLBUF_MASK);
	BDEV_SET(BCHP_CLK_GENET_NETWORK_PLL_4,
		BCHP_CLK_GENET_NETWORK_PLL_4_DIS_CH_MASK); 
		BDEV_RD(BCHP_CLK_GENET_NETWORK_PLL_4);
#endif

	spin_unlock_irqrestore(&g_magnum_spinlock, flags);
#endif /* BRCM_SATA_SUPPORTED */
}

static void moca_enable(void)
{
	/*XXX*/
}

static void moca_disable(void)
{
	/*XXX*/
}

/*
 * /sys/devices/platform/brcm-pm/ddr:
 *
 * 0  = no power management
 * >0 = enter self-refresh mode after <val> DDR clocks of inactivity
 */

static uint32_t ddr_get(void)
{
	uint32_t reg, inact_cnt, pdn_en, val;

#if defined (BCHP_MEMC_0_DDR_POWER_DOWN_MODE)
	reg = BDEV_RD(BCHP_MEMC_0_DDR_POWER_DOWN_MODE);
	pdn_en = (reg & BCHP_MEMC_0_DDR_POWER_DOWN_MODE_PDN_EN_MASK) >>
		BCHP_MEMC_0_DDR_POWER_DOWN_MODE_PDN_EN_SHIFT;
	inact_cnt = (reg & BCHP_MEMC_0_DDR_POWER_DOWN_MODE_INACT_CNT_MASK) >>
		BCHP_MEMC_0_DDR_POWER_DOWN_MODE_INACT_CNT_SHIFT;
	val = pdn_en ? inact_cnt : 0;
#endif
	return(val);
}

static void ddr_set(uint32_t val)
{
#if defined (BCHP_MEMC_0_DDR_POWER_DOWN_MODE)
	if(! val) {
		BDEV_WR(BCHP_MEMC_0_DDR_POWER_DOWN_MODE, 0);
	} else {
		BDEV_WR(BCHP_MEMC_0_DDR_POWER_DOWN_MODE,
			BCHP_MEMC_0_DDR_POWER_DOWN_MODE_PDN_EN_MASK |
			BCHP_MEMC_0_DDR_POWER_DOWN_MODE_PDN_MODE_MASK |
			((val << BCHP_MEMC_0_DDR_POWER_DOWN_MODE_INACT_CNT_SHIFT)
			& BCHP_MEMC_0_DDR_POWER_DOWN_MODE_INACT_CNT_MASK));
	}
#endif
}

/*
 * API FOR DRIVERS
 */

void brcm_pm_usb_add(void)
{
	if(atomic_inc_return(&usb_count) == 1)
		usb_enable();
}
EXPORT_SYMBOL(brcm_pm_usb_add);

void brcm_pm_usb_remove(void)
{
	if(atomic_dec_return(&usb_count) == 0)
		usb_disable();
}
EXPORT_SYMBOL(brcm_pm_usb_remove);

#ifdef ENET_PM_SUPPORTED

void brcm_pm_enet_add(void)
{
	if(atomic_inc_return(&enet_count) == 1)
		enet_enable();
}
EXPORT_SYMBOL(brcm_pm_enet_add);

void brcm_pm_enet_remove(void)
{
	if(atomic_dec_return(&enet_count) == 0)
		enet_disable();
}
EXPORT_SYMBOL(brcm_pm_enet_remove);

#endif

void brcm_pm_sata_add(void)
{
	if(atomic_inc_return(&sata_count) == 1)
		sata_enable();
}
EXPORT_SYMBOL(brcm_pm_sata_add);

void brcm_pm_sata_remove(void)
{
	if(atomic_dec_return(&sata_count) == 0)
		sata_disable();
}
EXPORT_SYMBOL(brcm_pm_sata_remove);

void brcm_pm_moca_enable(void)
{
		moca_enable();
}
EXPORT_SYMBOL(brcm_pm_moca_enable);

void brcm_pm_moca_disable(void)
{
		moca_disable();
}
EXPORT_SYMBOL(brcm_pm_moca_disable);

#define DECLARE_PM_REGISTER(func, name) \
	void func(int (*off_cb)(void *), int (*on_cb)(void *), void *arg) \
	{ \
		BUG_ON(name##_on_cb || name##_off_cb); \
		name##_on_cb = on_cb; \
		name##_off_cb = off_cb; \
		name##_cb_arg = arg; \
	} \
	EXPORT_SYMBOL(func)

#define DECLARE_PM_UNREGISTER(func, name) \
	void func(void) \
	{ \
		BUG_ON(! name##_on_cb || ! name##_off_cb); \
		name##_on_cb = name##_off_cb = NULL; \
	} \
	EXPORT_SYMBOL(func)

DECLARE_PM_REGISTER(brcm_pm_register_sata, sata);
DECLARE_PM_UNREGISTER(brcm_pm_unregister_sata, sata);

DECLARE_PM_REGISTER(brcm_pm_register_enet, enet);
DECLARE_PM_UNREGISTER(brcm_pm_unregister_enet, enet);

DECLARE_PM_REGISTER(brcm_pm_register_ehci, ehci);
DECLARE_PM_UNREGISTER(brcm_pm_unregister_ehci, ehci);

DECLARE_PM_REGISTER(brcm_pm_register_ohci, ohci);
DECLARE_PM_UNREGISTER(brcm_pm_unregister_ohci, ohci);


/*
 * API FOR USER PROGRAMS
 */

#define DECLARE_PM_SHOW(_func, _cb, _expr) \
	static ssize_t _func(struct device *dev, \
		struct device_attribute *attr, char *buf) \
		{ \
			return(sprintf(buf, "%d\n", (_cb) ? (_expr) : -1)); \
		}

DECLARE_PM_SHOW(usb_show, ehci_off_cb || ohci_off_cb,
	atomic_read(&usb_count) ? 1 : 0);
DECLARE_PM_SHOW(enet_show, enet_off_cb, atomic_read(&enet_count));
DECLARE_PM_SHOW(sata_show, sata_off_cb, atomic_read(&sata_count));
DECLARE_PM_SHOW(ddr_show, (void *)1, ddr_get());

static ssize_t ddr_store(struct device *dev, struct device_attribute *attr,
	const char *buf, size_t n)
{
	ddr_set(simple_strtoul(buf, NULL, 0));
	return(n);
}

static ssize_t sata_store(struct device *dev, struct device_attribute *attr,
	const char *buf, size_t n)
{
	if(! sata_off_cb || ! sata_on_cb) {
		printk(KERN_WARNING "brcm-pm: SATA callback not registered\n");
		return(n);
	}

	if(simple_strtoul(buf, NULL, 0) == 0)
		sata_off_cb(sata_cb_arg);
	else
		sata_on_cb(sata_cb_arg);

	return(n);
}

static ssize_t enet_store(struct device *dev, struct device_attribute *attr,
	const char *buf, size_t n)
{
	if(! enet_off_cb || ! enet_on_cb) {
		printk(KERN_WARNING "brcm-pm: ENET callback not registered\n");
		return(n);
	}

	if(simple_strtoul(buf, NULL, 0) == 0)
		enet_off_cb(enet_cb_arg);
	else
		enet_on_cb(enet_cb_arg);

	return(n);
}

static ssize_t usb_store(struct device *dev, struct device_attribute *attr,
	const char *buf, size_t n)
{
	if((! ehci_off_cb || ! ehci_on_cb) &&
	   (! ohci_off_cb || ! ohci_on_cb)) {
		printk(KERN_WARNING "brcm-pm: USB callback not registered\n");
		return(n);
	}

	if(simple_strtoul(buf, NULL, 0) == 0) {
		if(ehci_off_cb)
			ehci_off_cb(ehci_cb_arg);
		if(ohci_off_cb)
			ohci_off_cb(ohci_cb_arg);
	} else {
		if(ehci_on_cb)
			ehci_on_cb(ehci_cb_arg);
		if(ohci_on_cb)
			ohci_on_cb(ohci_cb_arg);
	}

	return(n);
}

DEVICE_ATTR(usb,0644,usb_show,usb_store);
DEVICE_ATTR(enet,0644,enet_show,enet_store);
DEVICE_ATTR(sata,0644,sata_show,sata_store);
#if defined (BCHP_MEMC_0_DDR_POWER_DOWN_MODE)
DEVICE_ATTR(ddr,0644,ddr_show,ddr_store);
#endif

static struct platform_driver brcm_pm_platform_driver = {
	.driver		= {
		.name	= "brcm-pm",
		.owner	= THIS_MODULE,
	},
};

static struct platform_device *brcm_pm_platform_device;

static int __init brcm_pm_init(void)
{
	int ret;
	struct platform_device *dev;
	struct device *cdev;

	/* power down all devices if nobody is using them */
	brcm_pm_usb_remove();
#ifdef ENET_PM_SUPPORTED
	brcm_pm_enet_remove();
#endif
	brcm_pm_sata_remove();

	ret = platform_driver_register(&brcm_pm_platform_driver);
	if(ret) {
		printk(KERN_ERR
			"brcm-pm: unable to register platform driver\n");
		goto bad;
	}

	brcm_pm_platform_device = dev = platform_device_alloc("brcm-pm", -1);
	if(! dev) {
		printk("brcm-pm: unable to allocate platform device\n");
		ret = -ENODEV;
		goto bad2;
	}

	ret = platform_device_add(dev);
	if(ret) {
		printk("brcm-pm: unable to add platform device\n");
		goto bad3;
		return(-ENODEV);
	}

	cdev = &dev->dev;
	device_create_file(cdev, &dev_attr_usb);
	device_create_file(cdev, &dev_attr_enet);
	device_create_file(cdev, &dev_attr_sata);
#if defined (BCHP_MEMC_0_DDR_POWER_DOWN_MODE)
	device_create_file(cdev, &dev_attr_ddr);
#endif

	return(0);

bad3:
	platform_device_put(dev);
bad2:
	platform_driver_unregister(&brcm_pm_platform_driver);
bad:
	return(ret);
}

static void __exit brcm_pm_exit(void)
{
	struct platform_device *dev = brcm_pm_platform_device;
	struct device *cdev = &dev->dev;

	device_remove_file(cdev, &dev_attr_usb);
	device_remove_file(cdev, &dev_attr_enet);
	device_remove_file(cdev, &dev_attr_sata);
#if defined (BCHP_MEMC_0_DDR_POWER_DOWN_MODE)
	device_remove_file(cdev, &dev_attr_ddr);
#endif

	platform_device_unregister(brcm_pm_platform_device);
	platform_driver_unregister(&brcm_pm_platform_driver);

	/* power up all devices, then exit */
	brcm_pm_usb_add();
#ifdef ENET_PM_SUPPORTED
	brcm_pm_enet_add();
#endif
	brcm_pm_sata_add();
	ddr_set(0);
}

module_init(brcm_pm_init);
module_exit(brcm_pm_exit);
