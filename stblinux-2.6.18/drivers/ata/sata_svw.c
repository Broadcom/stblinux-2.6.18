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
 *	May 24, 2007	Jian Peng <jipeng@broadcom.com>
 *					support port multiplier and bump version up to 3.0
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
#include <linux/libata.h>

#ifdef CONFIG_PPC_OF
#include <asm/prom.h>
#include <asm/pci-bridge.h>
#endif /* CONFIG_PPC_OF */

#define DRV_NAME	"sata_svw"
#define DRV_VERSION	"3.0"

#ifdef CONFIG_MIPS_BRCM97XXX
#include <asm/brcmstb/common/brcmstb.h>
#endif

struct k2_port_priv {
	int do_port_srst;			/* already perform softreset? */
};

#ifdef CONFIG_MIPS_BRCM97XXX
#define WRITE_CMD			1
#define READ_CMD			2
#define CMD_DONE			(1 << 15)
#define SATA_MMIO			0x24
#define SATA_MMIO_SCR2  	0x48

// 1. port is SATA port ( 0 or 1)
// 2. reg is the address of the MDIO register ( see spec )
// 3. MMIO_BASE_ADDR  is MMIO base address from SATA PCI configuration
// registers addr 24-27
static uint16_t __devinit mdio_read_reg( int port, int reg)
{
	volatile unsigned int *mdio = (volatile unsigned int *) ((MMIO_OFS + 0x8c));
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

static void __devinit mdio_write_reg(int port, int reg, uint16_t val )
{
	volatile unsigned int *mdio = (volatile unsigned int  *) ((MMIO_OFS + 0x8c) /*+ (0x100 * port) */);
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

void DisablePHY(int port)
{
	uint32_t *pScr2 = (uint32_t *)(MMIO_OFS + 0x100*port + 0x48);
	writel(1, pScr2);
}

void EnablePHY(int port)
{
	uint32_t *pScr2 = (uint32_t *)(MMIO_OFS + 0x100*port + 0x48);
	writel(0, pScr2);
}

static void bcm_sg_workaround(int port)
{
	int tmp16;

	DisablePHY(port);

	//Change interpolation BW
	tmp16 = mdio_read_reg(port,9);

	mdio_write_reg(port,9,tmp16 | 1); //Bump up interpolation

	//Do analog reset
	tmp16 = mdio_read_reg(port,4);
	tmp16 |= 8;
	mdio_write_reg(port,4,tmp16);

	udelay( 1000 );      //wait 1 ms
	tmp16 &= 0xFFF7;
	mdio_write_reg(port,4,tmp16 ); // Take reset out
	udelay( 1000 );      //wait

	//Enable PHY
	EnablePHY(port);
}
#endif	// CONFIG_MIPS_BRCM97XXX	
	
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
	K2_SATA_SCR_STATUS_OFFSET	= 0x40,
	K2_SATA_SCR_ERROR_OFFSET	= 0x44,
	K2_SATA_SCR_CONTROL_OFFSET	= 0x48,

	/* Others */
	K2_SATA_SICR1_OFFSET		= 0x80,
	K2_SATA_SICR2_OFFSET		= 0x84,
	K2_SATA_SIM_OFFSET		= 0x88,

	/* Port stride */
	K2_SATA_PORT_OFFSET		= 0x100,
};

static u8 k2_stat_check_status(struct ata_port *ap);


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


static void k2_sata_tf_load(struct ata_port *ap, const struct ata_taskfile *tf)
{
	struct ata_ioports *ioaddr = &ap->ioaddr;
	unsigned int is_addr = tf->flags & ATA_TFLAG_ISADDR;

	if (tf->ctl != ap->last_ctl) {
		writeb(tf->ctl, (void *)ioaddr->ctl_addr);
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


static void k2_sata_tf_read(struct ata_port *ap, struct ata_taskfile *tf)
{
	struct ata_ioports *ioaddr = &ap->ioaddr;
	u16 nsect, lbal, lbam, lbah, feature;

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
	/* issue r/w command if the access is to ATA*/
	if (qc->tf.protocol == ATA_PROT_DMA)
		ap->ops->exec_command(ap, &qc->tf);
}


static u8 k2_stat_check_status(struct ata_port *ap)
{
       	return readl((void *) ap->ioaddr.status_addr);
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

void k2_sata_exec_command(struct ata_port *ap, const struct ata_taskfile *tf)
{
	if (tf->command != 0xEC && ap->ops->freeze)
	ap->ops->freeze(ap);

	ata_exec_command(ap, tf);

	if (tf->command != 0xEC && ap->ops->thaw)
	ap->ops->thaw(ap);
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
		rc = 1;
		return rc;
	} 

	scontrol = readl(port + K2_SATA_SCR_CONTROL_OFFSET);
	writel((scontrol & ~0x000f0000) | (pmp << 16), port + K2_SATA_SCR_CONTROL_OFFSET);
			
	if (!ata_link_offline(link)) {
		rc = sata_scr_write(link, SCR_ERROR, 0xffffffff);
		rc = 0;
	}
	else
		rc = 1;

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

void k2_sata_dev_select (struct ata_port *ap, unsigned int device)
{
	if(ap->nr_pmp_links)
	k2_sata_pmp_attach_link(&ap->link, device);
	else
	ata_std_dev_select(ap, device);	
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
	ap->ops->tf_load(ap, &tf);	// jipeng - is this enough?
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

	pp->do_port_srst = 1;
 out:
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
	sata_pmp_do_eh(ap, ata_std_prereset, k2_sata_softreset, sata_pmp_std_hardreset,
					ata_std_postreset, sata_pmp_std_prereset,
					k2_sata_pmp_softreset, sata_pmp_std_hardreset,
					sata_pmp_std_postreset);
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

static struct scsi_host_template k2_sata_sht = {
	.module			= THIS_MODULE,
	.name			= DRV_NAME,
	.ioctl			= ata_scsi_ioctl,
	.queuecommand		= ata_scsi_queuecmd,
	.can_queue		= ATA_DEF_QUEUE,
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
	.port_disable		= ata_port_disable,
	.tf_load		= k2_sata_tf_load,
	.tf_read		= k2_sata_tf_read,
	.check_status	= k2_stat_check_status,

	.exec_command	= ata_exec_command,
	.dev_select		= k2_sata_dev_select,

	.bmdma_setup	= k2_bmdma_setup_mmio,
	.bmdma_start	= k2_bmdma_start_mmio,
	.bmdma_stop		= ata_bmdma_stop,
	.bmdma_status		= ata_bmdma_status,
	.qc_prep		= ata_qc_prep,
	.qc_issue		= ata_qc_issue_prot,
	.data_xfer		= ata_mmio_data_xfer,

	.pmp_attach		= k2_sata_pmp_attach,
	.pmp_detach		= k2_sata_pmp_detach,
	.pmp_read		= k2_sata_pmp_read,
	.pmp_write		= k2_sata_pmp_write,

	.freeze			= ata_bmdma_freeze,
	.thaw			= ata_bmdma_thaw,
	.error_handler	= k2_sata_error_handler,
	
	.port_start		= k2_sata_port_start,
	.post_internal_cmd	= ata_bmdma_post_internal_cmd,
	.hp_poll_activate	= sata_std_hp_poll_activate,
	.hp_poll		= sata_std_hp_poll,
	.irq_handler	= k2_sata_interrupt,	

	.irq_clear		= ata_bmdma_irq_clear,
	.scr_read		= k2_sata_scr_read,
	.scr_write		= k2_sata_scr_write,

	.port_stop		= ata_port_stop,
	.host_stop		= ata_pci_host_stop,
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

#ifdef CONFIG_MIPS_BRCM97XXX /* Fix to recognize some WD models */
static void bcm97xxx_sata_init(struct pci_dev *dev, struct ata_probe_ent *probe_ent)
{
	unsigned int reg;
	
	if(dev->device == PCI_DEVICE_ID_SERVERWORKS_BCM7400B0)
	{
		volatile uint16_t tmp;
		volatile uint32_t tmp32;
		int port,i;

		//dump bridge regs
		for (i=0;i<9; i++) { //skip after this
			printk("Addr %x Value %x\n", 0xb0500200 + i * 4,
				*(volatile uint32_t *)(0xb0500200 + i * 4));
		}

		//program VCO step bit [12:10] start with 111
		mdio_write_reg(0,0x13,0x1c00);

		printk("mdio_write_reg(0,0x13,0x1c00) done\n");

		udelay(100000);

		//start pll tuner
		mdio_write_reg(0,0x13,0x1e00); // 

		printk("mdio_write_reg(0,0x13,0x1e00) done\n");
		udelay(10000); // wait

		printk("Checking lock \n");
		//check lock bit

		do {
			tmp = mdio_read_reg(0,0x7);
			printk("Wait for PLL lock mdio_read_reg(0,0x7) returns %08x\n", tmp);
		} while((tmp & 0x8000) != 0x8000);

		printk("PLL locked\n");
		//do analog reset
		mdio_write_reg(0,0x4,8);
		printk("mdio_write_reg(0,0x4,8) done\n");
		udelay(10000); // wait
		mdio_write_reg(0,0x4,0);

		printk("mdio_write_reg(0,0x4,0) done\n");
		for(port = 0; port < 2; port++)
		{
			printk("Reset port %d addr %x\n", port, MMIO_OFS + 0x48 + port*0x100);
			writel(1, (void *)(MMIO_OFS + 0x48 + port * 0x100));
			udelay(10000); // wait
			//reset deskew TX FIFO
			//1. select port
			mdio_write_reg(0,7,1 << port);
			//toggle reset bit
			mdio_write_reg(0,0xd,0x8000);
			udelay(10000); // wait
			mdio_write_reg(0,0xd,0x0000);
			udelay(10000); // wait
			writel(0, (void *)(MMIO_OFS + 0x48 + port * 0x100));

			//enable 4G support
			tmp32 = readl((void *)(MMIO_OFS + 0x84 + port * 0x100));
			writel(tmp32 | 0x00000400, (void *)(MMIO_OFS + 0x84 + port * 0x100));
		}
	}
	
	// For the BCM7038, let the PCI configuration in brcmpci_fixups.c hold.
	if (dev->device != PCI_DEVICE_ID_SERVERWORKS_BCM7038) 
	{
		/* force Master Latency Timer value to 64 PCICLKs */
		pci_write_config_byte(dev, PCI_LATENCY_TIMER, 0x40);
	}	
	else
    	{
        	int port;
	        uint32_t mmio_reg;

#ifdef CONFIG_MIPS_BCM7038
#define FIXED_REV       0x70380024      //BCM7038C4,  BCM7438A0 (0x7438_0000) would also pass the test
static volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;
#elif defined( CONFIG_MIPS_BCM7400 )
#define FIXED_REV       0x74000001      /****** FIX ME *********/
static volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;
#elif defined( CONFIG_MIPS_BCM7440 )
#define FIXED_REV       0
static volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;
#elif defined( CONFIG_MIPS_BCM7401 )
#define FIXED_REV       0x74010010      /****** FIX ME Done *********/
static volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;
#elif defined( CONFIG_MIPS_BCM7403 )
#define FIXED_REV       0x74030010      /****** FIX ME Done *********/
static volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;
#elif defined( CONFIG_MIPS_BCM7118 )
#define FIXED_REV       0
static volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;
#endif

	printk("SUNDRY revision = %08lx\n", *pSundryRev);

        if (*pSundryRev >= FIXED_REV) {
			/*
			* Disable the port.
			* The phy for a port can be disabled by setting bit 0 of register
			* at offset 0x48 in the ports MMIO space
			*/
			(void) pci_read_config_dword(dev, SATA_MMIO, &mmio_reg);
			mmio_reg = KSEG1ADDR(mmio_reg);
			reg = readl((void *)(mmio_reg+SATA_MMIO_SCR2));
			writel(reg | 0x01, (void *)(mmio_reg+SATA_MMIO_SCR2));

			/*
			* Before accessing the MDIO registers through pci space disable external MDIO access.
			* write MDIO register at offset 0x07 with (1 << port number) where port number starts at 0.
			* Read MDIO register at offset 0x0D into variable reg.
			- reg_0d = reg_0d | 0x04
			- Write reg_0d to MDIO register at offset 0x0D.
			*/
			for (port = 0; port < probe_ent->n_ports; port++) {
				// Choose the port
				//mdio_write_reg(port, 7, 1<<port);
				// Read reg 0xd
				reg = mdio_read_reg(port, 0xd);
				printk("MDIO Read port%d: %04x\n", port, reg);
				reg |= 0x4;
				mdio_write_reg(port, 0xd, reg);
				reg = mdio_read_reg(port, 0xd);
				printk("MDIO Read2 port%d: %04x\n", port, reg);
			}
			// Re-enable the PHY
			reg = readl((void *)(mmio_reg+SATA_MMIO_SCR2));
			writel(reg ^ 0x01, (void *)(mmio_reg+SATA_MMIO_SCR2));
		}
	
		//PR22401: Identify Seagate drives with ST controllers.
		{
			int port;
	
			for (port=0; port < probe_ent->n_ports; port++)
			bcm_sg_workaround(port);
		}
	}
}
#endif	// CONFIG_MIPS_BRCM97XXX

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

	mmio_base = pci_iomap(pdev, 5, 0);
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
	writel(0x0, mmio_base + K2_SATA_SIM_OFFSET);

	probe_ent->sht = &k2_sata_sht;
	probe_ent->port_flags = ATA_FLAG_SATA | ATA_FLAG_NO_LEGACY |
				ATA_FLAG_MMIO | ATA_FLAG_HP_POLLING;
	probe_ent->port_ops = &k2_sata_ops;

#ifdef CONFIG_MIPS_BRCM97XXX 	// jipeng - FIXME we enabled only 2 ports in 7038C0 SATA core
	probe_ent->n_ports = 2;
#else
	probe_ent->n_ports = 4;
#endif
	
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
	
	/* FIXME: check ata_device_add return value */
	ata_device_add(probe_ent);
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
 * 0x8602 is device ID for BCM7400B0 SATA2
 *
 * driver_data element is number of ports (4x number of channels??)
 * */
static const struct pci_device_id k2_sata_pci_tbl[] = {
	{ PCI_VDEVICE(SERVERWORKS, 0x0240), 4 },
	{ PCI_VDEVICE(SERVERWORKS, 0x0241), 4 },
	{ PCI_VDEVICE(SERVERWORKS, PCI_DEVICE_ID_SERVERWORKS_BCM7038), 8 },
	{ PCI_VDEVICE(SERVERWORKS, 0x024a), 4 },
	{ PCI_VDEVICE(SERVERWORKS, 0x024b), 4 },
	{ PCI_VDEVICE(BROADCOM, PCI_DEVICE_ID_SERVERWORKS_BCM7400B0), 8 },
	{ }
};

static struct pci_driver k2_sata_pci_driver = {
	.name			= DRV_NAME,
	.id_table		= k2_sata_pci_tbl,
	.probe			= k2_sata_init_one,
	.remove			= ata_pci_remove_one,
};

static int __init k2_sata_init(void)
{
	return pci_register_driver(&k2_sata_pci_driver);
}

static void __exit k2_sata_exit(void)
{
	pci_unregister_driver(&k2_sata_pci_driver);
}

MODULE_AUTHOR("Benjamin Herrenschmidt");
MODULE_DESCRIPTION("low-level driver for K2 SATA controller");
MODULE_LICENSE("GPL");
MODULE_DEVICE_TABLE(pci, k2_sata_pci_tbl);
MODULE_VERSION(DRV_VERSION);

module_init(k2_sata_init);
module_exit(k2_sata_exit);
