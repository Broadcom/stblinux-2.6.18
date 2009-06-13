/*
 * drivers/usb/host/ohci-brcm.c
 *
 * Copyright (C) 2004-2005 Broadcom Corporation
 *
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
 * Broadcom OHCI HCD (Host Controller Driver) for USB.
 *
 * Bus Glue for Broadcom STB non-PCI USB controller
 *
 * Modified for brcm from ohci-lh7a404.c by ttruong@broadcom.com
 *
 */

#include <linux/platform_device.h>
#include <asm/brcmstb/common/brcmstb.h>
#include <asm/brcmstb/common/brcm-pm.h>
#include "brcmusb.h"

extern int usb_disabled(void);

#define BRCM_USB_AWAKE		0
#define BRCM_USB_SLEEPING	1
#define BRCM_USB_TRANSITION	2
static atomic_t ohci_brcm_state = ATOMIC_INIT(BRCM_USB_SLEEPING);

/*-------------------------------------------------------------------------*/

static void brcm_start_hc(struct platform_device *dev)
{
	printk(/*KERN_DEBUG*/ __FILE__
	       ": starting brcm OHCI USB Controller\n");

#ifndef CONFIG_USB_EHCI_HCD
	// Init common registers for board and HC specific issues
	brcm_usb_init();
#endif

#ifdef RESET_ON_START
  	{
  		unsigned int hcBaseAddr = (unsigned int) dev->resource[0].start;

		printk(" - Resetting at %08x\n", KSEG1ADDR(hcBaseAddr + 0x08));

	   /* This fix the lockup  during reboot */
	   *((volatile u32 *) KSEG1ADDR(hcBaseAddr + 0x08)) = 1;	
  	}
	printk("<-- %s\n", __FUNCTION__);
#endif
}

static void brcm_stop_hc(struct platform_device *dev)
{
	printk(KERN_DEBUG __FILE__
	       ": stopping brcm OHCI USB Controller\n");
}

/*-------------------------------------------------------------------------*/

/* configure so an HC device and id are always provided */
/* always called with process context; sleeping is OK */


/**
 * usb_hcd_brcm_probe - initialize brcm-based HCDs
 * Context: !in_interrupt()
 *
 * Allocates basic resources for this USB host controller, and
 * then invokes the start() method for the HCD associated with it
 * through the hotplug entry's driver_data.
 *
 */
static int usb_hcd_brcm_probe (const struct hc_driver *driver,
			  struct platform_device *dev)
{
	int retval;
	struct usb_hcd *hcd = 0;

	if (dev->resource[1].flags != IORESOURCE_IRQ) {
		pr_debug("resource[1] is not IORESOURCE_IRQ");
		return -ENOMEM;
	}

	hcd = usb_create_hcd(driver, &dev->dev, "ohci-brcm");
	if (!hcd)
		return -ENOMEM;


	hcd->rsrc_start = dev->resource[0].start;
	hcd->rsrc_len = dev->resource[0].end - dev->resource[0].start + 1;

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, hcd_name)) {
		pr_debug("request_mem_region failed");
		retval = -EBUSY;
		goto err1;
	}
	
	hcd->regs = ioremap_nocache(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		pr_debug("ioremap failed");
		retval = -ENOMEM;
		goto err2;
	}

	brcm_start_hc(dev);
	ohci_hcd_init(hcd_to_ohci(hcd));

	retval = usb_add_hcd(hcd, dev->resource[1].start, SA_SHIRQ|SA_INTERRUPT);
	if (retval == 0)
		return retval;

	brcm_stop_hc(dev);
	iounmap(hcd->regs);
 err2:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
 err1:
	usb_put_hcd(hcd);
	return retval;
}


/* may be called without controller electrically present */
/* may be called with controller, bus, and devices active */

/**
 * usb_hcd_brcm_remove - shutdown processing for brcm-based HCDs
 * @dev: USB Host Controller being removed
 * Context: !in_interrupt()
 *
 * Reverses the effect of usb_hcd_brcm_probe(), first invoking
 * the HCD's stop() method.  It is always called from a thread
 * context, normally "rmmod", "apmd", or something similar.
 *
 */
static void usb_hcd_brcm_remove (struct usb_hcd *hcd, struct platform_device *dev)
{
	usb_remove_hcd(hcd);
	brcm_stop_hc(dev);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
}

/*-------------------------------------------------------------------------*/

static int
ohci_brcm_start (struct usb_hcd *hcd)
{
	struct ohci_hcd	*ohci = hcd_to_ohci (hcd);
	int		ret;

	ohci_dbg (ohci, "ohci_brcm_start, ohci:%p", ohci);
			
	if ((ret = ohci_init(ohci)) < 0)
		return ret;

	if ((ret = ohci_run (ohci)) < 0) {
		err ("can't start %s", hcd->self.bus_name);
		ohci_stop (hcd);
		return ret;
	}
	return 0;
}

/*-------------------------------------------------------------------------*/

static const struct hc_driver ohci_brcm_hc_driver = {
	.description =		hcd_name,
	.product_desc =		"BRCM OHCI",
	.hcd_priv_size =	sizeof(struct ohci_hcd),

	/*
	 * generic hardware linkage
	 */
	.irq =			ohci_irq,
	.flags =		HCD_USB11 | HCD_MEMORY,

	/*
	 * basic lifecycle operations
	 */
	.start =		ohci_brcm_start,
#ifdef	CONFIG_PM
	/* suspend:		ohci_brcm_suspend,  -- tbd */
	/* resume:		ohci_brcm_resume,   -- tbd */
#endif /*CONFIG_PM*/
	.stop =			ohci_stop,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue =		ohci_urb_enqueue,
	.urb_dequeue =		ohci_urb_dequeue,
	.endpoint_disable =	ohci_endpoint_disable,

	/*
	 * scheduling support
	 */
	.get_frame_number =	ohci_get_frame,

	/*
	 * root hub support
	 */
	.hub_status_data =	ohci_hub_status_data,
	.hub_control =		ohci_hub_control,
};

/*-------------------------------------------------------------------------*/

static int ohci_hcd_brcm_drv_probe(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	int ret;

	if (usb_disabled())
		return -ENODEV;

	ret = usb_hcd_brcm_probe(&ohci_brcm_hc_driver, pdev);

	return ret;
}

static int ohci_hcd_brcm_drv_remove(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct usb_hcd *hcd = dev_get_drvdata(dev);

	usb_hcd_brcm_remove(hcd, pdev);
	return 0;
}
	/*TBD*/
/*static int ohci_hcd_brcm_drv_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct usb_hcd *hcd = dev_get_drvdata(dev);

	return 0;
}
static int ohci_hcd_brcm_drv_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct usb_hcd *hcd = dev_get_drvdata(dev);


	return 0;
}
*/

static struct device_driver ohci_hcd_brcm_driver[] = {
	{ /* 0 */
		.name		= "brcm-ohci-0",
		.bus			= &platform_bus_type,
		.probe		= ohci_hcd_brcm_drv_probe,
		.remove		= ohci_hcd_brcm_drv_remove,
		/*.suspend	= ohci_hcd_brcm_drv_suspend, */
		/*.resume	= ohci_hcd_brcm_drv_resume, */
	}
	/* 3560B0 only has 1 OHCI port */
	
#if (NUM_OHCI_HOSTS > 1)
	,
	{ /* 1 */
		.name		= "brcm-ohci-1",
		.bus			= &platform_bus_type,
		.probe		= ohci_hcd_brcm_drv_probe,
		.remove		= ohci_hcd_brcm_drv_remove,
		/*.suspend	= ohci_hcd_brcm_drv_suspend, */
		/*.resume	= ohci_hcd_brcm_drv_resume, */
	}
#endif
};

static struct platform_device *plat_dev[NUM_OHCI_HOSTS];

static int brcm_ohci_hcd_init (int ohci_id)
{
	int err = -1;
	struct resource devRes[2];
	
	printk (DRIVER_INFO " (OHCI-brcm-%.1d)\n", ohci_id);

	memset(devRes, 0, sizeof(devRes));
	devRes[0].name = ohci_id == 0 ? "brcm-ohci0-IO" : "brcm-ohci1-IO" ;
	devRes[0].start = ohci_id == 0 ? HC_BASE_ADDR: HC2_BASE_ADDR;
	devRes[0].end = ohci_id == 0 ? HC_END_ADDR: HC2_END_ADDR;
	devRes[0].flags = IORESOURCE_MEM;
	devRes[1].name = ohci_id == 0 ? "brcm-ohci0-irq" : "brcm-ohci1-irq";
	devRes[1].start = ohci_id == 0 ? HC_INT_VECTOR : HC2_INT_VECTOR;
	devRes[1].end = ohci_id == 0 ? HC_INT_VECTOR : HC2_INT_VECTOR ;
	devRes[1].flags = IORESOURCE_IRQ;

#ifdef CONFIG_MIPS_BCM7400
	/* IRQ lines for EHCI-1 and OHCI-0 are swapped on 7400 prior to D0 */
	if((BDEV_RD(BCHP_SUN_TOP_CTRL_PROD_REVISION) & 0xffff) < 0x0030) {
		devRes[1].start = ohci_id == 0 ? EHC2_INT_VECTOR : HC2_INT_VECTOR;
		devRes[1].end = ohci_id == 0 ? EHC2_INT_VECTOR : HC2_INT_VECTOR ;
	}
#endif

	// Before we register the driver, add a simple device matching our driver
	plat_dev[ohci_id] = platform_device_register_simple(
		(char *) ohci_hcd_brcm_driver[ohci_id].name,
		ohci_id, /* ID */
		devRes,
		2);
	if (IS_ERR(plat_dev[ohci_id])) {
		printk("ohci_hcd_brcm_init: device register failed, err=%d\n", err);
		return PTR_ERR(plat_dev[ohci_id]);
	}

	// Set up dma_mask for our platform device
	/*
		PR53481: make sure dma_mask at upper boundary of low RAM so it is DMAable and uncached (in kseg1)
	*/
	plat_dev[ohci_id]->dev.dma_mask = &plat_dev[ohci_id]->dev.coherent_dma_mask;
	plat_dev[ohci_id]->dev.coherent_dma_mask = (u64)(0x10000000UL - 1UL);
		
	err = driver_register(&ohci_hcd_brcm_driver[ohci_id]);
	if (err) {
		printk("ohci_hcd_brcm_init: driver_register failed, err=%d\n", err);
		return err;
	}
	
	return(0);
}

static int ohci_brcm_enable(void *arg)
{
	int i;
	int err = -1;

	if(atomic_cmpxchg(&ohci_brcm_state, BRCM_USB_SLEEPING,
		BRCM_USB_TRANSITION) != BRCM_USB_SLEEPING) {
		return(-1);
	}

	printk("ohci_hcd_brcm_init: Initializing %d OHCI controller(s)\n", NUM_OHCI_HOSTS);

	brcm_pm_usb_add();
	for (i=0; i < NUM_OHCI_HOSTS; i++) {
		err = brcm_ohci_hcd_init(i);
		if (err) {
			do {
				driver_unregister(&ohci_hcd_brcm_driver[i]);
				platform_device_unregister(plat_dev[i]);
				i--;
			} while(i >= 0);
			atomic_set(&ohci_brcm_state, BRCM_USB_SLEEPING);

			return err;
		}
	}
	atomic_set(&ohci_brcm_state, BRCM_USB_AWAKE);
	return(0);
}

static int ohci_brcm_disable(void *arg)
{
	int i;

	if(atomic_cmpxchg(&ohci_brcm_state, BRCM_USB_AWAKE,
		BRCM_USB_TRANSITION) != BRCM_USB_AWAKE) {
		return(-1);
	}

	printk("ohci_hcd_brcm_cleanup: Taking down %d OHCI controller(s)\n", NUM_OHCI_HOSTS);

	for (i=0; i<NUM_OHCI_HOSTS; i++) {
		driver_unregister(&ohci_hcd_brcm_driver[i]);
		platform_device_unregister(plat_dev[i]);
	}
	brcm_pm_usb_remove();
	atomic_set(&ohci_brcm_state, BRCM_USB_SLEEPING);
	return(0);
}

static int __init ohci_hcd_brcm_init (void)
{
	int ret;

	ret = ohci_brcm_enable(NULL);
	if(ret)
		return(ret);
#if defined(CONFIG_BRCM_PM)
	brcm_pm_register_ohci(ohci_brcm_disable, ohci_brcm_enable, NULL);
#endif
	return(0);
}


static void __exit ohci_hcd_brcm_cleanup (void)
{
#if defined(CONFIG_BRCM_PM)
	brcm_pm_unregister_ohci();
#endif
	ohci_brcm_disable(NULL);
}

module_init (ohci_hcd_brcm_init);
module_exit (ohci_hcd_brcm_cleanup);
