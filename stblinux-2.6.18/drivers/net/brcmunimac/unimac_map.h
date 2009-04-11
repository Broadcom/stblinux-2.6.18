/*
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
/* uniMAC register definations.*/

#ifndef __INTEMAC_MAP_H
#define __INTEMAC_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "unimac_defs.h"

#ifndef __ASSEMBLY__

#define UMAC_RX_CHAN               	1
#define UMAC_TX_CHAN               	0
/* Register block offset */
#define UMAC_DMA_REG_OFFSET		  	0x2400	/* UMAC start from 0x0004, DMA start from 0x2400 ! */
#define UMAC_TXRX_REG_OFFSET		0x0800	/* TxRx Control registerS offset */
#define UMAC_INTRL2_REG_OFFSET		0x0a00	/* INT2R registerS offset */
#define UMAC_EPLL_REG_OFFSET		0x3800	/* EPLL registers offset */
#define UMAC_HFB_OFFSET				0x1000	/* Hardware filter block offset */
#define UMAC_IOMAP_SIZE				0x4000

/* 64B Rx Status Block */
typedef struct RxStatus
{
	unsigned long length_status;		/* length and peripheral status */
	unsigned long ext_status;			/* Extended status*/
	unsigned long csum;					/* raw checksum */
	unsigned long filter_idex;			/* Filter index */
	unsigned long extracted_bytes[4];	/* Extracted byte 0 - 16 */
	unsigned long unused[8];			/* unused */
}RSB;

#define RSB_PS_MASK			0x0FFF
#define RSB_SOP				0x2000
#define RSB_EOP				0x4000
#define RSB_LENGTH_MASK		0xFFF
#define RSB_LENGTH_SHIFT	16
#define RSB_EXT_STATUS_MASK	0x1FFFFF
#define RSB_CSUM_MASK		0xFFFF
#define RSB_FILTER_IDX_MASK	0xFFFF

/* 64B Tx status Block */
typedef struct TxStatus
{
	unsigned long length_status;		/* ULP pointer and checksum offset */
	unsigned long unused[15];
}TSB;
#define TSB_CSUM_MASK		0x0FFF
#define TSB_ULP_MASK		0x0FFF
#define TSB_ULP_SHIFT		16
#define TSB_LV				0x80000000

/*
** DMA Channel Configuration
*/
typedef struct DmaChannel
{
  unsigned long cfg;                    /* (00) RW assorted configuration */
  unsigned long intStat;                /* (04) RO interrupts control and status */
  unsigned long intMask;                /* (08) RO interrupts mask */
  unsigned long maxBurst;               /* (0C) RW max burst length permitted */
  unsigned long descPtr;				/* (10) RO isdma base descriptor pointer */
  unsigned long descOffset;				/* (14) RO current descriptor offset the channel is pointing to*/
  /* Unused words */
  unsigned long unused0[26];
} DmaChannel;

/*
** DMA State RAM (1 .. 16)
*/
typedef struct DmaStateRam {
  unsigned long baseDescPtr;            /* (00) descriptor ring start address */
  unsigned long state_data;             /* (04) state/bytes done/ring offset */
  unsigned long desc_len_status;        /* (08) buffer descriptor status and len */
  unsigned long desc_base_bufptr;       /* (0C) buffer descrpitor current processing */
} DmaStateRam;

/*
** DMA Registers
*/
typedef struct DmaRegs 
{
    unsigned long controller_cfg;              /* (00) RW controller configuration */
    unsigned long flowctl_ch1_thresh_lo;       /* (04) RW EMAC1 RX DMA channel */
    unsigned long flowctl_ch1_thresh_hi;       /* (08) RW EMAC1 RX DMA channel */
    unsigned long flowctl_ch1_alloc;           /* (0C) RW EMAC1 RX DMA channel */
    unsigned long enet_isdma_rev;              /* (10) RO Enet rev             */
    unsigned long enet_isdma_tstctl;           /* (14) RW Enet test control    */
    unsigned long enet_isdma_pci_irq_sts;      /* (18) RO Enet pci intr status */
    unsigned long enet_isdma_pci_irq_msk;      /* (1C) RO Enet pci intr mask   */
    unsigned long enet_isdma_r5k_irq_sts;      /* (20) RO Enet r5k intr status */
    unsigned long enet_isdma_r5k_irq_msk;      /* (24) RO Enet r5k intr mask   */
    unsigned long enet_isdma_diag_ctl;         /* (28) RO Enet diag control   */
    unsigned long enet_isdma_diag_rdbk;        /* (2C) RO Enet diag readback   */
    unsigned long enet_isdma_mii_select;       /* (30) RW Enet PHY select */
    unsigned long unused0[3];
    unsigned long enet_isdma_desc_alloc;       /* (40) RO Enet RX desc allocation */
    unsigned long enet_isdma_desc_thres;       /* (44) RW Enet RX desc threshold */
    unsigned long enet_isdma_desc_timeout;     /* (48) RW Enet RX desc timeout */
    unsigned long enet_isdma_desc_irq_sts;     /* (4c) RO Enet RX desc irq status */
    unsigned long enet_isdma_desc_irq_msk;     /* (50) RO Enet RX desc irq mask */
    unsigned long unused1[43];

    /* Per channel registers/state ram */
    DmaChannel rx_chan;						   /* (100) Rx Channel configuration */
	DmaChannel tx_chan;						   /* (180) Tx Channel configuration */
} DmaRegs;

/*
** DMA Descriptor 
*/
typedef struct DmaDesc {
  unsigned long length_status;	 				/* in bytes of data in buffer */
  unsigned long address;         				/* address of data */
} DmaDesc;


/*
** UniMAC TSV or RSV (Transmit Status Vector or Receive Status Vector 
*/
/* Rx/Tx common counter group.*/
typedef struct PktCounterSize {
	unsigned long cnt_64;		 /* RO Recvied/Transmited 64 bytes packet */
	unsigned long cnt_127;		 /* RO Rx/Tx 127 bytes packet */
	unsigned long cnt_255;		 /* RO Rx/Tx 65-255 bytes packet */
	unsigned long cnt_511;		 /* RO Rx/Tx 256-511 bytes packet */
	unsigned long cnt_1023;		 /* RO Rx/Tx 512-1023 bytes packet */
	unsigned long cnt_1518;		 /* RO Rx/Tx 1024-1518 bytes packet */
	unsigned long cnt_mgv;		 /* RO Rx/Tx 1519-1522 good VLAN packet counter */
	unsigned long cnt_2047;		 /* RO Rx/Tx 1522-2047 bytes packet*/
	unsigned long cnt_4095;		 /* RO Rx/Tx 2048-4095 bytes packet*/
	unsigned long cnt_9216;		 /* RO Rx/Tx 4096-9216 bytes packet*/
}PktCounterSize;
/* RSV, Receive Status Vector */
typedef struct UniMacRSV {
	PktCounterSize stat_sz; 	 /* (0x400 - 0x424), statistics of receive packets classified by size.*/
	unsigned long rx_pkt;		 /* RO (0x428) Receive packet count*/
	unsigned long rx_bytes;		 /* RO Receive byte count */
	unsigned long rx_mca;		 /* RO Receive multicast packet count */
	unsigned long rx_bca;		 /* RO Receive broadcast packet count */
	unsigned long rx_fcs;		 /* RO Received FCS error count */
	unsigned long rx_cf;		 /* RO Received control frame packet count*/
	unsigned long rx_pf;		 /* RO Received pause frame packet count */
	unsigned long rx_uo;		 /* RO Received unknown op code packet count */
	unsigned long rx_aln;		 /* RO Received alignment error count */
	unsigned long rx_flr;		 /* RO Received frame length out of range count */
	unsigned long rx_cde;		 /* RO Received code error packet count */
	unsigned long rx_fcr;		 /* RO Received carrier sense error packet count */
	unsigned long rx_ovr;		 /* RO Received oversize packet count*/
	unsigned long rx_jbr;		 /* RO Received jabber count */
	unsigned long rx_mtue;		 /* RO Received MTU error packet count*/
	unsigned long rx_pok;		 /* RO Receivd good packet count */
	unsigned long rx_uc;		 /* RO Received unicast packet count */
	unsigned long rx_ppp;		 /* RO Received PPP packet count */
	unsigned long rcrc;			 /* RO (0x470),Received CRC match packet count */
}UniMacRSV;

/* TSV, Transmit Status Vector */
typedef struct UniMacTSV {
	PktCounterSize stat_sz;		/* (0x480 - 0x0x4a4), statistics of transmitted packets classified by size */
	unsigned long tx_pkt;		/* RO (0x4a8) Transmited packet count */
	unsigned long tx_mca;		/* RO Transmit multicast packet count */
	unsigned long tx_bca;		/* RO Transmit broadcast packet count */
	unsigned long tx_pf;		/* RO Transmit pause frame count */
	unsigned long tx_cf;		/* RO Transmit control frame count */
	unsigned long tx_fcs;		/* RO Transmit FCS error count */
	unsigned long tx_ovr;		/* RO Transmit oversize packet count */
	unsigned long tx_drf;		/* RO Transmit deferral packet count */
	unsigned long tx_edf;		/* RO Transmit Excessive deferral packet count*/
	unsigned long tx_scl;		/* RO Transmit single collision packet count */
	unsigned long tx_mcl;		/* RO Transmit multiple collision packet count*/
	unsigned long tx_lcl;		/* RO Transmit late collision packet count */
	unsigned long tx_ecl;		/* RO Transmit excessive collision packet count*/
	unsigned long tx_frg;		/* RO Transmit fragments packet count*/
	unsigned long tx_ncl;		/* RO Transmit total collision count */
	unsigned long tx_jbr;		/* RO Transmit jabber count*/
	unsigned long tx_bytes;		/* RO Transmit byte count */
	unsigned long tx_pok;		/* RO Transmit good packet count */
	unsigned long tx_uc;		/* RO (0x0x4f0)Transmit unitcast packet count */
}UniMacTSV;

typedef struct uniMacRegs {
	unsigned long unused;				/* (00) UMAC register start from offset 0x04, why? */
	unsigned long hdBkpCtrl;			/* (04) RW Half-duplex backpressure control*/
	unsigned long cmd;					/* (08) RW UniMAC command register.*/
	unsigned long mac_0;				/* (0x0c) RW */
	unsigned long mac_1;				/* (0x10) RW */
	unsigned long max_frame_len;		/* (0x14) RW Maximum frame length, also used in Tx */
	unsigned long pause_quant;			/* (0x18) RW bit time for pause frame.*/
	unsigned long unused0[9];			
	unsigned long sdf_offset;			/* (0x40) RW EFM preamble length, 5B to 16B */
	unsigned long mode;					/* (0x44) RO Mode */
	unsigned long frm_tag0;				/* (0x48) RW outer tag.*/
	unsigned long frm_tag1;				/* (0x4c) RW inner tag.*/
	unsigned long unused10[3];
	unsigned long tx_ipg_len;			/* (0x5c) RW inter-packet gap length. */
	unsigned long unused1[172];
	unsigned long macsec_tx_crc;		/* (0x310) RW */
	unsigned long macsec_ctrl;			/* (0x314) RW macsec control register */
	unsigned long ts_status;			/* (0x318) RO  Timestamp status */
	unsigned long ts_data;				/* (0x31c) RO Timestamp data.*/
	unsigned long unused2[4];
	unsigned long pause_ctrl;			/* (0x330) RW Tx Pause control */
	unsigned long tx_flush;				/* (0x334) RW Flush Tx engine. */
	unsigned long rxfifo_status;		/* (0x338) RO */
	unsigned long txfifo_status;		/* (0x33c) RO */
	unsigned long ppp_ctrl;				/* (0x340) RW ppp control.*/
	unsigned long ppp_refresh_ctrl;		/* (0x344) RW ppp refresh control.*/
	unsigned long unused11[4];			/* (0x348 - 0x354) RW tx_pause_prb not used here */
	unsigned long unused12[4];			/* (0x358 - 0x364) RW rx_pause_prb not used here*/
	unsigned long unused13[38];
	UniMacRSV rsv;						/* (0x400 - 0x470) Receive Status Vector */
	unsigned long unused3[3];
	UniMacTSV tsv;						/* (0x480 - 0x4f0) Transmit Status Vector */
	unsigned long unused4[7];			/* Ignore RUNT sutff for now! */
	unsigned long unused5[28];
	unsigned long mib_ctrl;				/* (0x580) RW Mib control register */
	unsigned long unused6[31];
	unsigned long pkpu_ctrl;			/* (0x600) RW backpressure control register.*/
	unsigned long mac_rxerr_mask;		/* (0x604) RW  */
	unsigned long max_pkt_size;			/* (0x608) RW max packet size, default 1518 bytes */
	unsigned long ppp_extern_ctrl;		/* (0x60c) RW */
	unsigned long ppp_status;			/* (0x610) RW */
	unsigned long mdio_cmd;				/* (0x614  RO mdio command register.*/
	unsigned long mdio_cfg;				/* (0x618) RW mdio configuration register */
	unsigned long unused7;
	unsigned long mpd_ctrl;				/* (0x620) RW Magic packet control */
	unsigned long mpd_pw_ms;			/* (0x624) RW Magic packet password 47:32*/
	unsigned long mpd_pw_ls;			/* (0x628) RW Magic packet password 31:0*/
	unsigned long unused8[5];
	unsigned long mdf_ctrl;				/* (0x650) RW MAC DA filter control*/
	unsigned long mdf_addr[33];			/* (0x654 - 0x6d8) Desination MAC address registers */

}uniMacRegs;

/* unitMac Rx/Tx Buffer registers */
typedef struct rbufRegs
{
	unsigned long rbuf_ctrl;			/* (00) rx buffer control register*/
	unsigned long rbuf_flush_ctrl;		/* (04) reset rx logit and flush rx buffer*/
	unsigned long rbuf_pkt_rdy_thld;	/* (08) threshold which PKT_RDY asserted defualt 0x7c*/
	unsigned long rbuf_status;			/* (0c) rx buffer status.*/
	unsigned long rbuf_endian_ctrl;		/* (10) rx buffer endianess control register*/
	unsigned long rbuf_chk_ctrl;		/* (14) raw checksum control register */
	unsigned long rbuf_rxc_offset[8];	/* (18 - 34) rxc extraction offset registers, for filtering*/
	unsigned long rbuf_hfb_ctrl;		/* (38) hardware filter block control register*/
	unsigned long rbuf_fltr_len[4];		/* (3c - 48)filter length registers */
	unsigned long unused0[13];
	unsigned long tbuf_ctrl;			/* (80) tx buffer control */
	unsigned long tbuf_flush_ctrl;		/* (84) flush tx buffer and reset tx engine*/
	unsigned long unused1[5];
	unsigned long tbuf_endian_ctrl;		/* (9c) tx buffer endianess control*/
	unsigned long tbuf_bp_mc;			/* (a0) tx buffer backpressure mask and control*/
	unsigned long tbuf_pkt_rdy_thld;	/* (a4) threshold for PKT_RDY , for jumbo frame, default 0x7C*/
	unsigned long unused2[2];
	unsigned long rgmii_oob_ctrl;		/* (b0) RGMII OOB control register */
	unsigned long rgmii_ib_status;		/* (b4) RGMII inBand status register*/
	unsigned long rgmii_led_ctrl;		/* (b8) RGMII LED control register */
	unsigned long unused3;
	unsigned long moca_status;			/* (c0) MOCA transmit buffer threshold corssed register*/
	unsigned long unused4[3];
	unsigned long ephy_pwr_mgmt;		/* (d0) PHY power management register*/
	unsigned long unused5;
	unsigned long emcg_ctrl;			/* (d8) ENDT EMCG control register*/
	unsigned long test_mux_ctrl;		/* (dc) ENET test mux control register*/
}rbufRegs;

typedef struct intrl2Regs
{
	unsigned long cpu_stat;				/*(00) CPU interrupt status */
	unsigned long cpu_set;				/*(04) set the corresponding irq*/
	unsigned long cpu_clear;			/*(08) clear the corresponding irq*/
	unsigned long cpu_mask_status;		/*(0c) Show current masking of irq*/
	unsigned long cpu_mask_set;			/*(10) Disable corresponding irq*/
	unsigned long cpu_mask_clear;		/*(14) Enable corresponding irq*/
	unsigned long pci_stat;				/*(00) PCI interrupt status */
	unsigned long pci_set;				/*(04) set the corresponding irq*/
	unsigned long pci_clear;			/*(08) clear the corresponding irq*/
	unsigned long pci_mask_status;		/*(0c) Show current masking of irq*/
	unsigned long pci_mask_set;			/*(10) Disable corresponding irq*/
	unsigned long pci_mask_clear;		/*(14) Enable corresponding irq*/
}intrl2Regs;

/* PLL control registers */
typedef struct epllRegs
{
	unsigned long epll_in_ctrl;			/*(00) input reference clock select*/
	unsigned long epll_fblp_ctrl;		/*(04) divider control*/
	unsigned long epll_stat0;			/*(08) status 0*/
	unsigned long epll_stat1;			/*(0c) status 1*/
	unsigned long epll_ch_ctrl[6];		/*(10-24) channel control.*/
}epllRegs;
	
#endif /* __ASSEMBLY__ */


#ifdef __cplusplus
}
#endif

#endif
