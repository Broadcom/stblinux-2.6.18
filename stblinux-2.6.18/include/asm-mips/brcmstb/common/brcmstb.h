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

#define BRCM_ENDIAN_LITTLE CONFIG_CPU_LITTLE_ENDIAN

#if defined(CONFIG_MIPS_BCM3560A0)
#include <asm/brcmstb/brcm93560/bcmuart.h>
#include <asm/brcmstb/brcm93560/bcmtimer.h>
#include <asm/brcmstb/brcm93560/bcmebi.h>
#include <asm/brcmstb/brcm93560/int1.h>
#include <asm/brcmstb/brcm93560/board.h>
#include <asm/brcmstb/brcm93560/bchp_irq0.h>
#include <asm/brcmstb/brcm93560/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM3560B0)
#include <asm/brcmstb/brcm93560b0/bcmuart.h>
#include <asm/brcmstb/brcm93560b0/bcmtimer.h>
#include <asm/brcmstb/brcm93560b0/bcmebi.h>
#include <asm/brcmstb/brcm93560b0/int1.h>
#include <asm/brcmstb/brcm93560b0/board.h>
#include <asm/brcmstb/brcm93560b0/bchp_irq0.h>
#include <asm/brcmstb/brcm93560b0/bcmintrnum.h>
#include <asm/brcmstb/brcm93560b0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm93560b0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm93560b0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm93560b0/bchp_usb_ohci1.h>

#elif defined(CONFIG_MIPS_BCM3563)
#include <asm/brcmstb/brcm93563/bcmuart.h>
#include <asm/brcmstb/brcm93563/bcmtimer.h>
#include <asm/brcmstb/brcm93563/bcmebi.h>
#include <asm/brcmstb/brcm93563/int1.h>
#include <asm/brcmstb/brcm93563/board.h>
#include <asm/brcmstb/brcm93563/bchp_irq0.h>
#include <asm/brcmstb/brcm93563/bcmintrnum.h>
#include <asm/brcmstb/brcm93563/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm93563/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm93563/bchp_usb_ohci.h>

#elif defined(CONFIG_MIPS_BCM7038A0)
#include <asm/brcmstb/brcm97038/bcmuart.h>
#include <asm/brcmstb/brcm97038/bcmtimer.h>
#include <asm/brcmstb/brcm97038/bcmebi.h>
#include <asm/brcmstb/brcm97038/int1.h>
#include <asm/brcmstb/brcm97038/board.h>
#include <asm/brcmstb/brcm97038/bchp_irq0.h>
#include <asm/brcmstb/brcm97038/bcmintrnum.h>
#include <asm/brcmstb/brcm97038/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97038/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97038/bchp_usb_ohci.h>

#elif defined(CONFIG_MIPS_BCM7038B0)
#include <asm/brcmstb/brcm97038b0/bcmuart.h>
#include <asm/brcmstb/brcm97038b0/bcmtimer.h>
#include <asm/brcmstb/brcm97038b0/bcmebi.h>
#include <asm/brcmstb/brcm97038b0/int1.h>
#include <asm/brcmstb/brcm97038b0/board.h>
#include <asm/brcmstb/brcm97038b0/bchp_irq0.h>
#include <asm/brcmstb/brcm97038b0/bcmintrnum.h>
#include <asm/brcmstb/brcm97038b0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97038b0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97038b0/bchp_usb_ohci.h>

#elif defined(CONFIG_MIPS_BCM7038C0)
#include <asm/brcmstb/brcm97038c0/bcmuart.h>
#include <asm/brcmstb/brcm97038c0/bcmtimer.h>
#include <asm/brcmstb/brcm97038c0/bcmebi.h>
#include <asm/brcmstb/brcm97038c0/int1.h>
#include <asm/brcmstb/brcm97038c0/board.h>
#include <asm/brcmstb/brcm97038c0/bchp_irq0.h>
#include <asm/brcmstb/brcm97038c0/bcmintrnum.h>
#include <asm/brcmstb/brcm97038c0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97038c0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97038c0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97038c0/bchp_usb_ohci1.h>

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

#elif defined(CONFIG_MIPS_BCM7400A0)
#include <asm/brcmstb/brcm97400a0/bcmuart.h>
#include <asm/brcmstb/brcm97400a0/bcmtimer.h>
#include <asm/brcmstb/brcm97400a0/bcmebi.h>
#include <asm/brcmstb/brcm97400a0/int1.h>
#include <asm/brcmstb/brcm97400a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97400a0/board.h>
#include <asm/brcmstb/brcm97400a0/bchp_irq0.h>
#include <asm/brcmstb/brcm97400a0/bcmintrnum.h>
#include <asm/brcmstb/brcm97400a0/bchp_nand.h>
#include <asm/brcmstb/brcm97400a0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97400a0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97400a0/bchp_usb_ehci1.h>
#include <asm/brcmstb/brcm97400a0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97400a0/bchp_usb_ohci1.h>

#elif defined(CONFIG_MIPS_BCM7400B0)
#include <asm/brcmstb/brcm97400b0/bcmuart.h>
#include <asm/brcmstb/brcm97400b0/bcmtimer.h>
#include <asm/brcmstb/brcm97400b0/bcmebi.h>
#include <asm/brcmstb/brcm97400b0/int1.h>
#include <asm/brcmstb/brcm97400b0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97400b0/board.h>
#include <asm/brcmstb/brcm97400b0/bchp_irq0.h>
#include <asm/brcmstb/brcm97400b0/bcmintrnum.h>
#include <asm/brcmstb/brcm97400b0/bchp_nand.h>
#include <asm/brcmstb/brcm97400b0/bchp_ebi.h>
#include <asm/brcmstb/brcm97400b0/bchp_sun_top_ctrl.h>
#include <asm/brcmstb/brcm97400b0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97400b0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97400b0/bchp_usb_ehci1.h>
#include <asm/brcmstb/brcm97400b0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97400b0/bchp_usb_ohci1.h>
#include <asm/brcmstb/brcm97400b0/bchp_pcix_bridge.h>

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

#elif defined(CONFIG_MIPS_BCM7440A0)
#include <asm/brcmstb/brcm97440a0/bcmuart.h>
#include <asm/brcmstb/brcm97440a0/bcmtimer.h>
#include <asm/brcmstb/brcm97440a0/bcmebi.h>
#include <asm/brcmstb/brcm97440a0/int1.h>
#include <asm/brcmstb/brcm97440a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97440a0/board.h>
#include <asm/brcmstb/brcm97440a0/bchp_irq0.h>
#include <asm/brcmstb/brcm97440a0/bcmintrnum.h>
#include <asm/brcmstb/brcm97440a0/bchp_nand.h>
#include <asm/brcmstb/brcm97440a0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97440a0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97440a0/bchp_usb_ohci.h>

/* FIXME when we have a 7402S with a real 7402 chip */
#elif defined(CONFIG_MIPS_BCM7401A0) || defined( CONFIG_MIPS_BCM7402S)
#include <asm/brcmstb/brcm97401a0/bcmuart.h>
#include <asm/brcmstb/brcm97401a0/bcmtimer.h>
#include <asm/brcmstb/brcm97401a0/bcmebi.h>
#include <asm/brcmstb/brcm97401a0/int1.h>
#include <asm/brcmstb/brcm97401a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97401a0/board.h>
#include <asm/brcmstb/brcm97401a0/bchp_irq0.h>
#include <asm/brcmstb/brcm97401a0/bcmintrnum.h>
#include <asm/brcmstb/brcm97401a0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97401a0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97401a0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97401a0/bchp_usb_ohci1.h>

#elif defined(CONFIG_MIPS_BCM7401B0) || defined(CONFIG_MIPS_BCM7402A0)
#include <asm/brcmstb/brcm97401b0/bcmuart.h>
#include <asm/brcmstb/brcm97401b0/bcmtimer.h>
#include <asm/brcmstb/brcm97401b0/bcmebi.h>
#include <asm/brcmstb/brcm97401b0/int1.h>
#include <asm/brcmstb/brcm97401b0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97401b0/board.h>
#include <asm/brcmstb/brcm97401b0/bchp_irq0.h>
#include <asm/brcmstb/brcm97401b0/bcmintrnum.h>
#include <asm/brcmstb/brcm97401b0/bchp_nand.h>
#include <asm/brcmstb/brcm97401b0/bchp_usb_ctrl.h>
#include <asm/brcmstb/brcm97401b0/bchp_usb_ehci.h>
#include <asm/brcmstb/brcm97401b0/bchp_usb_ohci.h>
#include <asm/brcmstb/brcm97401b0/bchp_usb_ohci1.h>

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

#elif defined(CONFIG_MIPS_BCM7110)
#include <asm/brcmstb/brcm97110/bcm97110.h>
#include <asm/brcmstb/brcm97110/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7111)
#include <asm/brcmstb/brcm97111/bcm97111.h>
#include <asm/brcmstb/brcm97111/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7112)
#include <asm/brcmstb/brcm97112/bcm97112.h>
#include <asm/brcmstb/brcm97112/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7115)
#include <asm/brcmstb/brcm97115/bcm97115.h>
#include <asm/brcmstb/brcm97115/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7312)
#include <asm/brcmstb/brcm97312/bcm97312.h>
#include <asm/brcmstb/brcm97312/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7314)
#include <asm/brcmstb/brcm97314/bcm97314.h>
#include <asm/brcmstb/brcm97314/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7315)
#include <asm/brcmstb/brcm97315/bcm97315.h>
#include <asm/brcmstb/brcm97315/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7317)
#include <asm/brcmstb/brcm97317/bcm97317.h>
#include <asm/brcmstb/brcm97317/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7318)
#include <asm/brcmstb/brcm97318/bcm97318.h>
#include <asm/brcmstb/brcm97318/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7319)
#include <asm/brcmstb/brcm97319/bcm97319.h>
#include <asm/brcmstb/brcm97319/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7320)
#include <asm/brcmstb/brcm97320/bcm97320.h>
#include <asm/brcmstb/brcm97320/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7327)
#include <asm/brcmstb/brcm97327/bcm97327.h>
#include <asm/brcmstb/brcm97327/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7328)
#include <asm/brcmstb/brcm97328/bcm97328.h>
#include <asm/brcmstb/brcm97328/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7329)
#include <asm/brcmstb/brcm97329/bcm97329.h>
#include <asm/brcmstb/brcm97329/bcmintrnum.h>
#else
#error "unknown BCM STB chip!!!"
#endif

#ifndef __ASSEMBLY__
extern void (*irq_setup)(void);
extern void uart_puts(const char*);


typedef int (*walk_cb_t)(unsigned long paddr, unsigned long size, long type, void* cbdata);
extern int brcm_walk_boot_mem_map(void* cbdata, walk_cb_t walk_cb);
extern unsigned long get_RAM_size(void);
extern unsigned long get_RSVD_size(void);
#endif


#endif /*__BRCMSTB_H */

