/*
 *
 * Copyright (c) 2002-2005 Broadcom Corporation 
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
// File Name  : bcmemac.c
//
// Description: This is Linux network driver for the BCM 
// 				EMAC Internal Ethenet Controller onboard the 7110
//               
// Updates    : 09/26/2001  jefchiao.  Created.
//**************************************************************************

#define CARDNAME    "BCMINTMAC"
#define VERSION     "1.0"
#define VER_STR     "v" VERSION " " __DATE__ " " __TIME__

// Turn this on to allow Hardware Multicast Filter
#define MULTICAST_HW_FILTER

#define PAUSE_LO_THRESHOLD 5
#define PAUSE_HI_THRESHOLD 10


#if defined(CONFIG_MODVERSIONS) && ! defined(MODVERSIONS)
   #include <linux/modversions.h> 
   #define MODVERSIONS
#endif  

//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
	#define MOD_DEC_USE_COUNT
	#define MOD_INC_USE_COUNT
//#endif

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

#if defined( CONFIG_BCMINTEMAC_7038 ) || defined( CONFIG_BCMINTEMAC_7038_MODULE )

#include "intemac_map.h"

#if 1
/* 5/24/06: Regardless of flash size, the offset of the MAC-addr is always the same */
#define FLASH_MACADDR_ADDR 0xBFFFF824
#else

  #ifdef CONFIG_MIPS_BCM7318
  #include <asm/brcmstb/common/brcmstb.h>
  /* 16MB Flash */
  #define FLASH_MACADDR_OFFSET 0x00FFF824
  
  #elif defined(CONFIG_MIPS_BCM7038) || defined(CONFIG_MIPS_BCM7400) || defined(CONFIG_MIPS_BCM7401) 
  /* 32MB Flash */
  #define FLASH_MACADDR_OFFSET 0x01FFF824

  #else
  #error "Unknown platform, FLASH_MACADDR_OFFSET must be defined\n"
  #endif

#endif
#endif

#include "if_net.h"


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


/*
 * IP Header Optimization, on 7401B0 and on-wards
 * We present an aligned buffer to the DMA engine, but ask it
 * to transfer the data with a 2-byte offset from the top.
 * The idea is to have the IP payload aligned on a 4-byte boundary
 * because the IP payload follows a 14-byte Ethernet header
 */

/*
 * Private fields for 7401B0 on-wards
 * Set the offset into the data buffer (bits 9-10 of RX_CONTROL)
 */
#define RX_CONFIG_PKT_OFFSET_SHIFT	9
#define RX_CONFIG_PKT_OFFSET_MASK		0x0000_0600

//#define DUMP_DATA

static inline int haveIPHdrOptimization(void)
{
	static int once;
	static int haveIPHeaderOpt;

	if (unlikely(!once)) {
		
// Which revisions of the chip have the fix.
#ifdef CONFIG_MIPS_BCM7438
#define FIXED_REV	0x74380010	/* FIXME */
		volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;
#elif defined CONFIG_MIPS_BCM7038
#define FIXED_REV	0x70380025	//FIXME BCM7038C5?
		volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;

#elif defined( CONFIG_MIPS_BCM7318 )
#define FIXED_REV	0x73180013	/****** 7318B3? *********/
		volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xfffe0600;

#elif defined( CONFIG_MIPS_BCM7400 )
#define FIXED_REV	0x74000001	/****** FIX ME *********/
		volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;
#elif defined( CONFIG_MIPS_BCM7401 ) || defined( CONFIG_MIPS_BCM7402 ) || defined( CONFIG_MIPS_BCM7402S )
#define FIXED_REV	0x74010010	/****** 7401B0 is first chip with the fix *********/
								/* Should also work with a true 7402 chip */
		volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;
#else
  #error "Unsupported platform"
#endif

		once = 1;
		haveIPHeaderOpt =  (*pSundryRev >= FIXED_REV);
		printk("SUNDRY revision = %08lx, have IP Hdr Opt=%d\n", *pSundryRev, haveIPHeaderOpt);
	}
	return haveIPHeaderOpt;
}



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
static irqreturn_t bcmemac_net_isr(int irq, void *, struct pt_regs *regs);
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
static void init_IUdma(BcmEnet_devctrl *pDevCtrl);
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

/* --------------------------------------------------------------------------
    Name: bcmemac_get_free_txdesc
 Purpose: Get Current Available TX desc count
-------------------------------------------------------------------------- */
int bcmemac_get_free_txdesc( struct net_device *net ){
	BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *) net->priv;
	return pDevCtrl->txFreeBds;
}
/* --------------------------------------------------------------------------
    Name: bcmemac_get_device
 Purpose: Obtain net_device handle (for export) 
-------------------------------------------------------------------------- */
struct net_device * bcmemac_get_device( void ){
	BcmEnet_devctrl *pDevCtrl = pDevContext;
	return pDevCtrl->dev;
}

static inline void IncFlowCtrlAllocRegister(BcmEnet_devctrl *pDevCtrl) 
{
	volatile unsigned long* pAllocReg = &pDevCtrl->dmaRegs->flowctl_ch1_alloc;
	
	/* Make sure we don't go over bound */
	if (*pAllocReg < NR_RX_BDS) {
		*pAllocReg = 1;
	}
	else {
		//printk("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ AllocReg = %d\n", *pAllocReg);
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

/* --------------------------------------------------------------------------
    Name: bcmemac_net_open
 Purpose: Open and Initialize the EMAC on the chip
-------------------------------------------------------------------------- */
static int bcmemac_net_open(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    unsigned long flags;

    ASSERT(pDevCtrl != NULL);

    TRACE(("%s: bcmemac_net_open, EMACConf=%x, &RxDMA=%x, rxDma.cfg=%x\n", 
    dev->name, EMAC->config, &pDevCtrl->rxDma, pDevCtrl->rxDma->cfg));

    MOD_INC_USE_COUNT;

    spin_lock_irqsave(&pDevCtrl->lock, flags);

    
    /* disable ethernet MAC while updating its registers */
    EMAC->config = EMAC_DISABLE ;
    while(EMAC->config & EMAC_DISABLE);

    EMAC->config |= EMAC_ENABLE;  
    pDevCtrl->dmaRegs->controller_cfg |= IUDMA_ENABLE;         

    // Tell Tx DMA engine to start from the top
#ifdef IUDMA_INIT_WORKAROUND
	{
		unsigned long diag_rdbk;

		pDevCtrl->dmaRegs->enet_iudma_diag_ctl = 0x100; /* enable to read diags. */
		diag_rdbk = pDevCtrl->dmaRegs->enet_iudma_diag_rdbk;
		pDevCtrl->txDma->descPtr = (uint32)CPHYSADDR(pDevCtrl->txFirstBdPtr);

		//pDevCtrl->rxBdAssignPtr = pDevCtrl->rxBdReadPtr = pDevCtrl->rxFirstBdPtr + ((diag_rdbk>>1)&0x3f);
		pDevCtrl->txNextBdPtr = pDevCtrl->txFirstBdPtr + ((diag_rdbk>>(16+1))&0x3f);

		//printk("enet_iudma_diag_rdbk = 0x%08lx.\n", diag_rdbk);
	}
#endif
    
	
    /* Initialize emac registers */
    if (haveIPHdrOptimization()) {
    	   EMAC->rxControl = EMAC_FC_EN | EMAC_UNIFLOW  | (2<<RX_CONFIG_PKT_OFFSET_SHIFT);  
    } 
    else {
    	   EMAC->rxControl = EMAC_FC_EN | EMAC_UNIFLOW /* |EMAC_PROM*/ ;   // Don't allow Promiscuous
    }
    pDevCtrl->rxDma->cfg |= DMA_ENABLE;

    
	

    atomic_inc(&pDevContext->devInUsed);
    //pDevCtrl->rxDma->intMask = DMA_DONE|DMA_NO_DESC;

   /* Are we previously closed by bcmemac_net_close, which set InUsed to -1 */
    if (0 == atomic_read(&pDevContext->devInUsed)) {
       atomic_inc(&pDevContext->devInUsed);
	//BcmHalInterruptEnable(BCM_LINUX_EMAC_IRQ); //TBD we do not need this anymore right???
       BcmHalInterruptEnable(pDevCtrl->rxIrq);
    }
    

#ifdef CONFIG_SMP
	if (smp_processor_id() == 0) {
#endif    
    pDevCtrl->timer.expires = jiffies + TIMEOUT_10MS;
    add_timer(&pDevCtrl->timer);
#ifdef CONFIG_SMP
	}
#endif    

    // Start the network engine
    netif_start_queue(dev);

    spin_unlock_irqrestore(&pDevCtrl->lock, flags);

    return 0;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
/*
 * Polling 'interrupt' - used by things like netconsole to send skbs
 * without having to re-enable interrupts. It's not called while
 * the interrupt routine is executing.
 */

static void bcmemac_poll (struct net_device *dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
	/* disable_irq is not very nice, but with the funny lockless design
	   we have no other choice. */
    //	disable_irq(dev->irq);
    /* Run TX Queue */
    bcmemac_net_xmit(NULL, pDevCtrl->dev);
    /* Run RX Queue */
    bcmemac_rx(pDevCtrl);
    /* Check for Interrupts */
    bcmemac_net_isr (dev->irq, dev, NULL);
    //	enable_irq(dev->irq);
}
#endif /* CONFIG_NET_POLL_CONTROLLER */




/* --------------------------------------------------------------------------
    Name: bcmemac_net_close
 Purpose: Stop communicating with the outside world
    Note: Caused by 'ifconfig ethX down'
-------------------------------------------------------------------------- */
static int bcmemac_net_close(struct net_device * dev)
{
    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    Enet_Tx_CB *txCBPtr;
    unsigned long flags;

    ASSERT(pDevCtrl != NULL);

    TRACE(("%s: bcmemac_net_close\n", dev->name));

    netif_stop_queue(dev);

    MOD_DEC_USE_COUNT;
    spin_lock_irqsave(&pDevCtrl->lock, flags);

    atomic_dec(&pDevContext->devInUsed);

    pDevCtrl->rxDma->cfg &= ~DMA_ENABLE;

    EMAC->config |= EMAC_DISABLE;           

    if (0 == atomic_read(&pDevContext->devInUsed)) {
       atomic_set(&pDevContext->devInUsed, -1); /* Mark interrupt disable */
    	BcmHalInterruptDisable(pDevCtrl->rxIrq);
    }

    del_timer_sync(&pDevCtrl->timer);

    /* free the skb in the txCbPtrHead */
    while (pDevCtrl->txCbPtrHead)  {
        pDevCtrl->txFreeBds += pDevCtrl->txCbPtrHead->nrBds;

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
    spin_unlock_irqrestore(&pDevCtrl->lock, flags);

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

    if (atomic_read(&pDevCtrl->rxDmaRefill) != 0) {
        atomic_set(&pDevCtrl->rxDmaRefill, 0);
        /* assign packet buffers to all available Dma descriptors */
        bdfilled = assign_rx_buffers(pDevCtrl);
        if (bdfilled) {
            //pDevCtrl->rxDma->fcBufAlloc = bdfilled;
            //pDevCtrl->dmaRegs->flowctl_ch1_alloc = bdfilled;
        }
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

#ifdef CONFIG_SMP
    if (smp_processor_id() == 0) {
#endif    
    pDevCtrl->timer.expires = jiffies + TIMEOUT_10MS;
    add_timer(&pDevCtrl->timer);
#ifdef CONFIG_SMP
    }
#endif    
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
    	// unsigned long txStatus = EMAC_SWAP32(pDevCtrl->txCbPtrHead->lastBdAddr->length_status);
        //TRACE(("length-status = %08x\n", txStatus));

        if (EMAC_SWAP32(pDevCtrl->txCbPtrHead->lastBdAddr->length_status) & (DMA_OWN)) {
            break;
        }

        pDevCtrl->txFreeBds += pDevCtrl->txCbPtrHead->nrBds;

		//bcmemac_stream_pkts( dev );

		if(pDevCtrl->txCbPtrHead->skb)
			dev_kfree_skb (pDevCtrl->txCbPtrHead->skb);

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

    /* Reclaim transmitted buffers */
    while (pDevCtrl->txCbPtrHead)  {
    	//unsigned short txStatus = EMAC_SWAP16(pDevCtrl->txCbPtrHead->lastBdAddr->status);
        //TRACE(("txStatus = %08x\n", txStatus));

        if (EMAC_SWAP32(pDevCtrl->txCbPtrHead->lastBdAddr->length_status) & DMA_OWN) {
            break;
        }

        pDevCtrl->txFreeBds += pDevCtrl->txCbPtrHead->nrBds;

		if(pDevCtrl->txCbPtrHead->skb)
            dev_kfree_skb (pDevCtrl->txCbPtrHead->skb);

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
        // TRACE(("%s: bcmemac_net_xmit NULL skb\n", dev->name));
        if (txCBPtr != NULL) {
            txCb_enq(pDevCtrl, txCBPtr);
            netif_wake_queue(dev);
        }
        spin_unlock_irqrestore(&pDevCtrl->lock, flags);
        return 0;
    }


 TRACE(("bcmemac_net_xmit, txCfg=%08x, txIntStat=%08x\n", pDevCtrl->txDma->cfg, pDevCtrl->txDma->intStat));
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

    pDevCtrl->txNextBdPtr->address = EMAC_SWAP32((uint32)CPHYSADDR(skb->data));
    pDevCtrl->txNextBdPtr->length_status  = EMAC_SWAP32((((unsigned long)((skb->len < ETH_ZLEN) ? ETH_ZLEN : skb->len))<<16));
    //pDevCtrl->txNextBdPtr->length  = (skb->len < ETH_ZLEN) ? ETH_ZLEN : skb->len;
//printk("1 txNextBdPtr->address = %x, txNextBdPtr->length_status = %x, len %x, swaplen %x\n", pDevCtrl->txNextBdPtr->address,
//pDevCtrl->txNextBdPtr->length_status, (((unsigned long)skb->len)<<16), EMAC_SWAP32((((unsigned long)skb->len)<<16)) );
    /*
     * Set status of DMA BD to be transmitted and
     * advance BD pointer to next in the chain.
     */
    if (pDevCtrl->txNextBdPtr == pDevCtrl->txLastBdPtr) {
        pDevCtrl->txNextBdPtr->length_status |= EMAC_SWAP32(DMA_WRAP);
        pDevCtrl->txNextBdPtr = pDevCtrl->txFirstBdPtr;
    }
    else {
        pDevCtrl->txNextBdPtr->length_status |= EMAC_SWAP32(0);
        pDevCtrl->txNextBdPtr++;
    }


#ifdef DUMP_DATA
    printk("bcmemac_net_xmit: len %d", skb->len);
    dumpHexData(skb->data, skb->len);
#endif
    /*
     * Turn on the "OWN" bit in the first buffer descriptor
     * This tells the switch that it can transmit this frame.
     */
    firstBdPtr->length_status |= EMAC_SWAP32(DMA_OWN | DMA_SOP | DMA_EOP | DMA_APPEND_CRC);

//bcmemac_dump(pDevCtrl);

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

/* --------------------------------------------------------------------------
    Name: bcmemac_xmit_fragment
 Purpose: Send ethernet traffic Buffer DESC and submit to UDMA
-------------------------------------------------------------------------- */
int bcmemac_xmit_fragment( int ch, unsigned char *buf, int buf_len, 
						   unsigned long tx_flags , struct net_device *dev)
{

    BcmEnet_devctrl *pDevCtrl = (BcmEnet_devctrl *)dev->priv;
    Enet_Tx_CB *txCBPtr;
    volatile DmaDesc *firstBdPtr;
    unsigned long flags;
	
    ASSERT(pDevCtrl != NULL);

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */
    spin_lock_irqsave(&pDevCtrl->lock, flags);
                
    txCBPtr = txCb_deq(pDevCtrl);
    

    /*
     * Obtain a transmit header from the free list.  If there are none
     * available, we can't send the packet. Discard the packet.
     */
    if (txCBPtr == NULL) {
        //netif_stop_queue(dev);
        spin_unlock_irqrestore(&pDevCtrl->lock, flags);
        return 1;
    }

    txCBPtr->nrBds = 1;
    txCBPtr->skb = NULL;

    /* If we could not queue this packet, free it */
    if (pDevCtrl->txFreeBds < txCBPtr->nrBds) {
        TRACE(("%s: bcmemac_net_xmit low on txFreeBds\n", dev->name));
        //txCb_enq(pDevCtrl, txCBPtr);
        //netif_stop_queue(dev);
        spin_unlock_irqrestore(&pDevCtrl->lock, flags);
        return 1;
    }

	/*--------first fragment------*/
    firstBdPtr = pDevCtrl->txNextBdPtr;
    /* store off the last BD address used to enqueue the packet */
    txCBPtr->lastBdAddr = pDevCtrl->txNextBdPtr;

    /* assign skb data to TX Dma descriptor */
    /*
     * Add the buffer to the ring.
     * Set addr and length of DMA BD to be transmitted.
     */
    //dma_cache_wback_inv((unsigned long)skb->data, skb->len);

	pDevCtrl->txNextBdPtr->address = EMAC_SWAP32((uint32)CPHYSADDR(buf));
	pDevCtrl->txNextBdPtr->length_status  = EMAC_SWAP32((((unsigned long)buf_len)<<16));	
    /*
     * Set status of DMA BD to be transmitted and
     * advance BD pointer to next in the chain.
     */
    if (pDevCtrl->txNextBdPtr == pDevCtrl->txLastBdPtr) {
        pDevCtrl->txNextBdPtr->length_status |= EMAC_SWAP32(DMA_WRAP);
        pDevCtrl->txNextBdPtr = pDevCtrl->txFirstBdPtr;
    }
    else {
        //pDevCtrl->txNextBdPtr->length_status |= EMAC_SWAP32(0);
        pDevCtrl->txNextBdPtr++;
    }

    /*
     * Turn on the "OWN" bit in the first buffer descriptor
     * This tells the switch that it can transmit this frame.
     */	
	firstBdPtr->length_status &= ~EMAC_SWAP32(DMA_SOP |DMA_EOP | DMA_APPEND_CRC);
    firstBdPtr->length_status |= EMAC_SWAP32(DMA_OWN | tx_flags | DMA_APPEND_CRC);
		
   
    /* Enable DMA for this channel */
    pDevCtrl->txDma->cfg |= DMA_ENABLE;

    /* Decrement total BD count */
    pDevCtrl->txFreeBds -= txCBPtr->nrBds;

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

    /* update stats */
    pDevCtrl->stats.tx_bytes += buf_len; //((skb->len < ETH_ZLEN) ? ETH_ZLEN : skb->len);
    pDevCtrl->stats.tx_bytes += 4;
    pDevCtrl->stats.tx_packets++;

    dev->trans_start = jiffies;

    spin_unlock_irqrestore(&pDevCtrl->lock, flags);

    return 0;
}

/* --------------------------------------------------------------------------
    Name: bcmemac_xmit_multibuf
 Purpose: Send ethernet traffic in multi buffers (hdr, buf, tail)
-------------------------------------------------------------------------- */
int bcmemac_xmit_multibuf( int ch, unsigned char *hdr, int hdr_len, unsigned char *buf, int buf_len, unsigned char *tail, int tail_len, struct net_device *dev)
{
	while(bcmemac_xmit_check(dev));
	
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
		return 0; /* Drop the packet */
	}
	
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
static irqreturn_t bcmemac_net_isr(int irq, void * dev_id, struct pt_regs * regs)
{
	BcmEnet_devctrl *pDevCtrl = dev_id;
	uint32 rxEvents;
	irqreturn_t ret = IRQ_NONE;

	/* get Rx interrupt events */
	rxEvents = pDevCtrl->rxDma->intStat;

	TRACE(("bcmemac_net_isr: intstat=%08x\n",rxEvents));

	if (rxEvents & DMA_BUFF_DONE)
	{
		pDevCtrl->rxDma->intStat = DMA_BUFF_DONE;  /* clr interrupt */
	}
	if (rxEvents & DMA_DONE)
	{
		pDevCtrl->rxDma->intStat = DMA_DONE;  // clr interrupt
// THT: For now we are not using this, will change to workqueue later on
#ifdef USE_BH       // USE_BH -- better for the over system performance
		// use bottom half (immediate queue).  Put the bottom half of interrupt in the tasklet
		queue_task(&pDevCtrl->task, &tq_immediate);
		mark_bh(IMMEDIATE_BH);
		ret = IRQ_HANDLED;
#else
		// no BH and handle every thing inside interrupt routine
		bcmemac_rx(pDevCtrl);
		ret = IRQ_HANDLED;
#endif // USE_BH
	}
	else {
		if (rxEvents & DMA_NO_DESC) {
			pDevCtrl->rxDma->intStat = DMA_NO_DESC;
			//BcmHalInterruptEnable(pDevCtrl->rxIrq);

			gNumNoDescErrors++;
printk(KERN_NOTICE "DMA_NO_DESC, count=%d, fc_alloc=%ld\n", 
			gNoDescCount, pDevCtrl->dmaRegs->flowctl_ch1_alloc);
             
		
        	pDevCtrl->rxDma->intStat = DMA_NO_DESC; // Clear the interrupt
			//if (gNumberOfOverflows > OVERFLOW_THRESHOLD || gNoDescCount > NODESC_THRESHOLD ) 
            {

				int bdfilled;
				
				gNoDescCount = 0;

				bdfilled = assign_rx_buffers(pDevCtrl);
				printk(KERN_DEBUG "bcmemac_rx_isr: After assigning buffers, filled=%d\n", bdfilled);
				if (bdfilled == 0) {
				/* For now, until we figure out why we can't reclaim the buffers */
					printk(KERN_CRIT "Running out of buffers, all busy\n");
				}
		        else {
			        //pDevCtrl->dmaRegs->flowctl_ch1_alloc = bdfilled;
			        /* Re-enable DMA if there are buffers available */  
		   	        pDevCtrl->rxDma->cfg |= DMA_ENABLE;
		        }
			    ResetEMAConErrors();
			}
		}
		//pDevCtrl->rxDma->intMask = DMA_DONE|DMA_NO_DESC;
		ret =  IRQ_HANDLED; // We did handle the error condition.
	}
	return ret;
}

/*
 *  bcmemac_rx: Process all received packets.
 */
 #define MAX_RX                  0x0fffffff //to disable it
static void bcmemac_rx(void *ptr)
{
    BcmEnet_devctrl *pDevCtrl = ptr;
    struct sk_buff *skb;
    struct net_device *dev;
    unsigned long dmaFlag;
    unsigned char *pBuf;
    int len;
    int bdfilled;
    unsigned int packetLength = 0;

 			
    dmaFlag = ((EMAC_SWAP32(pDevCtrl->rxBdReadPtr->length_status)) & 0xffff);
gLastDmaFlag = dmaFlag;

    loopCount = 0;

    /* Loop here for each received packet */
    while(!(dmaFlag & DMA_OWN) && (dmaFlag & DMA_EOP))
    {
    	   if (++loopCount > MAX_RX) {
	   	break;
	   }
	   
	    // Stop when we hit a buffer with no data, or a BD w/ no buffer.
        // This implies that we caught up with DMA, or that we haven't been
        // able to fill the buffers.
        if ( (pDevCtrl->rxBdReadPtr->address == (uint32) NULL))
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

        if (dmaFlag & (EMAC_CRC_ERROR | EMAC_OV | EMAC_NO | EMAC_LG |EMAC_RXER)) {
	        if (dmaFlag & EMAC_CRC_ERROR) {
                pDevCtrl->stats.rx_crc_errors++;
            }
	        if (dmaFlag & EMAC_OV) {
                pDevCtrl->stats.rx_over_errors++;
            }
	        if (dmaFlag & EMAC_NO) {
                pDevCtrl->stats.rx_frame_errors++;
            }
	        if (dmaFlag & EMAC_LG) {
                pDevCtrl->stats.rx_length_errors++;
            }
            pDevCtrl->stats.rx_dropped++;
            pDevCtrl->stats.rx_errors++;

/* Debug */
gLastErrorDmaFlag = dmaFlag;
            

	     /* THT Starting with 7110B0 (Atlanta's PR5760), look for resetting the 
              * EMAC on overflow condition
              */
	     if ((dmaFlag & DMA_OWN) == 0) {
		 uint32 bufferAddress;

               // PR5760 - keep track of the number of overflow packets.
               if (dmaFlag & EMAC_OV)
               {
                  	gNumberOfOverflows++;
			        gNumOverflowErrors++;
               }

	        // Keep a running total of the packet length.
               packetLength += ((EMAC_SWAP32(pDevCtrl->rxBdReadPtr->length_status))>>16);

		 // Pull the buffer from the BD and clear the BD so that it can be
               // reassigned later.
               bufferAddress = (uint32) EMAC_SWAP32(pDevCtrl->rxBdReadPtr->address);
               pDevCtrl->rxBdReadPtr->address = 0;

		 pBuf = (unsigned char *)KSEG0ADDR(bufferAddress);
		 skb = (struct sk_buff *)*(unsigned long *)(pBuf-4);
		 atomic_set(&skb->users, 1);
 
              if (atomic_read(&pDevCtrl->rxDmaRefill) == 0) {
                  bdfilled = assign_rx_buffers(pDevCtrl);
              }
	     
              if (bdfilled) {
                //pDevCtrl->rxDma->fcBufAlloc = bdfilled;
 	            //pDevCtrl->dmaRegs->flowctl_ch1_alloc = bdfilled;
		         //pDevCtrl->rxDma->cfg |= DMA_ENABLE;
              }

              /* Advance BD ptr to next in ring */
              IncRxBDptr(pDevCtrl->rxBdReadPtr, pDevCtrl);

	       // If this is the last buffer in the packet, stop.
        	if (dmaFlag & DMA_EOP)
        	{
        		packetLength = 0;
            		//break;
        	}

              //Read more until EOP or good packet
              //dmaFlag = ((EMAC_SWAP32(pDevCtrl->rxBdReadPtr->length_status)) & 0xffff);
 //gLastDmaFlag = dmaFlag;      
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

#if 0
// ALready done in ISR

   	     // If the DMA stalled because there were no descriptors, then start it back
            // up.
            if (pDevCtrl->rxDma->intStat & DMA_NO_DESC)
            {
                pDevCtrl->rxDma->intMask = 0; /* THT */
        	  pDevCtrl->rxDma->intStat |= DMA_NO_DESC;
		  // THT: We enable it here, but will correct it at the top of the ISR
                pDevCtrl->rxDma->cfg |= DMA_ENABLE;

    		  /* THT: set to interrupt on packet complete and no descriptor available */
    		  pDevCtrl->rxDma->intMask = DMA_DONE|DMA_NO_DESC;

                //EthMib.RxDmaOverflowCnt++;
		  gNoDescCount++;
		  gNumNoDescErrors++;
             }
	      else


           // PR5577 - for some reason, we can wind up getting cases where we need to
	    // kick the DMA, but the DMA_NO_DESC flag is not set.  This shows up
	    // primarily when our EMAC is connected to another EMAC via MII (like on
	    // RG platforms where there is an ethernet switch).  We have seen cases
	    // where lots of overflows can wind up making the DMA controller shut down,
	    // so that nothing goes between the two EMACs.  We'll go ahead and kick
	    // this here to un-wedge the MACs.
	    
	    {
	        pDevCtrl->rxDma->cfg = DMA_ENABLE;
		 pDevCtrl->rxDma->intMask = DMA_DONE|DMA_NO_DESC;	
	    }
#endif

           //Read more until EOP or good packet
           dmaFlag = ((EMAC_SWAP32(pDevCtrl->rxBdReadPtr->length_status)) & 0xffff);
gLastDmaFlag = dmaFlag; 
	   /* Exit if we surpass number of packets */
	   

           continue;
	   
        }/* if error packet */

        gNumberOfOverflows = 0;
	 gNoDescCount = 0;

       pBuf = (unsigned char *)(KSEG0ADDR(EMAC_SWAP32(pDevCtrl->rxBdReadPtr->address)));
TRACE(("bcmemac_rx: pBuf=%08p\n", pBuf));

#if defined( CONFIG_BCMINTEMAC_7038 ) || defined( CONFIG_BCMINTEMAC_7038_MODULE ) \
	/*
	  * THT: Invalidate the RAC cache again, since someone may have read near the vicinity
	  * of the buffer.  This is necessary because the RAC cache is much larger than the CPU cache
	  */
	bcm_inv_rac_all();
#endif

        len = ((EMAC_SWAP32(pDevCtrl->rxBdReadPtr->length_status))>>16);
        /* Null the BD field to prevent reuse */
        pDevCtrl->rxBdReadPtr->length_status &= EMAC_SWAP32(0xffff0000); //clear status.
        pDevCtrl->rxBdReadPtr->address = EMAC_SWAP32(0);

	/* Advance BD ptr to next in ring */
        IncRxBDptr(pDevCtrl->rxBdReadPtr, pDevCtrl);
	// Recover the SKB pointer saved during assignment.
        skb = (struct sk_buff *)*(unsigned long *)(pBuf-4);
		
        dmaFlag = ((EMAC_SWAP32(pDevCtrl->rxBdReadPtr->length_status))&0xffff);
gLastDmaFlag = dmaFlag;

        /* remove Ethernet CRC */
        skb_trim(skb, len - 4);
        
        dev = pDevCtrl->dev;

#ifdef DUMP_DATA
        printk("bcmemac_rx :");
        dumpHexData(skb->data, 32);
#endif

        /* Finish setting up the received SKB and send it to the kernel */
        skb->dev = dev;

        /* Set the protocol type */
        skb->protocol = eth_type_trans(skb, dev);

        /* Allocate a new SKB for the ring */
        if (atomic_read(&pDevCtrl->rxDmaRefill) == 0) {
            bdfilled = assign_rx_buffers(pDevCtrl);
        }
        if (bdfilled) {
  
	     // Tell the engine how many new buffers we just put back
                //pDevCtrl->dmaRegs->flowctl_ch1_alloc = bdfilled;
        }

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

    if(netif_running(dev))
        return -EBUSY;

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




static void handleAlignment(struct sk_buff* skb)
{
	/* Even that the EMAC now accept any buffers aligned on a 4-B boundary, we still gain
	 * better performance if we align the buffer on a 256-B boundary
	 */
	{
		// We will reserve 20 bytes, and wants that skb->data to be on 
		// a 256B boundary.

		volatile unsigned long boundary256, curData, resHeader;
		
		curData = (unsigned long) skb->data;
		boundary256 = (curData+20+255) & 0xffffff00;
		resHeader = boundary256 - curData;

		// If we don't have room for 20B, go to next boundary.  Sanity check
		if (resHeader < 20) {
			boundary256 += 256;
		}
		resHeader = boundary256 - curData - 4;
		// This advances skb->data 4B shy of the boundary,
		// and also includes the 16B headroom commented out below,
		// by virtue of our sanity check above.
		skb_reserve(skb, resHeader);

	        /* reserve space ditto __dev_alloc_skb */
	       // skb_reserve(skb, 16);

	        *(unsigned int *)skb->data = (unsigned int)skb;
	        skb_reserve(skb, 4);
			
		 // Make sure it is on 256B boundary, should never happen if our
		 // calculation was correct.
		 if ((unsigned long) skb->data & 0xff) {
	printk("$$$$$$$$$$$$$$$$$$$ skb buffer is NOT aligned on 256B boundary\n");
		}

		if (haveIPHdrOptimization()) {
			/* Advance the data pointer 2 bytes */
			skb_reserve(skb, 2);
		}
	}
#if 0
	else {
		/*
		 * We want the data pointer to be off 2 byte on a 4-B boundary,
		 * such that the IP payload is 4-byte aligned
		 * We will reserve 20 bytes plus the extra 2 bytes for 2-byte-off
		 * mis-alignment.
		 */
		/* reserve space ditto __dev_alloc_skb */
		skb_reserve(skb,16);

		*(unsigned int *)skb->data = (unsigned int)skb;
		/* This advances data to the 2-off bytes, so that data is off 2 byte for IP optimization
		 * Since the Ethernet header is 14 byte long, this will result in the payload being 4-byte aligned.
		 */
		skb_reserve(skb, 6);
		 // Make sure it is off 2 byte on a 4B boundary.  It should never happen if our
		 // calculation was correct.
		 if (((unsigned long) skb->data & 0x03) != 2) {
	printk("$$$$$$$$$$$$$$$$$$$ skb buffer is NOT 2-off on a 4Byte boundary\n");
		}
	}
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
                skb = pDevCtrl->skb_pool[pDevCtrl->nextskb++];
                if (pDevCtrl->nextskb == MAX_RX_BUFS)
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

	  atomic_set(&pDevCtrl->rxDmaRefill, 0);
        skb_headerinit(skb, NULL, 0);  /* clean state */

        /* init the skb, in case other app. modified the skb pointer */
        skb->data = skb->tail = skb->head;
        skb->end = skb->data + (skb->truesize - sizeof(struct sk_buff));
        skb->len = 0;
        skb->data_len = 0;
        skb->cloned = 0;

        handleAlignment(skb);

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
	  if (haveIPHdrOptimization()) {
	  	unsigned long dmaAddr = (unsigned long) skb->data-2;
		if (dmaAddr & 0xff) {
			printk("@@@@@@@@@@@@@@@ DMA Address %08x not aligned\n", (unsigned int) dmaAddr);
		}
	  	pDevCtrl->rxBdAssignPtr->address = EMAC_SWAP32((uint32)CPHYSADDR(dmaAddr));
	  }
	  else {
        	pDevCtrl->rxBdAssignPtr->address = EMAC_SWAP32((uint32)CPHYSADDR(skb->data));
	  }
        pDevCtrl->rxBdAssignPtr->length_status  = EMAC_SWAP32((pDevCtrl->rxBufLen<<16));

        IncFlowCtrlAllocRegister(pDevCtrl);

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

//bcmemac_dump(pDevCtrl);
//printk("init rx des **************\n");

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
    case 4:
        pmDataLo = &EMAC->pm4DataLo;
        pmDataHi = &EMAC->pm4DataHi;
        break;
    case 5:
        pmDataLo = &EMAC->pm5DataLo;
        pmDataHi = &EMAC->pm5DataHi;
        break;
    case 6:
        pmDataLo = &EMAC->pm6DataLo;
        pmDataHi = &EMAC->pm6DataHi;  		/* PR 10861 - fixed wrong value here */
        break;
    case 7:
        pmDataLo = &EMAC->pm7DataLo;
        pmDataHi = &EMAC->pm7DataHi;
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


    /* disable ethernet MAC while updating its registers */
    EMAC->config = EMAC_DISABLE ;
    while(EMAC->config & EMAC_DISABLE);

    /* issue soft reset, wait for it to complete */
    EMAC->config = EMAC_SOFT_RESET;
    while (EMAC->config & EMAC_SOFT_RESET);

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

	/* Ethernet header optimization, reserve 2 bytes at head of packet */
    	if (haveIPHdrOptimization()) {
		unsigned int packetOffset = (2 << RX_CONFIG_PKT_OFFSET_SHIFT);
      		EMAC->rxControl =   EMAC_FC_EN | EMAC_UNIFLOW | packetOffset;
    	}
	else {
    		EMAC->rxControl = EMAC_FC_EN | EMAC_UNIFLOW;  // THT dropped for RR support | EMAC_PROM;   // allow Promiscuous
    	}

#ifdef MAC_LOOPBACK
    EMAC->rxControl |= EMAC_LOOPBACK;
#endif
    EMAC->rxMaxLength = ENET_MAX_MTU_SIZE+(haveIPHdrOptimization()? 2: 0);
    EMAC->txMaxLength = ENET_MAX_MTU_SIZE;

    /* tx threshold = abs(63-(0.63*EMAC_DMA_MAX_BURST_LENGTH)) */
    EMAC->txThreshold = 0x180;  // THT PR12238 as per David Ferguson: Turning off RR, Was 32
    EMAC->mibControl  = 1;
    EMAC->intMask = 0;              /* mask all EMAC interrupts*/

	TRACE(("done init emac\n"));
}

/*
 * init_dma: Initialize DMA control register
 */
static void init_IUdma(BcmEnet_devctrl *pDevCtrl)
{
    TRACE(("bcmemacenet: init_dma\n"));

/* TBD : what about endianess??? */
#if 0
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
#endif

#if 0 /* TBD We may have to do this somehow. */
    uint32 uData;

    if (pDevCtrl->phyAddr == MII_EXTERNAL) {
        uData = *(uint32 *)0xfffe0418;
        *(uint32 *)0xfffe0418 |= 0x00007007; /* External MII */
        uData = *(uint32 *)0xfffe0418;
    }
#endif

    /*
     * initialize IUDMA controller register
     */
    pDevCtrl->dmaRegs->controller_cfg = DMA_FLOWC_CH1_EN;// |IUDMA_ENABLE;
    // 1, was 5, want to force to NO_DESC intr so that we can handle it
    pDevCtrl->dmaRegs->flowctl_ch1_thresh_lo = PAUSE_LO_THRESHOLD;//1 
    pDevCtrl->dmaRegs->flowctl_ch1_thresh_hi = PAUSE_HI_THRESHOLD;
    //pDevCtrl->dmaRegs->flowctl_ch1_alloc = 0;

    // transmit
    pDevCtrl->txDma->cfg = 0;       /* initialize first (will enable later) */
    pDevCtrl->txDma->maxBurst = DMA_MAX_BURST_LENGTH;  // THT per David F, to allow 256B burst, was 8; /*DMA_MAX_BURST_LENGTH;*/

    pDevCtrl->txDma->intMask = 0;   /* mask all ints */
    /* clr any pending interrupts on channel */
    pDevCtrl->txDma->intStat = DMA_DONE|DMA_NO_DESC|DMA_BUFF_DONE;
    /* set to interrupt on packet complete */
    /* THT: Follow VxWorks pDevCtrl->txDma->intMask = DMA_DONE; */
    pDevCtrl->txDma->intMask = 0;

	pDevCtrl->txDma->descPtr = (uint32)CPHYSADDR(pDevCtrl->txFirstBdPtr);

TRACE(("init_dma: txDMA.startAddr=%08x, virt=%08x\n", 
pDevCtrl->txDma->descPtr, pDevCtrl->txFirstBdPtr));

    // receive
    pDevCtrl->rxDma->cfg = 0;  // initialize first (will enable later)
    pDevCtrl->rxDma->maxBurst = DMA_MAX_BURST_LENGTH;  // THT per David F, to allow 256B burst, was 8; /*DMA_MAX_BURST_LENGTH;*/

    pDevCtrl->rxDma->descPtr = (uint32)CPHYSADDR(pDevCtrl->rxFirstBdPtr);
    pDevCtrl->rxDma->intMask = 0;   /* mask all ints */
    /* clr any pending interrupts on channel */
    pDevCtrl->rxDma->intStat = DMA_DONE|DMA_NO_DESC|DMA_BUFF_DONE;
    /* set to interrupt on packet complete and no descriptor available */
    pDevCtrl->rxDma->intMask = DMA_DONE; //|DMA_NO_DESC;

TRACE(("init_dma: rxDMA.startAddr=%08x, virt=%08x\n", pDevCtrl->rxDma->descPtr, pDevCtrl->rxFirstBdPtr));

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
        /* 4 bytes for skb pointer, 256B for alignment, plus 2 bytes for IP header optimization */
        skb = dev_alloc_skb(pDevCtrl->rxBufLen + 4 + 256);
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
        //pDevCtrl->rxDma->fcBufAlloc = bdfilled;
        printk("init_buffers: %d descriptors initialized\n", bdfilled);
    }
    // This avoid depending on flowclt_alloc which may go negative during init
    pDevCtrl->dmaRegs->flowctl_ch1_alloc = IUDMA_CH1_FLOW_ALLOC_FORCE | bdfilled;
    printk("init_buffers: %08lx descriptors initialized, from flowctl\n", pDevCtrl->dmaRegs->flowctl_ch1_alloc);

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

#if !defined(CONFIG_BCMINTEMAC_7038) && !defined(CONFIG_BCMINTEMAC_7038_MODULE)
    /* make sure emac clock is on */
    INTC->blkEnables |= EMAC_CLK_EN;
#endif

#if defined( CONFIG_BCMINTEMAC_7038 ) || defined( CONFIG_BCMINTEMAC_7038_MODULE )
    pDevCtrl->rxIrq = BCM_LINUX_CPU_ENET_IRQ;
#else
    pDevCtrl->rxIrq = INTERRUPT_ID_EMAC_RX_CHAN;
#endif
    //BcmHalInterruptDisable(pDevCtrl->rxIrq);

    /* setup buffer/pointer relationships here */
    pDevCtrl->nrTxBds = NR_TX_BDS;
    pDevCtrl->nrRxBds = NR_RX_BDS;
    pDevCtrl->rxBufLen = ENET_MAX_MTU_SIZE + (haveIPHdrOptimization() ? 2: 0);

    /* init rx/tx dma channels */
    
    pDevCtrl->dmaRegs = (DmaRegs *)(DMA_ADR_BASE);
    pDevCtrl->rxDma = &pDevCtrl->dmaRegs->chcfg[EMAC_RX_CHAN];
    pDevCtrl->txDma = &pDevCtrl->dmaRegs->chcfg[EMAC_TX_CHAN];

 TRACE(("bcmemacenet: bcmemac_init_dev &rxDMA=%08x, &txDma=%08x\n", 
    	pDevCtrl->rxDma, pDevCtrl->txDma));

#if 0
    /* allocate and assign tx buffer descriptors */
    if (!(p = kmalloc(pDevCtrl->nrTxBds*sizeof(DmaDesc), GFP_KERNEL))) {
        return -ENOMEM;
    }
    pDevCtrl->txBds = (DmaDesc *)CACHE_TO_NONCACHE(p);   /* tx bd ring */
#endif
	pDevCtrl->txBds = (DmaDesc *) EMAC_TX_DESC_BASE;

#if 0
    /* alloc space for the rx buffer descriptors */
    if (!(p = kmalloc(pDevCtrl->nrRxBds*sizeof(DmaDesc), GFP_KERNEL))) {
        kfree((void *)pDevCtrl->txBds);
        return -ENOMEM;
    }
    pDevCtrl->rxBds = (DmaDesc *)CACHE_TO_NONCACHE(p);   /* rx bd ring */
#endif
	pDevCtrl->rxBds = (DmaDesc *) EMAC_RX_DESC_BASE;
	
    /* alloc space for the tx control block pool */
    nrCbs = pDevCtrl->nrTxBds; 
    if (!(p = kmalloc(nrCbs*sizeof(Enet_Tx_CB), GFP_KERNEL))) {
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

    /* init dma registers */
    init_IUdma(pDevCtrl);

    /* init switch control registers */
    init_emac(pDevCtrl);

#ifdef IUDMA_INIT_WORKAROUND
	{
		unsigned long diag_rdbk;

		pDevCtrl->dmaRegs->enet_iudma_diag_ctl = 0x100; /* enable to read diags. */
		diag_rdbk = pDevCtrl->dmaRegs->enet_iudma_diag_rdbk;

		pDevCtrl->rxBdAssignPtr = pDevCtrl->rxBdReadPtr = pDevCtrl->rxFirstBdPtr + ((diag_rdbk>>1)&0x3f);
		pDevCtrl->txNextBdPtr = pDevCtrl->txFirstBdPtr + ((diag_rdbk>>(16+1))&0x3f);

		//printk("enet_iudma_diag_rdbk = 0x%08lx.\n", diag_rdbk);
	}
#endif	


#ifdef DUMP_DATA
	// THT 11/11/04: Dump ENET_TOP registers
{
	volatile unsigned long* paddr = (volatile unsigned long*) ENET_MAC_ADR_BASE;
	printk("ENET_TOP_RX_CONFIG@08x = %08x\n", paddr, *paddr);
	paddr++;
	printk("ENET_TOP_RX_LENGTH@08x = %08x\n", paddr, *paddr);
	paddr++;	
	printk("ENET_TOP_RX_MAXLENGTH@08x = %08x\n", paddr, *paddr);
	paddr++;	
	#if 0
	printk("ENET_TOP_MII_S_C@08x = %08x\n", paddr, *paddr);
	paddr++;	
	printk("ENET_TOP_MII_S_C@08x = %08x\n", paddr, *paddr);
	paddr++;
	printk("ENET_TOP_MII_S_C@08x = %08x\n", paddr, *paddr);
	paddr++;	
	printk("ENET_TOP_MII_S_C@08x = %08x\n", paddr, *paddr);
	paddr++;	
	printk("ENET_TOP_MII_S_C@08x = %08x\n", paddr, *paddr);
	paddr++;	
	printk("ENET_TOP_MII_S_C@08x = %08x\n", paddr, *paddr);
	paddr++;		
	printk("ENET_TOP_MII_S_C@08x = %08x\n", paddr, *paddr);
	paddr++;	
	#endif
	}
#endif	

    /* if we reach this point, we've init'ed successfully */
    return 0;
}

#ifdef USE_PROC

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
    int len = 0;
#if 0
	int  i;
    struct sk_buff *skb;
    static int bufcnt;
#endif
    switch (reqinfo) {

#if 0

/*----------------------------- TXBD --------------------------------*/
    case PROC_DUMP_TXBD_1of6: /* tx DMA BD descriptor ring 1 of 6 */
        len += sprintf(&buf[len], "\ntx buffer descriptor ring status.\n");
        len += sprintf(&buf[len], "BD\tlocation\tlength\tstatus addr\n");
//	 len += sprintf(&buf[len], "First 1/6 of %d buffers\n", pDevCtrl->nrTxBds);
        for (i = 0; i < pDevCtrl->nrTxBds/6; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(unsigned int)&pDevCtrl->txBds[i],
                   (pDevCtrl->txBds[i].length_status>>16),
                   (pDevCtrl->txBds[i].length_status&0xffff),
                   (pDevCtrl->txBds[i].address));
        }
        break;

    case PROC_DUMP_TXBD_2of6: /* tx DMA BD descriptor ring, 2 of 6 */
        for (i = pDevCtrl->nrTxBds/6; i < pDevCtrl->nrTxBds/3; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(unsigned int)&pDevCtrl->txBds[i],
                   (pDevCtrl->txBds[i].length_status>>16),
                   (pDevCtrl->txBds[i].length_status&0xffff),
                   (pDevCtrl->txBds[i].address));
        }
        break;

   case PROC_DUMP_TXBD_3of6: /* tx DMA BD descriptor ring, 3 of 6 */
        for (i = pDevCtrl->nrTxBds/3; i < pDevCtrl->nrTxBds/2; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(unsigned int)&pDevCtrl->txBds[i],
                   (pDevCtrl->txBds[i].length_status>>16),
                   (pDevCtrl->txBds[i].length_status&0xffff),
                   (pDevCtrl->txBds[i].address));
        }
        break;

   case PROC_DUMP_TXBD_4of6: /* tx DMA BD descriptor ring, 4 of 6 */
        for (i = pDevCtrl->nrTxBds/2; i < 2*pDevCtrl->nrTxBds/3; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(unsigned int)&pDevCtrl->txBds[i],
                   (pDevCtrl->txBds[i].length_status>>16),
                   (pDevCtrl->txBds[i].length_status&0xffff),
                   (pDevCtrl->txBds[i].address));
        }
        break;

  case PROC_DUMP_TXBD_5of6: /* tx DMA BD descriptor ring, 5 of 6 */
        for (i = 2*pDevCtrl->nrTxBds/3; i < 5*pDevCtrl->nrTxBds/6; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(unsigned int)&pDevCtrl->txBds[i],
                   (pDevCtrl->txBds[i].length_status>>16),
                   (pDevCtrl->txBds[i].length_status&0xffff),
                   (pDevCtrl->txBds[i].address));
        }
        break;

 case PROC_DUMP_TXBD_6of6: /* tx DMA BD descriptor ring, 5 of 6 */
        for (i = 5*pDevCtrl->nrTxBds/6; i < pDevCtrl->nrTxBds; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(unsigned int)&pDevCtrl->txBds[i],
                   (pDevCtrl->txBds[i].length_status>>16),
                   (pDevCtrl->txBds[i].length_status&0xffff),
                   (pDevCtrl->txBds[i].address));
        }
        break;

/*----------------------------- RXBD --------------------------------*/
    case PROC_DUMP_RXBD_1of6: /* rx DMA BD descriptor ring, 1 of 6 */
        len += sprintf(&buf[len], "\nrx buffer descriptor ring status.\n");
        len += sprintf(&buf[len], "BD\tlocation\tlength\tstatus\n");
        for (i = 0; i < pDevCtrl->nrRxBds/6; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(unsigned int)&pDevCtrl->rxBds[i],
                   (pDevCtrl->rxBds[i].length_status>>16),
                   (pDevCtrl->rxBds[i].length_status&0xffff),
				   (pDevCtrl->rxBds[i].address));
        }
        break;

   case PROC_DUMP_RXBD_2of6: /* rx DMA BD descriptor ring, 2 of 6 */
        for (i = pDevCtrl->nrRxBds/6; i < 2*pDevCtrl->nrRxBds/6; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(int)&pDevCtrl->rxBds[i],
                   (pDevCtrl->rxBds[i].length_status>>16),
                   (pDevCtrl->rxBds[i].length_status&0xffff),
				   (pDevCtrl->rxBds[i].address));
        }
		break;

   case PROC_DUMP_RXBD_3of6: /* rx DMA BD descriptor ring, 3 of 6 */
        for (i = 2*pDevCtrl->nrRxBds/6; i < 3*pDevCtrl->nrRxBds/6; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(int)&pDevCtrl->rxBds[i],
                   (pDevCtrl->rxBds[i].length_status>>16),
                   (pDevCtrl->rxBds[i].length_status&0xffff),
				   (pDevCtrl->rxBds[i].address));
        }
		break;

  case PROC_DUMP_RXBD_4of6: /* rx DMA BD descriptor ring, 4 of 6 */
        for (i = 3*pDevCtrl->nrRxBds/6; i < 4*pDevCtrl->nrRxBds/6; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(int)&pDevCtrl->rxBds[i],
                   (pDevCtrl->rxBds[i].length_status>>16),
                   (pDevCtrl->rxBds[i].length_status&0xffff),
				   (pDevCtrl->rxBds[i].address));
        }
		break;

  case PROC_DUMP_RXBD_5of6: /* rx DMA BD descriptor ring, 5 of 6 */
        for (i = 4*pDevCtrl->nrRxBds/6; i < 5*pDevCtrl->nrRxBds/6; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(int)&pDevCtrl->rxBds[i],
                   (pDevCtrl->rxBds[i].length_status>>16),
                   (pDevCtrl->rxBds[i].length_status&0xffff),
				   (pDevCtrl->rxBds[i].address));
        }
		break;

  case PROC_DUMP_RXBD_6of6: /* rx DMA BD descriptor ring, 5 of 6 */
        for (i = 5*pDevCtrl->nrRxBds/6; i < 6*pDevCtrl->nrRxBds/6; i++)
        {
            len += sprintf(&buf[len], "%03d\t%08x\t%04d\t%04x %08lx\n",
                   i,(int)&pDevCtrl->rxBds[i],
                   (pDevCtrl->rxBds[i].length_status>>16),
                   (pDevCtrl->rxBds[i].length_status&0xffff),
				   (pDevCtrl->rxBds[i].address));
        }
        break;
#endif

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
        //len += sprintf(&buf[len], "tx DMA Ring Offset\t\t%d\n", (unsigned int) pDevCtrl->txDma->unused[0]);

        len += sprintf(&buf[len], "\nrx DMA Channel Config\t\t%08x\n", (unsigned int) pDevCtrl->rxDma->cfg);
        len += sprintf(&buf[len], "rx DMA Intr Control/Status\t%08x\n", (unsigned int) pDevCtrl->rxDma->intStat);
        len += sprintf(&buf[len], "rx DMA Intr Mask\t\t%08x\n", (unsigned int) pDevCtrl->rxDma->intMask);
        //len += sprintf(&buf[len], "rx DMA Ring Offset\t\t%d\n", (unsigned int) pDevCtrl->rxDma->unused[0]);
        len += sprintf(&buf[len], "rx DMA Threshhold Lo\t\t%d\n", (unsigned int) pDevCtrl->dmaRegs->flowctl_ch1_thresh_lo);
        len += sprintf(&buf[len], "rx DMA Threshhold Hi\t\t%d\n", (unsigned int) pDevCtrl->dmaRegs->flowctl_ch1_thresh_hi);
        len += sprintf(&buf[len], "rx DMA Buffer Alloc\t\t%d\n", (unsigned int) pDevCtrl->dmaRegs->flowctl_ch1_alloc);
        break;

#if 0
/*----------------------------- SKBs --------------------------------*/
    case PROC_DUMP_SKB_1of6: /* skb buffer usage, 1 of 6 */
        bufcnt = 0;
        len += sprintf(&buf[len], "\nskb\taddress\t\tuser\tdataref\n");
        for (i = 0; i < MAX_RX_BUFS/6; i++) {
            skb = pDevCtrl->skb_pool[i];
	     if (skb) {
                len += sprintf(&buf[len], "%d\t%08x\t%d\t%d\n",
                        i, (int)skb, atomic_read(&skb->users), atomic_read(skb_dataref(skb)));
            	  if ((atomic_read(&skb->users) == 2) && (atomic_read(skb_dataref(skb)) == 1))
                    bufcnt++;
	     	}
        }
        //len += sprintf(&buf[len], "\nnumber rx buffer available %d\n", bufcnt);
        break;

    case PROC_DUMP_SKB_2of6: /* skb buffer usage, part 2 of 6 */
        len += sprintf(&buf[len], "\nskb\taddress\t\tuser\tdataref\n");
        for (i = MAX_RX_BUFS/6; i < 2*MAX_RX_BUFS/6; i++) {
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
#endif

    case PROC_DUMP_EMAC_REGISTERS: /* EMAC registers */
        len += sprintf(&buf[len], "\nEMAC registers\n");
        len += sprintf(&buf[len], "rx EMAC->rxControl\t\t%08x\n", (unsigned int) EMAC->rxControl);
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
	len += sprintf(&buf[len], "pm%d\t0x%08x 0x%08x\n", 4, (int)EMAC->pm4DataHi, (int)EMAC->pm4DataLo);
	len += sprintf(&buf[len], "pm%d\t0x%08x 0x%08x\n", 5, (int)EMAC->pm5DataHi, (int)EMAC->pm5DataLo);
	len += sprintf(&buf[len], "pm%d\t0x%08x 0x%08x\n", 6, (int)EMAC->pm6DataHi, (int)EMAC->pm6DataLo);
	len += sprintf(&buf[len], "pm%d\t0x%08x 0x%08x\n", 7, (int)EMAC->pm7DataHi, (int)EMAC->pm7DataLo);
        break;

    default:
        break;
    }

    return len;
}

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
        if (x == 0)
            break;
        if ((x + len) > left)
            x = -EINVAL;
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


#define BUFFER_LEN PAGE_SIZE


void bcmemac_dump(BcmEnet_devctrl *pDevCtrl)
{
//	int i;
	
	// We may have to do dynamic allocation here, since this is run from
	// somebody else's stack
	char buffer[BUFFER_LEN];
	int len, i;
	int bufcnt;
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
            len += printk("%03d\t%08x\t%04ld\t%04lx %08lx\n",
               i,(unsigned int)&pDevCtrl->txBds[i],
               (pDevCtrl->txBds[i].length_status>>16),
               (pDevCtrl->txBds[i].length_status&0xffff),
				pDevCtrl->txBds[i].address);
    }


	 printk("\nrx buffer descriptor ring status.\n");
        printk("BD\tlocation\tlength\tstatus\n");
        for (i = 0; i < pDevCtrl->nrRxBds; i++)
        {
            len += printk("%03d\t%08x\t%04ld\t%04lx %08lx\n",
                   i,(int)&pDevCtrl->rxBds[i],
                   
                   (pDevCtrl->rxBds[i].length_status>>16),
                   (pDevCtrl->rxBds[i].length_status&0xffff),
					pDevCtrl->rxBds[i].address);
        }


	/* DMA descriptor pointers and status */
	printk("\n\n=========== DMA descriptrs pointers and status ===========\n");
	len = bcmemac_net_dump(pDevContext, buffer, PROC_DUMP_DMADESC_STATUS);
	if (len >= BUFFER_LEN) printk("************ dump_emac: ERROR: Buffer too small, PROC_DUMP_DMADESC_STATUS need %d\n", len);
	buffer[len] = '\0';
	printk("%s\n\n", buffer);


	printk("\n\n=========== SKB buffer usage ===========\n");
#if 1
	 bufcnt = 0;
        printk("\nskb\taddress\t\tuser\tdataref\n");
        for (i = 0; i < MAX_RX_BUFS; i++) {
            skb = pDevCtrl->skb_pool[i];
            printk( "%d\t%08x\t%d\t%d\n",
                        i, (int)skb, atomic_read(&skb->users), atomic_read(skb_dataref(skb)));
            if ((atomic_read(&skb->users) == 2) && (atomic_read(skb_dataref(skb)) == 1))
                bufcnt++;

	}
#endif    
        printk("\nnumber RX SKBs available %d\n", bufcnt);

        		
	/* EMAC Registers */
	printk("\n\n=========== EMAC Registers  ===========\n");
	len = bcmemac_net_dump(pDevContext, buffer, PROC_DUMP_EMAC_REGISTERS);
	if (len >= BUFFER_LEN) printk("************ dump_emac: ERROR: Buffer too small, PROC_DUMP_EMAC_REGISTERS need %d\n", len);
	buffer[len] = '\0';
	printk("%s\n\n", buffer);

	printk("Other Debug info: Loop Count=%d, #rx_errors=%lu, #resets=%d, #overflow=%d,#NO_DESC=%d\n", 
		loopCount,  pDevCtrl->stats.rx_errors, gNumResetsErrors, gNumOverflowErrors, gNumNoDescErrors);
	printk("Last dma flag=%08x, last error dma flag = %08x, devInUsed=%d, linkState=%d\n",
		gLastDmaFlag, gLastErrorDmaFlag, /*gTimerScheduled, */atomic_read(&pDevCtrl->devInUsed), pDevCtrl->linkState);
	
}

#endif


void dump_emac(void)
	{
#ifdef USE_PROC
	if (pDevContext) {
		bcmemac_dump(pDevContext);
	}
#endif
}
EXPORT_SYMBOL(dump_emac);


static void bcmemac_getMacAddr(struct net_device* dev)
{
	uint8 flash_eaddr[ETH_ALEN];
	void *virtAddr;
	uint16 word;
	int i;

#ifndef CONFIG_BRCM_PCI_SLAVE
#if 1
	virtAddr = (void *)FLASH_MACADDR_ADDR;
#else
	//virtAddr = (void*) 0xBDFFF824; /* BCM97110 in-flash Ethernet MAC address */
	virtAddr = (void*) KSEG1ADDR(getPhysFlashBase() + FLASH_MACADDR_OFFSET); 
#endif

          /* It is a common problem that the flash and/or Chip Select are
	 * not initialized properly, so leave this printk on
	 */
	printk("%s: Reading MAC address from %08lX, FLASH_BASE=%08lx\n", 
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

	printk("%s: MAC address %02X:%02X:%02X:%02X:%02X:%02X fetched from addr %lX\n",
		dev->name,
		flash_eaddr[0],flash_eaddr[1],flash_eaddr[2],
		flash_eaddr[3],flash_eaddr[4],flash_eaddr[5],
		(uint32) virtAddr);

#else /* PCI slave cannot access the EBI bus */
/* Use hard coded value if Flash not properly initialized */
	//if ((*flash_eaddr & 0xff) == 0xff)
	{
//#ifdef USE_HARDCODED_MACADDR
		flash_eaddr[0] = 0x00;
		flash_eaddr[1] = 0xc0;
		flash_eaddr[2] = 0xa8;
		flash_eaddr[3] = 0x74;
		flash_eaddr[4] = 0x3b;
		flash_eaddr[5] = 0x51;
		printk("%s: Default MAC address %02X:%02X:%02X:%02X:%02X:%02X used\n",
			dev->name,
			flash_eaddr[0],flash_eaddr[1],flash_eaddr[2],
			flash_eaddr[3],flash_eaddr[4],flash_eaddr[5]);
//#else
//			printk("%s: Cannot read EMAC address from flash.  Please run EVAL codes\n", CARDNAME);
//			return -ENODEV;
//#endif
	}
#endif

        /* fill in the MAC address */
        for (i = 0; i < 6; i++) {
            dev->dev_addr[i] = flash_eaddr[i];
        }

        /* print the Ethenet address */
        printk("%s: MAC Address: ", dev->name);
        for (i = 0; i < 5; i++) {
            printk("%2.2X:", dev->dev_addr[i]);
        }
        printk("%2.2X\n", dev->dev_addr[i]);
}


static void bcmemac_dev_setup(struct net_device *dev)
{
	int ret;
	pDevContext = (struct BcmEnet_devctrl *) netdev_priv(dev);


	if (pDevContext == NULL) {
		printk((KERN_ERR CARDNAME ": unable to allocate device context\n"));
		return;
	}

		
	/* initialize the context memory */
	memset(pDevContext, 0, sizeof(BcmEnet_devctrl));
	/* figure out which chip we're running on */

	/* back pointer to our device */
	//dev->priv = pDevContext;
	pDevContext->dev = dev;

#if defined( CONFIG_MIPS_BCM7038 ) || defined(CONFIG_MIPS_BCM7400) || defined(CONFIG_MIPS_BCM7401) \
	|| defined( CONFIG_MIPS_BCM7402 ) || defined( CONFIG_MIPS_BCM7402S )
		/* TBD : need to get the symbol for these. */
        pDevContext->chipId  = (*((volatile unsigned long*)0xb0000200) & 0xFFFF0000) >> 16;
        pDevContext->chipRev = (*((volatile unsigned long*)0xb0000208) & 0xFF);
#elif defined(CONFIG_MIPS_BCM7318) 
        pDevContext->chipId  = (INTC->RevID & 0xFFFF0000) >> 16;
        pDevContext->chipRev = (INTC->RevID & 0xFF);

#else
  #error "Unsupported platform"
#endif

	switch (pDevContext->chipId) {
		case 0x6345:
		case 0x7110:
		case 0x7038:
		case 0x7318:
		case 0x7401:
		case 0x7400:
		case 0x7438:
		break;
	default:
		printk(KERN_DEBUG CARDNAME" not found\n");
            	free_netdev(dev);
		pDevContext = NULL;
		return;
	}
        /* print the ChipID and module version info */
        printk("Broadcom BCM%X%X Ethernet Network Device ",
                        pDevContext->chipId, pDevContext->chipRev);
        printk(VER_STR "\n");

#if defined( CONFIG_BCMINTEMAC_7038 ) || defined( CONFIG_BCMINTEMAC_7038_MODULE )
		pDevContext->phyAddr = MII_INTERNAL;
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

	bcmemac_getMacAddr(dev);

	if ((ret = bcmemac_init_dev(pDevContext))) {
		free_netdev(dev);
		pDevContext = NULL;
		return;
        }
	TRACE(("after init dev.\n"));
            /* setup the rx irq */
        /* register the interrupt service handler */
#if defined( CONFIG_BCMINTEMAC_7038 ) || defined( CONFIG_BCMINTEMAC_7038_MODULE )
	/* At this point dev is not initialized yet, so use dummy name */
	request_irq(BCM_LINUX_CPU_ENET_IRQ, bcmemac_net_isr, SA_INTERRUPT|SA_SHIRQ, dev->name, pDevContext);

#else
        BcmHalMapInterrupt((FN_ISR)bcmemac_net_isr, (unsigned int)pDevContext, pDevContext->rxIrq);
#endif

		TRACE(("after req irq.\n"));
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
            printk(KERN_INFO CARDNAME ": CPO BRCMCONFIG: %08X\n", read_c0_diag(/*$22*/));
            printk(KERN_INFO CARDNAME ": CPO MIPSCONFIG: %08X\n", read_c0_config(/*$16*/));
            printk(KERN_INFO CARDNAME ": CPO MIPSSTATUS: %08X\n", read_c0_status(/*$12*/));
#endif	
	dev->irq                	= pDevContext->rxIrq;
	dev->base_addr          	= ENET_MAC_ADR_BASE;
	dev->open               	= bcmemac_net_open;
	dev->stop               	= bcmemac_net_close;
	dev->hard_start_xmit   	= bcmemac_net_xmit;
	dev->tx_timeout         	= bcmemac_net_timeout;
	dev->watchdog_timeo  	= 2*HZ;
	dev->get_stats          	= bcmemac_net_query;
	dev->set_mac_address 	= bcm_set_mac_addr;
	dev->set_multicast_list	= bcm_set_multicast_list;
	dev->do_ioctl           	= &bcmemac_enet_ioctl;
#ifdef CONFIG_NET_POLL_CONTROLLER
	dev->poll_controller    = &bcmemac_poll;
#endif

	// These are default settings
	write_mac_address(dev);
	ether_setup(dev);
	
	// Let boot setup info overwrite default settings.
	netdev_boot_setup_check(dev);

	TRACE(("bcmemacenet: bcmemac_net_probe dev 0x%x\n", (unsigned int)dev));

	SET_MODULE_OWNER(dev);

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
	int ret;
	struct net_device *dev = NULL;


#if (!defined( CONFIG_BCMINTEMAC_7038 ) && !defined( CONFIG_BCMINTEMAC_7038_MODULE ))
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

		dev = alloc_netdev(sizeof(struct BcmEnet_devctrl), "eth%d", bcmemac_dev_setup);

		TRACE(("after alloc_netdev.\n"));
		// if bcmemac_dev_setup fails for some reason, pDevContext is NULL
		if (dev == NULL || pDevContext == NULL) {
			printk(KERN_ERR CARDNAME": Unable to allocate "
				"net_device structure!\n");
			return -ENODEV;
		}


	}

	if (0 != (ret = register_netdev(dev))) {
		free_netdev(dev);
		return ret;
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

            if(pDevCtrl->txCbPtrHead->skb)
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
		// On the 7038 these are mapped, not allocated.
		//k0p = (void*) NONCACHE_TO_CACHE(pDevCtrl->txBds);
            	// kfree(k0p);
	     	pDevCtrl->txBds = NULL;
        }

        /* free the receive buffer descriptor */
        if (pDevCtrl->rxBds) {
		//k0p = (void*) NONCACHE_TO_CACHE(pDevCtrl->rxBds);
        	//kfree(k0p);
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

EXPORT_SYMBOL(bcmemac_get_free_txdesc);
EXPORT_SYMBOL(bcmemac_get_device);
EXPORT_SYMBOL(bcmemac_xmit_multibuf);
EXPORT_SYMBOL(bcmemac_xmit_check);

#if !defined(MODULE)
module_init(bcmemac_module_init);
module_exit(bcmemac_module_cleanup);
#endif

