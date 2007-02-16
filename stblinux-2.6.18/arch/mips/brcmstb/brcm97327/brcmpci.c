/*
 *  arch/mips/brcm/brcmpci.c -- Broadcom BCM93730 pci setup routines
 *
 *  Copyright (C) 2000-2005 Troy Trammel <ttrammel@broadcom.com>
 *					Broadcom Corp.
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
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/ioport.h>


#include <asm/io.h>


/* from PCI spec, Maybe we can put this in some include file. */
#define PCI_ENABLE              0x80000000
#define PCI_IDSEL(x)		(((x)&0x1f)<<11)
#define PCI_FNCSEL(x)		(((x)&0x7)<<8)

/* Since the following is not defined in any of our header files. */
#define MIPS_PCI_XCFG_INDEX     0xbafe9100
#define MIPS_PCI_XCFG_DATA      0xbafe9104


#define BCM_SATA_VID		0x02421166

/* For now until the BSP defines them */
/* For SATA MMIO BAR5 */
#define CPU2PCI_PCI_PHYS_IO_WIN0_BASE 0x1af90000
#define CPU2PCI_PCI_PHYS_MEM_WIN0_BASE 0x1afa0000
#define BCM_LINUX_SATA_IRQ 5 /* bit 4 of INTC. */

#define PCI_DEVICE_ID_SATA 0 /* could be anything? */

#define PCI_SATA_PHYS_MEM_WIN5_BASE CPU2PCI_PCI_PHYS_MEM_WIN0_BASE

#define PCI_SATA_PHYS_MEM_WIN5_SIZE 0x2000


static int brcm_pci_read_config_dword( struct pci_dev *dev,
				int where, u32 *val)
{
    int slot_num, func_num;
    u32 cfc,cf8;

    /*
     *  For starters let's do configuration cycle 0 only (one bus only)
     */
    if (dev->bus->number)
	return PCIBIOS_FUNC_NOT_SUPPORTED;

    slot_num = PCI_SLOT(dev->devfn);
    func_num = PCI_FUNC(dev->devfn);
	cf8 = PCI_ENABLE + PCI_IDSEL(slot_num) + PCI_FNCSEL(func_num) + where;

	cf8 &= 0x7ff;
//printk("$$$$read addr %08x\n", cf8);
// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround
	*(volatile u32*)(MIPS_PCI_XCFG_INDEX) = cf8;
	cfc = *(volatile u32*)(MIPS_PCI_XCFG_DATA);
//printk("$$$$read data %08x\n", cfc);
//printk("&&&&&PCI intr stat %08x\n", *((volatile unsigned long *)0xbafe910c));
// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround
    *val = cfc;
    return PCIBIOS_SUCCESSFUL;
}

static int brcm_pci_write_config_dword(struct pci_dev *dev, int where,
					u32 val)
{
    int slot_num, func_num;
    u32 cfc;

    /*
     *  For starters let's do configuration cycle 0 only (one bus only)
     */
    if (dev->bus->number)
	return PCIBIOS_FUNC_NOT_SUPPORTED;

    slot_num = PCI_SLOT(dev->devfn);
    func_num = PCI_FUNC(dev->devfn);

	cfc = PCI_ENABLE + PCI_IDSEL(slot_num) + PCI_FNCSEL(func_num) + where;

	cfc &= 0x7ff;
// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround
//printk("$$$$write addr %08x with %08x\n", cfc, val);
	*(volatile u32*)(MIPS_PCI_XCFG_INDEX) = cfc;
	*(volatile u32*)(MIPS_PCI_XCFG_DATA) = val;

// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround
    return PCIBIOS_SUCCESSFUL;
}

static int brcm_pci_read_config_word(struct pci_dev *dev, int where, u16 *val)
{
    int status;
    u32 result;

    status = brcm_pci_read_config_dword(dev, where, &result);
    if (status != PCIBIOS_SUCCESSFUL)
	return status;
    if (where & 2)
	result >>= 16;
    *val = result & 0xffff;
    return PCIBIOS_SUCCESSFUL;
}

static int brcm_pci_read_config_byte(struct pci_dev *dev, int where, u8 *val)
{
    int status;
    u32 result;

    status = brcm_pci_read_config_dword(dev, where, &result);
    if (status != PCIBIOS_SUCCESSFUL)
	return status;
    if (where & 1)
	result >>= 8;
    if (where & 2)
	result >>= 16;
    *val = result & 0xff;
    return PCIBIOS_SUCCESSFUL;
}

static int brcm_pci_write_config_word(struct pci_dev *dev, int where, u16 val)
{
    int status, shift = 0;
    u32 result;
    status = brcm_pci_read_config_dword(dev, where, &result);
    if (status != PCIBIOS_SUCCESSFUL)
	return status;
    if (where & 2)
	shift += 16;
    result &= ~(0xffff << shift);
    result |= val << shift;
    return brcm_pci_write_config_dword(dev, where, result);
}

static int brcm_pci_write_config_byte( struct pci_dev *dev, int where, u8 val)
{
    int status, shift = 0;
    u32 result;

    status = brcm_pci_read_config_dword(dev, where, &result);
    if (status != PCIBIOS_SUCCESSFUL)
	return status;
    if (where & 2)
	shift += 16;
    if (where & 1)
	shift += 8;
    result &= ~(0xff << shift);
    result |= val << shift;
    return brcm_pci_write_config_dword(dev, where, result);
}

struct pci_ops brcm_pci_ops = {
    brcm_pci_read_config_byte,
    brcm_pci_read_config_word,
    brcm_pci_read_config_dword,
    brcm_pci_write_config_byte,
    brcm_pci_write_config_word,
    brcm_pci_write_config_dword
};





static void __init pcibios_enable_devices(void)
{
    struct pci_dev *dev;
    dev = pci_dev_g(pci_devices.next);{
    //pci_for_each_dev(dev) {
		/* <TDT> Our enable_device sets up bios data for the bootrom does not */
		brcm_pcibios_enable_device(dev);

		/* <TDT> force a rescan in case we still need bios data */
		pci_setup_device(dev);
    }
}

/*
 * Our current boot rom does not set these properly, so we change
 * them during setup.
 */
static void __init pcibios_fixup_irqs(void)
{
	struct pci_dev *dev;
	int slot_num;

    dev = pci_dev_g(pci_devices.next);{
//	pci_for_each_dev(dev) {
		slot_num = PCI_SLOT(dev->devfn);
		switch (slot_num) {
		case PCI_DEVICE_ID_SATA:	/* IDE controller */
			pcibios_update_irq(dev, BCM_LINUX_SATA_IRQ);
			dev->irq = BCM_LINUX_SATA_IRQ;
			break;

		default:
			printk("unknown slot num 0x%x for pcibios_fixup_irqs\n", slot_num);
		break;
		}
	}
}

void __init pcibios_init(void)
{
	printk("PCI: Probing PCI hardware\n");
	pci_scan_bus(0, &brcm_pci_ops, NULL);
	pcibios_enable_devices();
	pcibios_fixup_irqs();
}


static char *brcm_pcibios_setup (char *str)
{
    return str;
}

void __init pcibios_update_irq(struct pci_dev *dev, int irq)
{
    u8 val;

	pci_write_config_byte(dev, PCI_INTERRUPT_LINE, irq);

	/* Test this as it does not seem to be working */
	pci_read_config_byte(dev, PCI_INTERRUPT_LINE, &val);
}


int pcibios_enable_resources(struct pci_dev *dev)
{
	u16 cmd, old_cmd;
	int idx;
	struct resource *r;
	printk("PCI: Enabling device %x\n", dev->device);

	pci_read_config_word(dev, PCI_COMMAND, &cmd);
	old_cmd = cmd;
	for(idx=0; idx<6; idx++) {
		r = &dev->resource[idx];
		if (!r->start && r->end) {
			printk(KERN_ERR "PCI: Device %s not available because of resource collisions\n", dev->slot_name);
			printk("r->start=%lx, r->end=%lx\n",r->start,r->end);
			return -EINVAL;
		}
		if (r->flags & IORESOURCE_IO)
			cmd |= PCI_COMMAND_IO;
		if (r->flags & IORESOURCE_MEM)
			cmd |= PCI_COMMAND_MEMORY;
	}
	if (cmd != old_cmd) {
		printk("PCI: Enabling device %s (%04x -> %04x)\n", dev->slot_name, old_cmd, cmd);
		pci_write_config_word(dev, PCI_COMMAND, cmd);
	}
	return 0;
}

#if 0
// global to allow multiple slots
static u32 memAddr = PCI_EXPANSION_PHYS_MEM_WIN0_BASE;
static u32 ioAddr = PCI_EXPANSION_PHYS_IO_WIN0_BASE;
#endif

// Remembers which devices have already been assigned.
static  int slots[16];

/* Do IO/memory allocation for expansion slot here, merged from 2.4.25 implementation */
static __init void brcm_pcibios_fixup_expansion_slot(struct pci_dev *dev) 
{
#if 0
	int i,j,k,l, iosize, msize;
	//int useIOAddr = 0;
	u32 savRegData;
	u32 size;
	u32 mask;
	int slot;
	u32 regData;
		

	/*
	* Make sure that it is in the PCI slot, otherwise we punt.
	*/


	slot = PCI_SLOT(dev->devfn);

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

//	pcibios_update_irq(dev, BCM_LINUX_EXT_PCI_IRQ);
//	dev->irq = BCM_LINUX_EXT_PCI_IRQ;


	printk("\tPCI DEV in PCI slot, ID=%04x:%04x\n\r", dev->vendor, dev->device);


	/* Write FFFFFFFFH to BAR registers to probe for IO or Mem */
	iosize = msize = 0;
	for (i=0,j=0; i<6; i++,j+=4) {
		pci_write_config_dword(dev,PCI_BASE_ADDRESS_0+j,0xffffffff);
		pci_read_config_dword(dev,PCI_BASE_ADDRESS_0+j,&regData);
		if (regData) {
			printk("Expansion: PCI_BAR[%d] = %x\n", i, regData);


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
				printk("Expansion: size requested is %x\n", 1<<k);
				size = 1<<k;
				mask = 0xffffffff;
				for (l=0; l<k; l++) {
					mask &= ~(1<<l);
				}
			}
			else
				break;
			/* Only the first BAR counts on most devices */
			//if (i == 0) {
			//	useIOAddr = savRegData & 0x1;
			//}

			if (savRegData & 0x1) {	/* IO address */
				/* Calculate the next address that satisfies the boundary condition */
				ioAddr = (ioAddr + size - 1) & mask;
				pci_write_config_dword(dev, PCI_BASE_ADDRESS_0+j, ioAddr);
				printk("Expansion: Writing PCI_IO_BAR[%d]=%x, size=%d, mask=%x\n", i,
					ioAddr, 1<<k, mask);
				iosize += size;
			}
			else { /* Mem address, tag on to the last one, the SATA mem area */
				/* Calculate the next address that satisfies the alignment condition */
				memAddr = (memAddr + size - 1) & ~(size -1);
				pci_write_config_dword(dev, PCI_BASE_ADDRESS_0+j, memAddr);
				printk("Expansion: Writing PCI_MEM_BAR[%d]=%x, size=%d, mask=%x\n", i,
					memAddr, 1<<k, mask);
				msize = memAddr + size;
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

	printk("\tExpansion: PCI_DEV %04x:%04x, command=%x\n", 
		dev->vendor, dev->device, regData);
	pci_write_config_dword(dev,PCI_COMMAND,regData);
#endif

	
}


int brcm_pcibios_enable_device(struct pci_dev *dev)
{
	u32 dev_vendor_id;
	u32 regData, size;
	unsigned long temp;


	/********************************************************
	 * Configure the Device
	 ********************************************************/
	dev_vendor_id = (u32)dev->device<<16;
	dev_vendor_id |= dev->vendor;
	switch (dev_vendor_id) {
	/********************************************************
	 * SATA Controller
	 ********************************************************/
	case BCM_SATA_VID:
		printk("Setting up SATA controller\n");
		regData = 0x200;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, regData);
		dev->resource[0].start = regData;
		dev->resource[0].end = regData + 0x3f;
		dev->resource[0].flags = IORESOURCE_IO;
		regData = 0x240;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_1, regData);
		dev->resource[1].start = regData;
		dev->resource[1].end = regData + 0x3f;
		dev->resource[1].flags = IORESOURCE_IO;
		regData = 0x280;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_2, regData);
		dev->resource[2].start = regData;
		dev->resource[2].end = regData + 0x3f;
		dev->resource[2].flags = IORESOURCE_IO;
		regData = 0x2c0;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_3, regData);
		dev->resource[3].start = regData;
		dev->resource[3].end = regData + 0x3f;
		dev->resource[3].flags = IORESOURCE_IO;
		regData = 0x300;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_4, regData);
		dev->resource[4].start = regData;
		dev->resource[4].end = regData + 0x3f;
		dev->resource[4].flags = IORESOURCE_IO;
		/* 2.4.18sr7p2: Turn on BAR 5 in order to set PLL */
 		size = PCI_SATA_PHYS_MEM_WIN5_SIZE;
		regData = (PCI_SATA_PHYS_MEM_WIN5_BASE + size -1) & ~(size-1);
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_5, regData);
		dev->resource[5].start = regData;
		dev->resource[5].end = regData +size-1;
		dev->resource[5].flags = IORESOURCE_MEM;

pci_read_config_dword(dev,0xc, &regData);
printk("SATA config 0xc orig : %08x\n", regData);

		regData = 0xff;
		pci_write_config_byte(dev, 0xd, regData); /* SATA_CFG_MASTER_LATENCY_TIMER. */

pci_read_config_dword(dev,0xc, &regData);
printk("SATA config 0xc after : %08x\n", regData);

		/*  2.4.18sr7p2 Turn on MEM */
		pci_read_config_dword(dev, PCI_COMMAND, &regData);
printk("SATA command reg orig %08x\n", regData);
		regData |= (PCI_COMMAND_MASTER|PCI_COMMAND_IO| PCI_COMMAND_MEMORY);
		pci_write_config_dword(dev, PCI_COMMAND, regData);
printk("SATA command reg %08x\n", regData);

#define DONT_USE_SATA_DMA
#ifndef DONT_USE_SATA_DMA
		pci_read_config_dword(dev, PCI_BASE_ADDRESS_4, &regData);
		regData &= 0xfffffc; // Mask off  Reserved & Res Type bits.
		printk("SATA: Bus Master Status Register offset = %08lx + %08x = %08x\n",
			mips_io_port_base, regData, mips_io_port_base+regData);
// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround
		temp = *((volatile unsigned char *)(mips_io_port_base+regData));
		printk("SATA: before init Bus Master Status reg = 0x%08x.\n", temp);
// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround
		regData += 2; // Offset 302H
		temp = *((volatile unsigned char *)(mips_io_port_base+regData));
// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround
		temp |= 0x20;
		*((volatile unsigned char *)(mips_io_port_base+regData)) = temp;
// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround
		printk("SATA: after init Bus Master Status reg = 0x%08x.\n", *((volatile unsigned char *)(mips_io_port_base+regData)));
// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround
#endif

#ifndef CONFIG_MIPS_DP922
		/* 2.4.18sr7p2 */
		// Force PLLLOCK
		temp = *((volatile unsigned long *)(PCI_SATA_PHYS_MEM_WIN5_BASE+0xa0000000ul+0x84));
		temp |= 0x08000000;
// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround
		*((volatile unsigned long *)(PCI_SATA_PHYS_MEM_WIN5_BASE+0xa0000000ul+0x84)) = temp;
// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround

		printk("SATA: after PLLLOCK reg = 0x%08lx.\n", *((volatile unsigned long *)(PCI_SATA_PHYS_MEM_WIN5_BASE+0xa0000000ul+0x84)));
// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround
#endif

		// Turn off SATA 2
		temp = *((volatile unsigned long *)(PCI_SATA_PHYS_MEM_WIN5_BASE+0xa0000000ul+0x148));
		temp |= 0x00000001;
// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround
		*((volatile unsigned long *)(PCI_SATA_PHYS_MEM_WIN5_BASE+0xa0000000ul+0x148)) = temp;
// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround
		printk("SATA: after PLLLOCK reg = 0x%08lx.\n", *((volatile unsigned long *)(PCI_SATA_PHYS_MEM_WIN5_BASE+0xa0000000ul+0x148)));
// PCI workaround
*((volatile unsigned long *)MIPS_PCI_XCFG_INDEX) = 0x2c;
*((volatile unsigned long *)MIPS_PCI_XCFG_DATA) = 0x00;
// PCI workaround


#if 0
{ int i;
  printk("Print our resource map \n");
  for (i=0; i<6; i++) {
    printk("Bar %d: start=%04x, end=%04x\n", i, dev->resource[i].start, dev->resource[i].end);
  }
}
#endif


		break;


	default:
		printk("unknown dev_man id 0x%x for pcibios_enable_device\n", dev_vendor_id);
		brcm_pcibios_fixup_expansion_slot(dev);
		break;
	}
#ifdef DUMP_PCI_CONFIG
	{
	int i;
	printk("\tPCI Config Space IDSEL%d"),PCI_SLOT(dev->devfn));
	for (i=0;i<0x40;i++) {
		if ((i & 3) == 0) printk("\r\n\t\t");
		pci_read_config_byte(dev,i,(unsigned char *)&regData);
		printk("[%.2x]=%.2x  "),i,regData & 0xFF);
	}
	printk("\n\r");
	}
#endif
	return 0;
}

static void brcm_pcibios_update_resource(struct pci_dev *dev, struct resource *root,
			     struct resource *res, int resource)
{
#if 0
	u32 new, check;
	int reg;

	new = res->start | (res->flags & PCI_REGION_FLAG_MASK);
	if (resource < 6) {
		reg = PCI_BASE_ADDRESS_0 + 4*resource;
	} else if (resource == PCI_ROM_RESOURCE) {
		res->flags |= PCI_ROM_ADDRESS_ENABLE;
		reg = dev->rom_base_reg;
	} else {
		/* Somebody might have asked allocation of a non-standard resource */
		return;
	}
	
	pci_write_config_dword(dev, reg, new);
	pci_read_config_dword(dev, reg, &check);
	if ((new ^ check) & ((new & PCI_BASE_ADDRESS_SPACE_IO) ? PCI_BASE_ADDRESS_IO_MASK : PCI_BASE_ADDRESS_MEM_MASK)) {
		printk(KERN_ERR "PCI: Error while updating region "
		       "%s/%d (%08x != %08x)\n", dev->slot_name, resource,
		       new, check);
	}
#endif
}

void __init pcibios_fixup_bus(struct pci_bus *bus)
{
}

static void brcm_pcibios_align_resource(void *data, struct resource *res,
			    unsigned long size)
{
#if 0
	struct pci_dev *dev = data;

	if (res->flags & IORESOURCE_IO) {
		unsigned long start = res->start;

		/* We need to avoid collisions with `mirrored' VGA ports
		   and other strange ISA hardware, so we always want the
		   addresses kilobyte aligned.  */
		if (size > 0x100) {
			printk(KERN_ERR "PCI: I/O Region %s/%d too large"
			       " (%ld bytes)\n", dev->slot_name,
			       dev->resource - res, size);
		}

		start = (start + 1024 - 1) & ~(1024 - 1);
		res->start = start;
	}
#endif
}

unsigned __init int pcibios_assign_all_busses(void)
{
	return 1;
}

struct pci_fixup pcibios_fixups[] = {};

void dump_ide(void)
{}
EXPORT_SYMBOL(dump_ide);
