/*
<:copyright-broadcom 
 
 Copyright (c) 2002 Broadcom Corporation 
 All Rights Reserved 
 No portions of this material may be reproduced in any form without the 
 written permission of: 
          Broadcom Corporation 
          16215 Alton Parkway 
          Irvine, California 92619 
 All information contained in this document is Broadcom Corporation 
 company private, proprietary, and trade secret. 
 
:>
*/
#ifndef _BCMINTENET_H_
#define _BCMINTENET_H_

#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include "linux/kernel.h"	/* For barrier() */

#if defined( CONFIG_BCMINTEMAC ) || defined( CONFIG_BCMINTEMAC_MODULE )
#include <asm/brcmstb/common/bcmtypes.h>
#include <asm/brcmstb/brcm97110/bcm97110.h>
#include <asm/brcmstb/brcm97110/bcmintrnum.h>
// already included in bcm97110.h #include <asm/brcm/board.h>
#else
#include <6345_map.h>
#endif

#include <linux/spinlock.h>
#include <linux/tqueue.h>

/*---------------------------------------------------------------------*/
/* specify number of BDs and buffers to use                            */
/*---------------------------------------------------------------------*/
#define NR_TX_BDS               100
#define NR_RX_BDS               256
#define ENET_MAX_MTU_SIZE       1518    /* Body(1500) + EH_SIZE(14) + FCS(4) */
#define DMA_MAX_BURST_LENGTH    16      /* in 32 bit words */
#define MAX_RX_BUFS             (NR_RX_BDS * 2)

#define CACHE_TO_NONCACHE(x)	KSEG1ADDR(x)
#define NONCACHE_TO_CACHE(x)	KSEG0ADDR(x)

#define ERROR(x)        printk x
#ifndef ASSERT
#define ASSERT(x)       if (x); else ERROR(("assert: "__FILE__" line %d\n", __LINE__)); 
#endif

#if defined(DUMP_TRACE)
#define TRACE(x)        printk x
#else
#define TRACE(x)
#endif

#pragma pack(1)
typedef struct Enet_Tx_CB {
    struct sk_buff      *skb;
    uint32              nrBds;          /* # of bds req'd (incl 1 for this header) */
    volatile DmaDesc    *lastBdAddr;
    struct Enet_Tx_CB   *next;          /* ptr to next header in free list */
} Enet_Tx_CB;

typedef struct Enet_Rx_Buff {
    unsigned char       dAddr[ETH_ALEN];/* destination hardware address */
    unsigned char       sAddr[ETH_ALEN];/* source hardware address */
    uint16              type;           /* Ethernet type/length */
    char                buffer[1500];   /* data */
    uint32              crc;            /* normal checksum (FCS1) */
    uint16              pad1;           /* rsvd padding */
    uint32              pad2[4];        /* Must be mult of EMAC_DMA_MAX_BURST_LENGTH */
} Enet_Rx_Buff;					 
#pragma pack()

typedef struct PM_Addr {
    uint16              valid;          /* 1 indicates the corresponding address is valid */
    unsigned char       dAddr[ETH_ALEN];/* perfect match register's destination address */
    unsigned int        ref;            /* reference count */
} PM_Addr;					 
#define MAX_PMADDR      4               /* # of perfect match address */
/*
 * device context
 */ 
typedef struct BcmEnet_devctrl {
    struct net_device *dev;             /* ptr to net_device */
    spinlock_t      lock;               /* Serializing lock */
    struct timer_list timer;            /* used by Tx reclaim */
    atomic_t        devInUsed;          /* device in used */

    struct net_device_stats stats;      /* statistics used by the kernel */

    struct sk_buff  *skb_pool[MAX_RX_BUFS]; /* rx buffer pool */
    int             nextskb;            /* next free skb in skb_pool */ 
    atomic_t        rxDmaRefill;        /* rxDmaRefill == 1 needs refill rxDma */
    int             rxbuf_assign_busy;  /* assign_rx_buffers busy */

    /* transmit variables */
    volatile DmaChannel *dmaChannels;
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

    uint16          chipId;             /* chip's id */
    uint16          chipRev;            /* step */
    int             rxIrq;              /* rx dma irq */
    int             phyAddr;            /* 1 - external MII, 0 - internal MII (default after reset) */
    PM_Addr         pmAddr[MAX_PMADDR]; /* perfect match address */
#ifdef USE_BH
    struct tq_struct task;              /* task queue */
#endif
    int             linkState;
} BcmEnet_devctrl;

// BD macros
#define IncTxBDptr(x, s) if (x == ((BcmEnet_devctrl *)s)->txLastbdPtr) \
                             x = ((BcmEnet_devctrl *)s)->txFirstbdPtr; \
                      else x++

#define IncRxBDptr(x, s) if (x == ((BcmEnet_devctrl *)s)->rxLastBdPtr) \
                             x = ((BcmEnet_devctrl *)s)->rxFirstBdPtr; \
                      else x++

/* Let irq.c hamdle it */
#define BcmHalInterruptDisable(irq) //	disable_irq(irq)
#define BcmHalInterruptEnable(irq) // 	enable_irq(irq)


#ifdef CONFIG_CPU_LITTLE_ENDIAN
/* Little endian */
#define EMAC_SWAP16(x) ( (((x)&0xff00)>>8) |(((x)&0x00ff)<<8) )
#define EMAC_SWAP32(x) ( (((x)&0xff000000)>>24) |(((x)&0x00ff0000)>>8) | (((x)&0x0000ff00)<<8)  |(((x)&0x000000ff)<<24) )

#else
/* Big Endian */
#define EMAC_SWAP16(x) (x)
#define EMAC_SWAP32(x) (x)

#endif

#define EMAC_RD_REG(r)  	((sizeof *(r) == sizeof (uint32)) \
	? (uint32)(EMAC_SWAP32(*((uint32*)(r)))) \
	: (uint16)(EMAC_SWAP16(*((uint16*)(r)))))

#define EMAC_WR_REG(r, v) do { ((sizeof *(r) == sizeof (uint32)) \
	? (*((uint32*) (r)) = (uint32) EMAC_SWAP32(v)) \
	: (*((uint16*) (r)) = (uint16) EMAC_SWAP16(v)) ); \
	barrier(); } while(0)



#endif /* _BCMINTENET_H_ */
