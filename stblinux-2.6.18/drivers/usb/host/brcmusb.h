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
*/

#ifndef __BRCMUSB_H__
#define __BRCMUSB_H__

#include <linux/config.h>
#include <linux/version.h>
#include <asm/brcmstb/common/brcmstb.h>

#if defined(CONFIG_PCI) && defined(CONFIG_SWAP_IO_SPACE_L)
#undef CONFIG_SWAP_IO_SPACE_L
#endif

#if defined(CONFIG_MIPS_BCM7328)
#error "USB not supported on BCM7328"
#endif

#if defined(CONFIG_MIPS_BCM7038A0)
#error "USB not supported on BCM7038A0"
#endif

#if defined( CONFIG_MIPS_BCM7038 )						//remove when added to bcm7038.h
  #define USB_HOST_ADR_BASE        0x104c0400
  #define USB_HOST2_ADR_BASE       0x104c0500
  #define USB_EHC_ADR_BASE         0x104c0300
  #ifdef CONFIG_MIPS_BCM7038C0
    #define BCM_LINUX_USB_HOST2_IRQ  BCM_LINUX_USB_OHCI_1_CPU_INTR
  #else
    #define BCM_LINUX_USB_HOST2_IRQ  BCM_LINUX_USB_HOST_IRQ
  #endif

#elif defined(CONFIG_MIPS_BCM3560B0)
  #define USB_HOST_ADR_BASE        0x104c0400
// There is only 1 OHCI controller  #define USB_HOST2_ADR_BASE       0x104c0500
  #define USB_EHC_ADR_BASE         0x104c0300

#elif defined( CONFIG_MIPS_BCM7400 ) 
 #define USB_HOST_ADR_BASE        0x10480400
 #define USB_HOST2_ADR_BASE       0x10480600
 #define USB_EHC_ADR_BASE         0x10480300
 #define USB_EHC2_ADR_BASE        0x10480500
 
 #define BCM_LINUX_USB_HOST2_IRQ  BCM_LINUX_USB_OHCI_1_CPU_INTR

#elif defined( CONFIG_MIPS_BCM7401 ) || defined( CONFIG_MIPS_BCM7402 ) \
      || defined( CONFIG_MIPS_BCM7118A0 )
 #define USB_HOST_ADR_BASE        0x10480400
 #define USB_HOST2_ADR_BASE       0x10480500
 #define USB_EHC_ADR_BASE         0x10480300
 
 #define BCM_LINUX_USB_HOST2_IRQ  BCM_LINUX_USB_OHCI_1_CPU_INTR


#elif defined( CONFIG_MIPS_BCM7440 ) 
 #define USB_HOST_ADR_BASE        0x10480400
 //#define USB_HOST2_ADR_BASE       0x10480500
 #define USB_EHC_ADR_BASE         0x10480300
 
 //#define BCM_LINUX_USB_HOST2_IRQ  BCM_LINUX_USB_OHCI_1_CPU_INTR

 
#endif

#define TRACE()			printk("%s: @%d\n", __FUNCTION__, __LINE__)
//static volatile int Debug;

// No longer need this trick in 2.6
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
  #ifndef CONFIG_PCI
    #define CONFIG_PCI
  #endif

  #include <linux/pci.h>

#elif defined (CONFIG_MIPS_BRCM97XXX)
  // 2.6.x: Disable PCI for SuperChips bcm7xxx.  PCI is needed for Venom2 lines, however.
  #ifdef CONFIG_PCI
    #undef CONFIG_PCI
  #endif
#endif // if Kernel is 2.5.x or earlier, elif SUperChip

//*****************************************************************************
#define HC_BASE_ADDR             USB_HOST_ADR_BASE
#define HC_END_ADDR              (HC_BASE_ADDR+0x6f)
#if defined( CONFIG_MIPS_BCM7038C0 ) || defined(CONFIG_MIPS_BCM3560B0) \
 || defined( CONFIG_MIPS_BCM7400 ) || defined( CONFIG_MIPS_BCM7440 )\
 || defined( CONFIG_MIPS_BCM7401 ) || defined( CONFIG_MIPS_BCM7402 ) \
 || defined( CONFIG_MIPS_BCM7118A0 )
  #define HC_INT_VECTOR            BCM_LINUX_USB_OHCI_0_CPU_INTR
#else
#define HC_INT_VECTOR            BCM_LINUX_USB_HOST_IRQ
#endif

#if defined( CONFIG_MIPS_BCM7038 ) || defined(CONFIG_MIPS_BCM3560B0)
 #define BrcmUsbSetupReg         ((volatile u32 *) 0xb04c0228)
 #define EHC_BASE_ADDR            USB_EHC_ADR_BASE
 #define EHC_END_ADDR             (EHC_BASE_ADDR+0xa8)

#elif defined( CONFIG_MIPS_BCM7400 )
 #define BrcmUsbSetupReg         ((volatile u32 *) 0xb0480228)
 #define EHC_BASE_ADDR            USB_EHC_ADR_BASE
 #define EHC_END_ADDR             (EHC_BASE_ADDR+0xa8)
 
 #define EHC2_BASE_ADDR            USB_EHC2_ADR_BASE
 #define EHC2_END_ADDR             (EHC2_BASE_ADDR+0xa8)
#elif defined( CONFIG_MIPS_BCM7401 ) || defined( CONFIG_MIPS_BCM7402 ) \
  || defined( CONFIG_MIPS_BCM7118A0 ) || defined( CONFIG_MIPS_BCM7440 )
 #define BrcmUsbSetupReg         ((volatile u32 *) 0xb0480228)
 #define EHC_BASE_ADDR            USB_EHC_ADR_BASE
 #define EHC_END_ADDR             (EHC_BASE_ADDR+0xa8)
#else
   #if defined(CONFIG_MIPS_BCM7320) || defined(CONFIG_MIPS_BCM7319) \
   	|| defined(CONFIG_MIPS_BCM7327) || defined(CONFIG_MIPS_BCM7329)
   #define BrcmUsbSetupReg         ((volatile u32 *) (KSEG1ADDR(HC_BASE_ADDR+0xfc)))
   #else
   #define BrcmUsbSetupReg         ((volatile u32 *) (HC_BASE_ADDR+0xfc))
   #endif
 #define EHC_BASE_ADDR            (USB_HOST_ADR_BASE+0x70)
 #define EHC_END_ADDR             (EHC_BASE_ADDR+0x8f)
#endif
#if defined( CONFIG_MIPS_BCM7038C0 ) || defined(CONFIG_MIPS_BCM3560B0) \
 || defined( CONFIG_MIPS_BCM7400 ) || defined( CONFIG_MIPS_BCM7440 ) \
 || defined( CONFIG_MIPS_BCM7401 ) || defined( CONFIG_MIPS_BCM7402 ) \
 || defined( CONFIG_MIPS_BCM7118A0 )
  #define EHC_INT_VECTOR           BCM_LINUX_USB_EHCI_CPU_INTR
  #if defined( CONFIG_MIPS_BCM7400 )
    #define EHC2_INT_VECTOR        BCM_LINUX_USB_EHCI_1_CPU_INTR
  #endif
#else
#define EHC_INT_VECTOR           BCM_LINUX_USB_HOST_IRQ
#endif

#if defined(CONFIG_MIPS_BCM7320) || defined(CONFIG_MIPS_BCM7319) \
	/*|| defined(CONFIG_MIPS_BCM7329) */
 #define HC2_BASE_ADDR            USB_HOST2_ADR_BASE
 #define HC2_END_ADDR             (USB_HOST2_ADR_BASE+0xfff)
 #define HC2_INT_VECTOR           BCM_LINUX_USB_HOST2_IRQ
 #define BrcmUsb2SetupReg        ((volatile u32 *) (KSEG1ADDR(HC2_BASE_ADDR+0xfc)))
#elif defined(CONFIG_MIPS_BCM7038) || defined( CONFIG_MIPS_BCM7400 ) \
	|| defined( CONFIG_MIPS_BCM7401 ) || defined( CONFIG_MIPS_BCM7402 ) \
	|| defined( CONFIG_MIPS_BCM7118A0 )
 #define HC2_BASE_ADDR            USB_HOST2_ADR_BASE
 #define HC2_END_ADDR             (USB_HOST2_ADR_BASE+0x6f)
 #define HC2_INT_VECTOR           BCM_LINUX_USB_HOST2_IRQ
#endif

enum { BABO_LE=0x01, FNHW_LE=0x02, FNBO_REV=0x04, WABO_LE=0x08,
       IOC=0x10, IPP=0X20, CLV_BE=0x200000 };

//7038 & E* boards expect high active port power signal
#if defined(CONFIG_MIPS_BCM7038) || defined(CONFIG_DP522) || defined(CONFIG_DP922)  || defined(CONFIG_MIPS_BCM3560B0)
  #define PWR_CTL		IOC
#else
  #define PWR_CTL		(IPP+IOC)
#endif
#if defined (__MIPSEB__)
  #define BRCM_USB_SETUP_REG_VAL		(PWR_CTL+WABO_LE+FNBO_REV+FNHW_LE)
#else
  #define BRCM_USB_SETUP_REG_VAL		(PWR_CTL+FNHW_LE+BABO_LE)
#endif

/* THT PCI emulation no longer used, in 2.6.x, we now use the bus glue instead */
// Fix me: #ifdef case for Venom2 lines, which are PCI based.
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)


#define DRIVER_MODS "Modified for BRCM USB Host Controller"

#ifdef CONFIG_ROOT_NFS
static void usb_nfs_dev_wait(void)
{
	int i;

    printk("ROOT-NFS delay for device enumeration...");
   	for(i=0; i<4000; ++i)
    {
   	    schedule();     //let other threads run
        udelay(1000);
   	}
}

#else
#define usb_nfs_dev_wait()

#endif


//**********************************************************************************

#ifdef EHCI_BRCM
//some forward decls...
static const struct hc_driver ehci_driver;
int usb_hcd_pci_probe (struct pci_dev *dev, const struct pci_device_id *id);
void usb_hcd_pci_remove (struct pci_dev *dev);

static const struct pci_device_id __devinitdata brcm_ehci_pci_ids [] = 
{ 
	{

    /* handle any USB EHCI controller */
    .class =      ((PCI_CLASS_SERIAL_USB << 8) | 0x20),
    .class_mask =     ~0,
	.driver_data =	(unsigned long) &ehci_driver,

    /* no matter who makes it */
    vendor:     PCI_ANY_ID,
    device:     PCI_ANY_ID,
    subvendor:  PCI_ANY_ID,
    subdevice:  PCI_ANY_ID,

    }, { /* end: all zeroes */ }
};

static struct pci_dev *Pci_ehci_dev;
/* --------------------------------------------------------------------------
    Name: brcm_ehcd_init
 -------------------------------------------------------------------------- */

static inline int __init brcm_ehcd_init (void)
{
    int status = 0;

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

#ifdef CONFIG_MIPS_BCM7038
   /* This fix the lockup on 7038B1 during reboot */
   *((volatile u32 *) 0xb04c0310) = 2;
   *((volatile u32 *) 0xb04c0408) = 1;
   *((volatile u32 *) 0xb04c0508) = 1;
#endif

    Pci_ehci_dev = kmalloc( sizeof( struct pci_dev ), GFP_KERNEL );
    if( !Pci_ehci_dev )
        return -ENOMEM;
    strcpy( Pci_ehci_dev->name, "BRCM-EHCI" );
    strcpy( Pci_ehci_dev->slot_name, "EHCI-Direct");
    pci_resource_start(Pci_ehci_dev, 0) = EHC_BASE_ADDR;
    pci_resource_end(Pci_ehci_dev, 0) = EHC_END_ADDR;
    Pci_ehci_dev->irq = EHC_INT_VECTOR;
    status = usb_hcd_pci_probe (Pci_ehci_dev, &brcm_ehci_pci_ids[0]);

    return( status );
}

/* --------------------------------------------------------------------------
    Name: brcm_ehcd_cleanup
 -------------------------------------------------------------------------- */

static inline void brcm_ehcd_cleanup (void)
{
    usb_hcd_pci_remove( Pci_ehci_dev );
    if( Pci_ehci_dev )
        kfree( Pci_ehci_dev );
}

#elif defined(OHCI_BRCM)

//forward decls
static struct pci_driver ohci_pci_driver;
static int __devinit ohci_pci_probe (struct pci_dev *dev, const struct pci_device_id *id);
static void __devexit ohci_pci_remove (struct pci_dev *dev);

static const struct pci_device_id __devinitdata brcm_ohci_pci_ids [] = 
{ 
	{

    /* handle any USB OHCI controller */
    class:      ((PCI_CLASS_SERIAL_USB << 8) | 0x10),
    class_mask:     ~0,

    /* no matter who makes it */
    vendor:     PCI_ANY_ID,
    device:     PCI_ANY_ID,
    subvendor:  PCI_ANY_ID,
    subdevice:  PCI_ANY_ID,

    }, { /* end: all zeroes */ }
};

static struct pci_dev *Pci_dev[2];

/* --------------------------------------------------------------------------
    Name: brcm_ohcd_init
 -------------------------------------------------------------------------- */

static inline int __init brcm_ohcd_init (void)
{
    int status;

	// Init BRCM USB setup registers for board and HC specific issues
	writel( BRCM_USB_SETUP_REG_VAL, BrcmUsbSetupReg );
#ifdef CONFIG_MIPS_BCM7320
   	writel( BRCM_USB_SETUP_REG_VAL, BrcmUsb2SetupReg );
#endif

#ifdef CONFIG_MIPS_BCM7110
    {   //set 2nd external port to be USB device as per board
        u8 *tm_usb_sel = (u8 *) (TM_TOP_ADR_BASE + 2);
#if 1
		//GPIO5 (reset time) detection of device/host selection
		if( *((volatile u8 *) 0xfffe0053) & 0x20 )
			writeb( (readb( tm_usb_sel ) | 0x01), tm_usb_sel );    //Host port 2
		else
#endif
			writeb( (readb( tm_usb_sel ) & ~0x01), tm_usb_sel );   //Device
    }
#endif

    //record driver name for resource allocation purposes
    ohci_pci_driver.name = (char *) kmalloc( 30, GFP_KERNEL );
    strcpy( ohci_pci_driver.name, "brcm-usb-ohci" );
    ohci_pci_driver.id_table =  &brcm_ohci_pci_ids [0];

    Pci_dev[0] = kmalloc( sizeof( struct pci_dev ), GFP_KERNEL );
    if( !Pci_dev[0] )
        return -ENOMEM;
    strcpy( Pci_dev[0]->name, "BRCM-OHCI" );
    strcpy( Pci_dev[0]->slot_name, "OHCI-Direct" );
    pci_resource_start(Pci_dev[0], 0) = HC_BASE_ADDR;
    pci_resource_end(Pci_dev[0], 0) = HC_END_ADDR;
    Pci_dev[0]->irq = HC_INT_VECTOR;
    status = ohci_pci_probe (Pci_dev[0], &ohci_pci_driver.id_table[0]);

#if defined(CONFIG_MIPS_BCM7320) || defined(CONFIG_MIPS_BCM7038)
    Pci_dev[1] = kmalloc( sizeof( struct pci_dev ), GFP_KERNEL );
    if( !Pci_dev[1] )
        return -ENOMEM;
    strcpy( Pci_dev[1]->name, "BRCM-OHCI-2" );
    strcpy( Pci_dev[1]->slot_name, "OHCI-Direct2" );
    pci_resource_start(Pci_dev[1], 0) = HC2_BASE_ADDR;
    pci_resource_end(Pci_dev[1], 0) = HC2_END_ADDR;
    Pci_dev[1]->irq = HC2_INT_VECTOR;
    status = ohci_pci_probe (Pci_dev[1], &ohci_pci_driver.id_table[0]);
#endif

    return( status );
}

/* --------------------------------------------------------------------------
    Name: brcm_ohcd_cleanup
 -------------------------------------------------------------------------- */

static inline void  brcm_ohcd_cleanup (void)
{
    ohci_pci_remove( Pci_dev[0] );
    if( Pci_dev[0] )
        kfree( Pci_dev[0] );
    kfree( ohci_pci_driver.name );
#if defined(CONFIG_MIPS_BCM7320) || defined(CONFIG_MIPS_BCM7038)
    ohci_pci_remove( Pci_dev[1] );
    if( Pci_dev[1] )
        kfree( Pci_dev[1] );
#endif
}
#endif	//EHCI_BRCM, OHCI_BRCM


//some PCI prototypes
struct pci_pool * pci_pool_create (const char *, struct pci_dev *,
								   size_t, size_t, size_t, int);
void pci_pool_destroy (struct pci_pool *);
void * pci_pool_alloc (struct pci_pool *, int, dma_addr_t *);
void pci_pool_free (struct pci_pool *, void *, dma_addr_t);

//***********************************************************************
// PCI stub routines for non-PCI based Host Controllers

#define pci_register_driver		brcm_pci_register_driver
static inline int __init brcm_pci_register_driver(struct pci_driver *drv)
{

#ifdef EHCI_BRCM
	int ret;
    if ((ret = brcm_ehcd_init()) < 0)
        return ret;
#elif defined(OHCI_BRCM)
	int ret;
    if ((ret = brcm_ohcd_init()) < 0)
        return ret;
#endif	//EHCI_BRCM, OHCI_BRCM

    info(DRIVER_MODS);
#ifdef CONFIG_ROOT_NFS
    usb_nfs_dev_wait();
#endif
	
    return 1;
}

#define pci_unregister_driver		brcm_pci_unregister_driver
static inline void __exit brcm_pci_unregister_driver(struct pci_driver *drv)
{
#ifdef EHCI_BRCM
    brcm_ehcd_cleanup();
#elif defined(OHCI_BRCM)
    brcm_ohcd_cleanup();
#endif	//EHCI_BRCM, OHCI_BRCM
}


#define pci_module_init		brcm_pci_module_init
static inline int __init brcm_pci_module_init(struct pci_driver *drv)
{
    int rc = pci_register_driver (drv);
    if (rc > 0)
        return 0;
    if (rc == 0)
        rc = -ENODEV;

    /* if we get here, we need to clean up pci driver instance
     * and return some sort of error */
    //pci_unregister_driver (drv);

    return rc;
}

#define pci_set_master				brcm_pci_set_master
static inline void brcm_pci_set_master(struct pci_dev *dev)
{
}

#define pci_enable_device			brcm_pci_enable_device
static inline int brcm_pci_enable_device(struct pci_dev *dev)
{
	return 0;
}

#define pci_disable_device			brcm_pci_disable_device
static inline void brcm_pci_disable_device(struct pci_dev *dev)
{
}

#define pci_find_slot				brcm_pci_find_slot
static inline struct pci_dev *brcm_pci_find_slot(unsigned int bus, unsigned int devfn)
{
	return NULL;
}

#define pci_read_config_byte		brcm_pci_read_config_byte
static inline int brcm_pci_read_config_byte(struct pci_dev *dev, int where, u8 *val)
{
	*val = 0; 

	return 0;
}

#define pci_write_config_byte		brcm_pci_write_config_byte
static inline int brcm_pci_write_config_byte(struct pci_dev *dev, int where, u8 val)
{
	return 0;
}

#ifdef CONFIG_PM

#define pci_read_config_word		brcm_pci_read_config_word
static inline int brcm_pci_read_config_word(struct pci_dev *dev, int where, u16 *val)
{
	*val = 0; 

	return 0;
}

#define pci_write_config_word		brcm_pci_write_config_word
static inline int brcm_pci_write_config_word(struct pci_dev *dev, int where, u16 val)
{
	return 0;
}

#endif

#define pci_read_config_dword		brcm_pci_read_config_dword
static inline int brcm_pci_read_config_dword(struct pci_dev *dev, int where, u32 *val)
{
	*val = 0; 

	return 0;
}

#define pci_write_config_dword		brcm_pci_write_config_dword
static inline int brcm_pci_write_config_dword(struct pci_dev *dev, int where, u32 val)
{
	return 0;
}

#define pci_set_mwi					brcm_pci_set_mwi
static inline void brcm_pci_set_mwi(struct pci_dev *dev)
{
}

#define pci_clear_mwi				brcm_pci_clear_mwi
static inline void brcm_pci_clear_mwi(struct pci_dev *dev)
{
}

#define pci_set_dma_mask			brcm_pci_set_dma_mask
static inline int brcm_pci_set_dma_mask(struct pci_dev *dev, u64 mask)
{
	return 0;
}

#endif /* If Do Not use busglue codes */


#endif //__BRCMUSB_H__

