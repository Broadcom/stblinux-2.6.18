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
// File Name  : bcmmii.c
//
// Description: Broadcom PHY/GPHY/Ethernet Switch Configuration
// Revision:	09/25/2008, L.Sun created.
//               
//**************************************************************************

#include <linux/types.h>
#include <linux/delay.h>
#include <linux/mii.h>
#include <linux/ethtool.h>	/* use some constant definations */

#include "boardparms.h"
#include "unimac_map.h"
#include "bcmunimac.h"
#include "bcmmii.h"

typedef struct MII_CONFIG
{
	int auto_ne;	/* 1: Auto-negotiated, 0: forced */
	int pause;		/* Pause capability */
#define NO_PAUSE	0
#define TX_PAUSE	1
#define RX_PAUSE	2
	int hcd;		/* negotiated highest common denominitor */
	int feature;	/* What link feature does this board support.*/
	char * name;	/* Feature name string */
}MII_CONFIG;

/* all possible negotiation result, the SUPPORTED_XXX is defined in ethtool.h*/
static MII_CONFIG g_mii_configs[] = 
{
	{0, 0, LPA_1000FULL, SUPPORTED_1000baseT_Full, "1000BaseT-Full"},
	{0, 0, LPA_1000HALF, SUPPORTED_1000baseT_Half, "1000BaseT-Half"},
	{0, 0, MII_BRCM_AUX_AN_HCD_100TX_FULL, SUPPORTED_100baseT_Full, "100BaseTx-Full"},
	{0, 0, MII_BRCM_AUX_AN_HCD_100TX, SUPPORTED_100baseT_Half, "100BaseTx-Half"},
	{0, 0, MII_BRCM_AUX_AN_HCD_10T_FULL, SUPPORTED_10baseT_Full, "10BaseT-Full"},
	{0, 0, MII_BRCM_AUX_AN_HCD_10T, SUPPORTED_10baseT_Half, "10BaseT-Half"}
};
/* 
 * mapping inband status regiser value to LPA value(link partner ability)
 * Note that we don't support speed 2500Mbps here.
 * TODO: Fix me.
 */
static int	ibs2lpa[] = {
	LPA_10FULL,
	LPA_100FULL,
	LPA_1000FULL,
	0,
	LPA_10HALF,
	LPA_100HALF,
	LPA_1000HALF,
	0
};

extern char * gsPhyType[];	/* PHY type string, defined in boardparams.c */
/* local prototypes */
static MII_CONFIG * mii_getconfig(struct net_device *dev);
static MII_CONFIG * mii_autoconfigure(struct net_device *dev);
static void mii_soft_reset(struct net_device *dev, int PhyAddr);

#ifdef PHY_LOOPBACK
static void mii_loopback(struct net_device *dev);
#endif


/* read a value from the MII */
int mii_read(struct net_device *dev, int phy_id, int location) 
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    volatile uniMacRegs *umac = pDevCtrl->umac;

	if(phy_id == BRCM_PHY_ID_NONE)
		return (0x782d);
    umac->mdio_cmd = MDIO_RD | (phy_id << MDIO_PMD_SHIFT) | (location << MDIO_REG_SHIFT);
	/* Start MDIO transaction*/
	umac->mdio_cmd |= MDIO_START_BUSY;

    while ( (umac->mdio_cmd & MDIO_START_BUSY) )
	{
		udelay(20);
	}
	if (umac->mdio_cmd & MDIO_READ_FAIL)
	{
		TRACE(("MDIO read failure\n"));
		return 0;
	}
	
    return umac->mdio_cmd & 0xffff;
}

/* write a value to the MII */
void mii_write(struct net_device *dev, int phy_id, int location, int val)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    volatile uniMacRegs *umac = pDevCtrl->umac;

	if(phy_id == BRCM_PHY_ID_NONE)
		return;
    umac->mdio_cmd = MDIO_WR | (phy_id << MDIO_PMD_SHIFT ) | (location << MDIO_REG_SHIFT) | (0xffff & val);
	umac->mdio_cmd |= MDIO_START_BUSY;
    while ( (umac->mdio_cmd & MDIO_START_BUSY) )
        ;
}
/* probe for an external PHY via MDIO; return PHY address */
int mii_probe(unsigned long base_addr)
{
	volatile uniMacRegs *umac = (volatile uniMacRegs *)base_addr;
	volatile rbufRegs * txrx_ctrl = (volatile rbufRegs*)(base_addr + UMAC_TXRX_REG_OFFSET) ;
	int i;

	/* 
	 * Enable RGMII to interface external PHY, disable internal 10/100 MII.
	 */
	txrx_ctrl->rgmii_oob_ctrl |= RGMII_MODE_EN;

	for (i = 0; i < 32; i++) {
		umac->mdio_cmd = MDIO_RD | (i << MDIO_PMD_SHIFT) | (MII_BMSR << MDIO_REG_SHIFT);
		udelay(20);
		/* Start MDIO transaction*/
		umac->mdio_cmd |= MDIO_START_BUSY;
		while ( (umac->mdio_cmd & MDIO_START_BUSY) )
		{
			udelay(20);
        }
		if (umac->mdio_cmd & MDIO_READ_FAIL)
			continue;
		else if ((umac->mdio_cmd & 0xffff) != 0x0)
			return i;
	}
	return(BP_ENET_NO_PHY);
}

#ifdef PHY_LOOPBACK
/* set the MII loopback mode */
static void mii_loopback(struct net_device *dev)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    uint32 val;

    TRACE(("mii_loopback\n"));

    val = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_BMCR);
    /* Disable autonegotiation */
    val &= ~BMCR_ANENABLE;
    /* Enable Loopback */
    val |= BMCR_LOOPBACK;
    mii_write(dev, pDevCtrl->EnetInfo.PhyAddress, MII_BMCR, val);
}
#endif

#ifdef CONFIG_BRCM_SWITCH
void mii_enablephyinterrupt(struct net_device *dev, int phy_id)
{
    mii_write(dev, phy_id, MII_INTERRUPT, 
        MII_INTR_ENABLE | MII_INTR_MASK_FDX | MII_INTR_MASK_LINK_SPEED);
}

void mii_clearphyinterrupt(struct net_device *dev, int phy_id)
{
    mii_read(dev, phy_id, MII_INTERRUPT);
}
#endif /* CONFIG_BRCM_SWITCH */

/* return the current MII configuration */
static MII_CONFIG * mii_getconfig(struct net_device *dev)
{
	int i;
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	volatile rbufRegs * txrx_ctrl = pDevCtrl->txrx_ctrl;
    uint32 val;
	MII_CONFIG * ret = NULL;

    TRACE(("mii_getconfig\n"));

	/*
	 * if it's external PHY, check if it's GPHY.
	 */
	if(pDevCtrl->EnetInfo.PhyType == BP_ENET_EXTERNAL_GPHY)
	{
		/*
		 * GPHY should have extended status register
		 */
		val = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_BMSR);
		if (val & BMSR_ERCAP) {
			val = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_ESTATUS);
			if (val & (ESTATUS_1000_TFULL | ESTATUS_1000_THALF) ) {
				/* Read 1000base-T status register to find out link partner's ability*/
				val = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_STAT1000);
				TRACE(("MII_STAT1000=0x%x\n", val));
				
				for ( i = 0; i < sizeof(g_mii_configs)/sizeof(MII_CONFIG); i++)
				{
					if(g_mii_configs[i].hcd & val) { ret = &g_mii_configs[i]; break;}
				}
				
			}
		}
	}else if (pDevCtrl->EnetInfo.PhyType == BP_ENET_EXTERNAL_GPHY_IBS)
	{
		/*
		 * For GPHY with in-band signaling, read the speed/duplex info from 
		 * IBS register instead of PHY register.
		 */
		int ibs = (txrx_ctrl->rgmii_ib_status) & 0x07;
		if(ibs2lpa[ibs] == 0)
		{
			printk(KERN_WARNING "No matching setup for ibs=0x%x\n", ibs);
			return 0;
		}
		for( i = 0; i < sizeof(g_mii_configs)/sizeof(MII_CONFIG); i++)
		{
			if (g_mii_configs[i].hcd & ibs2lpa[ibs])
			{
				ret = &g_mii_configs[i];
				break;
			}
		}
	}
	if(ret != NULL)
	{
		/* 
		 * For external GPHY(with or without inband signaling)
		 * read brcm aux status summer for pause resolution.
		 */
		//val = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_LPA);
    	val = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_BRCM_AUX_STAT_SUM);
		if(val & MII_BRCM_AUX_GPHY_RX_PAUSE)
			ret->pause |= RX_PAUSE;
		if(val & MII_BRCM_AUX_GPHY_TX_PAUSE)
			ret->pause |= TX_PAUSE;

		return ret;
	}
	/* if we are here, it means:
	 * 1. It's an internal PHY
	 * 2. It's an external 10/100 PHY.
	 * 3. It's an external GPHY, but link partner's is not capable of 1000Mbps.
	 */
	
    /* Read the Link Partner Ability */
    val = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_BRCM_AUX_STAT_SUM);
	TRACE(("Aux val=0x%x\n", val));
	for(i = 0; i < sizeof(g_mii_configs)/sizeof(MII_CONFIG); i++)
	{
		if(g_mii_configs[i].hcd & ((val >> MII_BRCM_AUX_AN_HCD_SHIFT)&MII_BRCM_AUX_AN_HCD_MASK))
		{
			ret = &g_mii_configs[i];
			break;
		}
	}
	if(ret != NULL)
	{
		val = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_LPA);
		if(val & LPA_PAUSE_CAP)
			ret->pause = TX_PAUSE | RX_PAUSE;
		return ret;
	}

    return ret;
}

/* Auto-Configure this MII interface */
static MII_CONFIG * mii_autoconfigure(struct net_device *dev)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    int i;
    int val;
	MII_CONFIG * config = NULL;

    TRACE(("mii_autoconfigure, PhyAddress=0x%x\n", pDevCtrl->EnetInfo.PhyAddress));
	/* TODO set advertise register */

    /* enable and restart autonegotiation */
    val = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_BMCR);
    val |= (BMCR_ANRESTART | BMCR_ANENABLE);
    mii_write(dev, pDevCtrl->EnetInfo.PhyAddress, MII_BMCR, val);

    /* wait for it to finish */
    for (i = 0; i < 400; i++) {
		if (pDevCtrl->mii_pid != 0)
		{
			/* we are in thread, we can go into sleep.*/
			msleep(10);
		}else
		{
        	mdelay(10);
		}
        val = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_BMSR);
#if defined(CONFIG_BCM5325_SWITCH) && (CONFIG_BCM5325_SWITCH == 1)
        if ((val & BMSR_ANEGCOMPLETE) || ((val & BMSR_LSTATUS) == 0)) { 
#else
        if (val & BMSR_ANEGCOMPLETE) {
#endif
            break;
        }
    }
	if(i == 400)
		TRACE(("Auto-nego timedout!\n"));
	else
		TRACE(("Auto-nego completed, BMSR=0x%x\n", val));

    config =  mii_getconfig(dev);
	if( (config != NULL) && (val & BMSR_ANEGCOMPLETE) )
	{
		config->auto_ne = 1;
	}
	return config;
}
/*
 * restart auto-negotiation, config UMAC and RGMII block
 */
void mii_setup(struct net_device *dev)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	volatile uniMacRegs * umac = pDevCtrl->umac;
	volatile rbufRegs * txrx_ctrl = pDevCtrl->txrx_ctrl;
    MII_CONFIG * eMiiConfig = NULL;

    eMiiConfig = mii_autoconfigure(dev);

	if ( !eMiiConfig || !eMiiConfig->auto_ne) {
		printk(KERN_WARNING " Auto-negotiation failed, forcing to default\n");
		eMiiConfig = &g_mii_configs[3];
	}
	printk("%s: %s\n", eMiiConfig->name, eMiiConfig->auto_ne == 1 ? "Auto-Neg":"Forced");

	/*
	 * program UMAC and RGMII block accordingly, if the PHY is 
	 * not capable of in-band signaling.
	 */
	umac->cmd &= ~CMD_HD_EN;
	if(pDevCtrl->EnetInfo.PhyType != BP_ENET_EXTERNAL_GPHY_IBS)
	{
		txrx_ctrl->rgmii_oob_ctrl &= ~OOB_DISABLE;
		txrx_ctrl->rgmii_oob_ctrl |= RGMII_LINK;
		/* pause control*/
		if((eMiiConfig->pause & RX_PAUSE) == 0)
			umac->cmd |= CMD_RX_PAUSE_IGNORE;
			//umac->pause_ctrl |= PAUSE_CTRL_EN;
		else if ((eMiiConfig->pause & TX_PAUSE) == 0)
			umac->cmd |= CMD_TX_PAUSE_IGNORE;
			//umac->pause_ctrl &= ~PAUSE_CTRL_EN;
		/* duplex.*/
		if(eMiiConfig->hcd == MII_BRCM_AUX_AN_HCD_10T ||
		   eMiiConfig->hcd == MII_BRCM_AUX_AN_HCD_100TX ||
		   eMiiConfig->hcd == LPA_1000HALF)
		{
			umac->cmd |= CMD_HD_EN;
		}
		/* speed */
		umac->cmd = umac->cmd & ~(CMD_SPEED_MASK << CMD_SPEED_SHIFT);
		if(eMiiConfig->hcd == MII_BRCM_AUX_AN_HCD_10T || eMiiConfig->hcd == MII_BRCM_AUX_AN_HCD_10T_FULL)
			umac->cmd = umac->cmd  | (UMAC_SPEED_10 << CMD_SPEED_SHIFT);
		else if (eMiiConfig->hcd == MII_BRCM_AUX_AN_HCD_100TX || eMiiConfig->hcd == MII_BRCM_AUX_AN_HCD_100TX_FULL)
			umac->cmd = umac->cmd  | (UMAC_SPEED_100 << CMD_SPEED_SHIFT);
		else if (eMiiConfig->hcd == LPA_1000HALF || eMiiConfig->hcd == LPA_1000FULL)
			umac->cmd = umac->cmd  | (UMAC_SPEED_1000 << CMD_SPEED_SHIFT);

		printk(KERN_WARNING "Link Parameters: %s\n", eMiiConfig->name);
	}
#ifdef PHY_LOOPBACK
    /* Enable PHY loopback */
    mii_loopback(dev);
#endif

}

/* reset the MII */
static void mii_soft_reset(struct net_device *dev, int PhyAddr) 
{
    int val;

    mii_write(dev, PhyAddr, MII_BMCR, BMCR_RESET);
    udelay(10); /* wait ~10usec */
    do {
        val = mii_read(dev, PhyAddr, MII_BMCR);
    } while (val & BMCR_RESET);

}

int mii_init(struct net_device *dev)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    volatile uniMacRegs *umac;
	volatile rbufRegs * txrx_ctrl;
    int data32, advertise;

    printk(KERN_INFO "Config %s Through MDIO\n", gsPhyType[pDevCtrl->EnetInfo.PhyType] );

    umac = pDevCtrl->umac;
	txrx_ctrl = pDevCtrl->txrx_ctrl;
    switch(pDevCtrl->EnetInfo.PhyType) {

        case BP_ENET_INTERNAL_PHY:
		case BP_ENET_EXTERNAL_PHY:
            /* do we need to set mii clock? do soft reset of phy, default is 10Base-T */
            /* reset phy */
            mii_soft_reset(dev, pDevCtrl->EnetInfo.PhyAddress);
            mii_setup(dev);
            break;
        case BP_ENET_EXTERNAL_GPHY:
			txrx_ctrl->rgmii_oob_ctrl |= RGMII_MODE_EN;
			txrx_ctrl->rgmii_oob_ctrl |= (1 << 16);	/* Don't shift tx clock by 90 degree */
			/* Find out if the external PHY is really GPHY, advertise 1000 ability if it is*/
			data32 = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_BMSR);
			if (data32 & BMSR_ERCAP) {
				data32 = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_ESTATUS);
				advertise = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_CTRL1000);
				
				if (data32 & ESTATUS_1000_TFULL)
					advertise |= ADVERTISE_1000FULL;
				else if (data32 & ESTATUS_1000_THALF)
					advertise |= ADVERTISE_1000HALF;

				mii_write(dev, pDevCtrl->EnetInfo.PhyAddress, MII_CTRL1000, advertise);
			}
            mii_setup(dev);
            break;
		case BP_ENET_EXTERNAL_GPHY_IBS:
			txrx_ctrl->rgmii_oob_ctrl |= RGMII_MODE_EN;
			txrx_ctrl->rgmii_oob_ctrl |= (1 << 16);
			/* Use in-band signaling for auto config.*/
			txrx_ctrl->rgmii_oob_ctrl |= OOB_DISABLE;
			umac->cmd |= CMD_AUTO_CONFIG;
			/* Advertise 1000Base capability, neccesary?*/
			data32 = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_BMSR);
			if (data32 & BMSR_ERCAP) {
				data32 = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_ESTATUS);
				advertise = mii_read(dev, pDevCtrl->EnetInfo.PhyAddress, MII_CTRL1000);
				
				if (data32 & ESTATUS_1000_TFULL)
					advertise |= ADVERTISE_1000FULL;
				else if (data32 & ESTATUS_1000_THALF)
					advertise |= ADVERTISE_1000HALF;

				mii_write(dev, pDevCtrl->EnetInfo.PhyAddress, MII_CTRL1000, advertise);
			}
            mii_setup(dev);
            break;
		case BP_ENET_EXTERNAL_MOCA:
			umac->cmd = umac->cmd  | (UMAC_SPEED_1000 << CMD_SPEED_SHIFT);
			break;
        default:
            break;
    }

    return 0;
}
// End of file
