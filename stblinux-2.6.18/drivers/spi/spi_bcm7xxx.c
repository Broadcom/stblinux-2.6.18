/*---------------------------------------------------------------------------

    Copyright (c) 2008 Broadcom Corporation                      /\
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

    File: spi_bcm7xxx.c

    Description: 
    Driver for BCM7xxx/BCM35xx MSPI controller

    when        who         what
    -----       ---         ----
    20080430    cernekee    initial version
 ------------------------------------------------------------------------- */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/spi/spi.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <asm/brcmstb/common/brcmstb.h>
#include <asm/brcmstb/common/bcmspi.h>

#if 0
#define DBG printk
#else
#define DBG(...) /* */
#endif

#define NUM_CHIPSELECT		4
#define MSPI_BASE_FREQ		XTALFREQ
#define SPBR_MIN		8U
#define SPBR_MAX		255U
#define MAX_SPEED_HZ		(MSPI_BASE_FREQ / (SPBR_MIN * 2))
#define BSPI_BASE_ADDR		KSEG1ADDR(0x1f000000)

const struct bcmspi_parms bcmspi_default_parms_cs0 = {
	.speed_hz		= MAX_SPEED_HZ,
	.chip_select		= 0,
	.mode			= SPI_MODE_3,
	.bits_per_word		= 8,
};
EXPORT_SYMBOL(bcmspi_default_parms_cs0);

const struct bcmspi_parms bcmspi_default_parms_cs1 = {
	.speed_hz		= MAX_SPEED_HZ,
	.chip_select		= 1,
	.mode			= SPI_MODE_3,
	.bits_per_word		= 8,
};
EXPORT_SYMBOL(bcmspi_default_parms_cs1);

struct position {
	struct spi_message	*msg;
	struct spi_transfer	*trans;
	int			byte;
};

#define NUM_TXRAM		32
#define NUM_RXRAM		32
#define NUM_CDRAM		16

struct bcm_mspi_hw {
	u32			spcr0_lsb;		/* 0x000 */
	u32			spcr0_msb;		/* 0x004 */
	u32			spcr1_lsb;		/* 0x008 */
	u32			spcr1_msb;		/* 0x00c */
	u32			newqp;			/* 0x010 */
	u32			endqp;			/* 0x014 */
	u32			spcr2;			/* 0x018 */
	u32			reserved0;		/* 0x01c */
	u32			mspi_status;		/* 0x020 */
	u32			cptqp;			/* 0x024 */
	u32			reserved1[6];		/* 0x028 */
	u32			txram[NUM_TXRAM];	/* 0x040 */
	u32			rxram[NUM_RXRAM];	/* 0x0c0 */
	u32			cdram[NUM_CDRAM];	/* 0x140 */
};

struct bcm_bspi_hw {
	u32			revision_id;		/* 0x000 */
	u32			scratch;		/* 0x004 */
	u32			mast_n_boot_ctrl;	/* 0x008 */
	u32			busy_status;		/* 0x00c */
	u32			intr_status;		/* 0x010 */
	u32			b0_status;		/* 0x014 */
	u32			b0_ctrl;		/* 0x018 */
	u32			b1_status;		/* 0x01c */
	u32			b1_ctrl;		/* 0x020 */
	u32			strap_override_ctrl;	/* 0x024 */
};

#define STATE_IDLE		0
#define STATE_RUNNING		1
#define STATE_SHUTDOWN		2

struct bcmspi_priv {
	struct platform_device	*pdev;
	struct spi_master	*master;
	spinlock_t		lock;
	struct bcmspi_parms	last_parms;
	struct position		pos;
	struct list_head	msg_queue;
	int			state;
	int			outstanding_bytes;
	int			next_udelay;
	int			cs_change;
	unsigned int		max_speed_hz;
	volatile struct bcm_mspi_hw *mspi_hw;
	volatile struct bcm_bspi_hw *bspi_hw;
	int			irq;
	struct tasklet_struct	tasklet;
	wait_queue_head_t	idle_wait;
	int			bspi_enabled;
	int			bspi_chip_select;
};

static void bcmspi_disable_bspi(struct bcmspi_priv *priv)
{
	int i;

	if(! priv->bspi_hw || ! priv->bspi_enabled)
		return;
	if((priv->bspi_hw->mast_n_boot_ctrl & 1) == 1)
		return;

	DBG("disabling bspi\n");
	for(i = 0; i < 1000; i++) {
		if((priv->bspi_hw->busy_status & 1) == 0) {
			priv->bspi_hw->mast_n_boot_ctrl = 1;
			priv->bspi_enabled = 0;
			udelay(1);
			return;
		}
		udelay(1);
	}
	dev_warn(&priv->pdev->dev, "timeout setting MSPI mode\n");
}

static void bcmspi_enable_bspi(struct bcmspi_priv *priv)
{
	if(! priv->bspi_hw || priv->bspi_enabled)
		return;
	if((priv->bspi_hw->mast_n_boot_ctrl & 1) == 0)
		return;

	DBG("enabling bspi\n");
	/* flush prefetch buffers */
	priv->bspi_hw->b0_ctrl = 1;
	priv->bspi_hw->b1_ctrl = 1;
	udelay(1);
	priv->bspi_hw->mast_n_boot_ctrl = 0;
	priv->bspi_enabled = 1;
}

static void bcmspi_hw_set_parms(struct bcmspi_priv *priv,
	const struct bcmspi_parms *xp)
{
	if(xp->speed_hz) {
		unsigned int spbr = MSPI_BASE_FREQ / (2 * xp->speed_hz);

		priv->mspi_hw->spcr0_lsb = max(min(spbr, SPBR_MAX), SPBR_MIN);
	} else {
		priv->mspi_hw->spcr0_lsb = SPBR_MIN;
	}
	priv->mspi_hw->spcr0_msb = 0x80 |	// MSTR bit
		((xp->bits_per_word ? xp->bits_per_word : 8) << 2) |
		(xp->mode & 3);
	priv->last_parms = *xp;
}

#define PARMS_NO_OVERRIDE		0
#define PARMS_OVERRIDE			1

static int bcmspi_update_parms(struct bcmspi_priv *priv, struct spi_device *spidev,
	struct spi_transfer *trans, int override)
{
	struct bcmspi_parms xp;

	xp.speed_hz = min(trans->speed_hz ? trans->speed_hz :
		(spidev->max_speed_hz ? spidev->max_speed_hz : MAX_SPEED_HZ),
		MAX_SPEED_HZ);
	xp.chip_select = spidev->chip_select;
	xp.mode = spidev->mode;
	xp.bits_per_word = trans->bits_per_word ? trans->bits_per_word :
		(spidev->bits_per_word ? spidev->bits_per_word : 8);
	
	if((override == PARMS_OVERRIDE) ||
		((xp.speed_hz == priv->last_parms.speed_hz) &&
		 (xp.chip_select == priv->last_parms.chip_select) &&
		 (xp.mode == priv->last_parms.mode) &&
		 (xp.bits_per_word == priv->last_parms.bits_per_word))) {
		bcmspi_hw_set_parms(priv, &xp);
		return(0);
	}
	/* no override, and parms do not match */
	return(1);
}


static int bcmspi_setup(struct spi_device *spi)
{
	struct bcmspi_parms *xp;
	struct bcmspi_priv *priv = spi_master_get_devdata(spi->master);

	DBG("%s\n", __FUNCTION__);

	if(spi->bits_per_word > 16)
		return(-EINVAL);

	xp = spi_get_ctldata(spi);
	if(! xp) {
		xp = kzalloc(sizeof(struct bcmspi_parms), GFP_KERNEL);
		if(! xp)
			return(-ENOMEM);
		spi_set_ctldata(spi, xp);
	}
	if(spi->max_speed_hz < priv->max_speed_hz)
		xp->speed_hz = spi->max_speed_hz;
	else
		xp->speed_hz = 0;

	xp->chip_select = spi->chip_select;
	xp->mode = spi->mode;
	xp->bits_per_word = spi->bits_per_word ? spi->bits_per_word : 8;

	return(0);
}

#define FNB_BREAK_NONE			0
#define FNB_BREAK_EOM			1	/* stop at end of spi_message */
#define FNB_BREAK_DELAY			2	/* stop at end of spi_transfer if delay */
#define FNB_BREAK_CS_CHANGE		4	/* stop at end of spi_transfer if cs_change */
#define FNB_BREAK_NO_BYTES		8	/* stop if we run out of bytes */

/* events that make us stop filling TX slots */
#define FNB_BREAK_TX			(FNB_BREAK_EOM | FNB_BREAK_DELAY | \
					 FNB_BREAK_CS_CHANGE)

/* events that make us deassert CS */
#define FNB_BREAK_DESELECT		(FNB_BREAK_EOM | FNB_BREAK_CS_CHANGE)


static int find_next_byte(struct bcmspi_priv *priv, struct position *p,
	struct list_head *completed, int flags)
{
	int ret = FNB_BREAK_NONE;

	p->byte++;

	while(p->byte >= p->trans->len) {
		/* we're at the end of the spi_transfer */

		/* in TX mode, need to pause for a delay or CS change */
		if(p->trans->delay_usecs && (flags & FNB_BREAK_DELAY))
			ret |= FNB_BREAK_DELAY;
		if(p->trans->cs_change && (flags & FNB_BREAK_CS_CHANGE))
			ret |= FNB_BREAK_CS_CHANGE;
		if(ret)
			return(ret);

		/* advance to next spi_message? */
		if(list_is_last(&p->trans->transfer_list, &p->msg->transfers)) {
			struct spi_message *next_msg = NULL;

			/* TX breaks at the end of each message as well */
			if(!completed || (flags & FNB_BREAK_EOM)) {
				DBG("find_next_byte: advance msg exit\n");
				return(FNB_BREAK_EOM);
			}
			if(! list_is_last(&p->msg->queue, &priv->msg_queue)) {
				next_msg = list_entry(p->msg->queue.next,
					struct spi_message, queue);
			}
			/* delete from run queue, add to completion queue */
			list_del(&p->msg->queue);
			list_add_tail(&p->msg->queue, completed);

			p->msg = next_msg;
			p->byte = 0;
			if(p->msg == NULL) {
				p->trans = NULL;
				ret = FNB_BREAK_NO_BYTES;
				break;
			}

			/*
			 * move on to the first spi_transfer of the new
			 * spi_message
			 */
			p->trans = list_entry(p->msg->transfers.next,
				struct spi_transfer, transfer_list);
		} else {
			/* or just advance to the next spi_transfer */
			p->trans = list_entry(p->trans->transfer_list.next,
				struct spi_transfer, transfer_list);
			p->byte = 0;
		}
	}
	DBG("find_next_byte: msg %p trans %p len %d byte %d ret %x\n",
		p->msg, p->trans, p->trans ? p->trans->len : 0, p->byte, ret);
	return(ret);
}

static void read_from_hw(struct bcmspi_priv *priv, struct list_head *completed)
{
	struct position p;
	int slot = 0, n = priv->outstanding_bytes;

	DBG("%s\n", __FUNCTION__);

	p = priv->pos;

	while(n > 0) {
		BUG_ON(p.msg == NULL);

		if(p.trans->bits_per_word <= 8) {
			u8 *buf = p.trans->rx_buf;

			if(buf) {
				buf[p.byte] =
					priv->mspi_hw->rxram[(slot << 1) + 1]
						& 0xff;
			}
			DBG("RD %02x\n", buf ? buf[p.byte] : 0xff);
		} else {
			u16 *buf = p.trans->rx_buf;

			if(buf) {
				buf[p.byte] =
					((priv->mspi_hw->rxram[(slot << 1) + 1]
						& 0xff) << 0) |
					((priv->mspi_hw->rxram[(slot << 1) + 0]
						& 0xff) << 8);
			}
		}
		slot++;
		n--;
		p.msg->actual_length++;

		find_next_byte(priv, &p, completed, FNB_BREAK_NONE);
	}

	priv->pos = p;
	priv->outstanding_bytes = 0;
}

static void write_to_hw(struct bcmspi_priv *priv)
{
	struct position p;
	int slot = 0, fnb = 0;
	struct spi_message *msg = NULL;

	DBG("%s\n", __FUNCTION__);

	bcmspi_disable_bspi(priv);

	p = priv->pos;

	while(1) {
		if(p.msg == NULL)
			break;
		if(! msg) {
			msg = p.msg;
			bcmspi_update_parms(priv, msg->spi, p.trans,
				PARMS_OVERRIDE);
		} else {
			/* break if the speed, bits, etc. changed */
			if(bcmspi_update_parms(priv, msg->spi, p.trans,
				PARMS_NO_OVERRIDE)) {
				DBG("parms don't match, breaking\n");
				break;
			}
		}
		if(p.trans->bits_per_word <= 8) {
			const u8 *buf = p.trans->tx_buf;

			priv->mspi_hw->txram[slot << 1] = buf ?
				(buf[p.byte] & 0xff) : 0xff;
			DBG("WR %02x\n", buf ? buf[p.byte] : 0xff);
		} else {
			const u16 *buf = p.trans->tx_buf;

			priv->mspi_hw->txram[(slot << 1) + 0] = buf ?
				(buf[p.byte] >> 8) : 0xff;
			priv->mspi_hw->txram[(slot << 1) + 1] = buf ?
				(buf[p.byte] & 0xff) : 0xff;
		}
		priv->mspi_hw->cdram[slot] =
			((p.trans->bits_per_word <= 8) ? 0x8f : 0xcf) ^
			(1 << msg->spi->chip_select);
		slot++;

		fnb = find_next_byte(priv, &p, NULL, FNB_BREAK_TX);

		if(fnb & FNB_BREAK_CS_CHANGE)
			priv->cs_change = 1;
		if(fnb & FNB_BREAK_DELAY)
			priv->next_udelay = p.trans->delay_usecs;
		if(fnb || (slot == NUM_CDRAM))
			break;
	}
	if(slot) {
		DBG("submitting %d slots\n", slot);
		priv->mspi_hw->newqp = 0;
		priv->mspi_hw->endqp = slot - 1;

		/* deassert CS on the final byte */
		if(fnb & FNB_BREAK_DESELECT)
			priv->mspi_hw->cdram[slot - 1] &= ~0x80;
		mb();

		priv->mspi_hw->spcr2 = 0xe0;	// cont | spe | spifie

		priv->state = STATE_RUNNING;
		priv->outstanding_bytes = slot;
	} else {
		priv->state = STATE_IDLE;
		wake_up(&priv->idle_wait);
	}
}

static int bcmspi_emulate_flash_read(struct bcmspi_priv *priv,
	struct spi_message *msg)
{
	struct spi_transfer *trans;
	u32 addr, len;
	u8 *buf, *src;
	unsigned long flags;

	/* acquire lock when the MSPI is idle */
	while(1) {
		spin_lock_irqsave(&priv->lock, flags);
		if(priv->state == STATE_IDLE)
			break;
		if(priv->state == STATE_SHUTDOWN) {
			spin_unlock_irqrestore(&priv->lock, flags);
			return(-EIO);
		}
		spin_unlock_irqrestore(&priv->lock, flags);
		sleep_on(&priv->idle_wait);
	}
	bcmspi_enable_bspi(priv);

	/* first transfer - OPCODE_READ + 3-byte address */
	trans = list_entry(msg->transfers.next, struct spi_transfer,
		transfer_list);
	buf = (void *)trans->tx_buf;
	addr = (buf[1] << 16) | (buf[2] << 8) | buf[3];

	/*
	 * addr coming into this function was remapped by the m25p80 driver
	 * we need to revert it back to the BSPI address
	 */
	addr = (addr + 0xc00000) & 0xffffff;

	/* second transfer - read result into buffer */
	trans = list_entry(msg->transfers.next->next, struct spi_transfer,
		transfer_list);

	buf = (void *)trans->rx_buf;
	len = trans->len;
	src = (u8 *)BSPI_BASE_ADDR + addr;

	DBG("emulate_read: dst %p src %p len %x addr %x\n",
		buf, src, len, addr);

	if(! ((((unsigned long)buf) & 3) || (addr & 3))) {
		/* copy aligned words */
		while(len >= 4) {
			*((u32 *)buf) = *((u32 *)src);
			buf += 4;
			src += 4;
			len -= 4;
		}
	}
	/* copy bytes */
	while(len > 0) {
		*(buf++) = *(src++);
		len--;
	}

	msg->actual_length = 4 + trans->len;
	msg->complete(msg->context);

	spin_unlock_irqrestore(&priv->lock, flags);

	return(0);
}

/*
 * m25p80_read() calls wait_till_ready() before each read to check
 * the flash status register for pending writes.
 *
 * This can be safely skipped if our last transaction was just an
 * emulated BSPI read.
 */
static int bcmspi_emulate_flash_rdsr(struct bcmspi_priv *priv,
	struct spi_message *msg)
{
	u8 *buf;
	struct spi_transfer *trans;

	if(priv->bspi_enabled == 0)
		return(1);
	
	trans = list_entry(msg->transfers.next->next, struct spi_transfer,
		transfer_list);

	buf = (void *)trans->rx_buf;
	*buf = 0x00;
	
	msg->actual_length = 2;
	msg->complete(msg->context);

	return(0);
}

#define OPCODE_READ		0x03
#define OPCODE_RDSR		0x05

static int bcmspi_transfer(struct spi_device *spi, struct spi_message *msg)
{
	struct bcmspi_priv *priv = spi_master_get_devdata(spi->master);
	unsigned long flags;

	DBG("%s\n", __FUNCTION__);

	if(msg->spi->chip_select == priv->bspi_chip_select) {
		struct spi_transfer *trans;

		trans = list_entry(msg->transfers.next,
			struct spi_transfer, transfer_list);
		if(trans && trans->len && trans->tx_buf &&
		   (((u8 *)trans->tx_buf)[0] == OPCODE_READ)) {
			if(bcmspi_emulate_flash_read(priv, msg) == 0)
				return(0);
		}
		if(trans && trans->len && trans->tx_buf &&
		   (((u8 *)trans->tx_buf)[0] == OPCODE_RDSR)) {
			if(bcmspi_emulate_flash_rdsr(priv, msg) == 0)
				return(0);
		}
	}
	
	spin_lock_irqsave(&priv->lock, flags);

	if(priv->state == STATE_SHUTDOWN) {
		spin_unlock_irqrestore(&priv->lock, flags);
		return(-EIO);
	}

	msg->actual_length = 0;

	list_add_tail(&msg->queue, &priv->msg_queue);

	if(priv->state == STATE_IDLE) {
		BUG_ON(priv->pos.msg != NULL);
		priv->pos.msg = msg;
		priv->pos.trans = list_entry(msg->transfers.next,
			struct spi_transfer, transfer_list);
		priv->pos.byte = 0;

		write_to_hw(priv);
	}
	spin_unlock_irqrestore(&priv->lock, flags);

	return(0);
}

static void bcmspi_cleanup(const struct spi_device *spi)
{
	struct spi_device *spi2 = (struct spi_device *)spi;
	struct bcmspi_parms *xp = spi_get_ctldata(spi2);

	DBG("%s\n", __FUNCTION__);

	kfree(xp);
}

static irqreturn_t bcmspi_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct bcmspi_priv *priv = dev_id;

	DBG("%s\n", __FUNCTION__);

	if(priv->mspi_hw->mspi_status & 1) {
		/* clear interrupt */
		priv->mspi_hw->mspi_status &= ~1;

		tasklet_schedule(&priv->tasklet);
		return(IRQ_HANDLED);
	} else {
		return(IRQ_NONE);
	}
}

static void bcmspi_tasklet(unsigned long param)
{
	struct bcmspi_priv *priv = (void *)param;
	struct list_head completed;
	struct spi_message *msg;
	unsigned long flags;

	DBG("%s\n", __FUNCTION__);

	INIT_LIST_HEAD(&completed);
	spin_lock_irqsave(&priv->lock, flags);

	if(priv->next_udelay) {
		udelay(priv->next_udelay);
		priv->next_udelay = 0;
	}

	msg = priv->pos.msg;

	read_from_hw(priv, &completed);
	if(priv->cs_change) {
		udelay(10);
		priv->cs_change = 0;
	}

	write_to_hw(priv);
	spin_unlock_irqrestore(&priv->lock, flags);

	while (!list_empty(&completed)) {
		msg = list_entry(completed.next, struct spi_message, queue);
		list_del(&msg->queue);
		msg->status = 0;
		if (msg->complete)
			msg->complete(msg->context);
	}
}

static void bcmspi_complete(void *arg)
{
	complete(arg);
}

static struct spi_master *default_master = NULL;

int bcmspi_simple_transaction(struct bcmspi_parms *xp,
	const void *tx_buf, int tx_len, void *rx_buf, int rx_len)
{
	DECLARE_COMPLETION_ONSTACK(fini);
	struct spi_message m;
	struct spi_transfer t_tx, t_rx;
	struct spi_device spi;
	int ret;

	memset(&spi, 0, sizeof(spi));
	spi.max_speed_hz = xp->speed_hz;
	spi.chip_select = xp->chip_select;
	spi.mode = xp->mode;
	spi.bits_per_word = xp->bits_per_word;
	spi.master = default_master;

	spi_message_init(&m);
	m.complete = bcmspi_complete;
	m.context = &fini;
	m.spi = &spi;

	memset(&t_tx, 0, sizeof(t_tx));
	memset(&t_rx, 0, sizeof(t_rx));
	t_tx.tx_buf = tx_buf;
	t_tx.len = tx_len;
	t_rx.rx_buf = rx_buf;
	t_rx.len = rx_len;

	if(tx_len)
		spi_message_add_tail(&t_tx, &m);
	if(rx_len)
		spi_message_add_tail(&t_rx, &m);

	ret = bcmspi_transfer(&spi, &m);
	if(! ret)
		wait_for_completion(&fini);
	return(ret);
}
EXPORT_SYMBOL(bcmspi_simple_transaction);

static void bcmspi_hw_init(struct bcmspi_priv *priv)
{
	priv->mspi_hw->spcr1_lsb = 0;
	priv->mspi_hw->spcr1_msb = 0;
	priv->mspi_hw->newqp = 0;
	priv->mspi_hw->endqp = 0;
	priv->mspi_hw->spcr2 = 0x20;	// spifie

	bcmspi_hw_set_parms(priv, &bcmspi_default_parms_cs0);

	priv->bspi_enabled = 1;
	bcmspi_disable_bspi(priv);

	/* enable L2 UPG SPI interrupt */
#ifdef BCHP_IRQ0_SPI_IRQEN
	BDEV_SET(BCHP_IRQ0_SPI_IRQEN, BCHP_IRQ0_SPI_IRQEN_spi_MASK);
#else
	BDEV_SET(BCHP_IRQ0_IRQEN, BCHP_IRQ0_IRQEN_spi_irqen_MASK);
#endif
}

static void bcmspi_hw_uninit(struct bcmspi_priv *priv)
{
	priv->mspi_hw->spcr2 = 0x0;	// disable irq and enable bits
	bcmspi_enable_bspi(priv);
}

static int bcmspi_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct bcmspi_priv *priv;
	struct spi_master *master;
	struct resource *res;
	int ret;

	DBG("bcmspi_probe\n");

#ifdef SPI_PINMUX_SETUP
	BDEV_WR_ARRAY(SPI_PINMUX_SETUP);
#endif

	master = spi_alloc_master(dev, sizeof(struct bcmspi_priv));
	if(! master) {
		dev_err(&pdev->dev, "error allocating spi_master\n");
		return(-ENOMEM);
	}

	priv = spi_master_get_devdata(master);

	priv->pdev = pdev;
	priv->state = STATE_IDLE;
	priv->pos.msg = NULL;
	priv->master = master;

	master->bus_num = pdev->id;
	master->num_chipselect = NUM_CHIPSELECT;
	master->setup = bcmspi_setup;
	master->transfer = bcmspi_transfer;
	master->cleanup = bcmspi_cleanup;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(! res) {
		dev_err(&pdev->dev, "can't get resource 0\n");
		ret = -EIO;
		goto err3;
	}
	/* MSPI register range */
	priv->mspi_hw = (volatile void *)ioremap(res->start,
		res->end - res->start);
	if(! priv->mspi_hw) {
		dev_err(&pdev->dev, "can't ioremap\n");
		ret = -EIO;
		goto err3;
	}

	/* IRQ */
	priv->irq = platform_get_irq(pdev, 0);
	if(priv->irq < 0) {
		dev_err(&pdev->dev, "no IRQ defined\n");
		ret = -ENODEV;
		goto err2;
	}

	ret = request_irq(priv->irq, bcmspi_interrupt, 0, dev->bus_id, priv);
	if(ret < 0) {
		dev_err(&pdev->dev, "unable to allocate IRQ\n");
		goto err2;
	}

	/* BSPI register range (not supported on all platforms) */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if(res) {
		priv->bspi_hw = (volatile void *)ioremap(res->start,
			res->end - res->start);
		if(! priv->bspi_hw) {
			dev_err(&pdev->dev, "can't ioremap\n");
			ret = -EIO;
			goto err2;
		}
	} else {
		priv->bspi_hw = NULL;
	}

	bcmspi_hw_init(priv);

#ifdef BRCM_SPI_CHIP_SELECT
	priv->bspi_chip_select = BRCM_SPI_CHIP_SELECT;
#ifdef BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_nand_flash_boot_MASK
	if(BDEV_RD(BCHP_SUN_TOP_CTRL_STRAP_VALUE_0) &
		BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_nand_flash_boot_MASK) {
		/* BSPI is disabled if the board is strapped for NAND */
		priv->bspi_chip_select = -1;
	}
#endif /* BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_nand_flash_boot_MASK */
#else /* BRCM_SPI_CHIP_SELECT */
	priv->bspi_chip_select = -1;
#endif /* BRCM_SPI_CHIP_SELECT */

	INIT_LIST_HEAD(&priv->msg_queue);
	spin_lock_init(&priv->lock);
	init_waitqueue_head(&priv->idle_wait);

	platform_set_drvdata(pdev, priv);

	tasklet_init(&priv->tasklet, bcmspi_tasklet, (unsigned long)priv);

	ret = spi_register_master(master);
	if(ret < 0) {
		dev_err(&pdev->dev, "can't register master\n");
		goto err1;
	}
	if(! default_master)
		default_master = master;

	return(0);

err1:
	bcmspi_hw_uninit(priv);
	free_irq(priv->irq, priv);
err2:
	iounmap(priv->mspi_hw);
err3:
	spi_master_put(master);
	return(ret);
}

static int bcmspi_remove(struct platform_device *pdev)
{
	struct bcmspi_priv *priv = platform_get_drvdata(pdev);
	unsigned long flags;

	/* acquire lock when the MSPI is idle */
	while(1) {
		spin_lock_irqsave(&priv->lock, flags);
		if(priv->state == STATE_IDLE)
			break;
		spin_unlock_irqrestore(&priv->lock, flags);
		sleep_on(&priv->idle_wait);
	}
	priv->state = STATE_SHUTDOWN;
	spin_unlock_irqrestore(&priv->lock, flags);
	
	tasklet_kill(&priv->tasklet);
	platform_set_drvdata(pdev, NULL);
	bcmspi_hw_uninit(priv);
	if(priv->bspi_hw)
		iounmap((const volatile void __iomem *)priv->bspi_hw);
	free_irq(priv->irq, priv);
	iounmap((const volatile void __iomem *)priv->mspi_hw);

	spi_unregister_master(priv->master);

	return(0);
}

static struct platform_driver driver = {
	.driver = {
		.name = "bcm7xxx-spi",
		.bus = &platform_bus_type,
		.owner = THIS_MODULE,
	},
	.probe = bcmspi_probe,
	.remove = __devexit_p(bcmspi_remove),
	//.shutdown = bcmspi_shutdown,
};

static int __init bcmspi_spi_init(void)
{
	platform_driver_register(&driver);

	return 0;
}
module_init(bcmspi_spi_init);

static void __exit bcmspi_spi_exit(void)
{
	platform_driver_unregister(&driver);
}
module_exit(bcmspi_spi_exit);

MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("MSPI/BSPI driver");
MODULE_LICENSE("GPL");
