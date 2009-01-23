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

    File: ops-brcmstb.c

    Description: 
    PCI configuration read/write functions for STB platforms

 ------------------------------------------------------------------------- */

#include <linux/init.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/ioport.h>

#include <asm/delay.h>
#include <asm/io.h>
#include <asm/debug.h>
#include <asm/brcmstb/common/brcmstb.h>

#ifdef BCHP_PCIX_BRIDGE_SATA_CFG_INDEX
/* 7400 and newer, with SATA on internal PCI-X bridge */
#define MIPS_PCI_SATA_XCFG_INDEX \
	(0xb0000000 | BCHP_PCIX_BRIDGE_SATA_CFG_INDEX)
#define MIPS_PCI_SATA_XCFG_DATA \
	(0xb0000000 | BCHP_PCIX_BRIDGE_SATA_CFG_DATA)
#else
/* 7038, 7401, other pre-7400 platforms */
#define MIPS_PCI_SATA_XCFG_INDEX	0xb0500204
#define MIPS_PCI_SATA_XCFG_DATA		0xb0500208
#endif

#define CFG_INDEX(bus, devfn, reg) \
	((pci_io_map_base(bus) != PCIE_IO_START) ? \
		((0x80000000 | ((PCI_SLOT(devfn) & 0x1f) << 11) | \
			((PCI_FUNC(devfn) & 0x7) << 8) | (reg))) : \
		((((1 << 20) | (PCI_SLOT(devfn) & 0x1f) << 15) | \
			((PCI_FUNC(devfn) & 0x7) << 12) | (reg))))

static void get_pci_cfg_regs(struct pci_bus *bus, unsigned long *idx_reg,
	unsigned long *data_reg)
{
	switch(pci_io_map_base(bus)) {
		case PCI_IO_START:
			*idx_reg = MIPS_PCI_XCFG_INDEX;
			*data_reg = MIPS_PCI_XCFG_DATA;
			break;
		case PCI_SATA_IO_START:
			*idx_reg = MIPS_PCI_SATA_XCFG_INDEX;
			*data_reg = MIPS_PCI_SATA_XCFG_DATA;
			break;
		case PCIE_IO_START:
			*idx_reg = MIPS_PCIE_XCFG_INDEX;
			*data_reg = MIPS_PCIE_XCFG_DATA;
			break;
		default:
			BUG();
	}
}

static int devfn_ok(struct pci_bus *bus, unsigned int devfn)
{
	/* SATA is the only device on the bus, with devfn == 0 */
	if((pci_io_map_base(bus) == PCI_SATA_IO_START) && (devfn != 0))
		return(0);

#ifdef BRCM_PCIE_SUPPORTED
	if(pci_io_map_base(bus) == PCIE_IO_START) {
		/* slots 1-31 don't exist */
		if(PCI_SLOT(devfn) != 0)
			return(0);

		/* link up? */
		if((BDEV_RD(BCHP_PCIE_MISC_PCIE_STATUS) & 0x30) != 0x30)
			return(0);
	}
#endif
	return(1);	/* OK */
}

static int brcm_pci_write_config(struct pci_bus *bus, unsigned int devfn,
	int where, int size, u32 data)
{
	u32 val = 0, mask, shift;
	unsigned long idx_reg, data_reg;

	if(! devfn_ok(bus, devfn))
		return(PCIBIOS_FUNC_NOT_SUPPORTED);

	get_pci_cfg_regs(bus, &idx_reg, &data_reg);

	BUG_ON(((where & 3) + size) > 4);

	if(size < 4) {
		/* partial word - read, modify, write */
		DEV_WR(idx_reg, CFG_INDEX(bus, devfn, where & ~3));
		DEV_RD(idx_reg);
		val = DEV_RD(data_reg);
	}

	shift = (where & 3) << 3;
	mask = (0xffffffff >> ((4 - size) << 3)) << shift;

	DEV_WR(idx_reg, CFG_INDEX(bus, devfn, where & ~3));
	val = (val & ~mask) | ((data << shift) & mask);
	DEV_WR(data_reg, val);
	DEV_RD(data_reg);

	return(PCIBIOS_SUCCESSFUL);
}

static int brcm_pci_read_config(struct pci_bus *bus, unsigned int devfn,
	int where, int size, u32 *data)
{
	u32 val, mask, shift;
	unsigned long idx_reg, data_reg;

	if(! devfn_ok(bus, devfn))
		return(PCIBIOS_FUNC_NOT_SUPPORTED);

	get_pci_cfg_regs(bus, &idx_reg, &data_reg);

	BUG_ON(((where & 3) + size) > 4);

	DEV_WR(idx_reg, CFG_INDEX(bus, devfn, where & ~3));
	DEV_RD(idx_reg);
	val = DEV_RD(data_reg);

	shift = (where & 3) << 3;
	mask = (0xffffffff >> ((4 - size) << 3)) << shift;

	*data = (val & mask) >> shift;
	return(PCIBIOS_SUCCESSFUL);
}

struct pci_ops brcmstb_pci_ops = {
	.read = brcm_pci_read_config,
	.write = brcm_pci_write_config,
};
