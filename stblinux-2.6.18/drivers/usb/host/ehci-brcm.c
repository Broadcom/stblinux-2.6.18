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


extern int usb_disabled(void);

/*-------------------------------------------------------------------------*/

static void brcm_ehci_hw_init(struct platform_device *dev)
{
	// Init BRCM USB setup registers for board and HC specific issues
   	writel( BRCM_USB_SETUP_REG_VAL, BrcmUsbSetupReg );

#ifdef CONFIG_MIPS_BCM7317
    //straighten out frame length
    writel( 0x000c0020, (volatile u32 *) 0xfffe81f8 );

    //set generic_ctl_8 (USB_UTMICTRL.clock_60_invert) & 7 bit sync_det_length
    writel( ((readl( (volatile u32 *) 0xfffe81ec ) & ~(7 << 16)) | 0x00060100),
            (volatile u32 *) 0xfffe81ec );

#ifdef CONFIG_USB_BRCM_EXT_XTAL
    writel( 0xfd, (volatile u32 *) 0xfffe81f4 );
#endif
#endif

#if defined( CONFIG_MIPS_BCM7038 ) || defined( CONFIG_MIPS_BCM7400 ) \
	|| defined( CONFIG_MIPS_BCM3560B0 ) \
	|| defined( CONFIG_MIPS_BCM7401 ) || defined( CONFIG_MIPS_BCM7402 ) \
	|| defined( CONFIG_MIPS_BCM7440 ) 

  	{
  		unsigned long hcBaseAddr = (unsigned long) dev->resource[0].start;

printk("brcm_ehci_hw_init: Reset at %08x\n", KSEG1ADDR(hcBaseAddr + 0x10));

	   /* This fix the lockup  during reboot */
	   *((volatile u32 *) KSEG1ADDR(hcBaseAddr + 0x10)) = 2;
  	}
#endif
}

static void brcm_stop_ehci(struct platform_device *dev)
{
	printk(KERN_DEBUG __FILE__
	       ": stopping brcm EHCI USB Controller\n");
}



/*-------------------------------------------------------------------------*/

static int __devinit
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

	/* force HC to halt state */
	return ehci_halt (ehci);
	
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
	ehci_brcm_reset (hcd);
	ehci_init(hcd);

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

static int __devinit
ehci_brcm_start (struct usb_hcd *hcd)
{
	int retval;

	retval = ehci_brcm_reset(hcd);
	if (0 == retval) {
        	if ((retval = ehci_run (hcd)) < 0) {
				printk ("can't start %s", hcd->self.bus_name);
				return retval;
        	}
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
	 suspend:		ehci_suspend, 
	 resume:		ehci_resume,   
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
	.hub_suspend = ehci_hub_suspend,
	.hub_resume = ehci_hub_resume,
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
#ifdef CONFIG_MIPS_BCM7400
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

#define NUM_EHCI_PORT (sizeof(ehci_hcd_brcm_driver)/sizeof(struct device_driver))

#define DRIVER_INFO DRIVER_VERSION " " DRIVER_DESC

#ifndef EHC2_BASE_ADDR
#define EHC2_BASE_ADDR EHC_BASE_ADDR
#endif

#ifndef EHC2_END_ADDR
#define EHC2_END_ADDR EHC_END_ADDR
#endif

#ifndef EHC2_INT_VECTOR
#define EHC2_INT_VECTOR EHC_INT_VECTOR
#endif

static int __init ehci_hcd_brcm_init_each (int ehci_id)
{
	int err = 0;
	struct resource devRes[2];
	struct platform_device* pdev;
	
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

	// Before we register the driver, add a simple device matching our driver
	pdev = platform_device_register_simple(
		(char *) ehci_hcd_brcm_driver[ehci_id].name,
		ehci_id, /* ID */
		devRes,
		2);
	if (IS_ERR(pdev)) {
		printk("ehci_hcd_brcm_init: device register failed, err=%d\n", err);
		return PTR_ERR(pdev);
	}

	// Set up dma_mask for our platform device
	// Overwrite whatever value it was set to.
	if (1 /*!pdev->dev.dma_mask*/) {
		extern phys_t upper_memory;

		//dma_set_mask(&pdev->dev, (u64) ((unsigned long) upper_memory - 1UL)); // default is 32MB 0x01ffffff;
		//dma_set_mask(&pdev->dev, 0x01ffffff);
		//pdev->dev.dma_mask = (u64*) 0x01ffffff;  
		pdev->dev.dma_mask = &pdev->dev.coherent_dma_mask; 
		pdev->dev.coherent_dma_mask = (u64)  ( upper_memory - 1UL);

	}
		
	err = driver_register(&ehci_hcd_brcm_driver[ehci_id]);
	if (err) {
		printk("ehci_hcd_brcm_init: driver_register failed, err=%d\n", err);
		return err;
	}
	
}

static void __exit ehci_hcd_brcm_cleanup_each (int ehci_id)
{
	driver_unregister(&ehci_hcd_brcm_driver[ehci_id]);
}

static int __init ehci_hcd_brcm_init (void)
{
	int i;
	int err = -1;

	printk("ehci_hcd_brcm_init: Initializing %d EHCI devices\n", NUM_EHCI_PORT);
	for (i=0; i < NUM_EHCI_PORT; i++) {
		err = ehci_hcd_brcm_init_each(i);
		if (err) {
			return err;
		}
	}
	return err;
}


static void __exit ehci_hcd_brcm_cleanup (void)
{
	int i;

	printk("ehci_hcd_brcm_cleanup: Taking down %d EHCI devices\n", NUM_EHCI_PORT);
	for (i=0; i < NUM_EHCI_PORT; i++) {
		ehci_hcd_brcm_cleanup_each(i);
	}
}

module_init (ehci_hcd_brcm_init);
module_exit (ehci_hcd_brcm_cleanup);
