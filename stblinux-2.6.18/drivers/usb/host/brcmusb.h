/*
 * drivers/usb/brcmusb.h
 *
 * Copyright (C) 2001 Broadcom Corporation
 *                    Sasi Kumar <sasik@broadcom.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * BRCM adaption of the usb-ohci stack to support the direct (non-PCI) 
 * USB Host Controllers.  
 *
 * Include this file in usb-ohci.c BEFORE including usb-ohci.h.  This is
 * done to fake out the check for the CONFIG_PCI macro and avoid having 
 * to change the usb-ohci.h file!
 * 
 * 02-08-2002   SK     Created
 * 02-08-2007   SK     Simplified for 2.6
*/

#ifndef __BRCMUSB_H__
#define __BRCMUSB_H__

#include <linux/config.h>
#include <linux/version.h>
#include <asm/io.h>
#include <asm/brcmstb/common/brcmstb.h>

#if defined(CONFIG_PCI) && defined(CONFIG_SWAP_IO_SPACE_L)
#undef CONFIG_SWAP_IO_SPACE_L
#endif

//*****************************************************************************
// Platform base address and interrupt vector setup

#if defined( CONFIG_MIPS_BCM7318 )
 #define HC_BASE_ADDR   			USB_HOST_ADR_BASE
 #define EHC_BASE_ADDR   			(USB_HOST_ADR_BASE+0x70)
 #define BrcmUsbSetupReg      	 	((volatile u32 *) (HC_BASE_ADDR+0xfc))
 #define HC_INT_VECTOR            	BCM_LINUX_USB_HOST_IRQ
 #define EHC_INT_VECTOR           	BCM_LINUX_USB_HOST_IRQ
#else
 #define HC_BASE_ADDR   			(BCHP_USB_OHCI_HcRevision | BCHP_PHYSICAL_OFFSET)
 #define EHC_BASE_ADDR   			(BCHP_USB_EHCI_HCCAPBASE | BCHP_PHYSICAL_OFFSET)
 #define BrcmUsbSetupReg      	 	((volatile u32 *) (0xb0000000 | \
									 (BCHP_USB_CTRL_SETUP | BCHP_PHYSICAL_OFFSET)))
 #define HC_INT_VECTOR            	BCM_LINUX_USB_OHCI_0_CPU_INTR
 #define EHC_INT_VECTOR           	BCM_LINUX_USB_EHCI_CPU_INTR
 #define RESET_ON_START				// reset HC on start to avoid reboot issues
#endif /* CONFIG_MIPS_BCM7318 */

#ifdef BCHP_USB_EHCI1_HCCAPBASE		//2nd EHCI Controller?
 #define NUM_EHCI_HOSTS			2
 #define EHC2_BASE_ADDR   			(BCHP_USB_EHCI1_HCCAPBASE | BCHP_PHYSICAL_OFFSET)
 #define EHC2_INT_VECTOR        	BCM_LINUX_USB_EHCI_1_CPU_INTR
#else
 #define NUM_EHCI_HOSTS			1
 #define EHC2_BASE_ADDR   			EHC_BASE_ADDR
 #define EHC2_INT_VECTOR        	EHC_INT_VECTOR
#endif

#ifdef BCHP_USB_OHCI1_HcRevision	//2nd OHCI Controller?
 #define NUM_OHCI_HOSTS			2
 #define HC2_BASE_ADDR  			(BCHP_USB_OHCI1_HcRevision | BCHP_PHYSICAL_OFFSET)
 #define HC2_INT_VECTOR				BCM_LINUX_USB_OHCI_1_CPU_INTR
#else
 #define NUM_OHCI_HOSTS			1
 #define HC2_BASE_ADDR  			HC_BASE_ADDR
 #define HC2_INT_VECTOR				HC_INT_VECTOR
#endif

#define HC_END_ADDR              	(HC_BASE_ADDR+0x6f)
#define EHC_END_ADDR             	(EHC_BASE_ADDR+0xa8)
#define HC2_END_ADDR             	(HC2_BASE_ADDR+0x6f)
#define EHC2_END_ADDR             	(EHC2_BASE_ADDR+0xa8)


#if defined (CONFIG_MIPS_BRCM97XXX)
  // 2.6.x: Disable PCI for SuperChips bcm7xxx.  PCI is needed for Venom2 lines, however.
  #ifdef CONFIG_PCI
    #undef CONFIG_PCI
  #endif
#endif

//*****************************************************************************
// Platform power control chip and endianess setup

enum { BABO=0x01, FNHW=0x02, FNBO=0x04, WABO=0x08,
       IOC=0x10, IPP=0X20, CLV_BE=0x200000 };

#if defined(CONFIG_USB_BRCM_PWR_LO)
  #define USB_PWR_CTL		(IPP+IOC)
#else
  #define USB_PWR_CTL		IOC
#endif
#define USB_PWR_CTL_MASK	(IPP+IOC)

#if defined (__MIPSEB__)
  #define USB_ENDIAN_CTL	(WABO+FNBO+FNHW)
#else
  #define USB_ENDIAN_CTL	(FNHW+BABO)
#endif
#define USB_ENDIAN_MASK		(WABO+FNBO+FNHW+BABO)


#if defined(EHCI_BRCM) || (defined(OHCI_BRCM) && !defined(CONFIG_USB_EHCI_HCD))
static void brcm_usb_init(void)
{
	static int done = 0;
	u32 val;
	char mode = 'B';

	val = readl( BrcmUsbSetupReg );
	if( !done )
	{
		//set up endianess
		val = (val & ~USB_ENDIAN_MASK) | USB_ENDIAN_CTL;

		//set up power chip controls
#ifdef CONFIG_USB_BRCM_PWR_CTL
		val = (val & ~USB_PWR_CTL_MASK) | USB_PWR_CTL;
		mode = 'F';
#else
		if( !(val & IOC ) )		//IOC is always set by BRCM bootloader
		{
			val = (val & ~USB_PWR_CTL_MASK) | USB_PWR_CTL;
			mode = 'N';
		}
#endif

		writel( val, BrcmUsbSetupReg );
		printk( " - USB h/w setup mode <%c>\n"
				" - USB power enable set for active %s mode\n", 
				mode, ((val & IPP) ? "LOW" : "HIGH") );

		/* PR45703 - for OHCI->SCB bridge lockup */
		BDEV_UNSET(BCHP_USB_CTRL_OBRIDGE,
			BCHP_USB_CTRL_OBRIDGE_OBR_SEQ_EN_MASK);

		done = 1;
	}
}
#endif

#endif //__BRCMUSB_H__

