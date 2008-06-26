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

// Compatiblity between RDB and gas
#ifdef __ASSEMBLY__
#define _ASMLANGUAGE
#endif



#include <linux/config.h>

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
#include <asm/brcmstb/brcm93548a0/bchp_clk.h>
#include <asm/brcmstb/brcm93548a0/bchp_bmips4380.h>
#include <asm/brcmstb/brcm93548a0/bchp_memc_0_ddr.h>

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

#ifndef __ASSEMBLY__
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

#else
#include <asm/brcmstb/brcm97118c0/bcmtimer.h>
#endif

#define	BOOT_ROM_TYPE_STRAP_ADDR	(0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE)
#define	BOOT_ROM_TYPE_STRAP_MASK	BCHP_SUN_TOP_CTRL_STRAP_VALUE_strap_nand_flash_MASK

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


#elif defined(CONFIG_MIPS_BCM7405A0)
#include <asm/brcmstb/brcm97405a0/bcmuart.h>
#include <asm/brcmstb/brcm97405a0/bcmtimer.h>
#include <asm/brcmstb/brcm97405a0/bcmebi.h>
#include <asm/brcmstb/brcm97405a0/int1.h>
#include <asm/brcmstb/brcm97405a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97405a0/board.h>
#include <asm/brcmstb/brcm97405a0/bchp_irq0.h>
#include <asm/brcmstb/brcm97405a0/bcmintrnum.h>
#include <asm/brcmstb/brcm97405a0/bchp_nand.h>
#include <asm/brcmstb/brcm97405a0/bchp_ebi.h>
#include <asm/brcmstb/brcm97405a0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97405a0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97405a0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97405a0/bchp_usb_ehci1.h>
#include <asm/brcmstb/brcm97405a0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97405a0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97405a0/bchp_pcix_bridge.h>
#include <asm/brcmstb/brcm97405a0/bchp_clk.h>
#include <asm/brcmstb/brcm97405a0/bchp_memc_0_ddr.h>

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


#elif defined(CONFIG_MIPS_BCM7335A0)
#include <asm/brcmstb/brcm97335a0/bcmuart.h>
#include <asm/brcmstb/brcm97335a0/bcmtimer.h>
#include <asm/brcmstb/brcm97335a0/bcmebi.h>
#include <asm/brcmstb/brcm97335a0/int1.h>
#include <asm/brcmstb/brcm97335a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97335a0/board.h>
#include <asm/brcmstb/brcm97335a0/bchp_irq0.h>
#include <asm/brcmstb/brcm97335a0/bcmintrnum.h>
#include <asm/brcmstb/brcm97335a0/bchp_nand.h>
#include <asm/brcmstb/brcm97335a0/bchp_ebi.h>
#include <asm/brcmstb/brcm97335a0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97335a0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97335a0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97335a0/bchp_usb_ehci1.h>
#include <asm/brcmstb/brcm97335a0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97335a0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97335a0/bchp_pcix_bridge.h>
#include <asm/brcmstb/brcm97335a0/bchp_clk.h>
#include <asm/brcmstb/brcm97335a0/bchp_bmips4380.h>
#include <asm/brcmstb/brcm97335a0/bchp_memc_0_ddr.h>

#elif defined(CONFIG_MIPS_BCM7325A0)
#include <asm/brcmstb/brcm97325a0/bcmuart.h>
#include <asm/brcmstb/brcm97325a0/bcmtimer.h>
#include <asm/brcmstb/brcm97325a0/bcmebi.h>
#include <asm/brcmstb/brcm97325a0/int1.h>
#include <asm/brcmstb/brcm97325a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97325a0/board.h>
#include <asm/brcmstb/brcm97325a0/bchp_irq0.h>
#include <asm/brcmstb/brcm97325a0/bchp_irq1.h>
#include <asm/brcmstb/brcm97325a0/bcmintrnum.h>
#include <asm/brcmstb/brcm97325a0/bchp_nand.h>
#include <asm/brcmstb/brcm97325a0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97325a0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97325a0/bchp_usb_ehci1.h>
#include <asm/brcmstb/brcm97325a0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97325a0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97325a0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97325a0/bchp_memc_0_ddr.h>

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
#include <asm/brcmstb/brcm97440b0/bcmintrnum.h>
#include <asm/brcmstb/brcm97440b0/bchp_nand.h>
#include <asm/brcmstb/brcm97440b0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97440b0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97440b0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97440b0/bchp_sun_top_ctrl.h>

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

#define	BOOT_ROM_TYPE_STRAP_ADDR	(0xb0000000 | BCHP_SUN_TOP_CTRL_STRAP_VALUE_0)
#define	BOOT_ROM_TYPE_STRAP_MASK	BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_nand_flash_MASK	


#else
#error "unknown BCM STB chip!!!"
#endif


//THT: PR41560, leave this one in.  In the case of bootrom=NOR, we will get the MAC_ADDR from NOR.
#if defined(BOOT_ROM_TYPE_STRAP_ADDR) && defined(BOOT_ROM_TYPE_STRAP_ADDR)
#define	is_bootrom_nand		\
	(((*(volatile unsigned long *)BOOT_ROM_TYPE_STRAP_ADDR) & BOOT_ROM_TYPE_STRAP_MASK) ==  BOOT_ROM_TYPE_STRAP_MASK)
#else
#define	is_bootrom_nand		0	// default bootrom is NOR
#endif


#ifndef __ASSEMBLY__
extern void (*irq_setup)(void);
extern void uart_puts(const char*);


typedef int (*walk_cb_t)(unsigned long paddr, unsigned long size, long type, void* cbdata);
extern int brcm_walk_boot_mem_map(void* cbdata, walk_cb_t walk_cb);
extern unsigned long get_RAM_size(void);
extern unsigned long get_RSVD_size(void);

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

#endif

#endif /*__BRCMSTB_H */

