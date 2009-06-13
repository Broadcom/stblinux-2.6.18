/*
 * arch/mips/brcm/setup.c
 *
 * Copyright (C) 2001 Broadcom Corporation
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
 * 10-01-2003   THT    Created
 */


#ifndef _BRCMSTB_H
#define _BRCMSTB_H

#include <linux/config.h>
#include <asm/types.h>		/* For phys_t declaration */

#if defined(CONFIG_MIPS_BCM3548A0)
#include <asm/brcmstb/brcm93548a0/bcmuart.h>
#include <asm/brcmstb/brcm93548a0/bcmtimer.h>
#include <asm/brcmstb/brcm93548a0/bcmebi.h>
#include <asm/brcmstb/brcm93548a0/int1.h>
#include <asm/brcmstb/brcm93548a0/board.h>
#include <asm/brcmstb/brcm93548a0/bchp_irq0.h>
#include <asm/brcmstb/brcm93548a0/bcmintrnum.h>
#include <asm/brcmstb/brcm93548a0/bchp_nand.h>
#include <asm/brcmstb/brcm93548a0/bchp_ebi.h>
#include <asm/brcmstb/brcm93548a0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm93548a0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm93548a0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm93548a0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm93548a0/bchp_bmips4380.h>
#include <asm/brcmstb/brcm93548a0/bchp_clkgen.h>
#include <asm/brcmstb/brcm93548a0/bchp_memc_0_ddr.h>
#include <asm/brcmstb/brcm93548a0/bchp_mspi.h>
#include <asm/brcmstb/brcm93548a0/bchp_bspi.h>
#include <asm/brcmstb/brcm93548a0/bchp_vcxo_ctl_misc.h>

#elif defined(CONFIG_MIPS_BCM3548B0)
#include <asm/brcmstb/brcm93548b0/bcmuart.h>
#include <asm/brcmstb/brcm93548b0/bcmtimer.h>
#include <asm/brcmstb/brcm93548b0/bcmebi.h>
#include <asm/brcmstb/brcm93548b0/int1.h>

#include <asm/brcmstb/brcm93548b0/board.h>
#include <asm/brcmstb/brcm93548b0/bchp_irq0.h>
#include <asm/brcmstb/brcm93548b0/bcmintrnum.h>
#include <asm/brcmstb/brcm93548b0/bchp_nand.h>
#include <asm/brcmstb/brcm93548b0/bchp_edu.h>
#include <asm/brcmstb/brcm93548b0/bchp_hif_intr2.h> /* For EDU interrupts */
#include <asm/brcmstb/brcm93548b0/bchp_ebi.h>
#include <asm/brcmstb/brcm93548b0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm93548b0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm93548b0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm93548b0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm93548b0/bchp_bmips4380.h>
#include <asm/brcmstb/brcm93548b0/bchp_clkgen.h>
#include <asm/brcmstb/brcm93548b0/bchp_memc_0_ddr.h>
#include <asm/brcmstb/brcm93548b0/bchp_mspi.h>
#include <asm/brcmstb/brcm93548b0/bchp_bspi.h>
#include <asm/brcmstb/brcm93548b0/bchp_vcxo_ctl_misc.h>


#elif defined(CONFIG_MIPS_BCM3563C0)
#include <asm/brcmstb/brcm93563c0/bcmuart.h>
#include <asm/brcmstb/brcm93563c0/bcmtimer.h>
#include <asm/brcmstb/brcm93563c0/bcmebi.h>
#include <asm/brcmstb/brcm93563c0/int1.h>
#include <asm/brcmstb/brcm93563c0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm93563c0/board.h>
#include <asm/brcmstb/brcm93563c0/bchp_irq0.h>
#include <asm/brcmstb/brcm93563c0/bcmintrnum.h>
#include <asm/brcmstb/brcm93563c0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm93563c0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm93563c0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm93563c0/bchp_nand.h>
#include <asm/brcmstb/brcm93563c0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm93563c0/bchp_ebi.h>

#define	BOOT_ROM_TYPE_STRAP_ADDR	(0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE_0)
#define	BOOT_ROM_TYPE_STRAP_MASK	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_nand_flash_MASK

#elif defined(CONFIG_MIPS_BCM7038C0)
#include <asm/brcmstb/brcm97038c0/bcmuart.h>
#include <asm/brcmstb/brcm97038c0/bcmtimer.h>
#include <asm/brcmstb/brcm97038c0/bcmebi.h>
#include <asm/brcmstb/brcm97038c0/int1.h>
#include <asm/brcmstb/brcm97038c0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97038c0/board.h>
#include <asm/brcmstb/brcm97038c0/bchp_irq0.h>
#include <asm/brcmstb/brcm97038c0/bcmintrnum.h>
#include <asm/brcmstb/brcm97038c0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97038c0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97038c0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97038c0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97038c0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97038c0/bchp_pci_bridge.h>

#elif defined(CONFIG_MIPS_BCM7118A0)
#include <asm/brcmstb/brcm97118a0/bcmuart.h>
#include <asm/brcmstb/brcm97118a0/bcmtimer.h>
#include <asm/brcmstb/brcm97118a0/bcmebi.h>
#include <asm/brcmstb/brcm97118a0/int1.h>
#include <asm/brcmstb/brcm97118a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97118a0/board.h>
#include <asm/brcmstb/brcm97118a0/bchp_irq0.h>
#include <asm/brcmstb/brcm97118a0/bcmintrnum.h>
#include <asm/brcmstb/brcm97118a0/bchp_nand.h>
#include <asm/brcmstb/brcm97118a0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97118a0/bchp_ebi.h>
#include <asm/brcmstb/brcm97118a0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97118a0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97118a0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97118a0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97118a0/bchp_pci_bridge.h>

#define	BOOT_ROM_TYPE_STRAP_ADDR	(0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE)
#define	BOOT_ROM_TYPE_STRAP_MASK	BCHP_SUN_TOP_CTRL_STRAP_VALUE_strap_nand_flash_MASK

#elif defined(CONFIG_MIPS_BCM7118C0)

#include <asm/brcmstb/brcm97118c0/bcmuart.h>
#include <asm/brcmstb/brcm97118c0/bcmtimer.h>
#include <asm/brcmstb/brcm97118c0/bcmebi.h>
#include <asm/brcmstb/brcm97118c0/int1.h>
#include <asm/brcmstb/brcm97118c0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97118c0/board.h>
#include <asm/brcmstb/brcm97118c0/bchp_irq0.h>
#include <asm/brcmstb/brcm97118c0/bcmintrnum.h>
#include <asm/brcmstb/brcm97118c0/bchp_nand.h>
#include <asm/brcmstb/brcm97118c0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97118c0/bchp_ebi.h>
#include <asm/brcmstb/brcm97118c0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97118c0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97118c0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97118c0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97118c0/bchp_pci_bridge.h>
#include <asm/brcmstb/brcm97118c0/bcmtimer.h>

#define	BOOT_ROM_TYPE_STRAP_ADDR	(0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE)
#define	BOOT_ROM_TYPE_STRAP_MASK	BCHP_SUN_TOP_CTRL_STRAP_VALUE_strap_nand_flash_MASK

/* The following bchp headers are taken from brcm97400e0 rdb. Please look at PR 53107 for more information */
#elif defined(CONFIG_MIPS_BCM7400D0)
#include <asm/brcmstb/brcm97400d0/bcmuart.h>
#include <asm/brcmstb/brcm97400d0/bcmtimer.h>
#include <asm/brcmstb/brcm97400d0/bcmebi.h>
#include <asm/brcmstb/brcm97400d0/int1.h>
#include <asm/brcmstb/brcm97400d0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97400d0/board.h>
#include <asm/brcmstb/brcm97400d0/bchp_irq0.h>
#include <asm/brcmstb/brcm97400d0/bcmintrnum.h>
#include <asm/brcmstb/brcm97400d0/bchp_nand.h>
#include <asm/brcmstb/brcm97400d0/bchp_ebi.h>
#include <asm/brcmstb/brcm97400d0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97400d0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97400d0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97400d0/bchp_usb_ehci1.h>
#include <asm/brcmstb/brcm97400d0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97400d0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97400d0/bchp_pcix_bridge.h>
#include <asm/brcmstb/brcm97400d0/bchp_clk.h>
#include <asm/brcmstb/brcm97400d0/bchp_memc_0_ddr.h>

#define	BOOT_ROM_TYPE_STRAP_ADDR	(0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE_0)
#define	BOOT_ROM_TYPE_STRAP_MASK	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_rom_type_MASK


#elif defined(CONFIG_MIPS_BCM7405B0)
#include <asm/brcmstb/brcm97405b0/bcmuart.h>
#include <asm/brcmstb/brcm97405b0/bcmtimer.h>
#include <asm/brcmstb/brcm97405b0/bcmebi.h>
#include <asm/brcmstb/brcm97405b0/int1.h>
#include <asm/brcmstb/brcm97405b0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97405b0/board.h>
#include <asm/brcmstb/brcm97405b0/bchp_irq0.h>
#include <asm/brcmstb/brcm97405b0/bcmintrnum.h>
#include <asm/brcmstb/brcm97405b0/bchp_nand.h>
#include <asm/brcmstb/brcm97405b0/bchp_ebi.h>
#include <asm/brcmstb/brcm97405b0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97405b0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97405b0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97405b0/bchp_usb_ehci1.h>
#include <asm/brcmstb/brcm97405b0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97405b0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97405b0/bchp_pcix_bridge.h>
#include <asm/brcmstb/brcm97405b0/bchp_clk.h>
#include <asm/brcmstb/brcm97405b0/bchp_bmips4380.h>
#include <asm/brcmstb/brcm97405b0/bchp_memc_0_ddr.h>

#define	BOOT_ROM_TYPE_STRAP_ADDR	(0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE_0)
#define	BOOT_ROM_TYPE_STRAP_MASK	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_rom_type_MASK


#elif defined(CONFIG_MIPS_BCM7405D0)
#include <asm/brcmstb/brcm97405d0/bcmuart.h>
#include <asm/brcmstb/brcm97405d0/bcmtimer.h>
#include <asm/brcmstb/brcm97405d0/bcmebi.h>
#include <asm/brcmstb/brcm97405d0/int1.h>
#include <asm/brcmstb/brcm97405d0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97405d0/board.h>
#include <asm/brcmstb/brcm97405d0/bchp_irq0.h>
#include <asm/brcmstb/brcm97405d0/bcmintrnum.h>
#include <asm/brcmstb/brcm97405d0/bchp_nand.h>
#include <asm/brcmstb/brcm97405d0/bchp_ebi.h>
#include <asm/brcmstb/brcm97405d0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97405d0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97405d0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97405d0/bchp_usb_ehci1.h>
#include <asm/brcmstb/brcm97405d0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97405d0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97405d0/bchp_pcix_bridge.h>
#include <asm/brcmstb/brcm97405d0/bchp_clk.h>
#include <asm/brcmstb/brcm97405d0/bchp_bmips4380.h>
#include <asm/brcmstb/brcm97405d0/bchp_memc_0_ddr.h>

#define	BOOT_ROM_TYPE_STRAP_ADDR	(0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE_0)
#define	BOOT_ROM_TYPE_STRAP_MASK	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_rom_type_MASK


#elif defined(CONFIG_MIPS_BCM7335B0)
#include <asm/brcmstb/brcm97335b0/bcmuart.h>
#include <asm/brcmstb/brcm97335b0/bcmtimer.h>
#include <asm/brcmstb/brcm97335b0/bcmebi.h>
#include <asm/brcmstb/brcm97335b0/int1.h>
#include <asm/brcmstb/brcm97335b0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97335b0/board.h>
#include <asm/brcmstb/brcm97335b0/bchp_irq0.h>
#include <asm/brcmstb/brcm97335b0/bcmintrnum.h>
#include <asm/brcmstb/brcm97335b0/bchp_nand.h>
#include <asm/brcmstb/brcm97335b0/bchp_ebi.h>
#include <asm/brcmstb/brcm97335b0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97335b0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97335b0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97335b0/bchp_usb_ehci1.h>
#include <asm/brcmstb/brcm97335b0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97335b0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97335b0/bchp_pcix_bridge.h>
#include <asm/brcmstb/brcm97335b0/bchp_clk.h>
#include <asm/brcmstb/brcm97335b0/bchp_bmips4380.h>
#include <asm/brcmstb/brcm97335b0/bchp_memc_0_ddr.h>
#include <asm/brcmstb/brcm97335b0/bchp_vcxo_ctl_misc.h>

#elif defined(CONFIG_MIPS_BCM7336A0)
#include <asm/brcmstb/brcm97336a0/bcmuart.h>
#include <asm/brcmstb/brcm97336a0/bcmtimer.h>
#include <asm/brcmstb/brcm97336a0/bcmebi.h>
#include <asm/brcmstb/brcm97336a0/int1.h>
#include <asm/brcmstb/brcm97336a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97336a0/board.h>
#include <asm/brcmstb/brcm97336a0/bchp_irq0.h>
#include <asm/brcmstb/brcm97336a0/bcmintrnum.h>
#include <asm/brcmstb/brcm97336a0/bchp_nand.h>
#include <asm/brcmstb/brcm97336a0/bchp_ebi.h>
#include <asm/brcmstb/brcm97336a0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97336a0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97336a0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97336a0/bchp_usb_ehci1.h>
#include <asm/brcmstb/brcm97336a0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97336a0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97336a0/bchp_pcix_bridge.h>
#include <asm/brcmstb/brcm97336a0/bchp_clk.h>
#include <asm/brcmstb/brcm97336a0/bchp_bmips4380.h>
#include <asm/brcmstb/brcm97336a0/bchp_memc_0_ddr.h>
#include <asm/brcmstb/brcm97336a0/bchp_vcxo_ctl_misc.h>

#elif defined(CONFIG_MIPS_BCM7340A0)
#include <asm/brcmstb/brcm97340a0/bcmuart.h>
#include <asm/brcmstb/brcm97340a0/bcmtimer.h>
#include <asm/brcmstb/brcm97340a0/bcmebi.h>
#include <asm/brcmstb/brcm97340a0/int1.h>
#include <asm/brcmstb/brcm97340a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97340a0/board.h>
#include <asm/brcmstb/brcm97340a0/bchp_irq0.h>
#include <asm/brcmstb/brcm97340a0/bcmintrnum.h>
#include <asm/brcmstb/brcm97340a0/bchp_nand.h>
#include <asm/brcmstb/brcm97340a0/bchp_ebi.h>
#include <asm/brcmstb/brcm97340a0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97340a0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97340a0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97340a0/bchp_usb_ehci1.h>
#include <asm/brcmstb/brcm97340a0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97340a0/bchp_usb_ohci1.h>
// #include <asm/brcmstb/brcm97340a0/bchp_pcix_bridge.h>
#include <asm/brcmstb/brcm97340a0/bchp_clkgen.h>
#include <asm/brcmstb/brcm97340a0/bchp_bmips4380.h>
#include <asm/brcmstb/brcm97340a0/bchp_memc_ddr_0.h>
#include <asm/brcmstb/brcm97340a0/bchp_vcxo_ctl_misc.h>


#elif defined(CONFIG_MIPS_BCM7420A0)
#include <asm/brcmstb/brcm97420a0/bcmuart.h>
#include <asm/brcmstb/brcm97420a0/bcmtimer.h>
#include <asm/brcmstb/brcm97420a0/bcmebi.h>
#include <asm/brcmstb/brcm97420a0/int1.h>
#include <asm/brcmstb/brcm97420a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97420a0/board.h>
#include <asm/brcmstb/brcm97420a0/bchp_irq0.h>
#include <asm/brcmstb/brcm97420a0/bcmintrnum.h>
#include <asm/brcmstb/brcm97420a0/bchp_nand.h>
#include <asm/brcmstb/brcm97420a0/bchp_edu.h>
#include <asm/brcmstb/brcm97420a0/bchp_hif_intr2.h> /* For EDU interrupts */
#include <asm/brcmstb/brcm97420a0/bchp_ebi.h>
#include <asm/brcmstb/brcm97420a0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97420a0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97420a0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97420a0/bchp_usb_ehci1.h>
#include <asm/brcmstb/brcm97420a0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97420a0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97420a0/bchp_pcix_bridge.h>
#include <asm/brcmstb/brcm97420a0/bchp_clk.h>
#include <asm/brcmstb/brcm97420a0/bchp_memc_0_ddr.h>
#include <asm/brcmstb/brcm97420a0/bchp_pcie_dma.h>
#include <asm/brcmstb/brcm97420a0/bchp_pcie_intr2.h>
#include <asm/brcmstb/brcm97420a0/bchp_pcie_misc.h>
#include <asm/brcmstb/brcm97420a0/bchp_pcie_misc_perst.h>
#include <asm/brcmstb/brcm97420a0/bchp_pcie_rc_cfg_type1.h>
#include <asm/brcmstb/brcm97420a0/bchp_pcie_rc_cfg_vendor.h>
#include <asm/brcmstb/brcm97420a0/bchp_hif_rgr1.h>
#include <asm/brcmstb/brcm97420a0/bchp_mips_biu.h>
#include <asm/brcmstb/brcm97420a0/bchp_moca_hostmisc.h>

#define BOOT_ROM_TYPE_STRAP_BOOT_SHAPE_ADDR (0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE_0)
#define BOOT_ROM_TYPE_STRAP_BOOT_SHAPE_MASK BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_MASK

#elif defined(CONFIG_MIPS_BCM7420B0)
#include <asm/brcmstb/brcm97420b0/bcmuart.h>
#include <asm/brcmstb/brcm97420b0/bcmtimer.h>
#include <asm/brcmstb/brcm97420b0/bcmebi.h>
#include <asm/brcmstb/brcm97420b0/int1.h>
#include <asm/brcmstb/brcm97420b0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97420b0/board.h>
#include <asm/brcmstb/brcm97420b0/bchp_irq0.h>
#include <asm/brcmstb/brcm97420b0/bcmintrnum.h>
#include <asm/brcmstb/brcm97420b0/bchp_nand.h>
#include <asm/brcmstb/brcm97420b0/bchp_edu.h>
#include <asm/brcmstb/brcm97420b0/bchp_hif_intr2.h> /* For EDU interrupts */
#include <asm/brcmstb/brcm97420b0/bchp_ebi.h>
#include <asm/brcmstb/brcm97420b0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97420b0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97420b0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97420b0/bchp_usb_ehci1.h>
#include <asm/brcmstb/brcm97420b0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97420b0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97420b0/bchp_pcix_bridge.h>
#include <asm/brcmstb/brcm97420b0/bchp_clk.h>
#include <asm/brcmstb/brcm97420b0/bchp_memc_ddr_0.h>
#include <asm/brcmstb/brcm97420b0/bchp_pcie_dma.h>
#include <asm/brcmstb/brcm97420b0/bchp_pcie_intr2.h>
#include <asm/brcmstb/brcm97420b0/bchp_pcie_misc.h>
#include <asm/brcmstb/brcm97420b0/bchp_pcie_misc_perst.h>
#include <asm/brcmstb/brcm97420b0/bchp_pcie_rc_cfg_type1.h>
#include <asm/brcmstb/brcm97420b0/bchp_pcie_rc_cfg_vendor.h>
#include <asm/brcmstb/brcm97420b0/bchp_hif_rgr1.h>
#include <asm/brcmstb/brcm97420b0/bchp_mips_biu.h>
#include <asm/brcmstb/brcm97420b0/bchp_moca_hostmisc.h>
#include <asm/brcmstb/brcm97420b0/bchp_wktmr.h>
#include <asm/brcmstb/brcm97420b0/bchp_hif_mspi.h>
#include <asm/brcmstb/brcm97420b0/bchp_hif_spi_intr2.h>
#include <asm/brcmstb/brcm97420b0/bchp_bspi.h>
#include <asm/brcmstb/brcm97420b0/bchp_bspi_raf.h>

#define BOOT_ROM_TYPE_STRAP_BOOT_SHAPE_ADDR (0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE_0)
#define BOOT_ROM_TYPE_STRAP_BOOT_SHAPE_MASK BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_MASK

#elif defined(CONFIG_MIPS_BCM7325B0)
#include <asm/brcmstb/brcm97325b0/bcmuart.h>
#include <asm/brcmstb/brcm97325b0/bcmtimer.h>
#include <asm/brcmstb/brcm97325b0/bcmebi.h>
#include <asm/brcmstb/brcm97325b0/int1.h>
#include <asm/brcmstb/brcm97325b0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97325b0/board.h>
#include <asm/brcmstb/brcm97325b0/bchp_irq0.h>
#include <asm/brcmstb/brcm97325b0/bchp_irq1.h>
#include <asm/brcmstb/brcm97325b0/bcmintrnum.h>
#include <asm/brcmstb/brcm97325b0/bchp_nand.h>
#include <asm/brcmstb/brcm97325b0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97325b0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97325b0/bchp_usb_ehci1.h>
#include <asm/brcmstb/brcm97325b0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97325b0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97325b0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97325b0/bchp_memc_0_ddr.h>
#include <asm/brcmstb/brcm97325b0/bchp_clkgen.h>
#include <asm/brcmstb/brcm97325b0/bchp_vcxo_ctl_misc.h>

#elif defined(CONFIG_MIPS_BCM7401C0) || defined(CONFIG_MIPS_BCM7402C0)
#include <asm/brcmstb/brcm97401c0/bcmuart.h>
#include <asm/brcmstb/brcm97401c0/bcmtimer.h>
#include <asm/brcmstb/brcm97401c0/bcmebi.h>
#include <asm/brcmstb/brcm97401c0/int1.h>
#include <asm/brcmstb/brcm97401c0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97401c0/board.h>
#include <asm/brcmstb/brcm97401c0/bchp_irq0.h>
#include <asm/brcmstb/brcm97401c0/bcmintrnum.h>
#include <asm/brcmstb/brcm97401c0/bchp_nand.h>
#include <asm/brcmstb/brcm97401c0/bchp_ebi.h>
#include <asm/brcmstb/brcm97401c0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97401c0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97401c0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97401c0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97401c0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97401c0/bchp_pci_bridge.h>
#include <asm/brcmstb/brcm97401c0/bchp_memc_0_ddr.h>

#define	BOOT_ROM_TYPE_STRAP_ADDR	(0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE)
#define	BOOT_ROM_TYPE_STRAP_MASK	BCHP_SUN_TOP_CTRL_STRAP_VALUE_strap_nand_flash_MASK	
	

#elif defined(CONFIG_MIPS_BCM7403A0)
#include <asm/brcmstb/brcm97403a0/bcmuart.h>
#include <asm/brcmstb/brcm97403a0/bcmtimer.h>
#include <asm/brcmstb/brcm97403a0/bcmebi.h>
#include <asm/brcmstb/brcm97403a0/int1.h>
#include <asm/brcmstb/brcm97403a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97403a0/board.h>
#include <asm/brcmstb/brcm97403a0/bchp_irq0.h>
#include <asm/brcmstb/brcm97403a0/bcmintrnum.h>
#include <asm/brcmstb/brcm97403a0/bchp_nand.h>
#include <asm/brcmstb/brcm97403a0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97403a0/bchp_ebi.h>
#include <asm/brcmstb/brcm97403a0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97403a0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97403a0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97403a0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97403a0/bchp_pci_bridge.h>

#define	BOOT_ROM_TYPE_STRAP_ADDR	(0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE)
#define	BOOT_ROM_TYPE_STRAP_MASK	BCHP_SUN_TOP_CTRL_STRAP_VALUE_strap_nand_flash_MASK	

#elif defined(CONFIG_MIPS_BCM7440B0)
#include <asm/brcmstb/brcm97440b0/bcmuart.h>
#include <asm/brcmstb/brcm97440b0/bcmtimer.h>
#include <asm/brcmstb/brcm97440b0/bcmebi.h>
#include <asm/brcmstb/brcm97440b0/int1.h>
#include <asm/brcmstb/brcm97440b0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97440b0/board.h>
#include <asm/brcmstb/brcm97440b0/bchp_irq0.h>
#include <asm/brcmstb/brcm97440b0/bchp_irq1.h>
#include <asm/brcmstb/brcm97440b0/bchp_hif_cpu_intr1.h>
#include <asm/brcmstb/brcm97440b0/bcmintrnum.h>
#include <asm/brcmstb/brcm97440b0/bchp_nand.h>
#include <asm/brcmstb/brcm97440b0/bchp_hif_intr2.h>
#include <asm/brcmstb/brcm97440b0/bchp_edu.h>
#include <asm/brcmstb/brcm97440b0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97440b0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97440b0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97440b0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97440b0/bchp_pci_bridge.h>
#include <asm/brcmstb/brcm97440b0/boardmap.h>		/* BCM7440b0 address space is special */

#define	BOOT_ROM_TYPE_STRAP_ADDR	(0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE)
#define	BOOT_ROM_TYPE_STRAP_MASK	BCHP_SUN_TOP_CTRL_STRAP_VALUE_strap_nand_flash_MASK	


#elif defined(CONFIG_MIPS_BCM7443A0)
#include <asm/brcmstb/brcm97443a0/bcmuart.h>
#include <asm/brcmstb/brcm97443a0/bcmtimer.h>
#include <asm/brcmstb/brcm97443a0/bcmebi.h>
#include <asm/brcmstb/brcm97443a0/int1.h>
#include <asm/brcmstb/brcm97443a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97443a0/board.h>
#include <asm/brcmstb/brcm97443a0/bchp_irq0.h>
#include <asm/brcmstb/brcm97443a0/bchp_irq1.h>
#include <asm/brcmstb/brcm97443a0/bcmintrnum.h>
#include <asm/brcmstb/brcm97443a0/bchp_nand.h>
#include <asm/brcmstb/brcm97443a0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97443a0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97443a0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97443a0/bchp_sun_top_ctrl.h>

#include <asm/brcmstb/brcm97443a0/boardmap.h>		/* BCM744x address space are special */


#define	BOOT_ROM_TYPE_STRAP_ADDR	(0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE_0)
#define	BOOT_ROM_TYPE_STRAP_MASK	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_nand_flash_MASK	

#elif defined(CONFIG_MIPS_BCM7601B0)
#include <asm/brcmstb/brcm97601b0/bcmuart.h>
#include <asm/brcmstb/brcm97601b0/bcmtimer.h>
#include <asm/brcmstb/brcm97601b0/bcmebi.h>
#include <asm/brcmstb/brcm97601b0/int1.h>
#include <asm/brcmstb/brcm97601b0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97601b0/board.h>
#include <asm/brcmstb/brcm97601b0/bchp_irq0.h>
#include <asm/brcmstb/brcm97601b0/bchp_irq1.h>
#include <asm/brcmstb/brcm97601b0/bchp_hif_cpu_intr1.h>
#include <asm/brcmstb/brcm97601b0/bcmintrnum.h>
#include <asm/brcmstb/brcm97601b0/bchp_edu.h>
#include <asm/brcmstb/brcm97601b0/bchp_hif_intr2.h>
#include <asm/brcmstb/brcm97601b0/bchp_nand.h>
#include <asm/brcmstb/brcm97601b0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97601b0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97601b0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97601b0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97601b0/bchp_pcix_bridge.h>
#include <asm/brcmstb/brcm97601b0/boardmap.h>		/* BCM7601b0 address space is special */

#define	BOOT_ROM_TYPE_STRAP_ADDR	(0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE_0)
#define	BOOT_ROM_TYPE_STRAP_MASK	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_nand_flash_MASK

#elif defined(CONFIG_MIPS_BCM7635A0)
#include <asm/brcmstb/brcm97635a0/bcmuart.h>
#include <asm/brcmstb/brcm97635a0/bcmtimer.h>
#include <asm/brcmstb/brcm97635a0/bcmebi.h>
#include <asm/brcmstb/brcm97635a0/int1.h>
#include <asm/brcmstb/brcm97635a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97635a0/board.h>
#include <asm/brcmstb/brcm97635a0/bchp_irq0.h>
#include <asm/brcmstb/brcm97635a0/bchp_irq1.h>
#include <asm/brcmstb/brcm97635a0/bchp_hif_cpu_intr1.h>
#include <asm/brcmstb/brcm97635a0/bcmintrnum.h>
#include <asm/brcmstb/brcm97635a0/bchp_edu.h>
#include <asm/brcmstb/brcm97635a0/bchp_hif_intr2.h>
#include <asm/brcmstb/brcm97635a0/bchp_nand.h>
#include <asm/brcmstb/brcm97635a0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97635a0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97635a0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97635a0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97635a0/bchp_pcix_bridge_0.h>
#include <asm/brcmstb/brcm97635a0/boardmap.h>		/* BCM7635a0 address space is special */

#define	BOOT_ROM_TYPE_STRAP_ADDR	(0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE_0)
#define	BOOT_ROM_TYPE_STRAP_MASK	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_nand_flash_MASK

#else
#error "unknown BCM STB chip!!!"
#endif


//THT: PR41560, leave this one in.  In the case of bootrom=NOR, we will get the MAC_ADDR from NOR.
#if defined(BOOT_ROM_TYPE_STRAP_ADDR) && defined(BOOT_ROM_TYPE_STRAP_ADDR)
#define	is_bootrom_nand()		\
	(((*(volatile unsigned long *)BOOT_ROM_TYPE_STRAP_ADDR) & BOOT_ROM_TYPE_STRAP_MASK) ==  BOOT_ROM_TYPE_STRAP_MASK)

#elif defined(BRCM_SPI_CHIP_SELECT) && defined(BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_nand_flash_boot_MASK)
/* SPI based STBs */
#define	is_bootrom_nand()		\
  	((BDEV_RD(BCHP_SUN_TOP_CTRL_STRAP_VALUE_0) & \
                BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_nand_flash_boot_MASK) \
                == BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_nand_flash_boot_MASK)

#elif defined(BOOT_ROM_TYPE_STRAP_BOOT_SHAPE_ADDR) && defined(BOOT_ROM_TYPE_STRAP_BOOT_SHAPE_MASK)
/* 7420 and later */

// Later

#else
#define	is_bootrom_nand()		(0)	// default bootrom is NOR
#endif


#ifndef __ASSEMBLY__

#include <linux/spinlock.h>




struct brcm_reg_array {
	unsigned long		reg;
	unsigned long		mask;
	unsigned long		value;
};

extern void (*irq_setup)(void);
extern void uart_puts(const char*);

/* Only used for BCM7440B0 */
typedef struct si_bm_rsvd {
	phys_t addr;
	phys_t size;
	long type;
	int next, prev;
} bcm_memnode_t;

typedef int (*walk_cb_t)(unsigned long paddr, unsigned long size, long type, void* cbdata);
extern int brcm_walk_boot_mem_map(void* cbdata, walk_cb_t walk_cb);

/*
 * Returns total main memory in KSEG0 address space.
 */
extern unsigned long get_RAM_size(void); 

/*
 * Returns the reserved memory (not used by the kernel)
 */
extern unsigned long get_RSVD_size(void);

/*
 * bcm_discontigMem_t
 * Describes the physical memory layout of the chip/board
 */
#define BCM_MAX_MEM_REGION 8
typedef struct bcm_discontigMem {
	int nDdrCtrls;			/* Number of DDR controller or memory banks */
	unsigned long physAddr[BCM_MAX_MEM_REGION];	/* Physical Address of memory banks */
	unsigned long memSize[BCM_MAX_MEM_REGION];	/* Size of memory banks */
} bcm_discontigMem_t;
extern bcm_discontigMem_t* bcm_pdiscontig_memmap;

/*
 * Set up the TLB map
 * Requires #include <asm/mipsregs.h> for TLB page masks
 * Describe the Physical -> Virtual TLB mapping of the board.  
 * Uses information from bcm_discontigMem_t.
 * 
 * The design is such that the standard layout can be overwritten by 
 * board-specific layout defined in arch/mips/brcmstb/<platform>/board.c.
 */
typedef struct bcm_memmap {
	unsigned int tlb_mask;			/* Largest size of TLB page map, typically 4K - 64MB */
	unsigned long pci_vAddr;		/* Virtual Address for PCI mem address space */
	unsigned long pci_winSize;		/* SIze of PCI window, typically 256MB */
	unsigned long io_vAddr;		/* Virtual Address for PCI IO address space */
	unsigned long io_winSize;		/* Standard 0x0060_000b for all platforms */
	int nMemBanks;				/* How many virtual Address banks */
	unsigned long mem_vAddr[BCM_MAX_MEM_REGION];
	void (*tlb_memmap)(void);	/* Set up memory map */
	void (*tlb_pci_bridge)(void);		/* Set up PCI & IO map */
	void (*tlb_sata_bridge)(void);	/* Set up SATA bridge */
} bcm_memmap_t;
extern bcm_memmap_t* bcm_pmemmap;

/*
 * By default will return 0.  It's up to individual boards to
 * override the default, by overloading the function pointer.
 * returns the amount of memory not accounted for by get_RAM_size();
 */
extern unsigned long (* __get_discontig_RAM_size) (void);


/*------------ CFE Env Vars for Nand partition ---------------*/
#define MAX_CFE_PART_ENV_VARS 6

typedef enum  { 
	ROOTFS_PT=0, 	/* LINUX_FFS_STARTAD, LINUX_FFS_SSIZE */
	SPLASH_PT=1,	/* SPLASH_PART_STARTAD, SPLASH_PART_SIZE */
	KERNEL_PT=2,  	/* LINUX_PART_STARTAD, LINUX_PART_SIZE */
	OCAP_PT=3, 		/* OCAP_PART_STARTAD, OCAP_PART_SIZE */
	LAST_PT
} eCfePartEnvVar_t;

typedef struct {
	int numParts;
	struct {
		eCfePartEnvVar_t part; /* partition enum */
		uint32_t offset; /* Offset from start of NAND.  For DRAM, this is physical offset */
		uint32_t size;
	} parts[MAX_CFE_PART_ENV_VARS];
} cfePartitions_t;

typedef struct {
	const char* offset;
	const char* size;
} cfeEnvVarPairs_t;

extern cfePartitions_t gCfePartitions; 
extern cfeEnvVarPairs_t gCfeEnvVarPairs[];

extern void bcm_get_cfe_partition_env(void);

/*----------------------------------------------------*/

struct bcmumac_platform_data {
	int			phy_type;
	int			phy_id;
	unsigned char		macaddr[6];
};

#define BRCM_PHY_ID_AUTO	-1
#define BRCM_PHY_ID_NONE	-2

#define BRCM_PHY_TYPE_INT	1
#define BRCM_PHY_TYPE_EXT_MII	2
#define BRCM_PHY_TYPE_EXT_GMII	3
#define BRCM_PHY_TYPE_EXT_GMII_IBS	4
#define BRCM_PHY_TYPE_MOCA	5
#define BRCM_PHY_TYPE_SWITCH	6

/*
 * BRCM_SATA_SUPPORTED will be defined on platforms that have the SATA
 * registers in RDB (e.g. 7405, 7406, 7401, 7402, 7118RNG).  It will not
 * be defined on platforms that lack the SATA registers entirely (e.g. 3548).
 */

#if defined(BCHP_PCI_BRIDGE_PCI_CTRL) || defined(BCHP_PCIX_BRIDGE_PCIX_CTRL)
#define BRCM_SATA_SUPPORTED	1
#endif

#if defined(CONFIG_MIPS_BCM7420)
#define BRCM_75MHZ_SATA_PLL	1
#endif

/* NOTE: 7118 special case is handled in prom.c */

#if defined(BCHP_ENET_TOP_REG_START) || defined(BCHP_EMAC_0_REG_START) || \
	defined(CONFIG_MIPS_BCM7038)
#define BRCM_ENET_SUPPORTED	1
#endif

#if defined(BCHP_EMAC_1_REG_START) && ! defined(CONFIG_MIPS_BCM7405A0)
#define BRCM_EMAC_1_SUPPORTED	1
#endif

#if defined(BCHP_PCI_CFG_STATUS_COMMAND) && ! defined(CONFIG_BRCM_PCI_SLAVE)
#define BRCM_PCI_SUPPORTED	1
#endif

#if defined(BCHP_PCIE_MISC_MISC_CTRL) && ! defined(CONFIG_BRCM_PCI_SLAVE)
#define BRCM_PCIE_SUPPORTED	1
#endif

#if defined(BCHP_DATA_MEM_REG_START)
#define BRCM_MOCA_SUPPORTED	1
#define BRCM_MOCA_REG_START	BCHP_DATA_MEM_REG_START
#define BRCM_MOCA_REG_END	BCHP_MOCA_MOCAM2M_REG_END
#endif

#ifdef CONFIG_MIPS_BCM7420A0
/* old A0 GENET */
#if defined(BCHP_GENET_UMAC_REG_START)
#define BRCM_UMAC_0_SUPPORTED	1
#define BRCM_UMAC_0_REG_START	BCHP_GENET_UMAC_REG_START
#define BRCM_UMAC_0_REG_END	BCHP_GENET_SCB_REG_END
#endif

#if defined(BCHP_MOCA_GENET_UMAC_REG_START)
#define BRCM_UMAC_1_SUPPORTED	1
#define BRCM_UMAC_1_REG_START	BCHP_MOCA_GENET_UMAC_REG_START
#define BRCM_UMAC_1_REG_END	BCHP_MOCA_GENET_SCB_REG_END
#endif
#else /* CONFIG_MIPS_BCM7420A0 */
#if defined(BCHP_GENET_UMAC_REG_START)
#define BRCM_UMAC_0_SUPPORTED	1
#define BRCM_UMAC_0_REG_START	BCHP_GENET_UMAC_REG_START
#define BRCM_UMAC_0_REG_END	BCHP_GENET_TDMA_REG_END
#endif

#if defined(BCHP_MOCA_GENET_UMAC_REG_START)
#define BRCM_UMAC_1_SUPPORTED	1
#define BRCM_UMAC_1_REG_START	BCHP_MOCA_GENET_UMAC_REG_START
#define BRCM_UMAC_1_REG_END	BCHP_MOCA_GENET_TDMA_REG_END
#endif
#endif /* CONFIG_MIPS_BCM7420A0 */

/* BCM3548, BCM7420, and later chips do not have straps for memory size */
#if defined(CONFIG_MIPS_BCM3560) || defined(CONFIG_MIPS_BCM7038) || \
	defined(CONFIG_MIPS_BCM7118) || defined(CONFIG_MIPS_BCM7325) || \
	defined(CONFIG_MIPS_BCM7335) || defined(CONFIG_MIPS_BCM7400) || \
	defined(CONFIG_MIPS_BCM7401) || defined(CONFIG_MIPS_BCM7402) || \
	defined(CONFIG_MIPS_BCM7403) || defined(CONFIG_MIPS_BCM7405) || \
	defined(CONFIG_MIPS_BCM7440) || defined(CONFIG_MIPS_BCM7601) || \
	defined(CONFIG_MIPS_BCM7336) || defined(CONFIG_MIPS_BCM7340) ||	\
	defined(CONFIG_MIPS_BCM7635)

#define BRCM_MEMORY_STRAPS	1
#endif

/* MSPI SS deassertion bug in BCM3548A0 and earlier */
#if defined(BCHP_MSPI_SPCR2) && ! defined(BCHP_MSPI_SPCR2_cont_after_cmd_MASK)
#define BRCM_SPI_SS_WAR		1
#endif

/* e.g. BCM9745x */
extern int brcm_docsis_platform;

/*
 * If WIRED_PCI_MAPPING is defined, TLB entries are permanently dedicated to
 * mapping the PCI memory region into upper memory.  Side effects of this
 * include:
 *  - Accesses to PCI memory will never cause a page fault
 *  - ioremap() is essentially a no-op for this range
 *  - Fewer TLB entries are available for general use, possibly causing
 *    thrashing
 *  - Only 256MB (sometimes 128MB) of PCI space can be supported in the
 *    discontiguous memory model, because the kernel virtual address space
 *    at e000_0000 is needed to map upper memory
 *  - Not compatible with PCI Express, since PCIe adds a separate 512MB
 *    PCI memory region of its own
 *
 * If WIRED_PCI_MAPPING is not defined, ioremap() will map the PCI space into
 * the vmalloc region on an as-needed basis.  The vmalloc region in this
 * configuration will cover the former PCI region (e.g. d000_0000 to
 * efff_ffff or dfff_ffff).
 *
 * Note that BMIPS3300 and MIPS R5k do not support the 64MB page mask, so
 * a large number of 16MB TLB entries will be required to map the entire range.
 *
 * The PCI I/O region always has a wired mapping, regardless of this setting.
 */

#if defined(CONFIG_BRCM_COMMON_PCI)
#if (defined(CONFIG_BMIPS4380) || defined(CONFIG_MTI_R34K)) && \
	! (defined(CONFIG_DISCONTIGMEM) || defined(BRCM_PCIE_SUPPORTED))
#define WIRED_PCI_MAPPING	1
#endif
#else /* CONFIG_BRCM_COMMON_PCI */
#define WIRED_PCI_MAPPING	1
#endif

/* NOTE: all PHYSICAL addresses */
#define PCI_MEM_START		0xd0000000
#define PCI_MEM_SIZE		0x20000000
#define PCI_MEM_END		(PCI_MEM_START + PCI_MEM_SIZE - 1)

#define PCI_IO_START		0xf0000000
#define PCI_IO_SIZE		0x00600000
#define PCI_IO_END		(PCI_IO_START + PCI_IO_SIZE - 1)

#if defined(BCHP_PCIX_BRIDGE_GRB_REG_START)
#define PCI_SATA_MEM_START	(BCHP_PCIX_BRIDGE_GRB_REG_START + 0x10010000)
#define PCI_SATA_IO_START	(BCHP_PCIX_BRIDGE_GRB_REG_START + 0x10020000)
#else
#define PCI_SATA_MEM_START	0x10510000
#define PCI_SATA_IO_START	0x10520000
#endif

#define PCI_SATA_MEM_SIZE	0x00010000
#define PCI_SATA_MEM_END	(PCI_SATA_MEM_START + PCI_SATA_MEM_SIZE - 1)

#define PCI_SATA_IO_SIZE	0x00000200

#define MIPS_PCI_XCFG_INDEX	0xf0600004
#define MIPS_PCI_XCFG_DATA	0xf0600008

#define PCIE_MEM_START		0xa0000000
#define PCIE_MEM_SIZE		0x20000000
#define PCIE_MEM_END		(PCIE_MEM_START + PCIE_MEM_SIZE - 1)

#define PCIE_IO_START		0xf1000000
#define MIPS_PCIE_XCFG_INDEX	(PCIE_IO_START + 0x00)
#define MIPS_PCIE_XCFG_DATA	(PCIE_IO_START + 0x04)

/*
 * brcm_sata_enabled can be 0 on platforms that have a non-SATA variant:
 * 7406, 7118RNG, 7402, 7404/7452, etc.  This is detected at runtime.
 * Same basic idea for brcm_enet_enabled (7454 and others).
 */
extern int brcm_sata_enabled;
extern int brcm_enet_enabled;

/* other optional features */
extern int brcm_emac_1_enabled;
extern int brcm_pci_enabled;
extern int brcm_smp_enabled;

/* external PHY does not support MDIO (e.g. 97405-MSG) */
extern int brcm_enet_no_mdio;

/* 97455, 97456, 97458, 97459, etc. */
extern int brcm_docsis_platform;

/* EBI bug workaround */
extern int brcm_ebi_war;

/*
 * Sample usage:
 *
 * DEV_RD(0xb0404000)                       -> reads 0xb0404000
 * BDEV_RD(0x404000)                        -> reads 0xb0404000
 * BDEV_RD(BCHP_SUN_TOP_CTRL_PROD_REVISION) -> reads 0xb0404000
 */

#define DEV_RD(x) (*((volatile unsigned long *)(x)))
#define DEV_WR(x, y) do { *((volatile unsigned long *)(x)) = (y); } while(0)
#define DEV_UNSET(x, y) do { DEV_WR((x), DEV_RD(x) & ~(y)); } while(0)
#define DEV_SET(x, y) do { DEV_WR((x), DEV_RD(x) | (y)); } while(0)

#define BDEV_RD(x) (DEV_RD((x) | 0xb0000000UL))
#define BDEV_WR(x, y) do { DEV_WR((x) | 0xb0000000UL, (y)); } while(0)
#define BDEV_UNSET(x, y) do { BDEV_WR((x), BDEV_RD(x) & ~(y)); } while(0)
#define BDEV_SET(x, y) do { BDEV_WR((x), BDEV_RD(x) | (y)); } while(0)

#define BDEV_RD_F(reg, field) \
	((BDEV_RD(BCHP_##reg) & BCHP_##reg##_##field##_MASK) >> \
	 BCHP_##reg##_##field##_SHIFT)
#define BDEV_WR_F(reg, field, val) do { \
	BDEV_WR(BCHP_##reg, \
	(BDEV_RD(BCHP_##reg) & ~BCHP_##reg##_##field##_MASK) | \
	(((val) << BCHP_##reg##_##field##_SHIFT) & \
	 BCHP_##reg##_##field##_MASK)); \
	} while(0)
#define BPHYSADDR(x)	((x) | BCHP_PHYSICAL_OFFSET)

/*
 * use address/mask/value tuples to write several registers at once
 * (mostly used to set up the pinmux)
 */
#define BDEV_WR_ARRAY(reg_array) do { \
	struct brcm_reg_array ra[] = reg_array, *m = ra; \
	int i = sizeof(ra) / sizeof(ra[0]); \
	for(; i > 0; i--, m++) { \
		BDEV_WR(m->reg, (BDEV_RD(m->reg) & m->mask) | m->value); \
		BDEV_RD(m->reg); \
	} \
} while(0)

/*
 * g_magnum_spinlock must be used to protect accesses to CLKGEN, SW_RESET,
 * PIN_MUX, or GPIO registers
 */
extern spinlock_t g_magnum_spinlock;
#define BRCM_MAGNUM_SPINLOCK	1


#if defined(BOOT_ROM_TYPE_STRAP_BOOT_SHAPE_ADDR) && defined(BOOT_ROM_TYPE_STRAP_BOOT_SHAPE_MASK)
static inline int is_bootrom_nand(void)
{
	volatile unsigned long boot_mode = (BDEV_RD(BCHP_SUN_TOP_CTRL_STRAP_VALUE_0) & 
		BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_bus_mode_MASK) >> 
			BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_bus_mode_SHIFT;
	volatile unsigned long boot_shape = (*((volatile unsigned long *)BOOT_ROM_TYPE_STRAP_BOOT_SHAPE_ADDR) &
		BOOT_ROM_TYPE_STRAP_BOOT_SHAPE_MASK) >> BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_boot_shape_SHIFT;

	switch (boot_mode) {
	case 0:
	case 1:
	case 3:
		return (boot_shape & 0x1);
	}
	return 0;
}
#endif

#endif /* __ASSEMBLY__ */

#endif /*__BRCMSTB_H */
