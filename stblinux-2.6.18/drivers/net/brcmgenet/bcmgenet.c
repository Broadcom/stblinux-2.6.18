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
// 05/04/2009 lei sun, revision for 7420B0
//**************************************************************************

#define CARDNAME    "BCMGENET"
#define VERSION     "2.0"
#define VER_STR     "v" VERSION " " __DATE__ " " __TIME__

#if defined(CONFIG_MODVERSIONS) && ! defined(MODVERSIONS)
   #include <linux/modversions.h> 
   #define MODVERSIONS
#endif  

#include <linux/kernel.h>
#include <linux/module.h>
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
#include <linux/dma-mapping.h>

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
#include "bcmgenet.h"
#include "if_net.h"

#define RX_BUF_LENGTH		2048	/* 2Kb buffer */
#define SKB_ALIGNMENT		32		/* 256B alignment */
#define DMA_DESC_THRES		64		/* Rx Descriptor throttle threshold */
#define CLEAR_ALL_HFB		0xff
#define HFB_TCP_LEN 		19		/* filter content length for TCP packet */
#define HFB_ARP_LEN			21		/* filter content length for ARP packet */

// --------------------------------------------------------------------------
//      External, indirect entry points. 
// --------------------------------------------------------------------------
static int bcmgenet_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
// --------------------------------------------------------------------------
//      Called for "ifconfig ethX up" & "down"
// --------------------------------------------------------------------------
static int bcmgenet_open(struct net_device * dev);
static int bcmgenet_close(struct net_device * dev);
// --------------------------------------------------------------------------
//      Watchdog timeout
// --------------------------------------------------------------------------
static void bcmgenet_timeout(struct net_device * dev);
// --------------------------------------------------------------------------
//      Packet transmission. 
// --------------------------------------------------------------------------
static int bcmgenet_xmit(struct sk_buff * skb, struct net_device * dev);
// --------------------------------------------------------------------------
//      Set address filtering mode
// --------------------------------------------------------------------------
static void bcmgenet_set_multicast_list(struct net_device * dev);
// --------------------------------------------------------------------------
//      Set the hardware MAC address.
// --------------------------------------------------------------------------
static int bcmgenet_set_mac_addr(struct net_device *dev, void *p);

// --------------------------------------------------------------------------
//      Interrupt routine, for all interrupts except ring buffer interrupts
// --------------------------------------------------------------------------
static irqreturn_t bcmgenet_isr0(int irq, void *, struct pt_regs *regs);

/*---------------------------------------------------------------------------
 *      IRQ handler for ring buffer interrupt.
 *--------------------------------------------------------------------------*/
static irqreturn_t bcmgenet_isr1(int irq, void *, struct pt_regs *regs);
 
// --------------------------------------------------------------------------
//      dev->poll() method
// --------------------------------------------------------------------------
static int bcmgenet_poll(struct net_device * dev, int *budget);
// --------------------------------------------------------------------------
//      Process recived packet for descriptor based DMA 
// --------------------------------------------------------------------------
static uint32 bcmgenet_desc_rx(void *ptr, uint32 budget);

// --------------------------------------------------------------------------
//      Internal routines
// --------------------------------------------------------------------------
/* Allocate and initialize tx/rx buffer descriptor pools */
static int bcmgenet_init_dev(BcmEnet_devctrl *pDevCtrl);
static int bcmgenet_uninit_dev(BcmEnet_devctrl *pDevCtrl);
/* Assign the Rx descriptor ring */
static int assign_rx_buffers(BcmEnet_devctrl *pDevCtrl);
/* Initialise the uniMac control registers */
static int init_umac(BcmEnet_devctrl *pDevCtrl);
/* Initialize DMA control register */
static void init_edma(BcmEnet_devctrl *pDevCtrl);
/* Interrupt bottom-half */
static void bcmgenet_irq_task(BcmEnet_devctrl *pDevCtrl);
/* power management */
static void bcmgenet_power_down(BcmEnet_devctrl *pDevCtrl, int mode);
static void bcmgenet_power_up(BcmEnet_devctrl *pDevCtrl, int mode);

#ifdef CONFIG_BCMUMAC_DUMP_DATA
/* Display hex base data */
static void dumpHexData(unsigned char *head, int len);
/* dumpMem32 dump out the number of 32 bit hex data  */
static void dumpMem32(uint32 * pMemAddr, int iNumWords);
#endif

static struct net_device *eth_root_dev = NULL;
static int DmaDescThres = DMA_DESC_THRES;
/* 
 * HFB data for ARP request.
 * In WoL (Magic Packet or ACPI) mode, we need to response
 * ARP request, so dedicate an HFB to filter the ARP request.
 * NOTE: the last two words are to be filled by destination.
 */
static uint32 hfb_arp[] =
{
	0x000FFFFF, 0x000FFFFF, 0x000FFFFF,	0x00000000,	0x00000000,
	0x00000000, 0x000F0806,	0x000F0001,	0x000F0800,	0x000F0604,
	0x000F0001,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x00000000,	0x000F0000,	0x000F0000,	0x000F0000,	0x000F0000,
	0x000F0000
};
#if 0
/* --------------------------------------------------------------------------
    Name: bcmemac_get_free_txdesc
 Purpose: Get Current Available TX desc count
-------------------------------------------------------------------------- */
int bcmemac_get_free_txdesc( struct net_device *dev ){
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    return pDevCtrl->txFreeBds;
}
#endif
struct net_device * bcmemac_get_device(void) {
	return eth_root_dev;
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
/* --------------------------------------------------------------------------
    Name: bcmgenet_open
 Purpose: Open and Initialize the EMAC on the chip
-------------------------------------------------------------------------- */
static int bcmgenet_open(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;

    TRACE(("%s: bcmgenet_open, umac->cmd=%lx, RxDMA=%u, rdma_ctrl=%lu\n", 
        dev->name, pDevCtrl->umac->cmd, (int)pDevCtrl->rxDma, pDevCtrl->rxDma->rdma_ctrl));

    /* disable ethernet MAC while updating its registers */
    pDevCtrl->umac->cmd &= ~(CMD_TX_EN | CMD_RX_EN);

	pDevCtrl->txDma->tdma_ctrl = 0;
	pDevCtrl->rxDma->rdma_ctrl = 0;
	pDevCtrl->umac->tx_flush = 1;

	/* reset dma, start from begainning of the ring. */
    init_edma(pDevCtrl);
	/* reset internal book keeping variables. */
	pDevCtrl->txLastCIndex = 0;
	pDevCtrl->rxBdAssignPtr = pDevCtrl->rxBds;
	assign_rx_buffers(pDevCtrl);

	pDevCtrl->rxDma->rdma_ctrl = (1 << DMA_RING_DESC_INDEX | DMA_EN);
	pDevCtrl->txDma->tdma_ctrl = (1 << DMA_RING_DESC_INDEX | DMA_EN);

    // Start the network engine
    netif_start_queue(dev);
    
	pDevCtrl->umac->cmd |= (CMD_TX_EN | CMD_RX_EN);

    return 0;
}


/* --------------------------------------------------------------------------
    Name: bcmgenet_close
 	Purpose: Stop communicating with the outside world
    Note: Caused by 'ifconfig ethX down'
-------------------------------------------------------------------------- */
static int bcmgenet_close(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
	int timeout = 0;

    TRACE(("%s: bcgenet_close\n", dev->name));

	/* Stop Tx DMA */
	pDevCtrl->txDma->tdma_ctrl &= ~DMA_EN;
	while(timeout < 5000)
	{
		if(!(pDevCtrl->txDma->tdma_status & DMA_EN))
			break;
		udelay(1);
		timeout++;
	}
	if(timeout == 5000)
		printk(KERN_ERR "Timed out while shutting down Tx DMA\n");

	/* Disable Rx DMA*/
	pDevCtrl->rxDma->rdma_ctrl &= ~DMA_EN;
	timeout = 0;
	while(timeout < 5000)
	{
		if(!(pDevCtrl->rxDma->rdma_status & DMA_EN))
			break;
		udelay(1);
		timeout++;
	}
	if(timeout == 5000)
		printk(KERN_ERR "Timed out while shutting down Rx DMA\n");

    pDevCtrl->umac->cmd &= ~(CMD_RX_EN | CMD_TX_EN);

    /* free the transmited skb */
	bcmgenet_xmit(NULL, dev);
    netif_stop_queue(dev);

	return 0;
}

/* --------------------------------------------------------------------------
    Name: bcmgenet_net_timeout
 Purpose: 
-------------------------------------------------------------------------- */
static void bcmgenet_timeout(struct net_device * dev)
{
    BUG_ON(dev == NULL);

    TRACE(("%s: bcmgenet_timeout\n", dev->name));

    dev->trans_start = jiffies;

    netif_wake_queue(dev);
}

/* --------------------------------------------------------------------------
 Name: bcmgenet_set_multicast_list
 Purpose: Set the multicast mode, ie. promiscuous or multicast
-------------------------------------------------------------------------- */
static void bcmgenet_set_multicast_list(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	struct dev_mc_list * dmi;
	int i;
#define MAX_MC_COUNT	16

    TRACE(("%s: bcmgenet_set_multicast_list: %08X\n", dev->name, dev->flags));

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
/*
 * Set the hardware MAC address.
 */
static int bcmgenet_set_mac_addr(struct net_device *dev, void *p)
{
    struct sockaddr *addr=p;

    if(netif_running(dev))
        return -EBUSY;

    memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

    return 0;
}
static struct net_device_stats *
bcmgenet_get_stats(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;

    return &(pDevCtrl->stats);
}

/* 
 * Get ulp (upper layer protocol) information for tx checksum offloading
 */
static inline int bcmgenet_get_ulpinfo(struct sk_buff * skb, int * ulp_offset, int * csum_offset)
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
		printk(KERN_CRIT "bcmgenet_net_xmit attemping to insert csum for non-IP packet\n");
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
			printk(KERN_CRIT "bcmgenet_net_xmit unknown protocol %d \n", transport_proto);
			return -1;
			break;
	}
	return 0;
}
/* --------------------------------------------------------------------------
 Name: bcmgenet_xmit
 Purpose: Send ethernet traffic
-------------------------------------------------------------------------- */
static int bcmgenet_xmit(struct sk_buff * skb, struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    Enet_CB *txCBPtr;
	uint32 c_index, p_index;
	int lastTxedCnt, i;
    unsigned long flags;
#ifdef CONFIG_BCMUMAC_TX_CSUM
	TSB * tsb;
	struct iphdr * iph;
	int ulp_offset, csum_offset;
#endif

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */
    spin_lock_irqsave(&pDevCtrl->lock, flags);
        
	/* Compute how many buffers are transmited since last xmit call */
	c_index = pDevCtrl->txDma->tDmaRings[DMA_RING_DESC_INDEX].tdma_consumer_index;

	if(c_index > pDevCtrl->txLastCIndex) {
		lastTxedCnt = c_index - pDevCtrl->txLastCIndex;
	}else {
		lastTxedCnt = pDevCtrl->nrTxBds - pDevCtrl->txLastCIndex + c_index;
	}
	/* Recalaim transmitted buffers */
	for(i = pDevCtrl->txLastCIndex; i < lastTxedCnt; i++)
	{
		txCBPtr = &pDevCtrl->txCbs[i];
		if(txCBPtr->skb != NULL) {
            dev_kfree_skb_any (txCBPtr->skb);
		}
		pDevCtrl->txFreeBds += 1;
	}
    if (skb == NULL && pDevCtrl->txFreeBds > 0) {
		netif_wake_queue(dev);
        spin_unlock_irqrestore(&pDevCtrl->lock, flags);
        return 0;
    }
	pDevCtrl->txLastCIndex = c_index;
    TRACE(("bcmgenet_xmit, tdma_ctrl=%08x\n", (unsigned int)pDevCtrl->txDma->tdma_ctrl));
	p_index = pDevCtrl->txDma->tDmaRings[DMA_RING_DESC_INDEX].tdma_producer_index;
	txCBPtr = &pDevCtrl->txCbs[p_index];
    txCBPtr->skb = skb;
	txCBPtr->BdAddr = &pDevCtrl->txBds[p_index];
	txCBPtr->dma_addr = dma_map_single(&pDevCtrl->generic_dev, skb->data, 
			skb->len, DMA_TO_DEVICE);

	/* 
	 * If L4 checksum offloading enabled, must make sure skb has
	 * enough headroom for us to insert 64B status block.
	 */
#ifdef CONFIG_BCMUMAC_TX_CSUM
	if(skb->ip_summed  == CHECKSUM_HW)
	{
		pDevCtrl->rbuf->tbuf_ctrl |= RBUF_64B_EN;
		if(skb_headroom(skb) < 64) {
			printk(KERN_ERR "%s: bcmgenet_net_ximt no enough headroom for HW checksum\n", dev->name);
			goto err_out;
		}
		/* ipv6 or ipv4? only support IP , this is ugly!*/
		if(bcmgenet_get_ulpinfo(skb, &ulp_offset, &csum_offset) < 0)
		{
			printk(KERN_ERR "%s: bcmgenet_net_ximt Failed to get ulp info\n", dev->name);
			goto err_out;
		}
	
		/* Insert 64B TSB and set the flag */
		skb_push(skb, 64);
		tsb = (TSB *)skb->data;
		tsb->length_status = (ulp_offset << TSB_ULP_SHIFT) | csum_offset | TSB_LV; 
		TRACE(("TSB->length_status=0x%08lX csum=0x%04x\n", tsb->length_status,
					*(unsigned short*)(skb->data+64+csum_offset)));
	}else {
		pDevCtrl->rbuf->tbuf_ctrl &= ~RBUF_64B_EN;
	}
#endif	/* CONFIG_BCMUMAC_TX_CSUM */

    /*
     * Add the buffer to the ring.
     * Set addr and length of DMA BD to be transmitted.
     */

    txCBPtr->BdAddr->address = txCBPtr->dma_addr;
    txCBPtr->BdAddr->length_status  = ((unsigned long)((skb->len < ETH_ZLEN) ? ETH_ZLEN : skb->len))<<16;
#ifdef CONFIG_BCMUMAC_DUMP_DATA
    printk("bcmgenet_xmit: len %d", skb->len);
    dumpHexData(skb->data, skb->len);
#endif

	/* set wrap bit if we are the last BD*/
	if(p_index == pDevCtrl->nrTxBds-1) {
		txCBPtr->BdAddr->length_status |= DMA_WRAP;
	}
#ifdef CONFIG_BCMUMAC_TX_CSUM
	if(skb->ip_summed  == CHECKSUM_HW)
		txCBPtr->BdAddr->length_status |= DMA_TX_DO_CSUM;
#endif
	/* DEBUG , insert QTAG for MoCA */
	if(skb->len > 500)
		txCBPtr->BdAddr->length_status |= 1;
	else
		txCBPtr->BdAddr->length_status |= 10;
    /*
     * This tells the switch that it can transmit this frame.
     */
    txCBPtr->BdAddr->length_status |= DMA_OWN | DMA_SOP | DMA_EOP | DMA_TX_APPEND_CRC;

    /* Decrement total BD count */
    pDevCtrl->txFreeBds -= 1;
	if(p_index == pDevCtrl->nrTxBds - 1) {
		p_index = 0;
	}else {
		p_index += 1;
	}
	/* advance producer index.*/
	pDevCtrl->txDma->tDmaRings[DMA_RING_DESC_INDEX].tdma_producer_index = p_index;

    if ( pDevCtrl->txFreeBds == 0 ) {
        TRACE(("%s: bcmgenet_xmit no transmit queue space -- stopping queues\n", dev->name));
        netif_stop_queue(dev);
    }

    /* Enable DMA for this channel */
    pDevCtrl->txDma->tdma_ctrl |= DMA_EN;

    /* update stats */
    pDevCtrl->stats.tx_bytes += ((skb->len < ETH_ZLEN) ? ETH_ZLEN : skb->len);
    pDevCtrl->stats.tx_bytes += 4;
    pDevCtrl->stats.tx_packets++;

    dev->trans_start = jiffies;

    spin_unlock_irqrestore(&pDevCtrl->lock, flags);

    return 0;
	spin_unlock_irqrestore(&pDevCtrl->lock, flags);
	return 0;
}

/* NAPI polling method*/
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18)
static int bcmgenet_poll(struct napi_struct * napi, int budget)
{
	BcmEnet_devctrl *pDevCtrl = container_of(napi, struct BcmEnet_devctrl, napi);
	volatile intrl2Regs * intrl2 = pDevCtrl->intrl2_0;
	uint32 work_done;
	work_done = bcmgenet_desc_rx(pDevCtrl, budget);
	if(work_done < budget)
	{
		netif_rx_complete(napi);
		intrl2->cpu_mask_clear |= UMAC_IRQ_RXDMA_BDONE;
	}
	return work_done;
}	
#else
static int bcmgenet_poll(struct net_device * dev, int * budget)
{   
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	volatile intrl2Regs * intrl2 = pDevCtrl->intrl2_0;
    
    uint32 work_done;

    work_done = bcmgenet_desc_rx(pDevCtrl, *budget);
	if(work_done < *budget)
	{
		netif_rx_complete(dev);
		intrl2->cpu_mask_clear |= UMAC_IRQ_RXDMA_BDONE;
	}
	return work_done;
}
#endif

/*
 * Interrupt bottom half
 */
static void bcmgenet_irq_task(BcmEnet_devctrl *pDevCtrl)
{
	spin_lock_bh(&pDevCtrl->bh_lock);

	TRACE(("%s\n", __FUNCTION__));
	/* Cable plugged/unplugged event */
	if (pDevCtrl->irq_stat & UMAC_IRQ_PHY_DET_R) {
		pDevCtrl->irq_stat &= ~UMAC_IRQ_PHY_DET_R;
		printk(KERN_CRIT "Cable plugged in UMAC%d powering up\n", pDevCtrl->devnum);
		bcmgenet_power_up(pDevCtrl, BCMUMAC_POWER_CABLE_SENSE);
		/* restart auto-neg, program speed/duplex/pause info into umac */
		if (!(pDevCtrl->umac->cmd & CMD_AUTO_CONFIG)) {
			mii_setup(pDevCtrl->dev);
		}

	}else if (pDevCtrl->irq_stat & UMAC_IRQ_PHY_DET_F) {
		pDevCtrl->irq_stat &= ~UMAC_IRQ_PHY_DET_F;
		printk(KERN_CRIT "Cable unplugged in UMAC%d powering down\n", pDevCtrl->devnum);
		bcmgenet_power_down(pDevCtrl, BCMUMAC_POWER_CABLE_SENSE);
	}
	if (pDevCtrl->irq_stat & UMAC_IRQ_MPD_R) {
		pDevCtrl->irq_stat &= ~UMAC_IRQ_MPD_R;
		printk(KERN_CRIT "Magic packet detected, UMAC%d waking up\n", pDevCtrl->devnum);
		/* disable mpd interrupt */
		pDevCtrl->intrl2_0->cpu_mask_set |= UMAC_IRQ_MPD_R;
		/* disable CRC forward.*/
		pDevCtrl->umac->cmd &= ~CMD_CRC_FWD;
		bcmgenet_power_up(pDevCtrl, BCMUMAC_POWER_WOL_MAGIC);
		
	}else if (pDevCtrl->irq_stat & (UMAC_IRQ_HFB_SM | UMAC_IRQ_HFB_MM)) {
		pDevCtrl->irq_stat &= ~(UMAC_IRQ_HFB_SM | UMAC_IRQ_HFB_MM);
		printk(KERN_CRIT "ACPI pattern matched, UMAC%d waking up\n", pDevCtrl->devnum);
		/* disable HFB match interrupts */
		pDevCtrl->intrl2_0->cpu_mask_set |= (UMAC_IRQ_HFB_SM | UMAC_IRQ_HFB_MM);
		bcmgenet_power_up(pDevCtrl, BCMUMAC_POWER_WOL_ACPI);
	}

	/* Link UP/DOWN event */
	if(pDevCtrl->irq_stat & UMAC_IRQ_LINK_UP)
	{
		printk(KERN_CRIT "%s Link UP.\n", pDevCtrl->dev->name);
		/* Clear soft-copy of irq status*/
		pDevCtrl->irq_stat &= ~UMAC_IRQ_LINK_UP;
		//pDevCtrl->rxDma->cfg |= DMA_EN;
		
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
		//pDevCtrl->rxDma->cfg &= ~DMA_EN;
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
 * bcmgenet_net_isr: Handle various interrupts
 */
static irqreturn_t bcmgenet_isr0(int irq, void * dev_id, struct pt_regs * regs)
{
    BcmEnet_devctrl *pDevCtrl = dev_id;
	volatile intrl2Regs * intrl2 = pDevCtrl->intrl2_0;
	
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
		pDevCtrl->irq_stat &= ~UMAC_IRQ_DESC_THROT;
		TRACE(("%s: %d packets avaiable\n", __FUNCTION__, DmaDescThres));
		bcmgenet_desc_rx(pDevCtrl, DmaDescThres);
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

		schedule_work(&pDevCtrl->bcmgenet_irq_work);
	}

    return IRQ_HANDLED;
}
/*
 *  bcmgenet_desc_rx - descriptor based rx process.
 *  this could be called from bottom half, or from NAPI polling method.
 */
static uint32 bcmgenet_desc_rx(void *ptr, uint32 budget)
{
    BcmEnet_devctrl *pDevCtrl = ptr;
	Enet_CB * cb;
	struct sk_buff * skb;
    unsigned long dmaFlag;
    int len;
    uint32 rxpktprocessed = 0;
	uint32 rxpkttoprocess = 0;
	uint32 p_index = 0, c_index = 0;
	
	p_index = pDevCtrl->rxDma->rDmaRings[DMA_RING_DESC_INDEX].rdma_producer_index;
	c_index = pDevCtrl->rxDma->rDmaRings[DMA_RING_DESC_INDEX].rdma_consumer_index;

	if(p_index < c_index)
	{
		rxpkttoprocess = pDevCtrl->nrRxBds - c_index + p_index;
		TRACE(("RDMA wrapped, rxpkttoprocess=%d\n", rxpkttoprocess));
	}else {
		rxpkttoprocess = p_index - c_index;
		TRACE(("RDMA nowrapp, rxpkttoprocess=%d\n", rxpkttoprocess));
	}

	while((rxpktprocessed < rxpkttoprocess)
			&& (rxpktprocessed < budget) )
	{
    	dmaFlag = (pDevCtrl->rxBds[c_index].length_status & 0xffff);
        len = ((pDevCtrl->rxBds[c_index].length_status)>>16);

		if (dmaFlag & DMA_OWN) {
			break;
		}
		rxpktprocessed++;

		cb = &pDevCtrl->rxCbs[c_index];
		BUG_ON(cb->skb == NULL);
	
		if(c_index == pDevCtrl->nrRxBds-1 )
			c_index = 0;
		else
			c_index++;
		pDevCtrl->rxDma->rDmaRings[DMA_RING_DESC_INDEX].rdma_consumer_index = c_index;
		
		/* report errors */
        if (unlikely(dmaFlag & (DMA_RX_CRC_ERROR | DMA_RX_OV | DMA_RX_NO | DMA_RX_LG |DMA_RX_RXER))) {
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

			/* discard the packet and advance consumer index.*/
			dev_kfree_skb_any(cb->skb);
			continue;
		}/* error packet */

		if(pDevCtrl->rbuf->rbuf_ctrl & RBUF_64B_EN)
		{
			/* we have 64B rx status block enabled.*/
			if(pDevCtrl->rbuf->rbuf_chk_ctrl & RBUF_RXCHK_EN)
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
        printk("bcmgenet_desc_rx :");
        dumpHexData(skb->data, 32);
#endif

        /* Finish setting up the received SKB and send it to the kernel */
        skb->dev = pDevCtrl->dev;
        skb->protocol = eth_type_trans(skb, pDevCtrl->dev);
        pDevCtrl->stats.rx_packets++;
        pDevCtrl->stats.rx_bytes += len;
		if(dmaFlag & DMA_RX_MULT)
			pDevCtrl->stats.multicast++;

        /* Notify kernel */
#ifdef CONFIG_BCMUMAC_RX_DESC_THROTTLE
		netif_rx(skb);
#else
        netif_receive_skb(skb);
#endif
		cb->skb = NULL;
        TRACE(("pushed up to kernel\n"));
    }
#if 1
	/* Allocate new SKBs for the ring */
	assign_rx_buffers(pDevCtrl);
#endif

    return rxpktprocessed;
}


/*
 * assign_rx_buffers: 
 * Assign skb to RX DMA descriptor. 
 */
static int assign_rx_buffers(BcmEnet_devctrl *pDevCtrl)
{
    struct sk_buff *skb;
    uint16  bdsfilled=0;
	unsigned long flags;
	
	TRACE(("%s\n", __FUNCTION__));

    /*
     * This function may be called from irq bottom-half.
     */
#ifndef CONFIG_BCMUMAC_RX_DESC_THROTTLE
	(void)flags;
	spin_lock_bh(&pDevCtrl->bh_lock);
#else
	spin_lock_irqsave(&pDevCtrl->lock, flags);
#endif

    /* loop here for each buffer needing assign */
	while(pDevCtrl->rxBdAssignPtr->address == 0)
	{
		Enet_CB * cb = &pDevCtrl->rxCbs[pDevCtrl->rxBdAssignPtr - pDevCtrl->rxBds];
		skb = netdev_alloc_skb(pDevCtrl->dev, pDevCtrl->rxBufLen + SKB_ALIGNMENT );
		if(!skb) {
			printk(KERN_CRIT " failed to allocate skb for rx\n");
			break;
		}
        handleAlignment(pDevCtrl, skb);

        /* keep count of any BD's we refill */
        bdsfilled++;

		cb->skb = skb;
		cb->dma_addr = dma_map_single(&pDevCtrl->generic_dev, skb->data, pDevCtrl->rxBufLen, DMA_FROM_DEVICE);
        /* assign packet, prepare descriptor, and advance pointer */
        pDevCtrl->rxBdAssignPtr->address = cb->dma_addr;
        pDevCtrl->rxBdAssignPtr->length_status  = (pDevCtrl->rxBufLen<<16);

        /* turn on the newly assigned BD for DMA to use */
        if (pDevCtrl->rxBdAssignPtr == pDevCtrl->rxBds + pDevCtrl->nrRxBds - 1) {
            pDevCtrl->rxBdAssignPtr->length_status |= (DMA_OWN | DMA_WRAP);
            pDevCtrl->rxBdAssignPtr = pDevCtrl->rxBds ;
        }
        else {
            pDevCtrl->rxBdAssignPtr->length_status |= DMA_OWN;
            pDevCtrl->rxBdAssignPtr++;
        }
    }

	/* Enable rx DMA incase it was disabled due to running out of rx BD */
	pDevCtrl->rxDma->rdma_ctrl |= DMA_EN;

#ifndef CONFIG_BCMUMAC_RX_DESC_THROTTLE
	spin_unlock_bh(&pDevCtrl->bh_lock);
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
	intrl2 = pDevCtrl->intrl2_0;

    TRACE(("bcmgenet: init_umac "));

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
	/* check link status */
	schedule_work(&pDevCtrl->bcmgenet_mii_work);
    /* 
	 * init rx registers, enable ip header optimization.
	 */
    if (pDevCtrl->bIPHdrOptimize) {
        pDevCtrl->rbuf->rbuf_ctrl |= RBUF_ALIGN_2B ;
    }
	umac->mac_0 = dev->dev_addr[0] << 24 | dev->dev_addr[1] << 16 | dev->dev_addr[2] << 8 | dev->dev_addr[3];
	umac->mac_1 = dev->dev_addr[4] << 8 | dev->dev_addr[5];
	
#ifdef CONFIG_BCMUMAC_RX_CSUM
	/* Do this for little-endian mode */
	pDevCtrl->rbuf->rbuf_endian_ctrl &= ~RBUF_ENDIAN_NOSWAP;
	/* Enable/disable rx checksum in ethtool functions. */
#endif
#ifdef CONFIG_BCMUMAC_TX_CSUM
	/* do this for little-endian mode.*/
	pDevCtrl->rbuf->tbuf_endian_ctrl &= ~RBUF_ENDIAN_NOSWAP;
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
	if (pDevCtrl->phyType == BRCM_PHY_TYPE_INT ||
		pDevCtrl->phyType == BRCM_PHY_TYPE_EXT_MII ||
		pDevCtrl->phyType == BRCM_PHY_TYPE_EXT_GMII ) 
	{
		intrl2->cpu_mask_clear |= UMAC_IRQ_PHY_DET_R | UMAC_IRQ_PHY_DET_F;
		intrl2->cpu_mask_clear |= UMAC_IRQ_LINK_DOWN | UMAC_IRQ_LINK_UP ;
	}

	/* Enable rx/tx engine.*/

	TRACE(("done init umac\n"));

    return 0;

}

/*
 * init_edma: Initialize DMA control register
 */
static void init_edma(BcmEnet_devctrl *pDevCtrl)
{
#ifdef CONFIG_BCMUMAC_RX_DESC_THROTTLE
	int speeds[] = {10, 100, 1000, 2500};
	int speed_id = 1;
#endif
    TRACE(("bcmgenet: init_edma\n"));

	/* init rDma, disable it first while updating register */
	pDevCtrl->rxDma->rdma_ctrl = 0;
	pDevCtrl->rxDma->rdma_scb_burst_size = DMA_MAX_BURST_LENGTH;
	/* by default, enable ring 16 (descriptor based) */
	pDevCtrl->rxDma->rDmaRings[DMA_RING_DESC_INDEX].rdma_write_pointer = 0;
	pDevCtrl->rxDma->rDmaRings[DMA_RING_DESC_INDEX].rdma_producer_index = 0;
	pDevCtrl->rxDma->rDmaRings[DMA_RING_DESC_INDEX].rdma_consumer_index = 0;
	pDevCtrl->rxDma->rDmaRings[DMA_RING_DESC_INDEX].rdma_ring_buf_size = (TOTAL_DESC << DMA_RING_SIZE_SHIFT) | RX_BUF_LENGTH;
	pDevCtrl->rxDma->rDmaRings[DMA_RING_DESC_INDEX].rdma_start_addr = 0;
	pDevCtrl->rxDma->rDmaRings[DMA_RING_DESC_INDEX].rdma_end_addr = TOTAL_DESC;
	pDevCtrl->rxDma->rDmaRings[DMA_RING_DESC_INDEX].rdma_xon_xoff_threshold = (DMA_FC_THRESH_LO << DMA_XOFF_THRESHOLD_SHIFT) | DMA_FC_THRESH_HI;
	pDevCtrl->rxDma->rDmaRings[DMA_RING_DESC_INDEX].rdma_read_pointer = 0;

#ifdef  CONFIG_BCMUMAC_RX_DESC_THROTTLE
	/* 
	 * Use descriptor throttle, fire interrupt only when multiple packets are done!
	 */
	pDevCtrl->rxDma->rDmaRings[DMA_RING_DESC_INDEX].rdma_mbuf_done_threshold = DMA_DESC_THRES;
	/* 
	 * Enable push timer, that is, force the IRQ_DESC_THROT to fire when timeout
	 * occcred, to prevent system slow reponse when handling low throughput data.
	 */
	speed_id = (pDevCtrl->umac->cmd >> CMD_SPEED_SHIFT) & CMD_SPEED_MASK;
	pDevCtrl->rxDma->rdma_timeout[DMA_RING_DESC_INDEX] = (2*(DMA_DESC_THRES*ENET_MAX_MTU_SIZE)/speeds[speed_id]) & DMA_TIMEOUT_MASK;
#endif	/* CONFIG_BCMUMAC_RX_DESC_THROTTLE */


	/* Init tDma */
	pDevCtrl->txDma->tdma_ctrl = 0;
	pDevCtrl->txDma->tdma_scb_burst_size = DMA_MAX_BURST_LENGTH;
	/* by default, enable ring DMA_RING_DESC_INDEX (descriptor based) */
	pDevCtrl->txDma->tDmaRings[DMA_RING_DESC_INDEX].tdma_read_pointer = 0;
	pDevCtrl->txDma->tDmaRings[DMA_RING_DESC_INDEX].tdma_producer_index = 0;
	pDevCtrl->txDma->tDmaRings[DMA_RING_DESC_INDEX].tdma_consumer_index = 0;
	pDevCtrl->txDma->tDmaRings[DMA_RING_DESC_INDEX].tdma_ring_buf_size = (TOTAL_DESC << DMA_RING_SIZE_SHIFT) | RX_BUF_LENGTH;
	pDevCtrl->txDma->tDmaRings[DMA_RING_DESC_INDEX].tdma_start_addr = 0;
	pDevCtrl->txDma->tDmaRings[DMA_RING_DESC_INDEX].tdma_end_addr = TOTAL_DESC;
	pDevCtrl->txDma->tDmaRings[DMA_RING_DESC_INDEX].tdma_mbuf_done_threshold = 0;
	/* Disable rate control for now */
	pDevCtrl->txDma->tDmaRings[DMA_RING_DESC_INDEX].tdma_flow_period = 0;
	pDevCtrl->txDma->tDmaRings[DMA_RING_DESC_INDEX].tdma_write_pointer = 0;
	
}
int bcmgenet_init_ringbuf(struct net_device * dev, int index, int size, int buf_len, unsigned char * buf)
{

	return 0;
}
/*
 * bcmgenet_init_dev: initialize uniMac devie
 * allocate Tx/Rx buffer descriptors pool, Tx control block pool.
 */
static int bcmgenet_init_dev(BcmEnet_devctrl *pDevCtrl)
{
    int i;
    int nrCbs;
    void *p;
	DmaDesc * lastBd;

	TRACE(("%s\n", __FUNCTION__));
    /* setup buffer/pointer relationships here */
    pDevCtrl->nrTxBds = pDevCtrl->nrRxBds = TOTAL_DESC;
	/* Always use 2KB buffer for 7420*/
    pDevCtrl->rxBufLen = RX_BUF_LENGTH;

	/* register block locations */
	pDevCtrl->sys = (SysRegs *)(pDevCtrl->dev->base_addr);
	pDevCtrl->grb = (GrBridgeRegs *)(pDevCtrl->dev->base_addr + UMAC_GR_BRIDGE_REG_OFFSET);
	pDevCtrl->ext = (ExtRegs *)(pDevCtrl->dev->base_addr + UMAC_EXT_REG_OFFSET);
	pDevCtrl->intrl2_0 = (intrl2Regs *)(pDevCtrl->dev->base_addr + UMAC_INTRL2_0_REG_OFFSET);
	pDevCtrl->intrl2_1 = (intrl2Regs *)(pDevCtrl->dev->base_addr + UMAC_INTRL2_1_REG_OFFSET);
	pDevCtrl->rbuf = (rbufRegs *)(pDevCtrl->dev->base_addr + UMAC_RBUF_REG_OFFSET);
	pDevCtrl->umac = (uniMacRegs *)(pDevCtrl->dev->base_addr + UMAC_UMAC_REG_OFFSET);
	pDevCtrl->hfb = (unsigned long*)(pDevCtrl->dev->base_addr + UMAC_HFB_OFFSET);
	pDevCtrl->txDma = (tDmaRegs *)(pDevCtrl->dev->base_addr + UMAC_TDMA_REG_OFFSET + 2*TOTAL_DESC*sizeof(unsigned long));
	pDevCtrl->rxDma = (rDmaRegs *)(pDevCtrl->dev->base_addr + UMAC_RDMA_REG_OFFSET + 2*TOTAL_DESC*sizeof(unsigned long));

    pDevCtrl->rxBds = (DmaDesc *) (pDevCtrl->dev->base_addr + UMAC_RDMA_REG_OFFSET);
    pDevCtrl->txBds = (DmaDesc *) (pDevCtrl->dev->base_addr + UMAC_TDMA_REG_OFFSET);
	TRACE(("%s: rxbds=0x%08x txbds=0x%08x\n", __FUNCTION__, pDevCtrl->rxBds, pDevCtrl->txBds));
	
    /* alloc space for the tx control block pool */
    if (!(p = kmalloc(pDevCtrl->nrTxBds*sizeof(Enet_CB), GFP_KERNEL))) {
        return -ENOMEM;
    }
    memset(p, 0, nrCbs*sizeof(Enet_CB));
    pDevCtrl->txCbs = (Enet_CB *)p;

    /* initialize rx ring pointer variables. */
    pDevCtrl->rxBdAssignPtr = pDevCtrl->rxBds;
    
	if (!(p = kmalloc(pDevCtrl->nrRxBds*sizeof(Enet_CB), GFP_KERNEL))) {
        return -ENOMEM;
    }
    memset(p, 0, nrCbs*sizeof(Enet_CB));
    pDevCtrl->rxCbs = (Enet_CB *)p;

    /* init the receive buffer descriptor ring */
    for (i = 0; i < pDevCtrl->nrRxBds; i++)
    {
        (pDevCtrl->rxBds + i)->length_status = (pDevCtrl->rxBufLen<<16);
        (pDevCtrl->rxBds + i)->address = 0;
    }
	lastBd = pDevCtrl->rxBds + pDevCtrl->nrRxBds - 1;
    lastBd->length_status |= DMA_WRAP;

    /* clear the transmit buffer descriptors */
    for (i = 0; i < pDevCtrl->nrTxBds; i++)
    {
        (pDevCtrl->txBds + i)->length_status = 0<<16;
        (pDevCtrl->txBds + i)->address = 0;
    }
	lastBd = pDevCtrl->txBds + pDevCtrl->nrTxBds - 1;
    lastBd->length_status |= DMA_WRAP;
    pDevCtrl->txFreeBds = pDevCtrl->nrTxBds;

    /* fill receive buffers */
	if(assign_rx_buffers(pDevCtrl) == 0)
	{
		printk(KERN_ERR "can't assign rx buffers\n");
        kfree((void *)pDevCtrl->txCbs);
		return -ENOMEM;
	}

    /* init umac registers */
    if (init_umac(pDevCtrl)) {
        kfree((void *)pDevCtrl->txCbs);
        return -EFAULT;
    }
    
	/* init dma registers */
    init_edma(pDevCtrl);

	TRACE(("%s returned\n", __FUNCTION__));
    /* if we reach this point, we've init'ed successfully */
    return 0;
}

/* Uninitialize tx/rx buffer descriptor pools */
static int bcmgenet_uninit_dev(BcmEnet_devctrl *pDevCtrl)
{
    int i;

    if (pDevCtrl) {
        /* disable DMA */
		pDevCtrl->rxDma->rdma_ctrl = 0;
		pDevCtrl->txDma->tdma_ctrl = 0;

		for ( i = 0; i < pDevCtrl->nrTxBds; i++)
		{
			if(pDevCtrl->txCbs[i].skb != NULL) {
				dev_kfree_skb(pDevCtrl->txCbs[i].skb);
				pDevCtrl->txCbs[i].skb = NULL;
			}
		}
		for ( i = 0; i < pDevCtrl->nrRxBds; i++)
		{
			if(pDevCtrl->rxCbs[i].skb != NULL) {
				dev_kfree_skb(pDevCtrl->rxCbs[i].skb);
				pDevCtrl->rxCbs[i].skb = NULL;
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
        /* free the transmit control block pool */
        if (pDevCtrl->txCbs) {
            kfree(pDevCtrl->rxCbs);
            pDevCtrl->rxCbs = NULL;
        }
    }
   
    return 0;
}
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
/*
 * Program ACPI pattern into HFB. Return filter index if succesful.
 * if user == 1, the data will be copied from user space.
 */
static int bcmgenet_update_hfb(struct net_device *dev, uint32 *data, int len, int user)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	volatile rbufRegs * rbuf = pDevCtrl->rbuf;
	int filter, offset, count;
	uint32 * tmp;
	
	TRACE(("Updating HFB len=0x%d\n", len));
	if(rbuf->rbuf_hfb_ctrl & RBUF_HFB_256B) {
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
		if(!((rbuf->rbuf_hfb_ctrl >> (filter + RBUF_HFB_FILTER_EN_SHIFT)) & 0X1))
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
	rbuf->rbuf_fltr_len[3-(filter>>2)] |= (len*2 << (RBUF_FLTR_LEN_SHIFT * (filter&0x03)) );
	
	/*enable this filter.*/
	rbuf->rbuf_hfb_ctrl |= (1 << (RBUF_HFB_FILTER_EN_SHIFT + filter));

	return filter;
	
}
/*
 * read ACPI pattern data for a particular filter.
 */
static int bcmgenet_read_hfb(struct net_device * dev, struct acpi_data * u_data)
{
	int filter, offset, count, len;
	BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	volatile rbufRegs *rbuf = pDevCtrl->rbuf;

	if(get_user(filter, &(u_data->fltr_index)) ) {
		printk(KERN_ERR "Failed to get user data\n");
		return -EFAULT;
	}
	
	if(rbuf->rbuf_hfb_ctrl & RBUF_HFB_256B) {
		count = 8;
		offset = 256;
	}else {
		count = 16;
		offset = 128;
	}
	if (filter > count)
		return -EINVAL;
	
	/* see if this filter is enabled, if not, return length 0 */
	if ((rbuf->rbuf_hfb_ctrl & (1 << (filter + RBUF_HFB_FILTER_EN_SHIFT)) ) == 0) {
		len = 0;
		put_user(len , &u_data->count);
		return 0;
	}
	/* check the filter length, in bytes */
	len = RBUF_FLTR_LEN_MASK & (rbuf->rbuf_fltr_len[filter>>2] >> (RBUF_FLTR_LEN_SHIFT * (filter & 0x3)) );
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
static inline void bcmgenet_clear_hfb(BcmEnet_devctrl * pDevCtrl, int filter)
{
	int offset;

	if(pDevCtrl->rbuf->rbuf_hfb_ctrl & RBUF_HFB_256B) {
		offset = 256;
	}else {
		offset = 128;
	}
	if (filter == CLEAR_ALL_HFB)
	{
		pDevCtrl->rbuf->rbuf_hfb_ctrl &= ~(0xffff << (RBUF_HFB_FILTER_EN_SHIFT));
		pDevCtrl->rbuf->rbuf_hfb_ctrl &= ~RBUF_HFB_EN;
	}else 
	{
		/* disable this filter */
		pDevCtrl->rbuf->rbuf_hfb_ctrl &= ~(1 << (RBUF_HFB_FILTER_EN_SHIFT + filter));
		/* clear filter length register */
		pDevCtrl->rbuf->rbuf_fltr_len[3-(filter>>2)] &= ~(0xff << (RBUF_FLTR_LEN_SHIFT * (filter & 0x03)) );
	}
	
}
/* 
 * Utility function to get interface ip address in kernel space.
 */
static inline uint32 bcmgenet_getip(struct net_device * dev)
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
static void bcmgenet_get_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
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
static int bcmgenet_set_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	BcmEnet_devctrl * pDevCtrl = netdev_priv(dev);
	volatile uniMacRegs * umac = pDevCtrl->umac;
	uint32 ip;

	if(wol->wolopts & ~(WAKE_MAGIC | WAKE_MAGICSECURE))
		return -EINVAL;

	if((ip = bcmgenet_getip(dev)) == 0)
	{
		printk("IP address is not set, can't put in WoL mode\n");
		return -EINVAL;
	}else {
		hfb_arp[HFB_ARP_LEN-2] |= (ip >> 16);
		hfb_arp[HFB_ARP_LEN-1] |= (ip & 0xFFFF);
		/* Enable HFB, to response to ARP request.*/
		if(bcmgenet_update_hfb(dev, hfb_arp, HFB_ARP_LEN, 0) < 0)
		{
			printk(KERN_ERR "%s: Unable to update HFB\n", __FUNCTION__);
			return -EFAULT;
		} 
		pDevCtrl->rbuf->rbuf_hfb_ctrl |= RBUF_HFB_EN;
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
		bcmgenet_power_down(pDevCtrl, BCMUMAC_POWER_WOL_MAGIC);
	}
	return 0;
}
/*
 * ethtool function - get generic settings.
 */
static int bcmgenet_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	BcmEnet_devctrl * pDevCtrl = netdev_priv(dev);
	return mii_ethtool_gset(&pDevCtrl->mii, cmd);
}
/*
 * ethtool function - set settings.
 */
static int bcmgenet_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	int err = 0;
	BcmEnet_devctrl * pDevCtrl = netdev_priv(dev);
//	volatile uniMacRegs * umac = pDevCtrl->umac;

	if((err = mii_ethtool_sset(&pDevCtrl->mii, cmd)) < 0)
		return err;
	mii_setup(dev);
	
	if(cmd->maxrxpkt != 0)
		DmaDescThres = cmd->maxrxpkt;

	return err;
}
/*
 * ethtool function - get driver info.
 */
static void bcmgenet_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	strncpy(info->driver, CARDNAME, sizeof(info->driver));
	strncpy(info->version, VER_STR, sizeof(info->version));
	
}
static u32 bcmgenet_get_rx_csum(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	if(pDevCtrl->rbuf->rbuf_chk_ctrl & RBUF_RXCHK_EN)
		return 1;
	
	return 0;
}
static int bcmgenet_set_rx_csum(struct net_device * dev, u32 val)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	spin_lock_bh(&pDevCtrl->bh_lock);
	if( val == 0)
	{
		pDevCtrl->rbuf->rbuf_ctrl &= ~RBUF_64B_EN;
		pDevCtrl->rbuf->rbuf_chk_ctrl &= ~RBUF_RXCHK_EN;
	}else {
		pDevCtrl->rbuf->rbuf_ctrl |= RBUF_64B_EN;
		pDevCtrl->rbuf->rbuf_chk_ctrl |= RBUF_RXCHK_EN ;
	}
	spin_unlock_bh(&pDevCtrl->bh_lock);
	return 0;
}
static u32 bcmgenet_get_tx_csum(struct net_device * dev)
{
	if(dev->features & NETIF_F_IP_CSUM)
		return 1;
	return 0;
}
static int bcmgenet_set_tx_csum(struct net_device * dev, u32 val)
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
static struct ethtool_ops bcmgenet_ethtool_ops = {
	.get_settings		= bcmgenet_get_settings,
	.set_settings		= bcmgenet_set_settings,
	.get_drvinfo		= bcmgenet_get_drvinfo,
	.get_wol			= bcmgenet_get_wol,
	.set_wol			= bcmgenet_set_wol,
	.get_rx_csum		= bcmgenet_get_rx_csum,
	.set_rx_csum		= bcmgenet_set_rx_csum,
	.get_tx_csum		= bcmgenet_get_tx_csum,
	.set_tx_csum		= bcmgenet_set_tx_csum,
	.get_link			= ethtool_op_get_link,
};
/*
 * disable clocks according to mode, cable sense/WoL
 */
static void bcmgenet_disable_clocks(BcmEnet_devctrl * pDevCtrl, int mode)
{
	int  pm_ctrl;
	if(pDevCtrl->phyType == BRCM_PHY_TYPE_MOCA)
		pm_ctrl = BCHP_CLK_MOCA_CLK_PM_CTRL;
	else
		pm_ctrl = BCHP_CLK_GENET_CLK_PM_CTRL;
	
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
			if(pm_ctrl == BCHP_CLK_MOCA_CLK_PM_CTRL) {
				BDEV_SET(BCHP_CLK_MISC, BCHP_CLK_MISC_MOCA_ENET_CLK_SEL_MASK);
			}
			break;
		default:
			break;
	}
	
}
/*
 * Enable clocks.
 */
static void bcmgenet_enable_clocks(BcmEnet_devctrl * pDevCtrl, int mode)
{
	int  pm_ctrl;
	if(pDevCtrl->phyType == BRCM_PHY_TYPE_MOCA)
		pm_ctrl = BCHP_CLK_GENET_CLK_PM_CTRL;
	else
		pm_ctrl = BCHP_CLK_MOCA_CLK_PM_CTRL;

	TRACE(("Enabling clocks...\n"));
	/* Enable all clocks */
	BDEV_WR(pm_ctrl, 0);
	BDEV_RD(pm_ctrl);
	/* 
	 * Save some power by disabling RGMII tx clock and 
	 * MoCA Tx/Rx clocks (if it's not MoCA and GPHY).
	 */
	if(pDevCtrl->phyType == BRCM_PHY_TYPE_INT)
	{
		BDEV_UNSET(BCHP_CLK_MISC, BCHP_CLK_MISC_GENET_CLK_SEL_MASK);
		BDEV_SET(BCHP_CLK_MISC, BCHP_CLK_MISC_GENET_GMII_TX_CLK_SEL_MASK);
		BDEV_RD(pm_ctrl);
	}
	if(mode == BCMUMAC_POWER_WOL_ACPI && pDevCtrl->phyType == BRCM_PHY_TYPE_MOCA)
	{
		BDEV_UNSET(BCHP_CLK_MISC, BCHP_CLK_MISC_MOCA_ENET_CLK_SEL_MASK);

	}
}
/*
 * Power down the unimac, based on mode.
 */
static void bcmgenet_power_down(BcmEnet_devctrl *pDevCtrl, int mode)
{
	int retries = 0;

	switch(mode) {
		case BCMUMAC_POWER_CABLE_SENSE:
			/* PHY bug , disble DLL only for now */
			pDevCtrl->ext->ext_pwr_mgmt |= EXT_PWR_DOWN_DLL;
			//	EXT_PWR_DOWN_PHY ;
			pDevCtrl->rbuf->rgmii_oob_ctrl &= ~RGMII_MODE_EN;
			bcmgenet_disable_clocks(pDevCtrl, mode);
			break;
		case BCMUMAC_POWER_WOL_MAGIC:
			/* ENable CRC forward */
			pDevCtrl->umac->cmd |= CMD_CRC_FWD;
			pDevCtrl->umac->mpd_ctrl |= MPD_EN;
			while(!(pDevCtrl->rbuf->rbuf_status & RBUF_STATUS_WOL)) {
				retries++;
				if(retries > 5) {
					printk(KERN_CRIT "bcmumac_power_down polling wol mode timeout\n");
					pDevCtrl->umac->mpd_ctrl &= ~MPD_EN;
					return;
				}
				udelay(100);
			}
			/* Service Rx BD untill empty */
			bcmgenet_disable_clocks(pDevCtrl, mode);
			pDevCtrl->intrl2_0->cpu_mask_clear |= UMAC_IRQ_MPD_R;
			pDevCtrl->intrl2_0->cpu_mask_clear |= UMAC_IRQ_HFB_MM | UMAC_IRQ_HFB_SM;
			break;
		case BCMUMAC_POWER_WOL_ACPI:
			pDevCtrl->rbuf->rbuf_hfb_ctrl |= RBUF_ACPI_EN;
			while(!(pDevCtrl->rbuf->rbuf_status & RBUF_STATUS_WOL)) {
				retries++;
				if(retries > 5) {
					printk(KERN_CRIT "bcmumac_power_down polling wol mode timeout\n");
					pDevCtrl->rbuf->rbuf_hfb_ctrl &= ~RBUF_ACPI_EN;
					return;
				}
				udelay(100);
			}
			/* Service RX BD untill empty */
			bcmgenet_disable_clocks(pDevCtrl, mode);
			pDevCtrl->intrl2_0->cpu_mask_clear |= UMAC_IRQ_HFB_MM | UMAC_IRQ_HFB_SM;
			break;
		default:
			break;
	}
	
}
static void bcmgenet_power_up(BcmEnet_devctrl *pDevCtrl, int mode)
{
	switch(mode) {
		case BCMUMAC_POWER_CABLE_SENSE:
			/* 7420A0 bug, PHY_RESET doesn't work */
			//pDevCtrl->txrx_ctrl->ephy_pwr_mgmt |= PHY_RESET;
			BDEV_SET(0x46a414, 1);
			pDevCtrl->ext->ext_pwr_mgmt &= ~EXT_PWR_DOWN_DLL;
			pDevCtrl->ext->ext_pwr_mgmt &= ~EXT_PWR_DOWN_PHY;
			pDevCtrl->ext->ext_pwr_mgmt &= ~EXT_PWR_DOWN_BIAS;
			udelay(2);
			//pDevCtrl->txrx_ctrl->ephy_pwr_mgmt &= ~PHY_RESET;
			BDEV_UNSET(0x46a414, 1);
			udelay(150);
			bcmgenet_enable_clocks(pDevCtrl, mode);
			break;
		case BCMUMAC_POWER_WOL_MAGIC:
			pDevCtrl->umac->mpd_ctrl &= ~MPD_EN;
			/* 
			 * If ACPI is enabled at the same time, disable it, since 
			 * we have been waken up.
			 */
			if( !(pDevCtrl->rbuf->rbuf_hfb_ctrl & RBUF_ACPI_EN))
			{
				pDevCtrl->rbuf->rbuf_hfb_ctrl &= RBUF_ACPI_EN;
				bcmgenet_enable_clocks(pDevCtrl, BCMUMAC_POWER_WOL_ACPI);
				/* Stop monitoring ACPI interrupts */
				pDevCtrl->intrl2_0->cpu_mask_set |= (UMAC_IRQ_HFB_SM | UMAC_IRQ_HFB_MM);
			}
			bcmgenet_clear_hfb(pDevCtrl, CLEAR_ALL_HFB);
			bcmgenet_enable_clocks(pDevCtrl, mode);
			break;
		case BCMUMAC_POWER_WOL_ACPI:
			pDevCtrl->rbuf->rbuf_hfb_ctrl &= ~RBUF_ACPI_EN;
			/* 
			 * If Magic packet is enabled at the same time, disable it, 
			 */
			if(!(pDevCtrl->umac->mpd_ctrl & MPD_EN))
			{
				pDevCtrl->umac->mpd_ctrl &= ~MPD_EN;
				bcmgenet_enable_clocks(pDevCtrl, BCMUMAC_POWER_WOL_ACPI);
				/* stop monitoring magic packet interrupt and disable crc forward */
				pDevCtrl->intrl2_0->cpu_mask_set |= UMAC_IRQ_MPD_R;
				pDevCtrl->umac->cmd &= ~CMD_CRC_FWD;
			}
			bcmgenet_clear_hfb(pDevCtrl, CLEAR_ALL_HFB);
			bcmgenet_enable_clocks(pDevCtrl,mode); 
			break;
		default:
			break;
	}
}
/* 
 * ioctl handle special commands that are not present in ethtool.
 */
static int bcmgenet_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    unsigned long flags;
    struct acpi_data *u_data;
    int val = 0;

    /* we can add sub-command in ifr_data if we need to in the future */
    switch (cmd)
    {
	case SIOCSACPISET:
		spin_lock_irqsave(&pDevCtrl->lock, flags);
		bcmgenet_power_down(pDevCtrl, BCMUMAC_POWER_WOL_ACPI);
		spin_unlock_irqrestore(&pDevCtrl->lock, flags);
		break;
	case SIOCSACPICANCEL:
		spin_lock_irqsave(&pDevCtrl->lock, flags);
		bcmgenet_power_up(pDevCtrl, BCMUMAC_POWER_WOL_ACPI);
		spin_unlock_irqrestore(&pDevCtrl->lock, flags);
		break;
	case SIOCSPATTERN:
		u_data = (struct acpi_data *)rq->ifr_data;
		val =  bcmgenet_update_hfb(dev, (unsigned long *)u_data->p_data, u_data->count,1 );
		if(val >= 0)
			put_user(val, &u_data->fltr_index);
		break;
	case SIOCGPATTERN:
		u_data = (struct acpi_data *)rq->ifr_data;
		val = bcmgenet_read_hfb(dev, u_data);
		break;
	default:
		val = -EINVAL;
		break;
    }

    return val;       
}
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18)
static const struct net_device_ops bcmgenet_netdev_ops = {
	.ndo_open = bcmgenet_open;
	.ndo_stop = bcmgenet_close;
	.ndo_start_xmit = bcmgenet_xmit;
	.ndo_tx_timeout = bcmgenet_tx_timeout;
	.ndo_set_multicast_list = bcmgenet_set_multicast_list;
	.ndo_set_mac_address = bcmgenet_set_mac_addr;
	.ndo_do_ioctl = bcmgenet_ioctl;
};
#endif
static int bcmgenet_drv_probe(struct platform_device *pdev)
{
	struct resource *mres, *ires;
	void __iomem *base;
	unsigned long res_size;
	int err;
	struct bcmumac_platform_data *cfg = pdev->dev.platform_data;
	BcmEnet_devctrl *pDevCtrl;
	struct net_device *dev;

	mres = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ires = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if(! mres || ! ires) {
		printk(KERN_ERR "%s: can't get resources\n", __FUNCTION__);
		return(-EIO);
	}
	res_size = mres->end - mres->start + 1;
	if(!request_mem_region(mres->start, res_size, CARDNAME))
	{
		printk(KERN_ERR "%s: can't request mem region: start: %lu size: %lu\n", 
				CARDNAME, mres->start, res_size);
		return -ENODEV;
	}
	base = ioremap(mres->start, mres->end - mres->start + 1);
	TRACE(("%s: base=0x%x\n", __FUNCTION__, base));

	if(! base) {
		printk(KERN_ERR "%s: can't ioremap\n", __FUNCTION__);
		return(-EIO);
	}

	dev = alloc_etherdev(sizeof(*pDevCtrl));
	if(dev == NULL)
	{
		printk(KERN_ERR "bcmgenet: can't allocate net device\n");
		err = -ENOMEM;
		goto err0;
	}
	dev->base_addr = (unsigned long)base;
	pDevCtrl = (BcmEnet_devctrl *)netdev_priv(dev);
	dev_set_drvdata(&pdev->dev, pDevCtrl);
	memcpy(dev->dev_addr, cfg->macaddr, 6);
	dev->irq = pDevCtrl->irq0;
	dev->watchdog_timeo         = 2*HZ;
	SET_ETHTOOL_OPS(dev, &bcmgenet_ethtool_ops);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18)
	dev->netdev_ops = &bcmgenet_netdev_ops;
	netif_napi_add(dev, &pDevCtrl->napi, bcmgenet_poll, 64);
#else
	dev->open                   = bcmgenet_open;
	dev->stop                   = bcmgenet_close;
	dev->hard_start_xmit        = bcmgenet_xmit;
	dev->tx_timeout             = bcmgenet_timeout;
	dev->get_stats				= bcmgenet_get_stats;
	dev->set_mac_address        = bcmgenet_set_mac_addr;
	dev->set_multicast_list     = bcmgenet_set_multicast_list;
	dev->do_ioctl               = bcmgenet_ioctl;
	dev->poll                   = bcmgenet_poll;
	dev->weight                 = 64;
#endif
	
#ifdef CONFIG_BCMUMAC_TX_CSUM
	//dev->features				|= NETIF_F_IP_CSUM;
#endif

	// Let boot setup info override default settings.
	netdev_boot_setup_check(dev);

	memcpy(&pDevCtrl->generic_dev, &pdev->dev, sizeof(struct device));
	pDevCtrl->dev = dev;
	pDevCtrl->irq0 = platform_get_irq(pdev, 0);
	pDevCtrl->irq1 = platform_get_irq(pdev, 1);
	pDevCtrl->devnum = pdev->id;
	pDevCtrl->phyAddr = cfg->phy_id;
	pDevCtrl->bIPHdrOptimize = 1;
	pDevCtrl->bh_lock = SPIN_LOCK_UNLOCKED;
	
	spin_lock_init(&pDevCtrl->lock);
	spin_lock_init(&pDevCtrl->bh_lock);
	mutex_init(&pDevCtrl->mdio_mutex);
	init_waitqueue_head(&pDevCtrl->wq);
	mii_init(dev);

	if(mii_probe(dev, cfg->phy_id) < 0)
	{
		printk(KERN_ERR "No PHY detected, not registering interface:%d\n", pdev->id);
		goto err1;
	}
	INIT_WORK(&pDevCtrl->bcmgenet_irq_work, (void (*)(void *))bcmgenet_irq_task, pDevCtrl);
	INIT_WORK(&pDevCtrl->bcmgenet_mii_work, (void (*)(void *))mii_setup, dev);

	if(bcmgenet_init_dev(pDevCtrl) < 0)
	{
		goto err1;
	}
	if(request_irq(pDevCtrl->irq0, bcmgenet_isr0, SA_INTERRUPT|SA_SHIRQ, dev->name, pDevCtrl) < 0)
	{
		printk(KERN_ERR "can't request IRQ %d\n", pDevCtrl->irq0);
		goto err2;
	}
#if 0
	if (request_irq(pDevCtrl->irq1, bcmgenet_ring_isr, SA_INTERRUPT|SA_SHIRQ, dev->name, pDevCtrl) < 0)
	{
		printk(KERN_ERR "can't request IRQ %d\n", pDevCtrl->irq1);
		free_irq(pDevCtrl->irq0, pDevCtrl);
		goto err2;
	}
#endif
	if((err = register_netdev(dev)) != 0)
	{
		goto err2;
	}

	return(0);

err2:
	bcmgenet_uninit_dev(pDevCtrl);
err1:
	iounmap(base);
	free_netdev(dev);
err0:
	release_mem_region(mres->start, res_size);
	return(err);
}

static int bcmgenet_drv_remove(struct platform_device *pdev)
{
	BcmEnet_devctrl *pDevCtrl = dev_get_drvdata(&pdev->dev);

	unregister_netdev(pDevCtrl->dev);
	free_irq(pDevCtrl->irq0, pDevCtrl);
	//free_irq(pDevCtrl->irq1, pDevCtrl);
	bcmgenet_uninit_dev(pDevCtrl);
	iounmap((void __iomem *)pDevCtrl->base_addr);
	free_netdev(pDevCtrl->dev);
	/*TODO : release mem region*/
	return(0);
}

static struct platform_driver bcmgenet_plat_drv = {
	.probe =		bcmgenet_drv_probe,
	.remove =		bcmgenet_drv_remove,
	.driver = {
		.name =		"bcmgenet",
		.owner =	THIS_MODULE,
	},
};

static int bcmgenet_module_init(void)
{
	platform_driver_register(&bcmgenet_plat_drv);
	return(0);
}

static void bcmgenet_module_cleanup(void)
{
	platform_driver_unregister(&bcmgenet_plat_drv);
}

module_init(bcmgenet_module_init);
module_exit(bcmgenet_module_cleanup);

MODULE_LICENSE("GPL");
EXPORT_SYMBOL(bcmemac_get_device);
