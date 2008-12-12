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
/**************************************************************************
 * File Name  : boardparms.h
 *
 * Description: This file contains definitions and function prototypes for
 *              the BCM97xxx board parameter access functions.
 * 
 * Updates    : 09/22/2008  L.Sun modifed.
 ***************************************************************************/

#if !defined(_BOARDPARMS_H)
#define _BOARDPARMS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Return codes. */
#define BP_SUCCESS                              0
#define BP_BOARD_ID_NOT_FOUND                   1
#define BP_VALUE_NOT_DEFINED                    2
#define BP_BOARD_ID_NOT_SET                     3

/* Values for EthernetMacInfo PhyType. */
#define BP_ENET_NO_PHY                          0
#define BP_ENET_INTERNAL_PHY                    1
#define BP_ENET_EXTERNAL_PHY                    2	/* External 10/100 PHY */
#define BP_ENET_EXTERNAL_GPHY					3	/* External 1000 PHY, use RGMII*/
#define BP_ENET_EXTERNAL_GPHY_IBS				4	/* External 1000 PHY, use RGMII, the PHY support in-band signaling*/
#define BP_ENET_EXTERNAL_MOCA					5	/* External MoCA, use GMII*/
#define BP_ENET_EXTERNAL_SWITCH                 6

/* Value for Ethernet Switch mode */
#define BP_ENET_NO_MANAGEMENT_PORT              0
#define BP_ENET_MANAGEMENT_PORT                 1

/* Values for EthernetMacInfo Configuration type. */
#define BP_ENET_CONFIG_MDIO                     0       /* Internal PHY, External PHY, Switch+(no GPIO, no SPI, no MDIO Pseudo phy */
#define BP_ENET_CONFIG_MDIO_PSEUDO_PHY          1

/* Values for EthernetMacInfo Reverse MII. */
#define BP_ENET_NO_REVERSE_MII                  0
#define BP_ENET_REVERSE_MII                     1

/* Value for GPIO and external interrupt fields that are not used. */
#define BP_NOT_DEFINED                          0xffff
#define BP_UNEQUIPPED                           0xfff1

/* Maximum size of the board id string. */
#define BP_BOARD_ID_LEN                         32

#if !defined(__ASSEMBLER__)

/* Information about an Ethernet MAC.  If ucPhyType is BP_ENET_NO_PHY,
 * then the other fields are not valid.
 */
typedef struct EthernetMacInfo
{
    int PhyType;			/* NO_PHY/INTERNAL_PHY/EXTERNAL_PHY/EXTERNAL_SWITCH */
    int PhyAddress;			/* 0 to 31 */
    int numSwitchPorts;		/* for switch only */
    int ManagementSwitch;	/* for switch only */
    int ConfigType;			/* 0 MDIO or 1 PSEUDO_PHY for switch */
    int ReverseMii;			/* Reverse MII */
} ETHERNET_MAC_INFO;

int BpGetEthernetMacInfo( ETHERNET_MAC_INFO * pEnetInfos, int phy_type );


#endif /* __ASSEMBLER__ */

#ifdef __cplusplus
}
#endif

#endif /* _BOARDPARMS_H */
