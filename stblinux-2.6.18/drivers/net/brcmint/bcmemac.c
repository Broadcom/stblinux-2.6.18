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
//**************************************************************************
// File Name  : bcmemac.c
//
// Description: This is Linux network driver for the BCM 
// 				EMAC Internal Ethenet Controller onboard the 7110
//               
// Updates    : 09/26/2001  jefchiao.  Created.
//**************************************************************************

#define CARDNAME    "BCM7110EMAC"
#define VERSION     "0.1"
#define VER_STR     "v" VERSION " " __DATE__ " " __TIME__

#define MULTICAST_HW_FILTER

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
#include <linux/in.h>
#if LINUX_VERSION_CODE >= 0x020411      /* 2.4.17 */
#include <linux/slab.h>
#else
#include <linux/malloc.h>
#endif
#include <linux/string.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/errno.h>
#include <linux/delay.h>

#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>

#include <asm/mipsregs.h>

#if defined( CONFIG_BCMINTEMAC ) || defined( CONFIG_BCMINTEMAC_MODULE )
#include <asm/brcmstb/brcm97110/bcm97110.h>
#include <asm/brcmstb/brcm97110/bcmintrnum.h>
// already in bcm97110.h #include <asm/brcm/board.h>
#include <intemac_map.h>


#else
#include <board.h>
#include <6345_common.h>
#include <6345_map.h>
#include <6345_intr.h>
#endif

#include <if_net.h>

//#define DUMP_TRACE
//#define DUMP_DATA

#include "bcmemac.h"

#include <linux/stddef.h>

#ifdef USE_PROC
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#endif


//extern int kerSysNvRamGet(char*, int, int);

extern void bcm_inv_rac_all(void);
extern unsigned long getPhysFlashBase(void);

static int bcmemac_enet_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
int bcmIsEnetUp(uint32 uPhyAddr);

#define TIMEOUT_10MS        (HZ/100)

/* Board defines */
#define BCM9634X_BOARD_TYPE_MASK   0xE0
#define BCM96345SV_BOARD_TYPE      0xC0
#define BCM96345R00_BOARD_TYPE     0xE0
#define BCM96345I_BOARD_TYPE       0xA0

/* MII Control */
#define EPHY_LOOPBACK                   0x4000
#define EPHY_AUTONEGOTIATE              0x1000
#define EPHY_100MBIT                    0x2000
#define EPHY_FULLDUPLEX                 0x0100
#define EPHY_RESTART_AUTONEGOTIATION    0x0200
/* MII STATUS */
#define EPHY_AUTONEGOTIATION_COMPLETE   0x0020
/* MII Auxiliary Mode 2 */
#define EPHY_ACT_LINK_LED               0x0004

typedef enum {
    MII_EXTERNAL = 0,
    MII_INTERNAL = 1,
}   MII_PHYADDR;

typedef enum {
    MII_100MBIT     = 0x0001,
    MII_FULLDUPLEX  = 0x0002,
    MII_AUTONEG     = 0x0004,
}   MII_CONFIG;

#define MAKE4(x)   ((x[3] & 0xFF) | ((x[2] & 0xFF) << 8) | ((x[1] & 0xFF) << 16) | ((x[0] & 0xFF) << 24))
#define MAKE2(x)   ((x[1] & 0xFF) | ((x[0] & 0xFF) << 8))

#if LINUX_VERSION_CODE >= 0x020405      /* starting from 2.4.5 */
#define skb_dataref(x)   (&skb_shinfo(x)->dataref)
#else
#define skb_dataref(x)   skb_datarefp(x)
#endif
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
//      External, indirect entry points. 
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
//      Called for "ifconfig ethX up" & "down"
// --------------------------------------------------------------------------
static int bcmemac_net_open(struct net_device * dev);
static int bcmemac_net_close(struct net_device * dev);
// --------------------------------------------------------------------------
//      Watchdog timeout
// --------------------------------------------------------------------------
static void bcmemac_net_timeout(struct net_device * dev);
// --------------------------------------------------------------------------
//      Packet transmission. 
// --------------------------------------------------------------------------
static int bcmemac_net_xmit(struct sk_buff * skb, struct net_device * dev);
// --------------------------------------------------------------------------
//      Allow proc filesystem to query driver statistics
// --------------------------------------------------------------------------
static struct net_device_stats * bcmemac_net_query(struct net_device * dev);
// --------------------------------------------------------------------------
//      Set address filtering mode
// --------------------------------------------------------------------------
static void bcm_set_multicast_list(struct net_device * dev);
// --------------------------------------------------------------------------
//      Set the hardware MAC address.
// --------------------------------------------------------------------------
static int bcm_set_mac_addr(struct net_device *dev, void *p);

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
//      Interrupt routines
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
static void bcmemac_net_isr(int irq, void *, struct pt_regs *regs);
// --------------------------------------------------------------------------
//  Bottom half service routine. Process all received packets.                  
// --------------------------------------------------------------------------
static void bcmemac_rx(void *);

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
//      Internal routines
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
/* Add an address to the ARL register */
static void write_mac_address(struct net_device *dev);
/* Remove registered netdevice */
static void bcmemac_init_cleanup(struct net_device *dev);
/* Allocate and initialize tx/rx buffer descriptor pools */
static int bcmemac_init_dev(BcmEnet_devctrl *pDevCtrl);
/* Assign the Rx descriptor ring */
static int assign_rx_buffers(BcmEnet_devctrl *pDevCtrl);
/* Initialize driver's pools of receive buffers and tranmit headers */
static int init_buffers(BcmEnet_devctrl *pDevCtrl);
/* Initialise the Ethernet Switch control registers */
static void init_emac(BcmEnet_devctrl *pDevCtrl);
/* Initialize the Ethernet Switch MIB registers */
static void clear_mib(void);
/* Write a value to the MII */
static void mii_write(uint32 uPhyAddr, uint32 uRegAddr, uint32 data);
/* Read a value from the MII */
static uint32 mii_read(uint32 uPhyAddr, uint32 uRegAddr);
/* Reset the MII */
static void soft_reset(uint32 uPhyAddr);
/* Return the current MII configuration */
static MII_CONFIG mii_GetConfig(BcmEnet_devctrl *pDevCtrl);
/* Auto-Configure this MII interface */
static MII_CONFIG mii_AutoConfigure(BcmEnet_devctrl *pDevCtrl);
#ifdef PHY_LOOPBACK
/* Set the MII loopback mode */
static void mii_Loopback(BcmEnet_devctrl *pDevCtrl);
#endif
/* update an address to the EMAC perfect match registers */
static void perfectmatch_update(BcmEnet_devctrl *pDevCtrl, const uint8 * pAddr, bool bValid);

#ifdef MULTICAST_HW_FILTER
/* clear perfect match registers except given pAddr (PR10861) */
static void perfectmatch_clean(BcmEnet_devctrl *pDevCtrl, const uint8 * pAddr);
#endif

/* write an address to the EMAC perfect match registers */
static void perfectmatch_write(int reg, const uint8 * pAddr, bool bValid);
/* Initialize DMA control register */
static void init_dma(BcmEnet_devctrl *pDevCtrl);
/* Reclaim transmit frames which have been sent out */
static void tx_reclaim_timer(unsigned long arg);
/* Add a Tx control block to the pool */
static void txCb_enq(BcmEnet_devctrl *pDevCtrl, Enet_Tx_CB *txCbPtr);
/* Remove a Tx control block from the pool*/
static Enet_Tx_CB *txCb_deq(BcmEnet_devctrl *pDevCtrl);

#ifdef DUMP_DATA
/* Display hex base data */
static void dumpHexData(unsigned char *head, int len);
/* dumpMem32 dump out the number of 32 bit hex data  */
static void dumpMem32(uint32 * pMemAddr, int iNumWords);
#endif

static BcmEnet_devctrl *pDevContext = NULL;

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

} //dumpHexData

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

/* For debugging */
static int gTimerScheduled;

/* --------------------------------------------------------------------------
    Name: bcmemac_net_open
 Purpose: Open and Initialize the EMAC on the chip
-------------------------------------------------------------------------- */
static int bcmemac_net_open(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    int semCount;
    unsigned long flags;


    ASSERT(pDevCtrl != NULL);

    TRACE(("%s: bcmemac_net_open, EMACConf=%08x, &RxDMA=%x, rxDma.cfg=%08x\n",
    dev->name, (unsigned int) EMAC->config, (unsigned int) &pDevCtrl->rxDma, (unsigned int) pDevCtrl->rxDma->cfg));

    MOD_INC_USE_COUNT;

    spin_lock_irqsave(&pDevCtrl->lock, flags);

    
    /* disable ethernet MAC while updating its registers */
    EMAC->config = EMAC_DISABLE ;
    while(EMAC->config & EMAC_DISABLE);

    EMAC->config |= EMAC_ENABLE;

    /* Initialize emac registers */
    EMAC->rxControl = EMAC_FC_EN | EMAC_PROM;   // allow Promiscuous

    pDevCtrl->rxDma->cfg |= DMA_CHAINING|DMA_WRAP_EN|DMA_FLOWC_EN;
    pDevCtrl->rxDma->cfg |= DMA_ENABLE;


    // Tell Tx DMA engine where to restart, this is important cuz otherwise it will try to pick up
    // where it was left off, causing a 2 min delay in tx
    pDevCtrl->txDma->startAddr = (uint32)PHYSADDR((uint32 *)pDevCtrl->txFirstBdPtr);
    pDevCtrl->txDma->length = (UINT32)pDevCtrl->nrTxBds;
	
    pDevCtrl->txDma->intStat = DMA_DONE|DMA_NO_DESC|DMA_BUFF_DONE;
    pDevCtrl->txDma->intMask = 0;
    pDevCtrl->txDma->cfg =  DMA_CHAINING | DMA_WRAP_EN/* |DMA_FLOWC_EN*/;
    pDevCtrl->txDma->cfg |= DMA_ENABLE;

    /* THT Modified to only do it on first open, I just added the spinlock guard, but would like to keep the original
     * semaphore intact.
      */

    semCount = atomic_inc_return(&pDevContext->devInUsed);
    if (semCount < 1) {
	printk(KERN_NOTICE "Warning: bcmemac_net_open: Unbalanced net_close calls\n");
	atomic_set(&pDevContext->devInUsed, 1);
	/* Fall Thru */
    }



    if (1 == semCount) {
	// Not used
	// BcmHalInterruptEnable(BCM_LINUX_EMAC_IRQ);
	// Already enabled in irq.c?  This is a no-op for the superchip bcm7110
       BcmHalInterruptEnable(pDevCtrl->rxIrq);

    	pDevCtrl->timer.expires = jiffies + TIMEOUT_10MS;
    	add_timer(&pDevCtrl->timer);
	gTimerScheduled = 1;
    }
    else { /* (semCount > 1) , Called more than once */
		printk(KERN_NOTICE "bcmemac_net_open already called\n");
    }
 

    // Start the network engine
    TRACE(("bcmemac_net_open: Calling netif_start_queue semCount=%d\n", semCount));
    netif_start_queue(dev);

    spin_unlock_irqrestore(&pDevCtrl->lock, flags);
	
    TRACE(("Exiting bcmemac_net_open, semCount=%d\n", semCount));

    return 0;
}

/* --------------------------------------------------------------------------
    Name: bcmemac_net_close
 Purpose: Stop communicating with the outside world
    Note: Caused by 'ifconfig ethX down'
-------------------------------------------------------------------------- */
static int bcmemac_net_close(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    Enet_Tx_CB *txCBPtr;
    int semCount;
    unsigned long flags;

    ASSERT(pDevCtrl != NULL);

    TRACE(("%s: bcmemac_net_close\n", dev->name));

    
    MOD_DEC_USE_COUNT;

    spin_lock_irqsave(&pDevCtrl->lock, flags);

    /*
     * Regardless of how many times this is opened, close it on first call
     * This code was previously written with no spinlock hold, but I just wanna keep it
     */
    if (0 <= (semCount = atomic_dec_return(&pDevContext->devInUsed))) {
	TRACE(("bcmemac_net_close netif_stop_queue\n"));
	netif_stop_queue(dev);

      pDevCtrl->rxDma->cfg &= ~DMA_ENABLE;
      /* Wait till idle */
      while(pDevCtrl->rxDma->cfg & DMA_ENABLE)
	  	pDevCtrl->rxDma->cfg &= ~DMA_ENABLE;
    	
       // THis is a no-op for the Superchip on Linux
    	BcmHalInterruptDisable(pDevCtrl->rxIrq);
	//BcmHalInterruptDisable(BCM_LINUX_EMAC_IRQ);
    
	gTimerScheduled = 0;
    	del_timer_sync(&pDevCtrl->timer);


	    /* free the skb in the txCbPtrHead */
        while (pDevCtrl->txCbPtrHead)  {
	        pDevCtrl->txFreeBds += pDevCtrl->txCbPtrHead->nrBds;

	        dev_kfree_skb (pDevCtrl->txCbPtrHead->skb);

	        txCBPtr = pDevCtrl->txCbPtrHead;

	        /* Advance the current reclaim pointer */
	        pDevCtrl->txCbPtrHead = pDevCtrl->txCbPtrHead->next;

	        /* Finally, return the transmit header to the free list */
	        txCb_enq(pDevCtrl, txCBPtr);
	 }
	
        /* Adjust the tail if the list is empty */
        if(pDevCtrl->txCbPtrHead == NULL) {
          pDevCtrl->txCbPtrTail = NULL;
        }

        pDevCtrl->txNextBdPtr = pDevCtrl->txFirstBdPtr = pDevCtrl->txBds;

        // THT Turn it off
        pDevCtrl->linkState = 0;
	 atomic_set(&pDevContext->devInUsed, 0);
      }
      else if (semCount < 0) {
          printk(KERN_NOTICE "bcmemac_net_close: Negative semCount %d\n", semCount);
	   atomic_set(&pDevContext->devInUsed, 0);

      }

      spin_unlock_irqrestore(&pDevCtrl->lock, flags);

     TRACE(("Exiting bcmemac_net_close, semCount=%d\n", semCount));

    return 0;
}

/* --------------------------------------------------------------------------
    Name: bcmemac_net_timeout
 Purpose: 
-------------------------------------------------------------------------- */
static void bcmemac_net_timeout(struct net_device * dev)
{
    ASSERT(dev != NULL);

    TRACE(("%s: bcmemac_net_timeout\n", dev->name));

    dev->trans_start = jiffies;

    netif_wake_queue(dev);
}

/* --------------------------------------------------------------------------
    Name: bcm_set_multicast_list
 Purpose: Set the multicast mode, ie. promiscuous or multicast
-------------------------------------------------------------------------- */
static void bcm_set_multicast_list(struct net_device * dev)
{
    ASSERT(dev != NULL);

    TRACE(("%s: bcm_set_multicast_list: %08X\n", dev->name, dev->flags));

    /* Promiscous mode */
    if (dev->flags & IFF_PROMISC) {
        EMAC->rxControl |= EMAC_PROM;   
    } else {
        EMAC->rxControl &= ~EMAC_PROM;   
    }

#ifndef MULTICAST_HW_FILTER
    /* All Multicast packets (PR10861 Check for any multicast request) */
    if (dev->flags & IFF_ALLMULTI || dev->mc_count) {
        EMAC->rxControl |= EMAC_ALL_MCAST;
    } else {
        EMAC->rxControl &= ~EMAC_ALL_MCAST;
    }
#else
	{
    struct dev_mc_list *dmi = dev->mc_list;
	/* PR10861 - Filter specific group Multicast packets (R&C 2nd Ed., p463) */
    if (dev->flags & IFF_ALLMULTI  || dev->mc_count>(MAX_PMADDR-1)) {
		perfectmatch_clean((BcmEnet_devctrl *)dev->priv, dev->dev_addr);
		EMAC->rxControl |= EMAC_ALL_MCAST;
		return;
    } else {
        EMAC->rxControl &= ~EMAC_ALL_MCAST;
    }

	/* No multicast? Just get our own stuff */
	if (dev->mc_count == 0) return;

	/* Store multicast addresses in the prefect match registers */
	perfectmatch_clean((BcmEnet_devctrl *)dev->priv, dev->dev_addr);
	for(dmi = dev->mc_list;	dmi ; dmi = dmi->next)
		perfectmatch_update((BcmEnet_devctrl *)dev->priv, dmi->dmi_addr, 1);
	}
#endif
}

/* --------------------------------------------------------------------------
    Name: bcmemac_net_query
 Purpose: Return the current statistics. This may be called with the card 
          open or closed.
-------------------------------------------------------------------------- */
static struct net_device_stats *
bcmemac_net_query(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;

    TRACE(("%s: bcmemac_net_query\n", dev->name));

    ASSERT(pDevCtrl != NULL);

    return &(pDevCtrl->stats);
}

/*
 * tx_reclaim_timer: reclaim transmit frames which have been sent out
 */
static void tx_reclaim_timer(unsigned long arg)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)arg;
    int bdfilled;
    int linkState;
    static int counter = 0;

    gTimerScheduled = 0;
    if (atomic_read(&pDevCtrl->rxDmaRefill) != 0) {
        atomic_set(&pDevCtrl->rxDmaRefill, 0);
        /* assign packet buffers to all available Dma descriptors */
        bdfilled = assign_rx_buffers(pDevCtrl);
        if (bdfilled)
            pDevCtrl->rxDma->fcBufAlloc = bdfilled;
    }

    /* Reclaim transmit Frames which have been sent out */
    bcmemac_net_xmit(NULL, pDevCtrl->dev);

    if ( counter++ >= HZ/TIMEOUT_10MS ) {
        counter = 0;
        linkState = bcmIsEnetUp(pDevCtrl->phyAddr);
        if (linkState != pDevCtrl->linkState) {
            pDevCtrl->linkState = linkState;
            if ( linkState )
                printk((KERN_CRIT "%s Link UP.\n"),pDevCtrl->dev->name);
            else
                printk((KERN_CRIT "%s Link DOWN.\n"),pDevCtrl->dev->name);
        }
    }

    pDevCtrl->timer.expires = jiffies + TIMEOUT_10MS;

    // If we are going down, no need to reschedule it
    if (0 < atomic_read(&pDevCtrl->devInUsed )|| pDevCtrl->linkState) {
	gTimerScheduled = 1;
    	add_timer(&pDevCtrl->timer);
    }
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
    Name: bcmemac_net_xmit
 Purpose: Send ethernet traffic
-------------------------------------------------------------------------- */
static int bcmemac_net_xmit(struct sk_buff * skb, struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    Enet_Tx_CB *txCBPtr;
    Enet_Tx_CB *txedCBPtr;
    volatile DmaDesc *firstBdPtr;
    unsigned long flags;

    ASSERT(pDevCtrl != NULL);

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */
    spin_lock_irqsave(&pDevCtrl->lock, flags);
        
    txCBPtr = NULL;
    //TRACE((">>>>>>>>>>>>>bcmemac_net_xmit: skb=%08x: \n", (unsigned int) skb));


    /* Reclaim transmitted buffers */
    while (pDevCtrl->txCbPtrHead)  {
    	unsigned short txStatus = EMAC_SWAP16(pDevCtrl->txCbPtrHead->lastBdAddr->status);
       TRACE(("bcmemac_net_xmit: reclaim: txStatus = %08x\n", txStatus));

        /* We have caught up with the tx queue */
        if (txStatus & DMA_OWN) {
            break;
        }

	 // If there was an error, update MIB statics.
       if (txStatus & EMAC_UNDERRUN)
       {
         	//EthMib.StandardHalMib.ifOutErrors++;
            pDevCtrl->stats.tx_dropped++;
            pDevCtrl->stats.tx_errors++;
       }

        pDevCtrl->txFreeBds += pDevCtrl->txCbPtrHead->nrBds;

        dev_kfree_skb (pDevCtrl->txCbPtrHead->skb);

        txedCBPtr = pDevCtrl->txCbPtrHead;

        /* Advance the current reclaim pointer */
        pDevCtrl->txCbPtrHead = pDevCtrl->txCbPtrHead->next;

        /* 
         * Finally, return the transmit header to the free list.
         * But keep one around, so we don't have to allocate again
         * for this tx request
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
        TRACE(("@"));
        if (txCBPtr != NULL) {
            txCb_enq(pDevCtrl, txCBPtr);
            netif_wake_queue(dev);
        }
        spin_unlock_irqrestore(&pDevCtrl->lock, flags);
        return 0;
    }


 TRACE(("bcmemac_net_xmit, txCfg=%08lx, txIntStat=%08lx\n", 
 	pDevCtrl->txDma->cfg, pDevCtrl->txDma->intStat));
    if (txCBPtr == NULL) {
        txCBPtr = txCb_deq(pDevCtrl);
    }

    /*
     * Obtain a transmit header from the free list.  If there are none
     * available, we can't send the packet. Discard the packet.
     */
    if (txCBPtr == NULL) {
        TRACE(("%s: bcmemac_net_xmit: txCbPtr == NULL, dev->flags=%08x, state=%08x\n", 
			dev->name, dev->flags, dev->state));
        netif_stop_queue(dev);
        spin_unlock_irqrestore(&pDevCtrl->lock, flags);
        return 1;
    }

    txCBPtr->nrBds = 1;
    txCBPtr->skb = skb;

    /* If we could not queue this packet, free it */
    if (pDevCtrl->txFreeBds < txCBPtr->nrBds) {
        TRACE(("%s: bcmemac_net_xmit low on txFreeBds\n", dev->name));
        txCb_enq(pDevCtrl, txCBPtr);
        netif_stop_queue(dev);
        spin_unlock_irqrestore(&pDevCtrl->lock, flags);
        return 1;
    }


    firstBdPtr = pDevCtrl->txNextBdPtr;
    /* store off the last BD address used to enqueue the packet */
    txCBPtr->lastBdAddr = pDevCtrl->txNextBdPtr;

    /* assign skb data to TX Dma descriptor */
    /*
     * Add the buffer to the ring.
     * Set addr and length of DMA BD to be transmitted.
     */
    dma_cache_wback_inv((unsigned long)skb->data, skb->len);


/* Debug */
#if 0
    {
	struct timeval tv;
	int notTxed = 0;
	Enet_Tx_CB* txNode;

	do_gettimeofday(&tv);
	TRACE(("++++++++++++++++ Time= %u.%u\n", tv.tv_sec, tv.tv_usec));
	// Traverse the tx queue to see how many have not been sent.

	for (txNode = pDevCtrl->txCbPtrHead; txNode != NULL; txNode = txNode->next) {
		unsigned int status = EMAC_SWAP16(txNode->lastBdAddr->status);
		if (status & DMA_OWN) {	
			notTxed++;
		}
	}
	TRACE(("Tx queue length = %d vs %d not sent\n", 
		pDevCtrl->nrTxBds - pDevCtrl->txFreeBds, notTxed));
    }
#endif
/* End Debug */
	
    TRACE(("Tx: skb->addr=%08x, len=%d, tx.cfg=%08x, intrMask=%08x, intrStat=%08x\n",
		skb->data, skb->len, pDevCtrl->txDma->cfg, 
		pDevCtrl->txDma->intMask, pDevCtrl->txDma->intStat));

    pDevCtrl->txNextBdPtr->address = EMAC_SWAP32((uint32)PHYSADDR(skb->data));
    //pDevCtrl->txNextBdPtr->length  = EMAC_SWAP16(skb->len);
    pDevCtrl->txNextBdPtr->length  = EMAC_SWAP16((skb->len < ETH_ZLEN) ? ETH_ZLEN : skb->len);

    /*
     * Set status of DMA BD to be transmitted and
     * advance BD pointer to next in the chain.
     */
    
    if (pDevCtrl->txNextBdPtr == pDevCtrl->txLastBdPtr) {
        pDevCtrl->txNextBdPtr->status = EMAC_SWAP16(DMA_WRAP);
        pDevCtrl->txNextBdPtr = pDevCtrl->txFirstBdPtr;
    }
    else {
        pDevCtrl->txNextBdPtr->status = EMAC_SWAP16(0);
        pDevCtrl->txNextBdPtr++;
    }


#ifdef DUMP_DATA
    TRACE(("bcmemac_net_xmit: len %d", skb->len));
    dumpHexData(skb->data, 32);
#endif
    /*
     * Turn on the "OWN" bit in the first buffer descriptor
     * This tells the switch that it can transmit this frame.
     */

    firstBdPtr->status |= EMAC_SWAP16(DMA_SOP | DMA_EOP | DMA_APPEND_CRC);
    /* Do this separately to avoid race condition */
    firstBdPtr->status |= EMAC_SWAP16(DMA_OWN);

    /* Enable DMA for this channel */
    pDevCtrl->txDma->cfg |= DMA_ENABLE;

    /* Decrement total BD count */
    pDevCtrl->txFreeBds -= txCBPtr->nrBds;

    if ( (pDevCtrl->txFreeBds == 0) || (pDevCtrl->txCbQHead == NULL) ) {
        TRACE(("%s: bcm6352_net_xmit no transmit queue space -- stopping queues\n", dev->name));
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

    /* update stats */
    pDevCtrl->stats.tx_bytes += ((skb->len < ETH_ZLEN) ? ETH_ZLEN : skb->len);
    pDevCtrl->stats.tx_bytes += 4;
    pDevCtrl->stats.tx_packets++;

    dev->trans_start = jiffies;

    spin_unlock_irqrestore(&pDevCtrl->lock, flags);

    return 0;
}

/*
 * Atlanta PR5760 fix:
 *               // PR5760 - If there are no more packets available, and the last
 *               // one we saw had an overflow, we need to assume that the EMAC
 *               // has wedged and reset it.  This is the workaround for a
 *               // variant of this problem, where the MAC can stop sending us
 *               // packets altogether (the "silent wedge" condition).
 */
static int gNumberOfOverflows = 0;
static int gNoDescCount = 0;
static  int loopCount = 0;  // This should be local to the rx routine, but we need it here so that 
					    // dump_emac() can display it.  It is not really a global var.
static atomic_t resetting_EMAC = ATOMIC_INIT(0);

// PR5760 - if there are too many packets with the overflow bit set,
// then reset the EMAC.  We arrived at a threshold of 8 packets based
// on empirical analysis and testing (smaller values are too aggressive
// and larger values wait too long).
#define OVERFLOW_THRESHOLD 8
#define NODESC_THRESHOLD 20
#define RX_ETH_DATA_BUFF_SIZE       ENET_MAX_MTU_SIZE


/* These should be part of pDevCtrl, as they are not reset */
static int gNumResetsErrors = 0;
static int gNumNoDescErrors = 0;
static int gNumOverflowErrors = 0;
/* For debugging */
static unsigned int gLastErrorDmaFlag, gLastDmaFlag;


void
ResetEMAConErrors(void)
{
 	if (1 == atomic_add_return(1, &resetting_EMAC)) {
         	// Clear the overflow counter.
        	gNumberOfOverflows = 0;
		gNumResetsErrors++;
        	// Set the disable bit, wait for h/w to clear it, then set
        	// the enable bit.
        	EMAC->config |= EMAC_DISABLE;
        	while (EMAC->config & EMAC_DISABLE);
        	EMAC->config |= EMAC_ENABLE;   
 	}
	// Otherwise another thread is resetting it.
	else {
		printk(KERN_NOTICE "ResetEMAConErrors: Another thread is resetting the EMAC, count=%d\n", 
			atomic_read(&resetting_EMAC));
	}
	atomic_dec(&resetting_EMAC);
}


/*
 * bcmemac_net_isr: Acknowledge interrupts and check if any packets have
 *                  arrived
 */
static void bcmemac_net_isr(int irq, void * dev_id, struct pt_regs * regs)
{
    BcmEnet_devctrl *pDevCtrl = dev_id;
    uint32 rxEvents;

    /* get Rx interrupt events */
    rxEvents = pDevCtrl->rxDma->intStat;

    TRACE((">>>>>>>>>>>>>>>>>>> bcmemac_net_isr: intstat=%08lx\n",rxEvents));
    if (gNumberOfOverflows > 0)
        printk(KERN_CRIT "bcmemac_net_isr: intstat=%08lx, gNumberOfOverflow=%d\n",
	    rxEvents, gNumberOfOverflows);

    if (rxEvents & DMA_BUFF_DONE)
    {
        pDevCtrl->rxDma->intStat = DMA_BUFF_DONE;  /* clr interrupt */
    }
    if (rxEvents & DMA_DONE)
    {
        pDevCtrl->rxDma->intStat = DMA_DONE;  // clr interrupt
#ifdef USE_BH       // USE_BH -- better for the over system performance
        // use bottom half (immediate queue).  Put the bottom half of interrupt in the tasklet
        queue_task(&pDevCtrl->task, &tq_immediate);
        mark_bh(IMMEDIATE_BH);
#else
        // no BH and handle every thing inside interrupt routine
        bcmemac_rx(pDevCtrl);
#endif // USE_BH
    }

	/* THT The else-clause here is important, since otherwise, we handle the DMA_NO_DESC
	  * inside bcmemac_rx
	  */
    else { 
	  if (rxEvents & DMA_NO_DESC) {

		if (gNoDescCount++ % 10 == 0) {
			printk(KERN_NOTICE "DMA_NO_DESC, count=%d\n", gNoDescCount);
		}
		gNumNoDescErrors++;

             //pDevCtrl->rxDma->intStat = DMA_NO_DESC;
	      pDevCtrl->rxDma->intStat |= DMA_NO_DESC;
             pDevCtrl->rxDma->cfg = DMA_ENABLE|DMA_CHAINING|DMA_WRAP_EN;
				
             //BcmHalInterruptEnable(pDevCtrl->rxIrq);
              if (gNumberOfOverflows > OVERFLOW_THRESHOLD || gNoDescCount > NODESC_THRESHOLD ) {

			int bdfilled;
			
			
			gNoDescCount = 0;

			bdfilled = assign_rx_buffers(pDevCtrl);
			printk(KERN_DEBUG "bcmemac_rx_isr: After assigning buffers, filled=%d\n", bdfilled);
			if (bdfilled == 0) {
				/* For now, until we figure out why we can't reclaim the buffers */
				printk(KERN_CRIT "Running out of buffers, all busy\n");
			}
			ResetEMAConErrors();
              }
              //return; /* THT for sr5p1: Been seeing this a lot on B0 chips */
	  }
    }

    TRACE(("<<<<<<<<<<<<<<<<<<<<<<< bcmemac_net_isr.\n"));
    return;
}


 

/*
 *  bcmemac_rx: Process all received packets.
 */
static void bcmemac_rx(void *ptr)
{
    BcmEnet_devctrl *pDevCtrl = ptr;
    struct sk_buff *skb;
    struct net_device *dev;
    uint16 dmaFlag;
    unsigned char *pBuf;
    int len;
    int bdfilled;
    unsigned int packetLength = 0;

    dmaFlag = EMAC_SWAP16(pDevCtrl->rxBdReadPtr->status);
 
    gLastDmaFlag = dmaFlag;

    loopCount = 0;
	
    /* Loop here for each received packet */
    // THT : New scheme now, check for error first
    // old scheme:  while(!(dmaFlag & DMA_OWN) && (dmaFlag & DMA_EOP))
    // TO do: Handle incomplete packets.
    while(1)
    {
        if (++loopCount > MAX_RX_BUFS) {
			printk(KERN_CRIT "************** Error: Looping %d times, dmaFlag=%x\n",  loopCount, dmaFlag);
        }
	 // Stop when we hit a buffer with no data, or a BD w/ no buffer.
        // This implies that we caught up with DMA, or that we haven't been
        // able to fill the buffers.
        if ((dmaFlag & DMA_OWN) || (pDevCtrl->rxBdReadPtr->address == (uint32) NULL))
        {
            // PR5760 - If there are no more packets available, and the last
            // one we saw had an overflow, we need to assume that the EMAC
            // has wedged and reset it.  This is the workaround for a
            // variant of this problem, where the MAC can stop sending us
            // packets altogether (the "silent wedge" condition).
            if (gNumberOfOverflows > 0)
            {
                printk(KERN_CRIT "Handling last packet overflow, resetting EMAC->config, ovf=%d\n", gNumberOfOverflows);
                ResetEMAConErrors();
            }

            break;
        }


        if ((dmaFlag & (DMA_EOP | EMAC_CRC_ERROR | EMAC_OV | EMAC_NO | EMAC_LG  | EMAC_RXER)) != DMA_EOP) {
	     /*
	       * Handle errors, and incomplete packets
	       */
            pDevCtrl->stats.rx_dropped++;
            pDevCtrl->stats.rx_errors++;

/* Debug */
gLastErrorDmaFlag = dmaFlag;
            

	     /* THT Starting with 7110B0 (Atlanta's PR5760), look for resetting the 
              * EMAC on overflow condition
              */
	     while ((dmaFlag & DMA_OWN) == 0) {
		 uint32 bufferAddress;
		 
		 // PR5760 - keep track of the number of overflow packets.
               if (dmaFlag & EMAC_OV)
               {
                  	gNumberOfOverflows++;
			        gNumOverflowErrors++;
               }
			   
	        // Keep a running total of the packet length.
               packetLength += EMAC_SWAP16(pDevCtrl->rxBdReadPtr->length);



		 // Pull the buffer from the BD and clear the BD so that it can be
               // reassigned later.
               bufferAddress = (uint32) EMAC_SWAP32(pDevCtrl->rxBdReadPtr->address);
               pDevCtrl->rxBdReadPtr->address = 0;

		 pBuf = (unsigned char *)KSEG0ADDR(bufferAddress);
		 skb = (struct sk_buff *)*(unsigned long *)(pBuf-4);
		 atomic_set(&skb->users, 1);
 

	        /* Old error handling */
               bdfilled = assign_rx_buffers(pDevCtrl);
               if (bdfilled)
                   pDevCtrl->rxDma->fcBufAlloc = bdfilled;

		/* Advance BD ptr to next in ring */
              IncRxBDptr(pDevCtrl->rxBdReadPtr, pDevCtrl);
	
        	// If this is the last buffer in the packet, stop.
        	if (dmaFlag & DMA_EOP)
        	{
        		packetLength = 0;
            		break;
        	}

              //Read more until EOP or good packet
              dmaFlag = EMAC_SWAP16(pDevCtrl->rxBdReadPtr->status);
gLastDmaFlag = dmaFlag;
	     }

	     // Clear the EMAC block receive logic for oversized packets.  On
    	     // a really, really long packet (often caused by duplex
            // mismatches), the EMAC_LG bit may not be set, so I need to
            // check for this condition separately.
            if ((packetLength >= 2048) ||

            	// PR5760 - if there are too many packets with the overflow bit set,
        	// then reset the EMAC.  We arrived at a threshold of 8 packets based
        	// on empirical analysis and testing (smaller values are too aggressive
        	// and larger values wait too long).
        	(gNumberOfOverflows > OVERFLOW_THRESHOLD))
    	    {
  		  ResetEMAConErrors();
    	     }

    	     // If the DMA stalled because there were no descriptors, then start it back
            // up.
            if (pDevCtrl->rxDma->intStat & DMA_NO_DESC)
            {
                pDevCtrl->rxDma->intMask = 0; /* THT */
        	  pDevCtrl->rxDma->intStat |= DMA_NO_DESC;
                pDevCtrl->rxDma->cfg = DMA_ENABLE|DMA_CHAINING|DMA_WRAP_EN|DMA_FLOWC_EN;

    		  /* THT: set to interrupt on packet complete and no descriptor available */
    		  pDevCtrl->rxDma->intMask = DMA_DONE|DMA_NO_DESC;

                //EthMib.RxDmaOverflowCnt++;
		  gNoDescCount++;
		  gNumNoDescErrors++;
             }

           // PR5577 - for some reason, we can wind up getting cases where we need to
	    // kick the DMA, but the DMA_NO_DESC flag is not set.  This shows up
	    // primarily when our EMAC is connected to another EMAC via MII (like on
	    // RG platforms where there is an ethernet switch).  We have seen cases
	    // where lots of overflows can wind up making the DMA controller shut down,
	    // so that nothing goes between the two EMACs.  We'll go ahead and kick
	    // this here to un-wedge the MACs.
	    else
	    {
	        pDevCtrl->rxDma->cfg = DMA_ENABLE|DMA_CHAINING|DMA_WRAP_EN|DMA_FLOWC_EN;
		 pDevCtrl->rxDma->intMask = DMA_DONE|DMA_NO_DESC;	
	    }

	     
	     //Read new status at head of chain, before looping back
            dmaFlag = EMAC_SWAP16(pDevCtrl->rxBdReadPtr->status);
gLastDmaFlag = dmaFlag;
            continue;
        }/* if error packet */

        /*
          * THT: Get here only when we have received a good and complete packet
          */

//	if (gNumberOfOverflows > 0 || gNoDescCount > 0) {
 //       printk("bcmemac_rx: Received good packet dmaFlag=%08lx, gNumberOfOverflow=%d, noDesc=%d\n",
//	    dmaFlag, gNumberOfOverflows, gNoDescCount);
//    }
	 // PR5760 - clear the overflow counter since we just received a
        // valid packet.  The EMAC must be working properly if we can
        // receive valid packets.
        gNumberOfOverflows = 0;
	 gNoDescCount = 0;
	 pBuf = (unsigned char *)(KSEG0ADDR(EMAC_SWAP32(pDevCtrl->rxBdReadPtr->address)));
TRACE(("bcmemac_rx: pBuf=%8p\n", pBuf));

#if defined( CONFIG_BCMINTEMAC ) || defined( CONFIG_BCMINTEMAC_MODULE )
	/*
	  * THT: Invalidate the RAC cache again, since someone may have read near the vicinity
	  * of the buffer.  This is necessary because the RAC cache is much larger than the CPU cache
	  */
	bcm_inv_rac_all();
#endif

       len = EMAC_SWAP16(pDevCtrl->rxBdReadPtr->length);
        /* Null the BD field to prevent reuse */
        pDevCtrl->rxBdReadPtr->status = EMAC_SWAP16(0);
        pDevCtrl->rxBdReadPtr->address = EMAC_SWAP32(0);

        skb = (struct sk_buff *)*(unsigned long *)(pBuf-4);


	/* Advance BD ptr to next in ring */
        IncRxBDptr(pDevCtrl->rxBdReadPtr, pDevCtrl);  

        dmaFlag = EMAC_SWAP16(pDevCtrl->rxBdReadPtr->status);
gLastDmaFlag = dmaFlag;

        /* remove Ethernet CRC */
        skb_trim(skb, len - 4);


        dev = pDevCtrl->dev;

#ifdef DUMP_DATA
        printk(KERN_DEBUG "bcmemac_rx :");
        dumpHexData(skb->data, 32);
#endif

        /* Finish setting up the received SKB and send it to the kernel */
        skb->dev = dev;

        /* Set the protocol type */
        skb->protocol = eth_type_trans(skb, dev);

        /* Allocate a new SKB for the ring */
        bdfilled = assign_rx_buffers(pDevCtrl);
        if (bdfilled)
            pDevCtrl->rxDma->fcBufAlloc = bdfilled;

        pDevCtrl->stats.rx_packets++;
        pDevCtrl->stats.rx_bytes += len;

        /* Notify kernel */
        netif_rx(skb);

        dev->last_rx = jiffies;

        continue;
    }

    /* Re-enable interrupts to resume packet reception */
    //BcmHalInterruptEnable(pDevCtrl->rxIrq);

    loopCount = -1; // tell dump_emac that we are outside RX
}


/*
 * Set the hardware MAC address.
 */
static int bcm_set_mac_addr(struct net_device *dev, void *p)
{
    struct sockaddr *addr=p;

    TRACE(("%s: bcm_set_mac_addr\n", dev->name));
    if(netif_running(dev)) {
	 TRACE(("%s:bcm_set_mac_addr: Cant set MAC addr when running\n", dev->name));
        return -EBUSY;
    }

    memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

    write_mac_address(dev);

    return 0;
}

/*
 * write_mac_address: store MAC address into EMAC perfect match registers                   
 */
void write_mac_address(struct net_device *dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    volatile uint32 data32bit;

    ASSERT(pDevCtrl != NULL);

    data32bit = EMAC->config;
    if (data32bit & EMAC_ENABLE) {
        /* disable ethernet MAC while updating its registers */
        EMAC->config &= ~EMAC_ENABLE ;           
        EMAC->config |= EMAC_DISABLE ;           
        while(EMAC->config & EMAC_DISABLE);     
    }

    /* add our MAC address to the perfect match register */
    perfectmatch_update((BcmEnet_devctrl *)dev->priv, dev->dev_addr, 1);

    if (data32bit & EMAC_ENABLE) {
        EMAC->config = data32bit;
    }
}


/*******************************************************************************
*
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
    skb->list = NULL;
    skb->sk = NULL;
    skb->stamp.tv_sec=0;    /* No idea about time */
    skb->dev = NULL;
    skb->dst = NULL;
    /* memset(skb->cb, 0, sizeof(skb->cb)); */
    skb->pkt_type = PACKET_HOST;    /* Default type */
    skb->ip_summed = 0;
    skb->priority = 0;
    skb->security = 0;  /* By default packets are insecure */
    skb->destructor = NULL;

#ifdef CONFIG_NETFILTER
    skb->nfmark = skb->nfcache = 0;
    skb->nfct = NULL;
#ifdef CONFIG_NETFILTER_DEBUG
    skb->nf_debug = 0;
#endif
#endif
#ifdef CONFIG_NET_SCHED
    skb->tc_index = 0;
#endif
}

/*
 * assign_rx_buffers: Beginning with the first receive buffer descriptor
 *                  used to receive a packet since this function last
 *                  assigned a buffer, reassign buffers to receive
 *                  buffer descriptors, and mark them as EMPTY; available
 *                  to be used again.
 *
 */
static int assign_rx_buffers(BcmEnet_devctrl *pDevCtrl)
{
    struct sk_buff *skb;
    uint16  bdsfilled=0;
    int devInUsed;
    int i;

    /*
     * check assign_rx_buffers is in process?
     * This function may be called from various context, like timer
     * or bcm6352_rx
     */
    if(test_and_set_bit(0, &pDevCtrl->rxbuf_assign_busy)) {
        return bdsfilled;
    }

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
                skb = pDevCtrl->skb_pool[pDevCtrl->nextskb];
                if (++pDevCtrl->nextskb == MAX_RX_BUFS)
                    pDevCtrl->nextskb = 0;
                if ((skb_shared(skb) == 0) && 
                    atomic_read(skb_dataref(skb)) <= 1) {
                    /* found a free skb */
                    break;
                }
            }
            if (i == MAX_RX_BUFS) {
                /* no free skb available now */
                /* rxDma is empty, set flag and let timer function to refill later */
                atomic_set(&pDevCtrl->rxDmaRefill, 1);
                break;
            }
        }

        skb_headerinit(skb, NULL, 0);  /* clean state */

        /* init the skb, in case other app. modified the skb pointer */
        skb->data = skb->tail = skb->head;
        skb->end = skb->data + (skb->truesize - sizeof(struct sk_buff));
        skb->len = 0;
        skb->data_len = 0;
        skb->cloned = 0;
        /* reserve space ditto __dev_alloc_skb */
        skb_reserve(skb,16);

        *(unsigned int *)skb->data = (unsigned int)skb;
        skb_reserve(skb, 4);
        skb_put(skb, pDevCtrl->rxBufLen);

        /*
         * Set the user count to two so when the upper layer frees the
         * socket buffer, only the user count is decremented.
         */
        atomic_inc(&skb->users);

        /* kept count of any BD's we refill */
        bdsfilled++;

        dma_cache_wback_inv((unsigned long)skb->data, pDevCtrl->rxBufLen);


        /* assign packet, prepare descriptor, and advance pointer */
        pDevCtrl->rxBdAssignPtr->address = EMAC_SWAP32((uint32)PHYSADDR(skb->data));
        pDevCtrl->rxBdAssignPtr->length  = EMAC_SWAP16(pDevCtrl->rxBufLen);

        /* turn on the newly assigned BD for DMA to use */
        if (pDevCtrl->rxBdAssignPtr == pDevCtrl->rxLastBdPtr) {
            pDevCtrl->rxBdAssignPtr->status = EMAC_SWAP16(DMA_OWN | DMA_WRAP);
            pDevCtrl->rxBdAssignPtr = pDevCtrl->rxFirstBdPtr;
        }
        else {
            pDevCtrl->rxBdAssignPtr->status = EMAC_SWAP16(DMA_OWN);
            pDevCtrl->rxBdAssignPtr++;
        }
    }

    /*
     * restart DMA in case the DMA ran out of descriptors
     */
    devInUsed = atomic_read(&pDevCtrl->devInUsed);
    if (devInUsed > 0) {
        pDevCtrl->rxDma->cfg |= DMA_ENABLE;
    }

    clear_bit(0, &pDevCtrl->rxbuf_assign_busy);

    return bdsfilled;
}

/*
 * perfectmatch_write: write an address to the EMAC perfect match registers
 */
static void perfectmatch_write(int reg, const uint8 * pAddr, bool bValid)
{
    volatile uint32 *pmDataLo;
    volatile uint32 *pmDataHi;
  
    switch (reg) {
    case 0:
        pmDataLo = &EMAC->pm0DataLo;
        pmDataHi = &EMAC->pm0DataHi;
        break;
    case 1:
        pmDataLo = &EMAC->pm1DataLo;
        pmDataHi = &EMAC->pm1DataHi;
        break;
    case 2:
        pmDataLo = &EMAC->pm2DataLo;
        pmDataHi = &EMAC->pm2DataHi;  		/* PR 10861 - fixed wrong value here */
        break;
    case 3:
        pmDataLo = &EMAC->pm3DataLo;
        pmDataHi = &EMAC->pm3DataHi;
        break;
    default:
        return;
    }
    /* Fill DataHi/Lo */
    if (bValid == 1)
        *pmDataLo = MAKE4((pAddr + 2));
    *pmDataHi = MAKE2(pAddr) | (bValid << 16);
}

/*
 * perfectmatch_update: update an address to the EMAC perfect match registers
 */
static void perfectmatch_update(BcmEnet_devctrl *pDevCtrl, const uint8 * pAddr, bool bValid)
{
    int i;
    unsigned int aged_ref;
    int aged_entry;


    /* check if this address is in used */
    for (i = 0; i < MAX_PMADDR; i++) {
        if (pDevCtrl->pmAddr[i].valid == 1) {
            if (memcmp (pDevCtrl->pmAddr[i].dAddr, pAddr, ETH_ALEN) == 0) {
                if (bValid == 0) {
                    /* clear the valid bit in PM register */
                    perfectmatch_write(i, pAddr, bValid);
                    /* clear the valid bit in pDevCtrl->pmAddr */
                    pDevCtrl->pmAddr[i].valid = bValid;
                } else {
                    pDevCtrl->pmAddr[i].ref++;
                }
                return;
            }
        }
    }
    if (bValid == 1) {
        for (i = 0; i < MAX_PMADDR; i++) {
            /* find an available entry for write pm address */
            if (pDevCtrl->pmAddr[i].valid == 0) {
                break;
            }
        }
        if (i == MAX_PMADDR) {
            aged_ref = 0xFFFFFFFF;
            aged_entry = 0;
            /* aged out an entry */
            for (i = 0; i < MAX_PMADDR; i++) {
                if (pDevCtrl->pmAddr[i].ref < aged_ref) {
                    aged_ref = pDevCtrl->pmAddr[i].ref;
                    aged_entry = i;
                }
            }
            i = aged_entry;
        }
        /* find a empty entry and add the address */
        perfectmatch_write(i, pAddr, bValid);

        /* update the pDevCtrl->pmAddr */
        pDevCtrl->pmAddr[i].valid = bValid;
        memcpy(pDevCtrl->pmAddr[i].dAddr, pAddr, ETH_ALEN);
        pDevCtrl->pmAddr[i].ref = 1;
    }
}

#ifdef MULTICAST_HW_FILTER
/*
 * perfectmatch_clean: Clear EMAC perfect match registers not matched by pAddr (PR10861)
 */
static void perfectmatch_clean(BcmEnet_devctrl *pDevCtrl, const uint8 * pAddr)
{
    int i;

    /* check if this address is in used */
    for (i = 0; i < MAX_PMADDR; i++) {
        if (pDevCtrl->pmAddr[i].valid == 1) {
            if (memcmp (pDevCtrl->pmAddr[i].dAddr, pAddr, ETH_ALEN) != 0) {
                     /* clear the valid bit in PM register */
                    perfectmatch_write(i, pAddr, 0);
                    /* clear the valid bit in pDevCtrl->pmAddr */
                    pDevCtrl->pmAddr[i].valid = 0;
            }
        }
    }
}
#endif

#ifdef PHY_LOOPBACK
/*
 * mii_Loopback: Set the MII loopback mode
 */
static void mii_Loopback(BcmEnet_devctrl *pDevCtrl)
{
    uint32 uData;

    TRACE(("mii_Loopback\n"));

    uData = mii_read(pDevCtrl->phyAddr, 0x00) & 0xffff;
    /* Disable autonegotiation */
    uData &= ~EPHY_AUTONEGOTIATE;
    /* Enable Loopback */
    uData |= EPHY_LOOPBACK;
    mii_write(pDevCtrl->phyAddr, 0x00, uData);
}
#endif

/* 
 * mii_GetConfig: Return the current MII configuration
 */
static MII_CONFIG mii_GetConfig(BcmEnet_devctrl *pDevCtrl)
{
    uint32 uData;
    MII_CONFIG eConfig = 0;

    TRACE(("mii_GetConfig\n"));

    /* Read the Link Partner Ability */
    uData = mii_read(pDevCtrl->phyAddr, 0x05) & 0x0000FFFF;
    if (uData & 0x0100) {           /* 100 MB Full-Duplex */
        eConfig = (MII_100MBIT | MII_FULLDUPLEX);
    } else if (uData & 0x0080) {    /* 100 MB Half-Duplex */
        eConfig = MII_100MBIT;
    } else if (uData & 0x0040) {    /* 10 MB Full-Duplex */
        eConfig = MII_FULLDUPLEX;
    } 

    return eConfig;
}

/*
 * mii_AutoConfigure: Auto-Configure this MII interface
 */
static MII_CONFIG mii_AutoConfigure(BcmEnet_devctrl *pDevCtrl)
{
    int i;
    uint32 uData;
    MII_CONFIG eConfig;

    TRACE(("mii_AutoConfigure\n"));

    /* enable and restart autonegotiation */
    uData = mii_read(pDevCtrl->phyAddr, 0) & 0x0000FFFF;
    uData |= (EPHY_RESTART_AUTONEGOTIATION | EPHY_AUTONEGOTIATE);
    mii_write(pDevCtrl->phyAddr, 0x00, uData);

    /* wait for it to finish */
    for (i = 0; i < 1000; i++) {
        mdelay(1);
        uData = mii_read(pDevCtrl->phyAddr, 0x01);
        if (uData & EPHY_AUTONEGOTIATION_COMPLETE) {
            break;
        }
    }

    eConfig = mii_GetConfig(pDevCtrl);
    if (uData & EPHY_AUTONEGOTIATION_COMPLETE) {
        eConfig |= MII_AUTONEG;
    } 
    mii_write(pDevCtrl->phyAddr, 0x1A, 0x0F00);

    return eConfig;
}

/*
 * mii_write: Write a value to the MII
 */
static void mii_write(uint32 uPhyAddr, uint32 uRegAddr, uint32 data)
{
    uint32 d;
    d = 0x50020000 | (uPhyAddr << 23) | (uRegAddr << 18) | data;
    EMAC->mdioData = d;
    mdelay(1);
    while ( ! (EMAC->intStatus & 0x1) );
    EMAC->intStatus = 0x1;
}

/*
 * mii_read: Read a value from the MII
 */
static uint32 mii_read(uint32 uPhyAddr, uint32 uRegAddr) 
{
    EMAC->mdioData = 0x60020000 | (uPhyAddr << 23) | (uRegAddr << 18);
    mdelay(1);
    while ( ! (EMAC->intStatus & 0x00000001) );
    EMAC->intStatus = 0x1;
    return EMAC->mdioData;
}

/*
 * soft_reset: Reset the MII
 */
static void soft_reset(uint32 uPhyAddr) 
{
    uint32 uData;

    mii_write(uPhyAddr, 0, 0x8000);
    // wait ~10usec
    udelay(10);
    do {
        uData = mii_read(uPhyAddr, 0);
    } while (uData & 0x00008000);

}

/*
 * clear_mib: Initialize the Ethernet Switch MIB registers
 */
static void clear_mib()
{
    int                   i;
    volatile uint32       *pt;

TRACE(("-->clear_mib\n"));

    pt = (uint32 *)&EMAC->tx_mib;
    for( i = 0; i < NumEmacTxMibVars; i++ ) {
        *pt++ = 0;
    }

    pt = (uint32 *)&EMAC->rx_mib;;
    for( i = 0; i < NumEmacRxMibVars; i++ ) {
        *pt++ = 0;
    }
TRACE(("<-- clear_mib\n"));
}

/*
 * init_emac: Initializes the Ethernet Switch control registers
 */
static void init_emac(BcmEnet_devctrl *pDevCtrl)
{
    MII_CONFIG eMiiConfig;

    TRACE(("bcmemacenet: init_emac\n"));

    /* Initialize the Ethernet Switch MIB registers */
    clear_mib();


   TRACE(("bcmemacenet: disable-enable\n"));
    /* disable ethernet MAC while updating its registers */
    EMAC->config = EMAC_DISABLE ;
    while(EMAC->config & EMAC_DISABLE);

    TRACE(("bcmemacenet: soft-reset\n"));
   
    /* issue soft reset, wait for it to complete */
    EMAC->config = EMAC_SOFT_RESET;
    while (EMAC->config & EMAC_SOFT_RESET);

    TRACE(("bcmemacenet: soft-reset done\n"));
    /* init mii clock, do soft reset of phy, default is 10Base-T */
    if (pDevCtrl->phyAddr == MII_EXTERNAL) {
        EMAC->config |= EMAC_EXT_PHY;
        EMAC->mdioFreq = EMAC_MII_PRE_EN | 0x105;
        EMAC->txControl = EMAC_FD;  /* FULL DUPLEX */
    } else {
        EMAC->mdioFreq = EMAC_MII_PRE_EN | 0x01F;
        soft_reset(pDevCtrl->phyAddr);

        eMiiConfig = mii_AutoConfigure(pDevCtrl);

        if (! (eMiiConfig & MII_AUTONEG)) {
                printk(KERN_INFO CARDNAME ": Auto-negotiation timed-out\n");
        }

        if (eMiiConfig & (MII_100MBIT | MII_FULLDUPLEX)) {
                printk(KERN_INFO CARDNAME ": 100 MB Full-Duplex (auto-neg)\n");
        } else if (eMiiConfig & MII_100MBIT) {
                printk(KERN_INFO CARDNAME ": 100 MB Half-Duplex (auto-neg)\n");
        } else if (eMiiConfig & MII_FULLDUPLEX) {
                printk(KERN_INFO CARDNAME ": 10 MB Full-Duplex (auto-neg)\n");
        } else {
                printk(KERN_INFO CARDNAME ": 10 MB Half-Duplex (assumed)\n");
        }

#ifdef PHY_LOOPBACK
        /* Enable PHY loopback */
        mii_Loopback(pDevCtrl);
#endif

        /* Check for FULL/HALF duplex */
        if (eMiiConfig & MII_FULLDUPLEX) {
            EMAC->txControl = EMAC_FD;  /* FULL DUPLEX */
        }
    }

    /* Initialize emac registers */
    EMAC->rxControl = EMAC_FC_EN | EMAC_PROM;   // allow Promiscuous

#ifdef MAC_LOOPBACK
    EMAC->rxControl |= EMAC_LOOPBACK;
#endif
    EMAC->rxMaxLength = ENET_MAX_MTU_SIZE;
    EMAC->txMaxLength = ENET_MAX_MTU_SIZE;

    /* tx threshold = abs(63-(0.63*EMAC_DMA_MAX_BURST_LENGTH)) */
    EMAC->txThreshold = 16;
    EMAC->mibControl  = 1;
    EMAC->intMask = 0;              /* mask all EMAC interrupts*/

}

/*
 * init_dma: Initialize DMA control register
 */
static void init_dma(BcmEnet_devctrl *pDevCtrl)
{
    TRACE(("bcmemacenet: init_dma\n"));

#ifdef CONFIG_CPU_LITTLE_ENDIAN
    {
	unsigned long* pDMA = (unsigned long*) DMA_ADR_BASE; /* 0xffe6000 */
	unsigned long* pByteRev = &pDMA[1]; /* 0xfffe6004 */
	//unsigned long* pHalfRev = &pDMA[2]; /* 0xfffe6008 */

	TRACE(("init_dma: Setting Reverse Byte Order DMA at %lx\n", (unsigned long) pByteRev));
	
	/* Turn on byte reverse for Channel 17(RX) and 18(TX) (bits 16,17)*/
	*pByteRev |= 0x00030000;	
    }

#endif

    // transmit
    pDevCtrl->txDma->cfg = 0;       /* initialize first (will enable later) */
    pDevCtrl->txDma->maxBurst = DMA_MAX_BURST_LENGTH;

    pDevCtrl->txDma->startAddr = (uint32)PHYSADDR((uint32 *)pDevCtrl->txFirstBdPtr);
    pDevCtrl->txDma->length = (UINT32)pDevCtrl->nrTxBds;
    pDevCtrl->txDma->fcThreshold = 0;
    pDevCtrl->txDma->fcBufAlloc = 0;
    pDevCtrl->txDma->intMask = 0;   /* mask all ints */
    /* clr any pending interrupts on channel */
    pDevCtrl->txDma->intStat = DMA_DONE|DMA_NO_DESC|DMA_BUFF_DONE;
    /* set to interrupt on packet complete */
    /* THT: Follow VxWorks pDevCtrl->txDma->intMask = DMA_DONE; */
    pDevCtrl->txDma->intMask = 0;

TRACE(("init_dma: txDMA.startAddr=%08x, virt=%08x\n", 
pDevCtrl->txDma->startAddr, pDevCtrl->txFirstBdPtr));

    // receive
    pDevCtrl->rxDma->cfg = 0;  // initialize first (will enable later)
    pDevCtrl->rxDma->maxBurst = DMA_MAX_BURST_LENGTH;
    pDevCtrl->rxDma->startAddr = (uint32)PHYSADDR(pDevCtrl->rxFirstBdPtr);
    pDevCtrl->rxDma->length = pDevCtrl->nrRxBds;
    pDevCtrl->rxDma->fcThreshold = 5;
    pDevCtrl->rxDma->fcBufAlloc = 0;
    pDevCtrl->rxDma->intMask = 0;   /* mask all ints */
    /* clr any pending interrupts on channel */
    pDevCtrl->rxDma->intStat = DMA_DONE|DMA_NO_DESC|DMA_BUFF_DONE;
    /* set to interrupt on packet complete and no descriptor available */
    pDevCtrl->rxDma->intMask = DMA_DONE|DMA_NO_DESC;

TRACE(("init_dma: rxDMA.startAddr=%08x, virt=%08x\n", pDevCtrl->rxDma->startAddr, pDevCtrl->rxFirstBdPtr));

    /* configure DMA channels and enable Rx */
    pDevCtrl->rxDma->cfg = DMA_CHAINING|DMA_WRAP_EN|DMA_FLOWC_EN;
    pDevCtrl->txDma->cfg = DMA_CHAINING|DMA_WRAP_EN /*|DMA_FLOWC_EN*/;
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

    TRACE(("bcmemacenet: init_buffers\n"));

    /* set initial state of all BD pointers to top of BD ring */
    pDevCtrl->txCbPtrHead = pDevCtrl->txCbPtrTail = NULL;

    /* allocate recieve buffer pool */
    for (i = 0; i < MAX_RX_BUFS; i++) {
        /* allocate a new SKB for the ring */
        /* 4 bytes for skb pointer */
        skb = dev_alloc_skb(pDevCtrl->rxBufLen + 4);
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
    if (bdfilled)
        pDevCtrl->rxDma->fcBufAlloc = bdfilled;

    return 0;
}

/*
 * bcmemac_init_dev: initialize Ethernet Switch device,
 * allocate Tx/Rx buffer descriptors pool, Tx control block pool.
 */
static int bcmemac_init_dev(BcmEnet_devctrl *pDevCtrl)
{
    int i;
    int nrCbs;
    void *p;
    Enet_Tx_CB *txCbPtr;

    TRACE(("bcmemacenet: bcmemac_init_dev DMA_BASE=%x, RX=%d, TX=%d\n", 
    	DMA_BASE, EMAC_RX_CHAN, EMAC_TX_CHAN));

#if !defined(CONFIG_BCMINTEMAC) && !defined(CONFIG_BCMINTEMAC_MODULE)
    /* make sure emac clock is on */
    INTC->blkEnables |= EMAC_CLK_EN;
#endif

#if defined( CONFIG_BCMINTEMAC ) || defined( CONFIG_BCMINTEMAC_MODULE )
    pDevCtrl->rxIrq = BCM_LINUX_DMA_IRQ;
#else
    pDevCtrl->rxIrq = INTERRUPT_ID_EMAC_RX_CHAN;
#endif
    //BcmHalInterruptDisable(pDevCtrl->rxIrq);

    /* setup buffer/pointer relationships here */
    pDevCtrl->nrTxBds = NR_TX_BDS;
    pDevCtrl->nrRxBds = NR_RX_BDS;
    pDevCtrl->rxBufLen = ENET_MAX_MTU_SIZE;

    /* init rx/tx dma channels */
    
    pDevCtrl->dmaChannels = (DmaChannel *)(DMA_ADR_BASE);
    pDevCtrl->rxDma = &pDevCtrl->dmaChannels[EMAC_RX_CHAN];
    pDevCtrl->txDma = &pDevCtrl->dmaChannels[EMAC_TX_CHAN];

 TRACE(("bcmemacenet: bcmemac_init_dev &rxDMA=%08x, &txDma=%08x\n", 
    	pDevCtrl->rxDma, pDevCtrl->txDma));

    /* allocate and assign tx buffer descriptors */
    if (!(p = kmalloc(pDevCtrl->nrTxBds*sizeof(DmaDesc), GFP_KERNEL))) {
        return -ENOMEM;
    }



    pDevCtrl->txBds = (DmaDesc *)CACHE_TO_NONCACHE(p);   /* tx bd ring */

    /* alloc space for the rx buffer descriptors */
    if (!(p = kmalloc(pDevCtrl->nrRxBds*sizeof(DmaDesc), GFP_KERNEL))) {
        kfree((void *)pDevCtrl->txBds);
        return -ENOMEM;
    }

    pDevCtrl->rxBds = (DmaDesc *)CACHE_TO_NONCACHE(p);   /* rx bd ring */

    /* alloc space for the tx control block pool */
    nrCbs = pDevCtrl->nrTxBds; 
    if (!(p = kmalloc(nrCbs*sizeof(Enet_Tx_CB), GFP_KERNEL))) {
        kfree((void *)pDevCtrl->txBds);
        kfree((void *)pDevCtrl->rxBds);
        return -ENOMEM;
    }
    memset(p, 0, nrCbs*sizeof(Enet_Tx_CB));
    pDevCtrl->txCbs = (Enet_Tx_CB *)p;/* tx control block pool */

    /* initialize rx ring pointer variables. */
    pDevCtrl->rxBdAssignPtr = pDevCtrl->rxBdReadPtr =
                pDevCtrl->rxFirstBdPtr = pDevCtrl->rxBds;
    pDevCtrl->rxLastBdPtr = pDevCtrl->rxFirstBdPtr + pDevCtrl->nrRxBds - 1;

    /* init the receive buffer descriptor ring */
    for (i = 0; i < pDevCtrl->nrRxBds; i++)
    {
        (pDevCtrl->rxFirstBdPtr + i)->status = EMAC_SWAP16(0);
        (pDevCtrl->rxFirstBdPtr + i)->length = EMAC_SWAP16(pDevCtrl->rxBufLen);
        (pDevCtrl->rxFirstBdPtr + i)->address = EMAC_SWAP32(0);

    }
    pDevCtrl->rxLastBdPtr->status = EMAC_SWAP16(DMA_WRAP);

    /* init transmit buffer descriptor variables */
    pDevCtrl->txNextBdPtr = pDevCtrl->txFirstBdPtr = pDevCtrl->txBds;
    pDevCtrl->txLastBdPtr = pDevCtrl->txFirstBdPtr + pDevCtrl->nrTxBds - 1;

    /* clear the transmit buffer descriptors */
    for (i = 0; i < pDevCtrl->nrTxBds; i++)
    {
        (pDevCtrl->txFirstBdPtr + i)->status = EMAC_SWAP16(0);
        (pDevCtrl->txFirstBdPtr + i)->length = EMAC_SWAP16(0);
        (pDevCtrl->txFirstBdPtr + i)->address = EMAC_SWAP32(0);
    }
    pDevCtrl->txLastBdPtr->status = EMAC_SWAP16(DMA_WRAP);
    pDevCtrl->txFreeBds = pDevCtrl->nrTxBds;

    /* initialize the receive buffers and transmit headers */
    if (init_buffers(pDevCtrl)) {
        kfree((void *)pDevCtrl->txBds);
        kfree((void *)pDevCtrl->rxBds);
        kfree((void *)pDevCtrl->txCbs);
        return -ENOMEM;
    }

    for (i = 0; i < nrCbs; i++)
    {
        txCbPtr = pDevCtrl->txCbs + i;
        txCb_enq(pDevCtrl, txCbPtr);
    }

    /* init dma registers */
    init_dma(pDevCtrl);

    /* init switch control registers */
    init_emac(pDevCtrl);

    /* if we reach this point, we've init'ed successfully */
    return 0;
}


#define PROC_DUMP_TXBD_1of6	0
#define PROC_DUMP_TXBD_2of6	1
#define PROC_DUMP_TXBD_3of6	2
#define PROC_DUMP_TXBD_4of6	3
#define PROC_DUMP_TXBD_5of6	4
#define PROC_DUMP_TXBD_6of6	5

#define PROC_DUMP_RXBD_1of6	6
#define PROC_DUMP_RXBD_2of6	7
#define PROC_DUMP_RXBD_3of6	8
#define PROC_DUMP_RXBD_4of6	9
#define PROC_DUMP_RXBD_5of6	10
#define PROC_DUMP_RXBD_6of6	11

#define PROC_DUMP_DMADESC_STATUS	12

#define PROC_DUMP_SKB_1of6		13
#define PROC_DUMP_SKB_2of6		14
#define PROC_DUMP_SKB_3of6		15
#define PROC_DUMP_SKB_4of6		16
#define PROC_DUMP_SKB_5of6		17
#define PROC_DUMP_SKB_6of6		18

#define PROC_DUMP_EMAC_REGISTERS	19



/*
 * bcmemac_net_dump - display EMAC information
 */
int bcmemac_net_dump(BcmEnet_devctrl *pDevCtrl, char *buf, int reqinfo)
{
    int  i;
    int len = 0;
    struct sk_buff *skb;
    static int bufcnt;

    switch (reqinfo) {

/*----------------------------- TXBD --------------------------------*/
    case PROC_DUMP_TXBD_1of6: /* tx DMA BD descriptor ring 1 of 6 */
        len += sprintf(&buf[len], "\ntx buffer descriptor ring status.\n");
        len += sprintf(&buf[len], "BD\tlocation\tlength\tstatus addr\n");
//	 len += sprintf(&buf[len], "First 1/6 of %d buffers\n", pDevCtrl->nrTxBds);
        for (i = 0; i < pDevCtrl->nrTxBds/6; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(unsigned int)&pDevCtrl->txBds[i],
                   EMAC_SWAP16(pDevCtrl->txBds[i].length),
                   EMAC_SWAP16(pDevCtrl->txBds[i].status),
                   EMAC_SWAP32(pDevCtrl->txBds[i].address));
        }
        break;

    case PROC_DUMP_TXBD_2of6: /* tx DMA BD descriptor ring, 2 of 6 */
        for (i = pDevCtrl->nrTxBds/6; i < pDevCtrl->nrTxBds/3; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(unsigned int)&pDevCtrl->txBds[i],
                   EMAC_SWAP16(pDevCtrl->txBds[i].length),
                   EMAC_SWAP16(pDevCtrl->txBds[i].status),
                   EMAC_SWAP32(pDevCtrl->txBds[i].address));
        }
        break;
    case PROC_DUMP_TXBD_3of6: /* tx DMA BD descriptor ring, 3 of 6 */
        for (i = pDevCtrl->nrTxBds/3; i < pDevCtrl->nrTxBds/2; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(unsigned int)&pDevCtrl->txBds[i],
                   EMAC_SWAP16(pDevCtrl->txBds[i].length),
                   EMAC_SWAP16(pDevCtrl->txBds[i].status),
                   EMAC_SWAP32(pDevCtrl->txBds[i].address));
        }
        break;
		
   case PROC_DUMP_TXBD_4of6: /* tx DMA BD descriptor ring, 4 of 6 */
        for (i = pDevCtrl->nrTxBds/2; i < 2*pDevCtrl->nrTxBds/3; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(unsigned int)&pDevCtrl->txBds[i],
                   EMAC_SWAP16(pDevCtrl->txBds[i].length),
                   EMAC_SWAP16(pDevCtrl->txBds[i].status),
                   EMAC_SWAP32(pDevCtrl->txBds[i].address));
        }
        break;

   case PROC_DUMP_TXBD_5of6: /* tx DMA BD descriptor ring, 5 of 6 */
        for (i = 2*pDevCtrl->nrTxBds/3; i < 5*pDevCtrl->nrTxBds/6; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(unsigned int)&pDevCtrl->txBds[i],
                   EMAC_SWAP16(pDevCtrl->txBds[i].length),
                   EMAC_SWAP16(pDevCtrl->txBds[i].status),
                   EMAC_SWAP32(pDevCtrl->txBds[i].address));
        }
        break;

   case PROC_DUMP_TXBD_6of6: /* tx DMA BD descriptor ring, 6 of 6 */
        for (i = 5*pDevCtrl->nrTxBds/6; i < pDevCtrl->nrTxBds; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(unsigned int)&pDevCtrl->txBds[i],
                   EMAC_SWAP16(pDevCtrl->txBds[i].length),
                   EMAC_SWAP16(pDevCtrl->txBds[i].status),
                   EMAC_SWAP32(pDevCtrl->txBds[i].address));
        }
        break;

/*----------------------------- RXBD --------------------------------*/
    case PROC_DUMP_RXBD_1of6: /* rx DMA BD descriptor ring, 1 of 6 */
        len += sprintf(&buf[len], "\nrx buffer descriptor ring status.\n");
        len += sprintf(&buf[len], "BD\tlocation\tlength\tstatus\n");
        for (i = 0; i < pDevCtrl->nrRxBds/6; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(int)&pDevCtrl->rxBds[i],
                   EMAC_SWAP16(pDevCtrl->rxBds[i].length),
                   EMAC_SWAP16(pDevCtrl->rxBds[i].status),
                   EMAC_SWAP32(pDevCtrl->rxBds[i].address));
        }
        break;

    case PROC_DUMP_RXBD_2of6: /* rx DMA BD descriptor ring, 2 of 6 */
        for (i = pDevCtrl->nrRxBds/6; i < pDevCtrl->nrRxBds/3; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(int)&pDevCtrl->rxBds[i],
                   EMAC_SWAP16(pDevCtrl->rxBds[i].length),
                   EMAC_SWAP16(pDevCtrl->rxBds[i].status),
                   EMAC_SWAP32(pDevCtrl->rxBds[i].address));
        }
        break;
   case PROC_DUMP_RXBD_3of6: /* rx DMA BD descriptor ring, 3 of 6 */
        for (i = pDevCtrl->nrRxBds/3; i < pDevCtrl->nrRxBds/2; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(int)&pDevCtrl->rxBds[i],
                   EMAC_SWAP16(pDevCtrl->rxBds[i].length),
                   EMAC_SWAP16(pDevCtrl->rxBds[i].status),
                   EMAC_SWAP32(pDevCtrl->rxBds[i].address));
        }
        break;
   case PROC_DUMP_RXBD_4of6: /* rx DMA BD descriptor ring, 4 of 6 */
        for (i = pDevCtrl->nrRxBds/2; i < 2*pDevCtrl->nrRxBds/3; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(int)&pDevCtrl->rxBds[i],
                   EMAC_SWAP16(pDevCtrl->rxBds[i].length),
                   EMAC_SWAP16(pDevCtrl->rxBds[i].status),
                   EMAC_SWAP32(pDevCtrl->rxBds[i].address));
        }
        break;
   case PROC_DUMP_RXBD_5of6: /* rx DMA BD descriptor ring, 5 of 6 */
        for (i = 2*pDevCtrl->nrRxBds/3; i < 5*pDevCtrl->nrRxBds/6; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(int)&pDevCtrl->rxBds[i],
                   EMAC_SWAP16(pDevCtrl->rxBds[i].length),
                   EMAC_SWAP16(pDevCtrl->rxBds[i].status),
                   EMAC_SWAP32(pDevCtrl->rxBds[i].address));
        }
        break;
   case PROC_DUMP_RXBD_6of6: /* rx DMA BD descriptor ring, 6 of 6 */
        for (i = 5*pDevCtrl->nrRxBds/6; i < pDevCtrl->nrRxBds; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(int)&pDevCtrl->rxBds[i],
                   EMAC_SWAP16(pDevCtrl->rxBds[i].length),
                   EMAC_SWAP16(pDevCtrl->rxBds[i].status),
                   EMAC_SWAP32(pDevCtrl->rxBds[i].address));
        }
        break;

    case PROC_DUMP_DMADESC_STATUS:  /* DMA descriptors pointer and status */
        len += sprintf(&buf[len], "\nrx pointers:\n");
        len += sprintf(&buf[len], "DmaDesc *rxBds:\t\t%08x\n",(unsigned int)pDevCtrl->rxBds);
        len += sprintf(&buf[len], "DmaDesc *rxBdAssignPtr:\t%08x\n",(unsigned int)pDevCtrl->rxBdAssignPtr);
        len += sprintf(&buf[len], "DmaDesc *rxBdReadPtr:\t%08x\n",(unsigned int)pDevCtrl->rxBdReadPtr);
        len += sprintf(&buf[len], "DmaDesc *rxLastBdPtr:\t%08x\n",(unsigned int)pDevCtrl->rxLastBdPtr);
        len += sprintf(&buf[len], "DmaDesc *rxFirstBdPtr:\t%08x\n",(unsigned int)pDevCtrl->rxFirstBdPtr);

        len += sprintf(&buf[len], "\ntx pointers:\n");
        len += sprintf(&buf[len], "DmaDesc *txBds:\t\t%08x\n",(unsigned int)pDevCtrl->txBds);
        len += sprintf(&buf[len], "DmaDesc *txLastBdPtr:\t%08x\n",(unsigned int)pDevCtrl->txLastBdPtr);
        len += sprintf(&buf[len], "DmaDesc *txFirstBdPtr:\t%08x\n",(unsigned int)pDevCtrl->txFirstBdPtr);
        len += sprintf(&buf[len], "DmaDesc *txNextBdPtr:\t%08x\n",(unsigned int)pDevCtrl->txNextBdPtr);
        len += sprintf(&buf[len], "Enet_Tx_CB *txCbPtrHead:%08x\n",(unsigned int)pDevCtrl->txCbPtrHead);
        if (pDevCtrl->txCbPtrHead)
            len += sprintf(&buf[len], "txCbPtrHead->lastBdAddr:\t%08x\n",(unsigned int)pDevCtrl->txCbPtrHead->lastBdAddr);
        len += sprintf(&buf[len], "EnetTxCB *txCbPtrTail:\t%08x\n",(unsigned int)pDevCtrl->txCbPtrTail);
        if (pDevCtrl->txCbPtrTail)
            len += sprintf(&buf[len], "txCbPtrTail->lastBdAddr:\t%08x\n",(unsigned int)pDevCtrl->txCbPtrTail->lastBdAddr);
        len += sprintf(&buf[len], "txFreeBds (TxBDs %d):\t%d\n", NR_TX_BDS, (unsigned int) pDevCtrl->txFreeBds);

        len += sprintf(&buf[len], "\ntx DMA Channel Config\t\t%08x\n", (unsigned int) pDevCtrl->txDma->cfg);
        len += sprintf(&buf[len], "tx DMA Intr Control/Status\t%08x\n", (unsigned int) pDevCtrl->txDma->intStat);
        len += sprintf(&buf[len], "tx DMA Intr Mask\t\t%08x\n", (unsigned int) pDevCtrl->txDma->intMask);
        len += sprintf(&buf[len], "tx DMA Ring Offset\t\t%d\n", (unsigned int) pDevCtrl->txDma->unused[0]);

        len += sprintf(&buf[len], "\nrx DMA Channel Config\t\t%08x\n", (unsigned int) pDevCtrl->rxDma->cfg);
        len += sprintf(&buf[len], "rx DMA Intr Control/Status\t%08x\n", (unsigned int) pDevCtrl->rxDma->intStat);
        len += sprintf(&buf[len], "rx DMA Intr Mask\t\t%08x\n", (unsigned int) pDevCtrl->rxDma->intMask);
        len += sprintf(&buf[len], "rx DMA Ring Offset\t\t%d\n", (unsigned int) pDevCtrl->rxDma->unused[0]);
        len += sprintf(&buf[len], "rx DMA Threshhold\t\t%d\n", (unsigned int) pDevCtrl->rxDma->fcThreshold);
        len += sprintf(&buf[len], "rx DMA Buffer Alloc\t\t%d\n", (unsigned int) pDevCtrl->rxDma->fcBufAlloc);
        break;

/*----------------------------- SKBs --------------------------------*/
    case PROC_DUMP_SKB_1of6: /* skb buffer usage, 1 of 6 */
        bufcnt = 0;
        len += sprintf(&buf[len], "\nskb\taddress\t\tuser\tdataref\n");
        for (i = 0; i < MAX_RX_BUFS/6; i++) {
            skb = pDevCtrl->skb_pool[i];
            len += sprintf(&buf[len], "%d\t%08x\t%d\t%d\n",
                        i, (int)skb, atomic_read(&skb->users), atomic_read(skb_dataref(skb)));
            if ((atomic_read(&skb->users) == 2) && (atomic_read(skb_dataref(skb)) == 1))
                bufcnt++;
        }
        //len += sprintf(&buf[len], "\nnumber rx buffer available %d\n", bufcnt);
        break;

    case PROC_DUMP_SKB_2of6: /* skb buffer usage, part 2 of 6 */
        len += sprintf(&buf[len], "\nskb\taddress\t\tuser\tdataref\n");
        for (i = MAX_RX_BUFS/6; i < MAX_RX_BUFS/3; i++) {
            skb = pDevCtrl->skb_pool[i];
            len += sprintf(&buf[len], "%d\t%08x\t%d\t%d\n",
                        i, (int)skb, atomic_read(&skb->users), atomic_read(skb_dataref(skb)));
            if ((atomic_read(&skb->users) == 2) && (atomic_read(skb_dataref(skb)) == 1))
                bufcnt++;
        }
        //len += sprintf(&buf[len], "\nnumber rx buffer available %d\n", bufcnt);
        break;

    case PROC_DUMP_SKB_3of6: /* skb buffer usage, part 3 of 6 */
        len += sprintf(&buf[len], "\nskb\taddress\t\tuser\tdataref\n");
        for (i = MAX_RX_BUFS/3; i < MAX_RX_BUFS/2; i++) {
            skb = pDevCtrl->skb_pool[i];
            len += sprintf(&buf[len], "%d\t%08x\t%d\t%d\n",
                        i, (int)skb, atomic_read(&skb->users), atomic_read(skb_dataref(skb)));
            if ((atomic_read(&skb->users) == 2) && (atomic_read(skb_dataref(skb)) == 1))
                bufcnt++;
        }
        //len += sprintf(&buf[len], "\nnumber rx buffer available %d\n", bufcnt);
        break;

    case PROC_DUMP_SKB_4of6: /* skb buffer usage, part 4 of 4 */
        len += sprintf(&buf[len], "\nskb\taddress\t\tuser\tdataref\n");
        for (i = MAX_RX_BUFS/2; i < 2*MAX_RX_BUFS/3; i++) {
            skb = pDevCtrl->skb_pool[i];
            len += sprintf(&buf[len], "%d\t%08x\t%d\t%d\n",
                        i, (int)skb, atomic_read(&skb->users), atomic_read(skb_dataref(skb)));
            if ((atomic_read(&skb->users) == 2) && (atomic_read(skb_dataref(skb)) == 1))
                bufcnt++;
        }
        //len += sprintf(&buf[len], "\nnumber rx buffer available %d\n", bufcnt);
        break;
		
    case PROC_DUMP_SKB_5of6: /* skb buffer usage, part 4 of 4 */
        len += sprintf(&buf[len], "\nskb\taddress\t\tuser\tdataref\n");
        for (i = 2*MAX_RX_BUFS/3; i < 5*MAX_RX_BUFS/6; i++) {
            skb = pDevCtrl->skb_pool[i];
            len += sprintf(&buf[len], "%d\t%08x\t%d\t%d\n",
                        i, (int)skb, atomic_read(&skb->users), atomic_read(skb_dataref(skb)));
            if ((atomic_read(&skb->users) == 2) && (atomic_read(skb_dataref(skb)) == 1))
                bufcnt++;
        }
        //len += sprintf(&buf[len], "\nnumber rx buffer available %d\n", bufcnt);
        break;
		
    case PROC_DUMP_SKB_6of6: /* skb buffer usage, part 4 of 4 */
        len += sprintf(&buf[len], "\nskb\taddress\t\tuser\tdataref\n");
        for (i = 5*MAX_RX_BUFS/6; i < MAX_RX_BUFS; i++) {
            skb = pDevCtrl->skb_pool[i];
            len += sprintf(&buf[len], "%d\t%08x\t%d\t%d\n",
                        i, (int)skb, atomic_read(&skb->users), atomic_read(skb_dataref(skb)));
            if ((atomic_read(&skb->users) == 2) && (atomic_read(skb_dataref(skb)) == 1))
                bufcnt++;
        }
        len += sprintf(&buf[len], "\nnumber rx buffer available %d\n", bufcnt);
	 bufcnt = 0; /* Reset */
        break;


    case PROC_DUMP_EMAC_REGISTERS: /* EMAC registers */
        len += sprintf(&buf[len], "\nEMAC registers\n");
        len += sprintf(&buf[len], "rx config register\t0x%08x\n", (int)EMAC->rxControl);
        len += sprintf(&buf[len], "rx max length register\t0x%08x\n", (int)EMAC->rxMaxLength);
        len += sprintf(&buf[len], "tx max length register\t0x%08x\n", (int)EMAC->txMaxLength);
        len += sprintf(&buf[len], "interrupt mask register\t0x%08x\n", (int)EMAC->intMask);
        len += sprintf(&buf[len], "interrupt register\t0x%08x\n", (int)EMAC->intStatus);
        len += sprintf(&buf[len], "control register\t0x%08x\n", (int)EMAC->config);
        len += sprintf(&buf[len], "tx control register\t0x%08x\n", (int)EMAC->txControl);
        len += sprintf(&buf[len], "tx watermark register\t0x%08x\n", (int)EMAC->txThreshold);
		len += sprintf(&buf[len], "pm%d\t0x%08x 0x%08x\n", 0, (int)EMAC->pm0DataHi, (int)EMAC->pm0DataLo);
		len += sprintf(&buf[len], "pm%d\t0x%08x 0x%08x\n", 1, (int)EMAC->pm1DataHi, (int)EMAC->pm1DataLo);
		len += sprintf(&buf[len], "pm%d\t0x%08x 0x%08x\n", 2, (int)EMAC->pm2DataHi, (int)EMAC->pm2DataLo);
		len += sprintf(&buf[len], "pm%d\t0x%08x 0x%08x\n", 3, (int)EMAC->pm3DataHi, (int)EMAC->pm3DataLo);
		break;

    default:
        break;
    }

    return len;
}

#ifdef USE_PROC

static int dev_proc_engine(loff_t pos, char *buf)
{
    BcmEnet_devctrl *pDevCtrl;
    int len = 0;

    if (pDevContext)
        pDevCtrl = pDevContext;
    else
        return len;

    if (pos >= PROC_DUMP_TXBD_1of6 && pos <= PROC_DUMP_EMAC_REGISTERS) {
        len = bcmemac_net_dump(pDevCtrl, buf, pos);
    }
 
    return len;
}



/*
 *  read proc interface
 */
static ssize_t eth_proc_read(struct file *file, char *buf, size_t count,
        loff_t *pos)
{
    char *page;
    int len = 0, x, left;

    page = kmalloc(PAGE_SIZE, GFP_KERNEL);
    if (!page)
        return -ENOMEM;
    left = PAGE_SIZE - 256;
    if (count < left)
        left = count;

    for (;;) {
        x = dev_proc_engine(*pos, &page[len]);
	 if (x > PAGE_SIZE - 256) {
	 	 printk(KERN_CRIT "eth_proc_read: Error buffer too small: left=%d, written=%d, count=%d, arg=%d\n", 
	 		left, x, count, (int) *pos);
	 }
        if (x == 0)
            break;
        if ((x + len) > left) {
 
            x = -EINVAL;
        }
        if (x < 0) {
            break;
        }
        len += x;
        left -= x;
        (*pos)++;
        if (left < 256)
            break;
    }
    if (len > 0 && copy_to_user(buf, page, len))
        len = -EFAULT;
    kfree(page);
    return len;
}


/*
 * /proc/driver/eth_rtinfo
 */
static struct file_operations eth_proc_operations = {
        read: eth_proc_read, /* read_proc */
};

#endif


#define BUFFER_LEN PAGE_SIZE

// THT: Dump the EMAC stats on CTRL-W key stroke in serial ISR

void
dump_emac(void)
{
	// We may have to do dynamic allocation here, since this is run from
	// somebody else's stack
	char buffer[BUFFER_LEN];
	int len, i;
	int bufcnt;
	BcmEnet_devctrl *pDevCtrl = pDevContext;
	struct sk_buff *skb;


       if (pDevCtrl == NULL) {
	   	return; // EMAC not initialized
       }
	   
	/* First qtr of TX ring */
	 printk("\ntx buffer descriptor ring status.\n");
        printk("BD\tlocation\tlength\tstatus addr\n");
	 printk("%d TX buffers\n", pDevCtrl->nrTxBds);
        for (i = 0; i < pDevCtrl->nrTxBds; i++)
        {
            len += printk("%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(unsigned int)&pDevCtrl->txBds[i],
                   EMAC_SWAP16(pDevCtrl->txBds[i].length),
                   EMAC_SWAP16(pDevCtrl->txBds[i].status),
                   EMAC_SWAP32(pDevCtrl->txBds[i].address));
        }

	 /* RX ring */

	 printk("\nrx buffer descriptor ring status.\n");
        printk("BD\tlocation\tlength\tstatus\n");
        for (i = 0; i < pDevCtrl->nrRxBds; i++)
        {
            len += printk("%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(int)&pDevCtrl->rxBds[i],
                   EMAC_SWAP16(pDevCtrl->rxBds[i].length),
                   EMAC_SWAP16(pDevCtrl->rxBds[i].status),
                   EMAC_SWAP32(pDevCtrl->rxBds[i].address));
        }
	

	/* DMA descriptor pointers and status */
	printk("\n\n=========== DMA descriptrs pointers and status ===========\n");
	len = bcmemac_net_dump(pDevContext, buffer, PROC_DUMP_DMADESC_STATUS);
	if (len >= BUFFER_LEN) printk("************ dump_emac: ERROR: Buffer too small, PROC_DUMP_DMADESC_STATUS need %d\n", len);
	buffer[len] = '\0';
	printk("%s\n\n", buffer);

 
	printk("\n\n=========== SKB buffer usage ===========\n");

	 bufcnt = 0;
        printk("\nskb\taddress\t\tuser\tdataref\n");
        for (i = 0; i < MAX_RX_BUFS; i++) {
            skb = pDevCtrl->skb_pool[i];
            printk( "%d\t%08x\t%d\t%d\n",
                        i, (int)skb, atomic_read(&skb->users), atomic_read(skb_dataref(skb)));
            if ((atomic_read(&skb->users) == 2) && (atomic_read(skb_dataref(skb)) == 1))
                bufcnt++;
        }
        printk("\nnumber RX SKBs available %d\n", bufcnt);
        

	/* EMAC Registers */
	printk("\n\n=========== EMAC Registers  ===========\n");
	len = bcmemac_net_dump(pDevContext, buffer, PROC_DUMP_EMAC_REGISTERS);
	if (len >= BUFFER_LEN) printk("************ dump_emac: ERROR: Buffer too small, PROC_DUMP_EMAC_REGISTERS need %d\n", len);
	buffer[len] = '\0';
	printk("%s\n\n", buffer);

	printk("Other Debug info: Loop Count=%d, #rx_errors=%lu, #resets=%d, #overflow=%d,#NO_DESC=%d\n", 
		loopCount,  pDevCtrl->stats.rx_errors, gNumResetsErrors, gNumOverflowErrors, gNumNoDescErrors);
	printk("Last dma flag=%08x, last error dma flag = %08x, timer-scheduled=%d, devInUsed=%d, linkState=%d\n",
		gLastDmaFlag, gLastErrorDmaFlag, gTimerScheduled, atomic_read(&pDevCtrl->devInUsed), pDevCtrl->linkState);
}



/*
 *      bcmemac_net_probe: - Probe Ethernet switch and allocate device
 */
#ifdef MODULE
static
#endif
int __init bcmemac_net_probe(void)
{
    static int probed = 0;
    int status;
    struct net_device *dev = NULL;
    int i;
    unsigned char macAddr[ETH_ALEN];

#if (!defined( CONFIG_BCMINTEMAC ) && !defined( CONFIG_BCMINTEMAC_MODULE ))
    int addr = NVRAM_ENET_MAC_ADDRESS;
#endif

#ifdef USE_PROC
    struct proc_dir_entry *p;
#endif

    TRACE(("bcmemacenet: bcmemac_net_probe\n"));

    if (probed == 0) {
#ifdef USE_PROC
        /*
         * create a /proc entry for display driver runtime information
         */
        if ((p = create_proc_entry("driver/eth_rtinfo", 0, NULL)) == NULL)
            return -ENOMEM;
        p->proc_fops = &eth_proc_operations;
#endif
        pDevContext = kmalloc(sizeof(BcmEnet_devctrl), GFP_KERNEL);
        if (pDevContext == NULL) {
            printk((KERN_ERR CARDNAME ": unable to allocate device context\n"));
            return -ENOMEM;
        }
        /* initialize the context memory */
        memset(pDevContext, 0, sizeof(BcmEnet_devctrl));
        /* figure out which chip we're running on */

        pDevContext->chipId  = (INTC->RevID & 0xFFFF0000) >> 16;
            pDevContext->chipRev = (INTC->RevID & 0xFF);
        switch (pDevContext->chipId) {
            case 0x6345:
            case 0x7110:
                break;
            default:
                printk(KERN_DEBUG CARDNAME" not found\n");
                kfree(pDevContext);
                pDevContext = NULL;
                return -ENODEV;
        }
        /* print the ChipID and module version info */
        printk("Broadcom BCM%X%X Ethernet Network Device ",
                        pDevContext->chipId, pDevContext->chipRev);
        printk(VER_STR "\n");

#if defined( CONFIG_BCMINTEMAC ) || defined( CONFIG_BCMINTEMAC_MODULE )
		pDevContext->phyAddr = MII_EXTERNAL;
#else
        /* board type initialization */
        /* Identify BCM96345 board type by checking GPIO bits.
         * GPIO bit 7 6 5    Board type
         *          0 0 0    Undefined
         *          0 0 1    Undefined
         *          0 1 0    Undefined
         *          0 1 1    USB
         *          1 0 0    R 1.0
         *          1 0 1    I
         *          1 1 0    SV
         *          1 1 1    R 0.0
         */
        GPIO->GPIODir &= (unsigned short) ~BCM9634X_BOARD_TYPE_MASK;
        switch( (GPIO->GPIOio & BCM9634X_BOARD_TYPE_MASK) )
        {
        case BCM96345I_BOARD_TYPE:
            pDevContext->phyAddr = MII_EXTERNAL;
            break;

        case BCM96345SV_BOARD_TYPE:
        case BCM96345R00_BOARD_TYPE:
        default:
            pDevContext->phyAddr = MII_INTERNAL;
            break;
        }
#endif

        if ((status = bcmemac_init_dev(pDevContext))) {
            kfree(pDevContext);
            pDevContext = NULL;
            return status;
        }
            /* setup the rx irq */
        /* register the interrupt service handler */
#if defined( CONFIG_BCMINTEMAC ) || defined( CONFIG_BCMINTEMAC_MODULE )
	/* At this point dev is not initialized yet, so use dummy name */
	request_irq(BCM_LINUX_DMA_IRQ, bcmemac_net_isr, SA_SHIRQ, dev->name, pDevContext);

	// WOrk around for now
	//BcmHalInterruptDisable(BCM_LINUX_DMA_IRQ);
#else
        BcmHalMapInterrupt((FN_ISR)bcmemac_net_isr, (unsigned int)pDevContext, pDevContext->rxIrq);
#endif

        spin_lock_init(&pDevContext->lock);
        atomic_set(&pDevContext->devInUsed, 0);
#ifdef USE_BH
        pDevContext->task.routine = bcmemac_rx;
        pDevContext->task.data = (void *)pDevContext;
#endif
        /*
         * Setup the timer
         */
        init_timer(&pDevContext->timer);
        pDevContext->timer.data = (unsigned long)pDevContext;
        pDevContext->timer.function = tx_reclaim_timer;

#ifdef DUMP_DATA
            printk(KERN_INFO CARDNAME ": CPO BRCMCONFIG: %08X\n", read_32bit_cp0_register($22));
            printk(KERN_INFO CARDNAME ": CPO MIPSCONFIG: %08X\n", read_32bit_cp0_register($16));
            printk(KERN_INFO CARDNAME ": CPO MIPSSTATUS: %08X\n", read_32bit_cp0_register($12));
#endif

        dev = init_etherdev(NULL, 0);

        if (dev == NULL) {
            printk(KERN_ERR CARDNAME": Unable to allocate "
                    "net_device structure!\n");
            return -ENODEV;
        }

        TRACE(("bcmemacenet: bcmemac_net_probe dev 0x%x\n", (unsigned int)dev));

        SET_MODULE_OWNER(dev);

        /* store the pDevContext pointer in our device */
        dev->priv = pDevContext;

        /* back pointer to our device */
        pDevContext->dev = dev;

        /* Read MAC address */
#if (defined( CONFIG_BCMINTEMAC ) || defined( CONFIG_BCMINTEMAC_MODULE ))
/* 7110 */
	{
#define FLASH_MACADDR_OFFSET 0x00FFF824
		uint8 flash_eaddr[6];
		void *virtAddr;
		uint16 word;
		//virtAddr = (void*) 0xBDFFF824; /* BCM97110 in-flash Ethernet MAC address */
		virtAddr = (void*) (0xA0000000|(getPhysFlashBase() + FLASH_MACADDR_OFFSET)); 

              /* It is a common problem that the flash and/or Chip Select are
		 * not initialized properly, so leave this printk on
		 */
		printk(KERN_INFO "%s: Reading MAC address from %08lX, FLASH_BASE=%08lx\n", 
			dev->name,(uint32) virtAddr, (unsigned long) 0xA0000000L|getPhysFlashBase());

		word=0;
		word=readw(virtAddr);
		flash_eaddr[0]=(uint8) (word & 0x00FF);
		flash_eaddr[1]=(uint8) ((word & 0xFF00) >> 8);
		word=readw(virtAddr+2);
		flash_eaddr[2]=(uint8) (word & 0x00FF);
		flash_eaddr[3]=(uint8) ((word & 0xFF00) >> 8);
		word=readw(virtAddr+4);
		flash_eaddr[4]=(uint8) (word & 0x00FF);
		flash_eaddr[5]=(uint8) ((word & 0xFF00) >> 8);

		printk(KERN_INFO "%s: MAC address %02X:%02X:%02X:%02X:%02X:%02X fetched from addr %lX\n",
			dev->name,
			flash_eaddr[0],flash_eaddr[1],flash_eaddr[2],
			flash_eaddr[3],flash_eaddr[4],flash_eaddr[5],
			(uint32) virtAddr);

#if 1
/* Use hard coded value if Flash not properly initialized */
		if ((*flash_eaddr & 0xff) == 0xff)
		{
#ifdef USE_HARDCODED_MACADDR
			flash_eaddr[0] = 0x00;
			flash_eaddr[1] = 0xc0;
			flash_eaddr[2] = 0xa8;
			flash_eaddr[3] = 0x74;
			flash_eaddr[4] = 0x3b;
			flash_eaddr[5] = 0x51;
#else
			printk(KERN_ERR "%s: Cannot read EMAC address from flash.  Please run EVAL codes\n", CARDNAME);
			return -ENODEV;
#endif
		}
#endif
		memcpy(macAddr, flash_eaddr, 6);
	}



#else
        macAddr[0] = 0xff;
        kerSysNvRamGet((char *)macAddr, sizeof(macAddr), addr);

        if( (macAddr[0] & 0x01) == 0x01 ) {
            memcpy( macAddr, "\x00\x10\x18\x63\x00\x00", 6 );
	    	printk((KERN_CRIT "%s: MAC address has not been initialized in NVRAM.\n"),
	    		dev->name);
	}
#endif

        /* fill in the MAC address */
        for (i = 0; i < 6; i++) {
            dev->dev_addr[i] = macAddr[i];
        }

        /* print the Ethenet address */
        printk("%s: MAC Address: ", dev->name);
        for (i = 0; i < 5; i++) {
            printk("%2.2X:", dev->dev_addr[i]);
        }
        printk("%2.2X\n", dev->dev_addr[i]);

        dev->irq                = pDevContext->rxIrq;
        dev->base_addr          = ENET_MAC_ADR_BASE;
        dev->open               = bcmemac_net_open;
        dev->stop               = bcmemac_net_close;
        dev->hard_start_xmit    = bcmemac_net_xmit;
        dev->tx_timeout         = bcmemac_net_timeout;
        dev->watchdog_timeo     = 2*HZ;
        dev->get_stats          = bcmemac_net_query;
        dev->set_mac_address    = bcm_set_mac_addr;
        dev->set_multicast_list = bcm_set_multicast_list;
        dev->do_ioctl           = &bcmemac_enet_ioctl;

        write_mac_address(dev);
        probed++;
        return 0;
    }

    TRACE(("<-- bcmemac_net_probe\n"));
    /* device has already been initialized */
    return -ENXIO;
}

/* get ethernet port's status; return nonzero for Link up, 0 for Link down */
int bcmIsEnetUp(uint32 uPhyAddr)
{
    static int linkState = 0;
    static int sem = 0;

    if (uPhyAddr == MII_EXTERNAL) {
        linkState = 1;
    } else {
        unsigned short value;

        if ( sem )
            return linkState;
        else
            sem = 1;
        value = (unsigned short)(mii_read(uPhyAddr,0x1) & 0xffff);

        if (value & 0x20)
            linkState = 1;
        else
            linkState = 0;

        sem = 0;
    }
    return linkState;
}

/*
 * Generic cleanup handling data allocated during init. Used when the
 * module is unloaded or if an error occurs during initialization
 */
static void bcmemac_init_cleanup(struct net_device *dev)
{
    TRACE(("%s: bcmemac_init_cleanup\n", dev->name));

    unregister_netdev(dev);
#ifdef USE_PROC
    remove_proc_entry("driver/eth_rtinfo", NULL);
#endif
}

static void __exit bcmemac_module_cleanup(void)
{
    Enet_Tx_CB *txCBPtr;
    int i;
    BcmEnet_devctrl *pDevCtrl = pDevContext;
    void* k0p = NULL;

    TRACE(("bcmemacenet: bcmemac_module_cleanup\n"));

    if (pDevCtrl) {
        /* disable DMA */
        pDevCtrl->txDma->cfg = 0;
        pDevCtrl->rxDma->cfg = 0;

        /* free the irq */
            if (pDevCtrl->rxIrq)
        {
            BcmHalInterruptDisable(pDevCtrl->rxIrq);
            free_irq(pDevCtrl->rxIrq, pDevCtrl);
        }

        /* free the skb in the txCbPtrHead */
        while (pDevCtrl->txCbPtrHead)  {
            pDevCtrl->txFreeBds += pDevCtrl->txCbPtrHead->nrBds;

            dev_kfree_skb (pDevCtrl->txCbPtrHead->skb);

            txCBPtr = pDevCtrl->txCbPtrHead;

            /* Advance the current reclaim pointer */
            pDevCtrl->txCbPtrHead = pDevCtrl->txCbPtrHead->next;

            /* Finally, return the transmit header to the free list */
            txCb_enq(pDevCtrl, txCBPtr);
        }
        bcmemac_init_cleanup(pDevCtrl->dev);

        /* release allocated receive buffer memory */
        for (i = 0; i < MAX_RX_BUFS; i++) {
            if (pDevCtrl->skb_pool[i] != NULL) {
                dev_kfree_skb (pDevCtrl->skb_pool[i]);
                pDevCtrl->skb_pool[i] = NULL;
            }
        }

        /* free the transmit buffer descriptor */
        if (pDevCtrl->txBds) {
	     k0p = (void*) NONCACHE_TO_CACHE(pDevCtrl->txBds);
            kfree(k0p);
	     pDevCtrl->txBds = NULL;
        }


        /* free the receive buffer descriptor */
        if (pDevCtrl->rxBds) {
	     k0p = (void*) NONCACHE_TO_CACHE(pDevCtrl->rxBds);
            kfree(k0p);
	     pDevCtrl->rxBds = NULL;
        }
        /* free the transmit control block pool */
        if (pDevCtrl->txCbs) {
            kfree(pDevCtrl->txCbs);
	     pDevCtrl->txCbs = NULL;
        }

        kfree(pDevCtrl);
        pDevContext = NULL;

    }
}

static int bcmemac_enet_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    int *data=(int*)rq->ifr_data;
    int status;


    TRACE(("%s: bcmemac_enet_ioctl: cmd=%d\n", dev->name, cmd));

    /* we can add sub-command in ifr_data if we need to in the future */
    switch (cmd)
    {
    case SIOCGLINKSTATE:
        status = bcmIsEnetUp(((BcmEnet_devctrl *)dev->priv)->phyAddr);
        if (copy_to_user((void*)data, (void*)&status, sizeof(int))) {
            return -EFAULT;
        }
        break;

    case SIOCSCLEARMIBCNTR:
        ASSERT(pDevCtrl != NULL);
        memset(&pDevCtrl->stats, 0, sizeof(struct net_device_stats));
        break;
    }
    return 0;
}

int __init bcmemac_module_init(void)
{
    int status;

    TRACE(("bcmemacenet: bcmemac_module_init\n"));
    status = bcmemac_net_probe();

    return status;
}


#if defined(MODULE)
/*
 * Linux module entry.
 */
int init_module(void)
{
    return( bcmemac_module_init() );
}

/*
 * Linux module exit.
 */
void cleanup_module(void)
{
    if (MOD_IN_USE)
        printk(KERN_DEBUG CARDNAME" module is in use.  Cleanup is delayed.\n");
    else
        bcmemac_module_cleanup();
}
#endif

#if !defined(MODULE)
module_init(bcmemac_module_init);
module_exit(bcmemac_module_cleanup);
#endif

