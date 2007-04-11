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
#ifndef _BCMMII_H_
#define _BCMMII_H_

#include <asm/brcmstb/common/bcmtypes.h>

/*---------------------------------------------------------------------*/
/* Ethernet Switch Type                                                */
/*---------------------------------------------------------------------*/
#define ESW_TYPE_UNDEFINED                  0
#define ESW_TYPE_BCM5325M                   1
#define ESW_TYPE_BCM5325E                   2
#define ESW_TYPE_BCM5325F                   3
#define ESW_TYPE_BCM5397                    4

/****************************************************************************
    Internal PHY registers
****************************************************************************/

#define MII_AUX_CTRL_STATUS                 0x18
#define MII_TX_CONTROL                      0x19
#define MII_INTERRUPT                       0x1A
#define MII_AUX_STATUS3                     0x1C
#define MII_BRCM_TEST                       0x1f

/* MII Auxiliary control/status register */
#define MII_AUX_CTRL_STATUS_FULL_DUPLEX     0x0001
#define MII_AUX_CTRL_STATUS_SP100_INDICATOR 0x0002  /* Speed indication */

/* MII TX Control register. */
#define MII_TX_CONTROL_PGA_FIX_ENABLE       0x0100

/* MII Interrupt register. */
#define MII_INTR_ENABLE                     0x4000
#define MII_INTR_MASK_FDX                   0x0800
#define MII_INTR_MASK_LINK_SPEED            0x0400

/* MII Auxilliary Status 3 register. */
#define MII_AUX_STATUS3_MSE_MASK            0xFF00

/* Broadcom Test register. */
#define MII_BRCM_TEST_HARDRESET             0x0200
#define MII_BRCM_TEST_SHADOW_ENABLE         0x0080
#define MII_BRCM_TEST_10BT_SERIAL_NODRIB    0x0008
#define MII_BRCM_TEST_100TX_POWERDOWN       0x0002
#define MII_BRCM_TEST_10BT_POWERDOWN        0x0001

/* Advertisement control register. */
#define ADVERTISE_FDFC                      0x0400  /* MII Flow Control */

/****************************************************************************
    External switch pseudo PHY: Page (0x00)
****************************************************************************/

#define PAGE_CONTROL                                      0x00

    #define REG_MII_PORT_CONTROL                          0x08

        #define REG_MII_PORT_CONTROL_RX_UCST_EN           0x10
        #define REG_MII_PORT_CONTROL_RX_MCST_EN           0x08
        #define REG_MII_PORT_CONTROL_RX_BCST_EN           0x04  

    #define REG_SWITCH_MODE                               0x0b

        #define REG_SWITCH_MODE_FRAME_MANAGE_MODE         0x01
        #define REG_SWITCH_MODE_SW_FWDG_EN                0x02

    #define REG_CONTROL_MII1_PORT_STATE_OVERRIDE          0x0e

        #define REG_CONTROL_MPSO_MII_SW_OVERRIDE          0x80
#if 1 // 5325F
        #define REG_CONTROL_MPSO_REVERSE_MII              0x20
#else // 5325E
        #define REG_CONTROL_MPSO_REVERSE_MII              0x10
#endif
        #define REG_CONTROL_MPSO_LP_FLOW_CONTROL          0x08
        #define REG_CONTROL_MPSO_SPEED100                 0x04
        #define REG_CONTROL_MPSO_FDX                      0x02
        #define REG_CONTROL_MPSO_LINKPASS                 0x01

        #define REG_CONTROL_MPSO_LP_FLOW_CONTROL_5397     0x30
        #define REG_CONTROL_MPSO_SPEED1000_5397           0x08

        #define REG_CONTROL_MPSO_OVERRIDE_ALL ( \
                  REG_CONTROL_MPSO_MII_SW_OVERRIDE | \
                  REG_CONTROL_MPSO_REVERSE_MII | \
                  REG_CONTROL_MPSO_LP_FLOW_CONTROL | \
                  REG_CONTROL_MPSO_SPEED100 | \
                  REG_CONTROL_MPSO_FDX | \
                  REG_CONTROL_MPSO_LINKPASS \
                  )

        #define REG_CONTROL_MPSO_OVERRIDE_ALL_5397 ( \
                  REG_CONTROL_MPSO_MII_SW_OVERRIDE | \
                  REG_CONTROL_MPSO_LP_FLOW_CONTROL_5397 | \
                  REG_CONTROL_MPSO_SPEED100 | \
                  REG_CONTROL_MPSO_FDX | \
                  REG_CONTROL_MPSO_LINKPASS \
                  )

    #define REG_POWER_DOWN_MODE                           0x0f

        #define REG_POWER_DOWN_MODE_PORT1_PHY_DISABLE     0x01
        #define REG_POWER_DOWN_MODE_PORT2_PHY_DISABLE     0x02
        #define REG_POWER_DOWN_MODE_PORT3_PHY_DISABLE     0x04
        #define REG_POWER_DOWN_MODE_PORT4_PHY_DISABLE     0x08
        #define REG_POWER_DOWN_MODE_PORT5_PHY_DISABLE     0x10
        #define REG_POWER_DOWN_MODE_ALL                   0x1f

    #define REG_DISABLE_LEARNING                          0x3c

/****************************************************************************
    External switch pseudo PHY: Page (0x02)
****************************************************************************/

#define PAGE_MANAGEMENT                                   0x02

    #define REG_GLOBAL_CONFIG                             0x00

        #define ENABLE_MII_PORT						                0x80
        #define RECEIVE_IGMP						                  0x08
        #define RECEIVE_BPDU						                  0x02

    #define REG_DEV_ID                                    0x30

/****************************************************************************
    External switch pseudo PHY: Page (0x05)
****************************************************************************/

#define PAGE_ARL_ACCESS                                   0x05

    #define REG_ARL_ACCESS_CONTROL                        0x00

        #define REG_ARL_CONTROL_READ                      0x01
        #define REG_ARL_CONTROL_START                     0x80

    #define REG_ARL_ACCESS_ADDR_INDEX                     0x02

    #define REG_ARL_VID_TABLE_INDEX                       0x08

    #define REG_ARL_ACCESS_ARL_ENTRY0                     0x10
    #define REG_ARL_ACCESS_ARL_ENTRY1                     0x18

        #define REG_ARL_ENTRY_VALID                       0x80
        #define REG_ARL_ENTRY_STATIC                      0x40
        #define REG_ARL_ENTRY_PORTID_MASK                 0x0f
        #define REG_ARL_ENTRY_PORTID_0                    0x00
        #define REG_ARL_ENTRY_PORTID_1                    0x01
        #define REG_ARL_ENTRY_PORTID_2                    0x02
        #define REG_ARL_ENTRY_PORTID_3                    0x03
        #define REG_ARL_ENTRY_PORTID_4                    0x04
        #define REG_ARL_ENTRY_PORTID_MII                  0x08

/****************************************************************************
    External switch pseudo PHY: Page (0x31)
****************************************************************************/

#define PAGE_PORT_BASED_VLAN				                      0x31

/****************************************************************************
    External switch pseudo PHY: Page (0xff)
****************************************************************************/

#define PAGE_SELECT                                       0xff

/****************************************************************************
    External switch pseudo PHY: Page (0x34)
****************************************************************************/

#define PAGE_VLAN                                         0x34

    #define REG_VLAN_CTRL0                                0x00

        #define REG_VLAN_CTRL0_ENABLE_1Q                  (1 << 7)
        #define REG_VLAN_CTRL0_SVLM                       (0 << 5)
        #define REG_VLAN_CTRL0_IVLM                       (3 << 5)
        #define REG_VLAN_CTRL0_FR_CTRL_CHG_PRI            (1 << 2)
        #define REG_VLAN_CTRL0_FR_CTRL_CHG_VID            (2 << 2)

    #define REG_VLAN_CTRL1                                0x01
    #define REG_VLAN_CTRL2                                0x02
    #define REG_VLAN_CTRL3                                0x03

        #define REG_VLAN_CTRL3_8BIT_CHECK                 (1 << 7)
        #define REG_VLAN_CTRL3_MAXSIZE_1532               (1 << 6)
        #define REG_VLAN_CTRL3_MII_DROP_NON_1Q            (0 << 5)
        #define REG_VLAN_CTRL3_DROP_NON_1Q_SHIFT          0

    #define REG_VLAN_CTRL4                                0x04
    #define REG_VLAN_CTRL4_5397                           0x05

    #define REG_VLAN_CTRL5                                0x05
    #define REG_VLAN_CTRL5_5397                           0x06

        #define REG_VLAN_CTRL5_VID_HIGH_8BIT_NOT_CHECKED (1 << 5)
        #define REG_VLAN_CTRL5_APPLY_BYPASS_VLAN         (1 << 4)
        #define REG_VLAN_CTRL5_DROP_VTAB_MISS            (1 << 3)
        #define REG_VLAN_CTRL5_ENBL_MANAGE_RX_BYPASS     (1 << 1)
        #define REG_VLAN_CTRL5_ENBL_CRC_GEN              (1 << 0)

    #define REG_VLAN_ACCESS                               0x06

        #define REG_VLAN_ACCESS_START_DONE                (1 << 13)
        #define REG_VLAN_ACCESS_WRITE_STATE               (1 << 12)
        #define REG_VALN_ACCESS_HIGH8_VID_SHIFT           4
        #define REG_VALN_ACCESS_LOW4_VID_SHIFT            0

    #define REG_VLAN_WRITE                                0x08

        #define REG_VLAN_WRITE_VALID                      (1 << 20)
        #define REG_VLAN_HIGH_8BIT_VID_SHIFT              12
        #define REG_VLAN_UNTAG_SHIFT                      6
        #define REG_VLAN_GROUP_SHIFT                      0
        #define MII_PORT_FOR_VLAN                         5

    #define REG_VLAN_READ   0x0C
    #define REG_VLAN_PTAG0  0x10
    #define REG_VLAN_PTAG1  0x12
    #define REG_VLAN_PTAG2  0x14
    #define REG_VLAN_PTAG3  0x16
    #define REG_VLAN_PTAG4  0x18
    #define REG_VLAN_PTAG5  0x1A
    #define REG_VLAN_PMAP   0x20

/****************************************************************************
    Registers for pseudo PHY access
****************************************************************************/

#define PSEUDO_PHY_ADDR                                   0x1e

#define REG_PSEUDO_PHY_MII_REG16                          0x10

    #define REG_PPM_REG16_SWITCH_PAGE_NUMBER_SHIFT        8
    #define REG_PPM_REG16_MDIO_ENABLE                     0x01

#define REG_PSEUDO_PHY_MII_REG17                          0x11

    #define REG_PPM_REG17_REG_NUMBER_SHIFT                8
    #define REG_PPM_REG17_OP_DONE                         0x00
    #define REG_PPM_REG17_OP_WRITE                        0x01
    #define REG_PPM_REG17_OP_READ                         0x02

#define REG_PSEUDO_PHY_MII_REG24                          0x18
#define REG_PSEUDO_PHY_MII_REG25                          0x19
#define REG_PSEUDO_PHY_MII_REG26                          0x1a
#define REG_PSEUDO_PHY_MII_REG27                          0x1b

#define SPI_STATUS_OK                       0
#define SPI_STATUS_INVALID_LEN              -1

/****************************************************************************
    Prototypes
****************************************************************************/

extern int mii_init(struct net_device *dev);
extern int mii_read(struct net_device *dev, int phy_id, int location);
extern void mii_write(struct net_device *dev, int phy_id, int location, int data);
extern int mii_linkstatus(struct net_device *dev, int phy_id);
void mii_linkstatus_start(struct net_device *dev, int phy_id);
int mii_linkstatus_check(struct net_device *dev, int *up);
extern void mii_enablephyinterrupt(struct net_device *dev, int phy_id);
extern void mii_clearphyinterrupt(struct net_device *dev, int phy_id);
extern int ethsw_config_vlan(struct net_device *dev, int enable, unsigned int vid);
extern void ethsw_switch_power_off(void* context);
extern void ethsw_switch_frame_manage_mode(struct net_device *dev);
extern void ethsw_switch_unmanage_mode(struct net_device *dev);
extern int ethsw_add_proc_files(struct net_device *dev);
extern int ethsw_del_proc_files(void);

extern int ethsw_macaddr_to_portid(struct net_device *dev, uint8 *data, int len);
extern int ethsw_link_status(struct net_device *dev);

#endif /* _BCMMII_H_ */
