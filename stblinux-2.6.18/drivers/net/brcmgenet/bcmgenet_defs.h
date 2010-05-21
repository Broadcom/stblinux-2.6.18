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
/* uniMac common macro definations(chip revision independent, idealy!)
 * 
 * NOTE: the comments "group start" "group end" "non-proc" are for assisting 
 * perl script parsing, DONT'T delete it.
 * 
 * Revision: 10/03/2008, L.Sun, created.
 */
#ifndef __UNIMAC_DEFS_H__
#define __UNIMAC_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define UMAC_SPEED_10			0				/* 10M bps */
#define UMAC_SPEED_100			1				/* 100M bps */
#define UMAC_SPEED_1000			2				/* 1000M bps */
#define UMAC_SPEED_2500			3				/* 2500M bps */

#define PORT_MODE_INT_EPHY		0
#define PORT_MODE_INT_GPHY		1
#define PORT_MODE_EXT_EPHY		2
#define PORT_MODE_EXT_GPHY		3

/* umac register group start */
/* reg: umac->hdBkpCtrl */
#define	HD_FC_EN				(1 << 0 )		/* Enable half-duplex flow control.*/
#define HD_FC_BKOFF_OK			(1 << 1 )		/* Enable back-off algorithm. */
#define IPG_CONFIG_RX_SHIFT		2				/* IPG config shift */
#define IPG_CONFIG_RX_MASK		0x1F
/* reg: umac->cmd */
#define CMD_TX_EN				(1 << 0 )		/* Enable Rx */
#define CMD_RX_EN				(1 << 1 )		/* Enable Tx */
#define CMD_SPEED_SHIFT			2				/* Speed shift */
#define CMD_SPEED_MASK			3				/* speed mask*/
#define CMD_PROMISC				(1 << 4 )		/* Enable promiscous . */
#define CMD_PAD_EN				(1 << 5 )		/* Remove padding. */
#define CMD_CRC_FWD				(1 << 6 )		/* Forward CRC to host.*/
#define CMD_PAUSE_FWD			(1 << 7 )		/* Forward pause frame to host.*/
#define CMD_RX_PAUSE_IGNORE		(1 << 8 )		/* Ignore received pause frame */
#define CMD_TX_ADDR_INS			(1 << 9 )		/* Insert source MAC address from MAC_0 or MAC_1 */
#define CMD_HD_EN				(1 << 10)		/* Enable Half-duplex .*/
#define CMD_SW_RESET			(1 << 13)		/* Reset Tx and Rx engine. */
#define CMD_LCL_LOOP_EN			(1 << 15)		/* Enable GMII/MII loopback. */
#define CMD_AUTO_CONFIG			(1 << 22)		/* Enable auto config.*/
#define CMD_CNTL_FRM_EN			(1 << 23)		/* MAC Control frame enable.*/
#define CMD_NO_LEN_CHK			(1 << 24)		/* Don't check payload length/type field */
#define CMD_RMT_LOOP_EN			(1 << 25)		/* Enable remote loopback.*/
#define CMD_PRBL_EN				(1 << 27)		/* Enable extract/insert of EFM header.*/
#define CMD_TX_PAUSE_IGNORE		(1 << 28)		/* Ignore pause frame transmitting request.*/
#define CMD_TX_RX_EN			(1 << 29)		/* When 0, Enable Tx/Rx after auto-config.*/
#define CMD_RUNT_FILTER_DIS		(1 << 30)		/* Disable Rx side RUNT filter.*/
/* reg: umac->mode */
#define MODE_HD					(1 << 0 )		/* half duplex */
#define MODE_RX_PAUSE_EN		(1 << 3 )		/* Rx pause enabled.*/
#define MODE_TX_PAUSE_EN		(1 << 4 )		/* Tx pause enabled.*/
/* reg: umac->macsec_ctrl */
#define TX_LANUCH_EN			(1 << 0 )		/* ????*/
#define TX_CRC_CORRUPT_EN		(1 << 4 )		/* Enable CRC corruption on Tx packets */
#define TX_CRC_PROGRAM			(1 << 5 )		/* Replace Tx FCS with programmed FCS */
#define PAUSE_CTRL_EN			(1 << 17)
/* reg: umac->ts_status */
#define TX_TS_FIFO_FULL			(1 << 0 )		/* Transmit timestamp FIFO is full */
#define TX_TS_FIFO_EMPTY		(1 << 1 )		/* Transmit timestamp FIFO is empty.*/
/* reg: umac->txfifo_status */
#define TX_FIFO_OVERRUN			(1 << 1 )
#define TX_FIFO_UNDERRUN		(1 << 0 )
/* reg: umac->ppp_ctrl */
#define PPP_EN_TX				(1 << 0 )
#define PPP_EN_RX				(1 << 1 )
#define PPP_FORCE_XON			(1 << 2 )
/* reg: umac_ppp_refresh_ctrl */
#define PPP_REFRESH_EN			(1 << 0 )
/* reg: umac->mib_ctrl */
#define MIB_RESET_RX			(1 << 0 )		/* Reset RX status counter.*/
#define MIB_RESET_RUNT			(1 << 1 )		/* Reset RUNT statistics counter.*/
#define MIB_RESET_TX			(1 << 2 )		/* Reset Tx status conter.*/
/* reg: umac->mpd_ctrl */
#define MPD_EN					(1 << 0 )		/* Enable Magic packet detection */
#define MPD_PW_EN				(1 << 27) 		/* Enable password for magic packet detection*/
#define MPD_MSEQ_LEN_SHIFT		16			
#define MPD_MSEQ_LEN_MASK		0xFF			
/* reg: umac->mdio_cmd, non-proc export */
#define MDIO_START_BUSY			(1 << 29)		/* start MDIO transaction.*/
#define MDIO_READ_FAIL			(1 << 28) 		/* PHY doesn't reply read command. */
#define MDIO_RD					(2 << 26) 		/* READ for clause 22 */
#define MDIO_WR					(1 << 26)		/* WRITE for clause 22 */
#define MDIO_PMD_SHIFT 			21				/* Phy addr(clause 22)/port(clause 45) shift */
#define MDIO_PMD_MASK			0x1F
#define MDIO_REG_SHIFT 			16				/* Register addr(clause 22)/device addr(clause 45) shift */
#define MDIO_REG_MASK			0x1F			/* PHY addr and Reg addr mask.*/
/* umac register group end */

/* rbuf register group start */
/* reg: rbuf->rbuf_ctrl */
#define RBUF_64B_EN				(1 << 0 )		/* prepend 64B status block to each packet */
#define RBUF_ALIGN_2B			(1 << 1 )		/* align IP header to 32 bit boundary by prepending 2B to the packet.*/
#define RBUF_BAD_DIS			(1 << 2 )		/* discard bad packet (when MAC_RXERR asserted)*/
/* reg: rbuf->rbuf_status */
#define RBUF_STATUS_WOL			(1 << 0 )		/* ENET system in WOL status */
#define RBUF_STATUS_MPD_INTR_ACTIVE		(1 << 1 )	/* MPD(Magic Packet Detector) packet detected*/
#define RBUF_STATUS_ACPI_INTR_ACTIVE	(1 << 2 )	/* ACPI (Pattern Matching )packet detected */
/* reg: rbuf->rbuf_endian_ctrl */
#define RBUF_SWAP_EN			(1 << 2 )		/* swap byte*/
#define RBUF_BIG_ENDIAN_MODE	(1 << 0 )		/* don't swap byte */
#define RBUF_REG_MODE			(1 << 1 )		/* ignore endianess setting pin */
/* reg: rbuf->rbuf_chk_ctrl */
#define RBUF_RXCHK_EN			(1 << 0 )		/* enable raw checksum calculation */
#define RBUF_SKIP_FCS			(1 << 4 )		/* skip last 4B when doing checksum */
/* reg: rbuf->rbuf_hfb_ctrl */
#define RBUF_HFB_FILTER_EN_SHIFT	16
#define RBUF_HFB_FILTER_EN_MASK	0xffff0000
#define RBUF_HFB_EN				(1 << 0 )		/* Enable HFB block.*/
#define RBUF_HFB_256B			(1 << 1 )		/* 8 filter of 256B */
#define RBUF_ACPI_EN			(1 << 2 )		/* switch to acpi .*/
/* reg: rbuf->rbuf_fltr_len */
#define RBUF_FLTR_LEN_MASK		0xFF
#define RBUF_FLTR_LEN_SHIFT		8
/* reg: rbuf->tbuf_bp_mc */
#define TBUF_CPU_BP				(1 << 8 )		/* cpu assert , stop packet transmission */
/* reg: rbuf->rgmii_oob_ctrl */
#define RGMII_MODE_EN			(1 << 0 )		/* Enable RGII interface */
#define RGMII_LINK				(1 << 1 )		/* link indication*/
#define OOB_DISABLE				(1 << 2 )		
/* rbuf register group end */

/* ext register group start */
/* reg: ext->ephy_pwr_mgmt */
#define EXT_PWR_DOWN_BIAS		(1 << 0 )
#define EXT_PWR_DOWN_DLL		(1 << 1 )
#define EXT_PWR_DOWN_PHY		(1 << 2 )
#define EXT_PWR_DN_EN_LD		(1 << 3 )
#define EXT_ENERGY_DET			(1 << 4 )
#define EXT_IDDQ_FROM_PHY		(1 << 5 )
#define EXT_PHY_RESET			(1 << 8 )
#define EXT_ENERGY_DET_MASK		(1 << 12 )

/* reg: ext->emcg_ctrl */
#define EXT_Ck25_EN				(1 << 0 )
#define EPHY_CLK_SEL			(1 << 4 )

/* intrl2_0 register group start */
/* reg: intrl2_0->cpu_mask_status, others are same */
#define UMAC_IRQ_SCB			(1 << 0 )
#define UMAC_IRQ_EPHY			(1 << 1 )
#define UMAC_IRQ_PHY_DET_R		(1 << 2 )
#define UMAC_IRQ_PHY_DET_F		(1 << 3 )
#define UMAC_IRQ_LINK_UP		(1 << 4 )
#define UMAC_IRQ_LINK_DOWN		(1 << 5 )
#define UMAC_IRQ_UMAC			(1 << 6 )
#define UMAC_IRQ_UMAC_TSV		(1 << 7 )
#define UMAC_IRQ_TBUF_UNDERRUN	(1 << 8 )
#define UMAC_IRQ_RBUF_OVERFLOW	(1 << 9 )
#define UMAC_IRQ_HFB_SM			(1 << 10 )
#define UMAC_IRQ_HFB_MM			(1 << 11 )
#define UMAC_IRQ_MPD_R			(1 << 12)
#define UMAC_IRQ_RXDMA_MBDONE	(1 << 13)
#define UMAC_IRQ_RXDMA_PDONE	(1 << 14)
#define UMAC_IRQ_RXDMA_BDONE	(1 << 15)
#define UMAC_IRQ_TXDMA_MBDONE	(1 << 16)
#define UMAC_IRQ_TXDMA_PDONE	(1 << 17)
#define UMAC_IRQ_TXDMA_BDONE	(1 << 18)

#define DMA_RW_POINTER_MASK		0x1FF
/* reg: rDmaRingRegs->rdma_producer_index */
#define DMA_PRODUCER_INDEX_DISCARD_CNT_MASK		0xFFFF		/* For RDMA only */
#define DMA_PRODUCER_INDEX_DISCARD_CNT_SHIFT	16			/* For RDMA only */
#define DMA_BUFFER_DONE_CNT_MASK				0xFFFF		/* For TDMA only */
#define DMA_BUFFER_DONE_CNT_SHIFT				16			/* For TDMA only */
#define DMA_PRODUCER_INDEX_MASK					0xFFFF
#define DMA_CONSUMER_INDEX_MASK					0xFFFF
/* reg: rDmaRingRegs->rdma_ring_buf_size */
#define DMA_RING_SIZE_MASK						0xFFFF
#define DMA_RING_SIZE_SHIFT						16
#define DMA_RING_BUFFER_SIZE_MASK				0xFFFF
/* reg: rDmaRingRegs->rdma_mbuf_done_threshold */
#define DMA_INTR_THRESHOLD_MASK					0x00FF
/* reg: rDmaRingRegs->rdma_xon_xoff_threshold */
#define DMA_XON_THREHOLD_MASK					0xFFFF		/* For RDMA only */
#define DMA_XOFF_THRESHOLD_MASK					0xFFFF		/* For RDMA only */
#define DMA_XOFF_THRESHOLD_SHIFT				16			/* For RDMA only */
/* reg: dDmaRingRegs->tdma_flow_period */
#define DMA_FLOW_PERIOD_MASK					0xFFFF		/* For TDMA only */
#define DMA_MAX_PKT_SIZE_MASK					0xFFFF		/* For TDMA only */
#define DMA_MAX_PKT_SIZE_SHIFT					16			/* For TDMA only */

/* rdma register group start */
/* reg: rDmaRegs->rdma_ctrl */
#define DMA_EN									( 1 << 0)
#define DMA_RING_BUF_EN_SHIFT					0x01
#define DMA_RING_BUF_EN_MASK					0xFFFF
#define DMA_TSB_SWAP_EN							( 1 << 20)	/* For TDMA only */
/* reg : rDmaRegs->rdma_status */
#define DMA_DISABLED							( 1 << 0)
#define DMA_DESC_RAM_INIT_BUSY 					( 1 << 1)
/* reg: rDmaRegs->rdma_scb_burst_size */
#define DMA_SCB_BURST_SIZE_MASK					0x1F
/* reg: rDmaRegs->rdma_activity */
#define DMA_ACTIVITY_VECTOR_MASK				0x1FFFF
/* reg: rDmaRegs->rdma_mask */
#define DMA_BACKPRESSURE_MASK					0x1FFFF
#define DMA_PFC_ENABLE							( 1 << 31)
/* reg: rDmaRegs->rdma_map non-proc export*/
#define DMA_MAP_MASK							0x0F
#define DMA_MAP_SHIFT							4
/* reg: rDmaRegs->rdma_back_status */
#define DMA_BACKPRESSURE_STATUS_MASK			0x1FFFF
/* reg: rDmaRegs->rdma_override */
#define DMA_LITTLE_ENDIAN_MODE					( 1 << 0)
#define DMA_REGISTER_MODE						( 1 << 1)
/* reg: rDmaRegs->rdma_timeout non-proc export */
#define DMA_TIMEOUT_MASK						0xFFFF		/* For RDMA only */

#define DMA_RATE_LIMIT_EN_MASK					0xFFFF		/* For TDMA only */
#define DMA_ARBITER_MODE_MASK					0x03		/* For TDMA only */
#define DMA_RING_BUF_PRIORITY_MASK				0x1F		/* For TDMA only */
#define DMA_RING_BUF_PRIORITY_SHIFT				5			/* For TDMA only */
#define DMA_RATE_ADJ_MASK						0xFF		/* For TDMA only */

/*-------------------Dma Descriptor defines ------------------------------------*/
/* reg : none , non-proc export (not for registers ) */
/* Tx/Rx Dma Descriptor common bits*/
#define DMA_BUFLENGTH_MASK		0x0fff
#define DMA_BUFLENGTH_SHIFT		16
#define DMA_OWN					0x8000		/* cleared by DMA, set by SW , obsolete for GENET */
#define DMA_EOP					0x4000		/* last buffer in packet */
#define DMA_SOP					0x2000		/* First buffer in packet, obsolete for GENET */
#define DMA_WRAP				0x1000		/* Last BD */
/* Tx specific Dma descriptor bits */
#define DMA_TX_UNDERRUN			0x0200		/* Tx fifo underrun, set by DMA */
#define DMA_TX_APPEND_CRC		0x0040
#define DMA_TX_OW_CRC			0x0020		/* overrite CRC */
#define DMA_TX_DO_CSUM			0x0010		/* calculate checksum */
#define DMA_TX_QTAG_MASK		0x001F		/* bit 4-0 OTAG, for MoCA classification.*/
#define DMA_TX_QTAG_SHIFT		7

/* Rx Specific Dma descriptor bits */
#define DMA_RX_BRDCAST			0x0040		/* DA is broadcast.*/
#define DMA_RX_MULT				0x0020		/* DA is multicast */
#define DMA_RX_LG				0x0010		/* frame length > RX_LENGTH register value */
#define DMA_RX_NO				0x0008		/* Non-octet aligned */
#define DMA_RX_RXER				0x0004		/* RX_ERR on MII while RX_DV asserted */
#define DMA_RX_CRC_ERROR		0x0002		/* CRC error */
#define DMA_RX_OV				0x0001		/* overflow */
#define DMA_RX_FI_MASK			0x001F		/* filter index mask */
#define DMA_RX_FI_SHIFT			0x0007		/* filter index right shift */
#define DMA_DESC_ALLOC_MASK		0x00FF

#define DMA_ARBITER_RR			0x00		/* RR arbitration */
#define DMA_ARBITER_WRR			0x01		/* Weighted RR arbitration */
#define DMA_ARBITER_SP			0x02		/* Strict Priority Arbitration */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __UNIMAC_DEFS_H__ */
