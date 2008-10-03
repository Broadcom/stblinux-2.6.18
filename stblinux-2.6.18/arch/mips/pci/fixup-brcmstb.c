/*---------------------------------------------------------------------------

    Copyright (c) 2001-2008 Broadcom Corporation                 /\
                                                          _     /  \     _
    _____________________________________________________/ \   /    \   / \_
                                                            \_/      \_/  
    
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 as
 published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    File: fixup-brcmstb.c

    Description: 
    PCI fixups for STB platforms

 ------------------------------------------------------------------------- */

#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci_ids.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/brcmstb/common/brcmstb.h>

#define PCI_A0		\
	(1+32+BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCI_INTA_0_CPU_INTR_SHIFT)
#define PCI_A1		\
	(1+32+BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCI_INTA_1_CPU_INTR_SHIFT)
#define PCI_A2		\
	(1+32+BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCI_INTA_2_CPU_INTR_SHIFT)

#define NUM_SLOTS	16

/* Note for supporting customized boards: slot_number = idsel_line - 16 */

static char irq_tab_brcmstb[NUM_SLOTS][4] __initdata = {
	[4]  =	{  PCI_A2, PCI_A0,	0,	0	}, /* mPCI */
	[13] =	{  PCI_A0, PCI_A1,	0,	0	}, /* EXT PCI */
	[14] =	{  PCI_A1,	0,	0,	0	}, /* 1394 */
};

static char irq_tab_brcmstb_docsis[NUM_SLOTS][4] __initdata = {
	[4]  =	{  PCI_A2, PCI_A0,	0,	0	}, /* mPCI */
	[7]  =	{  PCI_A1,	0,	0,	0	}, /* BCM325x */
	[13] =	{  PCI_A0, PCI_A1,	0,	0	}, /* EXT PCI */
	[14] =	{  PCI_A2,	0,	0,	0	}, /* 1394 */
};

int __init pcibios_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
#if defined(BRCM_SATA_SUPPORTED)
	if(pci_io_map_base(dev->bus) == PCI_SATA_IO_START)
		return(BCM_LINUX_SATA_IRQ);
#endif
#if defined(BRCM_PCIE_SUPPORTED)
	if(pci_io_map_base(dev->bus) == PCIE_IO_START)
		return(BCM_LINUX_PCIE_INTA_IRQ);
#endif
	if((slot >= NUM_SLOTS) || ((pin - 1) > 3))
		return(0);
	return(brcm_docsis_platform ?
		irq_tab_brcmstb_docsis[slot][pin - 1] :
		irq_tab_brcmstb[slot][pin - 1]);
}

/* Do platform specific device initialization at pci_enable_device() time */
int pcibios_plat_dev_init(struct pci_dev *dev)
{
	return 0;
}

static void brcm_pcibios_fixup(struct pci_dev *dev) 
{
	int slot = PCI_SLOT(dev->devfn);
	char *busname = "UNKNOWN";

	switch(pci_io_map_base(dev->bus)) {
		case PCI_IO_START:
			busname = "PCI";
			break;
		case PCI_SATA_IO_START:
			busname = "SATA";
			break;
		case PCIE_IO_START:
			busname = "PCIe";
			break;
	}

	printk(KERN_INFO
		"PCI: found device %04x:%04x on %s bus, slot %d (irq %d)\n",
		dev->vendor, dev->device, busname, slot,
		pcibios_map_irq(dev, slot, 1));

	/* zero out the BARs and let Linux assign an address */
	pci_write_config_dword(dev, PCI_COMMAND, 0);
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, 0);
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_1, 0);
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_2, 0);
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_3, 0);
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_4, 0);
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_5, 0);
}

DECLARE_PCI_FIXUP_EARLY(PCI_ANY_ID, PCI_ANY_ID, brcm_pcibios_fixup);
