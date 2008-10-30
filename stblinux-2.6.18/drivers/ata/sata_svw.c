/*
 *  sata_svw.c - ServerWorks / Apple K2 SATA
 *
 *  Maintained by: Benjamin Herrenschmidt <benh@kernel.crashing.org> and
 *		   Jeff Garzik <jgarzik@pobox.com>
 *  		    Please ALWAYS copy linux-ide@vger.kernel.org
 *		    on emails.
 *
 *  Copyright 2003 Benjamin Herrenschmidt <benh@kernel.crashing.org>
 *
 *  Bits from Jeff Garzik, Copyright RedHat, Inc.
 *
 *  This driver probably works with non-Apple versions of the
 *  Broadcom chipset...
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 *  libata documentation is available via 'make {ps|pdf}docs',
 *  as Documentation/DocBook/libata.*
 *
 *  Hardware documentation available under NDA.
 *
 *  2007/05/24  Jian Peng <jipeng@broadcom.com>
 *              Support port multiplier and bump version up to 3.0
 *
 *  2007/08/21  Chanshine Nabanxang & Ton Truong <chanshine@broadcom.com>
 *              support for SATA2 3Gbps version 3.1
 *
 *  2007/09/17  Kevin Cernekee <cernekee>
 *              Add EXPERIMENTAL QDMA and NCQ support, bump version to 4.0
 *
 *  2008/01/08  Kevin Cernekee <cernekee>
 *              Add power management support
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <scsi/scsi_host.h>
#include <linux/spinlock.h>
#include <linux/libata.h>   /* BEWARE: redefines writel/readl/writew/readw */

#ifdef CONFIG_PPC_OF
#include <asm/prom.h>
#include <asm/pci-bridge.h>
#endif /* CONFIG_PPC_OF */

#define DRV_NAME	"sata_svw"
#define DRV_VERSION	"4.0"

#ifdef CONFIG_MIPS_BRCM97XXX
#include <asm/brcmstb/common/brcmstb.h>
#include <asm/brcmstb/common/brcm-pm.h>
#endif

#if defined(CONFIG_BRCM_PM)

#define SLEEP_FLAG(host) (long)(host->private_data)
#define SET_SLEEP_FLAG(host, val) do { \
	host->private_data = (void *)(val); \
	} while(0)

#define K2_AWAKE	0
#define K2_SLEEPING	1

static int k2_power_on(void *arg);

#define K2_POWER_ON(host) do { \
	if(SLEEP_FLAG(host) != K2_AWAKE) \
		k2_power_on(host); \
	} while(0)

static DEFINE_SPINLOCK(sleep_lock);

#else

#define K2_POWER_ON(host) do { } while(0)

#endif
// jipeng - if bcmsata2=1, but device only support SATA I, then downgrade to SATA I and reset SATA core
#define	AUTO_NEG_SPEED	

#ifdef  AUTO_NEG_SPEED
static unsigned int new_speed_mask = 0;
#endif

#ifdef	SATA_SVW_BRCM_WA
extern int dma_write_wa_needed;
#endif

#ifdef CONFIG_SATA_SVW_NCQ
#define USE_QDMA
#define USE_NCQ
#endif

#if ! defined(USE_QDMA)

struct k2_port_priv {
	int do_port_srst;			/* already perform softreset? */
};

#else /* ! defined(USE_QDMA) */

#define QDMA_RING_SIZE		64
#define QDMA_RING_MASK		(QDMA_RING_SIZE - 1)
#define MAX_PRD_ENTRIES		8

struct k2_cmd_descr
{
	uint32_t		w0, w1, w2, w3, w4, w5, w6, w7;
};

struct k2_port_priv
{
	struct k2_cmd_descr	*cmd_ring;
	dma_addr_t		cmd_ring_dma;
	struct k2_prd_entry	*prd_tbl;
	dma_addr_t		prd_tbl_dma;
	unsigned int		cmd_idx;
	char			tags[QDMA_RING_SIZE];
};

struct k2_prd_entry
{
	uint32_t		addr_lo;
	uint32_t		addrhi_count;
};

#define SCR1_ERR_DATA		(1 << 0)
#define SCR1_ERR_COMM		(1 << 1)
#define SCR1_ERR_UNRECOV	(1 << 8)
#define SCR1_ERR_PERSIST	(1 << 9)
#define SCR1_ERR_PROT		(1 << 10)
#define SCR1_ERR_INTERNAL	(1 << 11)
#define SCR1_ERR_PHYRDY		(1 << 16)
#define SCR1_ERR_PHYINT		(1 << 17)
#define SCR1_ERR_COMWAKE	(1 << 18)
#define SCR1_ERR_DECODE		(1 << 19)
#define SCR1_ERR_DISPARITY	(1 << 20)
#define SCR1_ERR_CRC		(1 << 21)
#define SCR1_ERR_RERR		(1 << 22)
#define SCR1_ERR_LINK		(1 << 23)
#define SCR1_ERR_TRANSPORT	(1 << 24)
#define SCR1_ERR_FIS		(1 << 25)
#define SCR1_ERR_EXCHANGED	(1 << 26)

#define SCR1_ERR_HOTPLUG	(SCR1_ERR_EXCHANGED | \
				 SCR1_ERR_COMWAKE | \
				 SCR1_ERR_PHYRDY)

#define SCR1_ERR_ATA		(SCR1_ERR_COMM | \
				 SCR1_ERR_UNRECOV | \
				 SCR1_ERR_PERSIST | \
				 SCR1_ERR_PROT | \
				 SCR1_ERR_INTERNAL | \
				 SCR1_ERR_PHYINT | \
				 SCR1_ERR_DECODE | \
				 SCR1_ERR_DISPARITY | \
				 SCR1_ERR_CRC | \
				 SCR1_ERR_RERR | \
				 SCR1_ERR_LINK | \
				 SCR1_ERR_TRANSPORT)

#define QSR_ERR_DONE		(1 << 0)
#define QSR_ERR_PAUSE_ACK	(1 << 1)
#define QSR_ERR_ABORT_ACK	(1 << 2)
#define QSR_ERR_RESET_ACK	(1 << 3)
#define QSR_ERR_HOTPLUG		(1 << 4)
#define QSR_ERR_CMDERR		(1 << 5)
#define QSR_ERR_PCI_ERR		(1 << 16)
#define QSR_ERR_ATA_CMD		(1 << 17)
#define QSR_ERR_PRD_UNDERFLOW	(1 << 18)
#define QSR_ERR_PRD_OVERFLOW	(1 << 19)
#define QSR_ERR_DATA_CRC	(1 << 20)
#define QSR_ERR_PCI_ABORT	(1 << 21)

#define QSR_ERR_HOST		(QSR_ERR_PCI_ABORT | \
				 QSR_ERR_PCI_ERR | \
				 QSR_ERR_PRD_UNDERFLOW | \
				 QSR_ERR_PRD_OVERFLOW)

#define QSR_ERR_DEVICE		(QSR_ERR_CMDERR | \
				 QSR_ERR_ATA_CMD | \
				 QSR_ERR_DATA_CRC | \
				 QSR_ERR_HOTPLUG)

#define QSR_ERR_ANY		(QSR_ERR_HOST | QSR_ERR_DEVICE)

#endif /* ! defined(USE_QDMA) */

enum {
	/* Taskfile registers offsets */
	K2_SATA_TF_CMD_OFFSET		= 0x00,
	K2_SATA_TF_DATA_OFFSET		= 0x00,
	K2_SATA_TF_ERROR_OFFSET		= 0x04,
	K2_SATA_TF_NSECT_OFFSET		= 0x08,
	K2_SATA_TF_LBAL_OFFSET		= 0x0c,
	K2_SATA_TF_LBAM_OFFSET		= 0x10,
	K2_SATA_TF_LBAH_OFFSET		= 0x14,
	K2_SATA_TF_DEVICE_OFFSET	= 0x18,
	K2_SATA_TF_CMDSTAT_OFFSET      	= 0x1c,
	K2_SATA_TF_CTL_OFFSET		= 0x20,

	/* DMA base */
	K2_SATA_DMA_CMD_OFFSET		= 0x30,

	/* SCRs base */
	K2_SATA_SCR_STATUS_OFFSET	= 0x40,	/* aka SCR0 */
	K2_SATA_SCR_ERROR_OFFSET	= 0x44,	/* aka SCR1 */
	K2_SATA_SCR_CONTROL_OFFSET	= 0x48,	/* aka SCR2 */
	K2_SATA_SCR_ACTIVE_OFFSET	= 0x4c,	/* aka SCR3 (but not compatible) */

	/* Others */
	K2_SATA_SICR1_OFFSET		= 0x80,
	K2_SATA_SICR2_OFFSET		= 0x84,
	K2_SATA_SIMR_OFFSET		= 0x88,	/* SATA interrupt mask register */
	K2_SATA_MDIO_OFFSET		= 0x8c,	/* SATA MDIO access register */
	K2_SATA_SCQR_OFFSET		= 0x94,	/* SATA command queue depth */
	K2_SATA_QAL_OFFSET		= 0xa0,	/* QDMA ring address lower */
	K2_SATA_QAU_OFFSET		= 0xa4,	/* QDMA ring address upper */
	K2_SATA_QPI_OFFSET		= 0xa8,	/* QDMA producer index */
	K2_SATA_QCI_OFFSET		= 0xac,	/* QDMA consumer index */
	K2_SATA_QCR_OFFSET		= 0xb0,	/* QDMA control register */
	K2_SATA_QDR_OFFSET		= 0xb4,	/* QDMA queue depth */
	K2_SATA_QSR_OFFSET		= 0xb8,	/* QDMA status register */
	K2_SATA_QMR_OFFSET		= 0xbc,	/* QDMA interrupt mask */
	K2_SATA_QCI2_OFFSET		= 0xc0,	/* QDMA internal QCI for NCQ */

	K2_SATA_E0_OFFSET		= 0xe0,
	K2_SATA_F0_OFFSET		= 0xf0,

	/* Port stride */
	K2_SATA_PORT_OFFSET		= 0x100,

	K2_SATA_GLOBAL_STATUS		= 0x1004,
	K2_SATA_GLOBAL_MASK		= 0x1018,
};

#define PORT_BASE(x, y)		((x) + (K2_SATA_PORT_OFFSET * (unsigned)(y)))
#define PORT_MMIO(x)		PORT_BASE((void __iomem *) \
					((x)->host->mmio_base), \
					(x)->port_no)

#ifdef CONFIG_MIPS_BRCM97XXX

/*
 * Extra init functions for stblinux platforms
 */

#define WRITE_CMD			1
#define READ_CMD			2
#define CMD_DONE			(1 << 15)
#define SATA_MMIO			0x24

// 1. port is SATA port ( 0 or 1)
// 2. reg is the address of the MDIO register ( see spec )
// 3. MMIO_BASE_ADDR  is MMIO base address from SATA PCI configuration
// registers addr 24-27
static uint16_t mdio_read_reg(void __iomem *mmio_base, int port,
	int reg)
{
	void __iomem *mdio = mmio_base + K2_SATA_MDIO_OFFSET;
	volatile unsigned int pSel = 1 << port;
	uint32_t cmd  = WRITE_CMD;
	
	if( reg > 0x13 )
	return( -1 );

	//Select Port
	writel(pSel<<16 | (cmd << 13) | 7, mdio);             //using dev_addr 0
	while( !(readl(mdio) & CMD_DONE) )
	udelay( 1);     //wait

	writel((READ_CMD << 13) + reg, mdio);
	while( !(readl(mdio) & CMD_DONE) )
	udelay( 1 );    //wait

	return( readl(mdio) >> 16 );
}

static void mdio_write_reg(void __iomem *mmio_base, int port,
	int reg, uint16_t val )
{
	void __iomem *mdio = mmio_base + K2_SATA_MDIO_OFFSET;
	volatile unsigned int pSel = 1 << port;
	uint32_t cmd  = WRITE_CMD;
	
	if( reg > 0x13 )
	return;

	//Select Port
	writel(pSel<<16 | (cmd << 13) | 7, mdio);             //using dev_addr 0
	while( !(readl(mdio) & CMD_DONE) )
	udelay( 1);     //wait

	writel((val << 16) + (WRITE_CMD << 13) + reg, mdio);          //using dev_addr 0
	while( !(readl(mdio) & CMD_DONE) )
	udelay( 1 );    //wait
}

static void DisablePHY(void __iomem *mmio_base, int port)
{
	uint32_t *pScr2 = PORT_BASE(mmio_base, port) + K2_SATA_SCR_CONTROL_OFFSET;
	writel(1, pScr2);
}

static void EnablePHY(void __iomem *mmio_base, int port)
{
	uint32_t *pScr2 = PORT_BASE(mmio_base, port) + K2_SATA_SCR_CONTROL_OFFSET;
	writel(0, pScr2);
}

static void bcm_sg_workaround(void __iomem *mmio_base, int port)
{
	int tmp16;
	extern int gSataInterpolation;
 
	DisablePHY(mmio_base, port);
 
	/* 
	 * Do Interpolation when 
	  * spread spectrucm clocking (SSC) is NOT enabled. But, the code must
	  * be used for a system with SSC-enabled drive.
	  * gSataInterpolation is not zero when the argument bcmssc=1 is specified
	  */

 	if (gSataInterpolation) {
		tmp16 = mdio_read_reg(mmio_base, port, 9);
    		mdio_write_reg(mmio_base, port, 9, tmp16 | 1); //Bump up interpolation
 	}

	//Do analog reset
	tmp16 = mdio_read_reg(mmio_base, port,4);
	tmp16 |= 8;
	mdio_write_reg(mmio_base, port,4,tmp16);
 
	udelay( 1000 );      //wait 1 ms
	tmp16 &= 0xFFF7;
	mdio_write_reg(mmio_base, port,4,tmp16 ); // Take reset out
	udelay( 1000 );      //wait
 
	//Enable PHY
	EnablePHY(mmio_base, port);
}

// Fix to turn on 3Gbps 

//This routine change (lower) bandwidth of SATA PLL to lower jitter from main internal // ref clk in attempt to use on chip refclock.
static void brcm_SetPllTxRxCtrl(void __iomem *mmio_base, int port)
{
	uint16_t tmp16;
	//Lower BW
	mdio_write_reg(mmio_base, port, 0, 0x1404);

	//Change Tx control
	mdio_write_reg(mmio_base, port, 0xa, 0x0260);
	mdio_write_reg(mmio_base, port, 0x11, 0x0a10);
	//Change Rx control
	tmp16 = mdio_read_reg(mmio_base, port, 0xF);
	tmp16 &= 0xc000;
	tmp16 |= 0x1000;
	mdio_write_reg(mmio_base, port, 0xF, tmp16);
}

static void brcm_TunePLL(void __iomem *mmio_base, int port)
{
	volatile uint16_t tmp;
	int i;

	//program VCO step bit [12:10] start with 111
	mdio_write_reg(mmio_base, port, 0x13, 0x1c00);

	udelay(100000);

	//start pll tuner
	mdio_write_reg(mmio_base, port, 0x13, 0x1e00); // 

	udelay(10000); // wait

	//check lock bit
	tmp = mdio_read_reg(mmio_base, port,0x7);

	for(i = 0; i < 10000; i++) {
		tmp = mdio_read_reg(mmio_base, port, 0x7);
		if((tmp & 0x8000) == 0x8000)
			return;
		udelay(1);
	}
	DPRINTK("PLL did not lock\n");
}

static void brcm_AnalogReset(void __iomem *mmio_base, int port)
{  
	//do analog reset
	mdio_write_reg(mmio_base, port, 0x4, 8);
	udelay(10000); // wait
	mdio_write_reg(mmio_base, port, 0x4, 0);

	bcm_sg_workaround(mmio_base,port);
}

static void brcm_InitSata_1_5Gb(void __iomem *mmio_base, int port)
{
	volatile uint16_t tmp;
	volatile uint32_t tmp32;
	void __iomem *port_mmio;

	port_mmio = PORT_BASE(mmio_base, port);
	writel(1, port_mmio + K2_SATA_SCR_CONTROL_OFFSET);
	udelay(10000); // wait
	//reset deskew TX FIFO
	//1. select port
	mdio_write_reg(mmio_base, port, 7, 1<<port);
	//toggle reset bit
	mdio_write_reg(mmio_base, port, 0xd, 0x4800);
	udelay(10000); // wait
	mdio_write_reg(mmio_base, port, 0xd, 0x0800);
	udelay(10000); // wait

	//Enable low speed (1.5G) mode
	tmp = mdio_read_reg(mmio_base, port, 8);
	mdio_write_reg(mmio_base, port, 8, tmp | 0x10);

	//Enable lock monitor.. if not set the lock bit is not updated
	tmp = mdio_read_reg(mmio_base, port, 0x13);

	mdio_write_reg(mmio_base, port, 0x13, tmp|2);
	udelay(10000);

#ifdef	AUTO_NEG_SPEED
	// disable 3G feature
	tmp32 = readl((void *)(MMIO_OFS + 0xf0 + port * 0x100));
	writel(tmp32 & 0xfffffbff, (void *)(MMIO_OFS + 0xf0 + port * 0x100));
	udelay(10000);
#endif

	//enable 4G addressing support
	tmp32 = readl(port_mmio + K2_SATA_SICR2_OFFSET);
	writel(tmp32 | 0x20009400, port_mmio + K2_SATA_SICR2_OFFSET);

	tmp32 = readl(port_mmio + K2_SATA_SICR1_OFFSET);
	tmp32 &= 0xffff0000;
	writel(tmp32 | 0x00000f10, port_mmio + K2_SATA_SICR1_OFFSET);

#ifndef USE_QDMA // PR35117
	//Clean up the fifo -- per Ajay
	tmp32 = readl(port_mmio + K2_SATA_E0_OFFSET);
	writel(tmp32 | 2, port_mmio + K2_SATA_E0_OFFSET);
#endif

	//Tweak PLL, Tx, and Rx
	brcm_SetPllTxRxCtrl(mmio_base, port);
	brcm_TunePLL(mmio_base, port);
	brcm_AnalogReset(mmio_base, port);
	udelay(10000); // wait

	writel(0, port_mmio + K2_SATA_SCR_CONTROL_OFFSET);
}

static void brcm_InitSata2_3Gb(void __iomem *mmio_base, int port)
{
	volatile uint16_t tmp;
	volatile uint32_t tmp32;
	void __iomem *port_base;

	port_base = PORT_BASE(mmio_base, port);
	writel(1, port_base + K2_SATA_SCR_CONTROL_OFFSET);
	udelay(10000); // wait

	//reset deskew TX FIFO
	//1. select port
	mdio_write_reg(mmio_base, port, 7, 1<<port);
	//toggle reset bit
	mdio_write_reg(mmio_base, port, 0xd, 0x4800);
	udelay(10000); // wait
	mdio_write_reg(mmio_base, port, 0xd, 0x0800);
	udelay(10000); // wait

	//run at 3G mode
	tmp = mdio_read_reg(mmio_base, port, 8);
	mdio_write_reg(mmio_base, port, 8, tmp & ~(0x10));
	udelay(10000); // wait

	//Enable lock monitor.. if not set the lock bit is not updated
	tmp = mdio_read_reg(mmio_base, port, 0x13);
	mdio_write_reg(mmio_base, port, 0x13, tmp|2);

	//Enable 3Gb interface
	tmp32 = readl(port_base + K2_SATA_F0_OFFSET);
	writel(tmp32 | 0x10500, port_base + K2_SATA_F0_OFFSET);
	udelay(10000); // wait

	//enable 4G addressing support
	tmp32 = readl(port_base + K2_SATA_SICR2_OFFSET);
	writel(tmp32 | 0x20009400, port_base + K2_SATA_SICR2_OFFSET);

	tmp32 = readl(port_base + K2_SATA_SICR1_OFFSET);
	tmp32 &= 0xffff0000;
	writel(tmp32 | 0x00000f10, port_base + K2_SATA_SICR1_OFFSET);

	//Clean up the fifo -- per Ajay
	tmp32 = readl(port_base + K2_SATA_E0_OFFSET);
	writel(tmp32|2, port_base + K2_SATA_E0_OFFSET);

	//Tweak PLL, Tx, and Rx
	brcm_SetPllTxRxCtrl(mmio_base, port);
	brcm_TunePLL(mmio_base, port);
	brcm_AnalogReset(mmio_base, port);
	udelay(10000); // wait

	writel(0, port_base + K2_SATA_SCR_CONTROL_OFFSET);
}

/* Kernel argument bcmsata2=0|1 */
extern int gSata2_3Gbps;

static inline void brcm_initsata2(void __iomem *mmio_base, int num_ports)
{
        int port;
        unsigned int first=1;

retry_brcm_initsata2:

        for (port=0; port < num_ports; port++) {
                /*
                 * Turn on 3Gbps if bcmsata2=1 is specified as kernel argument
                 * during bootup
                 */
                if(gSata2_3Gbps) {
                        if(new_speed_mask == 0)
                                brcm_InitSata2_3Gb(mmio_base, port);
                        else if(new_speed_mask & (1<<port))
                                brcm_InitSata_1_5Gb(mmio_base, port);
                }
                else
                        brcm_InitSata_1_5Gb(mmio_base, port);
        }

#ifdef  AUTO_NEG_SPEED
        if(gSata2_3Gbps && first)
        {
                int port;
                msleep(10);

                for (port=0; port < num_ports; port++)
                {
                        unsigned int status = readl((void *)(mmio_base + port*K2_SATA_PORT_OFFSET + K2_SATA_SCR_STATUS_OFFSET));
                        if( (status & 0xf) >= 0x5 && (status & 0xf0) == 0 && (status & 0xf00) == 0 ) 
			new_speed_mask |= 1<<port;
                }
                first = 0;
                if(new_speed_mask)
                goto retry_brcm_initsata2;
        }

	
	new_speed_mask = 0;
#endif
}

static void bcm97xxx_sata_init(struct pci_dev *dev, struct ata_probe_ent *probe_ent)
{
	unsigned int reg;
	void __iomem *mmio_base = probe_ent->mmio_base;
	
	/* minimum grant, to avoid Latency being reset to lower value */
	pci_write_config_byte(dev, PCI_MIN_GNT, 0x0f);

	/* SATA master latency timer */
	pci_write_config_byte(dev, PCI_LATENCY_TIMER, 0xff);

	if(dev->device == PCI_DEVICE_ID_SERVERWORKS_BCM7400D0)
	{
		brcm_initsata2(mmio_base, probe_ent->n_ports);
		return; /* Skip all workarounds.  Those have been fixed with 65nm */
	}
	
	if (dev->device != PCI_DEVICE_ID_SERVERWORKS_BCM7038) 
	{
		/* force Master Latency Timer value to 64 PCICLKs */
		pci_write_config_byte(dev, PCI_LATENCY_TIMER, 0x40);
	} else {
        	int port;
		void __iomem *port_base;

#if defined( CONFIG_MIPS_BCM7403 )
#define FIXED_REV       0x74030010
static volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;
#elif defined(CONFIG_MIPS_BCM7038)
#define FIXED_REV       0x70380024
static volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;
#else
#define FIXED_REV       0	/* assume fixed */
static volatile unsigned long* pSundryRev = NULL;
#endif

		/* "WD workaround" - only available on FIXED chips */
		if (! FIXED_REV || (*pSundryRev >= FIXED_REV)) {
			/*
			* Before accessing the MDIO registers through pci space disable external MDIO access.
			* write MDIO register at offset 0x07 with (1 << port number) where port number starts at 0.
			* Read MDIO register at offset 0x0D into variable reg.
			- reg_0d = reg_0d | 0x04
			- Write reg_0d to MDIO register at offset 0x0D.
			*/
			for (port = 0; port < probe_ent->n_ports; port++) {
				// Put PHY in reset
				port_base = PORT_BASE(mmio_base, 0);
				reg = readl(port_base+K2_SATA_SCR_CONTROL_OFFSET);
				writel((reg & ~0xf) | 1, port_base+K2_SATA_SCR_CONTROL_OFFSET);

				// Choose the port and read reg 0xd
				reg = mdio_read_reg(mmio_base, port, 0xd);
				reg |= 0x4;
				mdio_write_reg(mmio_base, port, 0xd, reg);
				reg = mdio_read_reg(mmio_base, port, 0xd);

				// Re-enable PHY
				reg = readl(port_base+K2_SATA_SCR_CONTROL_OFFSET);
				writel(reg & ~0xf, port_base+K2_SATA_SCR_CONTROL_OFFSET);
			}
		}
	
		//PR22401: Identify Seagate drives with ST controllers.
		for (port=0; port < probe_ent->n_ports; port++)
			bcm_sg_workaround(mmio_base,port);
	}
}

#endif	// CONFIG_MIPS_BRCM97XXX	


/*
 * Common (legacy/QDMA) functions
 */

static u32 k2_sata_scr_read (struct ata_port *ap, unsigned int sc_reg)
{
	if (sc_reg > SCR_CONTROL)
		return 0xffffffffU;
	return readl((void *) ap->ioaddr.scr_addr + (sc_reg * 4));
}


static void k2_sata_scr_write (struct ata_port *ap, unsigned int sc_reg,
			       u32 val)
{
	if (sc_reg > SCR_CONTROL)
		return;
	writel(val, (void *) ap->ioaddr.scr_addr + (sc_reg * 4));
}

static u8 k2_stat_check_status(struct ata_port *ap)
{
       	return readl((void *) ap->ioaddr.status_addr);
}

static void k2_sata_tf_read(struct ata_port *ap, struct ata_taskfile *tf)
{
	struct ata_ioports *ioaddr = &ap->ioaddr;
	u16 nsect, lbal, lbam, lbah, feature;
	void __iomem *port_mmio = PORT_MMIO(ap);
	u32 qcr, qsr;

	qcr = readl(port_mmio + K2_SATA_QCR_OFFSET);

	/* QDMA active, unpaused - pause the queue to read taskfile */
	if((qcr & 3) == 1)
	{
		int i = 0;

		writel(qcr | 2, port_mmio + K2_SATA_QCR_OFFSET);
		do {
			qsr = readl(port_mmio + K2_SATA_QSR_OFFSET);
			if(qsr & 2)
				break;
			if(++i == 1000) {
				ata_port_printk(ap, KERN_WARNING,
					"error pausing queue for taskfile read\n");
				break;
			}
			udelay(1);
		} while(1);
	}

	tf->command = k2_stat_check_status(ap);
	tf->device = readw((void *)ioaddr->device_addr);
	feature = readw((void *)ioaddr->error_addr);
	nsect = readw((void *)ioaddr->nsect_addr);
	lbal = readw((void *)ioaddr->lbal_addr);
	lbam = readw((void *)ioaddr->lbam_addr);
	lbah = readw((void *)ioaddr->lbah_addr);

	tf->feature = feature;
	tf->nsect = nsect;
	tf->lbal = lbal;
	tf->lbam = lbam;
	tf->lbah = lbah;

	if (tf->flags & ATA_TFLAG_LBA48) {
		tf->hob_feature = feature >> 8;
		tf->hob_nsect = nsect >> 8;
		tf->hob_lbal = lbal >> 8;
		tf->hob_lbam = lbam >> 8;
		tf->hob_lbah = lbah >> 8;
        }
	/* unpause queue */
	if((qcr & 3) == 1)
		writel(qcr, port_mmio + K2_SATA_QCR_OFFSET);
}

#ifdef CONFIG_PPC_OF
/*
 * k2_sata_proc_info
 * inout : decides on the direction of the dataflow and the meaning of the
 *	   variables
 * buffer: If inout==FALSE data is being written to it else read from it
 * *start: If inout==FALSE start of the valid data in the buffer
 * offset: If inout==FALSE offset from the beginning of the imaginary file
 *	   from which we start writing into the buffer
 * length: If inout==FALSE max number of bytes to be written into the buffer
 *	   else number of bytes in the buffer
 */
static int k2_sata_proc_info(struct Scsi_Host *shost, char *page, char **start,
			     off_t offset, int count, int inout)
{
	struct ata_port *ap;
	struct device_node *np;
	int len, index;

	/* Find  the ata_port */
	ap = ata_shost_to_port(shost);
	if (ap == NULL)
		return 0;

	/* Find the OF node for the PCI device proper */
	np = pci_device_to_OF_node(to_pci_dev(ap->host->dev));
	if (np == NULL)
		return 0;

	/* Match it to a port node */
	index = (ap == ap->host->ports[0]) ? 0 : 1;
	for (np = np->child; np != NULL; np = np->sibling) {
		const u32 *reg = get_property(np, "reg", NULL);
		if (!reg)
			continue;
		if (index == *reg)
			break;
	}
	if (np == NULL)
		return 0;

	len = sprintf(page, "devspec: %s\n", np->full_name);

	return len;
}
#endif /* CONFIG_PPC_OF */


#if ! defined(USE_QDMA)

/*
 * Legacy-only functions
 */

static void k2_sata_tf_load(struct ata_port *ap, const struct ata_taskfile *tf)
{
	struct ata_ioports *ioaddr = &ap->ioaddr;
	unsigned int is_addr = tf->flags & ATA_TFLAG_ISADDR;

	if (tf->ctl != ap->last_ctl) {
#if defined (CONFIG_MIPS_BCM_NDVD)
		unsigned int mask;
		if (tf->ctl & ATA_NIEN) {
			void __iomem *port_mmio = PORT_MMIO(ap);
			u32 simr = readl(port_mmio + K2_SATA_SIMR_OFFSET);
#if defined (CONFIG_MIPS_BCM7440)
			mask = 0xa0000000;
#else
			mask = 0x80000000;
#endif
			writel(simr | mask, port_mmio + K2_SATA_SIMR_OFFSET);
		}
		else
#endif
		{
			writeb(tf->ctl, ioaddr->ctl_addr);
		}
		ap->last_ctl = tf->ctl;
		ata_wait_idle(ap);
	}
	if (is_addr && (tf->flags & ATA_TFLAG_LBA48)) {
		writew(tf->feature | (((u16)tf->hob_feature) << 8), (void *)ioaddr->feature_addr);
		writew(tf->nsect | (((u16)tf->hob_nsect) << 8), (void *)ioaddr->nsect_addr);
		writew(tf->lbal | (((u16)tf->hob_lbal) << 8), (void *)ioaddr->lbal_addr);
		writew(tf->lbam | (((u16)tf->hob_lbam) << 8), (void *)ioaddr->lbam_addr);
		writew(tf->lbah | (((u16)tf->hob_lbah) << 8), (void *)ioaddr->lbah_addr);
	} else if (is_addr) {
		writew(tf->feature, (void *)ioaddr->feature_addr);
		writew(tf->nsect, (void *)ioaddr->nsect_addr);
		writew(tf->lbal, (void *)ioaddr->lbal_addr);
		writew(tf->lbam, (void *)ioaddr->lbam_addr);
		writew(tf->lbah, (void *)ioaddr->lbah_addr);
	}

	if (tf->flags & ATA_TFLAG_DEVICE)
		writeb(tf->device, (void *)ioaddr->device_addr);

	ata_wait_idle(ap);
}

/**
 *	k2_bmdma_setup_mmio - Set up PCI IDE BMDMA transaction (MMIO)
 *	@qc: Info associated with this ATA transaction.
 *
 *	LOCKING:
 *	spin_lock_irqsave(host lock)
 */

static void k2_bmdma_setup_mmio (struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;
	unsigned int rw = (qc->tf.flags & ATA_TFLAG_WRITE);
	u8 dmactl;
	void __iomem *mmio = (void __iomem *) ap->ioaddr.bmdma_addr;
	/* load PRD table addr. */
	mb();	/* make sure PRD table writes are visible to controller */
	writel(ap->prd_dma, mmio + ATA_DMA_TABLE_OFS);

	/* specify data direction, triple-check start bit is clear */
	dmactl = readb(mmio + ATA_DMA_CMD);
	dmactl &= ~(ATA_DMA_WR | ATA_DMA_START);
	if (!rw)
		dmactl |= ATA_DMA_WR;
	writeb(dmactl, mmio + ATA_DMA_CMD);

#if defined (CONFIG_MIPS_BCM_NDVD)
	/* Read back/flush the byte written */
	dmactl = readb(mmio + ATA_DMA_CMD);
#endif

	/* issue r/w command if this is not a ATA DMA command*/
	if (qc->tf.protocol != ATA_PROT_DMA)
		ap->ops->exec_command(ap, &qc->tf);
}

/**
 *	k2_bmdma_start_mmio - Start a PCI IDE BMDMA transaction (MMIO)
 *	@qc: Info associated with this ATA transaction.
 *
 *	LOCKING:
 *	spin_lock_irqsave(host lock)
 */

static void k2_bmdma_start_mmio (struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;
	void __iomem *mmio = (void __iomem *) ap->ioaddr.bmdma_addr;
	u8 dmactl;

	/* start host DMA transaction */
	dmactl = readb(mmio + ATA_DMA_CMD);
	writeb(dmactl | ATA_DMA_START, mmio + ATA_DMA_CMD);
	/* There is a race condition in certain SATA controllers that can
	   be seen when the r/w command is given to the controller before the
	   host DMA is started. On a Read command, the controller would initiate
	   the command to the drive even before it sees the DMA start. When there
	   are very fast drives connected to the controller, or when the data request
	   hits in the drive cache, there is the possibility that the drive returns a part
	   or all of the requested data to the controller before the DMA start is issued.
	   In this case, the controller would become confused as to what to do with the data.
	   In the worst case when all the data is returned back to the controller, the
	   controller could hang. In other cases it could return partial data returning
	   in data corruption. This problem has been seen in PPC systems and can also appear
	   on an system with very fast disks, where the SATA controller is sitting behind a
	   number of bridges, and hence there is significant latency between the r/w command
	   and the start command. */

#if defined (CONFIG_MIPS_BCM_NDVD)
	/* Read back/flush the byte written */
	dmactl = readb(mmio + ATA_DMA_CMD);
#endif

	/* issue r/w command if the access is to ATA*/
	if (qc->tf.protocol == ATA_PROT_DMA)
		ap->ops->exec_command(ap, &qc->tf);
}

static int k2_sata_port_start(struct ata_port *ap)
{
	struct k2_port_priv *pp;
	int rc = -ENOMEM;

	pp = kzalloc(sizeof(*pp), GFP_KERNEL);
	if (!pp)
		goto err_out;

	ap->private_data = pp;
	ata_port_start(ap);
	
	return 0;
err_out:
	return rc;
}

static int k2_sata_pmp_attach_link(struct ata_link *link, int pmp)
{
	struct ata_port *ap = link->ap;
	void __iomem *port = (void __iomem *)ap->ioaddr.cmd_addr;
	int rc;
	u32 scontrol;

	if(pmp < 0 || pmp >= 0xf) {
		rc = 1;
		return rc;
	}

	scontrol = readl(port + K2_SATA_SCR_CONTROL_OFFSET);
	if((scontrol & 0x000f0000)>>16 == pmp) {
		rc = 2;
		return rc;
	} 

	scontrol = readl(port + K2_SATA_SCR_CONTROL_OFFSET);
	writel((scontrol & ~0x000f0000) | (pmp << 16), port + K2_SATA_SCR_CONTROL_OFFSET);
			
	if (!ata_link_offline(link)) {
		rc = sata_scr_write(link, SCR_ERROR, 0xffffffff);
		rc = 0;
	}
	else {
		sata_scr_write(link, SCR_CONTROL, 1);
		rc = 3;
	}

	return rc;
}

static void k2_sata_pmp_attach(struct ata_port *ap)
{
	struct ata_link *link;
	
	ata_port_for_each_link(link, ap) 
	k2_sata_pmp_attach_link(link, link->pmp);
}

static void k2_sata_pmp_detach(struct ata_port *ap)
{
}

static void k2_sata_dev_select(struct ata_port *ap, unsigned int device)
{
	if(ap->nr_pmp_links)
	{
#ifdef	SATA_SVW_BRCM_WA
		if(dma_write_wa_needed)
		{
			struct ata_taskfile tf;
			struct ata_device *dev = ap->link.device;

			if(k2_sata_pmp_attach_link(&ap->link, device) == 2)
			{
				dma_write_wa_needed = 0;
				return;
			}

			ata_tf_init(dev, &tf);
			tf.command = 0xE5;
			tf.flags |= ATA_TFLAG_DEVICE;
			ap->ops->tf_load(ap, &tf);
			ap->ops->exec_command(ap, &tf);
			dma_write_wa_needed = 0;
		}	
#endif
		k2_sata_pmp_attach_link(&ap->link, device);
	} else {
		ata_std_dev_select(ap, device);	
	}
}

static int k2_sata_pmp_read(struct ata_device *dev, int pmp, int reg, u32 *r_val)
{
	struct ata_port *ap = dev->link->ap;
	void __iomem *port = (void __iomem *)ap->ioaddr.cmd_addr;
	struct ata_taskfile tf;
	u32 scr2_val;

	if (ap->ops->freeze)
		ap->ops->freeze(ap);

	scr2_val = readl(port + K2_SATA_SCR_CONTROL_OFFSET);
	writel(0x000F0000, port + K2_SATA_SCR_CONTROL_OFFSET);
	msleep(10);

	sata_pmp_read_init_tf(&tf, dev, pmp, reg);
	ap->ops->tf_load(ap, &tf);
	ata_exec_command(ap, &tf);
	msleep(10);
	
	memset(&tf, 0, sizeof(tf));
	ap->ops->tf_read(ap, &tf);
	*r_val = sata_pmp_read_val(&tf);
	
	if (ap->ops->thaw)
	ap->ops->thaw(ap);

	writel(scr2_val, port + K2_SATA_SCR_CONTROL_OFFSET);
	return 0;
}

static int k2_sata_pmp_write(struct ata_device *dev, int pmp, int reg, u32 val)
{
	struct ata_port *ap = dev->link->ap;
	void __iomem *port = (void __iomem *)ap->ioaddr.cmd_addr;
	struct ata_taskfile tf;
	u32 scr2_val;
	int rc=0;

	if (ap->ops->freeze)
		ap->ops->freeze(ap);

	scr2_val = readl(port + K2_SATA_SCR_CONTROL_OFFSET);
	writel(0xF0000, port + K2_SATA_SCR_CONTROL_OFFSET);
	msleep(1);

	sata_pmp_write_init_tf(&tf, dev, pmp, reg, val);
	ap->ops->tf_load(ap, &tf);	
	ata_exec_command(ap, &tf);

	msleep(10);

	if (ap->ops->thaw)
	ap->ops->thaw(ap);
		
	writel(scr2_val, port + K2_SATA_SCR_CONTROL_OFFSET);
	return rc;
}

static int k2_sata_do_softreset(struct ata_link *link, unsigned int *class, int pmp)
{
	struct ata_port *ap = link->ap;	
	void __iomem *port = (void __iomem *)ap->ioaddr.cmd_addr;
	struct k2_port_priv *pp = ap->private_data;
	struct ata_taskfile tf;

	if (pp->do_port_srst)
		goto out;
		
	if (ata_link_offline(link)) {
		*class = ATA_DEV_NONE;
		goto out;
	}

	writel(0x000F0000, port + K2_SATA_SCR_CONTROL_OFFSET);

	writeb(ATA_SRST, (void __iomem *) ap->ioaddr.ctl_addr);
	msleep(1);
	writeb(0x0, (void __iomem *) ap->ioaddr.ctl_addr);	
	msleep(1);
		
	memset(&tf, 0, sizeof(tf));

	ap->ops->tf_read(ap, &tf);
	*class = ata_dev_classify(&tf);

	if (*class == ATA_DEV_UNKNOWN)
		*class = ATA_DEV_NONE;

out:
	pp->do_port_srst = 1;
	return 0;
}

static unsigned int k2_sata_bus_softreset(struct ata_port *ap, unsigned int devmask)
{
	struct ata_ioports *ioaddr = &ap->ioaddr;

	if (ap->flags & ATA_FLAG_MMIO) {
		writeb(ap->ctl, (void __iomem *) ioaddr->ctl_addr);
		udelay(20);
		writeb(ap->ctl | ATA_SRST, (void __iomem *) ioaddr->ctl_addr);
		udelay(20);
		writeb(ap->ctl, (void __iomem *) ioaddr->ctl_addr);
	} else {
		outb(ap->ctl, ioaddr->ctl_addr);
		udelay(10);
		outb(ap->ctl | ATA_SRST, ioaddr->ctl_addr);
		udelay(10);
		outb(ap->ctl, ioaddr->ctl_addr);
	}

	msleep(150);
	return 0;
}

int k2_sata_std_softreset(struct ata_link *link, unsigned int *classes)
{
	struct ata_port *ap = link->ap;
	struct ata_device *dev = ap->link.device;
	struct ata_taskfile tf;	
	unsigned int devmask = 1<<(link->pmp), err_mask;

	if (ata_link_offline(link)) {
		classes[0] = ATA_DEV_NONE;
		goto out;
	}

	ap->ops->dev_select(ap, link->pmp);

	err_mask = k2_sata_bus_softreset(ap, devmask);
	if (err_mask) {
		ata_link_printk(link, KERN_ERR, "SRST failed (err_mask=0x%x)\n", err_mask);
		return -EIO;
	}

	ap->ops->dev_select(ap, link->pmp);
	memset(&tf, 0, sizeof(tf));
	ap->ops->tf_read(ap, &tf);	
	classes[0] = ata_dev_classify(&tf);

	if (classes[0] == ATA_DEV_UNKNOWN)
		classes[0] = ATA_DEV_NONE;
		
	if ((classes[0] == ATA_DEV_ATA) && (ata_chk_status(ap) == 0))
		classes[0] = ATA_DEV_NONE;

	dev->class = classes[0];
 out:
	return 0;
}

static int k2_sata_softreset(struct ata_link *link, unsigned int *class)
{
	return k2_sata_do_softreset(link, class, SATA_PMP_CTRL_PORT);
}

static int k2_sata_pmp_softreset(struct ata_link *link, unsigned int *class)
{
	struct ata_port *ap = link->ap;
	int rc = 0;

	if(link->pmp == 0xf)
		return k2_sata_do_softreset(link, class, link->pmp);
	else if(link->pmp < ap->nr_pmp_links) {
		if (ata_link_online(link)) {
			k2_sata_pmp_attach_link(link, link->pmp);
			rc = k2_sata_std_softreset(link, class);
		}
		else
			*class = ATA_DEV_NONE;
	}
	
	return rc;
}

static void k2_sata_error_handler(struct ata_port *ap)
{
	struct k2_port_priv *pp = ap->private_data;

	K2_POWER_ON(ap->host);

	if (pp->do_port_srst && !ap->nr_pmp_links) 
		ata_bmdma_error_handler(ap);
	else
	sata_pmp_do_eh(ap, ata_std_prereset, k2_sata_softreset,
			sata_pmp_std_hardreset, ata_std_postreset,
			sata_pmp_std_prereset, k2_sata_pmp_softreset,
			sata_pmp_std_hardreset, sata_pmp_std_postreset);
}

static inline struct ata_queued_cmd *k2_sata_get_qc(struct ata_port *ap)
{
	struct ata_queued_cmd *qc=NULL;
	void __iomem *port = (void __iomem *)ap->ioaddr.cmd_addr;
	struct ata_link *link;
	unsigned int pmp;

	if (ap->nr_pmp_links) {
		pmp = (readl(port + K2_SATA_SCR_CONTROL_OFFSET) & 0x000f0000)>>16;

		if (pmp < ap->nr_pmp_links) {
			link = &ap->pmp_link[pmp];
			qc = ata_qc_from_tag(ap, link->active_tag);
		} else {
			// fatal error here
			printk(KERN_ERR DRV_NAME ": out of range pmp %d\n", pmp);
		}
	} else {
		link = &ap->link;
		qc = ata_qc_from_tag(ap, link->active_tag);
	}
	
	return qc;
}

static irqreturn_t k2_sata_interrupt(int irq, void *dev_instance,
				   struct pt_regs *regs)
{
	struct ata_host *host = dev_instance;
	unsigned int i;
	unsigned int handled = 0;
	unsigned long flags;

	spin_lock_irqsave(&host->lock, flags);

	for (i = 0; i < host->n_ports; i++) {
		struct ata_port *ap;

		ap = host->ports[i];
		if (ap &&
		    !(ap->flags & ATA_FLAG_DISABLED)) {
			struct ata_queued_cmd *qc;

			qc = k2_sata_get_qc(ap);

			if (qc && (!(qc->tf.flags & ATA_TFLAG_POLLING)) &&
			    (qc->flags & ATA_QCFLAG_ACTIVE))
				handled |= ata_host_intr(ap, qc);
		}
	}

	spin_unlock_irqrestore(&host->lock, flags);
	return IRQ_RETVAL(handled);
}

#else /* ! defined(USE_QDMA) */

/*
 * QDMA-only functions
 */

#define PRD_TBL_BYTES (QDMA_RING_SIZE * LIBATA_MAX_PRD * \
	sizeof(struct k2_prd_entry))
#define CMD_RING_BYTES (QDMA_RING_SIZE * sizeof(struct k2_cmd_descr))

static int k2_qdma_port_start (struct ata_port *ap)
{
	struct device *dev = ap->dev;
	struct k2_port_priv *pp;
	int rc = -ENOMEM;

	pp = kmalloc(sizeof(*pp), GFP_KERNEL);
	if(! pp)
		goto nomem0;
	
	memset((void *)pp, 0, sizeof(*pp));
	ap->private_data = (void *)pp;

	pp->prd_tbl = dma_alloc_coherent(dev, PRD_TBL_BYTES, &pp->prd_tbl_dma,
		GFP_KERNEL);
	if(! pp->prd_tbl)
		goto nomem1;

	pp->cmd_ring = dma_alloc_coherent(dev, CMD_RING_BYTES,
		&pp->cmd_ring_dma, GFP_KERNEL);
	if(! pp->cmd_ring)
		goto nomem2;

	rc = ata_pad_alloc(ap, dev);
	if (rc)
		goto nomem3;

	return 0;

nomem3:
	dma_free_coherent(dev, CMD_RING_BYTES, pp->cmd_ring, pp->cmd_ring_dma);
nomem2:
	dma_free_coherent(dev, PRD_TBL_BYTES, pp->prd_tbl, pp->prd_tbl_dma);
nomem1:
	kfree(pp);
nomem0:
	return(rc);
}

static void k2_qdma_port_stop (struct ata_port *ap)
{
	struct device *dev = ap->dev;
	struct k2_port_priv *pp = ap->private_data;
	
	dma_free_coherent(dev, CMD_RING_BYTES, pp->cmd_ring, pp->cmd_ring_dma);
	dma_free_coherent(dev, PRD_TBL_BYTES, pp->prd_tbl, pp->prd_tbl_dma);
	kfree(pp);
	ata_pad_free(ap, dev);
}

static void k2_qdma_freeze(struct ata_port *ap)
{
	void __iomem *port_mmio = PORT_MMIO(ap);

	writel(0x0, port_mmio + K2_SATA_SIMR_OFFSET);
	writel(0x0, port_mmio + K2_SATA_QMR_OFFSET);
}

static void k2_qdma_thaw(struct ata_port *ap)
{
	void __iomem *port_mmio = PORT_MMIO(ap);

	writel(0x0, port_mmio + K2_SATA_QMR_OFFSET);
	writel(0, port_mmio + K2_SATA_SIMR_OFFSET);
	readl(port_mmio + K2_SATA_SIMR_OFFSET);	/* flush */
	writel(0x03ffffff, port_mmio + K2_SATA_SIMR_OFFSET);
	writel(0x21, port_mmio + K2_SATA_QMR_OFFSET);
}

static unsigned int k2_qdma_dev_classify(struct ata_port *ap)
{
	struct ata_taskfile tf;
	void __iomem *port_mmio = PORT_MMIO(ap);

	tf.lbah = readl(port_mmio + K2_SATA_TF_LBAH_OFFSET) & 0xff;
	tf.lbam = readl(port_mmio + K2_SATA_TF_LBAM_OFFSET) & 0xff;
	tf.lbal = readl(port_mmio + K2_SATA_TF_LBAL_OFFSET) & 0xff;
	tf.nsect = readl(port_mmio + K2_SATA_TF_NSECT_OFFSET) & 0xff;

	return(ata_dev_classify(&tf));
}

static int k2_qdma_reset(struct ata_link *link, unsigned int *class)
{
	struct ata_port *ap = link->ap;
	struct k2_port_priv *pp = ap->private_data;
	void __iomem *port_mmio = PORT_MMIO(link->ap);
	uint32_t ret;
	char *reason;
	int rc;

	writel(0x9, port_mmio + K2_SATA_QCR_OFFSET);
	mdelay(1);
	if(ata_link_online(link)) {
		// wait for reset acknowledge
		ret = ata_wait_register(port_mmio + K2_SATA_QSR_OFFSET,
			(1 << 3), 0, 1, 200);
		if(! (ret & (1 << 3))) {
			writel(0, port_mmio + K2_SATA_QCR_OFFSET);
			writel(readl(port_mmio + K2_SATA_QSR_OFFSET),
				port_mmio + K2_SATA_QSR_OFFSET);
			rc = -EIO;
			reason = "reset was not acknowledged";
			goto fail;
		}
	}

	writel(0, port_mmio + K2_SATA_QCR_OFFSET);
	writel(readl(port_mmio + K2_SATA_QSR_OFFSET),
		port_mmio + K2_SATA_QSR_OFFSET);

	writel(0, port_mmio + K2_SATA_QPI_OFFSET);
	writel(0, port_mmio + K2_SATA_QCI_OFFSET);
	writel(pp->cmd_ring_dma & 0xffffffff, port_mmio + K2_SATA_QAL_OFFSET);
	writel((pp->cmd_ring_dma >> 16) >> 16, port_mmio + K2_SATA_QAU_OFFSET);
	writel(QDMA_RING_SIZE - 1, port_mmio + K2_SATA_QDR_OFFSET);
	pp->cmd_idx = 0;

	if (ata_link_online(link)) {
		if (ata_busy_sleep(ap, ATA_TMOUT_BOOT_QUICK, ATA_TMOUT_BOOT)) {
			rc = -EIO;
			reason = "device not ready";
			goto fail;
		}
		*class = k2_qdma_dev_classify(ap);
	}
	writel(0x1f, port_mmio + K2_SATA_SCQR_OFFSET);
	return(0);

fail:
	ata_link_printk(link, KERN_ERR, "qdma reset failed (%s)\n", reason);
	return(rc);
}

static int k2_qdma_softreset(struct ata_link *link, unsigned int *class)
{
	int rc;
	void __iomem *port_mmio = PORT_MMIO(link->ap);

	*class = ATA_DEV_NONE;
	rc = k2_qdma_reset(link, class);

	if(! rc)
		writel(1, port_mmio + K2_SATA_QCR_OFFSET);  /* enable QDMA */
	
	return(rc);
}

static int k2_qdma_hardreset(struct ata_link *link, unsigned int *class)
{
	int rc;
	void __iomem *port_mmio = PORT_MMIO(link->ap);
	struct ata_port *ap = link->ap;
	struct ata_host *host = ap->host;

	/* shut off qdma */
	writel(0, port_mmio + K2_SATA_QCR_OFFSET);
	writel(readl(port_mmio + K2_SATA_QSR_OFFSET),
		port_mmio + K2_SATA_QSR_OFFSET);

	/* PHY reset */
	rc = sata_std_hardreset(link, class);
	if(rc)
		return(rc);
#ifdef CONFIG_MIPS_BRCM97XXX
	brcm_initsata2(link->ap->host->mmio_base, host->n_ports);
#endif
	/* reset and re-enable qdma, then probe device */
	return(k2_qdma_softreset(link, class));
}

static void k2_qdma_error_handler(struct ata_port *ap)
{
	ata_do_eh(ap, ata_std_prereset, k2_qdma_softreset, k2_qdma_hardreset,
		ata_std_postreset);
}

static void k2_qdma_error_intr(struct ata_port *ap, uint32_t irq_stat)
{
	void __iomem *port_mmio = PORT_MMIO(ap);
	uint32_t scr1 = readl(port_mmio + K2_SATA_SCR_ERROR_OFFSET);
	struct ata_eh_info *ehi = &ap->link.eh_info;
	unsigned int err_mask = 0, action = 0;
	struct ata_queued_cmd *qc;

	writel(scr1, port_mmio + K2_SATA_SCR_ERROR_OFFSET);

	ata_ehi_push_desc(ehi, "irq_stat 0x%08x, SCR1 0x%08x", irq_stat, scr1);

	if(irq_stat & QSR_ERR_HOST) {
		err_mask |= AC_ERR_HOST_BUS;
		action |= ATA_EH_SOFTRESET;
	}

	if(scr1 & SCR1_ERR_ATA) {
		err_mask |= AC_ERR_ATA_BUS;
		action |= ATA_EH_SOFTRESET;
		ata_ehi_push_desc(ehi, ", interface fatal error");
	}

	if(scr1 & SCR1_ERR_HOTPLUG) {
		ata_ehi_hotplugged(ehi);
	}

	if(scr1 & SCR1_ERR_FIS) {
		err_mask |= AC_ERR_HSM;
		action |= ATA_EH_SOFTRESET;
		ata_ehi_push_desc(ehi, ", unknown FIS");
	}

	ehi->serror |= scr1;
	ehi->action |= action;

	qc = ata_qc_from_tag(ap, ap->link.active_tag);
	if(qc)
		qc->err_mask |= err_mask;
	else
		ehi->err_mask |= err_mask;
	
	ata_port_freeze(ap);
}

static int k2_qdma_port_interrupt(struct ata_port *ap)
{
	struct k2_port_priv *pp = ap->private_data;
	int i;
	uint32_t act = 0, irq_stat;
	void __iomem *port_mmio = PORT_MMIO(ap);

	irq_stat = readl(port_mmio + K2_SATA_QSR_OFFSET);
	writel(irq_stat, port_mmio + K2_SATA_QSR_OFFSET);
	if(! irq_stat) {
		ata_port_printk(ap, KERN_WARNING, "spurious interrupt\n");
		return(1);
	}

	if(unlikely(irq_stat & QSR_ERR_ANY)) {
		k2_qdma_error_intr(ap, irq_stat);
		return(1);
	}
	
	/* find outstanding commands */
	if(ap->qc_active)
	{
		/* NCQ case: translate HW state into a bitmask */

		uint32_t sactive, qci, qci_test, qci2, qci2_test;
		int unseen = 0;

		/* reread until qci, qci2, and sactive are in sync */
		do {
			qci2_test = readl(port_mmio + K2_SATA_QCI2_OFFSET);
			qci_test = readl(port_mmio + K2_SATA_QCI_OFFSET);
			sactive = readl(port_mmio + K2_SATA_SCR_ACTIVE_OFFSET);
			qci2 = readl(port_mmio + K2_SATA_QCI2_OFFSET);
			qci = readl(port_mmio + K2_SATA_QCI_OFFSET);
		} while((qci_test != qci) || (qci2_test != qci2));

		/*
		 * We want to get the status of all outstanding cmds, in
		 * a format that libata can handle.
		 *
		 * qci2: (last command that the hardware has looked at) + 1
		 * qci: all commands < qci are complete
		 * qpi: same as cmd_idx (first empty entry in ring)
		 * sactive: bitmask of outstanding commands, starting
		 *   with qci and ending at qci2.  '1' = incomplete
		 *
		 * Sometimes there are commands past the end of sactive's
		 * range.  These are assumed to be outstanding since we have
		 * no way to know for sure.
		 */
		for(i = 0; ; i++)
		{
			int idx = (qci + i) & QDMA_RING_MASK;

			if(idx == pp->cmd_idx)
				break;
			if((idx == qci2) || (i == 32))
				unseen = 1;
			if(unseen || (sactive & (1 << (idx & 0x1f))))
				act |= 1 << pp->tags[idx];
		}
	} else {
		/* non-NCQ case: count outstanding cmds */
		uint32_t qci = readl(port_mmio + K2_SATA_QCI_OFFSET); 

		while(qci != pp->cmd_idx)
		{
			act |= (1 << pp->tags[qci]);
			qci = (qci + 1) & QDMA_RING_MASK;
		}
	}
	ata_qc_complete_multiple(ap, act, NULL);

	return(1);
}

static irqreturn_t k2_qdma_interrupt(int irq, void *dev_instance,
	struct pt_regs *regs)
{
	struct ata_host *host = dev_instance;
	uint32_t irq_stat;
	int i, ret = 0;

	irq_stat = readl(host->mmio_base + K2_SATA_GLOBAL_STATUS);
	if(! irq_stat)
		return(IRQ_NONE);

	spin_lock(&host->lock);

	for(i = 0; i < host->n_ports; i++) {
		if(! (irq_stat & (1 << i)))
			ret |= k2_qdma_port_interrupt(host->ports[i]);
	}

	spin_unlock(&host->lock);
	return(ret ? IRQ_HANDLED : IRQ_NONE);
}

static void k2_qdma_irq_clear(struct ata_port *ap)
{
	void __iomem *port_mmio = PORT_MMIO(ap);

	writel(readl(port_mmio + K2_SATA_SCR_ERROR_OFFSET),
		port_mmio + K2_SATA_SCR_ERROR_OFFSET);
	writel(readl(port_mmio + K2_SATA_QSR_OFFSET),
		port_mmio + K2_SATA_QSR_OFFSET);
}

#define FLOP(x) cpu_to_le32(x)

static inline int k2_fill_sg(struct ata_queued_cmd *qc, struct k2_prd_entry *prd)
{
	struct scatterlist *sg;
	int regions = 0;

	ata_for_each_sg(sg, qc) {
		dma_addr_t addr = sg_dma_address(sg);
		u32 sg_len = sg_dma_len(sg);

		prd->addr_lo = FLOP(addr & 0xffffffff);
		prd->addrhi_count = FLOP(((addr >> 16) & 0xffff0000) |
			sg_len);

		regions++;
		prd++;
	}
	BUG_ON(! regions);
	prd--;
	prd->addrhi_count |= FLOP(0x80000000);
	return(regions);
}

static void tf_to_cmd(struct ata_taskfile *tf, struct k2_cmd_descr *cmd)
{
	cmd->w4 = FLOP((tf->hob_feature << 24) | (tf->feature << 16) |
			(tf->device << 8) | (tf->command));
	cmd->w5 = FLOP((tf->hob_lbal << 24) | (tf->lbah << 16) |
			(tf->lbam << 8) | (tf->lbal));
	cmd->w6 = FLOP((tf->hob_nsect << 24) | (tf->nsect << 16) |
			(tf->hob_lbah << 8) | (tf->hob_lbam));
	cmd->w7 = FLOP(0);
}

static void tf_to_cmd_ncq(struct ata_taskfile *tf, struct k2_cmd_descr *cmd)
{
	cmd->w4 = FLOP((tf->device << 8) | (tf->command));
	cmd->w5 = FLOP((tf->hob_lbal << 24) | (tf->lbah << 16) |
			(tf->lbam << 8) | (tf->lbal));
	cmd->w6 = FLOP((tf->hob_feature << 24) | (tf->feature << 16) |
			(tf->hob_lbah << 8) | (tf->hob_lbam));
}

static void k2_qdma_qc_prep(struct ata_queued_cmd *qc)
{
	struct k2_port_priv *pp = qc->ap->private_data;
	struct ata_taskfile *tf = &qc->tf;

	uint32_t prd_offset = pp->cmd_idx * LIBATA_MAX_PRD *
		sizeof(struct k2_prd_entry);
	struct k2_prd_entry *prd = (void *)pp->prd_tbl + prd_offset;
	int sg_regions = 0;

	uint32_t dma_dir_flag = (qc->dma_dir == DMA_FROM_DEVICE) ? 0x200 : 0;
	struct k2_cmd_descr *cmd = &pp->cmd_ring[pp->cmd_idx];

	if(qc->flags & ATA_QCFLAG_DMAMAP)
		sg_regions = k2_fill_sg(qc, prd);

	prd_offset += (uint32_t)pp->prd_tbl_dma;

	switch(tf->protocol) {
		case ATA_PROT_NODATA:
			cmd->w0 = FLOP(0x00000102 | dma_dir_flag);
			cmd->w1 = FLOP(0);
			cmd->w2 = FLOP(prd_offset | 1);
			cmd->w3 = FLOP(0x00100000);
			tf_to_cmd(tf, cmd);
			break;
		case ATA_PROT_PIO:
			cmd->w0 = FLOP(0x00008100 | dma_dir_flag);
			cmd->w1 = FLOP(0);
			cmd->w2 = FLOP(prd_offset);
			cmd->w3 = FLOP(0);
			tf_to_cmd(tf, cmd);
			break;
		case ATA_PROT_DMA:
			cmd->w0 = FLOP(0x00000100 | dma_dir_flag);
			cmd->w1 = FLOP(0);
			cmd->w2 = FLOP(prd_offset);
			cmd->w3 = FLOP(0);
			tf_to_cmd(tf, cmd);
			break;
		case ATA_PROT_NCQ:
			cmd->w0 = FLOP(0x00000500 | dma_dir_flag);
			cmd->w1 = FLOP(((qc->tag) << 16) |
					(FLOP(prd->addrhi_count) >> 16));
			/* first sg region moves into cmd descriptor for NCQ */
			if(sg_regions > 1) {
				cmd->w2 = FLOP(prd_offset + sizeof(*prd));
				cmd->w3 = FLOP(FLOP(prd->addrhi_count) << 16);
				cmd->w7 = FLOP(FLOP(prd->addr_lo));
			} else {
				cmd->w2 = FLOP(0);
				cmd->w3 = FLOP(FLOP(prd->addrhi_count) << 16);
				cmd->w7 = FLOP(FLOP(prd->addr_lo) | 1);
			}
			tf_to_cmd_ncq(tf, cmd);
			break;

		/* TODO */
		case ATA_PROT_ATAPI:	/* ATAPI PIO */
			ata_port_printk(qc->ap, KERN_WARNING,
				"ATAPI PIO not supported\n");
			break;
		case ATA_PROT_ATAPI_NODATA:
			ata_port_printk(qc->ap, KERN_WARNING,
				"ATAPI NODATA not supported\n");
			break;
		case ATA_PROT_ATAPI_DMA:
			ata_port_printk(qc->ap, KERN_WARNING,
				"ATAPI DMA not supported\n");
			break;
		default:
			ata_port_printk(qc->ap, KERN_WARNING,
				"ATA protocol %d not supported\n",
				tf->protocol);
			break;
	}

	pp->tags[pp->cmd_idx] = qc->tag;
}

static unsigned int k2_qdma_qc_issue(struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;
	void __iomem *port_mmio = PORT_MMIO(ap);
	struct k2_port_priv *pp = ap->private_data;
	int idx = pp->cmd_idx;

	pp->cmd_idx = (idx + 1) & QDMA_RING_MASK;

	/* advance QPI to next empty entry */
	writel(pp->cmd_idx, port_mmio + K2_SATA_QPI_OFFSET);

	return(0);
}

#endif /* ! defined(USE_QDMA) */

#if defined(CONFIG_BRCM_PM)

/*
 * Power management
 */

static int k2_power_off(void *arg)
{
	struct ata_host *host = arg;
	int i, active = 0;
	long flags;

	spin_lock_irqsave(&sleep_lock, flags);
	if(SLEEP_FLAG(host) == K2_SLEEPING) {
		spin_unlock_irqrestore(&sleep_lock, flags);
		return(-1);
	}

	for(i = 0; i < host->n_ports; i++) {
		struct ata_port *ap;
		struct ata_link *link;
		struct ata_device *dev;

		ap = host->ports[i];

		/*
		 * dev->sdev is set to NULL in ata_scsi_slave_destroy()
		 * upon hot or warm unplug.  If it is non-NULL, the device
		 * is still enabled and it is unsafe to power off the core.
		 */
		ata_port_for_each_link(link, ap) {
			ata_link_for_each_dev(dev, link) {
				if(dev->sdev)
					active++;
			}
		}
	}

	if(active) {
		spin_unlock_irqrestore(&sleep_lock, flags);
		printk(KERN_INFO DRV_NAME
			": can't sleep with %d device(s) active\n", active);
		return(-1);
	} else {
		void __iomem *port_base;

		SET_SLEEP_FLAG(host, K2_SLEEPING);

		/* put all ports in Slumber mode */
		for(i = 0; i < host->n_ports; i++) {
			port_base = PORT_BASE(host->mmio_base, i);
			writel(readl(port_base + K2_SATA_SICR1_OFFSET) |
				(1 << 25), port_base + K2_SATA_SICR1_OFFSET);
			udelay(10);
			writel((readl(port_base + K2_SATA_SICR2_OFFSET) &
				~(7 << 18)) | (2 << 18),
				port_base + K2_SATA_SICR2_OFFSET);
			udelay(50);
			writel((readl(port_base + K2_SATA_SICR2_OFFSET) &
				~(7 << 18)) | (0 << 18),
				port_base + K2_SATA_SICR2_OFFSET);
		}

		disable_irq(host->irq);
		brcm_pm_sata_remove();
		spin_unlock_irqrestore(&sleep_lock, flags);

		return(0);
	}
}

static int k2_power_on(void *arg)
{
	struct ata_host *host = arg;
	void __iomem *port_base;
	int i;
	long flags;

	spin_lock_irqsave(&sleep_lock, flags);
	if(SLEEP_FLAG(host) == K2_AWAKE) {
		spin_unlock_irqrestore(&sleep_lock, flags);
		return(-1);
	}
	
	brcm_pm_sata_add();
	brcm_initsata2(host->mmio_base, host->n_ports);
	enable_irq(host->irq);

	/* wake up all ports */
	for(i = 0; i < host->n_ports; i++) {
		port_base = PORT_BASE(host->mmio_base, i);
		writel(readl(port_base + K2_SATA_SICR1_OFFSET) &
			~(1 << 25), port_base + K2_SATA_SICR1_OFFSET);
		udelay(10);
		writel((readl(port_base + K2_SATA_SICR2_OFFSET) &
			~(7 << 18)) | (4 << 18),
			port_base + K2_SATA_SICR2_OFFSET);
		udelay(50);
		writel((readl(port_base + K2_SATA_SICR2_OFFSET) &
			~(7 << 18)) | (0 << 18),
			port_base + K2_SATA_SICR2_OFFSET);
	}

	SET_SLEEP_FLAG(host, K2_AWAKE);
	spin_unlock_irqrestore(&sleep_lock, flags);

	return(0);
}

static int k2_sata_hp_poll(struct ata_port *ap)
{
	/* don't check for hotplug events while the HW is sleeping */
	if(SLEEP_FLAG(ap->host) == K2_SLEEPING)
		return(0);
	return(sata_std_hp_poll(ap));
}

static void k2_sata_remove_one(struct pci_dev *pdev)
{
	struct device *dev = pci_dev_to_dev(pdev);
	struct ata_host *host = dev_get_drvdata(dev);

	K2_POWER_ON(host);
	ata_pci_remove_one(pdev);

	brcm_pm_unregister_sata();
}

#endif /* CONFIG_BRCM_PM */

/*
 * Driver initialization
 */

static struct scsi_host_template k2_sata_sht = {
	.module			= THIS_MODULE,
	.name			= DRV_NAME,
	.ioctl			= ata_scsi_ioctl,
	.queuecommand		= ata_scsi_queuecmd,
#if defined(USE_QDMA) && defined(USE_NCQ)
	.can_queue		= ATA_MAX_QUEUE - 1,
	.change_queue_depth	= ata_scsi_change_queue_depth,
#else
	.can_queue		= ATA_DEF_QUEUE,
#endif
	.this_id		= ATA_SHT_THIS_ID,
	.sg_tablesize		= LIBATA_MAX_PRD,
	.cmd_per_lun		= ATA_SHT_CMD_PER_LUN,
	.emulated		= ATA_SHT_EMULATED,
	.use_clustering		= ATA_SHT_USE_CLUSTERING,
	.proc_name		= DRV_NAME,
	.dma_boundary		= ATA_DMA_BOUNDARY,
	.slave_configure	= ata_scsi_slave_config,
	.slave_destroy		= ata_scsi_slave_destroy,
#ifdef CONFIG_PPC_OF
	.proc_info		= k2_sata_proc_info,
#endif
	.bios_param		= ata_std_bios_param,
};


static const struct ata_port_operations k2_sata_ops = {
	.tf_read		= k2_sata_tf_read,
	.check_status		= k2_stat_check_status,
	.port_disable		= ata_port_disable,
	.hp_poll_activate	= sata_std_hp_poll_activate,
#if defined(CONFIG_BRCM_PM)
	.hp_poll		= k2_sata_hp_poll,
#else
	.hp_poll		= sata_std_hp_poll,
#endif
	.scr_read		= k2_sata_scr_read,
	.scr_write		= k2_sata_scr_write,

#if defined(USE_QDMA)
	.port_start		= k2_qdma_port_start,
	.port_stop		= k2_qdma_port_stop,
	.freeze			= k2_qdma_freeze,
	.thaw			= k2_qdma_thaw,
	.error_handler		= k2_qdma_error_handler,
	.irq_handler		= k2_qdma_interrupt,
	.irq_clear		= k2_qdma_irq_clear,
	.qc_prep		= k2_qdma_qc_prep,
	.qc_issue		= k2_qdma_qc_issue,
	.dev_select		= ata_noop_dev_select,
#else
	.port_start		= k2_sata_port_start,
	.port_stop		= ata_port_stop,
	.freeze			= ata_bmdma_freeze,
	.thaw			= ata_bmdma_thaw,
	.error_handler		= k2_sata_error_handler,
	.post_internal_cmd	= ata_bmdma_post_internal_cmd,
	.irq_handler		= k2_sata_interrupt,	
	.irq_clear		= ata_bmdma_irq_clear,
	.qc_prep		= ata_qc_prep,
	.qc_issue		= ata_qc_issue_prot,
	.dev_select		= k2_sata_dev_select,

	.bmdma_setup		= k2_bmdma_setup_mmio,
	.bmdma_start		= k2_bmdma_start_mmio,
	.bmdma_stop		= ata_bmdma_stop,
	.bmdma_status		= ata_bmdma_status,
	.tf_load		= k2_sata_tf_load,
	.exec_command		= ata_exec_command,
	.data_xfer		= ata_mmio_data_xfer,
	.host_stop		= ata_pci_host_stop,

	.pmp_attach		= k2_sata_pmp_attach,
	.pmp_detach		= k2_sata_pmp_detach,
	.pmp_read		= k2_sata_pmp_read,
	.pmp_write		= k2_sata_pmp_write,
#endif
};

static void k2_sata_setup_port(struct ata_ioports *port, unsigned long base)
{
	port->cmd_addr		= base + K2_SATA_TF_CMD_OFFSET;
	port->data_addr		= base + K2_SATA_TF_DATA_OFFSET;
	port->feature_addr	=
	port->error_addr	= base + K2_SATA_TF_ERROR_OFFSET;
	port->nsect_addr	= base + K2_SATA_TF_NSECT_OFFSET;
	port->lbal_addr		= base + K2_SATA_TF_LBAL_OFFSET;
	port->lbam_addr		= base + K2_SATA_TF_LBAM_OFFSET;
	port->lbah_addr		= base + K2_SATA_TF_LBAH_OFFSET;
	port->device_addr	= base + K2_SATA_TF_DEVICE_OFFSET;
	port->command_addr	=
	port->status_addr	= base + K2_SATA_TF_CMDSTAT_OFFSET;
	port->altstatus_addr	=
	port->ctl_addr		= base + K2_SATA_TF_CTL_OFFSET;
	port->bmdma_addr	= base + K2_SATA_DMA_CMD_OFFSET;
	port->scr_addr		= base + K2_SATA_SCR_STATUS_OFFSET;
}

static int k2_sata_init_one (struct pci_dev *pdev, const struct pci_device_id *ent)
{
	static int printed_version;
	struct ata_probe_ent *probe_ent = NULL;
	unsigned long base;
	void __iomem *mmio_base;
	int pci_dev_busy = 0;
	int rc;
	int i;

	if (!printed_version++)
		dev_printk(KERN_DEBUG, &pdev->dev, "version " DRV_VERSION "\n");

	/*
	 * If this driver happens to only be useful on Apple's K2, then
	 * we should check that here as it has a normal Serverworks ID
	 */
	rc = pci_enable_device(pdev);
	if (rc)
		return rc;
	/*
	 * Check if we have resources mapped at all (second function may
	 * have been disabled by firmware)
	 */
	if (pci_resource_len(pdev, 5) == 0)
		return -ENODEV;

	/* Request PCI regions */
	rc = pci_request_regions(pdev, DRV_NAME);
	if (rc) {
		pci_dev_busy = 1;
		goto err_out;
	}

	rc = pci_set_dma_mask(pdev, ATA_DMA_MASK);
	if (rc)
		goto err_out_regions;
	rc = pci_set_consistent_dma_mask(pdev, ATA_DMA_MASK);
	if (rc)
		goto err_out_regions;

	probe_ent = kmalloc(sizeof(*probe_ent), GFP_KERNEL);
	if (probe_ent == NULL) {
		rc = -ENOMEM;
		goto err_out_regions;
	}

	memset(probe_ent, 0, sizeof(*probe_ent));
	probe_ent->dev = pci_dev_to_dev(pdev);
	INIT_LIST_HEAD(&probe_ent->node);

#ifdef	CONFIG_MIPS_BCM7440	
	mmio_base = (void __iomem *)pci_resource_start(pdev, 5);
#else
	mmio_base = pci_iomap(pdev, 5, 0);
#endif
	if (mmio_base == NULL) {
		rc = -ENOMEM;
		goto err_out_free_ent;
	}
	base = (unsigned long) mmio_base;

	/* Clear a magic bit in SCR1 according to Darwin, those help
	 * some funky seagate drives (though so far, those were already
	 * set by the firmware on the machines I had access to)
	 */
	writel(readl(mmio_base + K2_SATA_SICR1_OFFSET) & ~0x00040000,
	       mmio_base + K2_SATA_SICR1_OFFSET);

	/* Clear SATA error & interrupts we don't use */
	writel(0xffffffff, mmio_base + K2_SATA_SCR_ERROR_OFFSET);
	writel(0x0, mmio_base + K2_SATA_SIMR_OFFSET);

	probe_ent->sht = &k2_sata_sht;

#if defined(USE_QDMA) && defined(USE_NCQ)
	probe_ent->port_flags = ATA_FLAG_SATA | ATA_FLAG_NO_LEGACY |
				ATA_FLAG_MMIO | ATA_FLAG_PIO_DMA |
				ATA_FLAG_NCQ;
#elif defined(USE_QDMA)
	probe_ent->port_flags = ATA_FLAG_SATA | ATA_FLAG_NO_LEGACY |
				ATA_FLAG_MMIO | ATA_FLAG_PIO_DMA;
#else
	probe_ent->port_flags = ATA_FLAG_SATA | ATA_FLAG_NO_LEGACY |
				ATA_FLAG_MMIO | ATA_FLAG_HP_POLLING;
#endif

	probe_ent->port_ops = &k2_sata_ops;

#ifdef CONFIG_MIPS_BRCM97XXX 	// jipeng - FIXME only 2 ports in 7038C0 SATA core
	probe_ent->n_ports = 2;
#else
	probe_ent->n_ports = 4;
#endif
	/* Unmask interrupts for all active ports */
	writel(~((1 << probe_ent->n_ports) - 1),
		mmio_base + K2_SATA_GLOBAL_MASK);
	
	probe_ent->irq = pdev->irq;
	probe_ent->irq_flags = IRQF_SHARED;
	probe_ent->mmio_base = mmio_base;

	/* We don't care much about the PIO/UDMA masks, but the core won't like us
	 * if we don't fill these
	 */
	probe_ent->pio_mask = 0x1f;
	probe_ent->mwdma_mask = 0x7;
	probe_ent->udma_mask = 0x7f;

	/* different controllers have different number of ports - currently 4 or 8 */
	/* All ports are on the same function. Multi-function device is no
	 * longer available. This should not be seen in any system. */
	for (i = 0; i < ent->driver_data; i++)
		k2_sata_setup_port(&probe_ent->port[i], base + i * K2_SATA_PORT_OFFSET);

	pci_set_master(pdev);

#ifdef CONFIG_MIPS_BRCM97XXX 
	bcm97xxx_sata_init(pdev, probe_ent);
#endif	

	probe_ent->private_data = 0;	/* aka K2_AWAKE */
	
	/* FIXME: check ata_device_add return value */
	ata_device_add(probe_ent);

#ifdef CONFIG_BRCM_PM
	/* arg is struct ata_host */
	brcm_pm_register_sata(k2_power_off, k2_power_on,
		dev_get_drvdata(probe_ent->dev));
#endif
	kfree(probe_ent);

	return 0;

err_out_free_ent:
	kfree(probe_ent);
err_out_regions:
	pci_release_regions(pdev);
err_out:
	if (!pci_dev_busy)
		pci_disable_device(pdev);
	return rc;
}

/* 0x240 is device ID for Apple K2 device
 * 0x241 is device ID for Serverworks Frodo4
 * 0x242 is device ID for Serverworks Frodo8 (same as 7038)
 * 0x24a is device ID for BCM5785 (aka HT1000) HT southbridge integrated SATA
 * controller
 * 0x8602 is device ID for BCM7400D0 SATA2
 *
 * driver_data element is number of ports (4x number of channels??)
 * */
static const struct pci_device_id k2_sata_pci_tbl[] = {
	{ PCI_VDEVICE(SERVERWORKS, 0x0240), 4 },
	{ PCI_VDEVICE(SERVERWORKS, 0x0241), 4 },
	{ PCI_VDEVICE(SERVERWORKS, PCI_DEVICE_ID_SERVERWORKS_BCM7038), 8 },
	{ PCI_VDEVICE(SERVERWORKS, 0x024a), 4 },
	{ PCI_VDEVICE(SERVERWORKS, 0x024b), 4 },
	{ PCI_VDEVICE(BROADCOM, PCI_DEVICE_ID_SERVERWORKS_BCM7400D0), 8 },
	{ }
};

static struct pci_driver k2_sata_pci_driver = {
	.name			= DRV_NAME,
	.id_table		= k2_sata_pci_tbl,
	.probe			= k2_sata_init_one,
#ifdef CONFIG_BRCM_PM
	.remove			= k2_sata_remove_one,
#else
	.remove			= ata_pci_remove_one,
#endif
};

static int __init k2_sata_init(void)
{
#ifdef CONFIG_MIPS_BRCM97XXX
	if(brcm_sata_enabled == 0)
		return(-ENODEV);
	brcm_pm_sata_add();
#endif
	return pci_register_driver(&k2_sata_pci_driver);
}

static void __exit k2_sata_exit(void)
{
	pci_unregister_driver(&k2_sata_pci_driver);
#ifdef CONFIG_MIPS_BRCM97XXX
	brcm_pm_sata_remove();
#endif
}

MODULE_AUTHOR("Benjamin Herrenschmidt");
MODULE_DESCRIPTION("low-level driver for K2 SATA controller");
MODULE_LICENSE("GPL");
MODULE_DEVICE_TABLE(pci, k2_sata_pci_tbl);
MODULE_VERSION(DRV_VERSION);

module_init(k2_sata_init);
module_exit(k2_sata_exit);
