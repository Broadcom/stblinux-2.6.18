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
//**************************************************************************
// File Name  : bcmunimac.c 
//
// Description: This is Linux network driver for the BCMUNIMAC
// 				internal Ethenet Controller onboard the 7420
//               
// Updates    : 09/24/2008  leisun.  Created.
//**************************************************************************

#define CARDNAME    "BCMUNIMAC"
#define VERSION     "1.0"
#define VER_STR     "v" VERSION " " __DATE__ " " __TIME__

// Turn this on to allow Hardware Multicast Filter
//#define MULTICAST_HW_FILTER

#if defined(CONFIG_MODVERSIONS) && ! defined(MODVERSIONS)
   #include <linux/modversions.h> 
   #define MODVERSIONS
#endif  

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>

#include <linux/sched.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
//#include <linux/in.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/icmp.h>

#include <asm/mipsregs.h>
#include <asm/cacheflush.h>
#include <asm/brcmstb/common/brcm-pm.h>

#include "bcmmii.h"
#include "bcmunimac.h"
#include "if_net.h"
#include "unimac_proc.h"

#include <linux/stddef.h>

extern unsigned long getPhysFlashBase(void);

#define POLLTIME_10MS		(HZ/100)	/* debug 10 ms */
#define POLLCNT_1SEC		(HZ/POLLTIME_10MS)
#define POLLCNT_FOREVER		((int) 0x80000000)

#define skb_dataref(x)		(&skb_shinfo(x)->dataref)

#define ENET_POLL_DONE     	0x80000000
#define RX_BUF_LENGTH		2048	/* 2Kb buffer */
#define SKB_ALIGNMENT		32		/* 256B alignment */
#define ISDMA_DESC_THRES	64		/* Rx Descriptor throttle threshold */
#define CLEAR_ALL_HFB		0xff

// --------------------------------------------------------------------------
//      External, indirect entry points. 
// --------------------------------------------------------------------------
static int bcmumac_enet_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
// --------------------------------------------------------------------------
//      Called for "ifconfig ethX up" & "down"
// --------------------------------------------------------------------------
static int bcmumac_net_open(struct net_device * dev);
static int bcmumac_net_close(struct net_device * dev);
// --------------------------------------------------------------------------
//      Watchdog timeout
// --------------------------------------------------------------------------
static void bcmumac_net_timeout(struct net_device * dev);
// --------------------------------------------------------------------------
//      Packet transmission. 
// --------------------------------------------------------------------------
static int bcmumac_net_xmit(struct sk_buff * skb, struct net_device * dev);
// --------------------------------------------------------------------------
//      Allow proc filesystem to query driver statistics
// --------------------------------------------------------------------------
static struct net_device_stats * bcmumac_net_query(struct net_device * dev);
// --------------------------------------------------------------------------
//      Set address filtering mode
// --------------------------------------------------------------------------
static void bcmumac_set_multicast_list(struct net_device * dev);
// --------------------------------------------------------------------------
//      Set the hardware MAC address.
// --------------------------------------------------------------------------
static int bcmumac_set_mac_addr(struct net_device *dev, void *p);

// --------------------------------------------------------------------------
//      Interrupt routines
// --------------------------------------------------------------------------
static irqreturn_t bcmumac_net_isr(int irq, void *, struct pt_regs *regs);
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
//      dev->poll() method
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
static int bcmumac_enet_poll(struct net_device * dev, int *budget);
// --------------------------------------------------------------------------
//  Bottom half service routine. Process all received packets.                  
// --------------------------------------------------------------------------
static uint32 bcmumac_rx(void *ptr, uint32 budget);

// --------------------------------------------------------------------------
//      Internal routines
// --------------------------------------------------------------------------
/* Allocate and initialize tx/rx buffer descriptor pools */
static int bcmumac_init_dev(BcmEnet_devctrl *pDevCtrl);
static int bcmumac_uninit_dev(BcmEnet_devctrl *pDevCtrl);
/* Assign the Rx descriptor ring */
static int assign_rx_buffers(BcmEnet_devctrl *pDevCtrl);
/* Initialize driver's pools of receive buffers and tranmit headers */
static int init_buffers(BcmEnet_devctrl *pDevCtrl);
/* Initialise the Ethernet Switch control registers */
static int init_umac(BcmEnet_devctrl *pDevCtrl);
/* Initialize DMA control register */
static void init_isdma(BcmEnet_devctrl *pDevCtrl);
/* Reclaim transmit frames which have been sent out */
static void tx_reclaim_timer(unsigned long arg);
/* Add a Tx control block to the pool */
static void txCb_enq(BcmEnet_devctrl *pDevCtrl, Enet_Tx_CB *txCbPtr);
/* Remove a Tx control block from the pool*/
static Enet_Tx_CB *txCb_deq(BcmEnet_devctrl *pDevCtrl);
/* Interrupt bottom-half */
static void bcmumac_task(BcmEnet_devctrl *pDevCtrl);
/* Background MII thread */
static int bcmumac_mii_thread(void * p);
/* power management */
static void bcmumac_power_down(BcmEnet_devctrl *pDevCtrl, int mode);
static void bcmumac_power_up(BcmEnet_devctrl *pDevCtrl, int mode);


#ifdef CONFIG_BCMUMAC_DUMP_DATA
/* Display hex base data */
static void dumpHexData(unsigned char *head, int len);
/* dumpMem32 dump out the number of 32 bit hex data  */
static void dumpMem32(uint32 * pMemAddr, int iNumWords);
#endif

static struct net_device *eth_root_dev = NULL;
static int DmaDescThres = ISDMA_DESC_THRES;

/* --------------------------------------------------------------------------
    Name: bcmemac_get_free_txdesc
 Purpose: Get Current Available TX desc count
-------------------------------------------------------------------------- */
int bcmemac_get_free_txdesc( struct net_device *dev ){
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    return pDevCtrl->txFreeBds;
}

struct net_device * bcmemac_get_device(void) {
	return eth_root_dev;
}
/* 
 * Increment the flowctl alloc register and desc buffer alloc register.
 */
static inline void IncAllocRegister(BcmEnet_devctrl *pDevCtrl) 
{
    volatile unsigned long* pflowAllocReg = &pDevCtrl->dmaRegs->flowctl_ch1_alloc;
    volatile unsigned long* pdescAllocReg = &pDevCtrl->dmaRegs->enet_isdma_desc_alloc;

    /* Make sure we don't go over bound */
    if (*pflowAllocReg < NR_RX_BDS) {
        *pflowAllocReg = 1;	
    }
    if (*pdescAllocReg < NR_RX_BDS) {
        *pdescAllocReg = 1;	
    }
}


#ifdef CONFIG_BCMUMAC_DUMP_DATA
/*
 * dumpHexData dump out the hex base binary data
 */
static void dumpHexData(unsigned char *head, int len)
{
    int i;
    unsigned char *curPtr = head;

    for (i = 0; i < len; ++i)
    {
        if (i % 16 == 0)
            printk("\n");       
        printk("0x%02X, ", *curPtr++);
    }
    printk("\n");

}

/*
 * dumpMem32 dump out the number of 32 bit hex data 
 */
static void dumpMem32(uint32 * pMemAddr, int iNumWords)
{
    int i = 0;
    static char buffer[80];

    sprintf(buffer, "%08X: ", (UINT)pMemAddr);
    printk(buffer);
    while (iNumWords) {
        sprintf(buffer, "%08X ", (UINT)*pMemAddr++);
        printk(buffer);
        iNumWords--;
        i++;
        if ((i % 4) == 0 && iNumWords) {
            sprintf(buffer, "\n%08X: ", (UINT)pMemAddr);
            printk(buffer);
        }
    }
    printk("\n");
}
#endif
static inline void handleAlignment(BcmEnet_devctrl *pDevCtrl, struct sk_buff* skb)
{
    /* 
	 * We request to allocate 2048 + 32 bytes of buffers, and the dev_alloc_skb() added
	 * 16B for NET_SKB_PAD, so we totally requested 2048+32+16 bytes buffer,
	 * the size was aligned to SMP_CACHE_BYTES, which is 64B.(is it?), so we 
	 * finnally ended up got 2112 bytes of buffer! Among which, the first 16B is reserved
	 * for NET_SKB_PAD, to make the skb->data aligned 32B  boundary, we should have 
	 * enough space to fullfill the 2KB buffer after alignment!
     */

    volatile unsigned long boundary32, curData, resHeader;

    curData = (unsigned long) skb->data;
    boundary32 = (curData + (SKB_ALIGNMENT - 1)) & ~(SKB_ALIGNMENT - 1);
    resHeader = boundary32 - curData ;
	/* 4 bytes for skb pointer */
	if(resHeader < 4) {
		boundary32 += 32;
	}

	resHeader = boundary32 - curData - 4;
	/* We'd have minimum 16B reserved by default. */
	skb_reserve(skb, resHeader);

	*(unsigned int *)skb->data = (unsigned int)skb;
	skb_reserve(skb, 4);
    /*
	 * Make sure it is on 32B boundary, should never happen if our
	 * calculation was correct.
	 */
    if ((unsigned long) skb->data & (SKB_ALIGNMENT - 1)) {
        printk("$$$$$$$$$$$$$$$$$$$ skb buffer is NOT aligned on %d boundary\n", SKB_ALIGNMENT);
    }

	/* 
	 *  we don't reserve 2B for IP Header optimization here,
	 *  use skb_pull when receiving packets
	 */
}
/*
 * skb_headerinit
 *
 * Reinitializes the socket buffer.  Lifted from skbuff.c
 *
 * RETURNS: None.
 */

static inline void skb_headerinit(void *p, kmem_cache_t *cache, 
        unsigned long flags)
{
    struct sk_buff *skb = p;

    skb->next = NULL;
    skb->prev = NULL;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16)
    skb->list = NULL;
    skb->stamp.tv_sec=0;    /* No idea about time */
    skb->security = 0;  /* By default packets are insecure */
#endif
    skb->sk = NULL;
    skb->dev = NULL;
    skb->dst = NULL;
    /* memset(skb->cb, 0, sizeof(skb->cb)); */
    skb->pkt_type = PACKET_HOST;    /* Default type */
    skb->ip_summed = 0;
    skb->priority = 0;
    skb->destructor = NULL;

#ifdef CONFIG_NETFILTER
    skb->nfmark = 0;
    skb->nfct = NULL;
#ifdef CONFIG_NETFILTER_DEBUG
    skb->nf_debug = 0;
#endif
#endif
#ifdef CONFIG_NET_SCHED
    skb->tc_index = 0;
#endif
}

/* --------------------------------------------------------------------------
    Name: bcmumac_net_open
 Purpose: Open and Initialize the EMAC on the chip
-------------------------------------------------------------------------- */
static int bcmumac_net_open(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    ASSERT(pDevCtrl != NULL);


    TRACE(("%s: bcmumac_net_open, umac->cmd=%lx, RxDMA=%u, rxDma.cfg=%lu\n", 
                dev->name, pDevCtrl->umac->cmd, (int)pDevCtrl->rxDma,
		pDevCtrl->rxDma->cfg));

    /* disable ethernet MAC while updating its registers */
    pDevCtrl->umac->cmd &= ~(CMD_TX_EN | CMD_RX_EN);

    pDevCtrl->dmaRegs->controller_cfg |= ISDMA_ENABLE;         

	
    pDevCtrl->rxDma->cfg |= DMA_ENABLE;

	//enable_irq(pDevCtrl->rxIrq);

    pDevCtrl->timer.expires = jiffies + POLLTIME_10MS;
    add_timer_on(&pDevCtrl->timer, 0);

    // Start the network engine
    netif_start_queue(dev);
    
	pDevCtrl->umac->cmd |= (CMD_TX_EN | CMD_RX_EN);

    return 0;
}


/* --------------------------------------------------------------------------
    Name: bcmumac_net_close
 Purpose: Stop communicating with the outside world
    Note: Caused by 'ifconfig ethX down'
-------------------------------------------------------------------------- */
static int bcmumac_net_close(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    Enet_Tx_CB *txCBPtr;
	int timeout = 0;

    ASSERT(pDevCtrl != NULL);

    TRACE(("%s: bcmumac_net_close\n", dev->name));

    netif_stop_queue(dev);

	/* Stop Tx DMA */
	pDevCtrl->txDma->cfg = DMA_PKT_HALT;
	while(timeout < 5000)
	{
		if(!(pDevCtrl->txDma->cfg & DMA_PKT_HALT))
			break;
		udelay(1);
		timeout++;
	}
	if(timeout == 5000)
		printk(KERN_ERR "Timed out while shutting down Tx DMA\n");

	/* Disable Rx DMA*/
    pDevCtrl->rxDma->cfg = DMA_PKT_HALT;
	timeout = 0;
	while(timeout < 5000)
	{
		if(!(pDevCtrl->rxDma->cfg & DMA_PKT_HALT))
			break;
		udelay(1);
		timeout++;
	}
	if(timeout == 5000)
		printk(KERN_ERR "Timed out while shutting down Tx DMA\n");

    pDevCtrl->umac->cmd &= ~(CMD_RX_EN | CMD_TX_EN);

    //disable_irq(pDevCtrl->rxIrq);

    del_timer_sync(&pDevCtrl->timer);

    /* free the skb in the txCbPtrHead */
    while (pDevCtrl->txCbPtrHead)  {
        pDevCtrl->txFreeBds += 1;

        if(pDevCtrl->txCbPtrHead->skb)
            dev_kfree_skb (pDevCtrl->txCbPtrHead->skb);

        txCBPtr = pDevCtrl->txCbPtrHead;

        /* Advance the current reclaim pointer */
        pDevCtrl->txCbPtrHead = pDevCtrl->txCbPtrHead->next;

        /* Finally, return the transmit header to the free list */
        txCb_enq(pDevCtrl, txCBPtr);
    }
    /* Adjust the tail if the list is empty */
    if(pDevCtrl->txCbPtrHead == NULL)
        pDevCtrl->txCbPtrTail = NULL;

    //pDevCtrl->txNextBdPtr = pDevCtrl->txFirstBdPtr = pDevCtrl->txBds;
    return 0;
}

/* --------------------------------------------------------------------------
    Name: bcmumac_net_timeout
 Purpose: 
-------------------------------------------------------------------------- */
static void bcmumac_net_timeout(struct net_device * dev)
{
    ASSERT(dev != NULL);

    TRACE(("%s: bcmumac_net_timeout\n", dev->name));

    dev->trans_start = jiffies;

    netif_wake_queue(dev);
}

/* --------------------------------------------------------------------------
    Name: bcmumac_set_multicast_list
 Purpose: Set the multicast mode, ie. promiscuous or multicast
-------------------------------------------------------------------------- */
static void bcmumac_set_multicast_list(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	struct dev_mc_list * dmi;
	int i;
#define MAX_MC_COUNT	16

    TRACE(("%s: bcm_set_multicast_list: %08X\n", dev->name, dev->flags));

    /* Promiscous mode */
    if (dev->flags & IFF_PROMISC) {
		pDevCtrl->umac->cmd |= CMD_PROMISC;   
    } else {
		pDevCtrl->umac->cmd &= ~CMD_PROMISC;
    }

	/* UniMac doesn't support ALLMULTI */
	if (dev->flags & IFF_ALLMULTI)
		return;
	
	if (dev->mc_count == 0 || dev->mc_count > MAX_MC_COUNT) {
		/* Disable MDF */
		pDevCtrl->umac->mdf_ctrl = 0;
		return;
	}
	/* update MDF filter */
	i = 0;
	pDevCtrl->umac->mdf_ctrl = 0;
	for(dmi = dev->mc_list; dmi; dmi = dmi->next) {
		pDevCtrl->umac->mdf_addr[i] = *(unsigned short *)&dmi->dmi_addr[4];
		pDevCtrl->umac->mdf_addr[i+1] = *(unsigned long *)&dmi->dmi_addr[0];
		pDevCtrl->umac->mdf_ctrl |= (1 << i);
	}
	/* finally, get ourself in*/
	pDevCtrl->umac->mdf_addr[i] = *(unsigned short*)&dev->dev_addr[4];
	pDevCtrl->umac->mdf_addr[i+1] = *(unsigned long*)&dev->dev_addr[0];
	pDevCtrl->umac->mdf_ctrl |= (1 << i);
}

/* --------------------------------------------------------------------------
    Name: bcmumac_net_query
 Purpose: Return the current statistics. This may be called with the card 
          open or closed.
-------------------------------------------------------------------------- */
static struct net_device_stats *
bcmumac_net_query(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;

    ASSERT(pDevCtrl != NULL);

    return &(pDevCtrl->stats);
}

/*
 * tx_reclaim_timer: reclaim transmited skb, refill rx bd if required.
 */
static void tx_reclaim_timer(unsigned long arg)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)arg;
    int bdfilled;

    if (atomic_read(&pDevCtrl->rxDmaRefill) != 0) {
        atomic_set(&pDevCtrl->rxDmaRefill, 0);
        /* assign packet buffers to all available Dma descriptors */
        bdfilled = assign_rx_buffers(pDevCtrl);
		TRACE(("%s: bdfilled=%d\n", __FUNCTION__, bdfilled));
    }

    /* Reclaim transmit Frames which have been sent out */
    bcmumac_net_xmit(NULL, pDevCtrl->dev);

    pDevCtrl->timer.expires = jiffies + POLLTIME_10MS;
    add_timer_on(&pDevCtrl->timer, 0);
}


/*
 * txCb_enq: add a Tx control block to the pool
 */
static void txCb_enq(BcmEnet_devctrl *pDevCtrl, Enet_Tx_CB *txCbPtr)
{
    txCbPtr->next = NULL;

    if (pDevCtrl->txCbQTail) {
        pDevCtrl->txCbQTail->next = txCbPtr;
        pDevCtrl->txCbQTail = txCbPtr;
    }
    else {
        pDevCtrl->txCbQHead = pDevCtrl->txCbQTail = txCbPtr;
    }
}

/*
 * txCb_deq: remove a Tx control block from the pool
 */
static Enet_Tx_CB *txCb_deq(BcmEnet_devctrl *pDevCtrl)
{
    Enet_Tx_CB *txCbPtr;

    if ((txCbPtr = pDevCtrl->txCbQHead)) {
        pDevCtrl->txCbQHead = txCbPtr->next;
        txCbPtr->next = NULL;

        if (pDevCtrl->txCbQHead == NULL)
            pDevCtrl->txCbQTail = NULL;
    }
    else {
        txCbPtr = NULL;
    }
    return txCbPtr;
}

/* --------------------------------------------------------------------------
 Name: bcmemac_xmit_check
 Purpose: Reclaims TX descriptors
-------------------------------------------------------------------------- */
int bcmemac_xmit_check(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    Enet_Tx_CB *txCBPtr;
    Enet_Tx_CB *txedCBPtr;
    unsigned long flags,ret;

    ASSERT(pDevCtrl != NULL);

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */
    spin_lock_irqsave(&pDevCtrl->lock, flags);
        
    txCBPtr = NULL;

    /* Reclaim transmitted buffers */
    while (pDevCtrl->txCbPtrHead)  {
		if(pDevCtrl->txCbPtrHead->BdAddr == (pDevCtrl->txFirstBdPtr + (pDevCtrl->txDma->descOffset>>1)))
			break;
        pDevCtrl->txFreeBds += 1;

        if(pDevCtrl->txCbPtrHead->skb) {
            dev_kfree_skb_any (pDevCtrl->txCbPtrHead->skb);
        }

        txedCBPtr = pDevCtrl->txCbPtrHead;

        /* Advance the current reclaim pointer */
        pDevCtrl->txCbPtrHead = pDevCtrl->txCbPtrHead->next;

        /* 
         * Finally, return the transmit header to the free list.
         * But keep one around, so we don't have to allocate again
         */
        txCb_enq(pDevCtrl, txedCBPtr);
    }

    /* Adjust the tail if the list is empty */
    if(pDevCtrl->txCbPtrHead == NULL)
        pDevCtrl->txCbPtrTail = NULL;

    netif_wake_queue(dev);

    if(pDevCtrl->txCbQHead && (pDevCtrl->txFreeBds>0))
        ret = 0;
    else
        ret = 1;

    spin_unlock_irqrestore(&pDevCtrl->lock, flags);
    return ret;
}
/* Get ulp (upper layer protocol) information for tx checksum offloading*/
static inline int bcmumac_get_ulpinfo(struct sk_buff * skb, int * ulp_offset, int * csum_offset)
{
	int transport_proto, iph_len;
	struct iphdr * iph = (struct iphdr *)(skb->data + ETH_HLEN);
	if(iph->version == 4) {
		*ulp_offset = ETH_HLEN + sizeof(struct iphdr);
		transport_proto = iph->protocol;
		iph_len = sizeof(struct iphdr);
	}else if(iph->version == 6) {
		struct ipv6hdr * ipv6h = skb->nh.ipv6h;
		*ulp_offset = ETH_HLEN + sizeof(struct ipv6hdr);
		transport_proto = ipv6h->nexthdr;
		iph_len = sizeof(struct ipv6hdr);
	}else {
		printk(KERN_CRIT "bcmumac_net_xmit attemping to insert csum for non-IP packet\n");
		return -1;
	}
	switch(transport_proto)
	{
		case IPPROTO_TCP:
			*csum_offset = ETH_HLEN + iph_len + (int)&(((struct tcphdr *)0)->check);
			break;
		case IPPROTO_UDP:
			*csum_offset = ETH_HLEN + iph_len + (int)&(((struct udphdr *)0)->check);
			break;
		case IPPROTO_ICMP:
			*csum_offset = ETH_HLEN + iph_len + (int)&(((struct icmphdr*)0)->checksum);
			break;
		default:
			printk(KERN_CRIT "bcmumac_net_xmit unknown protocol %d \n", transport_proto);
			return -1;
			break;
	}
	return 0;
}
/* --------------------------------------------------------------------------
    Name: bcmumac_net_xmit
 Purpose: Send ethernet traffic
-------------------------------------------------------------------------- */
static int bcmumac_net_xmit(struct sk_buff * skb, struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    Enet_Tx_CB *txCBPtr;
    Enet_Tx_CB *txedCBPtr;
    unsigned long flags;
#ifdef CONFIG_BCMUMAC_TX_CSUM
	TSB * tsb;
	struct iphdr * iph;
	int ulp_offset, csum_offset;
#endif
    ASSERT(pDevCtrl != NULL);

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */
    spin_lock_irqsave(&pDevCtrl->lock, flags);
        
    txCBPtr = NULL;
    /* Reclaim transmitted buffers */
    while (pDevCtrl->txCbPtrHead )
	{
		
		//if(pDevCtrl->txCbPtrHead->BdAddr == (pDevCtrl->txFirstBdPtr + (pDevCtrl->txDma->descOffset >> 1)) ||
		if(EMAC_SWAP32(pDevCtrl->txCbPtrHead->BdAddr->length_status)&DMA_OWN)
		{
			break;
		}
        
		pDevCtrl->txFreeBds += 1;

        if(pDevCtrl->txCbPtrHead->skb) {
            dev_kfree_skb_any (pDevCtrl->txCbPtrHead->skb);
        }

        txedCBPtr = pDevCtrl->txCbPtrHead;

        /* Advance the current reclaim pointer */
        pDevCtrl->txCbPtrHead = pDevCtrl->txCbPtrHead->next;

        /* 
         * Finally, return the transmit header to the free list.
         * But keep one around, so we don't have to allocate again
         */
        if (txCBPtr == NULL) {
            txCBPtr = txedCBPtr;
        }
        else {
            txCb_enq(pDevCtrl, txedCBPtr);
        }
    }

    /* Adjust the tail if the list is empty */
    if(pDevCtrl->txCbPtrHead == NULL)
        pDevCtrl->txCbPtrTail = NULL;

    if (skb == NULL) {
        if (txCBPtr != NULL) {
            txCb_enq(pDevCtrl, txCBPtr);
        }
		netif_wake_queue(dev);
        spin_unlock_irqrestore(&pDevCtrl->lock, flags);
        return 0;
    }

    TRACE(("bcmumac_net_xmit, txCfg=%08x\n", (unsigned int)pDevCtrl->txDma->cfg));
    if (txCBPtr == NULL) {
        txCBPtr = txCb_deq(pDevCtrl);
    }

    /*
     * Obtain a transmit header from the free list.  If there are none
     * available, we can't send the packet. Discard the packet.
     */
    if (txCBPtr == NULL) {
        netif_stop_queue(dev);
        spin_unlock_irqrestore(&pDevCtrl->lock, flags);
        return 1;
    }

    txCBPtr->skb = skb;

    /* If we could not queue this packet, free it */
    if (pDevCtrl->txFreeBds == 0) {
		printk("no free txBds\n");
        TRACE(("%s: bcmumac_net_xmit no free txBds\n", dev->name));
		goto err_out;
    }


    /* Get the BD , used to  enqueue the packet */
    txCBPtr->BdAddr = pDevCtrl->txNextBdPtr;
	/* 
	 * If L4 checksum offloading enabled, must make sure skb has
	 * enough headroom for us to insert 64B status block.
	 */
#ifdef CONFIG_BCMUMAC_TX_CSUM
	if(skb->ip_summed  == CHECKSUM_HW)
	{
		pDevCtrl->txrx_ctrl->tbuf_ctrl |= RBUF_64B_EN;
		if(skb_headroom(skb) < 64) {
			printk(KERN_ERR "%s: bcmumac_net_ximt no enough headroom for HW checksum\n", dev->name);
			goto err_out;
		}
		/* ipv6 or ipv4? only support IP , this is ugly!*/
		if(bcmumac_get_ulpinfo(skb, &ulp_offset, &csum_offset) < 0)
		{
			printk(KERN_ERR "%s: bcmumac_net_ximt Failed to get ulp info\n", dev->name);
			goto err_out;
		}
	
		/* Insert 64B TSB and set the flag */
		skb_push(skb, 64);
		tsb = (TSB *)skb->data;
		tsb->length_status = (ulp_offset << TSB_ULP_SHIFT) | csum_offset | TSB_LV; 
		TRACE(("TSB->length_status=0x%08lX csum=0x%04x\n", tsb->length_status,
					*(unsigned short*)(skb->data+64+csum_offset)));
	}else {
		pDevCtrl->txrx_ctrl->tbuf_ctrl &= ~RBUF_64B_EN;
	}
#endif	/* CONFIG_BCMUMAC_TX_CSUM */

    /*
     * Add the buffer to the ring.
     * Set addr and length of DMA BD to be transmitted.
     */
    dma_cache_wback_inv((unsigned long)skb->data, skb->len);

    txCBPtr->BdAddr->address = EMAC_SWAP32((uint32)virt_to_phys(skb->data));
    txCBPtr->BdAddr->length_status  = EMAC_SWAP32((((unsigned long)((skb->len < ETH_ZLEN) ? ETH_ZLEN : skb->len))<<16));
#ifdef CONFIG_BCMUMAC_DUMP_DATA
    printk("bcmumac_net_xmit: len %d", skb->len);
    dumpHexData(skb->data, skb->len);
#endif
    /*
     * Advance BD pointer to next in the chain.
     */
    if (pDevCtrl->txNextBdPtr == pDevCtrl->txLastBdPtr) {
        pDevCtrl->txNextBdPtr->length_status |= EMAC_SWAP32(DMA_WRAP);
        pDevCtrl->txNextBdPtr = pDevCtrl->txFirstBdPtr;
    }
    else {
		/* what is this?*/
        pDevCtrl->txNextBdPtr->length_status |= EMAC_SWAP32(0);
        pDevCtrl->txNextBdPtr++;
    }
#ifdef CONFIG_BCMUMAC_TX_CSUM
	if(skb->ip_summed  == CHECKSUM_HW)
		txCBPtr->BdAddr->length_status |= EMAC_SWAP32(DMA_TX_DO_CSUM);
#endif
	/* DEBUG , insert QTAG for MoCA */
	if(skb->len > 500)
		txCBPtr->BdAddr->length_status |= EMAC_SWAP32(1);
	else
		txCBPtr->BdAddr->length_status |= EMAC_SWAP32(10);
    /*
     * This tells the switch that it can transmit this frame.
     */
    txCBPtr->BdAddr->length_status |= EMAC_SWAP32(DMA_OWN | DMA_SOP | DMA_EOP | DMA_TX_APPEND_CRC);

    /* Decrement total BD count */
    pDevCtrl->txFreeBds -= 1;

    if ( (pDevCtrl->txFreeBds == 0) || (pDevCtrl->txCbQHead == NULL) ) {
        TRACE(("%s: bcmumac_net_xmit no transmit queue space -- stopping queues\n", dev->name));
        netif_stop_queue(dev);
    }
    /*
     * Packet was enqueued correctly.
     * Advance to the next Enet_Tx_CB needing to be assigned to a BD
     */
    txCBPtr->next = NULL;
    if(pDevCtrl->txCbPtrHead == NULL) {
        pDevCtrl->txCbPtrHead = txCBPtr;
        pDevCtrl->txCbPtrTail = txCBPtr;
    }
    else {
        pDevCtrl->txCbPtrTail->next = txCBPtr;
        pDevCtrl->txCbPtrTail = txCBPtr;
    }

	TRACE(("%s: BdAddr->Length_status=0x%08x BdAddr->address=0x%08x txNextBdPtr=0x%p\n", __FUNCTION__,
				txCBPtr->BdAddr->length_status,
				txCBPtr->BdAddr->address,
				pDevCtrl->txNextBdPtr));
    /* Enable DMA for this channel */
    pDevCtrl->txDma->cfg |= DMA_ENABLE;

    /* update stats */
    pDevCtrl->stats.tx_bytes += ((skb->len < ETH_ZLEN) ? ETH_ZLEN : skb->len);
    pDevCtrl->stats.tx_bytes += 4;
    pDevCtrl->stats.tx_packets++;

    dev->trans_start = jiffies;

    spin_unlock_irqrestore(&pDevCtrl->lock, flags);

    return 0;
err_out:
	txCb_enq(pDevCtrl, txCBPtr);
	//netif_stop_queue(dev);
	spin_unlock_irqrestore(&pDevCtrl->lock, flags);
	return 0;
}

/* --------------------------------------------------------------------------
    Name: bcmumac_xmit_fragment
 Purpose: Send ethernet traffic Buffer DESC and submit to UDMA
-------------------------------------------------------------------------- */
int bcmemac_xmit_fragment( int ch, unsigned char *buf, int buf_len, 
                           unsigned long tx_flags , struct net_device *dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    Enet_Tx_CB *txCBPtr;
    volatile DmaDesc *firstBdPtr;
	
    ASSERT(pDevCtrl != NULL);
    txCBPtr = txCb_deq(pDevCtrl);
    /*
     * Obtain a transmit header from the free list.  If there are none
     * available, we can't send the packet. Discard the packet.
     */
    if (txCBPtr == NULL) {
        return 1;
    }

    //txCBPtr->nrBds = 1;
    txCBPtr->skb = NULL;

    /* If we could not queue this packet, free it */
    if (pDevCtrl->txFreeBds == 0) {
        TRACE(("%s: bcmumac_net_xmit no txFreeBds\n", dev->name));
        txCb_enq(pDevCtrl, txCBPtr);
        return 1;
    }

	/*--------first fragment------*/
    firstBdPtr = pDevCtrl->txNextBdPtr;
    /* store off the last BD address used to enqueue the packet */
    txCBPtr->BdAddr = pDevCtrl->txNextBdPtr;

    /* assign skb data to TX Dma descriptor */
    /*
     * Add the buffer to the ring.
     * Set addr and length of DMA BD to be transmitted.
     */
    txCBPtr->BdAddr->address = EMAC_SWAP32((uint32)virt_to_phys(buf));
    txCBPtr->BdAddr->length_status  = EMAC_SWAP32((((unsigned long)buf_len)<<16));	
    /*
     * Set status of DMA BD to be transmitted and
     * advance BD pointer to next in the chain.
     */
    if (pDevCtrl->txNextBdPtr == pDevCtrl->txLastBdPtr) {
        pDevCtrl->txNextBdPtr->length_status |= EMAC_SWAP32(DMA_WRAP);
        pDevCtrl->txNextBdPtr = pDevCtrl->txFirstBdPtr;
    }
    else {
        pDevCtrl->txNextBdPtr++;
    }
    /*
     * Turn on the "OWN" bit in the first buffer descriptor
     * This tells the switch that it can transmit this frame.
     */	
    txCBPtr->BdAddr->length_status &= ~EMAC_SWAP32(DMA_SOP |DMA_EOP | DMA_TX_APPEND_CRC);
    txCBPtr->BdAddr->length_status |= EMAC_SWAP32(DMA_OWN | tx_flags | DMA_TX_APPEND_CRC);

   
    /* Decrement total BD count */
    pDevCtrl->txFreeBds -= 1;

	/*
     * Packet was enqueued correctly.
     * Advance to the next Enet_Tx_CB needing to be assigned to a BD
     */
    txCBPtr->next = NULL;
    if(pDevCtrl->txCbPtrHead == NULL) {
        pDevCtrl->txCbPtrHead = txCBPtr;
        pDevCtrl->txCbPtrTail = txCBPtr;
    }
    else{
        pDevCtrl->txCbPtrTail->next = txCBPtr;
        pDevCtrl->txCbPtrTail = txCBPtr;
    }

    /* Enable DMA for this channel */
    pDevCtrl->txDma->cfg |= DMA_ENABLE;

   /* update stats */
    pDevCtrl->stats.tx_bytes += buf_len; //((skb->len < ETH_ZLEN) ? ETH_ZLEN : skb->len);
    pDevCtrl->stats.tx_bytes += 4;
    pDevCtrl->stats.tx_packets++;

    dev->trans_start = jiffies;


    return 0;
}

EXPORT_SYMBOL(bcmemac_xmit_fragment);

/* --------------------------------------------------------------------------
    Name: bcmemac_xmit_multibuf
 Purpose: Send ethernet traffic in multi buffers (hdr, buf, tail)
-------------------------------------------------------------------------- */
int bcmemac_xmit_multibuf( int ch, unsigned char *hdr, int hdr_len, unsigned char *buf, 
		int buf_len, unsigned char *tail, int tail_len, struct net_device *dev)
{
	unsigned long flags;
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    
	while(bcmemac_xmit_check(dev));

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */
    spin_lock_irqsave(&pDevCtrl->lock, flags);

    /* Header + Optional payload in two parts */
    if((hdr_len> 0) && (buf_len > 0) && (tail_len > 0) && (hdr) && (buf) && (tail)){ 
        /* Send Header */
        while(bcmemac_xmit_fragment( ch, hdr, hdr_len, DMA_SOP, dev))
            bcmemac_xmit_check(dev);
        /* Send First Fragment */  
        while(bcmemac_xmit_fragment( ch, buf, buf_len, 0, dev))
            bcmemac_xmit_check(dev);
        /* Send 2nd Fragment */ 	
        while(bcmemac_xmit_fragment( ch, tail, tail_len, DMA_EOP, dev))
            bcmemac_xmit_check(dev);
    }
    /* Header + Optional payload */
    else if((hdr_len> 0) && (buf_len > 0) && (hdr) && (buf)){
        /* Send Header */
        while(bcmemac_xmit_fragment( ch, hdr, hdr_len, DMA_SOP, dev))
            bcmemac_xmit_check(dev);
        /* Send First Fragment */
        while(bcmemac_xmit_fragment( ch, buf, buf_len, DMA_EOP, dev))
            bcmemac_xmit_check(dev);
    }
    /* Header Only (includes payload) */
    else if((hdr_len> 0) && (hdr)){ 
        /* Send Header */
        while(bcmemac_xmit_fragment( ch, hdr, hdr_len, DMA_SOP | DMA_EOP, dev))
            bcmemac_xmit_check(dev);
    }
    else{
    	spin_unlock_irqrestore(&pDevCtrl->lock, flags);
        return 0; /* Drop the packet */
    }
    spin_unlock_irqrestore(&pDevCtrl->lock, flags);
    return 0;
}

/* NAPI polling method*/
static int bcmumac_enet_poll(struct net_device * dev, int * budget)
{   
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	volatile intrl2Regs * intrl2 = pDevCtrl->intrl2;
    
	uint32 work_to_do = min(dev->quota, *budget);
    uint32 work_done;
    uint32 ret_done;

    work_done = bcmumac_rx(pDevCtrl, work_to_do);
	TRACE(("%s: work_done=0x%x\n", __FUNCTION__, work_done));
    ret_done = work_done & ENET_POLL_DONE;
    work_done &= ~ENET_POLL_DONE;

    *budget -= work_done;
    dev->quota -= work_done;

    if (work_done < work_to_do && ret_done != ENET_POLL_DONE) {
       /* Did as much as could, but we are not done yet */
       return 1;
    }

    /* We are done */
    netif_rx_complete(dev);

    /* Reschedule if there are more packets on the DMA ring to be received. */
    if( (((EMAC_SWAP32(pDevCtrl->rxBdReadPtr->length_status)) & 0xffff) & DMA_OWN) == 0 ) {
		TRACE(("Reschdule rx\n"));
        netif_rx_reschedule(pDevCtrl->dev, work_done); /* ???? */
    }
    else {
		intrl2->cpu_mask_clear |= UMAC_IRQ_RXDMA_BDONE;
		TRACE(("%s:Enabling  RXDMA_BDONE interrupt\n", __FUNCTION__));
    }

    return 0;
}

/*
 * Interrupt bottom half
 */
static void bcmumac_task(BcmEnet_devctrl *pDevCtrl)
{
	spin_lock_bh(&pDevCtrl->bh_lock);

	TRACE(("%s\n", __FUNCTION__));
	/* Cable plugged/unplugged event */
	if (pDevCtrl->irq_stat & UMAC_IRQ_PHY_DET_R) {
		pDevCtrl->irq_stat &= ~UMAC_IRQ_PHY_DET_R;
		printk(KERN_CRIT "Cable plugged in UMAC%d powering up\n", pDevCtrl->devnum);
		bcmumac_power_up(pDevCtrl, BCMUMAC_POWER_CABLE_SENSE);
		/* restart auto-neg, program speed/duplex/pause info into umac */
		if (!(pDevCtrl->umac->cmd & CMD_AUTO_CONFIG)) {
			mii_setup(pDevCtrl->dev);
		}

	}else if (pDevCtrl->irq_stat & UMAC_IRQ_PHY_DET_F) {
		pDevCtrl->irq_stat &= ~UMAC_IRQ_PHY_DET_F;
		printk(KERN_CRIT "Cable unplugged in UMAC%d powering down\n", pDevCtrl->devnum);
		bcmumac_power_down(pDevCtrl, BCMUMAC_POWER_CABLE_SENSE);
	}
	if (pDevCtrl->irq_stat & UMAC_IRQ_MPD_R) {
		pDevCtrl->irq_stat &= ~UMAC_IRQ_MPD_R;
		printk(KERN_CRIT "Magic packet detected, UMAC%d waking up\n", pDevCtrl->devnum);
		/* disable mpd interrupt */
		pDevCtrl->intrl2->cpu_mask_set |= UMAC_IRQ_MPD_R;
		/* disable CRC forward.*/
		pDevCtrl->umac->cmd &= ~CMD_CRC_FWD;
		bcmumac_power_up(pDevCtrl, BCMUMAC_POWER_WOL_MAGIC);
		
	}else if (pDevCtrl->irq_stat & (UMAC_IRQ_HFB_SM | UMAC_IRQ_HFB_MM)) {
		pDevCtrl->irq_stat &= ~(UMAC_IRQ_HFB_SM | UMAC_IRQ_HFB_MM);
		printk(KERN_CRIT "ACPI pattern matched, UMAC%d waking up\n", pDevCtrl->devnum);
		/* disable HFB match interrupts */
		pDevCtrl->intrl2->cpu_mask_set |= (UMAC_IRQ_HFB_SM | UMAC_IRQ_HFB_MM);
		bcmumac_power_up(pDevCtrl, BCMUMAC_POWER_WOL_ACPI);
	}

	/* Link UP/DOWN event */
	if(pDevCtrl->irq_stat & UMAC_IRQ_LINK_UP)
	{
		printk(KERN_CRIT "%s Link UP.\n", pDevCtrl->dev->name);
		/* Clear soft-copy of irq status*/
		pDevCtrl->irq_stat &= ~UMAC_IRQ_LINK_UP;
		pDevCtrl->rxDma->cfg |= DMA_ENABLE;
		
#ifdef CONFIG_BCMINTEMAC_NETLINK
		if(atomic_read(&pDevCtrl->LinkState) == 0) {
			rtnl_lock();
			pDevCtrl->dev->flags |= IFF_RUNNING;
			netif_carrier_on(pDevCtrl->dev);
			rtmsg_ifinfo(RTM_NEWLINK, pDevCtrl->dev, IFF_RUNNING);
			rtnl_unlock();
		}
		atomic_set(&pDevCtrl->LinkState, 1);
#endif
	}else if (pDevCtrl->irq_stat & UMAC_IRQ_LINK_DOWN)
	{
		printk(KERN_CRIT "%s Link DOWN.\n", pDevCtrl->dev->name);
		pDevCtrl->irq_stat &= ~UMAC_IRQ_LINK_DOWN;	/* clear soft-copy of irq status */
		/* Disable DMA Rx channels.  */
		pDevCtrl->rxDma->cfg &= ~DMA_ENABLE;
		/* In case there are packets in the Rx descriptor */
#ifdef CONFIG_BCMINTEMAC_NETLINK
		if(atomic_read(&pDevCtrl->LinkState) == 1) {
			rtnl_lock()
			clear_bit(__LINK_STATE_START, &pDevCtrl->dev->state);
			netif_carrier_off(pDevCtrl->dev);
			pDevCtrl->dev->flags &= ~IFF_RUNNING;
			rtmsg_ifinfo(RTM_DELLLINK, pDevCtrl->dev, IFF_RUNNING);
			set_bit(__LINK_STATE_START, &pDevCtrl->dev->state);
			rtnl_unlock();
		}
		atomic_set(&pDevCtrl->LinkState, 0);
#endif
	}

	spin_unlock_bh(&pDevCtrl->bh_lock);
}
/*
 * bcmumac_net_isr: Handle various interrupts
 */
static irqreturn_t bcmumac_net_isr(int irq, void * dev_id, struct pt_regs * regs)
{
    BcmEnet_devctrl *pDevCtrl = dev_id;
	volatile intrl2Regs * intrl2 = pDevCtrl->intrl2;
	
	/* Save irq status for bottom-half processing. */
	pDevCtrl->irq_stat = intrl2->cpu_stat;
	/* clear inerrupts*/
	intrl2->cpu_clear |= pDevCtrl->irq_stat;

	TRACE(("IRQ=0x%x\n", pDevCtrl->irq_stat));
#ifndef CONFIG_BCMUMAC_RX_DESC_THROTTLE
	if (pDevCtrl->irq_stat & (UMAC_IRQ_RXDMA_BDONE|UMAC_IRQ_RXDMA_PDONE)) {
		/*
		 * We use NAPI(software interrupt throttling, if
		 * Rx Descriptor throttling is not used.
		 * Disable interrupt, will be enabled in the poll method.
		 */
		pDevCtrl->irq_stat &= ~(UMAC_IRQ_RXDMA_BDONE | UMAC_IRQ_RXDMA_PDONE);
		if(!netif_running(pDevCtrl->dev))
		{
			/* 
			 * interface is not up, received broadcast packet.
			 * if we can't schdule, simply ack the IRQ and wait for next time.
			 */
			if(__netif_rx_schedule_prep(pDevCtrl->dev))
			{
				intrl2->cpu_mask_set |= UMAC_IRQ_RXDMA_BDONE;
				TRACE(("%s:Disabling RXDMA_BDONE interrupt\n", __FUNCTION__));
				__netif_rx_schedule(pDevCtrl->dev);
			}
		}else
		{
			intrl2->cpu_mask_set |= UMAC_IRQ_RXDMA_BDONE;
			TRACE(("%s:Disabling RXDMA_BDONE interrupt\n", __FUNCTION__));
			/* interface is up, received both broadcast and unicast packet*/
			netif_rx_schedule(pDevCtrl->dev);
		}
	}
#else
	/* Multiple buffer done event. */
	if(pDevCtrl->irq_stat & UMAC_IRQ_DESC_THROT)
	{
		int budget;
		pDevCtrl->irq_stat &= ~UMAC_IRQ_DESC_THROT;
		budget = pDevCtrl->dmaRegs->enet_isdma_desc_thres;
		TRACE(("%s: %d packets avaiable\n", __FUNCTION__, budget));
		bcmumac_rx(pDevCtrl, budget);
	}
#endif
	if(pDevCtrl->irq_stat & (UMAC_IRQ_PHY_DET_R |
				UMAC_IRQ_PHY_DET_F |
				UMAC_IRQ_LINK_UP |
				UMAC_IRQ_LINK_DOWN |
				UMAC_IRQ_HFB_SM |
				UMAC_IRQ_HFB_MM |
				UMAC_IRQ_MPD_R) )
	{
		/* all other interested interrupts handled in bottom half */

		schedule_work(&pDevCtrl->bcmumac_task);
	}

    return IRQ_HANDLED;
}
/*
 *  bcmumac_rx: Process all received packets.
 *  this could be called from bottom half, or 
 *  from NAPI poll method.
 */
#define MAX_RX                  0x0fffffff
static uint32 bcmumac_rx(void *ptr, uint32 budget)
{
    BcmEnet_devctrl *pDevCtrl = ptr;
    struct sk_buff *skb;
    unsigned long dmaFlag;
    unsigned char *pBuf;
    int len;
    int bdfilled;
    uint32 rxpktgood = 0;
    uint32 rxpktprocessed = 0;
	uint32 rxpkttoprocess = 0;
	int offset = pDevCtrl->rxDma->descOffset >> 1;	/* offset increment by 2 for each packet? */

	if(pDevCtrl->rxBdReadPtr > (pDevCtrl->rxFirstBdPtr + offset) )
	{
		/* wrapped*/
		rxpkttoprocess = (uint32)(pDevCtrl->rxLastBdPtr - pDevCtrl->rxBdReadPtr);
		TRACE(("wrapped rxpkttoprocess=%d\n", rxpkttoprocess));
		rxpkttoprocess += 1;	/* current rxBdReadPtr need to be processed */
		rxpkttoprocess += offset;
	}else
	{
		rxpkttoprocess = (uint32)((pDevCtrl->rxFirstBdPtr + offset) - pDevCtrl->rxBdReadPtr) ;
		TRACE(("no wrap rxpkttoprocess=%d\n", rxpkttoprocess));
		rxpkttoprocess += 1;	/* current rxBdReadPtr need to be processed */
	}

	TRACE(("bcmumac_rx: budget=%d pkttoprocess=%d descOffset=%d rxBdReadPtr=0x%x rxFirstBdPtr=0x%x\n", 
				budget, rxpkttoprocess, pDevCtrl->rxDma->descOffset, pDevCtrl->rxBdReadPtr, pDevCtrl->rxFirstBdPtr));

	while((rxpktprocessed < rxpkttoprocess)
			&& (rxpktprocessed < budget) )
	{
    	dmaFlag = ((EMAC_SWAP32(pDevCtrl->rxBdReadPtr->length_status)) & 0xffff);

		if (dmaFlag & DMA_OWN) {
			TRACE(("Rx overrun?\n"));
			break;
		}
		rxpktprocessed++;
        /*
		 * Stop when we hit a buffer with no data, or a BD with no buffer 
		 * This implies that we caught up with DMA, or that we haven't been
		 * able to fill the buffers.
		 */
        if ( (pDevCtrl->rxBdReadPtr->address == (uint32) NULL)) {
			TRACE(("address is NULL\n"));
            break;
        }
		/* report errors */
        if (dmaFlag & (DMA_RX_CRC_ERROR | DMA_RX_OV | DMA_RX_NO | DMA_RX_LG |DMA_RX_RXER)) {
			TRACE(("ERROR: dmaFlag=0x%x\n", dmaFlag));
            if (dmaFlag & DMA_RX_CRC_ERROR) {
                pDevCtrl->stats.rx_crc_errors++;
            }
            if (dmaFlag & DMA_RX_OV) {
                pDevCtrl->stats.rx_over_errors++;
            }
            if (dmaFlag & DMA_RX_NO) {
                pDevCtrl->stats.rx_frame_errors++;
            }
            if (dmaFlag & DMA_RX_LG) {
                pDevCtrl->stats.rx_length_errors++;
            }
            pDevCtrl->stats.rx_dropped++;
            pDevCtrl->stats.rx_errors++;

			/* discard the packet and advance read ptr.*/
        	pBuf = (unsigned char *)(phys_to_virt(EMAC_SWAP32(pDevCtrl->rxBdReadPtr->address)));
        	pDevCtrl->rxBdReadPtr->length_status &= EMAC_SWAP32(0xffff0000); //clear status.
			pDevCtrl->rxBdReadPtr->address = 0;
			skb = (struct sk_buff *)*(unsigned long *)(pBuf - 4);
			atomic_set(&skb->users,1);
			
			if (atomic_read(&pDevCtrl->rxDmaRefill) == 0) {
				bdfilled = assign_rx_buffers(pDevCtrl);
			}
			/* Advance BD ptr to next in ring */
			IncRxBDptr(pDevCtrl->rxBdReadPtr, pDevCtrl);
    		dmaFlag = ((EMAC_SWAP32(pDevCtrl->rxBdReadPtr->length_status)) & 0xffff);

			/* process next packet */
			continue;

        }/* if error packet */

        pBuf = (unsigned char *)(phys_to_virt(EMAC_SWAP32(pDevCtrl->rxBdReadPtr->address)));

        /*
         * THT: Invalidate the RAC cache again, since someone may have read near the vicinity
         * of the buffer.  This is necessary because the RAC cache is much larger than the CPU cache
         */
        len = ((EMAC_SWAP32(pDevCtrl->rxBdReadPtr->length_status))>>16);

		brcm_inv_prefetch((unsigned long)pBuf, len);

        /* Null the BD field to prevent reuse */
        pDevCtrl->rxBdReadPtr->length_status &= EMAC_SWAP32(0xffff0000); //clear status.
        pDevCtrl->rxBdReadPtr->address = 0;

        /* Advance BD ptr to next in ring */
        IncRxBDptr(pDevCtrl->rxBdReadPtr, pDevCtrl);
        /* Recover the SKB pointer saved during assignment.*/
        skb = (struct sk_buff *)*(unsigned long *)(pBuf-4);
    	
		dmaFlag = ((EMAC_SWAP32(pDevCtrl->rxBdReadPtr->length_status)) & 0xffff);
		
		if(pDevCtrl->txrx_ctrl->rbuf_ctrl & RBUF_64B_EN)
		{
			/* we have 64B rx status block enabled.*/
			if(pDevCtrl->txrx_ctrl->rbuf_chk_ctrl & RBUF_RXCHK_EN)
			{
				RSB * rsb = (RSB *)(skb->data);
				{
					struct iphdr * iph = (struct iphdr *)(skb->data + 64 + ETH_HLEN + 2);
					if(iph->version == 4)
					{
						skb->csum = (~rsb->csum) & RSB_CSUM_MASK;
						/*should swap bytes based on rbuf->endian_ctrl */
						skb->csum = ((skb->csum << 8) & 0xFF00) | ((skb->csum >> 8) & 0xFF);
					}

				}
				skb->ip_summed = CHECKSUM_HW;
				skb_pull(skb, 64);
				len -= 64;
			}
		}

		if(pDevCtrl->bIPHdrOptimize)
		{
			skb_pull(skb, 2);
			len -= 2;
		}

		if(pDevCtrl->umac->cmd &CMD_CRC_FWD)
		{
			skb_trim(skb, len - 4);
			len -= 4;
		}else
			skb_trim(skb, len);
#ifdef CONFIG_BCMUMAC_DUMP_DATA
        printk("bcmumac_rx :");
        dumpHexData(skb->data, 32);
#endif

        /* Finish setting up the received SKB and send it to the kernel */
        skb->dev = pDevCtrl->dev;
        skb->protocol = eth_type_trans(skb, pDevCtrl->dev);

        /* Allocate a new SKB for the ring */
        if (atomic_read(&pDevCtrl->rxDmaRefill) == 0) {
            bdfilled = assign_rx_buffers(pDevCtrl);
        }

        pDevCtrl->stats.rx_packets++;
        pDevCtrl->stats.rx_bytes += len;
		if(dmaFlag & DMA_RX_MULT)
			pDevCtrl->stats.multicast++;
        rxpktgood++;

        /* Notify kernel */
#ifdef CONFIG_BCMUMAC_RX_DESC_THROTTLE
		netif_rx(skb);
#else
        netif_receive_skb(skb);
#endif
        TRACE(("pushed up to kernel\n"));
        if (--budget <= 0) {
            break;
        }
    }

    if (dmaFlag & DMA_OWN) {
        rxpktgood |= ENET_POLL_DONE;
    }
    pDevCtrl->dev->last_rx = jiffies;

    return rxpktgood;
}


/*
 * Set the hardware MAC address.
 */
static int bcmumac_set_mac_addr(struct net_device *dev, void *p)
{
    struct sockaddr *addr=p;

    if(netif_running(dev))
        return -EBUSY;

    memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

    return 0;
}

/*
 * assign_rx_buffers: 
 * Assign skb to RX DMA descriptor. If no free SKB available, set the flag,
 * refill the descriptor when tx_reclaim_timer runs.
 *
 */
static int assign_rx_buffers(BcmEnet_devctrl *pDevCtrl)
{
    struct sk_buff *skb;
    uint16  bdsfilled=0;
	int i;
	unsigned long flags;
	
	TRACE(("%s\n", __FUNCTION__));

    /*
     * This function may be called from timer or irq bottom-half.
     */
#ifndef CONFIG_BCMUMAC_RX_DESC_THROTTLE
	spin_lock_bh(&pDevCtrl->bh_lock);
    //if(test_and_set_bit(0, &pDevCtrl->rxbuf_assign_busy)) {
    //    return bdsfilled;
	//}
#else
	spin_lock_irqsave(&pDevCtrl->lock, flags);
#endif

    /* loop here for each buffer needing assign */
    for (;;)
    {
        /* exit if no receive buffer descriptors are in "unused" state */
        if(EMAC_SWAP32(pDevCtrl->rxBdAssignPtr->address) != 0)
        {
            break;
        }

        skb = pDevCtrl->skb_pool[pDevCtrl->nextskb++];
        if (pDevCtrl->nextskb == MAX_RX_BUFS)
            pDevCtrl->nextskb = 0;

        /* check if skb is free */
        if (skb_shared(skb) || 
            atomic_read(skb_dataref(skb)) > 1) {
            /* find next free skb */
            for (i = 0; i < MAX_RX_BUFS; i++) {
                skb = pDevCtrl->skb_pool[pDevCtrl->nextskb++];
                if (pDevCtrl->nextskb == MAX_RX_BUFS)
                    pDevCtrl->nextskb = 0;
                if ((skb_shared(skb) == 0) && 
                    atomic_read(skb_dataref(skb)) <= 1) {
                    /* found a free skb */
                    break;
                }
            }
			if(i == MAX_RX_BUFS) {
				/* No free skb avaiable now, set the flag and let the timer function to refill. */
				atomic_set(&pDevCtrl->rxDmaRefill, 1);
				break;
			}
        }

        atomic_set(&pDevCtrl->rxDmaRefill, 0);
        skb_headerinit(skb, NULL, 0);  /* clean state */

        /* init the skb, in case other app. modified the skb pointer */
        skb->data = skb->tail = skb->head;
        skb->end = skb->data + (skb->truesize - sizeof(struct sk_buff));
        skb->len = 0;
        skb->data_len = 0;
        skb->cloned = 0;

        handleAlignment(pDevCtrl, skb);

        skb_put(skb, pDevCtrl->rxBufLen);

        /*
         * Set the user count to two so when the upper layer frees the
         * socket buffer, only the user count is decremented.
         */
		if(atomic_read(&skb->users) == 1)
        	atomic_inc(&skb->users);

        /* kept count of any BD's we refill */
        bdsfilled++;

        dma_cache_wback_inv((unsigned long)skb->data, pDevCtrl->rxBufLen);


        /* assign packet, prepare descriptor, and advance pointer */
        pDevCtrl->rxBdAssignPtr->address = EMAC_SWAP32((uint32)virt_to_phys(skb->data));
        pDevCtrl->rxBdAssignPtr->length_status  = EMAC_SWAP32((pDevCtrl->rxBufLen<<16));

        IncAllocRegister(pDevCtrl);

        /* turn on the newly assigned BD for DMA to use */
        if (pDevCtrl->rxBdAssignPtr == pDevCtrl->rxLastBdPtr) {
            pDevCtrl->rxBdAssignPtr->length_status |= EMAC_SWAP32(DMA_OWN | DMA_WRAP);
#if 0
			TRACE(("rxBdAssignPtr=0x%08x, length_status=0x%08x address=0x%08x\n", pDevCtrl->rxBdAssignPtr,
					pDevCtrl->rxBdAssignPtr->length_status ,
					pDevCtrl->rxBdAssignPtr->address));
#endif
            pDevCtrl->rxBdAssignPtr = pDevCtrl->rxFirstBdPtr;
        }
        else {
            pDevCtrl->rxBdAssignPtr->length_status |= EMAC_SWAP32(DMA_OWN);
#if 0
			TRACE(("rxBdAssignPtr=0x%08x, length_status=0x%08x address=0x%08x\n", pDevCtrl->rxBdAssignPtr,
					pDevCtrl->rxBdAssignPtr->length_status ,
					pDevCtrl->rxBdAssignPtr->address));
#endif
            pDevCtrl->rxBdAssignPtr++;
        }
    }

	/* Enable rx DMA incase it was disabled due to running out of rx BD */
    pDevCtrl->rxDma->cfg |= DMA_ENABLE;

#ifndef CONFIG_BCMUMAC_RX_DESC_THROTTLE
	spin_unlock_bh(&pDevCtrl->bh_lock);
    //clear_bit(0, &pDevCtrl->rxbuf_assign_busy);
#else
	spin_unlock_irqrestore(&pDevCtrl->lock, flags);
#endif

	TRACE(("%s return bdsfilled=%d\n", __FUNCTION__, bdsfilled));
    return bdsfilled;
}


/*
 * init_umac: Initializes the uniMac controller
 */
static int init_umac(BcmEnet_devctrl *pDevCtrl)
{
    volatile uniMacRegs *umac;
	volatile intrl2Regs *intrl2; 
	struct net_device * dev = pDevCtrl->dev;
	
	umac = pDevCtrl->umac;;
	intrl2 = pDevCtrl->intrl2;

    TRACE(("bcmumac: init_umac "));

    /* disable MAC while updating its registers */
    umac->cmd = 0 ;

    /* issue soft reset, wait for it to complete */
    umac->cmd = CMD_SW_RESET;
	udelay(1000);
	umac->cmd = 0;
    /* clear tx/rx counter */
    umac->mib_ctrl = MIB_RESET_RX | MIB_RESET_TX | MIB_RESET_RUNT;
	umac->mib_ctrl = 0;

#ifdef MAC_LOOPBACK
	/* Enable GMII/MII loopback */
    umac->cmd |= CMD_LCL_LOOP_EN;
#endif
	umac->max_frame_len = ENET_MAX_MTU_SIZE;
#if 0
	/* 
	 * mii_init() will program speed/duplex/pause 
	 * parameters into umac and rbuf registers.
	 */
    if (mii_init(pDevCtrl->dev))
        return -EFAULT;
#endif
	pDevCtrl->mii_pid = 0;

	if((pDevCtrl->mii_pid = kernel_thread(bcmumac_mii_thread, (void*)pDevCtrl, CLONE_FS | CLONE_FILES)) < 0)
	{
		printk(KERN_ERR "%s: Failed to create thread for MII \n", __FUNCTION__);
		return -ENOMEM;
	}
    /* 
	 * init rx registers, enable ip header optimization.
	 */
    if (pDevCtrl->bIPHdrOptimize) {
        pDevCtrl->txrx_ctrl->rbuf_ctrl |= RBUF_ALIGN_2B ;
    }
	umac->mac_0 = dev->dev_addr[0] << 24 | dev->dev_addr[1] << 16 | dev->dev_addr[2] << 8 | dev->dev_addr[3];
	umac->mac_1 = dev->dev_addr[4] << 8 | dev->dev_addr[5];
	
#ifdef CONFIG_BCMUMAC_RX_CSUM
	/* Do this for little-endian mode */
	pDevCtrl->txrx_ctrl->rbuf_endian_ctrl &= ~RBUF_ENDIAN_NOSWAP;
	/* Enable/disable rx checksum in ethtool functions. */
#endif
#ifdef CONFIG_BCMUMAC_TX_CSUM
	/* do this for little-endian mode.*/
	pDevCtrl->txrx_ctrl->tbuf_endian_ctrl &= ~RBUF_ENDIAN_NOSWAP;
#endif
	/* Mask all interrupts.*/
	intrl2->cpu_mask_set = 0xFFFFFFFF;
	intrl2->cpu_clear = 0xFFFFFFFF;
	intrl2->cpu_mask_clear = 0x0;

#ifdef CONFIG_BCMUMAC_RX_DESC_THROTTLE
	intrl2->cpu_mask_clear |= UMAC_IRQ_DESC_THROT;
#else
	intrl2->cpu_mask_clear |= UMAC_IRQ_RXDMA_BDONE;
	TRACE(("%s:Enabling RXDMA_BDONE interrupt\n", __FUNCTION__));
#endif /* CONFIG_BCMUMAC_RX_DESC_THROTTLE */
    
	/* Monitor cable plug/unpluged event for internal PHY */
	if (pDevCtrl->EnetInfo.PhyType == BP_ENET_INTERNAL_PHY ||
		pDevCtrl->EnetInfo.PhyType == BP_ENET_EXTERNAL_PHY ||
		pDevCtrl->EnetInfo.PhyType == BP_ENET_EXTERNAL_GPHY ) 
	{
		intrl2->cpu_mask_clear |= UMAC_IRQ_PHY_DET_R | UMAC_IRQ_PHY_DET_F;
		intrl2->cpu_mask_clear |= UMAC_IRQ_LINK_DOWN | UMAC_IRQ_LINK_UP ;
	}

	/* Enable rx/tx engine.*/
	//umac->cmd |= CMD_TX_EN | CMD_RX_EN;

	TRACE(("done init umac\n"));

    return 0;

}

/*
 * init_isdma: Initialize DMA control register
 */
static void init_isdma(BcmEnet_devctrl *pDevCtrl)
{
#ifdef CONFIG_BCMUMAC_RX_DESC_THROTTLE
	int speeds[] = {10, 100, 1000, 2500};
	int speed_id = 1;
#endif
    TRACE(("bcmumac: init_dma\n"));

    /*
     * initialize ISDMA controller register.
     */
    pDevCtrl->dmaRegs->controller_cfg = DMA_FLOWC_CH1_EN;
    pDevCtrl->dmaRegs->flowctl_ch1_thresh_lo = DMA_FC_THRESH_LO;
    pDevCtrl->dmaRegs->flowctl_ch1_thresh_hi = DMA_FC_THRESH_HI;
    
	/* Tx*/
    pDevCtrl->txDma->cfg = 0;       /* initialize first (will enable later) */
    pDevCtrl->txDma->maxBurst = DMA_MAX_BURST_LENGTH;
	pDevCtrl->txDma->descPtr = (uint32)CPHYSADDR(pDevCtrl->txFirstBdPtr);

    /* Rx */
    pDevCtrl->rxDma->cfg = 0; 
    pDevCtrl->rxDma->maxBurst = DMA_MAX_BURST_LENGTH; 
    pDevCtrl->rxDma->descPtr = (uint32)CPHYSADDR(pDevCtrl->rxFirstBdPtr);

#ifdef  CONFIG_BCMUMAC_RX_DESC_THROTTLE
	/* 
	 * Use descriptor throttle, fire interrupt only when multiple packets are done!
	 */
	pDevCtrl->dmaRegs->enet_isdma_desc_thres = ISDMA_DESC_THRES;
	/* 
	 * Enable push timer, that is, force the IRQ_DESC_THROT to fire when timeout
	 * occcred, to prevent system slow reponse when handling low throughput data.
	 */
	speed_id = (pDevCtrl->umac->cmd >> CMD_SPEED_SHIFT) & CMD_SPEED_MASK;
	pDevCtrl->dmaRegs->enet_isdma_desc_timeout = 2*(pDevCtrl->dmaRegs->enet_isdma_desc_thres*ENET_MAX_MTU_SIZE)/speeds[speed_id];
#endif	/* CONFIG_BCMUMAC_RX_DESC_THROTTLE */
}

/*
 *  init_buffers: initialize driver's pools of receive buffers
 *  and tranmit headers
 */
static int init_buffers(BcmEnet_devctrl *pDevCtrl)
{
    struct sk_buff *skb;
    int bdfilled;
    int i;

	TRACE(("%s\n", __FUNCTION__));
    /* set initial state of all BD pointers to top of BD ring */
    pDevCtrl->txCbPtrHead = pDevCtrl->txCbPtrTail = NULL;

    /* allocate recieve buffer pool */
    for (i = 0; i < MAX_RX_BUFS; i++) {
        /* allocate a new SKB for the ring */
        skb = dev_alloc_skb(pDevCtrl->rxBufLen + SKB_ALIGNMENT);
        if (skb == NULL)
        {
            printk(KERN_NOTICE CARDNAME": Low memory.\n");
            break;
        }
        /* save skb pointer */
        pDevCtrl->skb_pool[i] = skb;
    }

    if (i < MAX_RX_BUFS) {
        /* release allocated receive buffer memory */
        for (i = 0; i < MAX_RX_BUFS; i++) {
            if (pDevCtrl->skb_pool[i] != NULL) {
                dev_kfree_skb (pDevCtrl->skb_pool[i]);
                pDevCtrl->skb_pool[i] = NULL;
            }
        }
        return -ENOMEM;
    }
    /* init the next free skb index */
    pDevCtrl->nextskb = 0;
    atomic_set(&pDevCtrl->rxDmaRefill, 0);
    clear_bit(0, &pDevCtrl->rxbuf_assign_busy);

    /* assign packet buffers to all available Dma descriptors */
    bdfilled = assign_rx_buffers(pDevCtrl);
    if (bdfilled > 0) {
        TRACE(("init_buffers: %d descriptors initialized\n", bdfilled));
    }
    // This avoid depending on flowclt_alloc which may go negative during init
    pDevCtrl->dmaRegs->flowctl_ch1_alloc = DMA_CH1_FLOW_ALLOC_FORCE | bdfilled;
    //pDevCtrl->dmaRegs->enet_isdma_desc_alloc = DMA_CH1_FLOW_ALLOC_FORCE | bdfilled;
    pDevCtrl->rxDma->cfg |= DMA_ENABLE;

    TRACE(("init_buffers: %d descriptors initialized, from flowctl\n",
    		pDevCtrl->dmaRegs->flowctl_ch1_alloc));

    return 0;
}

/*
 * bcmumac_init_dev: initialize uniMac devie
 * allocate Tx/Rx buffer descriptors pool, Tx control block pool.
 */
static int bcmumac_init_dev(BcmEnet_devctrl *pDevCtrl)
{
    int i;
    int nrCbs;
    void *p;
    Enet_Tx_CB *txCbPtr;

	TRACE(("%s\n", __FUNCTION__));
    /* setup buffer/pointer relationships here */
    pDevCtrl->nrTxBds = NR_TX_BDS;
    pDevCtrl->nrRxBds = NR_RX_BDS;
	/* Always use 2KB buffer for 7420*/
    pDevCtrl->rxBufLen = RX_BUF_LENGTH;

	/* Get register block locations */
	pDevCtrl->umac = (uniMacRegs*)(pDevCtrl->dev->base_addr);
    pDevCtrl->dmaRegs = (DmaRegs *)(pDevCtrl->dev->base_addr + UMAC_DMA_REG_OFFSET);
	pDevCtrl->txrx_ctrl = (rbufRegs *)(pDevCtrl->dev->base_addr + UMAC_TXRX_REG_OFFSET);
	pDevCtrl->intrl2 = (intrl2Regs *)(pDevCtrl->dev->base_addr + UMAC_INTRL2_REG_OFFSET);
	pDevCtrl->epll = (epllRegs *)(pDevCtrl->dev->base_addr + UMAC_EPLL_REG_OFFSET);
	pDevCtrl->hfb = (unsigned long*)(pDevCtrl->dev->base_addr + UMAC_HFB_OFFSET);

    /* init rx/tx dma channels */
    pDevCtrl->rxDma = &pDevCtrl->dmaRegs->rx_chan;
    pDevCtrl->txDma = &pDevCtrl->dmaRegs->tx_chan;
    pDevCtrl->rxBds = (DmaDesc *) (pDevCtrl->dev->base_addr + UMAC_RX_DESC_OFFSET);
    pDevCtrl->txBds = (DmaDesc *) (pDevCtrl->dev->base_addr + UMAC_TX_DESC_OFFSET);
	TRACE(("%s: rxbds=0x%08x txbds=0x%08x\n", __FUNCTION__, pDevCtrl->rxBds, pDevCtrl->txBds));
	
    /* alloc space for the tx control block pool */
    nrCbs = pDevCtrl->nrTxBds; 
    if (!(p = kmalloc(nrCbs*sizeof(Enet_Tx_CB), GFP_KERNEL))) {
        return -ENOMEM;
    }
    memset(p, 0, nrCbs*sizeof(Enet_Tx_CB));
    pDevCtrl->txCbs = (Enet_Tx_CB *)p;

    /* initialize rx ring pointer variables. */
    pDevCtrl->rxBdAssignPtr = pDevCtrl->rxBdReadPtr =
                pDevCtrl->rxFirstBdPtr = pDevCtrl->rxBds;
    pDevCtrl->rxLastBdPtr = pDevCtrl->rxFirstBdPtr + pDevCtrl->nrRxBds - 1;
	TRACE(("%s: rxFirstBdPtr=0x%08x rxLastBdPtr=0x%08x rxBdAssignPtr=0x%08x\n", __FUNCTION__, 
				pDevCtrl->rxFirstBdPtr, pDevCtrl->rxLastBdPtr, pDevCtrl->rxBdAssignPtr));

    /* init the receive buffer descriptor ring */
    for (i = 0; i < pDevCtrl->nrRxBds; i++)
    {
        (pDevCtrl->rxFirstBdPtr + i)->length_status = EMAC_SWAP32((pDevCtrl->rxBufLen<<16));
        (pDevCtrl->rxFirstBdPtr + i)->address = EMAC_SWAP32(0);
    }
    pDevCtrl->rxLastBdPtr->length_status |= EMAC_SWAP32(DMA_WRAP);

    /* init transmit buffer descriptor variables */
    pDevCtrl->txNextBdPtr = pDevCtrl->txFirstBdPtr = pDevCtrl->txBds;
    pDevCtrl->txLastBdPtr = pDevCtrl->txFirstBdPtr + pDevCtrl->nrTxBds - 1;

    /* clear the transmit buffer descriptors */
    for (i = 0; i < pDevCtrl->nrTxBds; i++)
    {
        (pDevCtrl->txFirstBdPtr + i)->length_status = EMAC_SWAP32((0<<16));
        (pDevCtrl->txFirstBdPtr + i)->address = EMAC_SWAP32(0);
    }
    pDevCtrl->txLastBdPtr->length_status |= EMAC_SWAP32(DMA_WRAP);
    pDevCtrl->txFreeBds = pDevCtrl->nrTxBds;

    /* initialize the receive buffers and transmit headers */
    if (init_buffers(pDevCtrl)) {
        kfree((void *)pDevCtrl->txCbs);
        return -ENOMEM;
    }

    for (i = 0; i < nrCbs; i++)
    {
        txCbPtr = pDevCtrl->txCbs + i;
        txCb_enq(pDevCtrl, txCbPtr);
    }


    /* init umac registers */
    if (init_umac(pDevCtrl)) {
        kfree((void *)pDevCtrl->txCbs);
        return -EFAULT;
    }
    
	/* init dma registers */
    init_isdma(pDevCtrl);

	TRACE(("%s returned\n", __FUNCTION__));
    /* if we reach this point, we've init'ed successfully */
    return 0;
}

#ifdef CONFIG_BCMUMAC_USE_PROC
/*
 * display/decode Rx BD status
 */
int bcmumac_display_rxbds(char * buf, unsigned long * addr)
{
	int i;
	int len = 0;
	
	len = sprintf(buf, "Addr      Len O L F W FI   B M LG NO RxER CRC OV B_Addr\n");
	for ( i = 0 ; i < NR_RX_BDS; i ++) {
		len += sprintf(buf+len, "0x%08lX %03lx %1u %1u %1u %1u 0x%02lX %1u %1u %1u %1u %1u %1u %1u 0x%08lX\n",
			(uint32)addr,
			(*addr >> DMA_BUFLENGTH_SHIFT)&DMA_BUFLENGTH_MASK,
			(*addr & DMA_OWN)>0? 1:0,
			(*addr & DMA_EOP)>0? 1:0,
			(*addr & DMA_SOP)>0? 1:0,
			(*addr & DMA_WRAP)>0? 1:0,
			(*addr >> DMA_RX_FI_SHIFT) & DMA_RX_FI_MASK,
			(*addr & DMA_RX_BRDCAST)>0? 1:0,
			(*addr & DMA_RX_MULT)>0? 1:0,
			(*addr & DMA_RX_LG)>0? 1:0,
			(*addr & DMA_RX_NO)>0? 1:0,
			(*addr & DMA_RX_RXER)>0? 1:0,
			(*addr & DMA_RX_CRC_ERROR)>0? 1:0,
			(*addr & DMA_RX_OV)>0? 1:0,
			*(addr+1)
			),
		addr += 2;
	}
	return len;
}
/*
 * display/decode Tx BD status.
 */
int bcmumac_display_txbds(char *buf, unsigned long * addr)
{
	int i;
	int len = 0;
	len = sprintf(buf, "Addr       Len O L F W UR   A_CRC O_CRC CSUM QTAG B_Addr\n");
	for ( i = 0 ; i < NR_TX_BDS; i ++) {
		len += sprintf(buf+len, "0x%08lX %03lx %1u %1u %1u %1u %1u %1u %1u %1u 0x%02lX 0x%08lX\n",
			(uint32)addr,
			(*addr >> DMA_BUFLENGTH_SHIFT)&DMA_BUFLENGTH_MASK,
			(*addr & DMA_OWN)>0? 1:0,
			(*addr & DMA_EOP)>0? 1:0,
			(*addr & DMA_SOP)>0? 1:0,
			(*addr & DMA_WRAP)>0? 1:0,
			(*addr & DMA_TX_UNDERRUN)>0? 1:0,
			(*addr & DMA_TX_APPEND_CRC)>0? 1:0,
			(*addr & DMA_TX_OW_CRC)>0? 1:0,
			(*addr & DMA_TX_DO_CSUM)>0? 1:0,
			*addr & DMA_TX_QTAG_MASK,
			*(addr+1)
			),
		addr += 2;
	}
	return len;
}
/*
 * display MDF content
 */
int  bcmumac_display_mdf(char * buf, unsigned long * addr)
{
	int i ;
	int len = 0;
	for (i = 0; i < 34; i += 2) {
		len += sprintf(buf+len, "%02lx %02lx %02lx %02lx %02lx %02lx\n",
				(*(addr) >> 8) & 0xFF,
				(*addr) & 0xFF,
				(*(addr+1) >> 24) & 0xFF,
				(*(addr+1) >> 16) & 0xFF,
				(*(addr+1) >> 8 ) & 0xFF,
				(*(addr+1)) & 0xFF
			   );
	}
	return len;
}
/* 
 * disaplay HFB memory content.
 */
int bcmumac_display_hfb(char * buf, unsigned long * addr)
{
	int i;
	int len = 0;
	for (i = 0 ; i < 1024; i++) {
		len += sprintf(buf+len, "%01lx %04lx\n", (*addr >> 16) & 0xF,
			*addr & 0xFFFF);
	}
	return len;
}
/* 
 * Special proc entries for debug,
 * other entries are generated by perl script.
 */
umacProcEntry umac0_debug_entries[] = {
	{0, &bcmumac_display_rxbds, 0,S_IRUSR|S_IRGRP|S_IROTH, 0,0,0, "uniMacRegs/rxbds"},
	{0, &bcmumac_display_txbds, 0,S_IRUSR|S_IRGRP|S_IROTH,0,0,0, "uniMacRegs/txbds"},
	{0, &bcmumac_display_mdf, 0,S_IRUSR|S_IRGRP|S_IROTH, 0,0,0, "uniMacRegs/mdf"},
	{0, &bcmumac_display_hfb, 0,S_IRUSR|S_IRGRP|S_IROTH, 0,0,0, "hfb"}
};
umacProcEntry umac1_debug_entries[] = {
	{0, &bcmumac_display_rxbds, 0,S_IRUSR|S_IRGRP|S_IROTH, 0,0,0, "uniMacRegs/rxbds"},
	{0, &bcmumac_display_txbds, 0,S_IRUSR|S_IRGRP|S_IROTH,0,0,0, "uniMacRegs/txbds"},
	{0, &bcmumac_display_mdf, 0,S_IRUSR|S_IRGRP|S_IROTH, 0,0,0, "uniMacRegs/mdf"},
	{0, &bcmumac_display_hfb, 0,S_IRUSR|S_IRGRP|S_IROTH, 0,0,0, "hfb"}
};
/*
 * initialize and create proc entries
 */
static void bcmumac_init_proc(BcmEnet_devctrl *pDevCtrl)
{
	int i;
	char filename[32];
	umacProcEntry * entries ;
	umacProcEntry * dbg_entries;

	if(pDevCtrl->devnum == 0)
	{
		entries = &umac0_proc_entries[0];
		dbg_entries = &umac0_debug_entries[0];
	}else
	{
		entries = &umac1_proc_entries[0];
		dbg_entries = &umac1_debug_entries[0];
	}
	
	sprintf(filename, "/proc/umac%d/", pDevCtrl->devnum);
	TRACE(("creating proc for %s\n", filename));

	for (i = 0 ; i < sizeof(umac0_proc_entries)/sizeof(umacProcEntry); i++)
	{
		entries[i].context = (void*)pDevCtrl;
		entries[i].fn = NULL;
		if(bcmumac_proc_entry_create(filename, &entries[i]) < 0)
			printk(KERN_ERR "%s: Failed to create proc entry for %s\n", __FUNCTION__, 
					entries[i].name);
	}
	for (i = 0 ; i < sizeof(umac0_debug_entries)/sizeof(umacProcEntry); i++)
	{
		if( strstr(dbg_entries[i].name, "rxbds") != NULL)
			dbg_entries[i].context = (void*)(pDevCtrl->rxFirstBdPtr);
		else if(strstr(dbg_entries[i].name, "txbds") != NULL)
			dbg_entries[i].context = (void*)(pDevCtrl->txFirstBdPtr);

		if(bcmumac_proc_entry_create(filename, &dbg_entries[i]) < 0)
			printk(KERN_ERR "%s: Failed to create proc entry for %s\n", __FUNCTION__, 
					dbg_entries[i].name);
	}
	
}

/* remove proc entries */
static void bcmumac_uninit_proc(BcmEnet_devctrl * pDevCtrl)
{
	int i;
	char filename[32];
	umacProcEntry * entries;
	umacProcEntry * dbg_entries;

	if(pDevCtrl->devnum == 0)
	{
		entries = &umac0_proc_entries[0];
		dbg_entries = &umac0_debug_entries[0];
	}else
	{
		entries = &umac1_proc_entries[0];
		dbg_entries = &umac1_debug_entries[0];
	}	

	sprintf(filename, "/proc/umac%d/", pDevCtrl->devnum);

	for (i = 0 ; i < sizeof(umac0_proc_entries)/sizeof(umacProcEntry); i++)
	{
		bcmumac_proc_entry_remove(filename, &entries[i]);
	}
	for (i = 0 ; i < sizeof(umac0_debug_entries)/sizeof(umacProcEntry); i++)
	{
		bcmumac_proc_entry_remove(filename, &dbg_entries[i]);
	}
}
#endif	/* CONFIG_BCMUMAC_USE_PROC */

static void bcmumac_null_setup(struct net_device *dev)
{
}

/* Uninitialize tx/rx buffer descriptor pools */
static int bcmumac_uninit_dev(BcmEnet_devctrl *pDevCtrl)
{
    Enet_Tx_CB *txCBPtr;
    int i;

    if (pDevCtrl) {
        /* disable DMA */
        pDevCtrl->txDma->cfg = 0;
        pDevCtrl->rxDma->cfg = 0;

        /* free the irq */
        if (pDevCtrl->rxIrq)
        {
            //pDevCtrl->dmaRegs->enet_iudma_r5k_irq_msk &= ~0x2;
            disable_irq(pDevCtrl->rxIrq);
            free_irq(pDevCtrl->rxIrq, pDevCtrl);
        }

        /* free the skb in the txCbPtrHead */
        while (pDevCtrl->txCbPtrHead)  {
            pDevCtrl->txFreeBds += 1;

            if(pDevCtrl->txCbPtrHead->skb)
                dev_kfree_skb (pDevCtrl->txCbPtrHead->skb);

            txCBPtr = pDevCtrl->txCbPtrHead;

            /* Advance the current reclaim pointer */
            pDevCtrl->txCbPtrHead = pDevCtrl->txCbPtrHead->next;

            /* Finally, return the transmit header to the free list */
            txCb_enq(pDevCtrl, txCBPtr);
        }

#ifdef CONFIG_BCMUMAC_USE_PROC
	//bcmumac_proc_entry_remove("/proc/umacInfo", &debug_entries[i]);
#endif
        /* release allocated receive buffer memory */
        for (i = 0; i < MAX_RX_BUFS; i++) {
            if (pDevCtrl->skb_pool[i] != NULL) {
				/* skb->users was increased by 1 in assign_rx_buffers,we force to 
				 * decrease it before free the skb, this prevent memory leaks!
				 */
				if(atomic_read(&(pDevCtrl->skb_pool[i]->users)) > 1)
				{
					atomic_dec(&(pDevCtrl->skb_pool[i]->users));
				}
                dev_kfree_skb (pDevCtrl->skb_pool[i]);
                pDevCtrl->skb_pool[i] = NULL;
            }
        }
        /* free the transmit buffer descriptor */
        if (pDevCtrl->txBds) {
            pDevCtrl->txBds = NULL;
        }
        /* free the receive buffer descriptor */
        if (pDevCtrl->rxBds) {
            pDevCtrl->rxBds = NULL;
        }
        /* free the transmit control block pool */
        if (pDevCtrl->txCbs) {
            kfree(pDevCtrl->txCbs);
            pDevCtrl->txCbs = NULL;
        }
#ifdef CONFIG_BCMUMAC_USE_PROC
		(void)bcmumac_uninit_proc(pDevCtrl);
#endif

        if (pDevCtrl->dev) {
            if (pDevCtrl->dev->reg_state != NETREG_UNINITIALIZED)
                unregister_netdev(pDevCtrl->dev);

			free_netdev(pDevCtrl->dev);
        }
    }
   
    return 0;
}
/* 
 * HFB data for ARP request.
 * In WoL (Magic Packet or ACPI) mode, we need to response
 * ARP request, so dedicate an HFB to filter the ARP request.
 */
static uint32 hfb_arp[] =
{
	0x000FFFFF,
	0x000FFFFF,
	0x000FFFFF,
	0x00000000,
	0x00000000,
	0x00000000,
	0x000F0806,
	0x000F0001,
	0x000F0800,
	0x000F0604,
	0x000F0001,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x000F0000,
	0x000F0000,
	0x000F0000,
	0x000F0000,	/* IPDA to be filled.*/
	0x000F0000	/* IPDA to be filled.*/
};
#if 0
/* Debug Rx byte extraction, the filter check for TCP destination port*/
static uint32 hfb_tcp[] =
{
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x000F0800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x0000F1389
};
#endif
#define HFB_TCP_LEN 19
#define HFB_ARP_LEN	21
/*
 * Program ACPI pattern into HFB.
 * Return filter index if succesful.
 * if user == 1, the data will be copied from user space.
 */
static int bcmumac_update_hfb(struct net_device *dev, uint32 *data, int len, int user)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	volatile rbufRegs *txrx_ctrl = pDevCtrl->txrx_ctrl;
	int filter, offset, count;
	uint32 * tmp;
	
	TRACE(("Updating HFB len=0x%d\n", len));
	if(txrx_ctrl->rbuf_hfb_ctrl & RBUF_HFB_256B) {
		if (len > 256)
			return -EINVAL;
		count = 8;
		offset = 256;
	}else {
		if (len > 128)
			return -EINVAL;
		count = 16;
		offset = 128;
	}
	/* find next unused filter */
	for (filter = 0; filter < count; filter++) {
		if(!((txrx_ctrl->rbuf_hfb_ctrl >> (filter + RBUF_HFB_FILTER_EN_SHIFT)) & 0X1))
			break;
	}
	if(filter == count) {
		printk(KERN_ERR "no unused filter available!\n");
		return -EINVAL;	/* all filters have been enabled*/
	}
	
	if( user)
	{
		if((tmp = kmalloc(len*sizeof(uint32), GFP_KERNEL)) == NULL)
		{
			printk(KERN_ERR "%s: Malloc faild\n", __FUNCTION__);
			return -EFAULT;
		}
		/* copy pattern data */
		if(copy_from_user(tmp, data, len*sizeof(uint32)) != 0)
		{
			printk(KERN_ERR "Failed to copy user data: src=%p, dst=%p\n", 
				data, pDevCtrl->hfb + filter*offset);
			return -EFAULT;
		}
	}else {
		tmp = data;
	}
	/* Copy pattern data into HFB registers.*/
	for(count = 0; count < offset; count++)
	{
		if( count < len)
			pDevCtrl->hfb[filter * offset + count] = *(tmp + count);
		else
			pDevCtrl->hfb[filter * offset + count] = 0;
	}
	if(user)
	{
		kfree(tmp);
	}

	/* set the filter length*/
	txrx_ctrl->rbuf_fltr_len[3-(filter>>2)] |= (len*2 << (RBUF_FLTR_LEN_SHIFT * (filter&0x03)) );
	
	/*enable this filter.*/
	txrx_ctrl->rbuf_hfb_ctrl |= (1 << (RBUF_HFB_FILTER_EN_SHIFT + filter));

	return filter;
	
}
/*
 * read ACPI pattern data for a particular filter.
 */
static int bcmumac_read_hfb(struct net_device * dev, struct acpi_data * u_data)
{
	int filter, offset, count, len;
	BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	volatile rbufRegs *txrx_ctrl = pDevCtrl->txrx_ctrl;

	if(get_user(filter, &(u_data->fltr_index)) ) {
		printk(KERN_ERR "Failed to get user data\n");
		return -EFAULT;
	}
	
	if(txrx_ctrl->rbuf_hfb_ctrl & RBUF_HFB_256B) {
		count = 8;
		offset = 256;
	}else {
		count = 16;
		offset = 128;
	}
	if (filter > count)
		return -EINVAL;
	
	/* see if this filter is enabled, if not, return length 0 */
	if ((txrx_ctrl->rbuf_hfb_ctrl & (1 << (filter + RBUF_HFB_FILTER_EN_SHIFT)) ) == 0) {
		len = 0;
		put_user(len , &u_data->count);
		return 0;
	}
	/* check the filter length, in bytes */
	len = RBUF_FLTR_LEN_MASK & (txrx_ctrl->rbuf_fltr_len[filter>>2] >> (RBUF_FLTR_LEN_SHIFT * (filter & 0x3)) );
	if( u_data->count < len)
		return -EINVAL;
	/* copy pattern data */
	if(copy_to_user((void*)(u_data->p_data), (void*)(pDevCtrl->hfb + filter*offset), len)) {
		printk(KERN_ERR "Failed to copy data to user space: src=%p, dst=%p\n", 
				pDevCtrl->hfb+filter*offset, u_data->p_data);
		return -EFAULT;
	}
	return len;
}
/*
 * clear the HFB, disable filter indexed by "filter" argument.
 */
static inline void bcmumac_clear_hfb(BcmEnet_devctrl * pDevCtrl, int filter)
{
	int offset;

	if(pDevCtrl->txrx_ctrl->rbuf_hfb_ctrl & RBUF_HFB_256B) {
		offset = 256;
	}else {
		offset = 128;
	}
	if (filter == CLEAR_ALL_HFB)
	{
		pDevCtrl->txrx_ctrl->rbuf_hfb_ctrl &= ~(0xffff << (RBUF_HFB_FILTER_EN_SHIFT));
		pDevCtrl->txrx_ctrl->rbuf_hfb_ctrl &= ~RBUF_HFB_EN;
	}else 
	{
		/* disable this filter */
		pDevCtrl->txrx_ctrl->rbuf_hfb_ctrl &= ~(1 << (RBUF_HFB_FILTER_EN_SHIFT + filter));
		/* clear filter length register */
		pDevCtrl->txrx_ctrl->rbuf_fltr_len[3-(filter>>2)] &= ~(0xff << (RBUF_FLTR_LEN_SHIFT * (filter & 0x03)) );
	}
	
}
/* utility to get interface ip address in kernel space.*/
static uint32 bcmumac_getip(struct net_device * dev)
{
	struct net_device * pnet_device;
	uint32 ip = 0;
	
	read_lock(&dev_base_lock);
	/* read all devices */
	pnet_device = dev_base;
	while(pnet_device != NULL)
	{
		if((netif_running(pnet_device)) && 
				(pnet_device->ip_ptr != NULL) &&
				(!strcmp(pnet_device->name, dev->name)) )
		{
			struct in_device * pin_dev;
			pin_dev = (struct in_device *)(pnet_device->ip_ptr);
			ip = htonl(pin_dev->ifa_list->ifa_address);
			//ip = *pip;
			break;
		}
		pnet_device = pnet_device->next;
	}
	read_unlock(&dev_base_lock);
	return ip;
}				
	
/* 
 * ethtool function - get WOL (Wake on LAN) settings, 
 * Only Magic Packet Detection is supported through ethtool,
 * the ACPI (Pattern Matching) WOL option is supported in
 * bcmumac_do_ioctl function.
 */
static void bcmumac_get_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	BcmEnet_devctrl * pDevCtrl = netdev_priv(dev);
	volatile uniMacRegs * umac = pDevCtrl->umac;
	wol->supported = WAKE_MAGIC | WAKE_MAGICSECURE;
	
	if(umac->mpd_ctrl & MPD_EN)
		wol->wolopts = WAKE_MAGIC;
	if(umac->mpd_ctrl & MPD_PW_EN)
	{
		unsigned short pwd_ms;
		unsigned long pwd_ls;
		wol->wolopts |= WAKE_MAGICSECURE;
		pwd_ls = umac->mpd_pw_ls;
		copy_to_user(&wol->sopass[0], &pwd_ls, 4);
		pwd_ms = umac->mpd_pw_ms & 0xFFFF;
		copy_to_user(&wol->sopass[4], &pwd_ms, 2);
	}else {
		memset(&wol->sopass[0], 0, sizeof(wol->sopass));
	}
}
/*
 * ethtool function - set WOL (Wake on LAN) settings.
 * Only for magic packet detection mode.
 */
static int bcmumac_set_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	BcmEnet_devctrl * pDevCtrl = netdev_priv(dev);
	volatile uniMacRegs * umac = pDevCtrl->umac;
	uint32 ip;

	TRACE(("%s: wolopts=0x%08x\n", __FUNCTION__, wol->wolopts));
	if(wol->wolopts & ~(WAKE_MAGIC | WAKE_MAGICSECURE))
		return -EINVAL;

	if((ip = bcmumac_getip(dev)) == 0)
	{
		printk("IP address is not set, can't put in WoL mode\n");
		return -EINVAL;
	}else {
		hfb_arp[HFB_ARP_LEN-2] |= (ip >> 16);
		hfb_arp[HFB_ARP_LEN-1] |= (ip & 0xFFFF);
		/* Enable HFB, to response to ARP request.*/
		if(bcmumac_update_hfb(dev, hfb_arp, HFB_ARP_LEN, 0) < 0)
		{
			printk("%s: Unable to update HFB\n", __FUNCTION__);
			return -EFAULT;
		} 
		pDevCtrl->txrx_ctrl->rbuf_hfb_ctrl |= RBUF_HFB_EN;
#if 0
		int i, filter;
		for( i = 0 ; i < 16; i++)
		{
			filter = bcmumac_update_hfb(dev, hfb_tcp, HFB_TCP_LEN, 0);
			if (filter < 0) 
			{
				printk(KERN_ERR "Failed to update HFB\n");
				return -EFAULT;
			}
		}
		/* Set extraction offset register */
		if(filter & 0x1)
		{
			pDevCtrl->txrx_ctrl->rbuf_rxc_offset[7+(filter >> 1)] &= ~(0x7FF << 15);
			pDevCtrl->txrx_ctrl->rbuf_rxc_offset[7+(filter >> 1)] |= (26 << 15);
		}else {
			pDevCtrl->txrx_ctrl->rbuf_rxc_offset[7+(filter >> 1)] &= ~(0x7FF);
			pDevCtrl->txrx_ctrl->rbuf_rxc_offset[7+(filter >> 1)] |= (26);
		}
		pDevCtrl->txrx_ctrl->rbuf_hfb_ctrl |= RBUF_HFB_EN;
#endif			
	}
	if(wol->wolopts & WAKE_MAGICSECURE)
	{
		umac->mpd_pw_ls = *(unsigned long *)&wol->sopass[0];
		umac->mpd_pw_ms = *(unsigned short*)&wol->sopass[4];
		umac->mpd_ctrl |= MPD_PW_EN;
	}
	if(wol->wolopts & WAKE_MAGIC)
	{
		/* Power down the umac, with magic packet mode.*/
		bcmumac_power_down(pDevCtrl, BCMUMAC_POWER_WOL_MAGIC);
	}
	return 0;
}
/*
 * ethtool function - get generic settings.
 */
static int bcmumac_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	int val;
	BcmEnet_devctrl * pDevCtrl = netdev_priv(dev);
	volatile uniMacRegs * umac = pDevCtrl->umac;
	int speeds[] = {SPEED_10, SPEED_100, SPEED_1000, SPEED_2500};

	cmd->supported = SUPPORTED_10baseT_Half |
		SUPPORTED_10baseT_Full |
		SUPPORTED_100baseT_Half |
		SUPPORTED_100baseT_Full |
		SUPPORTED_Autoneg;
	if(pDevCtrl->EnetInfo.PhyType == BP_ENET_EXTERNAL_GPHY)
	{
		cmd->supported |= SUPPORTED_1000baseT_Half | SUPPORTED_1000baseT_Full;
		cmd->advertising = ADVERTISED_1000baseT_Half | ADVERTISED_1000baseT_Full;
	}else
	{
		cmd->advertising = ADVERTISED_100baseT_Half | ADVERTISED_100baseT_Full;
	}
	cmd->port = PORT_MII;
	cmd->phy_address = pDevCtrl->EnetInfo.PhyAddress;
	
	/* get speed/duplex info from MAC or from PHY?*/
	cmd->speed = speeds[umac->mode & 0x03];
	
	if(umac->mode & MODE_HD)
		cmd->duplex = DUPLEX_HALF;
	else
		cmd->duplex = DUPLEX_FULL;
	/* get the Autoneg setting from PHY */
	val = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_BMCR);
	if(val & BMCR_ANENABLE)
		cmd->autoneg = AUTONEG_ENABLE;
	else
		cmd->autoneg = AUTONEG_DISABLE;

	if(pDevCtrl->EnetInfo.PhyType == BP_ENET_INTERNAL_PHY)
		cmd->transceiver = XCVR_INTERNAL;
	else
		cmd->transceiver = XCVR_EXTERNAL;

	cmd->maxrxpkt = DmaDescThres;
	return 0;
}
/*
 * ethtool function - set settings.
 */
static int bcmumac_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	BcmEnet_devctrl * pDevCtrl = netdev_priv(dev);
	volatile uniMacRegs * umac = pDevCtrl->umac;

	if(cmd->autoneg == AUTONEG_ENABLE) {
		mii_setup(dev);
	}else {
		if (cmd->duplex == DUPLEX_HALF)
			umac->cmd |= CMD_HD_EN;
		else
			umac->cmd &= ~CMD_HD_EN;

		if(pDevCtrl->EnetInfo.PhyType == BP_ENET_INTERNAL_PHY &&
			cmd->speed == SPEED_1000)
			return -EINVAL;
		umac->cmd &= CMD_SPEED_MASK;
		switch(cmd->speed){
			case SPEED_10:
				umac->cmd |= UMAC_SPEED_10;
				break;
			case SPEED_100:
				umac->cmd |= UMAC_SPEED_100;
				break;
			case SPEED_1000:
				umac->cmd |= UMAC_SPEED_1000;
				break;
			default:
				umac->cmd |= UMAC_SPEED_100;
		}
	}
	if(cmd->maxrxpkt != 0)
		DmaDescThres = cmd->maxrxpkt;

	return 0;
}
	
/*
 * ethtool function - get driver info.
 */
static void bcmumac_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	strncpy(info->driver, CARDNAME, sizeof(info->driver));
	strncpy(info->version, VER_STR, sizeof(info->version));
	
}
static u32 bcmumac_get_rx_csum(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	if(pDevCtrl->txrx_ctrl->rbuf_chk_ctrl & RBUF_RXCHK_EN)
		return 1;
	
	return 0;
}
static int bcmumac_set_rx_csum(struct net_device * dev, u32 val)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	spin_lock_bh(&pDevCtrl->bh_lock);
	if( val == 0)
	{
		pDevCtrl->txrx_ctrl->rbuf_ctrl &= ~RBUF_64B_EN;
		pDevCtrl->txrx_ctrl->rbuf_chk_ctrl &= ~RBUF_RXCHK_EN;
	}else {
		pDevCtrl->txrx_ctrl->rbuf_ctrl |= RBUF_64B_EN;
		pDevCtrl->txrx_ctrl->rbuf_chk_ctrl |= RBUF_RXCHK_EN ;
	}
	spin_unlock_bh(&pDevCtrl->bh_lock);
	return 0;
}
static u32 bcmumac_get_tx_csum(struct net_device * dev)
{
	if(dev->features & NETIF_F_IP_CSUM)
		return 1;
	return 0;
}
static int bcmumac_set_tx_csum(struct net_device * dev, u32 val)
{
    unsigned long flags;
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    spin_lock_irqsave(&pDevCtrl->lock, flags);
	if(val == 0)
		dev->features &= ~NETIF_F_IP_CSUM;
	else
		dev->features |= NETIF_F_IP_CSUM ;
	spin_unlock_irqrestore(&pDevCtrl->lock, flags);
	return 0;
}
/* 
 * standard ethtool support functions.
 */
static struct ethtool_ops bcmumac_ethtool_ops = {
	.get_settings		= bcmumac_get_settings,
	.set_settings		= bcmumac_set_settings,
	.get_drvinfo		= bcmumac_get_drvinfo,
	.get_wol			= bcmumac_get_wol,
	.set_wol			= bcmumac_set_wol,
	.get_rx_csum		= bcmumac_get_rx_csum,
	.set_rx_csum		= bcmumac_set_rx_csum,
	.get_tx_csum		= bcmumac_get_tx_csum,
	.set_tx_csum		= bcmumac_set_tx_csum,
	.get_link			= ethtool_op_get_link,
};
/*
 * disable clocks according to mode, cable sense/WoL
 */
static void bcmumac_disable_clocks(BcmEnet_devctrl * pDevCtrl, int mode)
{
	int  pm_ctrl;
	if(pDevCtrl->EnetInfo.PhyType == BP_ENET_INTERNAL_PHY)
		pm_ctrl = BCHP_CLK_GENET_CLK_PM_CTRL;
	else
		pm_ctrl = BCHP_CLK_MOCA_CLK_PM_CTRL;
	
	TRACE(("Disabling clocks...\n"));
	
	/* common clocks that should be disabled regardless of mode.*/
	BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_216M_CLK_MASK);
	BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_ISDMA_27_125M_CLK_MASK);
	BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_UNIMAC_SYS_TX_27_125M_CLK_MASK);
	/* 
	 * Disable RGMII 250M clock, since we don't have external GPHY, 
	 * in the future, may need to remove this.
	 */
	//BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_RGMII_250M_CLK_MASK);
	BDEV_RD(pm_ctrl);
	switch(mode) {
		case BCMUMAC_POWER_CABLE_SENSE:
			BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_HFB_27_125M_CLK_MASK);
			BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_UNIMAC_SYS_RX_27_125M_CLK_MASK);
			BDEV_SET(BCHP_CLK_MISC, BCHP_CLK_MISC_GENET_CLK_SEL_MASK);
			BDEV_UNSET(BCHP_CLK_MISC, BCHP_CLK_MISC_GENET_GMII_TX_CLK_SEL_MASK);
			BDEV_RD(pm_ctrl);
			break;
		case BCMUMAC_POWER_WOL_MAGIC:
			BDEV_SET(BCHP_CLK_MISC, BCHP_CLK_MISC_GENET_CLK_SEL_MASK);
			BDEV_UNSET(BCHP_CLK_MISC, BCHP_CLK_MISC_GENET_GMII_TX_CLK_SEL_MASK);
			BDEV_RD(BCHP_CLK_MISC);
			BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_RGMII_250M_CLK_MASK);
			break;
		case BCMUMAC_POWER_WOL_ACPI:
			/* no need to do anything here */
			break;
		default:
			break;
	}
	
}
/*
 * Enable clocks.
 */
static void bcmumac_enable_clocks(BcmEnet_devctrl * pDevCtrl, int mode)
{
	int  pm_ctrl;
	if(pDevCtrl->EnetInfo.PhyType == BP_ENET_INTERNAL_PHY)
		pm_ctrl = BCHP_CLK_GENET_CLK_PM_CTRL;
	else
		pm_ctrl = BCHP_CLK_MOCA_CLK_PM_CTRL;

	/* Enable all clocks */
	BDEV_WR(pm_ctrl, 0);
	BDEV_RD(pm_ctrl);
	/* 
	 * Save some power by disabling RGMII tx clock and 
	 * MoCA Tx/Rx clocks (if it's not MoCA and GPHY).
	 */
	if(pDevCtrl->EnetInfo.PhyType == BP_ENET_INTERNAL_PHY)
	{
		//BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_RGMII_250M_CLK_MASK);
		//BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_GMII_TX_27_125M_CLK_MASK);
		BDEV_UNSET(BCHP_CLK_MISC, BCHP_CLK_MISC_GENET_CLK_SEL_MASK);
		BDEV_SET(BCHP_CLK_MISC, BCHP_CLK_MISC_GENET_GMII_TX_CLK_SEL_MASK);
		BDEV_RD(pm_ctrl);
	}
	TRACE(("Enabling clocks...\n"));
}
/*
 * Power down the unimac, based on mode.
 */
static void bcmumac_power_down(BcmEnet_devctrl *pDevCtrl, int mode)
{
	int retries = 0;

	switch(mode) {
		case BCMUMAC_POWER_CABLE_SENSE:
			/* PHY bug , disble DLL only for now */
			pDevCtrl->txrx_ctrl->ephy_pwr_mgmt |= EXT_PWR_DOWN_DLL;
			//	EXT_PWR_DOWN_PHY ;
			pDevCtrl->txrx_ctrl->rgmii_oob_ctrl &= ~RGMII_MODE_EN;
			bcmumac_disable_clocks(pDevCtrl, mode);
			break;
		case BCMUMAC_POWER_WOL_MAGIC:
			/* ENable CRC forward */
			pDevCtrl->umac->cmd |= CMD_CRC_FWD;
			pDevCtrl->umac->mpd_ctrl |= MPD_EN;
			while(!(pDevCtrl->txrx_ctrl->rbuf_status & RBUF_STATUS_WOL)) {
				retries++;
				if(retries > 5) {
					printk(KERN_CRIT "bcmumac_power_down polling wol mode timeout\n");
					pDevCtrl->umac->mpd_ctrl &= ~MPD_EN;
					return;
				}
				udelay(100);
			}
			/* Service Rx BD untill empty */
			bcmumac_disable_clocks(pDevCtrl, mode);
			pDevCtrl->intrl2->cpu_mask_clear |= UMAC_IRQ_MPD_R;
			pDevCtrl->intrl2->cpu_mask_clear |= UMAC_IRQ_HFB_MM | UMAC_IRQ_HFB_SM;
			break;
		case BCMUMAC_POWER_WOL_ACPI:
			pDevCtrl->txrx_ctrl->rbuf_hfb_ctrl |= RBUF_ACPI_EN;
			while(!(pDevCtrl->txrx_ctrl->rbuf_status & RBUF_STATUS_WOL)) {
				retries++;
				if(retries > 5) {
					printk(KERN_CRIT "bcmumac_power_down polling wol mode timeout\n");
					pDevCtrl->txrx_ctrl->rbuf_hfb_ctrl &= ~RBUF_ACPI_EN;
					return;
				}
				udelay(100);
			}
			/* Service RX BD untill empty */
			bcmumac_disable_clocks(pDevCtrl, mode);
			pDevCtrl->intrl2->cpu_mask_clear |= UMAC_IRQ_HFB_MM | UMAC_IRQ_HFB_SM;
			break;
		default:
			break;
	}
	
}
static void bcmumac_power_up(BcmEnet_devctrl *pDevCtrl, int mode)
{
	u32 mask;
	switch(mode) {
		case BCMUMAC_POWER_CABLE_SENSE:
			/* 7420A0 bug, PHY_RESET doesn't work */
			//pDevCtrl->txrx_ctrl->ephy_pwr_mgmt |= PHY_RESET;
			BDEV_SET(0x46a414, 1);
			pDevCtrl->txrx_ctrl->ephy_pwr_mgmt &= ~EXT_PWR_DOWN_DLL;
			pDevCtrl->txrx_ctrl->ephy_pwr_mgmt &= ~EXT_PWR_DOWN_PHY;
			pDevCtrl->txrx_ctrl->ephy_pwr_mgmt &= ~EXT_PWR_DOWN_BIAS;
			udelay(2);
			//pDevCtrl->txrx_ctrl->ephy_pwr_mgmt &= ~PHY_RESET;
			BDEV_UNSET(0x46a414, 1);
			udelay(150);
			bcmumac_enable_clocks(pDevCtrl, mode);
			break;
		case BCMUMAC_POWER_WOL_MAGIC:
			pDevCtrl->umac->mpd_ctrl &= ~MPD_EN;
			/* 
			 * If ACPI is enabled at the same time, disable it, since 
			 * we have been waken up.
			 */
			if( !(pDevCtrl->txrx_ctrl->rbuf_hfb_ctrl & RBUF_ACPI_EN))
			{
				pDevCtrl->txrx_ctrl->rbuf_hfb_ctrl &= RBUF_ACPI_EN;
				bcmumac_enable_clocks(pDevCtrl, BCMUMAC_POWER_WOL_ACPI);
				/* Stop monitoring ACPI interrupts */
				pDevCtrl->intrl2->cpu_mask_set |= (UMAC_IRQ_HFB_SM | UMAC_IRQ_HFB_MM);
			}
			bcmumac_clear_hfb(pDevCtrl, CLEAR_ALL_HFB);
			bcmumac_enable_clocks(pDevCtrl, mode);
			break;
		case BCMUMAC_POWER_WOL_ACPI:
			pDevCtrl->txrx_ctrl->rbuf_hfb_ctrl &= ~RBUF_ACPI_EN;
			/* 
			 * If Magic packet is enabled at the same time, disable it, 
			 */
			if(!(pDevCtrl->umac->mpd_ctrl & MPD_EN))
			{
				pDevCtrl->umac->mpd_ctrl &= ~MPD_EN;
				bcmumac_enable_clocks(pDevCtrl, BCMUMAC_POWER_WOL_ACPI);
				/* stop monitoring magic packet interrupt and disable crc forward */
				pDevCtrl->intrl2->cpu_mask_set |= UMAC_IRQ_MPD_R;
				pDevCtrl->umac->cmd &= ~CMD_CRC_FWD;
			}
			bcmumac_clear_hfb(pDevCtrl, CLEAR_ALL_HFB);
			bcmumac_enable_clocks(pDevCtrl,mode); 
			break;
		default:
			break;
	}
}
/* 
 * ioctl handle special commands that are not present in ethtool.
 */
static int bcmumac_enet_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    struct mii_ioctl_data *mii;
    unsigned long flags;
    struct acpi_data *u_data;
    int val = 0;

    /* we can add sub-command in ifr_data if we need to in the future */
    switch (cmd)
    {
    case SIOCGMIIPHY:       /* Get address of MII PHY in use. */
        mii = (struct mii_ioctl_data *)&rq->ifr_data;
        mii->phy_id = pDevCtrl->EnetInfo.PhyAddress;
		break;

    case SIOCGMIIREG:       /* Read MII PHY register. */
        spin_lock_irqsave(&pDevCtrl->lock, flags);
        mii = (struct mii_ioctl_data *)&rq->ifr_data;
        mii->val_out = mii_read(dev, mii->phy_id & 0x1f, mii->reg_num & 0x1f);
        spin_unlock_irqrestore(&pDevCtrl->lock, flags);
        break;

    case SIOCSMIIREG:       /* Write MII PHY register. */
        spin_lock_irqsave(&pDevCtrl->lock, flags);
        mii = (struct mii_ioctl_data *)&rq->ifr_data;
        mii_write(dev, mii->phy_id & 0x1f, mii->reg_num & 0x1f, mii->val_in);
        spin_unlock_irqrestore(&pDevCtrl->lock, flags);
        break;
	case SIOCSACPISET:
		spin_lock_irqsave(&pDevCtrl->lock, flags);
		bcmumac_power_down(pDevCtrl, BCMUMAC_POWER_WOL_ACPI);
		spin_unlock_irqrestore(&pDevCtrl->lock, flags);
		break;
	case SIOCSACPICANCEL:
		spin_lock_irqsave(&pDevCtrl->lock, flags);
		bcmumac_power_up(pDevCtrl, BCMUMAC_POWER_WOL_ACPI);
		spin_unlock_irqrestore(&pDevCtrl->lock, flags);
		break;
	case SIOCSPATTERN:
		u_data = (struct acpi_data *)rq->ifr_data;
		val =  bcmumac_update_hfb(dev, (unsigned long *)u_data->p_data, u_data->count,1 );
		if(val >= 0)
			put_user(val, &u_data->fltr_index);
		break;
	case SIOCGPATTERN:
		u_data = (struct acpi_data *)rq->ifr_data;
		val = bcmumac_read_hfb(dev, u_data);
		break;
	default:
		val = -EINVAL;
		break;
    }

    return val;       
}

static int bcmumac_mii_thread(void * p)
{
	BcmEnet_devctrl * pDevCtrl;	
	int ret = 0;
	pDevCtrl = (BcmEnet_devctrl *)p;
	daemonize("%s", "bcmumac_miid");
	strcpy(current->comm, "bcmumac_miid");
	/* 
	 * mii_init() will program speed/duplex/pause 
	 * parameters into umac and rbuf registers.
	 */
    if (mii_init(pDevCtrl->dev))
	{
       printk(KERN_CRIT "mii_init failed\n"); 
	   ret = -EFAULT;
	}
	pDevCtrl->umac->cmd |= CMD_TX_EN | CMD_RX_EN;
	pDevCtrl->mii_pid = 0;
	return ret;
}
static int bcmumac_probe(struct platform_device *pdev)
{
	struct resource *mres, *ires;
	void __iomem *base;
	int phy_id, err;
	struct bcmumac_platform_data *cfg = pdev->dev.platform_data;
	BcmEnet_devctrl *pDevCtrl;
	struct net_device *dev;

	mres = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ires = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if(! mres || ! ires) {
		printk(KERN_ERR "%s: can't get resources\n", __FUNCTION__);
		return(-EIO);
	}

	base = ioremap(mres->start, mres->end - mres->start + 1);
	TRACE(("%s: base=0x%x\n", __FUNCTION__, base));

	if(! base) {
		printk(KERN_ERR "%s: can't ioremap\n", __FUNCTION__);
		return(-EIO);
	}

	phy_id = cfg->phy_id;
	if(phy_id == BRCM_PHY_ID_AUTO) {
		phy_id = mii_probe((unsigned long)base);
		if(phy_id == BP_ENET_NO_PHY) {
			printk(KERN_INFO
				"bcmumac: no PHY detected for device %d\n",
				pdev->id);
			err = -ENODEV;
			goto bad;
		} else {
			printk(KERN_INFO
				"bcmumac: found PHY at ID %d for device %d\n",
				phy_id, pdev->id);
		}
	}

	dev = alloc_netdev(sizeof(*pDevCtrl), "eth%d", bcmumac_null_setup);
	if(dev == NULL) {
		printk(KERN_ERR "bcmumac: can't allocate netdev\n");
		err = -ENOMEM;
		goto bad;
	}

	ether_setup(dev);
	dev->base_addr = (unsigned long)base;
	memcpy(dev->dev_addr, cfg->macaddr, 6);
	dev->irq                    = ires->start;

	dev->open                   = bcmumac_net_open;
	dev->stop                   = bcmumac_net_close;
	dev->hard_start_xmit        = bcmumac_net_xmit;
	dev->tx_timeout             = bcmumac_net_timeout;
	dev->watchdog_timeo         = 2*HZ;
	dev->get_stats              = bcmumac_net_query;
	dev->set_mac_address        = bcmumac_set_mac_addr;
	dev->set_multicast_list     = bcmumac_set_multicast_list;
	dev->do_ioctl               = &bcmumac_enet_ioctl;
	dev->poll                   = bcmumac_enet_poll;
	dev->weight                 = 64;
#ifdef CONFIG_BCMUMAC_TX_CSUM
	//dev->features				|= NETIF_F_IP_CSUM;
#endif
	SET_ETHTOOL_OPS(dev, &bcmumac_ethtool_ops);
	SET_MODULE_OWNER(dev);

	// Let boot setup info override default settings.
	netdev_boot_setup_check(dev);

	pDevCtrl = (BcmEnet_devctrl *)netdev_priv(dev);
	pDevCtrl->dev = dev;
	pDevCtrl->devnum = pdev->id;
	pDevCtrl->phyAddr = phy_id;
	pDevCtrl->rxIrq = ires->start;
	pDevCtrl->bIPHdrOptimize = 1;
	pDevCtrl->bh_lock = SPIN_LOCK_UNLOCKED;

	init_timer(&pDevCtrl->timer);
	pDevCtrl->timer.data = (unsigned long)pDevCtrl;
	pDevCtrl->timer.function = tx_reclaim_timer;

	spin_lock_init(&pDevCtrl->lock);
	INIT_WORK(&pDevCtrl->bcmumac_task, (void (*)(void *))bcmumac_task, pDevCtrl);

	if(BpGetEthernetMacInfo(&pDevCtrl->EnetInfo, cfg->phy_type) != BP_SUCCESS) {
		printk(KERN_ERR "%s: Unknown PHY type: %d\n", __FUNCTION__,
			cfg->phy_type);
		err = -EINVAL;
		goto bad;
	}
	pDevCtrl->EnetInfo.PhyAddress = phy_id;

	err = bcmumac_init_dev(pDevCtrl);
	if(err < 0)
		goto bad;

	dev_set_drvdata(&pdev->dev, pDevCtrl);
	request_irq(pDevCtrl->rxIrq, bcmumac_net_isr, SA_INTERRUPT|SA_SHIRQ, dev->name, pDevCtrl);

	err = register_netdev(dev);
	if(err != 0) {
		bcmumac_uninit_dev(pDevCtrl);
		goto bad;
	}
	pDevCtrl->next_dev = eth_root_dev;
	eth_root_dev = dev;
#ifdef CONFIG_BCMUMAC_USE_PROC
	bcmumac_init_proc(pDevCtrl);
#endif

	return(0);

bad:
	iounmap(base);
	return(err);
}

static int bcmumac_remove(struct platform_device *pdev)
{
	BcmEnet_devctrl *pDevCtrl = dev_get_drvdata(&pdev->dev);

	bcmumac_uninit_dev(pDevCtrl);
	iounmap((void __iomem *)pDevCtrl->base_addr);

	return(0);
}

static struct platform_driver bcmumac_plat_drv = {
	.probe =		bcmumac_probe,
	.remove =		bcmumac_remove,
	.driver = {
		.name =		"bcmumac",
		.owner =	THIS_MODULE,
	},
};

static int bcmumac_module_init(void)
{
	platform_driver_register(&bcmumac_plat_drv);
	return(0);
}

static void bcmumac_module_cleanup(void)
{
	platform_driver_unregister(&bcmumac_plat_drv);
}

int isEnetConnected(char *pn)
{
	BcmEnet_devctrl *pDevCtrl = netdev_priv(bcmemac_get_device());
	return atomic_read(&pDevCtrl->linkState); 
}

module_init(bcmumac_module_init);
module_exit(bcmumac_module_cleanup);

MODULE_LICENSE("GPL");

EXPORT_SYMBOL(bcmemac_get_free_txdesc);
EXPORT_SYMBOL(bcmemac_get_device);
EXPORT_SYMBOL(bcmemac_xmit_multibuf);
EXPORT_SYMBOL(isEnetConnected);
EXPORT_SYMBOL(bcmemac_xmit_check);
