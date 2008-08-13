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
#include <asm/brcmstb/brcm93548a0/bchp_memc_0_ddr.h>
#include <asm/brcmstb/brcm93548a0/bchp_mspi.h>
#include <asm/brcmstb/brcm93548a0/bchp_bspi.h>

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
#include <asm/brcmstb/brcm97440b0/bchp_hif_cpu_intr1.h>
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
#define	is_bootrom_nand()		\
	(((*(volatile unsigned long *)BOOT_ROM_TYPE_STRAP_ADDR) & BOOT_ROM_TYPE_STRAP_MASK) ==  BOOT_ROM_TYPE_STRAP_MASK)

#elif defined(BRCM_SPI_CHIP_SELECT) && defined(BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_nand_flash_boot_MASK)
/* SPI based STBs */
#define	is_bootrom_nand()		\
  	((BDEV_RD(BCHP_SUN_TOP_CTRL_STRAP_VALUE_0) & \
                BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_nand_flash_boot_MASK) \
                == BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_nand_flash_boot_MASK)

#else
#define	is_bootrom_nand()		(0)	// default bootrom is NOR
#endif


#ifndef __ASSEMBLY__

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
 * 
 * By default will return 0.  It's up to individual boards to
 * override the default, by overloading the function pointer.
 * returns the amount of memory not accounted for by get_RAM_size();
 */
extern unsigned long (* __get_discontig_RAM_size) (void);

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

#endif

#endif /*__BRCMSTB_H */
