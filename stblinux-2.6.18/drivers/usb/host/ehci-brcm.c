/*
 * drivers/usb/host/ehci-brcm.c
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
 * Broadcom EHCI HCD (Host Controller Driver) for USB.
 *
 * Bus Glue for Broadcom STB non-PCI USB controller
 *
 * Modified for brcm from ohci-lh7a404.c by ttruong@broadcom.com
 *
 */

#include <linux/platform_device.h>
#include <asm/brcmstb/common/brcmstb.h>
#include <asm/brcmstb/common/brcm-pm.h>

extern int usb_disabled(void);

#define BRCM_USB_AWAKE		0
#define BRCM_USB_SLEEPING	1
#define BRCM_USB_TRANSITION	2
static atomic_t ehci_brcm_state = ATOMIC_INIT(BRCM_USB_SLEEPING);

/*-------------------------------------------------------------------------*/

static void brcm_ehci_hw_init(struct platform_device *dev)
{
	printk(/*KERN_DEBUG*/ __FILE__
			": starting brcm EHCI USB Controller\n");

	// Init common registers for board and HC specific issues
	brcm_usb_init();
}

static void brcm_stop_ehci(struct platform_device *dev)
{
	printk(KERN_DEBUG __FILE__
	       ": stopping brcm EHCI USB Controller\n");
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

	hcd = usb_create_hcd(driver, &dev->dev, "ehci-brcm");
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


	brcm_ehci_hw_init(dev);

	retval = usb_add_hcd(hcd, dev->resource[1].start, SA_SHIRQ|SA_INTERRUPT);
	if (retval == 0) {
		return retval;
	}

	brcm_stop_ehci(dev);
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
	brcm_stop_ehci(dev);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
}

/*-------------------------------------------------------------------------*/

static int
ehci_brcm_reset (struct usb_hcd *hcd)
{
	struct ehci_hcd		*ehci = hcd_to_ehci (hcd);

	ehci->caps = (struct ehci_caps *) hcd->regs;
	ehci->regs = (struct ehci_regs *) (hcd->regs + 
				HC_LENGTH (readl (&ehci->caps->hc_capbase)));
	dbg_hcs_params (ehci, "reset");
	dbg_hcc_params (ehci, "reset");

	/* cache this readonly data; minimize PCI reads */
	ehci->hcs_params = readl (&ehci->caps->hcs_params);

#ifdef RESET_ON_START
	/* This fixes the lockup during reboot due to prior interrupts */
	writel (CMD_RESET, &ehci->regs->command);
#endif

	/* force HC to halt state */
	return ehci_halt (ehci);
	
}


/*-------------------------------------------------------------------------*/

static int
ehci_brcm_start (struct usb_hcd *hcd)
{
	int retval;

	/* device structure init */
	if ((retval = ehci_init(hcd)) < 0)
		return retval;

	if ((retval = ehci_run (hcd)) < 0)
	{
		if(hcd)
		{
			printk ("can't start %s", hcd->self.bus_name);
			ehci_stop (hcd);
		}
		else
			printk("FATAL ERROR: \n");
	}

	return retval;
}

/*-------------------------------------------------------------------------*/

static const struct hc_driver ehci_brcm_hc_driver = {
	.description =		hcd_name,
	.product_desc =		"BRCM EHCI",
	.hcd_priv_size =	sizeof(struct ehci_hcd),

	/*
	 * generic hardware linkage
	 */
	.irq =			ehci_irq,
	.flags =		HCD_MEMORY | HCD_USB2,

	/*
	 * basic lifecycle operations
	 */
	.reset =	    ehci_brcm_reset,
	.start =		ehci_brcm_start,
#ifdef	CONFIG_PM
	 suspend:		ehci_bus_suspend, 
	 resume:		ehci_bus_resume,   
#endif /*CONFIG_PM*/
	.stop =			ehci_stop,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue =		ehci_urb_enqueue,
	.urb_dequeue =		ehci_urb_dequeue,
	.endpoint_disable =	ehci_endpoint_disable,

	/*
	 * scheduling support
	 */
	.get_frame_number =	ehci_get_frame,

	/*
	 * root hub support
	 */
	.hub_status_data =	ehci_hub_status_data,
	.hub_control =		ehci_hub_control,
#ifdef	CONFIG_PM
	.bus_suspend = ehci_bus_suspend,
	.bus_resume = ehci_bus_resume,
#endif
};

/*-------------------------------------------------------------------------*/

static int ehci_hcd_brcm_drv_probe(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	int ret;

	if (usb_disabled())
		return -ENODEV;

	ret = usb_hcd_brcm_probe(&ehci_brcm_hc_driver, pdev);

	return ret;
}

static int ehci_hcd_brcm_drv_remove(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct usb_hcd *hcd = dev_get_drvdata(dev);

	usb_hcd_brcm_remove(hcd, pdev);
	return 0;
}
	/*TBD*/
/*static int ehci_hcd_brcm_drv_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct usb_hcd *hcd = dev_get_drvdata(dev);

	return 0;
}
static int ehci_hcd_brcm_drv_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct usb_hcd *hcd = dev_get_drvdata(dev);


	return 0;
}
*/


static struct device_driver ehci_hcd_brcm_driver[] = {
	{ /* 0 */
		.name		= "brcm-ehci",
		.bus		= &platform_bus_type,
		.probe		= ehci_hcd_brcm_drv_probe,
		.remove		= ehci_hcd_brcm_drv_remove,
		/*.suspend	= ehci_hcd_brcm_drv_suspend, */
		/*.resume	= ehci_hcd_brcm_drv_resume, */
	}
#if (NUM_EHCI_HOSTS > 1)
	,
	{ /* 1 */
		.name		= "brcm-ehci-1",
		.bus		= &platform_bus_type,
		.probe		= ehci_hcd_brcm_drv_probe,
		.remove		= ehci_hcd_brcm_drv_remove,
		/*.suspend	= ehci_hcd_brcm_drv_suspend, */
		/*.resume	= ehci_hcd_brcm_drv_resume, */
	}
#endif
};

#define DRIVER_INFO DRIVER_VERSION " " DRIVER_DESC

static struct platform_device *plat_dev[NUM_EHCI_HOSTS];

static int ehci_hcd_brcm_init_each (int ehci_id)
{
	int err=0;
	struct resource devRes[2];
	
	printk (DRIVER_INFO " (EHCI-brcm-%d)\n", ehci_id);

	memset(devRes, 0, sizeof(devRes));
	devRes[0].name = ehci_id == 0 ? "brcm-ehci0-IO" :  "brcm-ehci1-IO";
	devRes[0].start = ehci_id == 0 ? EHC_BASE_ADDR : EHC2_BASE_ADDR;
	devRes[0].end = ehci_id == 0 ? EHC_END_ADDR : EHC2_END_ADDR;
	devRes[0].flags = IORESOURCE_MEM;
	devRes[1].name = ehci_id == 0 ? "brcm-ehci0-irq" :  "brcm-ehci1-irq";
	devRes[1].start = ehci_id == 0 ? EHC_INT_VECTOR : EHC2_INT_VECTOR;
	devRes[1].end = ehci_id == 0 ? EHC_INT_VECTOR : EHC2_INT_VECTOR;
	devRes[1].flags = IORESOURCE_IRQ;

#ifdef CONFIG_MIPS_BCM7400
	/* IRQ lines for EHCI-1 and OHCI-0 are swapped on 7400 prior to D0 */
	if((BDEV_RD(BCHP_SUN_TOP_CTRL_PROD_REVISION) & 0xffff) < 0x0030) {
		devRes[1].start = ehci_id == 0 ? EHC_INT_VECTOR : HC_INT_VECTOR;
		devRes[1].end = ehci_id == 0 ? EHC_INT_VECTOR : HC_INT_VECTOR;
	}
#endif

	// Before we register the driver, add a simple device matching our driver
	plat_dev[ehci_id] = platform_device_register_simple(
		(char *) ehci_hcd_brcm_driver[ehci_id].name,
		ehci_id, /* ID */
		devRes,
		2);
	if (IS_ERR(plat_dev[ehci_id])) {
		printk("ehci_hcd_brcm_init: device register failed, err=%d\n", err);
		return PTR_ERR(plat_dev[ehci_id]);
	}

        // Set up dma_mask for our platform device
        /*
                PR53481: make sure dma_mask at upper boundary of low RAM so it is DMAable and uncached (in kseg1)
        */
        plat_dev[ehci_id]->dev.dma_mask = &plat_dev[ehci_id]->dev.coherent_dma_mask;
        plat_dev[ehci_id]->dev.coherent_dma_mask = (u64)(0x10000000UL - 1UL);

	err = driver_register(&ehci_hcd_brcm_driver[ehci_id]);
	if (err) {
		printk("ehci_hcd_brcm_init: driver_register failed, err=%d\n", err);
		return err;
	}

	return(0);
}

static int ehci_brcm_enable(void *arg)
{
	int i;
	int err = -1;

	if(atomic_cmpxchg(&ehci_brcm_state, BRCM_USB_SLEEPING,
		BRCM_USB_TRANSITION) != BRCM_USB_SLEEPING) {
		return(-1);
	}

	printk("ehci_hcd_brcm_init: Initializing %d EHCI controller(s)\n", NUM_EHCI_HOSTS);

	brcm_pm_usb_add();
	for (i=0; i < NUM_EHCI_HOSTS; i++) {
		err = ehci_hcd_brcm_init_each(i);
		if (err) {
			do {
				driver_unregister(&ehci_hcd_brcm_driver[i]);
				platform_device_unregister(plat_dev[i]);
				i--;
			} while(i >= 0);
			atomic_set(&ehci_brcm_state, BRCM_USB_SLEEPING);

			return err;
		}
	}
	atomic_set(&ehci_brcm_state, BRCM_USB_AWAKE);
	return(0);
}

static int ehci_brcm_disable(void *arg)
{
	int i;

	if(atomic_cmpxchg(&ehci_brcm_state, BRCM_USB_AWAKE,
		BRCM_USB_TRANSITION) != BRCM_USB_AWAKE) {
		return(-1);
	}

	printk("ehci_hcd_brcm_cleanup: Taking down %d EHCI controller(s)\n", NUM_EHCI_HOSTS);

	for (i=0; i < NUM_EHCI_HOSTS; i++) {
		driver_unregister(&ehci_hcd_brcm_driver[i]);
		platform_device_unregister(plat_dev[i]);
	}
	brcm_pm_usb_remove();
	atomic_set(&ehci_brcm_state, BRCM_USB_SLEEPING);
	return(0);
}

static int __init ehci_hcd_brcm_init (void)
{
	int ret;

	ret = ehci_brcm_enable(NULL);
	if(ret)
		return(ret);
#if defined(CONFIG_BRCM_PM)
	brcm_pm_register_ehci(ehci_brcm_disable, ehci_brcm_enable, NULL);
#endif
	return(0);
}


static void __exit ehci_hcd_brcm_cleanup (void)
{
#if defined(CONFIG_BRCM_PM)
	brcm_pm_unregister_ehci();
#endif
	ehci_brcm_disable(NULL);
}

module_init (ehci_hcd_brcm_init);
module_exit (ehci_hcd_brcm_cleanup);
