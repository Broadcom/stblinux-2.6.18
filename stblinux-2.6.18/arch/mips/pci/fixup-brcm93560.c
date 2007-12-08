/*
 *
 * BRIEF MODULE DESCRIPTION
 *	Bcm93560 Board specific pci fixups.
 *
 * Copyright 2004-2005 Broadcom Corp.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Revision Log
 * who when    what
 * tht  080904 New way of doing it in 2.6 kernel
 * tht  041004 Adapted from sample codes from kernel tree
 */
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci_ids.h>
#include <linux/ioport.h>
#include <asm/io.h>

#ifdef CONFIG_MIPS_BCM3560A0
#include <asm/brcmstb/brcm93560/boardmap.h>
#include <asm/brcmstb/brcm93560/bchp_hif_cpu_intr1.h>
#include <asm/brcmstb/brcm93560/bcmintrnum.h>

#elif defined( CONFIG_MIPS_BCM3560B0 )
#include <asm/brcmstb/brcm93560b0/boardmap.h>
#include <asm/brcmstb/brcm93560b0/bchp_hif_cpu_intr1.h>
#include <asm/brcmstb/brcm93560b0/bcmintrnum.h>

#elif defined( CONFIG_MIPS_BCM3563 )
#include <asm/brcmstb/brcm93563/boardmap.h>
#include <asm/brcmstb/brcm93563/bchp_hif_cpu_intr1.h>
#include <asm/brcmstb/brcm93563/bcmintrnum.h>

#elif defined( CONFIG_MIPS_BCM3563C0 )
#include <asm/brcmstb/brcm93563c0/boardmap.h>
#include <asm/brcmstb/brcm93563c0/bchp_hif_cpu_intr1.h>
#include <asm/brcmstb/brcm93563c0/bcmintrnum.h>

#else
#error "Invalid platform"
#endif

#define DEBUG 0

//#define USE_SECONDARY_SATA  // TUrn on 2nd channel, and turn on DMA for 2nd channel
//#define USE_7038B0_WAR		// Will no longer needed with B1 rev.
//#define USE_TURN_OFF_SATA2_WAR  // when not defined, turn on 2nd channel only, no DMA


/* Since the following is not defined in any of our header files. */
#define MIPS_PCI_XCFG_INDEX     0xf0600004
#define MIPS_PCI_XCFG_DATA      0xf0600008


/* For now until the BSP defines them */
/* For SATA MMIO BAR5 */
#define PCI_SATA_PHYS_MEM_WIN5_BASE \
	(PCI_7041_PHYS_MEM_WIN0_BASE+0xc0000)
//#define PCI_SATA_PHYS_MEM_WIN5_BASE 0x02000000
#define PCI_SATA_PHYS_MEM_WIN5_SIZE 0x2000

#define PCI_EXPANSION_PHYS_MEM_WIN0_BASE \
	(PCI_SATA_PHYS_MEM_WIN5_BASE+PCI_SATA_PHYS_MEM_WIN5_SIZE)

#define PCI_EXPANSION_PHYS_IO_WIN0_BASE 0x400

// 2nd PCI slot on certain boards.
#define PCI_DEVICE_ID_EXT2 0x0c



static char irq_tab_brcm97038[] __initdata = {
//[slot]  = IRQ
#ifdef CONFIG_MIPS_BCM3560A0
  [PCI_DEVICE_ID_USB]     = BCM_LINUX_USB_HOST_IRQ,     /* BCM3250  */
#endif

  [PCI_DEVICE_ID_1394]    = BCM_LINUX_1394_IRQ,     	/* 1394 */
// jipeng - FIXME
#if !defined(CONFIG_MIPS_BCM3563) && !defined(CONFIG_MIPS_BCM3563C0)
  [PCI_DEVICE_ID_EXT] 	  = BCM_LINUX_EXT_PCI_IRQ, 		/* On-board PCI slot */
#endif
};


int __init pcibios_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
	return irq_tab_brcm97038[slot];
}

/* 
 * Do platform specific device initialization at pci_enable_device() time
 *
 * THT: For the external USB Host controller on 3560A0, need to reset the chip,
 * or they will report random errors
 */
int pcibios_plat_dev_init(struct pci_dev *dev)
{
#if 0
	u32 base;
	volatile u32* ohci_cmd;
	volatile u32* ehci_cmd;
	
	if (PCI_DEVICE_ID_USB == PCI_SLOT(dev->devfn)) {
		switch(PCI_FUNC(dev->devfn)) {
		case 0:
		case 1:
			/* OHCI function 0 and 1 */
			pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &base);
			ohci_cmd = (u32*) (base + 0x8);
printk("OHCI base = %p\n", ohci_cmd);
			*ohci_cmd |= 1;

			break;
			
		case 2:
			pci_read_config_dword( dev, PCI_BASE_ADDRESS_0, &base);
			ehci_cmd = (u32*) (base + 0x10);
printk("EHCI base = %p\n", ehci_cmd);
			writel (2, ehci_cmd); 
			break;

		default:
			printk("Unrecognized function for device %4x:%4x\n", dev->vendor, dev->device);
		}
	}
	return 0;
#endif	
}



// global to allow multiple slots
static u32 memAddr = 0xd0000000;	//PCI_EXPANSION_PHYS_MEM_WIN0_BASE;
static u32 ioAddr = 0x100; 		    //PCI_EXPANSION_PHYS_IO_WIN0_BASE;

// Remembers which devices have already been assigned.
static  int slots[32];


static int __devinit brcm_quirk_io_mem_region(struct pci_dev *dev, unsigned region, unsigned size, int nr, unsigned int flag)
{
	region &= ~(size-1);
	if (region) {
		struct resource *res = dev->resource + nr;

		res->name = pci_name(dev);
		res->start = region;
		res->end = region + size - 1;
		res->flags = flag;
		// THT This call prevents request_mem_region() from succeeding
		//return pci_claim_resource(dev, nr);
		return 0; 
	}
	return -EINVAL;
}

struct pci_dev* usbdev[5];


/* Do IO/memory allocation for expansion slot here */
static void __devinit brcm_pcibios_fixup_plugnplay(struct pci_dev *dev) 
{
	int i,j,k,l, iosize, msize;
	//int useIOAddr = 0;
	u32 savRegData;
	u32 size;
	u32 mask;
	int slot;
	u32 regData;

static int	 usbind;	

	/*
	* Make sure that it is in the PCI slot, otherwise we punt.
	*/


	slot = PCI_SLOT(dev->devfn);

#if 0
/* Disable 1394 */
if (PCI_DEVICE_ID_1394 == slot) {
	return;
}
#endif



//	pcibios_update_irq(dev, BCM_LINUX_EXT_PCI_IRQ);
//	dev->irq = BCM_LINUX_EXT_PCI_IRQ;


	printk("\tPCI DEV in PCI slot, ID=%04x:%04x\n\r", dev->vendor, dev->device);
if (usbind < 5) {
usbdev[usbind] = dev;
usbind++;
}
	

	/* Write FFFFFFFFH to BAR registers to probe for IO or Mem */
	iosize = msize = 0;
	for (i=0,j=0; i<6; i++,j+=4) {
		pci_write_config_dword(dev,PCI_BASE_ADDRESS_0+j,0xffffffff);
		pci_read_config_dword(dev,PCI_BASE_ADDRESS_0+j,&regData);
		if (regData) {
			printk("PnP: PCI_BAR[%d] = %x\n", i, regData);


			savRegData = regData;
			regData &= 0xfffffff0; /* Get rid of Bit 0-3 0=Mem, 2:1 32/64, 3=
Prefetchable */
			/*
			* determine io/mem size here by looking at the BAR
			* register, skipping the first 4 bits
			*/
			for (k=4; k<32; k++) {
				if (regData & (1<<k)) {
					break;
				}
			}
			if (k < 32) {
				printk("PnP: size requested is %x\n", 1<<k);
				size = 1<<k;
				mask = 0xffffffff;
				for (l=0; l<k; l++) {
					mask &= ~(1<<l);
				}
			}
			else
				break;

			if (savRegData & 0x1) {	/* IO address */
				/* Calculate the next address that satisfies the boundary condition */
				regData = (ioAddr + size - 1) & mask;
				if (brcm_quirk_io_mem_region(dev, regData, size, j, IORESOURCE_IO)) {
					printk("PnP: Cannot allocate IO resource[%d]=(%04x,%04x)\n, j, regData, regData + size - 1");
				}
				pci_write_config_dword(dev, PCI_BASE_ADDRESS_0+j, regData);
				printk("PnP: Writing PCI_IO_BAR[%d]=%x, size=%d, mask=%x\n", i,
					regData, 1<<k, mask);
	printk("Bar%d: start=%08x, len=%d\n", j, pci_resource_start(dev, j), pci_resource_len(dev, j));
				iosize = regData + size;
				ioAddr = regData + size; // Advance it
			}
			else { /* Mem address, tag on to the last one, the 7041 mem area */
				/* Calculate the next address that satisfies the boundary condition */
				regData = (memAddr + size - 1) & ~(size -1);
				if (brcm_quirk_io_mem_region(dev, regData, size, j, IORESOURCE_MEM)) {
					printk("PnP: Cannot allocate MEM resource[%d]=(%08x,%08x)\n, j, regData, regData + size - 1");
				}
				pci_write_config_dword(dev, PCI_BASE_ADDRESS_0+j, regData);
				printk("PnP: Writing PCI_MEM_BAR[%d]=%x, size=%d, mask=%x\n", i,
					regData, 1<<k, mask);
printk("Bar%d: start=%08x, len=%d\n", j, pci_resource_start(dev, j), pci_resource_len(dev, j));
				msize = regData + size;
				memAddr = regData+size; // Advance it
			}
		}
	}
	/*  now that it's all configured, turn on the Command Register */
	pci_read_config_dword(dev,PCI_COMMAND,&regData);
	regData |= PCI_COMMAND_SERR|PCI_COMMAND_PARITY |PCI_COMMAND_MASTER;
	if (iosize > 0)
		regData |= PCI_COMMAND_IO;
	if (msize > 0)
		regData |= PCI_COMMAND_MEMORY;

	printk("\tPnP: PCI_DEV %04x:%04x, command=%x, msize=%08x, iosize=%08x\n", 
		dev->vendor, dev->device, regData, msize, iosize);
	pci_write_config_dword(dev,PCI_COMMAND,regData);

	
}

#if 0
DECLARE_PCI_FIXUP_FINAL(PCI_ANY_ID, PCI_ANY_ID,
	 	brcm_pcibios_fixup_plugnplay);
#endif


void dump_pci(void)
{
#if 0
	struct pci_dev* dev;

printk("$$$$$$$$$$$$$$$$$$$$$$$$$$$ dump_pci $$$$$$$$$$$$$$$$$$$$$$$\n");
	//for_each_pci_dev(dev) 

	int i;

	for (i=0; i< 5; i++)
	{
	  if (usbdev[i] != NULL) {
	  	dev = usbdev[i];
		
		printk("dump_pci: @dev=%p, dev->irq = %d, devfn=%x, vendor=%x, did=%x\n", dev, dev->irq, dev->devfn, dev->vendor, dev->device);
		printk("dump_pci: @res=%p, res_start=%08x, len=%08x, bar.start=%08x, end=%08x\n", 
			&dev->resource[0], pci_resource_start (dev, 0), pci_resource_len (dev, 0), dev->resource[0].start, dev->resource[0].end);		
	    }
	  }
#endif
}
EXPORT_SYMBOL(dump_pci);



