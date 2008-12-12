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

#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/ipv6.h>

#include <asm/mipsregs.h>
#include <asm/cacheflush.h>
#include <asm/brcmstb/common/brcm-pm.h>

#include "bcmmii.h"
#include "bcmunimac.h"
#include "if_net.h"
#include "unimac_proc.h"

#include <linux/stddef.h>

extern unsigned long getPhysFlashBase(void);

#define POLLTIME_10MS		(HZ/100)
#define POLLCNT_1SEC		(HZ/POLLTIME_10MS)
#define POLLCNT_FOREVER		((int) 0x80000000)

#define skb_dataref(x)		(&skb_shinfo(x)->dataref)

#define ENET_POLL_DONE     	0x80000000
#define RX_BUF_LENGTH		2048	/* 2Kb buffer */
#define SKB_ALIGNMENT		32		/* 256B alignment */
#define ISDMA_DESC_THRES	64		/* Rx Descriptor throttle threshold */

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
/* Remove registered netdevice */
static void bcmumac_init_cleanup(struct net_device *dev);
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
/* power management */
static void bcmumac_power_down(BcmEnet_devctrl *pDevCtrl, int mode);
static void bcmumac_power_up(BcmEnet_devctrl *pDevCtrl, int mode);


#ifdef DUMP_DATA
/* Display hex base data */
static void dumpHexData(unsigned char *head, int len);
/* dumpMem32 dump out the number of 32 bit hex data  */
static void dumpMem32(uint32 * pMemAddr, int iNumWords);
#endif

static struct net_device *eth_root_dev = NULL;
static int g_num_devs = 0;
static struct net_device *g_devs[BCMUMAC_MAX_DEVS];
static uint8 g_flash_eaddr[ETH_ALEN];
static int DmaDescThres = ISDMA_DESC_THRES;

/* UMAC base physical address, platform dependent.*/
static unsigned long umac_base_addr[BCMUMAC_MAX_DEVS] = {
	BCHP_EMAC_0_REG_START,
	BCHP_EMAC_1_REG_START
};

/* --------------------------------------------------------------------------
    Name: bcmumac_get_free_txdesc
 Purpose: Get Current Available TX desc count
-------------------------------------------------------------------------- */
int bcmumac_get_free_txdesc( struct net_device *dev ){
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    return pDevCtrl->txFreeBds;
}

struct net_device * bcmumac_get_device(void) {
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


#ifdef DUMP_DATA
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


    TRACE(("%s: bcmumac_net_open, umac->cmd=%xul, RxDMA=%ul, rxDma.cfg=%ul\n", 
                dev->name, pDevCtrl->umac->cmd, pDevCtrl->rxDma, pDevCtrl->rxDma->cfg));

    /* disable ethernet MAC while updating its registers */
    pDevCtrl->umac->cmd &= ~(CMD_TX_EN | CMD_RX_EN);

    pDevCtrl->dmaRegs->controller_cfg |= ISDMA_ENABLE;         

	
    pDevCtrl->rxDma->cfg |= DMA_ENABLE;

	enable_irq(pDevCtrl->rxIrq);

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

    ASSERT(pDevCtrl != NULL);

    TRACE(("%s: bcmumac_net_close\n", dev->name));

    netif_stop_queue(dev);


    pDevCtrl->rxDma->cfg &= ~DMA_ENABLE;

    pDevCtrl->umac->cmd &= ~(CMD_RX_EN | CMD_TX_EN);

    disable_irq(pDevCtrl->rxIrq);

    del_timer_sync(&pDevCtrl->timer);
	//del_timer_sync(&pDevCtrl->poll_timer);

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

    pDevCtrl->txNextBdPtr = pDevCtrl->txFirstBdPtr = pDevCtrl->txBds;
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
 Name: bcmumac_xmit_check
 Purpose: Reclaims TX descriptors
-------------------------------------------------------------------------- */
int bcmumac_xmit_check(struct net_device * dev)
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
		if(pDevCtrl->txCbPtrHead->BdAddr == (pDevCtrl->txFirstBdPtr + pDevCtrl->txDma->descOffset))
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
#ifdef CONFIG_L4_CSUM
	TSB * tsb;
	struct iphdr * iph;
	struct ipv6hdr *ipv6h;
	int ulp_offset, csum_offset;
#endif
    ASSERT(pDevCtrl != NULL);

    //bcmumac_POWER_ON(dev);

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */
    spin_lock_irqsave(&pDevCtrl->lock, flags);
        
    txCBPtr = NULL;

    /* Reclaim transmitted buffers */
    while (pDevCtrl->txCbPtrHead )
	{
		
		if(pDevCtrl->txCbPtrHead->BdAddr == (pDevCtrl->txFirstBdPtr + pDevCtrl->txDma->descOffset))
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

    //txCBPtr->nrBds = 1;
    txCBPtr->skb = skb;

    /* If we could not queue this packet, free it */
    if (pDevCtrl->txFreeBds == 0) {
        TRACE(("%s: bcmumac_net_xmit no free txBds\n", dev->name));
		goto err_out;
    }


    /* Get the BD , used to  enqueue the packet */
    txCBPtr->BdAddr = pDevCtrl->txNextBdPtr;
	/* 
	 * If L4 checksum offloading enabled, must make sure skb has
	 * enough headroom for us to insert 64B status block.
	 */
#ifdef CONFIG_L4_CSUM
	if(skb_headroom(skb) < 64) {
		printk(KERN_CRIT "%s: bcmumac_net_ximt no enough headroom for HW checksum\n", dev->name);
		goto err_out;
	}
	/* ipv6 or ipv4? only support IP , this is ugly!*/
	iph = (struct iphdr *)(skb->data + ETH_HLEN);
	if(iph->version == 4) {
		ulp_offset = ETH_HLEN + sizeof(struct iphdr) - 1;
		bcmumac_get_csum_offset(iph->protocol, &csum_offset);
	}else if(iph->version == 6) {
		ipv6h = skb->nh.ipv6h;
		ulp_offset = ETH_HLEN + sizeof(struct ipv6hdr) - 1;
		bcmumac_get_csum_offset(ipv6h->protocol, &csum_offset);
	}else {
		printk(KERN_CRIT "%s:bcmumac_net_xmit attemping to insert csum for non-IP packet\n", dev->name);
		goto err_out;
	}
	
	/* Insert 64B TSB and set the flag */
	skb_push(skb, 64);
	tsb = (TSB *)skb->data;
	tsb->length_status = (ulp_offset << TSB_ULP_OFFSET_SHIFT) | csum_offset | TSB_LV; 
	
#endif	/* CONFIG_L4_CSUM */

    /*
     * Add the buffer to the ring.
     * Set addr and length of DMA BD to be transmitted.
     */
    dma_cache_wback_inv((unsigned long)skb->data, skb->len);

    txCBPtr->BdAddr->address = EMAC_SWAP32((uint32)virt_to_phys(skb->data));
    txCBPtr->BdAddr->length_status  = EMAC_SWAP32((((unsigned long)((skb->len < ETH_ZLEN) ? ETH_ZLEN : skb->len))<<16));
#ifdef DUMP_DATA
    printk("bcmumac_net_xmit: len %d", skb->len);
    dumpHexData(skb->data, skb->len);
#endif
    /*
     * Turn on the "OWN" bit in the first buffer descriptor
     * This tells the switch that it can transmit this frame.
     */
    txCBPtr->BdAddr->length_status |= EMAC_SWAP32(DMA_OWN | DMA_SOP | DMA_EOP | DMA_TX_APPEND_CRC);
#ifdef CONFIG_L4_CSUM
	txCbPtr->BdAddr->length_status |= EMAC_SWAP32(DMA_TX_DO_CHKSUM);
#endif

    /*
     * Advance BD pointer to next in the chain.
     */
    if (pDevCtrl->txNextBdPtr == pDevCtrl->txLastBdPtr) {
        pDevCtrl->txNextBdPtr->length_status |= EMAC_SWAP32(DMA_WRAP);
        pDevCtrl->txNextBdPtr = pDevCtrl->txFirstBdPtr;
    }
    else {
        pDevCtrl->txNextBdPtr->length_status |= EMAC_SWAP32(0);
        pDevCtrl->txNextBdPtr++;
    }
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

    /* Enable DMA for this channel */
    //pDevCtrl->txDma->cfg |= DMA_ENABLE;

    /* update stats */
    pDevCtrl->stats.tx_bytes += ((skb->len < ETH_ZLEN) ? ETH_ZLEN : skb->len);
    pDevCtrl->stats.tx_bytes += 4;
    pDevCtrl->stats.tx_packets++;

    dev->trans_start = jiffies;

    spin_unlock_irqrestore(&pDevCtrl->lock, flags);

    return 0;
err_out:
	txCb_enq(pDevCtrl, txCBPtr);
	netif_stop_queue(dev);
	spin_unlock_irqrestore(&pDevCtrl->lock, flags);
	return 1;
}

/* --------------------------------------------------------------------------
    Name: bcmumac_xmit_fragment
 Purpose: Send ethernet traffic Buffer DESC and submit to UDMA
-------------------------------------------------------------------------- */
int bcmumac_xmit_fragment( int ch, unsigned char *buf, int buf_len, 
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
     * Turn on the "OWN" bit in the first buffer descriptor
     * This tells the switch that it can transmit this frame.
     */	
    txCBPtr->BdAddr->length_status &= ~EMAC_SWAP32(DMA_SOP |DMA_EOP | DMA_TX_APPEND_CRC);
    txCBPtr->BdAddr->length_status |= EMAC_SWAP32(DMA_OWN | tx_flags | DMA_TX_APPEND_CRC);
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
    //pDevCtrl->txDma->cfg |= DMA_ENABLE;

   /* update stats */
    pDevCtrl->stats.tx_bytes += buf_len; //((skb->len < ETH_ZLEN) ? ETH_ZLEN : skb->len);
    pDevCtrl->stats.tx_bytes += 4;
    pDevCtrl->stats.tx_packets++;

    dev->trans_start = jiffies;


    return 0;
}

EXPORT_SYMBOL(bcmumac_xmit_fragment);

/* --------------------------------------------------------------------------
    Name: bcmumac_xmit_multibuf
 Purpose: Send ethernet traffic in multi buffers (hdr, buf, tail)
-------------------------------------------------------------------------- */
int bcmumac_xmit_multibuf( int ch, unsigned char *hdr, int hdr_len, unsigned char *buf, 
		int buf_len, unsigned char *tail, int tail_len, struct net_device *dev)
{
	unsigned long flags;
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    
	while(bcmumac_xmit_check(dev));

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */
    spin_lock_irqsave(&pDevCtrl->lock, flags);

    /* Header + Optional payload in two parts */
    if((hdr_len> 0) && (buf_len > 0) && (tail_len > 0) && (hdr) && (buf) && (tail)){ 
        /* Send Header */
        while(bcmumac_xmit_fragment( ch, hdr, hdr_len, DMA_SOP, dev))
            bcmumac_xmit_check(dev);
        /* Send First Fragment */  
        while(bcmumac_xmit_fragment( ch, buf, buf_len, 0, dev))
            bcmumac_xmit_check(dev);
        /* Send 2nd Fragment */ 	
        while(bcmumac_xmit_fragment( ch, tail, tail_len, DMA_EOP, dev))
            bcmumac_xmit_check(dev);
    }
    /* Header + Optional payload */
    else if((hdr_len> 0) && (buf_len > 0) && (hdr) && (buf)){
        /* Send Header */
        while(bcmumac_xmit_fragment( ch, hdr, hdr_len, DMA_SOP, dev))
            bcmumac_xmit_check(dev);
        /* Send First Fragment */
        while(bcmumac_xmit_fragment( ch, buf, buf_len, DMA_EOP, dev))
            bcmumac_xmit_check(dev);
    }
    /* Header Only (includes payload) */
    else if((hdr_len> 0) && (hdr)){ 
        /* Send Header */
        while(bcmumac_xmit_fragment( ch, hdr, hdr_len, DMA_SOP | DMA_EOP, dev))
            bcmumac_xmit_check(dev);
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
    uint32 work_to_do = min(dev->quota, *budget);
    uint32 work_done;
    uint32 ret_done;

    work_done = bcmumac_rx(pDevCtrl, work_to_do);
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
        netif_rx_reschedule(pDevCtrl->dev, work_done); /* ???? */
    }
    else {
        enable_irq(pDevCtrl->rxIrq);
    }

    return 0;
}

/*
 * Interrupt bottom half
 */
static void bcmumac_task(BcmEnet_devctrl *pDevCtrl)
{
	spinlock_bf(&pDeCtrl->bh_lock);

	/* Cable plugged/unplugged event */
	if (pDevCtrl->irq_stat & UMAC_IRQ_PHY_DET_F) {
		pDevCtrl->irq_stat &= ~UMAC_IRQ_PHY_DET_F;
		printk(KERN_CRIT "Cable plugged in UMAC%d powering up\n", pDevCtrl->devnum);
		bcmumac_power_up(pDevCtrl, BCMUMAC_POWER_CABLE_SENSE);
		/* restart auto-neg, program speed/duplex/pause info into umac */
		if (!(pDevCtrl->umac->cmd & CMD_AUTO_CONFIG)) {
			mii_setup(pDevCtrl->dev);
		}

	}else if (pDevCtrl->irq_stat & UMAC_IRQ_PHY_DET_R) {
		pDevCtrl->irq_stat &= ~UMAC_IRQ_PHY_DET_R;
		printk(KERN_CRIT "Cable unplugged in UMAC%d powering down\n", pDevCtrl->devnum);
		bcmumac_power_down(pDevCtrl, BCMUMAC_POWER_CABLE_SENSE);
	}
	if (pDevCtrl->irq_stat & UMAC_IRQ_MPD_R) {
		pDevCtrl->irq_stat &= ~UMAC_IRQ_MPD_R;
		printk(KERN_CRIT "Magic packet detected, UMAC%d waking up\n", pDevCtrl->devnum);
		/* disable mpd interrupt */
		pDevCtrl->intrl2->cpu_mask_set |= UMAC_IRQ_MPD_R;
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
		/* TODO: Read phy register 0x19 for speed/duplex/pause info.*/
		/* enable DMA channels */
		pDevCtrl->dmaRegs->flowctl_ch1_alloc = (DMA_CH1_FLOW_ALLOC_FORCE | 0);
		pDevCtrl->dmaRegs->flowctl_ch1_alloc = NR_RX_BDS;
		pDevCtrl->rxDma->cfg = DMA_ENABLE;
		pDevCtrl->txDma->cfg = DMA_ENABLE;
		
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
#if 0
		/* Disable DMA Tx/Rx channels.  */
		pDevCtrl->rxDma->cfg &= ~DMA_ENABLE;
		pDevCtrl->rxDma->cfg |= DMA_PKT_HALT;
		pDevCtrl->txDma->cfg &= ~DMA_ENABLE;
		pDevCtrl->txDma->cfg |= DMA_PKT_HALT;
#endif
#ifdef CONFIG_BCMINTEMAC_NETLINK
		if(atomic_read(&pDevCtrl->LinkState) == 1) {
			rtnl_lock()
			clear_bit(__LINK_STATE_START, &pDevCtrl->dev->state);
			netif_carrier_off(pDevCtrl->dev);
			pDevCtrl->dev->flags &= ~IFF_RUNNING;
			rtmsg_ifinfo(RTM_NEWLINK, pDevCtrl->dev, IFF_RUNNING);
			set_bit(__LINK_STATE_START, &pDevCtrl->dev->state);
			rtnl_unlock();
		}
		atomic_set(&pDevCtrl->LinkState, 0);
#endif
	}

	spin_unlock_bf(&pDeCtrl->bh_lock);
	/*
	 * multiple buffer done event */
	if (pDevCtrl->irq_stat & UMAC_IRQ_DESC_THROT)
	{
		int budget;
		pDevCtrl->irq_stat &= ~UMAC_IRQ_DESC_THROT;
		/* process all received packets */
		budget = pDevCtrl->dmaRegs->enet_isdma_desc_thres;
		bcmumac_rx(pDevCtrl, budget);
	}
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

	if (intrl2->cpu_stat & UMAC_IRQ_RXDMA_BDONE) {
		/* Rx buffer done */
		intrl2->cpu_clear |= UMAC_IRQ_RXDMA_BDONE;
		/*
		 * We use NAPI(software interrupt throttling, if
		 * Rx Descriptor throttling is not used.
		 */
		disable_irq_nosync(pDevCtrl->rxIrq);	/*??? disable all irqs ?*/
		netif_rx_schedule(pDevCtrl->dev);
	}else {

		/* all other IRQs are processed in bottom-half */
		intrl2->cpu_clear |= pDevCtrl->irq_stat;
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

	while((pDevCtrl->dmaRegs->enet_isdma_desc_alloc & DMA_DESC_ALLOC_MASK)
			&& (rxpktprocessed < budget) )
	{

    	dmaFlag = ((EMAC_SWAP32(pDevCtrl->rxBdReadPtr->length_status)) & 0xffff);
    	loopCount = 0;

		if (dmaFlag & DMA_OWN) {
			break;
		}
		rxpktprocessed++;
        /*
		 * Stop when we hit a buffer with no data, or a BD w/ no buffer.
		 * This implies that we caught up with DMA, or that we haven't been
		 * able to fill the buffers.
		 */
        if ( (pDevCtrl->rxBdReadPtr->address == (uint32) NULL)) {
            break;
        }
		/* report errors */
        if (dmaFlag & (DMA_RX_CRC_ERROR | DMA_RX_OV | DMA_RX_NO | DMA_RX_LG |DMA_RX_RXER)) {
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

			/* process next packet */
			continue;

        }/* if error packet */

        pBuf = (unsigned char *)(phys_to_virt(EMAC_SWAP32(pDevCtrl->rxBdReadPtr->address)));

        /*
         * THT: Invalidate the RAC cache again, since someone may have read near the vicinity
         * of the buffer.  This is necessary because the RAC cache is much larger than the CPU cache
         */
        bcm_inv_rac_all();

        len = ((EMAC_SWAP32(pDevCtrl->rxBdReadPtr->length_status))>>16);
        /* Null the BD field to prevent reuse */
        pDevCtrl->rxBdReadPtr->length_status &= EMAC_SWAP32(0xffff0000); //clear status.
        pDevCtrl->rxBdReadPtr->address = 0;

        /* Advance BD ptr to next in ring */
        IncRxBDptr(pDevCtrl->rxBdReadPtr, pDevCtrl);
        /* Recover the SKB pointer saved during assignment.*/
        skb = (struct sk_buff *)*(unsigned long *)(pBuf-4);

		if(pDevCtrl->bIPHdrOptimize)
			skb_pull(skb, 2);

#ifdef DUMP_DATA
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
        netif_receive_skb(skb);
        
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

    /*
     * This function may be called from timer of irq bottom-half.
     */
	spinlock_bh(&pDevCtrl->bf_lock);

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
			/* No free skb avaiable now, set the flag and let the timer function to refill. */
			atomic_set(&pDevCtrl->rxDmaRefill, 1);
			break;
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
            pDevCtrl->rxBdAssignPtr = pDevCtrl->rxFirstBdPtr;
        }
        else {
            pDevCtrl->rxBdAssignPtr->length_status |= EMAC_SWAP32(DMA_OWN);
            pDevCtrl->rxBdAssignPtr++;
        }
    }

    pDevCtrl->rxDma->cfg |= DMA_ENABLE;

	spin_unlock_bh(&pDevCtrl->bh_lock);

    return bdsfilled;
}


/*
 * init_umac: Initializes the uniMac controller
 */
static int init_umac(BcmEnet_devctrl *pDevCtrl)
{
    volatile uniMacRegs *umac;
	volatile intrl2Regs *intrl2; 
	
	umac = pDevCtrl->umac;;
	intrl2 = pDevCtrl->intrl2;

    TRACE(("bcmumacenet: init_umac\n"));

    /* disable MAC while updating its registers */
    umac->cmd = 0 ;
    /* clear tx/rx counter */
    umac->mib_ctrl = MIB_RESET_RX | MIB_RESET_TX | MIB_RESET_RUNT;

    /* issue soft reset, wait for it to complete */
    umac->cmd = CMD_SW_RESET;
	udelay(100);
	umac->cmd = ~CMD_SW_RESET;

#ifdef MAC_LOOPBACK
	/* Enable GMII/MII loopback */
    umac->cmd |= CMD_LCL_LOOP_EN;
#endif
	umac->max_frame_len = ENET_MAX_MTU_SIZE;

	/* 
	 * mii_init() will program speed/duplex/pause 
	 * parameters into umac and rbuf registers.
	 */
    if (mii_init(pDevCtrl->dev))
        return -EFAULT;

    /* 
	 * init rx registers, enable ip header optimization.
	 */
    if (pDevCtrl->bIPHdrOptimize) {
        pDevCtrl->txrx_ctrl->rbuf_ctrl = RBUF_ALIGN_2B ;
    }
#ifdef CONFIG_L4_CSUM
	umac->rxtx_ctrl->rbuf_ctrl |= RBUF_64B_EN;
	umac->txrx_ctrl->tbuf_ctrl |= RBUF_64B_EN;
#endif

	/* Interrupt mask */
	intrl2->cpu_mask_clear = UMAC_IRQ_LINK_DOWN | UMAC_IRQ_LINK_UP ;

#ifdef CONFIG_UMAC_RX_DESC_THROTTLE
	intrl2->cpu_mask_clear |= UMAC_IRQ_DESC_THROT;
#else
	intrl2->cpu_mask_clear |= UMAC_IRQ_RXDMA_BDONE;
#endif /* CONFIG_UMAC_RX_DESC_THROTTLE */
    
	/* Monitor cable plug/unpluged event for internal PHY */
	if (pDevCtrl->devnum == BCMUMAC_INT_PHY_DEV) {
		intrl2->cpu_mask_clear |= UMAC_IRQ_PHY_DET_R | UMAC_IRQ_PHY_DET_F;
	}

	/* Enable rx/tx engine.*/
	umac->cmd |= CMD_TX_EN | CMD_RX_EN;
	/* TODO: Enable more interrupts in debugging mode ?*/

	TRACE(("done init umac\n"));

    return 0;

}

/*
 * init_isdma: Initialize DMA control register
 */
static void init_isdma(BcmEnet_devctrl *pDevCtrl)
{
	volatile uniMacRegs * umac = pDevCtrl->umac;
	int speeds[] = {10, 100, 1000, 2500};
	int speed_id;
    TRACE(("bcmumacenet: init_dma\n"));

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
	/* in B0, not nessesary*/
	pDevCtrl->txDma->descOffset = (uint32)CPHYSADDR(pDevCtrl->txFirstBdPtr);

    /* Rx */
    pDevCtrl->rxDma->cfg = 0; 
    pDevCtrl->rxDma->maxBurst = DMA_MAX_BURST_LENGTH; 
    pDevCtrl->rxDma->descPtr = (uint32)CPHYSADDR(pDevCtrl->rxFirstBdPtr);
	pDevCtrl->rxDma->descOffset = (uint32)CPHYSADDR(pDevCtrl->rxFirstBdPtr);

#ifdef  CONFIG_UMAC_RX_DESC_THROTTLE
	/* 
	 * Use descriptor throttle, fire interrupt only when multiple packets are done!
	 */
	pDevCtrl->dmaRegs->enet_isdma_desc_thres = ISDMA_DESC_THRES;
	/* 
	 * Enable push timer, that is, force the IRQ_DESC_THROT to fire when timeout
	 * occcred, to prevent system slow reponse when handling low throughput data.
	 */
	speed_id = (umac->cmd >> CMD_SPEED_SHIFT) & CMD_SPEED_MASK;
	pDevCtrl->dmaRegs->enet_isdma_desc_timeout = 2*(pDevCtrl->dmaRegs->enet_isdma_desc_thres*ENET_MAX_MTU_SIZE)/speeds[speed_id];
#endif	/* CONFIG_UMAC_RX_DESC_THROTTLE */
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
    TRACE(("init_buffers: %08lx descriptors initialized, from flowctl\n",
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

#ifdef BCM_LINUX_CPU_ENET_1_IRQ
    pDevCtrl->rxIrq = pDevCtrl->devnum ? BCM_LINUX_CPU_ENET_1_IRQ : BCM_LINUX_CPU_ENET_IRQ;
#else
    pDevCtrl->rxIrq = BCM_LINUX_CPU_ENET_IRQ;
#endif
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
    pDevCtrl->rxDma = &pDevCtrl->dmaRegs->chcfg[UMAC_RX_CHAN];
    pDevCtrl->txDma = &pDevCtrl->dmaRegs->chcfg[UMAC_TX_CHAN];
    pDevCtrl->rxBds = (DmaDesc *) (pDevCtrl->dev->base_addr + UMAC_RX_DESC_OFFSET);
    pDevCtrl->txBds = (DmaDesc *) (pDevCtrl->dev->base_addr + UMAC_TX_DESC_OFFSET);
	
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

    /* if we reach this point, we've init'ed successfully */
    return 0;
}

#ifdef USE_PROC
/*
 * display/decode Rx BD status
 */
char * bcmumac_display_rxbds(char * buf, unsigned long * addr)
{
	int i;
	int len = 0;
	
	len = sprintf(buf, "Addr      O L F W FI   B M LG NO RxER CRC OV B_Addr\n");
	for ( i = 0 ; i < NR_RX_BDS; i ++) {
		len += sprintf(buf+len, "0x%08lX %lu %lu %lu %lu 0x%02lX %lu %lu %lu %lu %lu %lu %lu 0x%08lX\n",
			(unsigned long)addr,
			(*addr >> 16) & DMA_OWN,
			(*addr >> 16) & DMA_EOP,
			(*addr >> 16) & DMA_SOP,
			(*addr >> 16) & DMA_WRAP,
			((*addr >> 16) >> DMA_RX_FI_SHIFT) & DMA_RX_FI_MASK,
			(*addr >> 16) & DMA_RX_BRDCAST,
			(*addr >> 16) & DMA_RX_MULT,
			(*addr >> 16) & DMA_RX_LG,
			(*addr >> 16) & DMA_RX_NO,
			(*addr >> 16) & DMA_RX_RXER,
			(*addr >> 16) & DMA_RX_CRC_ERROR,
			(*addr >> 16) & DMA_RX_OV,
			*(addr+1)
			),
		addr += 2;
	}
	return buf;
}
/*
 * display/decode Tx BD status.
 */
char * bcmumac_display_txbds(char *buf, unsigned long * addr)
{
	int i;
	int len = 0;
	len = sprintf(buf, "Addr      O L F W UR   A_CRC O_CRC CSUM QTAG B_Addr\n");
	for ( i = 0 ; i < NR_TX_BDS; i ++) {
		len += sprintf(buf+len, "0x%08lX %lu %lu %lu %lu %lu %lu %lu %lu 0x%02lX 0x%08lX\n",
			(unsigned long)addr,
			(*addr >> 16) & DMA_OWN,
			(*addr >> 16) & DMA_EOP,
			(*addr >> 16) & DMA_SOP,
			(*addr >> 16) & DMA_WRAP,
			(*addr >> 16) & DMA_TX_UNDERRUN,
			(*addr >> 16) & DMA_TX_APPEND_CRC,
			(*addr >> 16) & DMA_TX_OW_CRC,
			(*addr >> 16) & DMA_TX_DO_CSUM,
			(*addr >> 16) & DMA_TX_QTAG_MASK,
			*(addr+1)
			),
		addr += 2;
	}
	return buf;
}
/*
 * display MDF content
 */
char * bcmumac_display_mdf(char * buf, unsigned long * addr)
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
	return buf;
}
/* 
 * disaplay HFB memory content.
 */
char *bcmumac_display_hfb(char * buf, unsigned long * addr)
{
	int i;
	int len = 0;
	for (i = 0 ; i < 1024; i++) {
		len += sprintf(buf+len, "%01lx %04lx\n", (*addr >> 16) & 0xF,
			*addr & 0xFFFF);
	}
	return buf;
}
/* 
 * Special proc entries for debug,
 * other entries are generated by perl script.
 */
umacProcEntry debug_entries[] = {
	{0, &bcmumac_display_rxbds, 0,0,0,0,0, "uniMacRegs/rxbds"},
	{0, &bcmumac_display_txbds, 0,0,0,0,0, "uniMacRegs/txbds"},
	{0, &bcmumac_display_mdf, 0,0,0,0,0, "uniMacRegs/mdf"},
	{0, &bcmumac_display_hfb, 0,0,0,0,0, "hfb"}
};
/*
 * initialize and create proc entries
 */
static void bcmumac_init_proc(BcmEnet_devctrl *pDevCtrl)
{
	int i;
	for (i = 0 ; i < sizeof(umac_proc_entries)/sizeof(umacProcEntry); i++)
	{
		umac_proc_entries->context = (void*)pDevCtrl;
		if(bcmumac_proc_create_entry("/proc/umacInfo", &umac_proc_entries[i]) < 0)
			printk(KERN_ERR "%s: Failed to create proc entry for %s\n", __FUNCTION__, 
					umac_proc_entries[i].name);
	}
	for (i = 0 ; i < sizeof(debug_entries)/sizeof(umacProcEntry); i++)
	{
		debug_entries->context = (void*)pDevCtrl;
		if(bcmumac_proc_entry_create("/proc/umacInfo", &debug_entries[i]) < 0)
			printk(KERN_ERR "%s: Failed to create proc entry for %s\n", __FUNCTION__, 
					debug_entries[i].name);
	}
	
}

/* remove proc entries */
static void bcmumac_uninit_proc(BcmEnet_devctrl * pDevCtrl)
{
	int i;
	for (i = 0 ; i < sizeof(umac_proc_entries)/sizeof(umacProcEntry); i++)
	{
		bcmumac_proc_entry_remove("/proc/umacInfo", umac_proc_entries[i].name);
	}
	for (i = 0 ; i < sizeof(debug_entries)/sizeof(umacProcEntry); i++)
	{
		bcmumac_proc_entry_remove("/proc/umacInfo", debug_entries[i].name);
	}
}
#endif	/* USE_PROC */

static void bcmumac_getMacAddr(void)
{
	extern int gNumHwAddrs;
	extern unsigned char* gHwAddrs[];
	int i;
	
   	if (gNumHwAddrs >= 1) {
		for (i=0; i < 6; i++)
			g_flash_eaddr[i] = (uint8) gHwAddrs[0][i];
   	} else {
		printk(KERN_ERR "%s: No MAC addresses defined\n", __FUNCTION__);
	}
}
/*
 * Setup device structure after allocating netdevice.
 */
static void bcmumac_dev_setup(struct net_device *dev, int devnum, int phy_id, int phy_type, unsigned long base_addr)
{
    int ret;
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);

    if (pDevCtrl == NULL) {
        printk((KERN_ERR CARDNAME ": unable to allocate device context\n"));
        return;
    }

    /* initialize the context memory */
    memset(pDevCtrl, 0, sizeof(BcmEnet_devctrl));
    /* back pointer to our device */
    pDevCtrl->dev = dev;
    pDevCtrl->devnum = devnum;
    dev->base_addr = base_addr;

    pDevCtrl->chipId = BDEV_RD(BCHP_SUN_TOP_CTRL_PROD_REVISION) >> 16;
    pDevCtrl->chipRev = BDEV_RD(BCHP_SUN_TOP_CTRL_PROD_REVISION) & 0xffff;

    /* print the ChipID and module version info */
    if(! devnum) {
        printk("Broadcom BCM%X P%X Ethernet Network Device ", pDevCtrl->chipId, pDevCtrl->chipRev + 0x10);
        printk(VER_STR "\n");
    }

    if( BpGetEthernetMacInfo( &(pDevCtrl->EnetInfo), phy_type) != BP_SUCCESS ) {
        printk(KERN_DEBUG CARDNAME" Unknown PHY type: %d \n", phy_type);
        return;
    }
    if(phy_id != BP_ENET_NO_PHY) 
        pDevCtrl->EnetInfo.PhyAddress = phy_id;

	/* L.Sun, Always have IPHdrOptimize */
	pDevCtrl->bIPHdrOptimize = 1;
	
	/* g_flash_eaddr? */
    memcpy(dev->dev_addr, g_flash_eaddr, ETH_ALEN);
    /* create a unique address for each interface */
    dev->dev_addr[4] += devnum;

    if ((ret = bcmumac_init_dev(pDevCtrl))) {
        bcmumac_uninit_dev(pDevCtrl);
        return;
    }

    /* setup the rx irq */
    /* register the interrupt service handler */
    /* At this point dev is not initialized yet, so use dummy name */
    request_irq(pDevCtrl->rxIrq, bcmumac_net_isr, SA_INTERRUPT|SA_SHIRQ, dev->name, pDevCtrl);

    spin_lock_init(&pDevCtrl->lock);
    /*
     * Setup the timer for skb reclaim.
     */
    init_timer(&pDevCtrl->timer);
    pDevCtrl->timer.data = (unsigned long)pDevCtrl;
    pDevCtrl->timer.function = tx_reclaim_timer;

#ifdef DUMP_DATA
    printk(KERN_INFO CARDNAME ": CPO BRCMCONFIG: %08X\n", read_c0_diag(/*$22*/));
    printk(KERN_INFO CARDNAME ": CPO MIPSCONFIG: %08X\n", read_c0_config(/*$16*/));
    printk(KERN_INFO CARDNAME ": CPO MIPSSTATUS: %08X\n", read_c0_status(/*$12*/));
#endif	
    ether_setup(dev);
   	dev->irq                    = pDevCtrl->rxIrq;
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

	SET_ETHTOOL_OPS(dev, &bcmumac_ethtool_ops);
    // These are default settings
    //write_mac_address(dev);

    // Let boot setup info overwrite default settings.
    netdev_boot_setup_check(dev);

    TRACE(("bcmumacenet: bcmumac_net_probe dev 0x%x\n", (unsigned int)dev));

    SET_MODULE_OWNER(dev);
}

static void bcmumac_null_setup(struct net_device *dev)
{
}

/*
 *      bcmumac_net_probe: - Probe Ethernet and allocate device
 */
static int __init bcmumac_net_probe(int devnum, int phy_id, unsigned long base_addr)
{
    /*static*/ 
	int probed = 0;
    int ret;
    struct net_device *dev = NULL;
    BcmEnet_devctrl *pDevCtrl;
	int phy_type = BP_ENET_NO_PHY;

	/*
	 * 7420 Phy0 is internal phy, phy1 is MOCA.
	 * This is the place where different board should set different PHY type
	 */
#if defined CONFIG_MIPS_BCM7420
	phy_type = devnum == 0 ? BP_ENET_INTERNAL_PHY : BP_ENET_EXTERNAL_MOCA;
#else
	phy_type = devnum == 0 ? BP_ENET_INTERNAL_PHY : BP_ENET_EXTERNAL_MOCA;
#endif

    if (probed == 0) {
		/* NOTE: BcmEnet_devctrl struct is allocated with kzalloc */
        dev = alloc_netdev(sizeof(struct BcmEnet_devctrl), "eth%d", bcmumac_null_setup);
        if (dev == NULL) {
            printk(KERN_ERR CARDNAME": Unable to allocate net_device structure!\n");
            return -ENODEV;
        }
        bcmumac_dev_setup(dev, devnum, phy_id, phy_type, base_addr);
    } else {
        /* device has already been initialized */
        return -ENXIO;
    }

    pDevCtrl = (BcmEnet_devctrl *)netdev_priv(dev);

    if (0 != (ret = register_netdev(dev))) {
        bcmumac_uninit_dev(pDevCtrl);
        return ret;
    }

    pDevCtrl->next_dev = eth_root_dev;
    eth_root_dev = dev;

	INIT_WORK(&pDevCtrl->bcmumac_task, (void (*)(void *))bcmumac_task, pDevCtrl);

    if(ret == 0) {
        g_devs[g_num_devs] = dev;
        g_num_devs++;
    }

    return ret;
}

/*
 * Generic cleanup handling data allocated during init. Used when the
 * module is unloaded or if an error occurs during initialization
 */
static void bcmumac_init_cleanup(struct net_device *dev)
{
    TRACE(("%s: bcmumac_init_cleanup\n", dev->name));

#ifdef USE_PROC
	bcmumac_proc_entry_remove("/proc/umacInfo", debug_entries[i].name);
#endif
}

/* Uninitialize tx/rx buffer descriptor pools */
static int bcmumac_uninit_dev(BcmEnet_devctrl *pDevCtrl)
{
    Enet_Tx_CB *txCBPtr;
    int i, devnum __attribute_unused__ = pDevCtrl->devnum;

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

        bcmumac_init_cleanup(pDevCtrl->dev);
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
#ifdef USE_PROC
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
		wol->wolopts |= WAKE_MAGICSECURE;
		copy_to_user(&wol->sopass[0], &umac->mpd_pw_ls, 4);
		pwd_ms = umac->mpd_pw_ms & 0xFFFF;
		copy_to_user(&wol->sopass[4], &umac->mpd_pw_ms, 2);
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
	unsigned short msb;

	if(wol->wolopts & ~(WAKE_MAGIC | WAKE_MAGICSECURE))
		return -EINVAL;
	if(wol->wolopts & WAKE_MAGICSECURE)
	{
		if(copy_from_user(&msb, &wol->sopass[0], 2)) {
			return -EFAULT;
		}
		umac->mpd_pw_ms = msb;
		if(copy_from_user(&umac->mpd_pw_ls, &wol->sopass[2], 4)){
			return -EFAULT;
		}
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

	if(pDevCtrl->devnum == BCMUMAC_INT_PHY_DEV)
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
	uniMacRegs * umac = pDevCtrl->umac;

	if(cmd->autoneg == AUTONEG_ENABLE) {
		mii_setup(dev);
	}else {
		if (cmd->duplex == DUPLEX_HALF)
			umac->cmd |= CMD_HD_EN;
		else
			umac->cmd &= ~CMD_HD_EN;

		if(pDevCtrl->devnum == BCMUMAC_INT_PHY_DEV && cmd->speed == SPEED_1000)
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
	.get_link			= ethtool_op_get_link,
//	.nway_reset			= bcmumac_nway_reset,
};
/*
 * disable clocks according to mode, cable sense/WoL
 */
static void bcmumac_disable_clocks(BcmEnet_devctrl * pDevCtrl, int mode)
{
	int  pm_ctrl;
#if 0
	if(pDevCtrl->devnum == BCMUMAC_INT_PHY_DEV)
		pm_ctrl = BCHP_CLK_GENET_CLK_PM_CTRL;
	else
		pm_ctrl = BCHP_CLK_MOCA_CLK_PM_CTRL;
	
	/* common clocks that should be disabled regardless of mode.*/
	BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_216M_CLK_MASK);
	BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_ISDMA_27_125M_CLK_MASK);
	BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_UNIMAC_SYS_TX_27_125M_CLK_MASK);
	BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_GISB_108M_CLK_MASK);
	/* 
	 * Disable RGMII 250M clock, since we don't have external GPHY, 
	 * in the future, may need to remove this.
	 */
	BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_RGMII_250M_CLK_MASK);
	BDEV_RD(pm_ctrl);
	switch(mode) {
		case BCMUMAC_POWER_CABLE_SENSE:
			BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_GMII_TX_27_125M_CLK_MASK);
			BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_HFB_27_125M_CLK_MASK);
			BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_UNIMAC_SYS_RX_27_125M_CLK_MASK);
			BDEV_RD(pm_ctrl);
			break;
		case BCMUMAC_POWER_WOL_MAGIC:
			BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_HFB_27_125M_CLK_MASK);
			BDEV_RD(pm_ctrl);
			break;
		case BCMUMAC_POWER_WOL_ACPI:
			/* no need to do anything here */
			break;
		default:
			break;
	}
#endif
	TRACE(("Disabling clocks...\n"));
	
}
/*
 * Enable clocks.
 */
static void bcmumac_enable_clocks(BcmEnet_devctrl * pDevCtrl, int mode)
{
	int  pm_ctrl;
#if 0
	if(pDevCtrl->devnum == BCMUMAC_INT_PHY_DEV)
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
		BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_RGMII_250M_CLK_MASK);
		BDEV_SET(pm_ctrl, BCHP_CLK_GENET_CLK_PM_CTRL_DIS_GENET_GMII_27_125M_CLK_MASK);
		BDEV_RD(pm_ctrl);
	}
#endif
	TRACE(("Enabling clocks...\n"));
	
}
/*
 * Program ACPI pattern into HFB.
 * Return filter index if succesful.
 */
static int bcmumac_update_hfb(struct net_device *dev, unsigned long *data, int len)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	volatile rbufRegs *txrx_ctrl = pDevCtrl->txrx_ctrl;
	int filter, i, offset, count;
	
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
		if((txrx_ctrl->rbuf_hfb_ctrl >> (filter + RBUF_HFB_FILTER_EN_SHIFT)) & 0X1)
			break;
	}
	if(filter == count) {
		printk(KERN_ERR "no unused filter available!\n");
		return -EINVAL;	/* all filters have been enabled*/
	}
	
	/* copy pattern data */
	if(copy_from_user((pDevCtrl->hfb + filter*offset),
				data, len*sizeof(unsigned long)) != 0)
	{
		printk(KERN_ERR "Failed to copy user data: src=0x%08Xul, dst=0x%08Xul\n", 
				data, pDevCtrl->hfb + filter*offset);
		return -EFAULT;
	}
	/* set the filter length*/
	txrx_ctrl->rbuf_fltr_len[filter/4] = (len << RBUF_FLTR_LEN_SHIFT * (filter%4));
	/*enable this filter.*/
	txrx_ctrl->rbuf_hfb_ctrl |= (filter << RBUF_HFB_FILTER_EN_SHIFT);

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
		printk(KERN_ERR "failed to get user data\n");
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
	if (txrx_ctrl->rbuf_hfb_ctrl & (filter << RBUF_HFB_FILTER_EN_SHIFT) == 0) {
		len = 0;
		put_user(len , &u_data->count);
		return 0;
	}
	/* check the filter length*/
	len = RBUF_FLTR_LEN_MASK & (txrx_ctrl->rbuf_fltr_len[filter/4] >> RBUF_FLTR_LEN_SHIFT * (filter%4));
	/* copy pattern data */
	if(copy_to_user(u_data->p_data, pDevCtr->hfb + filter*offset, len)) {
		printk(KERN_ERR "Failed to copy data to user space: src=0x%08Xul, dst=0x%08Xul\n", 
				pDevCtrl->hfb+filter*offset, u_data->p_data);
		return -EFAULT;
	}
	return 0;
}
/*
 * clear the HFB, disable all filters.
 */
static inline void bcmumac_clear_hfb(BcmEnet_devctrl * pDevCtrl)
{
	pDevCtrl->txrx_ctrl->rbuf_hfb_ctrl &= ~RBUF_HFB_FILTER_EN_MASK;
	
}
/*
 * Power down the unimac, based on mode.
 */
static void bcmumac_power_down(BcmEnet_devctrl *pDevCtrl, int mode)
{
	int retries = 0;

	switch(mode) {
		case BCMUMAC_POWER_CABLE_SENSE:
			pDevCtrl->txrx_ctrl->ephy_pwr_mgmt |= EXT_PWR_DOWN_PHY |
				EXT_PWR_DOWN_DLL |
				EXT_PWR_DOWN_BIAS;
			pDevCtrl->txrx_ctrl->rgmii_oob_ctrl |= RGMII_MODE_EN;
			bcmumac_disable_clocks(pDevCtrl, mode);
			break;
		case BCMUMAC_POWER_WOL_MAGIC:
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
			/* put umac in 27Mhz clock, how?*/
			bcmumac_disable_clocks(pDevCtrl, mode);
			pDevCtrl->intrl2->cpu_mask_clear |= UMAC_IRQ_MPD_R;
			break;
		case BCMUMAC_POWER_WOL_ACPI:
			bcmumac_clear_hfb(pDevCtrl);
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
			//bcmumac_update_hfb(pDevCtrl, data, len);
			bcmumac_disable_clocks(pDevCtrl, mode);
			pDevCtrl->intrl2->cpu_mask_clear |= UMAC_IRQ_HFB_MM | UMAC_IRQ_HFB_SM;
			break;
		default:
			break;
	}
	
}
static void bcmumac_power_up(BcmEnet_devctrl *pDevCtrl, int mode)
{
	switch(mode) {
		case BCMUMAC_POWER_CABLE_SENSE:
			pDevCtrl->txrx_ctrl->ephy_pwr_mgmt |= PHY_RESET;
			pDevCtrl->txrx_ctrl->ephy_pwr_mgmt &= ~EXT_PWR_DOWN_DLL;
			pDevCtrl->txrx_ctrl->ephy_pwr_mgmt &= ~EXT_PWR_DOWN_PHY;
			udelay(1);
			pDevCtrl->txrx_ctrl->ephy_pwr_mgmt &= ~PHY_RESET;
			udelay(200);
			bcmumac_enable_clocks(pDevCtrl, mode);
			//mii_setup(pDevCtrl->dev);
			break;
		case BCMUMAC_POWER_WOL_MAGIC:
			pDevCtrl->umac->mpd_ctrl &= ~MPD_EN;
			bcmumac_enable_clocks(pDevCtrl, mode);
			break;
		case BCMUMAC_POWER_WOL_ACPI:
			pDevCtrl->txrx_ctrl->rbuf_hfb_ctrl &= ~RBUF_ACPI_EN;
			bcmumac_clear_hfb(pDevCtrl);
			bcmumac_enable_clocks(pDevCtrl, mode);
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
	struct acpi_data u_data;
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
		val =  bcmumac_update_hfb(dev, (unsigned long *)u_data->p_data, u_data->count)
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

static int __init bcmumac_module_init(void)
{
    int status = 0;
	int phy_id = BP_ENET_NO_PHY;
	int i;
	unsigned long base_addr;
	struct resource * res;

    bcmumac_getMacAddr();

#if defined(CONFIG_BCMINTEMAC_7038_EXTMII)
    /* set up pinmux (same code for EMAC_0 or EMAC_1) */
    BDEV_WR_ARRAY(MII_PINMUX_SETUP);
#endif

    TRACE(("bcmumac_module_init\n"));

	/* Probe the PHY */
	for ( i = 0; i < BCMUMAC_MAX_DEVS; i++)
	{
		if(umac_base_addr[i] != 0)
		{
			/*
			 * standard linux io address scheme, map the physical to virtual
			 * so mii_probe can access the mdio registers.
			 */
			if(( res = request_region(umac_base_addr[i], UMAC_IOMAP_SIZE, "UMACIO")) == NULL)
			{
				printk("Faild to request region: base=0x%08x size=0x%08x\n", 
						umac_base_addr[i], UMAC_IOMAP_SIZE);
				return -ENODEV;
			}else if ((base_addr = (unsigned long)ioremap(umac_base_addr[i], UMAC_IOMAP_SIZE)) == NULL)
			{
				printk("Failed to ioremap: physical addr=0x%08lx size=0x%08x\n",
						umac_base_addr[i], UMAC_IOMAP_SIZE);
				release_region(umac_base_addr[i], UMAC_IOMAP_SIZE);
				return -ENODEV;
			}
			if( (phy_id = mii_probe(base_addr)) != BP_ENET_NO_PHY)
			{
				printk("bcmumac: detected PHY at address %d on UMAC%d\n", phy_id, i);
				status = bcmumac_net_probe(i, phy_id, base_addr);
			}else
			{
				printk("bcmumac: no PHY detected on UMAC%d , disabling\n", i);
			}
		}
	}
	TRACE(("bcmumac_module_init done\n"));
						
	return status;
}

static void __exit bcmumac_module_cleanup(void)
{
	int i;
    BcmEnet_devctrl *pDevCtrl;
	
	for(i = 0; i < g_num_devs; i++) {
		pDevCtrl = netdev_priv(g_devs[i]);
		
		if (pDevCtrl->base_addr != 0) {
			iounmap(pDevCtrl->base_addr);
			release_region(pDevCtrl->base_addr, UMAC_IOMAP_SIZE);
		}
		bcmumac_uninit_dev(pDevCtrl);
	}
	eth_root_dev = NULL;
    TRACE(("bcmumac: bcmumac_module_cleanup\n"));
}

int isEnetConnected(char *pn)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(bcmumac_get_device());
	return atomic_read(&pDevCtrl->linkState); 
}

module_init(bcmumac_module_init);
module_exit(bcmumac_module_cleanup);

EXPORT_SYMBOL(bcmumac_get_free_txdesc);
EXPORT_SYMBOL(bcmumac_get_device);
EXPORT_SYMBOL(bcmumac_xmit_multibuf);
EXPORT_SYMBOL(isEnetConnected);
EXPORT_SYMBOL(bcmumac_xmit_check);
