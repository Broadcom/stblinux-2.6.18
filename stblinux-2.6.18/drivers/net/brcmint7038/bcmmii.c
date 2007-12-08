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
// Description: Broadcom PHY/Ethernet Switch Configuration
//               
//**************************************************************************

#include <linux/types.h>
#include <linux/delay.h>
#include <linux/mii.h>
#include <linux/stddef.h>
#include <linux/ctype.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include "boardparms.h"
#include "intemac_map.h"
#include "bcmemac.h"
#include "bcmmii.h"

typedef enum {
    MII_100MBIT     = 0x0001,
    MII_FULLDUPLEX  = 0x0002,
    MII_AUTONEG     = 0x0004,
}   MII_CONFIG;

#define EMAC_MDC            0x1f

/* local prototypes */
static MII_CONFIG mii_getconfig(struct net_device *dev);
static MII_CONFIG mii_autoconfigure(struct net_device *dev);
static void mii_setup(struct net_device *dev);
static void mii_soft_reset(struct net_device *dev, int PhyAddr);
static void ethsw_mdio_rreg(struct net_device *dev, int page, int reg, uint8 *data, int len);
static void ethsw_mdio_wreg(struct net_device *dev, int page, int reg, uint8 *data, int len);
static void ethsw_rreg(struct net_device *dev, int page, int reg, uint8 *data, int len);
static void ethsw_wreg(struct net_device *dev, int page, int reg, uint8 *data, int len);
static int ethsw_switch_type(struct net_device *dev);
static int ethsw_set_rmii_mode(struct net_device *dev);
static void str_to_num(char *in, char *out, int len);
static int proc_get_param(char *page, char **start, off_t off, int cnt, int *eof, void *data);
static int proc_set_param(struct file *f, const char *buf, unsigned long cnt, void *data);

#ifdef PHY_LOOPBACK
static void mii_loopback(struct net_device *dev);
#endif

/* read a value from the MII */
int mii_read(struct net_device *dev, int phy_id, int location) 
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    volatile EmacRegisters *emac = pDevCtrl->emac;

    emac->mdioData = MDIO_RD | (phy_id << MDIO_PMD_SHIFT) | (location << MDIO_REG_SHIFT);

    while ( ! (emac->intStatus & EMAC_MDIO_INT) )
        ;
    emac->intStatus |= EMAC_MDIO_INT;
    return emac->mdioData & 0xffff;
}

/* write a value to the MII */
void mii_write(struct net_device *dev, int phy_id, int location, int val)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    volatile EmacRegisters *emac = pDevCtrl->emac;
    uint32 data;

    data = MDIO_WR | (phy_id << MDIO_PMD_SHIFT) | (location << MDIO_REG_SHIFT) | val;
    emac->mdioData = data;

    while ( ! (emac->intStatus & EMAC_MDIO_INT) )
        ;
    emac->intStatus |= EMAC_MDIO_INT;
}

#ifdef PHY_LOOPBACK
/* set the MII loopback mode */
static void mii_loopback(struct net_device *dev)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    uint32 val;

    TRACE(("mii_loopback\n"));

    val = mii_read(dev, pDevCtrl->EnetInfo.ucPhyAddress, MII_BMCR);
    /* Disable autonegotiation */
    val &= ~BMCR_ANENABLE;
    /* Enable Loopback */
    val |= BMCR_LOOPBACK;
    mii_write(dev, pDevCtrl->EnetInfo.ucPhyAddress, MII_BMCR, val);
}
#endif

int mii_linkstatus(struct net_device *dev, int phy_id)
{
    int val;

    val = mii_read(dev, phy_id, MII_BMSR);
    /* reread: the link status bit is sticky */
    val = mii_read(dev, phy_id, MII_BMSR);

    if (val & BMSR_LSTATUS)
        return 1;
    else
        return 0;
}

void mii_linkstatus_start(struct net_device *dev, int phy_id)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    volatile EmacRegisters *emac = pDevCtrl->emac;
    int location = MII_BMSR;

    emac->mdioData = MDIO_RD | (phy_id << MDIO_PMD_SHIFT) |
        (location << MDIO_REG_SHIFT);
}

int mii_linkstatus_check(struct net_device *dev, int *up)
{
    int done = 0;
    int val;
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    volatile EmacRegisters *emac = pDevCtrl->emac;

    if( emac->intStatus & EMAC_MDIO_INT ) {

        emac->intStatus |= EMAC_MDIO_INT;
        val = emac->mdioData & 0xffff;
        *up = ((val & BMSR_LSTATUS) == BMSR_LSTATUS) ? 1 : 0;
        done = 1;
    }

    return( done );
}

void mii_enablephyinterrupt(struct net_device *dev, int phy_id)
{
    mii_write(dev, phy_id, MII_INTERRUPT, 
        MII_INTR_ENABLE | MII_INTR_MASK_FDX | MII_INTR_MASK_LINK_SPEED);
}

void mii_clearphyinterrupt(struct net_device *dev, int phy_id)
{
    mii_read(dev, phy_id, MII_INTERRUPT);
}

/* return the current MII configuration */
static MII_CONFIG mii_getconfig(struct net_device *dev)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    uint32 val;
    MII_CONFIG eConfig = 0;

    TRACE(("mii_getconfig\n"));

    /* Read the Link Partner Ability */
    val = mii_read(dev, pDevCtrl->EnetInfo.ucPhyAddress, MII_LPA);
    if (val & LPA_100FULL) {          /* 100 MB Full-Duplex */
        eConfig = (MII_100MBIT | MII_FULLDUPLEX);
    } else if (val & LPA_100HALF) {   /* 100 MB Half-Duplex */
        eConfig = MII_100MBIT;
    } else if (val & LPA_10FULL) {    /* 10 MB Full-Duplex */
        eConfig = MII_FULLDUPLEX;
    } 

    return eConfig;
}

/* Auto-Configure this MII interface */
static MII_CONFIG mii_autoconfigure(struct net_device *dev)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    int i;
    int val;
    MII_CONFIG eConfig;

    TRACE(("mii_autoconfigure\n"));

    /* enable and restart autonegotiation */
    val = mii_read(dev, pDevCtrl->EnetInfo.ucPhyAddress, MII_BMCR);
    val |= (BMCR_ANRESTART | BMCR_ANENABLE);
    mii_write(dev, pDevCtrl->EnetInfo.ucPhyAddress, MII_BMCR, val);

    /* wait for it to finish */
    for (i = 0; i < 1000; i++) {
        mdelay(1);
        val = mii_read(dev, pDevCtrl->EnetInfo.ucPhyAddress, MII_BMSR);
#if defined(CONFIG_BCM5325_SWITCH) && (CONFIG_BCM5325_SWITCH == 1)
        if ((val & BMSR_ANEGCOMPLETE) || ((val & BMSR_LSTATUS) == 0)) { 
#else
        if (val & BMSR_ANEGCOMPLETE) {
#endif
            break;
        }
    }

    eConfig = mii_getconfig(dev);
    if (val & BMSR_ANEGCOMPLETE) {
        eConfig |= MII_AUTONEG;
    } 

    return eConfig;
}

static void mii_setup(struct net_device *dev)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    MII_CONFIG eMiiConfig;

    eMiiConfig = mii_autoconfigure(dev);

    if (! (eMiiConfig & MII_AUTONEG)) {
        printk(KERN_INFO ": Auto-negotiation timed-out\n");
    }

    if ((eMiiConfig & (MII_100MBIT | MII_FULLDUPLEX)) == (MII_100MBIT | MII_FULLDUPLEX)) {
        printk(KERN_INFO ": 100 MB Full-Duplex (auto-neg)\n");
    } else if (eMiiConfig & MII_100MBIT) {
        printk(KERN_INFO ": 100 MB Half-Duplex (auto-neg)\n");
    } else if (eMiiConfig & MII_FULLDUPLEX) {
        printk(KERN_INFO ": 10 MB Full-Duplex (auto-neg)\n");
    } else {
        printk(KERN_INFO ": 10 MB Half-Duplex (assumed)\n");
    }

#ifdef PHY_LOOPBACK
    /* Enable PHY loopback */
    mii_loopback(dev);
#endif

    /* Check for FULL/HALF duplex */
    if (eMiiConfig & MII_FULLDUPLEX) {
        pDevCtrl->emac->txControl = EMAC_FD;
    }
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

/* BCM5325E PSEUDO PHY register access through MDC/MDIO */

/* When reading or writing PSEUDO PHY registers, we must use the exact starting address and
   exact length for each register as defined in the data sheet.  In other words, for example,
   dividing a 32-bit register read into two 16-bit reads will produce wrong result.  Neither
   can we start read/write from the middle of a register.  Yet another bad example is trying
   to read a 32-bit register as a 48-bit one.  This is very important!!
*/

static void ethsw_mdio_rreg(struct net_device *dev, int page, int reg, uint8 *data, int len)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    volatile EmacRegisters *emac; 
    int v;
    int i;

    if (((len != 1) && (len % 2) != 0) || len > 8)
        panic("ethsw_mdio_rreg: wrong length!\n");

    emac = pDevCtrl->emac;

    v = (page << REG_PPM_REG16_SWITCH_PAGE_NUMBER_SHIFT) | REG_PPM_REG16_MDIO_ENABLE;
    mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG16, v);

    v = (reg << REG_PPM_REG17_REG_NUMBER_SHIFT) | REG_PPM_REG17_OP_READ;
    mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG17, v);

    for (i = 0; i < 5; i++)
    {
        v = mii_read(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG17);

        if ((v & (REG_PPM_REG17_OP_WRITE | REG_PPM_REG17_OP_READ)) == REG_PPM_REG17_OP_DONE)
            break;

        udelay(10);
    }

    if (i == 5)
    {
        printk("ethsw_mdio_rreg: timeout!\n");
        return;
    }

    switch (len) 
    {
        case 1:
            v = mii_read(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24);
            data[0] = (uint8)v;
            break;
        case 2:
            v = mii_read(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24);
            ((uint16 *)data)[0] = (uint16)v;
            break;
        case 4:
            v = mii_read(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG25);
            ((uint16 *)data)[0] = (uint16)v;
            v = mii_read(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24);
            ((uint16 *)data)[1] = (uint16)v;
            break;
        case 6:
            v = mii_read(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG26);
            ((uint16 *)data)[0] = (uint16)v;
            v = mii_read(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG25);
            ((uint16 *)data)[1] = (uint16)v;
            v = mii_read(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24);
            ((uint16 *)data)[2] = (uint16)v;
            break;
        case 8:
            v = mii_read(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG27);
            ((uint16 *)data)[0] = (uint16)v;
            v = mii_read(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG26);
            ((uint16 *)data)[1] = (uint16)v;
            v = mii_read(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG25);
            ((uint16 *)data)[2] = (uint16)v;
            v = mii_read(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24);
            ((uint16 *)data)[3] = (uint16)v;
            break;
    }
}

static void ethsw_mdio_wreg(struct net_device *dev, int page, int reg, uint8 *data, int len)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    volatile EmacRegisters *emac; 
    int v;
    int i;

    if (((len != 1) && (len % 2) != 0) || len > 8)
        panic("ethsw_mdio_wreg: wrong length!\n");

    emac = pDevCtrl->emac;

    v = (page << REG_PPM_REG16_SWITCH_PAGE_NUMBER_SHIFT) | REG_PPM_REG16_MDIO_ENABLE;
    mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG16, v);

    switch (len) 
    {
        case 1:
            v = data[0];
            mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24, v);
            break;
        case 2:
            v = ((uint16 *)data)[0];
            mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24, v);
            break;
        case 4:
            v = ((uint16 *)data)[0];
            mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG25, v);
            v = ((uint16 *)data)[1];
            mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24, v);
            break;
        case 6:
            v = ((uint16 *)data)[0];
            mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG26, v);
            v = ((uint16 *)data)[1];
            mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG25, v);
            v = ((uint16 *)data)[2];
            mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24, v);
            break;
        case 8:
            v = ((uint16 *)data)[0];
            mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG27, v);
            v = ((uint16 *)data)[1];
            mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG26, v);
            v = ((uint16 *)data)[2];
            mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG25, v);
            v = ((uint16 *)data)[3];
            mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG24, v);
            break;
    }

    v = (reg << REG_PPM_REG17_REG_NUMBER_SHIFT) | REG_PPM_REG17_OP_WRITE;
    mii_write(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG17, v);

    for (i = 0; i < 5; i++)
    {
        v = mii_read(dev, PSEUDO_PHY_ADDR, REG_PSEUDO_PHY_MII_REG17);

        if ((v & (REG_PPM_REG17_OP_WRITE | REG_PPM_REG17_OP_READ)) == REG_PPM_REG17_OP_DONE)
            break;

        udelay(10);
    }

    if (i == 5)
        printk("ethsw_mdio_wreg: timeout!\n");
}

static void ethsw_rreg(struct net_device *dev, int page, int reg, uint8 *data, int len)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);

    switch (pDevCtrl->EnetInfo.usConfigType) {
        case BP_ENET_CONFIG_MDIO_PSEUDO_PHY:
            ethsw_mdio_rreg(dev, page, reg, data, len);
            break;
        default:
            break;
    }
}

static void ethsw_wreg(struct net_device *dev, int page, int reg, uint8 *data, int len)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);

    switch (pDevCtrl->EnetInfo.usConfigType) {
        case BP_ENET_CONFIG_MDIO_PSEUDO_PHY:
            ethsw_mdio_wreg(dev, page, reg, data, len);
            break;

        default:
            break;
    }
}

/* Configure the switch for VLAN support */
static int ethsw_switch_type(struct net_device *dev)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    uint8 val8   = 0;
    int type;

    if ( pDevCtrl->EnetInfo.ucPhyType != BP_ENET_EXTERNAL_SWITCH ||
        (pDevCtrl->EnetInfo.usConfigType != BP_ENET_CONFIG_MDIO_PSEUDO_PHY) )
         return ESW_TYPE_UNDEFINED;

    /*
     * Determine if the external PHY is connected to a 5325E, 5325F or 5325M
     * read CTRL REG 4 if the default value is 
     * 1 - 5325E
     * 3 - 5325F
     * else - 5325M
     */ 
    ethsw_rreg(dev, PAGE_MANAGEMENT, REG_DEV_ID, &val8, sizeof(val8));

    if (val8 == 0x97)
        return ESW_TYPE_BCM5397;

    ethsw_rreg(dev, PAGE_VLAN, REG_VLAN_CTRL4, (char*)&val8, sizeof(val8));

    switch (val8)
    {
    case 1: /* BCM5325E */
        type = ESW_TYPE_BCM5325E;
        break;
    case 3: /* BCM5325F */
        type = ESW_TYPE_BCM5325F;
        break;
    default: /* default to BCM5325M */
        type = ESW_TYPE_BCM5325M;
    }
    return type;
}

/* Configure the switch for VLAN support */
int ethsw_config_vlan(struct net_device *dev, int enable, unsigned int vid)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    uint8 val8   = 0;
    uint16 val16 = 0;
    uint32 val32 = 0;
    uint port;
    int reg_addr;
    int max_retry = 0;

    switch(pDevCtrl->ethSwitch.type) {
        case ESW_TYPE_BCM5397:
        case ESW_TYPE_BCM5325E:
        case ESW_TYPE_BCM5325F:
            break;
        default:
            return -EFAULT;
    }

    /* Enable 802.1Q VLAN and Individual VLAN learning mode */
    if (enable) {
        val8 = REG_VLAN_CTRL0_ENABLE_1Q | REG_VLAN_CTRL0_IVLM;
    }
    else
    {
        val8 = (uint8) ~REG_VLAN_CTRL0_ENABLE_1Q;
    }
    ethsw_wreg(dev, PAGE_VLAN, REG_VLAN_CTRL0, (uint8 *)&val8, sizeof(val8));

    /* Enable high order 8 bit VLAN table check and drop non .1Q frames for MII port */
    val8 = REG_VLAN_CTRL3_8BIT_CHECK | REG_VLAN_CTRL3_MII_DROP_NON_1Q;
    ethsw_wreg(dev, PAGE_VLAN, REG_VLAN_CTRL3, (uint8 *)&val8, sizeof(val8));

    /* Drop V_table miss frames */
    val8 = REG_VLAN_CTRL5_DROP_VTAB_MISS;
    val8 |= REG_VLAN_CTRL5_ENBL_MANAGE_RX_BYPASS;
    val8 |= REG_VLAN_CTRL5_ENBL_CRC_GEN;

    reg_addr = (pDevCtrl->ethSwitch.type == ESW_TYPE_BCM5397)? REG_VLAN_CTRL5_5397: REG_VLAN_CTRL5;
    ethsw_wreg(dev, PAGE_VLAN, reg_addr, (uint8 *)&val8, sizeof(val8));

    for (port = 0; port < 5; port++) {
        /* default tag for every port */
        val16 = vid + port;
        ethsw_wreg(dev, PAGE_VLAN, REG_VLAN_PTAG0 + port * sizeof(uint16), (uint8 *)&val16, sizeof(val16));

        /* Program VLAN table for each VLAN */
        val32 = ((1 << MII_PORT_FOR_VLAN) | (1 << port)) << REG_VLAN_GROUP_SHIFT;
        val32 |= (1 << port) << REG_VLAN_UNTAG_SHIFT;
        val32 |= REG_VLAN_WRITE_VALID;
        ethsw_wreg(dev, PAGE_VLAN, REG_VLAN_WRITE, (uint8 *)&val32, sizeof(val32));
        val16 = REG_VLAN_ACCESS_START_DONE | REG_VLAN_ACCESS_WRITE_STATE | (vid + port);
        ethsw_wreg(dev, PAGE_VLAN, REG_VLAN_ACCESS, (uint8 *)&val16, sizeof(val16));
        do {
            ethsw_rreg(dev, PAGE_VLAN, REG_VLAN_ACCESS, (char*)&val16, sizeof(val16));
            udelay(10);
        } while ((max_retry++ < 5) && (val16 & REG_VLAN_ACCESS_START_DONE));
    }

    return 0;
}

void ethsw_switch_power_off(void *context)
{
    struct net_device *dev = (struct net_device *)context;
    
    int switch_type = ((BcmEnet_devctrl *) netdev_priv(dev))->ethSwitch.type;
    int phy_addr    = ((BcmEnet_devctrl *) netdev_priv(dev))->EnetInfo.ucPhyAddress;

    if (switch_type == ESW_TYPE_BCM5397)
    {
      mii_write(dev, phy_addr + 0, MII_BMCR, BMCR_PDOWN);
      mii_write(dev, phy_addr + 1, MII_BMCR, BMCR_PDOWN);
      mii_write(dev, phy_addr + 2, MII_BMCR, BMCR_PDOWN);
      mii_write(dev, phy_addr + 3, MII_BMCR, BMCR_PDOWN);
      mii_write(dev, phy_addr + 4, MII_BMCR, BMCR_PDOWN);
    }
    else
    {
      uint8 ports = REG_POWER_DOWN_MODE_ALL;
      ethsw_wreg(dev, PAGE_CONTROL, REG_POWER_DOWN_MODE, &ports, sizeof(ports));
    }
}

void ethsw_add_static_addr(struct net_device *dev, uint8 *mac, uint8 flag)
{
    struct arl_entry {
        uint16 flags_port;
        uint8 mac[6];
    } __attribute__((packed));

    struct arl_entry e;
    uint8  v8;

    e.flags_port = (flag != 0) ? (flag << 8) | 0x0008: 0;
    memmove(e.mac, mac, 6);

    ethsw_wreg(dev, PAGE_ARL_ACCESS, REG_ARL_ACCESS_ARL_ENTRY0, (uint8 *)&e, sizeof(e));
    ethsw_wreg(dev, PAGE_ARL_ACCESS, REG_ARL_ACCESS_ARL_ENTRY1, (uint8 *)&e, sizeof(e));
    ethsw_wreg(dev, PAGE_ARL_ACCESS, REG_ARL_ACCESS_ADDR_INDEX, mac, 6);

    v8 = REG_ARL_CONTROL_START;
    ethsw_wreg(dev, PAGE_ARL_ACCESS, REG_ARL_ACCESS_CONTROL, (uint8 *)&v8, sizeof(v8));

    while (v8 & REG_ARL_CONTROL_START)
        ethsw_rreg(dev, PAGE_ARL_ACCESS, REG_ARL_ACCESS_CONTROL, (uint8 *)&v8, sizeof(v8));
}

#if 0
/* This routine dumps out ARL entries to a buffer.  It can be easily added to
   PROC file system.
*/

int ethsw_dump_static_addr(struct net_device *dev, uint8 *mac, char *buf)
{
    struct arl_entry {
        uint16 flags_port;
        uint8 mac[6];
    } __attribute__((packed));

    struct arl_entry e0, e1;
    uint8  v8;
    int r;

    ethsw_wreg(dev, PAGE_ARL_ACCESS, REG_ARL_ACCESS_ADDR_INDEX, mac, 6);

    v8 = REG_ARL_CONTROL_START | REG_ARL_CONTROL_READ;
    ethsw_wreg(dev, PAGE_ARL_ACCESS, REG_ARL_ACCESS_CONTROL, (uint8 *)&v8, sizeof(v8));

    while (v8 & REG_ARL_CONTROL_START)
        ethsw_rreg(dev, PAGE_ARL_ACCESS, REG_ARL_ACCESS_CONTROL, (uint8 *)&v8, sizeof(v8));

    memset(&e0, 0, sizeof(e0));
    memset(&e1, 0, sizeof(e1));
    ethsw_rreg(dev, PAGE_ARL_ACCESS, REG_ARL_ACCESS_ARL_ENTRY0, (uint8 *)&e0, sizeof(e0));
    ethsw_rreg(dev, PAGE_ARL_ACCESS, REG_ARL_ACCESS_ARL_ENTRY1, (uint8 *)&e1, sizeof(e1));

    r = sprintf(buf, "e0: flag %04x, %02x:%02x:%02x:%02x:%02x:%02x\n", 
        e0.flags_port, e0.mac[0], e0.mac[1], e0.mac[2], e0.mac[3], e0.mac[4], e0.mac[5]);
    r += sprintf(buf + r, "e1: flag %04x, %02x:%02x:%02x:%02x:%02x:%02x\n", 
        e1.flags_port, e1.mac[0], e1.mac[1], e1.mac[2], e1.mac[3], e1.mac[4], e1.mac[5]);

    return r;
}
#endif

void ethsw_switch_frame_manage_mode(struct net_device *dev)
{
    int switch_type = ((BcmEnet_devctrl *) netdev_priv(dev))->ethSwitch.type;
    uint8  v8;
    uint16 v16;
    uint8 mac[6];
    int i;
    int reg_addr;

    // Put switch in frame management mode

    ethsw_rreg(dev, PAGE_CONTROL, REG_SWITCH_MODE, &v8, sizeof(v8));
    v8 |= REG_SWITCH_MODE_FRAME_MANAGE_MODE;
    v8 |= REG_SWITCH_MODE_SW_FWDG_EN;
    ethsw_wreg(dev, PAGE_CONTROL, REG_SWITCH_MODE, &v8, sizeof(v8));

    // MII port

    v8 = 0xa0;
    v8 |= REG_MII_PORT_CONTROL_RX_UCST_EN;
    v8 |= REG_MII_PORT_CONTROL_RX_MCST_EN;
    v8 |= REG_MII_PORT_CONTROL_RX_BCST_EN;
    ethsw_wreg(dev, PAGE_CONTROL, REG_MII_PORT_CONTROL, &v8, sizeof(v8));

    // 10/100 ports

    v8 = 0xa0;
    ethsw_wreg(dev, PAGE_CONTROL, 0x00, &v8, sizeof(v8));
    ethsw_wreg(dev, PAGE_CONTROL, 0x01, &v8, sizeof(v8));
    ethsw_wreg(dev, PAGE_CONTROL, 0x02, &v8, sizeof(v8));
    ethsw_wreg(dev, PAGE_CONTROL, 0x03, &v8, sizeof(v8));
    ethsw_wreg(dev, PAGE_CONTROL, 0x04, &v8, sizeof(v8));

    // Management port configuration

    v8 = ENABLE_MII_PORT | RECEIVE_IGMP | RECEIVE_BPDU;
    ethsw_wreg(dev, PAGE_MANAGEMENT, REG_GLOBAL_CONFIG, &v8, sizeof(v8));

    // Recreate FCS

    reg_addr = (switch_type == ESW_TYPE_BCM5397) ? REG_VLAN_CTRL5_5397: REG_VLAN_CTRL5;

    ethsw_rreg(dev, PAGE_VLAN, reg_addr, &v8, sizeof(v8));
    v8 |= REG_VLAN_CTRL5_ENBL_MANAGE_RX_BYPASS;
    v8 |= REG_VLAN_CTRL5_ENBL_CRC_GEN;
    ethsw_wreg(dev, PAGE_VLAN, reg_addr, &v8, sizeof(v8));

    // Setup port-based VLAN

    v16 = 0x0100; // port -> mii only
    ethsw_wreg(dev, PAGE_PORT_BASED_VLAN, 0x00, (uint8 *)&v16, sizeof(v16));
    ethsw_wreg(dev, PAGE_PORT_BASED_VLAN, 0x02, (uint8 *)&v16, sizeof(v16));
    ethsw_wreg(dev, PAGE_PORT_BASED_VLAN, 0x04, (uint8 *)&v16, sizeof(v16));
    ethsw_wreg(dev, PAGE_PORT_BASED_VLAN, 0x06, (uint8 *)&v16, sizeof(v16)); 
    ethsw_wreg(dev, PAGE_PORT_BASED_VLAN, 0x08, (uint8 *)&v16, sizeof(v16)); 

    v16 = 0xffff; // mii -> all ports
    ethsw_wreg(dev, PAGE_PORT_BASED_VLAN, 0x10, (uint8 *)&v16, sizeof(v16));

    if (switch_type == ESW_TYPE_BCM5397)
    {
        v16 = 0xffff;
        ethsw_wreg(dev, PAGE_CONTROL, REG_DISABLE_LEARNING, (uint8 *)&v16, sizeof(v16));
    }
    else
    {
        // Fill up all 512 buckets of entries

        v8 = 0x11;
        ethsw_wreg(dev, 0x04, 0x00, &v8, sizeof(v8));

        for (i = 0; i < 1024; i++)
        {
            mac[0] = (i + 0) & 0xff;
            mac[1] = (i + 1) & 0xff;
            mac[2] = (i + 3) & 0xff;
            mac[3] = (i + 5) & 0xff;
            mac[4] = (1024 - i) >> 8;
            mac[5] = (1024 - i) & 0xff;
            ethsw_add_static_addr(dev, mac, 0xc0);
        }
    }

    // Set GARP multiport addresses

    mac[0] = 0x01;
    mac[1] = 0x80;
    mac[2] = 0xc2;
    mac[3] = 0x00;
    mac[4] = 0x00;
    mac[5] = 0x20;
    v16 = 0x0100;

    ethsw_wreg(dev, 0x04, 0x10, mac, 6);
    ethsw_wreg(dev, 0x04, 0x16, (uint8 *)&v16, sizeof(v16));

    mac[5] = 0x21;
    ethsw_wreg(dev, 0x04, 0x20, mac, 6);
    ethsw_wreg(dev, 0x04, 0x26, (uint8 *)&v16, sizeof(v16));
}

void ethsw_switch_unmanage_mode(struct net_device *dev)
{
    int switch_type = ((BcmEnet_devctrl *) netdev_priv(dev))->ethSwitch.type;
    uint8  v8;
    uint16 v16;
    uint8 mac[8];
    int i;

    ethsw_rreg(dev, PAGE_CONTROL, REG_SWITCH_MODE, &v8, sizeof(v8));
    v8 &= ~REG_SWITCH_MODE_FRAME_MANAGE_MODE;
    v8 |= REG_SWITCH_MODE_SW_FWDG_EN;
    ethsw_wreg(dev, PAGE_CONTROL, REG_SWITCH_MODE, &v8, sizeof(v8));

    v8 = 0;
    ethsw_wreg(dev, PAGE_MANAGEMENT, REG_GLOBAL_CONFIG, &v8, sizeof(v8));

    v16 = 0x01ff;
    ethsw_wreg(dev, PAGE_PORT_BASED_VLAN, 0x00, (uint8 *)&v16, sizeof(v16));
    ethsw_wreg(dev, PAGE_PORT_BASED_VLAN, 0x02, (uint8 *)&v16, sizeof(v16));
    ethsw_wreg(dev, PAGE_PORT_BASED_VLAN, 0x04, (uint8 *)&v16, sizeof(v16));
    ethsw_wreg(dev, PAGE_PORT_BASED_VLAN, 0x06, (uint8 *)&v16, sizeof(v16)); 
    ethsw_wreg(dev, PAGE_PORT_BASED_VLAN, 0x08, (uint8 *)&v16, sizeof(v16)); 
    ethsw_wreg(dev, PAGE_PORT_BASED_VLAN, 0x10, (uint8 *)&v16, sizeof(v16));

    if (switch_type == ESW_TYPE_BCM5397)
    {
        v16 = 0;
        ethsw_wreg(dev, PAGE_CONTROL, REG_DISABLE_LEARNING, (uint8 *)&v16, sizeof(v16));
    }
    else
    {
        // Clear all 512 buckets of entries

        v8 = 0x01;
        ethsw_wreg(dev, 0x04, 0x00, &v8, sizeof(v8));

        for (i = 0; i < 1024; i++)
        {
            mac[0] = (i + 0) & 0xff;
            mac[1] = (i + 1) & 0xff;
            mac[2] = (i + 3) & 0xff;
            mac[3] = (i + 5) & 0xff;
            mac[4] = (1024 - i) >> 8;
            mac[5] = (1024 - i) & 0xff;
            ethsw_add_static_addr(dev, mac, 0x00);
        }

        v8 = 0x00;
        ethsw_wreg(dev, 0x04, 0x00, &v8, sizeof(v8));
    }
}

static int ethsw_set_rmii_mode(struct net_device *dev)
{ 
    uint8 v8;
    int switch_type = ((BcmEnet_devctrl *) netdev_priv(dev))->ethSwitch.type;

    ethsw_rreg(dev, PAGE_CONTROL, REG_CONTROL_MII1_PORT_STATE_OVERRIDE, &v8, 1);

    if (switch_type == ESW_TYPE_BCM5397)
      v8 |= REG_CONTROL_MPSO_OVERRIDE_ALL_5397;
    else
      v8 |= REG_CONTROL_MPSO_OVERRIDE_ALL;

    ethsw_wreg(dev, PAGE_CONTROL, REG_CONTROL_MII1_PORT_STATE_OVERRIDE, &v8, 1);

    v8 = 0;
    ethsw_rreg(dev, PAGE_CONTROL, REG_CONTROL_MII1_PORT_STATE_OVERRIDE, &v8, 1);
    return (v8 != 0xff)? 0: -1;
}

int mii_init(struct net_device *dev)
{
    BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
    volatile EmacRegisters *emac;
    int data32;
    uint8 data8;
    int i;
    char *phytype = "";
    char *setup = "";

    switch(pDevCtrl->EnetInfo.ucPhyType) {
        case BP_ENET_INTERNAL_PHY:
            phytype = "Internal PHY";
            break;

        case BP_ENET_EXTERNAL_PHY:
            phytype = "External PHY";
            break;

        case BP_ENET_EXTERNAL_SWITCH:
            phytype = "Ethernet Switch";
            break;

        default:
            printk(KERN_INFO ": Unknown PHY type\n");
            return -1;
    }
    switch (pDevCtrl->EnetInfo.usConfigType) {
        case BP_ENET_CONFIG_MDIO_PSEUDO_PHY:
            setup = "MDIO Pseudo PHY Interface";
            break;

        case BP_ENET_CONFIG_MDIO:
            setup = "MDIO";
            break;

        default:
            setup = "Undefined Interface";
            break;
    }
    printk("Config %s Through %s\n", phytype, setup);

    emac = pDevCtrl->emac;
    switch(pDevCtrl->EnetInfo.ucPhyType) {

        case BP_ENET_INTERNAL_PHY:
            /* init mii clock, do soft reset of phy, default is 10Base-T */
            emac->mdioFreq = EMAC_MII_PRE_EN | EMAC_MDC;
            /* reset phy */
            mii_soft_reset(dev, pDevCtrl->EnetInfo.ucPhyAddress);
            mii_setup(dev);
            break;

        case BP_ENET_EXTERNAL_PHY:
            emac->config |= EMAC_EXT_PHY;
            emac->mdioFreq = EMAC_MII_PRE_EN | EMAC_MDC;
            /* reset phy */
            mii_soft_reset(dev, pDevCtrl->EnetInfo.ucPhyAddress);

            data32 = mii_read(dev, pDevCtrl->EnetInfo.ucPhyAddress, MII_ADVERTISE);
            data32 |= ADVERTISE_FDFC; /* advertise flow control capbility */
            mii_write(dev, pDevCtrl->EnetInfo.ucPhyAddress, MII_ADVERTISE, data32);

            mii_setup(dev);
            break;

        case BP_ENET_EXTERNAL_SWITCH:
            emac->config |= EMAC_EXT_PHY;
            emac->mdioFreq = EMAC_MII_PRE_EN | EMAC_MDC;
            emac->txControl = EMAC_FD;
            switch (pDevCtrl->EnetInfo.usConfigType) {
                case BP_ENET_CONFIG_MDIO_PSEUDO_PHY:
                    mii_soft_reset(dev, PSEUDO_PHY_ADDR);
                    break;

                case BP_ENET_CONFIG_MDIO:
                    /* reset phy */
                    if (pDevCtrl->EnetInfo.numSwitchPorts) {
                        for (i = 0; i < pDevCtrl->EnetInfo.numSwitchPorts; i++) {
                            mii_soft_reset(dev, pDevCtrl->EnetInfo.ucPhyAddress | i);
                        }
                    }
                    return 0;

                default:
                    printk(KERN_INFO ": Unknown PHY configuration type\n");
                    break;
            }

            pDevCtrl->ethSwitch.type = ethsw_switch_type(dev);

            if ((pDevCtrl->ethSwitch.type != ESW_TYPE_BCM5397) && (pDevCtrl->EnetInfo.numSwitchPorts != 0)) {
                data8 = 0;
                switch (pDevCtrl->EnetInfo.numSwitchPorts) {
                    case 5:
                        data8 |= REG_POWER_DOWN_MODE_PORT5_PHY_DISABLE;
                    case 4:
                        data8 |= REG_POWER_DOWN_MODE_PORT4_PHY_DISABLE;
                    case 3:
                        data8 |= REG_POWER_DOWN_MODE_PORT3_PHY_DISABLE;
                    case 2:
                        data8 |= REG_POWER_DOWN_MODE_PORT2_PHY_DISABLE;
                    case 1:
                        data8 |= REG_POWER_DOWN_MODE_PORT1_PHY_DISABLE;
                        break;
                    default:
                        break;
                }
                /* disable Switch port PHY */
                ethsw_wreg(dev, PAGE_CONTROL, REG_POWER_DOWN_MODE, (uint8 *)&data8, sizeof(data8));
                /* enable Switch port PHY */
                data8 &= ~data8;
                ethsw_wreg(dev, PAGE_CONTROL, REG_POWER_DOWN_MODE, (uint8 *)&data8, sizeof(data8));
            }

            /* Reset all ports to default no spanning tree state */

            data8 = 0;

            for (i = 0; i < pDevCtrl->EnetInfo.numSwitchPorts; i++)
                ethsw_wreg(dev, 0, i, &data8, 1);

            ethsw_wreg(dev, 0, 8, &data8, 1);

            /* set swtich rmii mode */
            if (((BcmEnet_devctrl *) netdev_priv(dev))->EnetInfo.usReverseMii == BP_ENET_REVERSE_MII)
            {
              if (ethsw_set_rmii_mode(dev) < 0)
              {
                printk("mii_init: failed to set switch mii interface!\n");
                return -1;
              }
            }
            break;

        default:
            break;
    }

    return 0;
}

static uint8 swdata[16];

int ethsw_add_proc_files(struct net_device *dev)
{
  struct proc_dir_entry *p;

  p = create_proc_entry("switch", 0644, NULL);

  if (p == NULL)
    return -1;

  memset(swdata, 0, sizeof(swdata));

  p->data        = dev;
  p->read_proc   = proc_get_param;
  p->write_proc  = proc_set_param;
  p->owner       = THIS_MODULE;
  return 0;
}

int ethsw_del_proc_files(void)
{
  remove_proc_entry("switch", NULL);
  return 0;
}

static void str_to_num(char* in, char* out, int len)
{
  int i;
  memset(out, 0, len);

  for (i = 0; i < len * 2; i ++)
  {
    if ((*in >= '0') && (*in <= '9'))
      *out += (*in - '0');
    else if ((*in >= 'a') && (*in <= 'f'))
      *out += (*in - 'a') + 10;
    else if ((*in >= 'A') && (*in <= 'F'))
      *out += (*in - 'A') + 10;
    else
      *out += 0;

    if ((i % 2) == 0)
      *out *= 16;
    else
      out ++;

    in ++;
  }
  return;
}

static int proc_get_param(char *page, char **start, off_t off, int cnt, int *eof, void *data)
{
  int reg_page  = swdata[0];
  int reg_addr  = swdata[1];
  int reg_len   = swdata[2];
  int i = 0;
  int r = 0;

  *eof = 1;

  if (reg_len == 0)
    return 0;

  ethsw_rreg((struct net_device *)data, reg_page, reg_addr, swdata + 3, reg_len);

  r += sprintf(page + r, "[%02x:%02x] = ", swdata[0], swdata[1]);
 
  for (i = 0; i < reg_len; i ++)
    r += sprintf(page + r, "%02x ", swdata[3 + i]);

  r += sprintf(page + r, "\n");
  return (r < cnt)? r: 0;
}

static int proc_set_param(struct file *f, const char *buf, unsigned long cnt, void *data)
{
  char input[32];
  int i = 0;
  int r = cnt;
  int num_of_octets;

  int reg_page;
  int reg_addr;
  int reg_len;

  if ((cnt > 32) || (copy_from_user(input, buf, cnt) != 0))
    return -EFAULT;

  for (i = 0; i < r; i ++)
  {
    if (!isxdigit(input[i]))
    {
      memmove(&input[i], &input[i + 1], r - i - 1);
      r --;
      i --;
    }
  }

  num_of_octets = r / 2;

  if (num_of_octets < 3) // page, addr, len
    return -EFAULT;

  str_to_num(input, swdata, num_of_octets);

  reg_page  = swdata[0];
  reg_addr  = swdata[1];
  reg_len   = swdata[2];

  if (((reg_len != 1) && (reg_len % 2) != 0) || reg_len > 8)
  {
    memset(swdata, 0, sizeof(swdata));
    return -EFAULT;
  }

  if ((num_of_octets > 3) && (num_of_octets != reg_len + 3))
  {
    memset(swdata, 0, sizeof(swdata));
    return -EFAULT;
  }

  if (num_of_octets > 3)
    ethsw_wreg((struct net_device *)data, reg_page, reg_addr, swdata + 3, reg_len);

  return cnt;
}

// End of file
