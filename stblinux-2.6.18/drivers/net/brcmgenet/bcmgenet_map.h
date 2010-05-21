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

#ifndef __BCMGENET_MAP_H__
#define __BCMGENET_MAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bcmgenet_defs.h"

#ifndef __ASSEMBLY__

/* 64B Rx Status Block */
typedef struct Status
{
	unsigned long length_status;		/* length and peripheral status */
	unsigned long ext_status;			/* Extended status*/
	unsigned long rx_csum;				/* partial rx checksum */
	unsigned long filter_index;			/* Filter index */
	unsigned long extracted_bytes[4];	/* Extracted byte 0 - 16 */
	unsigned long reserved[4];
	unsigned long tx_csum_info;			/* Tx checksum info. */
	unsigned long unused[3];			/* unused */
}StatusBlock;
/* Rx status */
#define STATUS_RX_EXT_MASK		0x1FFFFF
#define STATUS_RX_CSUM_MASK		0xFFFF
#define STATUS_RX_CSUM_OK		0x10000		/* checksum ok */
#define STATUS_RX_CSUM_FR		0x20000		/* Fragmented packet, partial checksum */
#define STATUS_RX_PROTO_TCP		0
#define STATUS_RX_PROTO_UDP		1
#define STATUS_RX_PROTO_ICMP	2
#define STATUS_RX_PROTO_OTHER	3
#define STATUS_RX_PROTO_MASK	3
#define STATUS_RX_PROTO_SHIFT	18
#define STATUS_FILTER_INDEX_MASK	0xFFFF
/* Tx status */
#define STATUS_TX_CSUM_START_MASK	0X7FFF
#define STATUS_TX_CSUM_START_SHIFT	16
#define STATUS_TX_CSUM_PROTO_UDP	0x8000
#define STATUS_TX_CSUM_OFFSET_MASK	0x7FFF
#define STATUS_TX_CSUM_LV			0x80000000
/*
** DMA Descriptor 
*/
typedef struct DmaDesc {
  unsigned long length_status;	 		/* in bytes of data in buffer */
  unsigned long address;         		/* address of data */
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
	unsigned long bkpu_ctrl;			/* (0x600) RW backpressure control register.*/
	unsigned long mac_rxerr_mask;		/* (0x604) RW  */
	unsigned long max_pkt_size;			/* (0x608) RW max packet size, default 1518 bytes */
	unsigned long unused7[2];
	unsigned long mdio_cmd;				/* (0x614  RO mdio command register.*/
	unsigned long mdio_cfg;				/* (0x618) RW mdio configuration register */
#ifdef CONFIG_BRCM_HAS_GENET2
	unsigned long unused9;
#else
	unsigned long rbuf_ovfl_pkt_cnt;	/* (0x61c) RO rbuf overflow count. */
#endif
	unsigned long mpd_ctrl;				/* (0x620) RW Magic packet control */
	unsigned long mpd_pw_ms;			/* (0x624) RW Magic packet password 47:32*/
	unsigned long mpd_pw_ls;			/* (0x628) RW Magic packet password 31:0*/
	unsigned long unused8[9];
	unsigned long mdf_ctrl;				/* (0x650) RW MAC DA filter control*/
	unsigned long mdf_addr[34];			/* (0x654 - 0x6d8) Desination MAC address registers */

}uniMacRegs;

#ifdef CONFIG_BRCM_HAS_GENET2
typedef struct tbufRegs
{
	unsigned long tbuf_ctrl;			/* (00) tx buffer control */
	unsigned long unused0;
	unsigned long tbuf_endian_ctrl;		/* (08) tx buffer endianess control*/
	unsigned long tbuf_bp_mc;			/* (0c) tx buffer backpressure mask and control*/
	unsigned long tbuf_pkt_rdy_thld;	/* (10) threshold for PKT_RDY , for jumbo frame, default 0x7C*/
	unsigned long tbuf_energy_ctrl;		/* (14) */
	unsigned long tbuf_ext_bp_stats;	/* (18) */
	unsigned long tbuf_tsv_mask0;
	unsigned long tbuf_tsv_mask1;
	unsigned long tbuf_tsv_status0;
	unsigned long tbuf_tsv_status1;
}tbufRegs;

typedef struct rbufRegs
{
	unsigned long rbuf_ctrl;			/* (00) rx buffer control register*/
	unsigned long unused0;
	unsigned long rbuf_pkt_rdy_thld;	/* (08) threshold which PKT_RDY asserted defualt 0x7c*/
	unsigned long rbuf_status;			/* (0c) rx buffer status.*/
	unsigned long rbuf_endian_ctrl;		/* (10) rx buffer endianess control register*/
	unsigned long rbuf_chk_ctrl;		/* (14) raw checksum control register */
	unsigned long rbuf_rxc_offset[8];	/* (18 - 34) rxc extraction offset registers, for filtering*/
	unsigned long unused1[18];
	unsigned long rbuf_ovfl_pkt_cnt;	/* (80) */
	unsigned long rbuf_err_cnt;			/* (84) */
	unsigned long rbuf_energy_ctrl;		/* (88) */
	
	unsigned long unused2[7];
	unsigned long rbuf_pd_sram_ctrl;	/* (a8) */
	unsigned long unused3[12];
	unsigned long rbuf_test_mux_ctrl;	/* (dc) */
}rbufRegs;

typedef struct hfbRegs
{
	unsigned long hfb_ctrl;
	unsigned long hfb_fltr_len[4];
}hfbRegs;

#else	/*! CONFIG_BRCM_HAS_GENET2 */
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
	unsigned long unused4[6];
	unsigned long test_mux_ctrl;		/* (dc) ENET test mux control register*/
}rbufRegs;
#endif
/* uniMac intrl2 registers */
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

/* Register block offset */
#define UMAC_GR_BRIDGE_REG_OFFSET	0x0040	/* uniMac register start from 0x468000, which is GENET_SYS */
#define UMAC_EXT_REG_OFFSET			0x0080	/* external register offset */
#define UMAC_INTRL2_0_REG_OFFSET	0x0200
#define UMAC_INTRL2_1_REG_OFFSET	0x0240
#define UMAC_RBUF_REG_OFFSET		0X0300
#define UMAC_UMAC_REG_OFFSET		0x0800
#define UMAC_HFB_OFFSET				0x1000

#ifdef CONFIG_BRCM_HAS_GENET2
#define UMAC_TBUF_REG_OFFSET		0x0600	
#define UMAC_RDMA_REG_OFFSET		0x3000
#define UMAC_TDMA_REG_OFFSET		0x4000
#define UMAC_HFB_REG_OFFSET			0x2000
#else
#define UMAC_RDMA_REG_OFFSET		0x2000
#define UMAC_TDMA_REG_OFFSET		0x3000
#endif

typedef struct SysRegs
{
	unsigned long sys_rev_ctrl;
	unsigned long sys_port_ctrl;
#ifdef CONFIG_BRCM_HAS_GENET2
	unsigned long rbuf_flush_ctrl;
	unsigned long tbuf_flush_ctrl;
#endif
}SysRegs;

typedef struct GrBridgeRegs
{
	unsigned long gr_bridge_rev;
	unsigned long gr_bridge_ctrl;
	unsigned long gr_bridge_sw_reset_0;
	unsigned long gr_bridge_sw_reset_1;
}GrBridgeRegs;

typedef struct ExtRegs
{
	unsigned long ext_pwr_mgmt;
	unsigned long ext_emcg_ctrl;
	unsigned long ext_test_ctrl;
#ifdef CONFIG_BRCM_HAS_GENET2
	unsigned long rgmii_oob_ctrl;
	unsigned long rgmii_ib_status;
	unsigned long rgmii_led_ctrl;
	unsigned long ext_genet_pwr_mgmt;
#else
	unsigned long ext_in_ctrl;
	unsigned long ext_fblp_ctrl;
	unsigned long ext_stat0;
	unsigned long ext_stat1;
	unsigned long ext_ch_ctrl[6];
#endif
}ExtRegs;

typedef struct rDmaRingRegs
{
	unsigned long rdma_write_pointer;
	unsigned long rdma_producer_index;
	unsigned long rdma_consumer_index;
	unsigned long rdma_ring_buf_size;
	unsigned long rdma_start_addr;
	unsigned long rdma_end_addr;
	unsigned long rdma_mbuf_done_threshold;
	unsigned long rdma_xon_xoff_threshold;
	unsigned long rdma_read_pointer;
	unsigned long unused[7];
}rDmaRingRegs;

typedef struct tDmaRingRegs
{
	unsigned long tdma_read_pointer;
	unsigned long tdma_consumer_index;
	unsigned long tdma_producer_index;
	unsigned long tdma_ring_buf_size;
	unsigned long tdma_start_addr;
	unsigned long tdma_end_addr;
	unsigned long tdma_mbuf_done_threshold;
	unsigned long tdma_flow_period;
	unsigned long tdma_write_pointer;
	unsigned long unused[7];
}tDmaRingRegs;

typedef struct rDmaRegs
{
	rDmaRingRegs rDmaRings[17];
#ifdef CONFIG_BRCM_HAS_GENET2
	unsigned long rdma_ring_cfg;
#endif
	unsigned long rdma_ctrl;
	unsigned long rdma_status;
	unsigned long unused;
	unsigned long rdma_scb_burst_size;
	unsigned long rdma_activity;
	unsigned long rdma_mask;
	unsigned long rdma_map[3];
	unsigned long rdma_back_status;
	unsigned long rdma_override;
	unsigned long rdma_timeout[17];
	unsigned long rdma_test;
	unsigned long rdma_debug;
}rDmaRegs;

typedef struct tDmaRegs
{
	tDmaRingRegs tDmaRings[17];
#ifdef CONFIG_BRCM_HAS_GENET2
	unsigned long tdma_ring_cfg;
#endif
	unsigned long tdma_ctrl;
	unsigned long tdma_status;
#ifndef CONFIG_BRCM_HAS_GENET2
	unsigned long unused;
#endif
	unsigned long tdma_scb_burst_size;
	unsigned long tdma_activity;
	unsigned long tdma_mask;
	unsigned long tdma_map[3];
	unsigned long tdma_back_status;
	unsigned long tdma_override;
	unsigned long tdma_rate_limit_ctrl;
	unsigned long tdma_arb_ctrl;
	unsigned long tdma_priority[3];
	unsigned long tdma_test;
	unsigned long tdma_debug;
	unsigned long tdma_rate_adj;
}tDmaRegs;


#endif /* __ASSEMBLY__ */

#ifdef __cplusplus
}
#endif

#endif
