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
 * File Name  : boardparms.c
 *
 * Description: This file contains the implementation for the BCM97xxx board
 *              parameter access functions.
 * 
 * Updates    : 07/14/2003  Created.
 *              09/25/2008  L,Sun modified.
 ***************************************************************************/

/* Includes. */
#include <linux/kernel.h>
#include "boardparms.h"

/* PHY type string. */
char * gsPhyType[] = {
	"NO PHY",
	"INTERNAL PHY",
	"EXTERNAL PHY",
	"EXTERNAL GPHY",
	"EXTERNAL GPHY-IBS",
	"MOCA",
	"EXTERNAL SWITCH"
};
/*
 * array of all supported board configurations
 */
static  ETHERNET_MAC_INFO g_MacInfos[] =
{
	/* Internal PHY */
	{ 
		BP_ENET_INTERNAL_PHY,		/* PHY type*/
		0x01,						/* PHY address*/
	  	0x01,						/* N/A for switch only*/
	  	BP_NOT_DEFINED,				/* N/A for switch only*/
	  	BP_ENET_CONFIG_MDIO,		/* Configuration type */
	  	BP_NOT_DEFINED				/* Reverse MII */
	},
	/* External PHY 10/100 only PHY*/
	{
		BP_ENET_EXTERNAL_PHY,
		0x01,
		0x01,
		BP_NOT_DEFINED,
		BP_ENET_CONFIG_MDIO,
		BP_NOT_DEFINED
	},
	/* Extern GPHY */
	{
		BP_ENET_EXTERNAL_GPHY,
		0x01,
		0x01,
		BP_NOT_DEFINED,
		BP_ENET_CONFIG_MDIO,
		BP_NOT_DEFINED
	},
	/* Extern GPHY, support in-band signaling */
	{
		BP_ENET_EXTERNAL_GPHY_IBS,
		0x01,
		0x01,
		BP_NOT_DEFINED,
		BP_ENET_CONFIG_MDIO,
		BP_NOT_DEFINED
	},
	/* Extern MOCA */
	{
		BP_ENET_EXTERNAL_MOCA,
		0x01,
		0x01,
		BP_NOT_DEFINED,
		BP_NOT_DEFINED,
		BP_NOT_DEFINED
	},
	/*
	 * Place holder to add switch config later
	 */
	/* NO PHY */
	{
		BP_ENET_NO_PHY,
		0x0,
		0x0,
		BP_NOT_DEFINED,
		BP_NOT_DEFINED,
		BP_NOT_DEFINED
	}

};

/**************************************************************************
 * Name       : BpGetEthernetMacInfo
 *
 * Description: This function returns EthernetMacInfo for specified PHY type
 * e.g. external/internal/ or switch
 *
 * Parameters : [In]  PhyType, PHY type.
 * 				[OUT] pEnetInfos - Address of ETHERNET_MAC_INFO to be filled.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 ***************************************************************************/
int BpGetEthernetMacInfo( ETHERNET_MAC_INFO * pEnetInfos, int PhyType )
{
    int i, nRet;

	pEnetInfos->PhyType = BP_ENET_NO_PHY;
	for ( i = 0; i < sizeof(g_MacInfos)/sizeof(ETHERNET_MAC_INFO); i++)
	{
		if(g_MacInfos[i].PhyType == PhyType)
		{
			memcpy(pEnetInfos, g_MacInfos[i], sizeof(ETHERNET_MAC_INFO) );
			nRet = BP_SUCCESS;
			break;
		}
	}

	if(pEnetInfos->PhyType == BP_ENET_NO_PHY)
		nRet = BP_BOARD_ID_NOT_SET;

	return nRet;
} /* BpGetEthernetMacInfo */
