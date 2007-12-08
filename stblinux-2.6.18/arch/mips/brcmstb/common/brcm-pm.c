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
 ------------------------------------------------------------------------- */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/types.h>
#include <asm/brcmstb/common/brcmstb.h>
#include <asm/brcmstb/common/brcm-pm.h>

static atomic_t usb_count = ATOMIC_INIT(1);
static atomic_t enet_count = ATOMIC_INIT(1);
static atomic_t sata_count = ATOMIC_INIT(1);

#define PROC_ENTRY_NAME		"driver/brcm-pm"

static void usb_enable(void)
{
	printk(KERN_INFO "brcm-pm: enabling power to USB block\n");
#if defined(CONFIG_MIPS_BCM7400B0) || defined(CONFIG_MIPS_BCM7405A0)
	BDEV_UNSET(BCHP_CLK_PM_CTRL, BCHP_CLK_PM_CTRL_DIS_USB_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL);

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
#else
#error Power management is not supported on this platform.
#endif
}

static void usb_disable(void)
{
#if defined(CONFIG_MIPS_BCM7400B0) || defined(CONFIG_MIPS_BCM7405A0)
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

	BDEV_SET(BCHP_CLK_PM_CTRL, BCHP_CLK_PM_CTRL_DIS_USB_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL);
#endif
	printk(KERN_INFO "brcm-pm: disabling power to USB block\n");
}

static void enet_enable(void)
{
	printk(KERN_INFO "brcm-pm: enabling power to ENET block\n");
#if defined(CONFIG_MIPS_BCM7400B0) || defined(CONFIG_MIPS_BCM7405A0)
	BDEV_UNSET(BCHP_CLK_PM_CTRL, BCHP_CLK_PM_CTRL_DIS_RPTD_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL);

	BDEV_UNSET(BCHP_CLK_PM_CTRL_1,
		BCHP_CLK_PM_CTRL_1_DIS_ENET_25M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL_1);
#endif
}

static void enet_disable(void)
{
	printk(KERN_INFO "brcm-pm: disabling power to ENET block\n");
#if defined(CONFIG_MIPS_BCM7400B0) || defined(CONFIG_MIPS_BCM7405A0)
	BDEV_SET(BCHP_CLK_PM_CTRL_1,
		BCHP_CLK_PM_CTRL_1_DIS_ENET_25M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL_1);

	BDEV_SET(BCHP_CLK_PM_CTRL, BCHP_CLK_PM_CTRL_DIS_RPTD_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL);
#endif
}

static void sata_enable(void)
{
	printk(KERN_INFO "brcm-pm: enabling power to SATA block\n");
#if defined(CONFIG_MIPS_BCM7400B0) || defined(CONFIG_MIPS_BCM7405A0)
	BDEV_UNSET(BCHP_CLK_PM_CTRL_2,
		BCHP_CLK_PM_CTRL_2_DIS_SATA_PCI_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL_2);

	BDEV_UNSET(BCHP_CLK_PM_CTRL, BCHP_CLK_PM_CTRL_DIS_SATA_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL);

	BDEV_UNSET(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1,
		BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1_sata_ana_pwrdn_MASK);
	BDEV_RD(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1);
#endif
}

static void sata_disable(void)
{
	printk(KERN_INFO "brcm-pm: disabling power to SATA block\n");
#if defined(CONFIG_MIPS_BCM7400B0) || defined(CONFIG_MIPS_BCM7405A0)
	BDEV_SET(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1,
		BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1_sata_ana_pwrdn_MASK);
	BDEV_RD(BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1);

	BDEV_SET(BCHP_CLK_PM_CTRL, BCHP_CLK_PM_CTRL_DIS_SATA_108M_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL);

	BDEV_SET(BCHP_CLK_PM_CTRL_2,
		BCHP_CLK_PM_CTRL_2_DIS_SATA_PCI_CLK_MASK);
	BDEV_RD(BCHP_CLK_PM_CTRL_2);
#endif
}

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

static struct seq_operations brcm_pm_seq_op;

static int brcm_pm_proc_open(struct inode *inode, struct file *file)
{
	return(seq_open(file, &brcm_pm_seq_op));
}

static int brcm_pm_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "usb: %d\n", atomic_read(&usb_count));
	seq_printf(m, "enet: %d\n", atomic_read(&enet_count));
	seq_printf(m, "sata: %d\n", atomic_read(&sata_count));
	return(0);
}

static void *brcm_pm_seq_start(struct seq_file *m, loff_t *pos)
{
	return((*pos < 1) ? pos : NULL);
}

static void *brcm_pm_seq_next(struct seq_file *m, void *v, loff_t *pos)
{
	++*pos;
	return(brcm_pm_seq_start(m, pos));
}

static void brcm_pm_seq_stop(struct seq_file *m, void *v)
{
}

static struct seq_operations brcm_pm_seq_op = {
	.show		= brcm_pm_proc_show,
	.start		= brcm_pm_seq_start,
	.next		= brcm_pm_seq_next,
	.stop		= brcm_pm_seq_stop,
};

static struct file_operations brcm_pm_file_op = {
	.open		= brcm_pm_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
};

static int __init brcm_pm_init(void)
{
	struct proc_dir_entry *p;

	/* power down all devices if nobody is using them */
	brcm_pm_usb_remove();
	brcm_pm_enet_remove();
	brcm_pm_sata_remove();

	p = create_proc_entry(PROC_ENTRY_NAME, 0444, NULL);
	if(! p) {
		printk(KERN_WARNING "brcm-pm: unable to create /proc entry\n");
	} else {
		p->proc_fops = &brcm_pm_file_op;
	}

	return(0);
}

static void __exit brcm_pm_exit(void)
{
	remove_proc_entry(PROC_ENTRY_NAME, NULL);

	/* power up all devices, then exit */
	brcm_pm_usb_add();
	brcm_pm_enet_add();
	brcm_pm_sata_add();
}

module_init(brcm_pm_init);
module_exit(brcm_pm_exit);
