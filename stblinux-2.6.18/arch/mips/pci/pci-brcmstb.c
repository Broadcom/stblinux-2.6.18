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

    File: pci-brcmstb.c

    Description: 
    PCI controller registration for STB platforms

 ------------------------------------------------------------------------- */

#include <linux/init.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <asm/brcmstb/common/brcmstb.h>

/* external PCI bus */
static struct resource pci_io_resource = {
	.name = "ext pci IO space",
	.start = 0x400,
	.end = PCI_IO_SIZE - 1,
	.flags = IORESOURCE_IO,
};

static struct resource pci_mem_resource = {
	.name = "ext pci memory space",
	.start = PCI_MEM_START,
	.end = PCI_MEM_END,
	.flags = IORESOURCE_MEM,
};

/* internal PCI bus for SATA core */
static struct resource pci_sata_io_resource = {
	.name = "sata pci IO space",
	.start = 0x200,
	.end = 0x3ff,
	.flags = IORESOURCE_IO,
};

static struct resource pci_sata_mem_resource = {
	.name = "sata pci memory space",
	.start = PCI_SATA_MEM_START,
	.end = PCI_SATA_MEM_END,
	.flags = IORESOURCE_MEM,
};

/* external PCIe bus */
static struct resource pcie_mem_resource = {
	.name = "ext pcie memory space",
	.start = PCIE_MEM_START,
	.end = PCIE_MEM_END,
	.flags = IORESOURCE_MEM,
};

static struct resource pcie_dummy_io_resource = {
	.name = "ext DUMMY pcie IO space",
	.start = PCI_IO_SIZE,
	.end = PCI_IO_SIZE + 0x400,
	.flags = IORESOURCE_IO,
};


extern struct pci_ops brcmstb_pci_ops; 

struct pci_controller brcmstb_pci_controller = {
	.pci_ops 	= &brcmstb_pci_ops, 
	.io_resource 	= &pci_io_resource, 
	.mem_resource   = &pci_mem_resource,
	.io_map_base	= PCI_IO_START,
};

struct pci_controller brcmstb_sata_controller = {
	.pci_ops 	= &brcmstb_pci_ops, 
	.io_resource 	= &pci_sata_io_resource, 
	.mem_resource   = &pci_sata_mem_resource,
	.io_map_base	= PCI_SATA_IO_START,
};

struct pci_controller brcmstb_pcie_controller = {
	.pci_ops 	= &brcmstb_pci_ops, 
	/* no IO space supported for PCIe devices */
	.io_resource	= &pcie_dummy_io_resource,
	.mem_resource	= &pcie_mem_resource,
	.io_map_base	= PCIE_IO_START,
};

#define PCI_SATA_MEM_ENABLE			1
#define PCI_SATA_BUS_MASTER_ENABLE		2
#define PCI_SATA_PERR_ENABLE			0x10
#define PCI_SATA_SERR_ENABLE			0x20

#define PCI_BUS_MASTER  BCHP_PCI_CFG_STATUS_COMMAND_BUS_MASTER_MASK
#define PCI_IO_ENABLE   BCHP_PCI_CFG_STATUS_COMMAND_MEMORY_SPACE_MASK
#define PCI_MEM_ENABLE  BCHP_PCI_CFG_STATUS_COMMAND_IO_SPACE_MASK

#if defined(CONFIG_CPU_BIG_ENDIAN)
#define	CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN	2
#else
#define	CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN	0
#endif


static void brcm_setup_sata_bridge(void)
{
#if defined(BCHP_PCI_BRIDGE_PCI_CTRL) && defined(BRCM_SATA_SUPPORTED)

	/* Internal PCI SATA bridge setup for 7038, 7401, 7403, 7118, etc. */

	BDEV_SET(BCHP_PCI_BRIDGE_PCI_CTRL,
		(PCI_SATA_MEM_ENABLE|PCI_SATA_BUS_MASTER_ENABLE|
		 PCI_SATA_PERR_ENABLE|PCI_SATA_SERR_ENABLE));

	/* PCI slave window (SATA access to MIPS memory) */
	BDEV_WR(BCHP_PCI_BRIDGE_PCI_SLV_MEM_WIN_BASE,
		0 | CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);

	/* PCI master window (MIPS access to SATA BARs) */
	BDEV_WR(BCHP_PCI_BRIDGE_CPU_TO_SATA_MEM_WIN_BASE,
		PCI_SATA_MEM_START |
		CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);

	BDEV_WR(BCHP_PCI_BRIDGE_CPU_TO_SATA_IO_WIN_BASE,
		CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);

	/* do a PCI config read */
	BDEV_WR(BCHP_PCI_BRIDGE_SATA_CFG_INDEX, PCI_DEV_NUM_SATA);
	if(BDEV_RD(BCHP_PCI_BRIDGE_SATA_CFG_DATA) == 0xffffffff)
		printk(KERN_WARNING "Internal SATA is not responding\n");

#elif defined(BCHP_PCIX_BRIDGE_PCIX_CTRL) && defined(BRCM_SATA_SUPPORTED)

	/* Internal PCI-X SATA bridge setup for 7400, 7405, 7335 */

	BDEV_WR(BCHP_PCIX_BRIDGE_PCIX_CTRL,
		(PCI_SATA_MEM_ENABLE|PCI_SATA_BUS_MASTER_ENABLE|
		 PCI_SATA_PERR_ENABLE|PCI_SATA_SERR_ENABLE));
	BDEV_WR(BCHP_PCIX_BRIDGE_PCIX_SLV_MEM_WIN_BASE, 1);

	/* PCI slave window (SATA access to MIPS memory) */
	BDEV_WR(BCHP_PCIX_BRIDGE_PCIX_SLV_MEM_WIN_MODE,
		CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);

	/* PCI master window (MIPS access to SATA BARs) */
	BDEV_WR(BCHP_PCIX_BRIDGE_CPU_TO_SATA_MEM_WIN_BASE,
		PCI_SATA_MEM_START |
		CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);
	BDEV_WR(BCHP_PCIX_BRIDGE_CPU_TO_SATA_IO_WIN_BASE,
		CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);

	/* do a PCI config read */
	BDEV_WR(BCHP_PCIX_BRIDGE_SATA_CFG_INDEX, 0x80000000|PCI_DEV_NUM_SATA);
	if(BDEV_RD(BCHP_PCIX_BRIDGE_SATA_CFG_DATA) == 0xffffffff)
		printk(KERN_WARNING "Internal SATA is not responding\n");
#endif
}

static void brcm_setup_pci_bridge(void)
{
#if defined(BRCM_PCI_SUPPORTED)

	/* External PCI bridge setup (most chips) */

	BDEV_SET(BCHP_PCI_CFG_STATUS_COMMAND,
		PCI_BUS_MASTER|PCI_IO_ENABLE|PCI_MEM_ENABLE);

	BDEV_WR(BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN0, PCI_MEM_START);
	BDEV_WR(BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN1, PCI_MEM_START + 0x08000000);
	BDEV_WR(BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2, PCI_MEM_START + 0x10000000);
	BDEV_WR(BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3, PCI_MEM_START + 0x18000000);

	BDEV_WR(BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0,
		0x00000000 | CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);
	BDEV_WR(BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1,
		0x00200000 | CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);
	BDEV_WR(BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2,
		0x00400000 | CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);

	BDEV_WR(BCHP_PCI_CFG_MEMORY_BASE_W0, 0xfffffff0);
	printk(KERN_INFO "PCI->SDRAM window size: %luMB\n",
		(~(BDEV_RD(BCHP_PCI_CFG_MEMORY_BASE_W0) & ~0xfUL) + 1UL) >> 20);

	BDEV_WR(BCHP_PCI_CFG_MEMORY_BASE_W0, 0x00000000);

	/* not used - move them out of the way */
	BDEV_WR(BCHP_PCI_CFG_MEMORY_BASE_W1, 0x80000000);
	BDEV_WR(BCHP_PCI_CFG_MEMORY_BASE_W2, 0x80000000);
	BDEV_WR(BCHP_PCI_CFG_GISB_BASE_W, 0x80000000);

	/* set endianness for W0 */
	BDEV_UNSET(BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL,
		BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_MASK);
	BDEV_SET(BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL,
		CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);

	/* do a PCI config read */
	DEV_WR(MIPS_PCI_XCFG_INDEX, PCI_DEV_NUM_EXT);
	DEV_RD(MIPS_PCI_XCFG_DATA);
#endif
}

static void brcm_setup_pcie_bridge(void)
{
#if defined(BRCM_PCIE_SUPPORTED)
	int i;

	/* reset the bridge and the endpoint device */
	BDEV_SET(BCHP_HIF_RGR1_SW_RESET_1,
		BCHP_HIF_RGR1_SW_RESET_1_PCIE_BRIDGE_SW_RESET_MASK |
		BCHP_HIF_RGR1_SW_RESET_1_PCIE_SW_PERST_MASK);
	BDEV_RD(BCHP_HIF_RGR1_SW_RESET_1);
	udelay(100);

	/* take the bridge out of reset */
	BDEV_UNSET(BCHP_HIF_RGR1_SW_RESET_1,
		BCHP_HIF_RGR1_SW_RESET_1_PCIE_BRIDGE_SW_RESET_MASK);
	BDEV_RD(BCHP_HIF_RGR1_SW_RESET_1);

	/* enable CSR_READ_UR_MODE and SCB_ACCESS_EN */
	BDEV_WR(BCHP_PCIE_MISC_MISC_CTRL, 0x3000);

	/* set up MIPS->PCIE memory windows (4x 128MB) */
	BDEV_WR(BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN0_LO,
		PCIE_MEM_START + 0x00000000);
	BDEV_WR(BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN0_HI, 0);

	BDEV_WR(BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN1_LO,
		PCIE_MEM_START + 0x08000000);
	BDEV_WR(BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN1_HI, 0);

	BDEV_WR(BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN2_LO,
		PCIE_MEM_START + 0x10000000);
	BDEV_WR(BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN2_HI, 0);

	BDEV_WR(BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN3_LO,
		PCIE_MEM_START + 0x18000000);
	BDEV_WR(BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN3_HI, 0);

	/* set up 1GB PCIE->SCB memory window on BAR2 */
	BDEV_WR(BCHP_PCIE_MISC_RC_BAR2_CONFIG_LO, 0x0000000f);
	BDEV_WR(BCHP_PCIE_MISC_RC_BAR2_CONFIG_HI, 0x00000000);

	/* disable PCIE->GISB window */
	BDEV_WR(BCHP_PCIE_MISC_RC_BAR1_CONFIG_LO, 0x00000000);
	/* disable the other PCIE->SCB memory window */
	BDEV_WR(BCHP_PCIE_MISC_RC_BAR3_CONFIG_LO, 0x00000000);

	/* disable MSI (for now...) */
	BDEV_WR(BCHP_PCIE_MISC_MSI_BAR_CONFIG_LO, 0);

	/* set up L2 interrupt masks */
	BDEV_WR(BCHP_PCIE_INTR2_CPU_CLEAR, 0);
	BDEV_RD(BCHP_PCIE_INTR2_CPU_CLEAR);
	BDEV_WR(BCHP_PCIE_INTR2_CPU_MASK_CLEAR, 0);
	BDEV_RD(BCHP_PCIE_INTR2_CPU_MASK_CLEAR);
	BDEV_WR(BCHP_PCIE_INTR2_CPU_MASK_SET, 0xffffffff);
	BDEV_RD(BCHP_PCIE_INTR2_CPU_MASK_SET);

	/* take the EP device out of reset */
	BDEV_UNSET(BCHP_HIF_RGR1_SW_RESET_1,
		BCHP_HIF_RGR1_SW_RESET_1_PCIE_SW_PERST_MASK);
	BDEV_RD(BCHP_HIF_RGR1_SW_RESET_1);

	/* give the RC/EP time to wake up, before trying to configure RC */
	for(i = 50; i > 0; i--) {
		if((BDEV_RD(BCHP_PCIE_MISC_PCIE_STATUS) & 0x30) == 0x30)
			break;
		mdelay(1);
	}
	if(i == 0) {
		printk(KERN_INFO "PCI: PCIe link down\n");
		return;
	}
	printk(KERN_INFO "PCI: PCIe link up\n");

	/* enable MEM_SPACE and BUS_MASTER for RC */
	BDEV_WR(BCHP_PCIE_RC_CFG_TYPE1_STATUS_COMMAND, 0x6);

	/* set base/limit for outbound transactions */
	BDEV_WR(BCHP_PCIE_RC_CFG_TYPE1_RC_MEM_BASE_LIMIT, 0xbff0a000);
	/* disable the prefetch range */
	BDEV_WR(BCHP_PCIE_RC_CFG_TYPE1_RC_PREF_BASE_LIMIT, 0x0000fff0);

	/* set pri/sec bus numbers */
	BDEV_WR(BCHP_PCIE_RC_CFG_TYPE1_PRI_SEC_BUS_NO, 0x00010100);

	/* PCIE->SCB endian mode for BAR2 */
	BDEV_WR_F(PCIE_RC_CFG_VENDOR_VENDOR_SPECIFIC_REG1, ENDIAN_MODE_BAR2,
		CPU2PCI_CPU_PHYS_MEM_WIN_BYTE_ALIGN);
	BDEV_RD(BCHP_PCIE_RC_CFG_VENDOR_VENDOR_SPECIFIC_REG1);
#endif
}

static int __init brcmstb_pci_init(void)
{
	brcm_setup_pci_bridge();
	if(brcm_sata_enabled)
		brcm_setup_sata_bridge();
	brcm_setup_pcie_bridge();

#ifdef BRCM_PCI_SUPPORTED
	register_pci_controller(&brcmstb_pci_controller);
#endif
#ifdef BRCM_SATA_SUPPORTED
	if(brcm_sata_enabled)
		register_pci_controller(&brcmstb_sata_controller);
#endif
#ifdef BRCM_PCIE_SUPPORTED
	register_pci_controller(&brcmstb_pcie_controller);
#endif
	return 0;
}

arch_initcall(brcmstb_pci_init);
