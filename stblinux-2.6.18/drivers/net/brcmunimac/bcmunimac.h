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
#ifndef _BCMUNIMAC_H_
#define _BCMUNIMAC_H_

#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include "linux/kernel.h"	/* For barrier() */

#include <asm/brcmstb/common/bcmtypes.h>
#include <asm/brcmstb/common/brcmstb.h>
#include "unimac_map.h"
#include "boardparms.h"

#include <linux/spinlock.h>
#ifdef CONFIG_BCMINTEMAC_NETLINK 
#include <linux/rtnetlink.h>	
#endif	

/*---------------------------------------------------------------------*/
/* specify number of BDs and buffers to use                            */
/*---------------------------------------------------------------------*/

#define TOTAL_DESC				256		/* total number of Buffer Descriptors */
//#define TOTAL_DESC				16		/* total number of Buffer Descriptors */
#define RX_RATIO				1/2		/* ratio of RX descriptors number in total */
//#define EXTRA_TX_DESC			24		/* fine adjustment in TX descriptor number */
#define EXTRA_TX_DESC			0

#define DESC_MASK				(TOTAL_DESC - 1)
#define NR_RX_BDS               ((TOTAL_DESC*RX_RATIO) - EXTRA_TX_DESC)
#define NR_TX_BDS               (TOTAL_DESC - NR_RX_BDS)

#define ENET_MAX_MTU_SIZE       1536    /* Body(1500) + EH_SIZE(14) + VLANTAG(4) + BRCMTAG(6) + FCS(4) = 1528.  1536 is multiple of 256 bytes */
#define DMA_MAX_BURST_LENGTH    0x40    /* in 32 bit words = 256 bytes  THT per David F, to allow 256B burst */

#ifdef CONFIG_BCMINTEMAC_7038_STREAMING
#define MAX_RX_BUFS             (NR_RX_BDS * 12)
#else
#define MAX_RX_BUFS             (NR_RX_BDS * 4)
#endif

#define ETH_CRC_LEN             4
/* misc. configuration */
#define DMA_FC_THRESH_LO        5
#define DMA_FC_THRESH_HI        (NR_RX_BDS / 2)

#define UMAC_RX_DESC_OFFSET   	0x2800								/* MAC DMA Rx Descriptor word */

#define UMAC_TX_DESC_OFFSET   	(UMAC_RX_DESC_OFFSET+(8*NR_RX_BDS))	/* MAC DMA Tx Descriptor word */


#define CACHE_TO_NONCACHE(x)	KSEG1ADDR(x)

#define ERROR(x)        printk x
#ifndef ASSERT
#define ASSERT(x)       if (x); else ERROR(("assert: "__FILE__" line %d\n", __LINE__)); 
#endif

#if defined(CONFIG_BCMUMAC_DUMP_TRACE)
#define TRACE(x)        printk x
#else
#define TRACE(x)
#endif

typedef struct ethsw_info_t {
    int cid;                            /* Current chip ID */
    int page;                           /* Current page */
    int type;                           /* Ethernet Switch type */
} ethsw_info_t;

#pragma pack(1)
typedef struct Enet_Tx_CB {
    struct sk_buff      *skb;
    volatile DmaDesc    *BdAddr;
    struct Enet_Tx_CB   *next;          /* ptr to next header in free list */
} Enet_Tx_CB;

#define BCMUMAC_MAX_DEVS			2
#define BCMUMAC_NO_PHY_ID			-1
#define BCMUMAC_INT_PHY_DEV			1	/*which MAC has internal PHY?*/
#define BCMUMAC_EXT_PHY_DEV			0	/*which MAC has external PHY/MoCA?*/

/* power management mode */
#define BCMUMAC_POWER_CABLE_SENSE	0
#define BCMUMAC_POWER_WOL_MAGIC		1
#define BCMUMAC_POWER_WOL_ACPI		2

/*
 * device context
 */ 
typedef struct BcmEnet_devctrl {
    struct net_device *dev;             /* ptr to net_device */
	struct net_device *next_dev;
    spinlock_t      lock;               /* Serializing lock */
    struct timer_list timer;            /* used by Tx reclaim */

    struct net_device_stats stats;      /* statistics used by the kernel */

    struct sk_buff  *skb_pool[MAX_RX_BUFS]; 	/* rx buffer pool */
    int             nextskb;            /* next free skb in skb_pool */ 
    atomic_t        rxDmaRefill;        /* rxDmaRefill == 1 needs refill rxDma */
	
	spinlock_t	bh_lock;				/* soft IRQ lock */
	volatile unsigned long rxbuf_assign_busy;

	unsigned long base_addr;			/* UNIMAC register start address. */
    volatile uniMacRegs *umac;     		/* UNIMAC register block base address */
	volatile rbufRegs	*txrx_ctrl;		/* tbuf and rbuf registers */
	volatile intrl2Regs *intrl2;		/* INTR2 registers */
	volatile epllRegs  *epll;			/* PLL registers */
    volatile DmaRegs *dmaRegs;			/* DMA registers */
	volatile unsigned long *hfb;		/* HFB registers */

    /* transmit variables */
    volatile DmaChannel *txDma;         /* location of transmit DMA register set */

    Enet_Tx_CB      *txCbPtrHead;       /* points to EnetTxCB struct head */
    Enet_Tx_CB      *txCbPtrTail;       /* points to EnetTxCB struct tail */

    Enet_Tx_CB      *txCbQHead;         /* points to EnetTxCB struct queue head */
    Enet_Tx_CB      *txCbQTail;         /* points to EnetTxCB struct queue tail */
    Enet_Tx_CB      *txCbs;             /* memory locaation of tx control block pool */

    volatile DmaDesc *txBds;            /* Memory location of tx Dma BD ring */
    volatile DmaDesc *txLastBdPtr;      /* ptr to last allocated Tx BD */
    volatile DmaDesc *txFirstBdPtr;     /* ptr to first allocated Tx BD */
    volatile DmaDesc *txNextBdPtr;      /* ptr to next Tx BD to transmit with */

    int             nrTxBds;            /* number of transmit bds */
    int             txFreeBds;          /* # of free transmit bds */

    /* receive variables */
    volatile DmaChannel *rxDma;         /* location of receive DMA register set */
    volatile DmaDesc *rxBds;            /* Memory location of rx bd ring */
    volatile DmaDesc *rxBdAssignPtr;    /* ptr to next rx bd to become full */
    volatile DmaDesc *rxBdReadPtr;      /* ptr to next rx bd to be processed */
    volatile DmaDesc *rxLastBdPtr;      /* ptr to last allocated rx bd */
    volatile DmaDesc *rxFirstBdPtr;     /* ptr to first allocated rx bd */

    int             nrRxBds;            /* number of receive bds */
    int             rxBufLen;           /* size of rx buffers for DMA */


    int             rxIrq;              /* rx dma irq */
    int             phyAddr;            /* 1 - external MII, 0 - internal MII (default after reset) */
    atomic_t        linkState;			/* 1 - linkup, 0 - link down */
	int				bIPHdrOptimize;
	int				irq_stat;			/* software copy of irq status, for botom half processing */
	struct work_struct bcmumac_task;	/* bottom half work */
	int				mii_pid;			/* mii thread pid */
    
	ETHERNET_MAC_INFO EnetInfo;
    int             devnum;				/* 0=EMAC_0, 1=EMAC_1 */
} BcmEnet_devctrl;

// BD macros
#define IncTxBDptr(x, s) if (x == ((BcmEnet_devctrl *)s)->txLastbdPtr) \
                             x = ((BcmEnet_devctrl *)s)->txFirstbdPtr; \
                      else x++

#define IncRxBDptr(x, s) if (x == ((BcmEnet_devctrl *)s)->rxLastBdPtr) \
                             x = ((BcmEnet_devctrl *)s)->rxFirstBdPtr; \
                      else x++

/* Big Endian */
#define EMAC_SWAP16(x) (x)
#define EMAC_SWAP32(x) (x)

#endif /* _BCMUNIMAC_H_ */
