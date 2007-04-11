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

#include <asm/brcmstb/brcm97038b0/boardmap.h>
#include <asm/brcmstb/brcm97038b0/bchp_hif_cpu_intr1.h>
#include <asm/brcmstb/brcm97038b0/bcmintrnum.h>

#include <asm/io.h>


/* from PCI spec, Maybe we can put this in some include file. */
#define PCI_ENABLE              0x80000000
#define PCI_IDSEL(x)		(((x)&0x1f)<<11)
#define PCI_FNCSEL(x)		(((x)&0x7)<<8)

/* Since the following is not defined in any of our header files. */
#define MIPS_PCI_XCFG_INDEX     0xf0600004
#define MIPS_PCI_XCFG_DATA      0xf0600008


#define BCM7038_SATA_VID		0x02421166
#define BCM3250_VID				0x325014e4
#define BCM7041_VID				0xa000141f

//#define BCM7041_MEM_BASE    0xd2000000
//#define BCM3250_REG_BASE    0xd0000000
//#define BCM3250_MEM_BASE    0xd1000000

#define PCI_SATA_PHYS_MEM_WIN5_BASE \
	(PCI_7041_PHYS_MEM_WIN0_BASE+0xc0000)

//#define PCI_DEVICE_ID_SATA      0x0e
//#define PCI_DEVICE_ID_EXT       0x0d
//#define PCI_DEVICE_ID_7041      0x04
//#define PCI_DEVICE_ID_3250      0x01


static int brcm_pcibios_enable_device(struct pci_dev *dev);


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

	*(volatile u32*)(MIPS_PCI_XCFG_INDEX) = cf8;
	cfc = *(volatile u32*)(MIPS_PCI_XCFG_DATA);
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
	*(volatile u32*)(MIPS_PCI_XCFG_INDEX) = cfc;
	*(volatile u32*)(MIPS_PCI_XCFG_DATA) = val;

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


#if 0
/*  This stuff is messed up, we will change it when I understand what it is. TDT */
struct {
    struct resource ram;
    struct resource flash;
    struct resource pci_io;
    struct resource pci_mem;
    struct resource bcm7020;
    struct resource boot;
} bcm93725_resources = {
    { "RAM", 0x00000000, 0x03ffffff,
      IORESOURCE_MEM | PCI_BASE_ADDRESS_MEM_TYPE_64 },
    { "Flash ROM", 0x1e000000, 0x1f000000 },
    { "PCI I/O", 0x06100000, 0x07ffffff },
    { "PCI mem", 0x09000000, 0x0fffffff, IORESOURCE_MEM },
    { "7020 ctrl", 0x1fa00000, 0x1fbfffff,
      IORESOURCE_MEM | PCI_BASE_ADDRESS_MEM_TYPE_64 },
    { "Boot ROM", 0x1fc00000, 0x1fffffff }
};
#endif



/*
 * We enable all devices during setup as the ALi must be setup in order
 * to access devices which use it's PIC.
 */
static void __init pcibios_enable_devices(void)
{
    struct pci_dev *dev;
    pci_for_each_dev(dev) {
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

	pci_for_each_dev(dev) {
		slot_num = PCI_SLOT(dev->devfn);
		switch (slot_num) {
		case PCI_DEVICE_ID_3250:		/* BCM3250  */
			pcibios_update_irq(dev, BCM_LINUX_3250_IRQ);
			dev->irq = BCM_LINUX_3250_IRQ;
			break;
		case PCI_DEVICE_ID_SATA:	/* IDE controller */
			pcibios_update_irq(dev, BCM_LINUX_SATA_IRQ);
			dev->irq = BCM_LINUX_SATA_IRQ;
			break;
		case PCI_DEVICE_ID_EXT:	/* On-board PCI slot */
			pcibios_update_irq(dev, BCM_LINUX_EXT_PCI_IRQ);
			dev->irq = BCM_LINUX_EXT_PCI_IRQ;
			break;
		case PCI_DEVICE_ID_7041:
			/* do not need to fixup. */
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

static int brcm_pcibios_enable_device(struct pci_dev *dev)
{
	u32 dev_vendor_id;
	u32 regData;

	/********************************************************
	 * Configure the Device
	 ********************************************************/
	dev_vendor_id = (u32)dev->device<<16;
	dev_vendor_id |= dev->vendor;
	switch (dev_vendor_id) {
	/********************************************************
	 * SATA Controller
	 ********************************************************/
	case BCM7038_SATA_VID:
		printk("Setting up SATA controller\n");
		
		regData = 0x201;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, regData);
		regData = 0x241;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_1, regData);
		regData = 0x281;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_2, regData);
		regData = 0x2c1;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_3, regData);
		regData = 0x301;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_4, regData);
		
		/* On 2.4.25, we have to assign some mem region for MMIO even though we
		 * have no intention of using it
		 */
		regData = PCI_SATA_PHYS_MEM_WIN5_BASE; //0x02000000;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_5, regData);
	
		
		regData = 0xff;
		pci_write_config_byte(dev, 0xd, regData); /* SATA_CFG_MASTER_LATENCY_TIMER. */

		pci_read_config_dword(dev, PCI_COMMAND, &regData);
		regData |= (PCI_COMMAND_MASTER|PCI_COMMAND_IO);
		pci_write_config_dword(dev, PCI_COMMAND, regData);

#if 0

#endif

//#define DONT_USE_SATA_DMA
#ifndef DONT_USE_SATA_DMA

		pci_read_config_dword(dev, PCI_BASE_ADDRESS_4, &regData);
		regData &= 0xfffffc; // Mask off  Reserved & Res Type bits.
		printk("SATA: Bus Master Status Register offset = %08lx + %08x = %08x\n", 
			mips_io_port_base, regData, mips_io_port_base+regData);
		printk("SATA: before init Bus Master Status reg = 0x%08x.\n", *((volatile unsigned char *)(mips_io_port_base+regData)));
		regData += 2; // Offset 302H
		*((volatile unsigned char *)(mips_io_port_base+regData)) |= 0x20;
		printk("SATA: after init Bus Master Status reg = 0x%08x.\n", *((volatile unsigned char *)(mips_io_port_base+regData)));
#endif

		break;

	/********************************************************
	 * Config the BCM3250 chip
	 ********************************************************/
	case BCM3250_VID:
		printk("\tBCM3250\n\r");
		regData = BCM3250_REG_BASE;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, regData);
		regData = BCM3250_MEM_BASE;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_1, regData);

		pci_read_config_dword(dev, PCI_COMMAND, &regData);
		regData |= PCI_COMMAND_MEMORY;
		pci_write_config_dword(dev, PCI_COMMAND, regData);

		break;

	/********************************************************
	 * Config the BCM7041 chip
	 ********************************************************/
	case BCM7041_VID:
		printk("\tBCM7041 Dual Channel MPEG2 Encoder\n\r");
		regData = BCM7041_MEM_BASE;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, regData);
		regData += 0x20000;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_1, regData);
		regData += 0x20000;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_2, regData);
		regData += 0x20000;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_3, regData);
		regData += 0x20000;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_4, regData);
		regData += 0x20000;
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_5, regData);
 
		pci_read_config_dword(dev, PCI_COMMAND, &regData);
		regData |= PCI_COMMAND_MEMORY;
		pci_write_config_dword(dev, PCI_COMMAND, regData);
		break;

	default:
		printk("unknown dev_man id 0x%x for pcibios_enable_device\n", dev_vendor_id);
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
