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
/***********************************************************************/
/*                                                                     */
/*   MODULE:  intEMAC_map.h (was 6345_map.h)                           */
/*   DATE:    96/12/19                                                 */
/*   PURPOSE: Define addresses of major hardware components of         */
/*            the internal MAC component of the 7110, which are not    */
/*            defined by bcm711x_map.h                                 */
/*                                                                     */
/***********************************************************************/
#ifndef __INTEMAC_MAP_H
#define __INTEMAC_MAP_H

#if __cplusplus
extern "C" {
#endif

#include <asm/brcmstb/common/bcmtypes.h>
#include <asm/brcmstb/brcm97110/bcm97110.h>

/* macro to convert logical data addresses to physical */
/* DMA hardware must see physical address */
#define LtoP( x )       ( (uint32)x & 0x1fffffff )
#define PtoL( x )       ( LtoP(x) | 0xa0000000 )

#define EMAC_CLK_EN     0x0080




/* HDLC Descriptor Status definitions */
#define          DMA_HDLC_TX_ABORT      0x0100
#define          DMA_HDLC_RX_OVERRUN    0x4000
#define          DMA_HDLC_RX_TOO_LONG   0x2000
#define          DMA_HDLC_RX_CRC_OK     0x1000
#define          DMA_HDLC_RX_ABORT      0x0100


/*
** EMAC transmit MIB counters
*/
typedef struct EmacTxMib {
  uint32        tx_good_octets;         /* (200) good byte count */
  uint32        tx_good_pkts;           /* (204) good pkt count */
  uint32        tx_octets;              /* (208) good and bad byte count */
  uint32        tx_pkts;                /* (20c) good and bad pkt count */
  uint32        tx_broadcasts_pkts;     /* (210) good broadcast packets */
  uint32        tx_multicasts_pkts;     /* (214) good mulitcast packets */
  uint32        tx_len_64;              /* (218) RMON tx pkt size buckets */
  uint32        tx_len_65_to_127;       /* (21c) */
  uint32        tx_len_128_to_255;      /* (220) */
  uint32        tx_len_256_to_511;      /* (224) */
  uint32        tx_len_512_to_1023;     /* (228) */
  uint32        tx_len_1024_to_max;     /* (22c) */
  uint32        tx_jabber_pkts;         /* (230) > 1518 with bad crc */
  uint32        tx_oversize_pkts;       /* (234) > 1518 with good crc */
  uint32        tx_fragment_pkts;       /* (238) < 63   with bad crc */
  uint32        tx_underruns;           /* (23c) fifo underrun */
  uint32        tx_total_cols;          /* (240) total collisions in all tx pkts */
  uint32        tx_single_cols;         /* (244) tx pkts with single collisions */
  uint32        tx_multiple_cols;       /* (248) tx pkts with multiple collisions */
  uint32        tx_excessive_cols;      /* (24c) tx pkts with excessive cols */
  uint32        tx_late_cols;           /* (250) tx pkts with late cols */
  uint32        tx_defered;             /* (254) tx pkts deferred */
  uint32        tx_carrier_lost;        /* (258) tx pkts with CRS lost */
  uint32        tx_pause_pkts;          /* (25c) tx pause pkts sent */
#define NumEmacTxMibVars        24
} EmacTxMib;

/*
** EMAC receive MIB counters
*/
typedef struct EmacRxMib {
  uint32        rx_good_octets;         /* (280) good byte count */
  uint32        rx_good_pkts;           /* (284) good pkt count */
  uint32        rx_octets;              /* (288) good and bad byte count */
  uint32        rx_pkts;                /* (28c) good and bad pkt count */
  uint32        rx_broadcasts_pkts;     /* (290) good broadcast packets */
  uint32        rx_multicasts_pkts;     /* (294) good mulitcast packets */
  uint32        rx_len_64;              /* (298) RMON rx pkt size buckets */
  uint32        rx_len_65_to_127;       /* (29c) */
  uint32        rx_len_128_to_255;      /* (2a0) */
  uint32        rx_len_256_to_511;      /* (2a4) */
  uint32        rx_len_512_to_1023;     /* (2a8) */
  uint32        rx_len_1024_to_max;     /* (2ac) */
  uint32        rx_jabber_pkts;         /* (2b0) > 1518 with bad crc */
  uint32        rx_oversize_pkts;       /* (2b4) > 1518 with good crc */
  uint32        rx_fragment_pkts;       /* (2b8) < 63   with bad crc */
  uint32        rx_missed_pkts;         /* (2bc) missed packets */
  uint32        rx_crc_align_errs;      /* (2c0) both or either */
  uint32        rx_undersize;           /* (2c4) < 63   with good crc */
  uint32        rx_crc_errs;            /* (2c8) crc errors (only) */
  uint32        rx_align_errs;          /* (2cc) alignment errors (only) */
  uint32        rx_symbol_errs;         /* (2d0) pkts with RXERR assertions (symbol errs) */
  uint32        rx_pause_pkts;          /* (2d4) MAC control, PAUSE */
  uint32        rx_nonpause_pkts;       /* (2d8) MAC control, not PAUSE */
#define NumEmacRxMibVars        23
} EmacRxMib;

typedef struct EmacRegisters {
  uint32        rxControl;              /* (00) receive control */
#define          EMAC_PM_REJ    0x80    /*      - reject DA match in PMx regs */
#define          EMAC_UNIFLOW   0x40    /*      - accept cam match fc */
#define          EMAC_FC_EN     0x20    /*      - enable flow control */
#define          EMAC_LOOPBACK  0x10    /*      - loopback */
#define          EMAC_PROM      0x08    /*      - promiscuous */
#define          EMAC_RDT       0x04    /*      - ignore transmissions */
#define          EMAC_ALL_MCAST 0x02    /*      - ignore transmissions */
#define          EMAC_NO_BCAST  0x01    /*      - ignore transmissions */


  uint32        rxMaxLength;            /* (04) receive max length */
  uint32        txMaxLength;            /* (08) transmit max length */
  uint32        unused1[1];
  uint32        mdioFreq;               /* (10) mdio frequency */
#define          EMAC_MII_PRE_EN 0x0100 /* prepend preamble sequence */
#define          EMAC_MDIO_PRE   0x100  /*      - enable MDIO preamble */
#define          EMAC_MDC_FREQ   0x0ff  /*      - mdio frequency */

  uint32        mdioData;               /* (14) mdio data */
#define          MDIO_WR        0x50020000 /*   - write framing */
#define          MDIO_RD        0x60020000 /*   - read framing */
#define          MDIO_PMD_SHIFT  23
#define          MDIO_REG_SHIFT  18

  uint32        intMask;                /* (18) int mask */
  uint32        intStatus;              /* (1c) int status */
#define          EMAC_FLOW_INT  0x04    /*      - flow control event */
#define          EMAC_MIB_INT   0x02    /*      - mib event */
#define          EMAC_MDIO_INT  0x01    /*      - mdio event */

  uint32        unused2[3];
  uint32        config;                 /* (2c) config */
#define          EMAC_ENABLE    0x001   /*      - enable emac */
#define          EMAC_DISABLE   0x002   /*      - disable emac */
#define          EMAC_SOFT_RST  0x004   /*      - soft reset */
#define          EMAC_SOFT_RESET 0x004  /*      - emac soft reset */
#define          EMAC_EXT_PHY   0x008   /*      - external PHY select */

  uint32        txControl;              /* (30) transmit control */
#define          EMAC_FD        0x001   /*      - full duplex */
#define          EMAC_FLOWMODE  0x002   /*      - flow mode */
#define          EMAC_NOBKOFF   0x004   /*      - no backoff in  */
#define          EMAC_SMALLSLT  0x008   /*      - small slot time */

  uint32        txThreshold;            /* (34) transmit threshold */
  uint32        mibControl;             /* (38) mib control */
#define          EMAC_NO_CLEAR  0x001   /* don't clear on read */

  uint32        unused3[7];

  uint32        pm0DataLo;              /* (58) perfect match 0 data lo */
  uint32        pm0DataHi;              /* (5C) perfect match 0 data hi (15:0) */
  uint32        pm1DataLo;              /* (60) perfect match 1 data lo */
  uint32        pm1DataHi;              /* (64) perfect match 1 data hi (15:0) */
  uint32        pm2DataLo;              /* (68) perfect match 2 data lo */
  uint32        pm2DataHi;              /* (6C) perfect match 2 data hi (15:0) */
  uint32        pm3DataLo;              /* (70) perfect match 3 data lo */
  uint32        pm3DataHi;              /* (74) perfect match 3 data hi (15:0) */
#define          EMAC_CAM_V   0x10000  /*      - cam index */
#define          EMAC_CAM_VALID 0x00010000

  uint32        unused4[98];            /* (78-1fc) */

  EmacTxMib     tx_mib;                 /* (200) emac tx mib */
  uint32        unused5[8];             /* (260-27c) */

  EmacRxMib     rx_mib;                 /* (280) rx mib */

} EmacRegisters;

/* register offsets for subrouting access */
#define EMAC_RX_CONTROL         0x00
#define EMAC_RX_MAX_LENGTH      0x04
#define EMAC_TX_MAC_LENGTH      0x08
#define EMAC_MDIO_FREQ          0x10
#define EMAC_MDIO_DATA          0x14
#define EMAC_INT_MASK           0x18
#define EMAC_INT_STATUS         0x1C
#define EMAC_CAM_DATA_LO        0x20
#define EMAC_CAM_DATA_HI        0x24
#define EMAC_CAM_CONTROL        0x28
#define EMAC_CONTROL            0x2C
#define EMAC_TX_CONTROL         0x30
#define EMAC_TX_THRESHOLD       0x34
#define EMAC_MIB_CONTROL        0x38


#define EMAC ((volatile EmacRegisters * const) ENET_MAC_ADR_BASE)
#define DMA_BASE				DMA_ADR_BASE  /* DMA control registers defined in bcm97110.h */


#if __cplusplus
}
#endif

#endif

