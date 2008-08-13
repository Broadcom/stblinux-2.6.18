/*
 *
 * BRIEF MODULE DESCRIPTION
 *	Bcm97038C0 Board specific pci fixups.
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
 * who      when    what
 * tht      080904  New way of doing it in 2.6 kernel
 * tht      041004  Adapted from sample codes from kernel tree
 * jipeng   032806  Add support 3255 IRQ for 97455/97456
 */
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci_ids.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/brcmstb/common/brcmstb.h>

/* from PCI spec, Maybe we can put this in some include file. */
#define PCI_ENABLE              0x80000000
#define PCI_IDSEL(x)		(((x)&0x1f)<<11)
#define PCI_FNCSEL(x)		(((x)&0x7)<<8)

//#define BCM7401_SATA_VID		0x02421166

#define BCM7401_1394_VID		0x8101104c


// SATA mem is now on different window, so expansion starts where the OLD Bx 7041 was
#define PCI_EXPANSION_PHYS_MEM_WIN0_BASE 0xd1000000


#define PCI_EXPANSION_PHYS_IO_WIN0_BASE 0x400

static char irq_tab_brcm97402[] __initdata = {
//[slot]  = IRQ
//  [PCI_DEVICE_ID_SATA] = BCM_LINUX_SATA_IRQ,    /* SATA controller */
  [PCI_DEVICE_ID_EXT]  = BCM_LINUX_EXT_PCI_IRQ, /* On-board PCI slot */
  [PCI_DEVICE_ID_MINI] = BCM_LINUX_MINI_PCI_IRQ,					    
  [PCI_DEVICE_ID_1394] = BCM_LINUX_1394_IRQ,                 
};


int __init pcibios_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
#ifdef DEBUG	
    printk("pcibios_map_irq: slot %d pin %d IRQ %d\n", slot, pin, irq_tab_brcm97402[slot]);
#endif
    return irq_tab_brcm97402[slot];
}

/* Do platform specific device initialization at pci_enable_device() time */
int pcibios_plat_dev_init(struct pci_dev *dev)
{
	return 0;
}



// global to allow multiple slots
static u32 memAddr = PCI_EXPANSION_PHYS_MEM_WIN0_BASE;
static u32 ioAddr = PCI_EXPANSION_PHYS_IO_WIN0_BASE;

// Remembers which devices have already been assigned.
static  int slots[32];

/* Do IO/memory allocation for expansion slot here */
static void brcm_pcibios_fixup_plugnplay(struct pci_dev *dev) 
{
	int i,j,k,l;
	volatile unsigned int iosize, msize;
	//int useIOAddr = 0;
	u32 savRegData;
	u32 size;
	u32 mask;
	int slot;
	u32 regData;
		

	/*
	* Make sure that it is in the PCI slot, otherwise we punt.
	*/

	if (dev->bus->number != 0)
		return;


	slot = PCI_SLOT(dev->devfn);

#if 0
	/* Only handle devices in the PCI slot */
	if ( !(PCI_DEVICE_ID_EXT == slot || PCI_DEVICE_ID_EXT2 == slot)) {
		printk("\tDev ID %04x:%04x ignored\n", dev->vendor, dev->device);
		return;
	}
	

	if (slots[slot]) {
		printk("Skip resource assignment for dev ID %04x:%04x already allocated\n", dev->vendor, dev->device);
		return;
	}

	slots[slot] = 1;
#endif

	printk("\tPCI DEV in slot %x, ID=%04x:%04x\n\r", slot, dev->vendor, dev->device);
	

	/* Write FFFFFFFFH to BAR registers to probe for IO or Mem */
	iosize = msize = 0;
	for (i=0,j=0; i<6; i++,j+=4) {
		pci_write_config_dword(dev,PCI_BASE_ADDRESS_0+j,0xffffffff);
		pci_read_config_dword(dev,PCI_BASE_ADDRESS_0+j,&regData);
		if (regData) {
			printk("PCI PnP: PCI_BAR[%d] = %x\n", i, regData);


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
				printk("PCI PnP: size requested is %x\n", 1<<k);
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
				dev->resource[i].start = regData;
				dev->resource[i].end = regData + size - 1;
				if (insert_resource(&ioport_resource, &dev->resource[i])) {
					printk("PnP: Cannot allocate IO resource[%d]=(%04x,%04x)\n", i, regData, regData + size - 1);
				}
				pci_write_config_dword(dev, PCI_BASE_ADDRESS_0+j, regData);
				printk("PnP: Writing PCI_IO_BAR[%d]=%x, size=%d, mask=%x\n", i,
					regData, 1<<k, mask);
				iosize = regData + size;
				ioAddr = regData + size; // Advance it
			}
			else { /* Mem address, tag on to the last one, the 7041 mem area */
				/* Calculate the next address that satisfies the boundary condition */
				regData = (memAddr + size - 1) & ~(size -1);
				dev->resource[i].start = regData;
				dev->resource[i].end = regData + size - 1;
				if (insert_resource(&iomem_resource, &dev->resource[i])) {
					printk("PnP: Cannot allocate MEM resource[%d]=(%08x,%08x)\n", i, regData, regData + size - 1);
				}
				pci_write_config_dword(dev, PCI_BASE_ADDRESS_0+j, regData);
				printk("PnP: Writing PCI_MEM_BAR[%d]=%x, size=%d, mask=%x\n", i,
					regData, 1<<k, mask);
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



DECLARE_PCI_FIXUP_HEADER(PCI_ANY_ID, PCI_ANY_ID,
	 	brcm_pcibios_fixup_plugnplay);


//#endif CONFIG_PCI_AUTO



/* --------------------------------------------------------------------------
    Name: DumpRegs
Abstract: Dump the IDE registers
 -------------------------------------------------------------------------- */

void
dump_ide(void)
{


}
EXPORT_SYMBOL(dump_ide);




