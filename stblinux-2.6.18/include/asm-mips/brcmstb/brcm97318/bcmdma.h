/***********************************************************************/
/*                                                                     */
/*   MODULE:  bcmdma.h                                                 */
/*   DATE:    96/12/19                                                 */
/*   PURPOSE: Definitions for DMA block                                */
/*                                                                     */
/***********************************************************************/

/***************************************************************************
 *     Copyright (c) 1999-2005, Broadcom Corporation
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
 */

#ifndef BCMDMA_H
#define BCMDMA_H

#ifdef _cplusplus
extern "C" {
#endif


/* DMA channel assignments */
#define EBI_RX_CHAN             1
#define EBI_TX_CHAN             2
#define MODEM_RX_CHAN           3
#define MODEM_TX_CHAN			4

#ifndef _ASMLANGUAGE
/*
 ***********************************************************************
 *
 * DMA Channel: DMA over ISB between peripherals and system memory.
 *
 * Chan 0	: reserverd
 * Chan 1	: EBI Rx (from EBI to system memory)
 * Chan 2	: EBI Tx
 * CHan 3-8	: Davic
 * Chan 9-10: Softmodem
 *
 ***********************************************************************
*/
typedef struct DmaBlock {
  unsigned long  intStat;
  unsigned long  byteRev;
  unsigned long  wordRev;
} DmaBlock;

#define DMA_BLOCK ((volatile DmaBlock *)DMA_ADR_BASE)

typedef struct DmaChannel {
  unsigned long  cfg;                    /* (00) assorted configuration */
#define          DMA_FLOWC_EN   0x00000010      /* flow control enable */
#define          DMA_WRAP_EN    0x00000008      /* use DMA_WRAP bit */
#define          DMA_CHAINING   0x00000004      /* chaining mode */
#define          DMA_STALL      0x00000002      
#define          DMA_ENABLE     0x00000001      /* set to enable channel */
  unsigned long  maxBurst;               /* (04) max burst length permitted */
                                        /*      non-chaining / chaining */
  unsigned long  startAddr;              /* (08) source addr  / ring start address */
  unsigned long  length;                 /* (0c) xfer len     / ring len */
#define          DMA_KICKOFF    0x80000000      /* start non-chaining xfer */

  unsigned long  bufStat;                /* (10) buffer status for non-chaining */
  unsigned long  intStat;                /* (14) interrupts control and status */
  unsigned long  intMask;                /* (18) interrupts mask */
#define         DMA_BUFF_DONE   0x00000001      /* buffer done */
#define         DMA_DONE        0x00000002      /* packet xfer complete */
#define         DMA_NO_DESC     0x00000004      /* no valid descriptors */

/* DMA HW bits are clugy in this version of chip (mask/status shifted) */
#define         DMA_BUFF_DONE_MASK  0x00000004      /* buffer done */
#define         DMA_DONE_MASK       0x00000001      /* packet xfer complete */
#define         DMA_NO_DESC_MASK    0x00000002      /* no valid descriptors */

  unsigned long  fcThreshold;            /* (1c) flow control threshold */
  unsigned long  fcBufAlloc;             /* (20) flow control buffer allocation */
  unsigned long  ringOffset;             /* (24) ring offset register */
  unsigned long  unused[6];              /* (28-3c) pad to next descriptor */
} DmaChannel;

/* register offsets, useful for ebi master access */
#define DMA_CFG                 0
#define DMA_MAX_BURST           4
#define DMA_START_ADDR          8
#define DMA_LENGTH              12
#define DMA_BUF_STAT            16
#define DMA_INT_STAT            20
#define DMA_INT_MASK            24
#define DMA_FC_THRESHOLD        28
#define DMA_FC_ALLOC            32
#define DMA_RING_OFFSET         36

#define DMA_CHAN_OFFSET		    0x40

#define DMA_INTR_GLOBAL		((unsigned int *) DMA_ADR_BASE)

#define DMA_CHAN ((volatile DmaChannel *)DMA_ADR_BASE)

/* paste in your program ...
DmaChannel *dma1 = dmaChannels[1];
*/

/*
** DMA Descriptor
*/
typedef struct DmaDesc {
  unsigned short length;                /* in bytes of data in buffer */
  unsigned short status;                /* buffer status */
#define          DMA_OWN        0x8000  /* cleared by DMA, set by SW */
#define          DMA_EOP        0x0800  /* last buffer in packet */
#define          DMA_SOP        0x0400  /* first buffer in packet */
#define          DMA_WRAP       0x0200  /* */
#define          DMA_APPEND_CRC 0x0100  /* .. for emac tx */
#define          DATA_FLAG      0x0100  /* .. for secmod rx */
#define          AUTH_FAIL_FLAG 0x0100  /* .. for secmod tx */

/* EMAC Descriptor Status definitions */
#define          EMAC_UNDERRUN  0x4000   /* Tx underrun */
#define          EMAC_MISS      0x0080  /* framed address recognition failed (promiscuous) */
#define          EMAC_BRDCAST   0x0040  /* DA is Broadcast */
#define          EMAC_MULT      0x0020  /* DA is multicast */
#define          EMAC_LG        0x0010  /* frame length > RX_LENGTH register value */
#define          EMAC_NO        0x0008  /* Non-Octet aligned */
#define          EMAC_RXER      0x0004  /* RX_ERR on MII while RX_DV assereted */
#define          EMAC_CRC_ERROR 0x0002  /* CRC error */
#define          EMAC_OV        0x0001  /* Overflow */

  unsigned long  address;                        /* address of data */
} DmaDesc;
#define DMA_DESC_LENGTH 0
#define DMA_DESC_STATUS 2
#define DMA_DESC_ADDR   4

#endif /* _ASMLANGUAGE */

#ifdef _cplusplus
}
#endif

#endif
