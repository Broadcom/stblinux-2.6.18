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

#include <asm/brcmstb/brcm97038c0/boardmap.h>
#include <asm/brcmstb/brcm97038c0/bchp_hif_cpu_intr1.h>
#include <asm/brcmstb/brcm97038c0/bcmintrnum.h>

//#define DEBUG 0
#undef DEBUG

#ifdef DEBUG
#define PRINTK printk
#else
#define PRINTK(...) do { } while(0)
#endif

//#define USE_SECONDARY_SATA  // TUrn on 2nd channel, and turn on DMA for 2nd channel
//#define USE_7038B0_WAR		// Will no longer needed with B1 rev.
//#define USE_TURN_OFF_SATA2_WAR  // when not defined, turn on 2nd channel only, no DMA


/* from PCI spec, Maybe we can put this in some include file. */
#define PCI_ENABLE              0x80000000
#define PCI_IDSEL(x)		(((x)&0x1f)<<11)
#define PCI_FNCSEL(x)		(((x)&0x7)<<8)

#define BCM7038_SATA_VID		0x02421166
#define BCM3250_VID				0x325014e4
#define BCM7041_VID				0xa000141f


#define BCM7042_VID				0x704214e4
#define PCI_DEVICE_ID_7042		PCI_DEVICE_ID_7041


#define PCI_DEVICE_ID_1394		0xe	
#define BCM7038_1394_VID		0x8101104c



/* For now until the BSP defines them */
/* For SATA MMIO BAR5 */

#define PCI_SATA_PHYS_MEM_WIN5_SIZE 0x2000

// SATA mem is now on different window, so expansion starts where the OLD Bx 7041 was
#define PCI_EXPANSION_PHYS_MEM_WIN0_BASE \
	(PCI_7041_PHYS_MEM_WIN0_BASE /*was SATA+PCI_SATA_PHYS_MEM_WIN5_SIZE*/)

#define PCI_EXPANSION_PHYS_IO_WIN0_BASE 0x400

// 2nd PCI slot on certain boards.
#define PCI_DEVICE_ID_EXT2 0x0c

#define CPU2PCI_PCI_SATA_PHYS_MEM_WIN0_BASE  0x10510000
#define CPU2PCI_PCI_SATA_PHYS_IO_WIN0_BASE   0x10520000

#define SATA_IO_BASE	KSEG1ADDR(CPU2PCI_PCI_SATA_PHYS_IO_WIN0_BASE)
#define SATA_MEM_BASE	KSEG1ADDR(CPU2PCI_PCI_SATA_PHYS_MEM_WIN0_BASE)



static char irq_tab_brcm97038[] __initdata = {
//[slot]  = IRQ
  [PCI_DEVICE_ID_3250] = BCM_LINUX_3250_IRQ,    /* BCM3250  */
  [PCI_DEVICE_ID_SATA] = BCM_LINUX_SATA_IRQ,    /* SATA controller */
  [PCI_DEVICE_ID_EXT]  = BCM_LINUX_EXT_PCI_IRQ, /* On-board PCI slot */
  [PCI_DEVICE_ID_7042] = 0,					    /* do not need to fixup. */
  [PCI_DEVICE_ID_1394] = 0,                     /* Do not need fixup for now */
  [PCI_DEVICE_ID_EXT2] = BCM_LINUX_EXT_PCI_IRQ  /* On-board 2nd PCI slot */
			/* For board with 2 expansion slots, both slots share the same IRQ */
};


int __init pcibios_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
	return irq_tab_brcm97038[slot];
}

/* Do platform specific device initialization at pci_enable_device() time */
int pcibios_plat_dev_init(struct pci_dev *dev)
{
	return 0;
}



// global to allow multiple slots
static u32 memAddr = PCI_EXPANSION_PHYS_MEM_WIN0_BASE;
static u32 ioAddr = PCI_EXPANSION_PHYS_IO_WIN0_BASE;

#if 0
// Remembers which devices have already been assigned.
static  int slots[32];
#endif

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



	/********************************************************
	 * SATA Controller Fix-ups
	 ********************************************************/
#define CHIPREV_7038B0 0x00000510

static void brcm_pcibios_fixup_SATA(struct pci_dev *dev)
{
	u32 regData;
	u32 size;
	int use_7038B0_pll_war = 0;
	volatile int use_turn_off_sata2_war = 0;
	volatile int use_secondary_sata = 0;
	//volatile unsigned long sundry_top_ctrl_sun_rev = *(volatile unsigned long*) 0xb0404004;
	volatile unsigned long vd_top_vd_rev_id = *(volatile unsigned long*) 0xb01c1400;

#define CHIPREV_7038B0 0x00000510
	printk("Setting up SATA controller\n");

#if defined( USE_7038B0_WAR ) || defined ( USE_TURN_OFF_SATA2_WAR )
#ifdef USE_7038B0_WAR
	use_7038B0_pll_war = 1;
	use_secondary_sata = 0;
#endif
#ifdef USE_TURN_OFF_SATA2_WAR
	use_turn_off_sata2_war = 1;
	use_secondary_sata = 0;
#endif

#else 
/* No forced configs, just read the chip rev and determine what to do,
 * This is the default behavior starting with 2.4.25-2.0
 */
	if (vd_top_vd_rev_id <= CHIPREV_7038B0) {
		/* B0 chip */
		use_7038B0_pll_war = 1;
		use_turn_off_sata2_war = 1;
		use_secondary_sata = 0;
	}
	else { 
		/* B1 or later */
		use_7038B0_pll_war = 0;
		use_turn_off_sata2_war = 0;
		use_secondary_sata = 1;
	}
#endif
	PRINTK("Setting up SATA controller, VD Rev=%08lx, pll_war=%d, sata2_war=%d, sata2_on=%d\n",
		vd_top_vd_rev_id,
		use_7038B0_pll_war, use_turn_off_sata2_war, use_secondary_sata);
			
	regData = 0x200;
	dev->resource[0].start = regData;
	dev->resource[0].end = regData + 0x3f;
	//dev->resource[0].flags = IORESOURCE_IO;
	if (insert_resource(&ioport_resource, &dev->resource[0])) {
		//printk("fixup: Cannot allocate resource 0\n");
	}
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, regData);	
	
	regData = 0x240;
	dev->resource[1].start = regData;
	dev->resource[1].end = regData + 0x3f;
	//dev->resource[1].flags = IORESOURCE_IO;
	if (insert_resource(&ioport_resource, &dev->resource[1])) {
		PRINTK("fixup: Cannot allocate resource 1\n");
	}
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_1, regData);
	
	regData = 0x280;
	dev->resource[2].start = regData;
	dev->resource[2].end = regData + 0x3f;
	//dev->resource[2].flags = IORESOURCE_IO;
	if (insert_resource(&ioport_resource, &dev->resource[2])) {
		PRINTK("fixup: Cannot allocate resource 2\n");
	}
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_2, regData);
	
	regData = 0x2c0;
	dev->resource[3].start = regData;
	dev->resource[3].end = regData + 0x3f;
	//dev->resource[3].flags = IORESOURCE_IO;
	if (insert_resource(&ioport_resource, &dev->resource[3])) {
		printk("fixup: Cannot allocate resource 3\n");
	}
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_3, regData);
	
	regData = 0x300;
	dev->resource[4].start = regData;
	dev->resource[4].end = regData + 0x3f;
	//dev->resource[4].flags = IORESOURCE_IO;	
	if (insert_resource(&ioport_resource, &dev->resource[4])) {
		printk("fixup: Cannot allocate resource 4\n");
	}
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_4, regData);

	/* On 2.4.25, we have to assign some mem region for MMIO even though we
	 * have no intention of using it
	 */
	size = PCI_SATA_PHYS_MEM_WIN5_SIZE;
	regData = (CPU2PCI_PCI_SATA_PHYS_MEM_WIN0_BASE + size -1) & ~(size-1); 
	dev->resource[5].start = regData;
	dev->resource[5].end = regData +size-1;
	//dev->resource[5].flags = IORESOURCE_MEM;
	if (insert_resource(&iomem_resource, &dev->resource[5])) {
		//printk("fixup: Cannot allocate resource 5\n");
	}
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_5, regData);
		
	regData = 0x0f;
	pci_write_config_byte(dev, 0x3e, regData); /* Minimum Grant, to avoid Latency being reset to lower value */
	
	regData = 0xff;
	pci_write_config_byte(dev, 0xd, regData); /* SATA_CFG_MASTER_LATENCY_TIMER. */


	pci_read_config_dword(dev, PCI_COMMAND, &regData);
	regData |= (PCI_COMMAND_SERR|PCI_COMMAND_PARITY|PCI_COMMAND_MASTER|PCI_COMMAND_IO| PCI_COMMAND_MEMORY);
	pci_write_config_dword(dev, PCI_COMMAND, regData);


	/* Primary */
	pci_read_config_dword(dev, PCI_BASE_ADDRESS_4, &regData);
	regData &= 0xfffffc; // Mask off  Reserved & Res Type bits.
	PRINTK("SATA: Primary Bus Master Status Register offset = %08x + %08x = %08x\n", 
			SATA_IO_BASE, regData, SATA_IO_BASE+regData);

	regData += 2; // Offset 302H for primary
		PRINTK("SATA: before init Primary Bus Master Status reg = 0x%08x.\n", *((volatile unsigned char *)(SATA_IO_BASE+regData)));
		*((volatile unsigned char *)(SATA_IO_BASE+regData)) |= 0x20; // Both Prim and Sec DMA Capable
		PRINTK("SATA: after init Primary Bus Master Status reg = 0x%08x.\n", *((volatile unsigned char *)(SATA_IO_BASE+regData)));

	if (use_secondary_sata) {
		/* Secondary */
		pci_read_config_dword(dev, PCI_BASE_ADDRESS_4, &regData);
		regData &= 0xfffffc; // Mask off  Reserved & Res Type bits.
		PRINTK("SATA: Secondary Bus Master Status Register offset = %08x + %08x = %08x\n", 
				SATA_IO_BASE, regData, SATA_IO_BASE+regData);

		regData += 0xa; // Offset 30AH for secondary
			PRINTK("SATA: before init Secondary Bus Master Status reg = 0x%08x.\n", *((volatile unsigned char *)(SATA_IO_BASE+regData)));
			*((volatile unsigned char *)(SATA_IO_BASE+regData)) |= 0x60; // Both Prim and Sec DMA Capable
			PRINTK("SATA: after init Secondary Bus Master Status reg = 0x%08x.\n", *((volatile unsigned char *)(SATA_IO_BASE+regData)));			
	}

#if 0

//#ifdef USE_7038B0_WAR
	if (use_7038B0_pll_war) {
		// Force PLLLOCK
		*((volatile unsigned long *)(PCI_SATA_PHYS_MEM_WIN5_BASE+0x84)) |= 0x08000000;

		printk("SATA: after PLLLOCK reg@%08x = 0x%08lx.\n", PCI_SATA_PHYS_MEM_WIN5_BASE+0x84,
			*((volatile unsigned long *)(PCI_SATA_PHYS_MEM_WIN5_BASE+0x84)));
		}
//#endif


//#ifdef USE_TURN_OFF_SATA2_WAR
		if (use_turn_off_sata2_war) {
	// Turn off SATA 2
	*((volatile unsigned long *)(PCI_SATA_PHYS_MEM_WIN5_BASE+0x148)) |= 0x00000001;
	printk("SATA: after PLLLOCK reg = 0x%08lx.\n", *((volatile unsigned long *)(PCI_SATA_PHYS_MEM_WIN5_BASE+0x148)));
		}
//#endif
#endif

#if 0
		printk("Turning on SIMR and SERR\n");
		// Turn on SATA error checking, for development debugging only.  Turn this off in production codes.
		*((volatile unsigned long *)(SATA_MEM_BASE+0x88)) |=  0xffffffff & ~(0x94050000);
		printk("SATA: SIMR = 0x%08lx.\n", *((volatile unsigned long *)(SATA_MEM_BASE+0x88)));
		printk("SATA: SCR1 = 0x%08lx.\n", *((volatile unsigned long *)(SATA_MEM_BASE+0x44)));

#endif

		// BRCM_ANDOVER
		// Enable ECO fix to force SYNCs on the SATA interface
		// between PIO data transfer of the packet CDB and DMA mode transfer.
		*((volatile unsigned long *)(SATA_MEM_BASE+0x88)) |= 0x20000000;
		printk("SATA: SIMR = 0x%08lx after ECO fix enable\n", *((volatile unsigned long *)(SATA_MEM_BASE+0x88)));
}

	/********************************************************
	 * Config the BCM3250 chip
	 ********************************************************/
#if 0
static void brcm_pcibios_fixup_3250(struct pci_dev *dev)
{
	u32 regData;
	u32 size;


	printk("\tBCM3250\n\r");
	regData = BCM3250_REG_BASE;
	size = BCM3250_MEM_BASE - BCM3250_REG_BASE;
	dev->resource[0].start = regData;
	dev->resource[0].end = regData + size - 1;
	if (insert_resource(&iomem_resource, &dev->resource[0])) {
		printk("fixup_3250: Cannot allocate resource 0\n");
	}
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, regData);
	regData = BCM3250_MEM_BASE;
	size = BCM7041_MEM_BASE - BCM3250_MEM_BASE;
	dev->resource[1].start = regData;
	dev->resource[1].end = regData + size - 1;
	if (insert_resource(&iomem_resource, &dev->resource[1])) {
		printk("fixup_3250: Cannot allocate resource 1\n");
	}
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_1, regData);

	pci_read_config_dword(dev, PCI_COMMAND, &regData);
	regData |= PCI_COMMAND_MEMORY;
	pci_write_config_dword(dev, PCI_COMMAND, regData);
}
#endif

		

#if 0
	/********************************************************
	 * Config the BCM7042 chip
	 ********************************************************/
	 The 7041 is out, and we decide that just let the Plug&Play logic does the assignment
	case BCM7042_VID:
		Now we configure the 7042 in the plug-n-play fixup
static void brcm_pcibios_fixup_7041(struct pci_dev *dev)
{
	u32 regData;
	u32 size = 0x20000;
	int i;
	
	if (PCI_DEVICE_ID_EXT == PCI_SLOT(dev->devfn) ||
	    PCI_DEVICE_ID_EXT2 == PCI_SLOT(dev->devfn)) {
		printk("\tBCM7041 Dual Channel MPEG2 Encoder on PCI slot\n\r");
		brcm_pcibios_fixup_plugnplay(dev);
		return;
	}
	printk("\tBCM7041 Dual Channel MPEG2 Encoder\n\r");
	
	regData = BCM7041_MEM_BASE;
	for (i=0; i <= 5; i++) {
		dev->resource[i].start = regData;
		dev->resource[i].end = regData + size - 1;
		if (insert_resource(&iomem_resource, &dev->resource[i])) {
			printk("fixup_3250: Cannot allocate resource i\n", i);
		}
		pci_write_config_dword(dev, PCI_BASE_ADDRESS_0+i, regData);
		regData += 0x20000;
	}

	pci_read_config_dword(dev, PCI_COMMAND, &regData);
	regData |= PCI_COMMAND_MEMORY;
	pci_write_config_dword(dev, PCI_COMMAND, regData);
}

#endif

#if 0
static void brcm_pcibios_fixup_SandVideo(struct pci_dev *dev)
{
	u32 regData;
	u32 size;
	int slot;	
	//u32 memAddr = PCI_EXPANSION_PHYS_MEM_WIN0_BASE;
	//u32 ioAddr = PCI_EXPANSION_PHYS_IO_WIN0_BASE;
		
	printk("\tSand Video\n\r");

	slot = PCI_SLOT(dev->devfn);
	if (slots[slot]) {
		printk("Skip resource assignment for dev ID %04x:%04x already allocated\n", dev->vendor, dev->device);
		return;
	}

	slots[slot] = 1;
	regData = memAddr;
	size = 256;
	memAddr += size;
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, regData);
printk("SandVideo: Writing PCI_MEM_BAR[0]=%x, size=%08x\n",
				regData, size);
	
	regData = ioAddr;
	size = 256;
	ioAddr += size;
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_1, regData);
printk("SandVideo: Writing PCI_IO_BAR[0]=%x, size=%08x\n",
				regData, size);
	
	size = 0x02000000;
	regData = (memAddr + size -1) & ~(size -1);
	memAddr = regData + size;
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_2, regData);
printk("SandVideo: Writing PCI_MEM_BAR[2]=%x, size=%08x\n",
				regData, size);

	size = 0x10000;
	regData = (memAddr + size -1) & ~(size -1); // which is just memAddr	
	memAddr = regData + size;  
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_3, regData);
printk("SandVideo: Writing PCI_MEM_BAR[3]=%x, size=%08x\n",
				regData, size);

	pci_read_config_dword(dev, PCI_COMMAND, &regData);
	regData |= (PCI_COMMAND_MASTER|PCI_COMMAND_IO| PCI_COMMAND_MEMORY);
	pci_write_config_dword(dev, PCI_COMMAND, regData);


}
#endif


//#define PCI_VENDOR_ID_BCM7041 (BCM7041_VID&0xffff)
//#define PCI_DEVICE_ID_BCM7041 (BCM7041_VID>>16)

/* 
 * SATA is on the secondary bus, and requires special handling.
 */

DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_SERVERWORKS, PCI_DEVICE_ID_SERVERWORKS_BCM7038,
	 	brcm_pcibios_fixup_SATA);

#if 0
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_BROADCOM, 0x3250,
	 	brcm_pcibios_fixup_3250);
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_BCM7041, PCI_DEVICE_ID_BCM7041,
	 	brcm_pcibios_fixup_7041);
//DECLARE_PCI_FIXUP_HEADER(0x5406, 0x10b5,
//	 	brcm_pcibios_fixup_SandVideo);
#endif
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

# if 0
	struct pci_dev *dev = find_pci_device();	
	volatile unsigned long mmioBase = pci_resource_start(dev, 5);
	u16 cmd, status;
	u32 sataIntStatus;

//	extern int gDbgWhere;
    

//	printk("ide_do_request: %d\n", gDbgWhere);
	printk("7038B0 SATA: Status Reg = %08x\n", *((volatile unsigned long *) (mmioBase+0x1C)));
	printk("7038B0 SATA: Port0 Master Status = %08x\n", *((volatile unsigned long *) (mmioBase+0x30)));
	printk("7038B0 SATA: Port0 SATA Status = %08x\n", *((volatile unsigned long *)  (mmioBase+0x40)));
	printk("7038B0 SATA: Port0 SATA Error = %08x\n", *((volatile unsigned long *)  (mmioBase+0x44)));
#if 0
       pci_find_device(BCM7038_SATA_VID>>16, BCM7038_SATA_VID&0xffff) {

	
	pci_read_config_word(dev, PCI_COMMAND, &cmd);
	pci_read_config_word(dev, PCI_STATUS, &status);
	pci_read_config_dword(dev, 0x80, &sataIntStatus);
#endif
#endif

}
EXPORT_SYMBOL(dump_ide);




