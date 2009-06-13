/*
 *
 * Copyright (c) 2002-2008 Broadcom Corporation 
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
 *
*/
#ifndef __BCMGENET_H__
#define __BCMGENET_H__

#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/spinlock.h>
#include <linux/version.h>

#include <asm/brcmstb/common/bcmtypes.h>
#include <asm/brcmstb/common/brcmstb.h>
#include "bcmgenet_map.h"

#define TOTAL_DESC				256		/* total number of Buffer Descriptors, same for Rx/Tx */
#define DMA_RING_DESC_INDEX		16		/* which ring is descriptor based */

#define ENET_MAX_MTU_SIZE       1536    /* Body(1500) + EH_SIZE(14) + VLANTAG(4) + BRCMTAG(6) + FCS(4) = 1528.  1536 is multiple of 256 bytes */
#define DMA_MAX_BURST_LENGTH    0x10    

#define MAX_RX_BUFS             (TOTAL_DESC * 6)

#define ETH_CRC_LEN             4
/* misc. configuration */
#define DMA_FC_THRESH_LO        5
#define DMA_FC_THRESH_HI        4*DMA_FC_THRESH_LO


typedef struct Enet_CB {
    struct sk_buff      *skb;
    volatile DmaDesc    *BdAddr;
	dma_addr_t			dma_addr;
	int					dma_len;
    struct Enet_CB   *next;          /* ptr to next header in free list */
} Enet_CB;

#define BCMUMAC_MAX_DEVS			2

/* power management mode */
#define BCMUMAC_POWER_CABLE_SENSE	0
#define BCMUMAC_POWER_WOL_MAGIC		1
#define BCMUMAC_POWER_WOL_ACPI		2

/*
 * device context
 */ 
typedef struct BcmEnet_devctrl {
    struct net_device *dev;             /* ptr to net_device */
    spinlock_t      lock;               /* Serializing lock */
	spinlock_t		bh_lock;			/* soft IRQ lock */
	struct mii_if_info mii;				/* mii interface info */
	struct mutex mdio_mutex;			/* mutex for mii_read/write */
	wait_queue_head_t	wq;				/* mii wait queue */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18)
	struct napi_struct napi;
#else
	struct net_device_stats stats;
	struct device generic_dev;
#endif
	
	unsigned long base_addr;			/* UNIMAC register start address. */
    volatile uniMacRegs *umac;     		/* UNIMAC register block base address */
	volatile rbufRegs	*rbuf;			/* rbuf registers */
	volatile intrl2Regs *intrl2_0;		/* INTR2_0 registers */
	volatile intrl2Regs *intrl2_1;		/* INTR2_1 registers */
	volatile SysRegs    *sys;			
	volatile GrBridgeRegs *grb;
	volatile ExtRegs    *ext;
	volatile unsigned long *hfb;		/* HFB registers */

    /* transmit variables */
    volatile tDmaRegs *txDma;			/* location of transmit DMA register set */
    Enet_CB      *txCbs;             /* memory locaation of tx control block pool */
    volatile DmaDesc *txBds;            /* Memory location of tx Dma BD ring */

    int	nrTxBds;						/* number of transmit bds */
    int	txFreeBds;						/* # of free transmit bds */
	int	txLastCIndex;					/* consumer index for the last xmit call */

    /* receive variables */
    volatile rDmaRegs *rxDma;			/* location of receive DMA register set */
    volatile DmaDesc *rxBds;            /* Memory location of rx bd ring */
    volatile DmaDesc *rxBdAssignPtr;    /* ptr to next rx bd to become full */
    Enet_CB       *rxCbs;            /* memory locaation of rx control block pool */

    int	nrRxBds;						/* number of receive bds */
    int	rxBufLen;						/* size of rx buffers for DMA */

    int irq0;							/* regular IRQ */
	int	irq1;							/* ring buf IRQ */
    int             phyAddr;            /* 1 - external MII, 0 - internal MII (default after reset) */
	int				phyType;
    atomic_t        linkState;			/* 1 - linkup, 0 - link down */
	int				bIPHdrOptimize;
	int				irq_stat;			/* software copy of irq status, for botom half processing */
	struct work_struct bcmgenet_irq_work;	/* bottom half work */
	struct work_struct bcmgenet_mii_work;	/* mii work */
    
    int             devnum;				/* 0=EMAC_0, 1=EMAC_1 */
} BcmEnet_devctrl;

#if defined(CONFIG_BCMUMAC_DUMP_TRACE)
#define TRACE(x)        printk x
#else
#define TRACE(x)
#endif

#endif /* __BCMGENET_H__ */
