/*---------------------------------------------------------------------------

    Copyright (c) 2008-2009 Broadcom Corporation                 /\
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

    File: bmoca.c

    Description: MoCA messaging driver

 ------------------------------------------------------------------------- */

#include <linux/module.h>
#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/pagemap.h>
#include <linux/poll.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/version.h>
#include <linux/scatterlist.h>

#include <asm/io.h>

#define DRV_VERSION		0x00040000
#define DRV_BUILD_NUMBER	0x07300901

#if   defined(CONFIG_MIPS_BCM7405) || defined(CONFIG_BCM7405)
#define M2M_64_WAR		1
#define MOCA6816		1
#define NATIVE			0
#include <linux/bmoca.h>
#elif defined(CONFIG_MIPS_BCM7420) || defined(CONFIG_BCM7420)
#define M2M_64_WAR		1
#define MOCA6816		0
#define NATIVE			1
#include <linux/bmoca.h>
#elif defined(DSL_MOCA)
#define M2M_64_WAR	1
#define MOCA6816		1
#define NATIVE			1
#include "bmoca.h"
#include <linux/netdevice.h>
#include <boardparms.h>
#else
#define M2M_64_WAR		1
#define MOCA6816		1
#define NATIVE			1
#include <linux/bmoca.h>
#endif

#if defined(CONFIG_MIPS_BRCM97XXX)
#include <asm/brcmstb/common/brcmstb.h>
#include <asm/brcmstb/common/brcm-pm.h>
#elif defined(CONFIG_BRCMSTB)
#include <asm/brcmstb/brcmstb.h>
#endif

#define MOCA_ENABLE		1
#define MOCA_DISABLE		0

/* offsets from the start of the MoCA core */
#define OFF_DATA_MEM		0x00000000
#define OFF_CNTL_MEM		0x0004c000
#define OFF_M2M_SRC		0x000a2000
#define OFF_M2M_DST		0x000a2004
#define OFF_M2M_CMD		0x000a2008
#define OFF_M2M_STATUS		0x000a200c

#if MOCA6816
#define OFF_GP0			0x000a1418
#define OFF_GP1			0x000a141c
#define OFF_RINGBELL		0x000a1404
#else
#define OFF_SW_RESET		0x000a2040
#define OFF_LED_CTRL		0x000a204c

#define OFF_GP0			0x000a2050
#define OFF_GP1			0x000a2054
#define OFF_RINGBELL		0x000a2060

#define OFF_L2_STATUS		0x000a2080
#define OFF_L2_CLEAR		0x000a2088
#define OFF_L2_MASK_SET		0x000a2090
#define OFF_L2_MASK_CLEAR	0x000a2094
#endif

/* region size */
#define DATA_MEM_SIZE		(256 * 1024)
#define CNTL_MEM_SIZE		(80 * 1024)

#define DATA_MEM_END		(OFF_DATA_MEM + DATA_MEM_SIZE)
#define CNTL_MEM_END		(OFF_CNTL_MEM + CNTL_MEM_SIZE)

/* mailbox layout */
#define HOST_REQ_SIZE		304
#define HOST_RESP_SIZE		256
#define CORE_REQ_SIZE		400
#define CORE_RESP_SIZE		64

/* offsets from the mailbox pointer */
#define HOST_REQ_OFFSET		0
#define HOST_RESP_OFFSET	(HOST_REQ_OFFSET + HOST_REQ_SIZE)
#define CORE_REQ_OFFSET		(HOST_RESP_OFFSET + HOST_RESP_SIZE)
#define CORE_RESP_OFFSET	(CORE_REQ_OFFSET + CORE_REQ_SIZE)

/* local H2M, M2H buffers */
#define NUM_CORE_MSG		8
#define NUM_HOST_MSG		8

#define FW_CHUNK_SIZE		4096
#define MAX_FW_PAGES		((DATA_MEM_SIZE >> PAGE_SHIFT) + 1)
#define MAX_LAB_PRINTF		104

#ifdef CONFIG_CPU_LITTLE_ENDIAN
#define M2M_WRITE		((1 << 31) | (1 << 27) | (1 << 28))
#define M2M_READ		((1 << 30) | (1 << 27) | (1 << 28))
#else
#define M2M_WRITE		((1 << 31) | (1 << 27))
#define M2M_READ		((1 << 30) | (1 << 27))
#endif

#define M2M_TIMEOUT_MS		10

#define NO_FLUSH_IRQ		0
#define FLUSH_IRQ		1
#define FLUSH_DMA_ONLY		2

/* DMA buffers may not share a cache line with anything else */
#define __DMA_ALIGN__		__attribute__((__aligned__(L1_CACHE_BYTES)))

struct moca_host_msg {
	u32			data[HOST_REQ_SIZE / 4] __DMA_ALIGN__;
	struct list_head	chain __DMA_ALIGN__;
	u32			len;
};

struct moca_core_msg {
	u32			data[CORE_REQ_SIZE / 4] __DMA_ALIGN__;
	struct list_head	chain __DMA_ALIGN__;
	u32			len;
};

struct moca_priv_data {
	struct platform_device	*pdev;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 26)
	struct class_device	*dev;
#else
	struct device		*dev;
#endif

	unsigned int		minor;
	int			irq;
	struct work_struct	work;
	void __iomem		*base;
	void __iomem		*i2c_base;

	unsigned int		mbx_offset;
	struct page		*fw_pages[MAX_FW_PAGES];
	struct scatterlist	fw_sg[MAX_FW_PAGES];
	struct completion	copy_complete;
	struct completion	chunk_complete;

	struct list_head	host_msg_free_list;
	struct list_head	host_msg_pend_list;
	struct moca_host_msg	host_msg_queue[NUM_HOST_MSG] __DMA_ALIGN__;
	wait_queue_head_t	host_msg_wq;

	struct list_head	core_msg_free_list;
	struct list_head	core_msg_pend_list;
	u32			core_resp_buf[CORE_RESP_SIZE / 4] __DMA_ALIGN__;
	struct moca_core_msg	core_msg_queue[NUM_CORE_MSG] __DMA_ALIGN__;
	wait_queue_head_t	core_msg_wq;

	spinlock_t		list_lock;
	struct mutex		dev_mutex;
	struct mutex		copy_mutex;
	int			host_mbx_busy;
	int			host_resp_pending;
	int			core_req_pending;
	int			assert_pending;
	int			wdt_pending;

	int			enabled;
	int			running;

	int			refcount;
	unsigned long		start_time;
	int			continuous_power_tx_mode;
};

/* support for multiple MoCA devices */
#define NUM_MINORS		8
static struct moca_priv_data *minor_tbl[NUM_MINORS];
static struct class *moca_class;

/* multiple 3450s are allowed to share one I2C controller */
static DEFINE_MUTEX(moca_i2c_mutex);

/* character major device number */
#define MOCA_MAJOR		234
#define MOCA_CLASS		"bmoca"

/* interrupt bits */
#define H2M_RESP		0x1
#define H2M_REQ			0x2

#define M2H_RESP		(1 << 0)
#define M2H_REQ			(1 << 1)
#define M2H_ASSERT		(1 << 2)
#define M2H_NEXTCHUNK		(1 << 3)
#define M2H_WDT			(1 << 10)
#define M2H_DMA			(1 << 11)

/* does this word contain a NIL byte (i.e. end of string)? */
#define HAS0(x)			((((x) & 0xff) == 0) || \
				 (((x) & 0xff00) == 0) || \
				 (((x) & 0xff0000) == 0) || \
				 (((x) & 0xff000000) == 0))

#define MOCA_SET(x, y)		do { \
	MOCA_WR(x, MOCA_RD(x) | (y)); \
} while(0)
#define MOCA_UNSET(x, y)	do { \
	MOCA_WR(x, MOCA_RD(x) & ~(y)); \
} while(0)

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 23)
static inline void sg_set_page(struct scatterlist *sg, struct page *page,
			       unsigned int len, unsigned int offset)
{
	sg->page = page;
	sg->length = len;
	sg->offset = offset;
}
static inline struct page *sg_page(struct scatterlist *sg)
{
	return sg->page;
}
#endif

#ifdef CONFIG_BRCM_MOCA_BUILTIN_FW
#error Not supported in this version
#else
static const char *bmoca_fw_image = NULL;
#endif

#if MOCA6816

#include "bmoca-6816.c"

#else

/*
 * LOW-LEVEL DEVICE OPERATIONS
 */

#define MOCA_RD(x)		(*((volatile u32 *)((uintptr_t)(x))))
#define MOCA_WR(x, y)		do { \
	(*((volatile u32 *)((uintptr_t)(x)))) = (y); \
} while(0)

#define I2C_RD(x)		MOCA_RD(x)
#define I2C_WR(x, y)		MOCA_WR(x, y)

static void moca_hw_reset(struct moca_priv_data *priv)
{
	/* disable and clear all interrupts */
	MOCA_WR(priv->base + OFF_L2_MASK_SET, 0xffffffff);
	MOCA_RD(priv->base + OFF_L2_MASK_SET);
	
	MOCA_WR(priv->base + OFF_L2_CLEAR, 0xffffffff);
	MOCA_RD(priv->base + OFF_L2_CLEAR);

	/* assert resets */
	MOCA_SET(priv->base + OFF_SW_RESET, ~((1 << 3) | (1 << 7)));
	MOCA_UNSET(priv->base + OFF_SW_RESET, (1 << 7));
	MOCA_RD(priv->base + OFF_SW_RESET);
}

/* called any time we start/restart/stop MoCA */
static void moca_hw_init(struct moca_priv_data *priv, int action)
{
#ifdef CONFIG_BRCM_PM
	if((action == MOCA_ENABLE) && ! priv->enabled) {
		brcm_pm_moca_enable(priv->minor);
		priv->enabled = 1;
	}
#endif

	moca_hw_reset(priv);
	udelay(1);

	/* deassert moca_sys_reset */
	MOCA_UNSET(priv->base + OFF_SW_RESET, (1 << 1));
	MOCA_RD(priv->base + OFF_SW_RESET);

	/* clear junk out of GP0/GP1 */
	MOCA_WR(priv->base + OFF_GP0, 0xffffffff);
	MOCA_WR(priv->base + OFF_GP1, 0x0);

	/* enable DMA completion interrupts */
	MOCA_WR(priv->base + OFF_RINGBELL, 0);
	MOCA_WR(priv->base + OFF_L2_MASK_CLEAR, M2H_DMA);
	MOCA_RD(priv->base + OFF_L2_MASK_CLEAR);

	/* set up activity LED for 50% duty cycle */
	MOCA_WR(priv->base + OFF_LED_CTRL, 0x40004000);

#ifdef CONFIG_BRCM_PM
	if((action == MOCA_DISABLE) && priv->enabled) {
		brcm_pm_moca_disable(priv->minor);
		priv->enabled = 0;
	}
#endif
}

static void moca_ringbell(struct moca_priv_data *priv, u32 mask)
{
	MOCA_WR(priv->base + OFF_RINGBELL, mask);
	MOCA_RD(priv->base);
}

static u32 moca_irq_status(struct moca_priv_data *priv, int flush)
{
	u32 stat = MOCA_RD(priv->base + OFF_L2_STATUS);
	if(flush == FLUSH_IRQ) {
		MOCA_WR(priv->base + OFF_L2_CLEAR, stat);
		MOCA_RD(priv->base + OFF_L2_CLEAR);
	}
	if(flush == FLUSH_DMA_ONLY) {
		MOCA_WR(priv->base + OFF_L2_CLEAR,
			stat & (M2H_DMA | M2H_NEXTCHUNK));
		MOCA_RD(priv->base + OFF_L2_CLEAR);
	}
	return(stat);
}

static void moca_enable_irq(struct moca_priv_data *priv)
{
	/* unmask everything */
	MOCA_WR(priv->base + OFF_L2_MASK_CLEAR,
		M2H_REQ | M2H_RESP | M2H_ASSERT | M2H_WDT |
		M2H_NEXTCHUNK | M2H_DMA);
	MOCA_RD(priv->base + OFF_L2_MASK_CLEAR);
}

static void moca_disable_irq(struct moca_priv_data *priv)
{
	/* mask everything except DMA completions */
	MOCA_WR(priv->base + OFF_L2_MASK_SET,
		M2H_REQ | M2H_RESP | M2H_ASSERT | M2H_WDT | M2H_NEXTCHUNK);
	MOCA_RD(priv->base + OFF_L2_MASK_SET);
}

static u32 moca_start_mips(struct moca_priv_data *priv)
{
	MOCA_UNSET(priv->base + OFF_SW_RESET, (1 << 0));
	MOCA_RD(priv->base + OFF_SW_RESET);
	return(0);
}

static void moca_m2m_xfer(struct moca_priv_data *priv,
	u32 dst, u32 src, u32 ctl)
{
	u32 status;

	MOCA_WR(priv->base + OFF_M2M_SRC, src);
	MOCA_WR(priv->base + OFF_M2M_DST, dst);
	MOCA_WR(priv->base + OFF_M2M_STATUS, 0);
	MOCA_RD(priv->base + OFF_M2M_STATUS);
	MOCA_WR(priv->base + OFF_M2M_CMD, ctl);

	if(wait_for_completion_timeout(&priv->copy_complete,
		1000 * M2M_TIMEOUT_MS) <= 0) {
		printk(KERN_WARNING "%s: DMA interrupt timed out, status %x\n",
			__FUNCTION__, moca_irq_status(priv, NO_FLUSH_IRQ));
	}
	status = MOCA_RD(priv->base + OFF_M2M_STATUS);

	if(status & (3 << 29))
		printk(KERN_WARNING "%s: bad status %08x "
			"(s/d/c %08x %08x %08x)\n", __FUNCTION__,
			status, src, dst, ctl);
}

static void moca_write_mem(struct moca_priv_data *priv,
	u32 dst_offset, void *src, unsigned int len)
{
	dma_addr_t pa;

	if((dst_offset >= CNTL_MEM_END) ||
		((dst_offset + len) > CNTL_MEM_END)) {
		printk(KERN_WARNING "%s: copy past end of cntl memory: %08x\n",
			__FUNCTION__, dst_offset);
		return;
	}

	pa = dma_map_single(&priv->pdev->dev, src, len, DMA_TO_DEVICE);
	moca_m2m_xfer(priv, dst_offset + OFF_DATA_MEM, (u32)pa,
		len | M2M_WRITE);
	dma_unmap_single(&priv->pdev->dev, pa, len, DMA_TO_DEVICE);
}

static void moca_read_mem(struct moca_priv_data *priv,
	void *dst, u32 src_offset, unsigned int len)
{
	dma_addr_t pa;

	if((src_offset >= CNTL_MEM_END) ||
		((src_offset + len) > CNTL_MEM_END)) {
		printk(KERN_WARNING "%s: copy past end of cntl memory: %08x\n",
			__FUNCTION__, src_offset);
		return;
	}

	pa = dma_map_single(&priv->pdev->dev, dst, len, DMA_FROM_DEVICE);
	moca_m2m_xfer(priv, (u32)pa, src_offset + OFF_DATA_MEM, len | M2M_READ);
	dma_unmap_single(&priv->pdev->dev, pa, len, DMA_FROM_DEVICE);
}

static void moca_write_sg(struct moca_priv_data *priv,
	u32 dst_offset, struct scatterlist *sg, int nents)
{
	int j;
	uintptr_t addr = OFF_DATA_MEM + dst_offset;

	dma_map_sg(&priv->pdev->dev, sg, nents, DMA_TO_DEVICE);

	for(j = 0; j < nents; j++) {
		moca_m2m_xfer(priv, addr, (u32)sg[j].dma_address,
			sg[j].length | M2M_WRITE);

		addr += sg[j].length;
	}

	dma_unmap_sg(&priv->pdev->dev, sg, nents, DMA_TO_DEVICE);
}

static void moca_read_sg(struct moca_priv_data *priv,
	u32 src_offset, struct scatterlist *sg, int nents)
{
	int j;
	uintptr_t addr = OFF_DATA_MEM + src_offset;

	dma_map_sg(&priv->pdev->dev, sg, nents, DMA_TO_DEVICE);

	for(j = 0; j < nents; j++) {
		moca_m2m_xfer(priv, (u32)sg[j].dma_address, addr,
			sg[j].length | M2M_READ);

		addr += sg[j].length;
		SetPageDirty(sg_page(&sg[j]));
	}

	dma_unmap_sg(&priv->pdev->dev, sg, nents, DMA_TO_DEVICE);
}
#endif

static void moca_put_pages(struct moca_priv_data *priv, int pages)
{
	int i;

	for(i = 0; i < pages; i++)
		page_cache_release(priv->fw_pages[i]);
}

static int moca_get_pages(struct moca_priv_data *priv, unsigned long addr,
	int size, unsigned int moca_addr, int write)
{
	unsigned int pages, chunk_size;
	int ret, i;

	if((addr & 3) || (size & 3) || (moca_addr & 3))
		return(-EINVAL);
	if((size <= 0) || (size > DATA_MEM_SIZE))
		return(-EINVAL);
	if(moca_addr >= OFF_CNTL_MEM) {
		if((moca_addr >= CNTL_MEM_END) ||
			((moca_addr + size) > CNTL_MEM_END))
			return(-EINVAL);
	} else {
		if((moca_addr >= DATA_MEM_END) ||
			((moca_addr + size) > DATA_MEM_END))
			return(-EINVAL);
	}
#if M2M_64_WAR
	if(((addr & 7) != (moca_addr & 7)) || (size < 32))
		return(-EINVAL);
#endif

	pages = ((addr & ~PAGE_MASK) + size + PAGE_SIZE - 1) >> PAGE_SHIFT;

	down_read(&current->mm->mmap_sem);
	ret = get_user_pages(current, current->mm, addr & PAGE_MASK, pages,
		write, 0, priv->fw_pages, NULL);
	up_read(&current->mm->mmap_sem);

	if(ret < 0)
		return(ret);
	BUG_ON((ret > MAX_FW_PAGES) || (pages == 0));

	if(ret < pages) {
		printk(KERN_WARNING "%s: get_user_pages returned %d, "
			"expecting %d\n", __FUNCTION__, ret, pages);
		moca_put_pages(priv, ret);
		return(-EFAULT);
	}

	chunk_size = PAGE_SIZE - (addr & ~PAGE_MASK);
	if(size < chunk_size)
		chunk_size = size;

	sg_set_page(&priv->fw_sg[0], priv->fw_pages[0], chunk_size,
		addr & ~PAGE_MASK);
	size -= chunk_size;

	for(i = 1; i < pages; i++) {
		sg_set_page(&priv->fw_sg[i], priv->fw_pages[i],
			size > PAGE_SIZE ? PAGE_SIZE : size, 0);
		size -= PAGE_SIZE;
	}
	return(ret);
}

static int moca_write_img(struct moca_priv_data *priv, struct moca_xfer * x)
{
	int pages, i, ret = -EINVAL;

	pages = moca_get_pages(priv, (unsigned long)x->buf, x->len, 0, 0);
	if(pages < 0)
		return(pages);
	if(pages < 3)
		goto out;

	/* host must use FW_CHUNK_SIZE MMU pages (for now) */
	BUG_ON(FW_CHUNK_SIZE != PAGE_SIZE);

	/* write the first two chunks, then start the MIPS */
	moca_write_sg(priv, 0, &priv->fw_sg[0], 2);
	moca_enable_irq(priv);
	moca_start_mips(priv);
	ret = 0;

	/* wait for an ACK, then write each successive chunk */
	for(i = 2; i < pages; i++) {
		if(wait_for_completion_timeout(&priv->chunk_complete,
				1000 * M2M_TIMEOUT_MS) <= 0) {
			moca_disable_irq(priv);
			printk(KERN_WARNING "%s: chunk ack timed out\n",
				__FUNCTION__);
			ret = -EIO;
			break;
		}
		moca_write_sg(priv, FW_CHUNK_SIZE, &priv->fw_sg[i], 1);
	}

out:
	moca_put_pages(priv, pages);
	return ret;
}

/*
 * MESSAGE AND LIST HANDLING
 */

static void moca_msg_reset(struct moca_priv_data *priv)
{
	int i;

	if(priv->running)
		moca_disable_irq(priv);
	priv->running = 0;
	priv->host_mbx_busy = 0;
	priv->host_resp_pending = 0;
	priv->core_req_pending = 0;
	priv->assert_pending = 0;
	priv->mbx_offset = -1;

	spin_lock_bh(&priv->list_lock);
	INIT_LIST_HEAD(&priv->core_msg_free_list);
	INIT_LIST_HEAD(&priv->core_msg_pend_list);

	for(i = 0; i < NUM_CORE_MSG; i++)
		list_add_tail(&priv->core_msg_queue[i].chain,
			&priv->core_msg_free_list);

	INIT_LIST_HEAD(&priv->host_msg_free_list);
	INIT_LIST_HEAD(&priv->host_msg_pend_list);

	for(i = 0; i < NUM_HOST_MSG; i++)
		list_add_tail(&priv->host_msg_queue[i].chain,
			&priv->host_msg_free_list);
	spin_unlock_bh(&priv->list_lock);
}

static struct list_head *moca_detach_head(struct moca_priv_data *priv,
	struct list_head *h)
{
	struct list_head *r = NULL;

	spin_lock_bh(&priv->list_lock);
	if(! list_empty(h)) {
		r = h->next;
		list_del(r);
	}
	spin_unlock_bh(&priv->list_lock);

	return(r);
}

static void moca_attach_tail(struct moca_priv_data *priv,
	struct list_head *elem, struct list_head *list)
{
	spin_lock_bh(&priv->list_lock);
	list_add_tail(elem, list);
	spin_unlock_bh(&priv->list_lock);
}

static int moca_recvmsg(struct moca_priv_data *priv, uintptr_t offset,
	u32 max_size, uintptr_t reply_offset)
{
	struct list_head *ml = NULL;
	struct moca_core_msg *m;
	unsigned int i, w, rw, num_ies;
	u32 data;
	char *msg;
	int err = -ENOMEM;
	u32 *reply = priv->core_resp_buf;

	ml = moca_detach_head(priv, &priv->core_msg_free_list);
	if(ml == NULL) {
		msg = "no entries left on core_msg_free_list";
		goto bad;
	}
	m = list_entry(ml, struct moca_core_msg, chain);

	BUG_ON((uintptr_t)m->data & (L1_CACHE_BYTES - 1));
	moca_read_mem(priv, m->data, offset + priv->mbx_offset, max_size);

	data = be32_to_cpu(m->data[0]);

	num_ies = data & 0xffff;

	if(reply_offset) {
		/* ACK + seq number + number of IEs */
		reply[0] = cpu_to_be32((data & 0x00ff0000) | 0x04000000 |
			num_ies);
		rw = 1;
	}

	err = -EINVAL;
	w = 1;
	max_size >>= 2;
	while(num_ies) {
		if(w >= max_size) {
			msg = "dropping long message";
			goto bad;
		}

		data = be32_to_cpu(m->data[w++]);

		if(reply_offset) {
			/*
			 * ACK each IE in the original message;
			 * return code is always 0
			 */
			if((rw << 2) >= CORE_RESP_SIZE)
				printk(KERN_WARNING "%s: Core ack buffer "
					"overflowed\n", __FUNCTION__);
			else {
				reply[rw] = cpu_to_be32((data & ~0xffff) | 4);
				rw++;
				reply[rw] = cpu_to_be32(0);
				rw++;
			}
		}

		if(data & 3) {
			msg = "IE is not a multiple of 4 bytes";
			goto bad;
		}
		w += ((data & 0xffff) >> 2);

		if(w > max_size) {
			msg = "dropping long message";
			goto bad;
		}
		num_ies--;
	}
	m->len = w << 2;

	/* special case for lab_printf traps */
	if((be32_to_cpu(m->data[0]) & 0xff0000ff) == 0x09000001 &&
		be32_to_cpu(m->data[1]) == 0x600b0008 &&
		(be32_to_cpu(m->data[3]) <= MAX_LAB_PRINTF)) {

		u32 str_len = (be32_to_cpu(m->data[3]) + 3) & ~3;
		u32 str_addr = be32_to_cpu(m->data[2]) & 0x1fffffff;

		m->len = 8 + str_len;
#if M2M_64_WAR
		if(str_addr & 0x7) {
			int offset = (str_addr & 0x7) >> 2;
			moca_read_mem(priv, &m->data[4], str_addr & ~0x7,
				MAX_LAB_PRINTF + 12);
			for(i = 0; i < (str_len >> 2); i++)
				data = m->data[2 + i] = m->data[4 + offset + i];
		} else {
			moca_read_mem(priv, &m->data[2], str_addr,
				MAX_LAB_PRINTF);
		}
#else
		moca_read_mem(priv, &m->data[2], str_addr, str_len);
#endif

		m->data[1] = cpu_to_be32((MOCA_IE_DRV_PRINTF << 16) +
			m->len - 8);
	}

	if(reply_offset) {
		if(moca_irq_status(priv, NO_FLUSH_IRQ) & M2H_ASSERT) {
			/* do not retry - message is gone forever */
			err = 0;
			msg = "core_req overwritten by assertion";
			goto bad;
		}
		moca_write_mem(priv, reply_offset + priv->mbx_offset,
			reply, rw << 2);
		moca_ringbell(priv, H2M_RESP);
	}

	moca_attach_tail(priv, ml, &priv->core_msg_pend_list);
	wake_up(&priv->core_msg_wq);

	return(0);

bad:
	printk(KERN_WARNING "%s: %s\n", __FUNCTION__, msg);

	if(ml)
		moca_attach_tail(priv, ml, &priv->core_msg_free_list);

	return(err);
}

static int moca_h2m_sanity_check(struct moca_host_msg *m)
{
	unsigned int w, num_ies;
	u32 data;

	data = be32_to_cpu(m->data[0]);
	num_ies = data & 0xffff;

	w = 1;
	while(num_ies) {
		if(w >= (m->len << 2))
			return(-1);

		data = be32_to_cpu(m->data[w++]);

		if(data & 3)
			return(-1);
		w += (data & 0xffff) >> 2;
		num_ies--;
	}
	return(w << 2);
}

static int moca_sendmsg(struct moca_priv_data *priv)
{
	struct list_head *ml = NULL;
	struct moca_host_msg *m;

	if(priv->host_mbx_busy == 1)
		return(-1);

	ml = moca_detach_head(priv, &priv->host_msg_pend_list);
	if(ml == NULL)
		return(-EAGAIN);
	m = list_entry(ml, struct moca_host_msg, chain);
	
	moca_write_mem(priv, priv->mbx_offset + HOST_REQ_OFFSET,
		m->data, m->len);
	
	moca_ringbell(priv, H2M_REQ);
	moca_attach_tail(priv, ml, &priv->host_msg_free_list);
	wake_up(&priv->host_msg_wq);

	return(0);
}

static int moca_wdt(struct moca_priv_data *priv)
{
	struct list_head *ml = NULL;
	struct moca_core_msg *m;

	ml = moca_detach_head(priv, &priv->core_msg_free_list);
	if(ml == NULL) {
		printk(KERN_WARNING
			"%s: no entries left on core_msg_free_list\n",
			__FUNCTION__);
		return(-ENOMEM);
	}

	/*
	 * generate phony wdt message to pass to the user
	 * type = 0x09 (trap)
	 * IE type = 0xff01 (wdt), zero length
	 */
	m = list_entry(ml, struct moca_core_msg, chain);
	m->data[0] = cpu_to_be32(0x09000001);
	m->data[1] = cpu_to_be32(MOCA_IE_WDT << 16);
	m->len = 8;

	moca_attach_tail(priv, ml, &priv->core_msg_pend_list);
	wake_up(&priv->core_msg_wq);

	return(0);
}

/*
 * INTERRUPT / WORKQUEUE BH
 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
static void moca_work_handler(struct work_struct *work)
{
	struct moca_priv_data *priv =
		container_of(work, struct moca_priv_data, work);
#else
static void moca_work_handler(void *arg)
{
	struct moca_priv_data *priv = arg;
#endif
	u32 mask = moca_irq_status(priv, FLUSH_IRQ);
	int ret, stopped = 0;

	if(mask & M2H_DMA)
		complete(&priv->copy_complete);

	if((mask & (M2H_REQ | M2H_RESP)) && priv->mbx_offset == -1) {
		uintptr_t base = MOCA_RD(priv->base + OFF_GP0) & 0x1fffffff;

		if((base == 0) || (base > CNTL_MEM_END) || (base & 7)) {
			printk(KERN_WARNING "%s: can't get mailbox base\n",
				__FUNCTION__);
			return;
		}
		priv->mbx_offset = base;
	}

	mutex_lock(&priv->dev_mutex);

	/* fatal events */
	if(mask & M2H_ASSERT) {
		ret = moca_recvmsg(priv, CORE_REQ_OFFSET, CORE_REQ_SIZE, 0);
		if(ret == -ENOMEM)
			priv->assert_pending = 1;
		stopped = 1;
	}
	if(mask & M2H_WDT) {
		ret = moca_wdt(priv);
		if(ret == -ENOMEM)
			priv->wdt_pending = 1;
		stopped = 1;
	}
	if(stopped) {
		priv->running = 0;
		priv->core_req_pending = 0;
		priv->host_resp_pending = 0;
		priv->host_mbx_busy = 1;
		mutex_unlock(&priv->dev_mutex);
		wake_up(&priv->core_msg_wq);
		return;
	}

	/* normal events */
	if(mask & M2H_REQ) {
		ret = moca_recvmsg(priv, CORE_REQ_OFFSET,
			CORE_REQ_SIZE, CORE_RESP_OFFSET);
		if(ret == -ENOMEM)
			priv->core_req_pending = 1;
	}
	if(mask & M2H_RESP) {
		ret = moca_recvmsg(priv, HOST_RESP_OFFSET, HOST_RESP_SIZE, 0);
		if(ret == -ENOMEM)
			priv->host_resp_pending = 1;
		if(ret == 0) {
			priv->host_mbx_busy = 0;
			moca_sendmsg(priv);
		}
	}
	mutex_unlock(&priv->dev_mutex);

	moca_enable_irq(priv);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 19)
static irqreturn_t moca_interrupt(int irq, void *arg)
#else
static irqreturn_t moca_interrupt(int irq, void *arg, struct pt_regs *regs)
#endif
{
	struct moca_priv_data *priv = arg;
#if NATIVE
	u32 mask = moca_irq_status(priv, FLUSH_DMA_ONLY);

	/* need to handle DMA completions ASAP */
	if(mask & M2H_DMA) {
		complete(&priv->copy_complete);
		mask &= ~M2H_DMA;
	}
	if(mask & M2H_NEXTCHUNK) {
		complete(&priv->chunk_complete);
		mask &= ~M2H_NEXTCHUNK;
	}

	if(!mask)
		return(IRQ_HANDLED);
#endif

	moca_disable_irq(priv);
	schedule_work(&priv->work);
	return(IRQ_HANDLED);
}

/*
 * BCM3450 ACCESS VIA I2C
 */

struct bsc_regs {
	u32			chip_address;
	u32			data_in[8];
	u32			cnt_reg;
	u32			ctl_reg;
	u32			iic_enable;
	u32			data_out[8];
	u32			ctlhi_reg;
	u32			scl_param;
};

static int moca_3450_wait(struct moca_priv_data *priv)
{
	volatile struct bsc_regs *bsc = priv->i2c_base;
	long timeout = HZ / 200;	// 1/200s = 5ms
	DECLARE_WAIT_QUEUE_HEAD(wait);
	int i = 0;

	do {
		if(I2C_RD(&bsc->iic_enable) & 2) {
			I2C_WR(&bsc->iic_enable, 0);
			return(0);
		}
		if(i++ > 10) {
			I2C_WR(&bsc->iic_enable, 0);
			printk(KERN_WARNING "%s: 3450 I2C timed out\n",
				__FUNCTION__);
			return(-1);
		}
		sleep_on_timeout(&wait, timeout ? timeout : 1);
	} while(1);
}

static void moca_3450_write(struct moca_priv_data *priv, u8 addr, u32 data)
{
	volatile struct bsc_regs *bsc = priv->i2c_base;
	struct moca_platform_data *pd = priv->pdev->dev.platform_data;

	I2C_WR(&bsc->iic_enable, 0);
	I2C_WR(&bsc->chip_address, pd->bcm3450_i2c_addr << 1);
	I2C_WR(&bsc->data_in[0], (addr >> 2) | (data << 8));
	I2C_WR(&bsc->data_in[1], data >> 24);
	I2C_WR(&bsc->cnt_reg, (5 << 0) | (0 << 6));	// 5B out, 0B in
	I2C_WR(&bsc->ctl_reg, (1 << 4) | (0 << 0));	// write only, 390kHz
	I2C_WR(&bsc->ctlhi_reg, (1 << 6));		// 32-bit words
	I2C_WR(&bsc->iic_enable, 1);

	moca_3450_wait(priv);
}

static u32 moca_3450_read(struct moca_priv_data *priv, u8 addr)
{
	volatile struct bsc_regs *bsc = priv->i2c_base;
	struct moca_platform_data *pd = priv->pdev->dev.platform_data;

	I2C_WR(&bsc->iic_enable, 0);
	I2C_WR(&bsc->chip_address, pd->bcm3450_i2c_addr << 1);
	I2C_WR(&bsc->data_in[0], (addr >> 2));
	I2C_WR(&bsc->cnt_reg, (1 << 0) | (4 << 6));	// 1B out then 4B in
	I2C_WR(&bsc->ctl_reg, (1 << 4) | (3 << 0));	// write/read, 390kHz
	I2C_WR(&bsc->ctlhi_reg, (1 << 6));		// 32-bit words
	I2C_WR(&bsc->iic_enable, 1);

	if(moca_3450_wait(priv) == 0)
		return(I2C_RD(&bsc->data_out[0]));
	else
		return(0xffffffff);
}

#define BCM3450_CHIP_ID		0x00
#define BCM3450_CHIP_REV	0x04
#define BCM3450_PACNTL		0x18
#define BCM3450_MISC		0x1c

static void moca_3450_init(struct moca_priv_data *priv, int action)
{
	u32 data;

	mutex_lock(&moca_i2c_mutex);

	if (action == MOCA_ENABLE) 
	{
		/* reset the 3450's I2C block */
		moca_3450_write(priv, BCM3450_MISC,
			moca_3450_read(priv, BCM3450_MISC) | 1);

		/* verify chip ID */
		data = moca_3450_read(priv, BCM3450_CHIP_ID);
		if(data != 0x3450)
			printk(KERN_WARNING "%s: invalid 3450 chip ID 0x%08x\n",
				__FUNCTION__, data);

		/* reset the 3450's deserializer */
		data = moca_3450_read(priv, BCM3450_MISC);
		data &= ~0x8000; /* power on PA/LNA */
		moca_3450_write(priv, BCM3450_MISC, data | 2);
		moca_3450_write(priv, BCM3450_MISC, data & ~2);

		/* set new PA gain */
		data = moca_3450_read(priv, BCM3450_PACNTL);
		moca_3450_write(priv, BCM3450_PACNTL, (data & ~0x7ffc) |
			(0x09 << 11) |		// RDEG
			(0x38 << 5) |		// CURR_CONT
			(0x05 << 2));		// CURR_FOLLOWER
	}
	else
	{
		/* power down the PA/LNA */
		data = moca_3450_read(priv, BCM3450_MISC);
		moca_3450_write(priv, BCM3450_MISC, data | 0x8000);
	}
	mutex_unlock(&moca_i2c_mutex);
}

/*
 * FILE OPERATIONS
 */

static int moca_file_open(struct inode *inode, struct file *file)
{
	unsigned int minor = iminor(inode);
	struct moca_priv_data *priv;

	if((minor > NUM_MINORS) || minor_tbl[minor] == NULL)
		return(-ENODEV);
	
	file->private_data = priv = minor_tbl[minor];

	mutex_lock(&priv->dev_mutex);
	priv->refcount++;
	mutex_unlock(&priv->dev_mutex);
	return(0);
}

static int moca_file_release(struct inode *inode, struct file *file)
{
	struct moca_priv_data *priv = file->private_data;

	mutex_lock(&priv->dev_mutex);
	priv->refcount--;
	if(priv->refcount == 0 && priv->running == 1) {
		/* last user closed the device */
		moca_msg_reset(priv);
		moca_hw_init(priv, MOCA_DISABLE);
	}
	mutex_unlock(&priv->dev_mutex);
	return(0);
}

static int moca_ioctl_readmem(struct moca_priv_data *priv,
	unsigned long xfer_uaddr)
{
	struct moca_xfer x;
	int pages;

	if(copy_from_user(&x, (void __user *)xfer_uaddr, sizeof(x)))
		return(-EFAULT);
	
	pages = moca_get_pages(priv,
		(unsigned long)x.buf, x.len, x.moca_addr, 1);
	if(pages < 0)
		return(pages);

	moca_read_sg(priv, x.moca_addr, priv->fw_sg, pages);
	moca_put_pages(priv, pages);

	return(0);
}

static int moca_ioctl_get_drv_info(struct moca_priv_data *priv,
	unsigned long arg)
{
	struct moca_kdrv_info info;
	struct moca_platform_data *pd = priv->pdev->dev.platform_data;

	info.version = DRV_VERSION;
	info.build_number = DRV_BUILD_NUMBER;
	info.builtin_fw = !!bmoca_fw_image;

	info.uptime = (jiffies - priv->start_time) / HZ;
	info.refcount = priv->refcount;
	info.gp1 = priv->running ? MOCA_RD(priv->base + OFF_GP1) : 0;

	memcpy(info.enet_name, pd->enet_name, IFNAMSIZ);
	info.enet_id = pd->enet_id;
	info.macaddr_hi = pd->macaddr_hi;
	info.macaddr_lo = pd->macaddr_lo;
	info.hw_rev = pd->hw_rev;
	info.rf_band = pd->rf_band;

	if(copy_to_user((void *)arg, &info, sizeof(info)))
		return(-EFAULT);
	
	return(0);
}

static long moca_file_ioctl(struct file *file, unsigned int cmd,
	unsigned long arg)
{
	struct moca_priv_data *priv = file->private_data;
	struct moca_start     start;
	long ret = -ENOTTY;

	mutex_lock(&priv->dev_mutex);
	switch(cmd) {
		case MOCA_IOCTL_START:
			ret = 0;

			if(copy_from_user(&start, (void __user *)arg,
					sizeof(start)))
				ret = -EFAULT;

			if(ret >= 0) {
				moca_msg_reset(priv);
				priv->continuous_power_tx_mode =
					start.continuous_power_tx_mode;
				moca_hw_init(priv, MOCA_ENABLE);
				moca_3450_init(priv, MOCA_ENABLE);
				moca_irq_status(priv, FLUSH_IRQ);
				ret = moca_write_img(priv, &start.x);
				if(ret >= 0)
					priv->running = 1;
			}
			break;
		case MOCA_IOCTL_STOP:
			moca_msg_reset(priv);
			moca_3450_init(priv, MOCA_DISABLE);
			moca_hw_init(priv, MOCA_DISABLE);
			ret = 0;
			break;
		case MOCA_IOCTL_READMEM:
			ret = moca_ioctl_readmem(priv, arg);
			break;
		case MOCA_IOCTL_GET_DRV_INFO:
			ret = moca_ioctl_get_drv_info(priv, arg);
			break;
	}
	mutex_unlock(&priv->dev_mutex);

	return(ret);
}

static ssize_t moca_file_read(struct file *file, char __user *buf,
	size_t count, loff_t *ppos)
{
	struct moca_priv_data *priv = file->private_data;
	DECLARE_WAITQUEUE(wait, current);
	struct list_head *ml = NULL;
	struct moca_core_msg *m = NULL;
	ssize_t ret;
	int empty_free_list = 0;

	if(count < CORE_REQ_SIZE)
		return(-EINVAL);

	add_wait_queue(&priv->core_msg_wq, &wait);
	do {
		__set_current_state(TASK_INTERRUPTIBLE);

		ml = moca_detach_head(priv, &priv->core_msg_pend_list);
		if(ml != NULL) {
			m = list_entry(ml, struct moca_core_msg, chain);
			ret = 0;
			break;
		}
		if (file->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			break;
		}
		if (signal_pending(current)) {
			ret = -ERESTARTSYS;
			break;
		}
		schedule();
	} while (1);
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&priv->core_msg_wq, &wait);

	if(ret < 0)
		return(ret);

	if(copy_to_user(buf, m->data, m->len))
		ret = -EFAULT;	// beware: message will be dropped
	else
		ret = m->len;

	spin_lock_bh(&priv->list_lock);
	if(list_empty(&priv->core_msg_free_list))
		empty_free_list = 1;
	list_add_tail(ml, &priv->core_msg_free_list);
	spin_unlock_bh(&priv->list_lock);

	if(empty_free_list) {
		/*
		 * we just freed up space for another message, so if there was
		 * a backlog, clear it out
		 */
		mutex_lock(&priv->dev_mutex);
		if(priv->assert_pending)
			if(moca_recvmsg(priv, CORE_REQ_OFFSET, CORE_REQ_SIZE,
				0) != -ENOMEM)
				priv->assert_pending = 0;
		if(priv->wdt_pending)
			if(moca_wdt(priv) != -ENOMEM)
				priv->wdt_pending = 0;
		if(priv->core_req_pending)
			if(moca_recvmsg(priv, CORE_REQ_OFFSET,
				CORE_REQ_SIZE, CORE_RESP_OFFSET) != -ENOMEM)
				priv->core_req_pending = 0;
		if(priv->host_resp_pending)
			if(moca_recvmsg(priv, HOST_RESP_OFFSET,
				HOST_RESP_SIZE, 0) != -ENOMEM)
				priv->host_resp_pending = 0;
		mutex_unlock(&priv->dev_mutex);
	}

	return(ret);
}

static ssize_t moca_file_write(struct file *file, const char __user *buf,
	size_t count, loff_t *ppos)
{
	struct moca_priv_data *priv = file->private_data;
	DECLARE_WAITQUEUE(wait, current);
	struct list_head *ml = NULL;
	struct moca_host_msg *m = NULL;
	ssize_t ret;

	if(count > HOST_REQ_SIZE)
		return(-EINVAL);

	add_wait_queue(&priv->host_msg_wq, &wait);
	do {
		__set_current_state(TASK_INTERRUPTIBLE);

		ml = moca_detach_head(priv, &priv->host_msg_free_list);
		if(ml != NULL) {
			m = list_entry(ml, struct moca_host_msg, chain);
			ret = 0;
			break;
		}
		if (file->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			break;
		}
		if (signal_pending(current)) {
			ret = -ERESTARTSYS;
			break;
		}
		schedule();
	} while (1);
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&priv->host_msg_wq, &wait);

	if(ret < 0)
		return(ret);

	m->len = count;

	if(copy_from_user(m->data, buf, m->len)) {
		ret = -EFAULT;
		goto bad;
	}
	ret = moca_h2m_sanity_check(m);
	if(ret < 0) {
		ret = -EINVAL;
		goto bad;
	}

	moca_attach_tail(priv, ml, &priv->host_msg_pend_list);

	mutex_lock(&priv->dev_mutex);
	if(priv->running)
		moca_sendmsg(priv);
	else
		ret = -EIO;
	mutex_unlock(&priv->dev_mutex);

	return(ret);

bad:
	moca_attach_tail(priv, ml, &priv->host_msg_free_list);

	return(ret);
}

static unsigned int moca_file_poll(struct file *file, poll_table *wait)
{
	struct moca_priv_data *priv = file->private_data;
	unsigned int ret = 0;

	poll_wait(file, &priv->core_msg_wq, wait);
	poll_wait(file, &priv->host_msg_wq, wait);

	spin_lock_bh(&priv->list_lock);
	if(! list_empty(&priv->core_msg_pend_list))
		ret |= POLLIN | POLLRDNORM;
	if(! list_empty(&priv->host_msg_free_list))
		ret |= POLLOUT | POLLWRNORM;
	spin_unlock_bh(&priv->list_lock);

	return(ret);
}

static struct file_operations moca_fops = {
	.owner =		THIS_MODULE,
	.open =			moca_file_open,
	.release =		moca_file_release,
	.unlocked_ioctl =	moca_file_ioctl,
	.read =			moca_file_read,
	.write =		moca_file_write,
	.poll =			moca_file_poll,
};

/*
 * PLATFORM DRIVER
 */

static int moca_probe(struct platform_device *pdev)
{
	struct moca_priv_data *priv;
	struct resource *mres, *ires;
	int minor, err;
	struct moca_platform_data *pd = pdev->dev.platform_data;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if(! priv) {
		printk(KERN_ERR "%s: out of memory\n", __FUNCTION__);
		return(-ENOMEM);
	}
	dev_set_drvdata(&pdev->dev, priv);
	priv->pdev = pdev;
	priv->start_time = jiffies;

	init_waitqueue_head(&priv->host_msg_wq);
	init_waitqueue_head(&priv->core_msg_wq);
	init_completion(&priv->copy_complete);
	init_completion(&priv->chunk_complete);
	
	spin_lock_init(&priv->list_lock);
	mutex_init(&priv->dev_mutex);
	mutex_init(&priv->copy_mutex);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
	INIT_WORK(&priv->work, moca_work_handler);
#else
	INIT_WORK(&priv->work, moca_work_handler, priv);
#endif

	priv->minor = -1;
	for(minor = 0; minor < NUM_MINORS; minor++) {
		if(minor_tbl[minor] == NULL) {
			priv->minor = minor;
			break;
		}
	}

	if(priv->minor == -1) {
		printk(KERN_ERR "%s: can't allocate minor device\n",
			__FUNCTION__);
		err = -EIO;
		goto bad;
	}

	mres = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ires = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if(! mres || ! ires) {
		printk(KERN_ERR "%s: can't get resources\n", __FUNCTION__);
		err = -EIO;
		goto bad;
	}
	priv->base = ioremap(mres->start, mres->end - mres->start + 1);
	priv->irq = ires->start;
	priv->i2c_base = ioremap(pd->bcm3450_i2c_base, sizeof(struct bsc_regs));

	/* leave core in reset until we get an ioctl */
	moca_hw_reset(priv);

	if(request_irq(priv->irq, moca_interrupt, 0, "moca", priv) < 0) {
		printk(KERN_WARNING  "%s: can't request interrupt\n",
			__FUNCTION__);
		err = -EIO;
		goto bad2;
	}
	moca_hw_init(priv, MOCA_ENABLE);
	moca_disable_irq(priv);
	moca_msg_reset(priv);
	moca_hw_init(priv, MOCA_DISABLE);

	printk(KERN_INFO "bmoca: adding minor #%d at base 0x%08x, IRQ %d, "
		"I2C 0x%08lx/0x%02x\n", priv->minor, mres->start, ires->start,
		pd->bcm3450_i2c_base, pd->bcm3450_i2c_addr);

	minor_tbl[priv->minor] = priv;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 26)
	priv->dev = class_device_create(moca_class, NULL,
		MKDEV(MOCA_MAJOR, priv->minor), &pdev->dev,
		"bmoca%d", priv->minor);
#else
	priv->dev = device_create(moca_class, NULL,
		MKDEV(MOCA_MAJOR, priv->minor), NULL, "bmoca%d", priv->minor);
#endif
	if(IS_ERR(priv->dev)) {
		printk(KERN_WARNING "bmoca: can't register class device\n");
		priv->dev = NULL;
	}

	return(0);

bad2:
	if(priv->base)
		iounmap(priv->base);
	if(priv->i2c_base)
		iounmap(priv->i2c_base);
bad:
	kfree(priv);
	return(err);
}

static int moca_remove(struct platform_device *pdev)
{
	struct moca_priv_data *priv = dev_get_drvdata(&pdev->dev);

	if(priv->dev)
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 26)
		class_device_destroy(moca_class,
			MKDEV(MOCA_MAJOR, priv->minor));
#else
		device_destroy(moca_class, MKDEV(MOCA_MAJOR, priv->minor));
#endif
	minor_tbl[priv->minor] = NULL;

	free_irq(priv->irq, priv);
	iounmap(priv->i2c_base);
	iounmap(priv->base);
	kfree(priv);

	return(0);
}

static struct platform_driver moca_plat_drv = {
	.probe =		moca_probe,
	.remove =		moca_remove,
	.driver = {
		.name =		"bmoca",
		.owner =	THIS_MODULE,
	},
};

static int moca_init(void)
{
	int ret;
#if MOCA6816
	moca_read_mac_addr(NULL, &moca_data.macaddr_hi, &moca_data.macaddr_lo);
	platform_device_register(&moca_plat_dev);
#endif
	memset(minor_tbl, 0, sizeof(minor_tbl));
	ret = register_chrdev(MOCA_MAJOR, MOCA_CLASS, &moca_fops);
	if(ret < 0) {
		printk(KERN_ERR "bmoca: can't register major %d\n", MOCA_MAJOR);
		goto bad;
	}
	moca_class = class_create(THIS_MODULE, MOCA_CLASS);
	if(IS_ERR(moca_class)) {
		printk(KERN_ERR "bmoca: can't create device class\n");
		ret = PTR_ERR(moca_class);
		goto bad2;
	}
	ret = platform_driver_register(&moca_plat_drv);
	if(ret < 0) {
		printk(KERN_ERR "bmoca: can't register platform_driver\n");
		goto bad3;
	}
	return(0);

bad3:
	class_destroy(moca_class);
bad2:
	unregister_chrdev(MOCA_MAJOR, MOCA_CLASS);
bad:
	return(ret);
}

static void moca_exit(void)
{
	class_destroy(moca_class);
	unregister_chrdev(MOCA_MAJOR, MOCA_CLASS);
	platform_driver_unregister(&moca_plat_drv);
#if MOCA6816
	platform_device_unregister(&moca_plat_dev);
#endif
}

module_init(moca_init);
module_exit(moca_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("MoCA messaging driver");
