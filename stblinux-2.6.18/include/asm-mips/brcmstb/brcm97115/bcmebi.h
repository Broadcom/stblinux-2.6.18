/***********************************************************************/
/*                                                                     */
/*   MODULE:  bcmebi.h                                                 */
/*   DATE:    96/12/19                                                 */
/*   PURPOSE: Definitions for EBI block                                */
/*                                                                     */
/***********************************************************************/

/*     Copyright (c) 1999-2005, Broadcom Corporation
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

#ifndef BCMEBI_H
#define BCMEBI_H

#ifdef _cplusplus
extern "C" {
#endif

#define CS0BASE         0x00
#define CS0CNTL         0x04
#define CS1BASE         0x08
#define CS1CNTL         0x0c
#define CS2BASE         0x10
#define CS2CNTL         0x14
#define CS3BASE         0x18
#define CS3CNTL         0x1c
#define CS4BASE         0x20
#define CS4CNTL         0x24
#define CS5BASE         0x28
#define CS5CNTL         0x2c
#define CS6BASE         0x30
#define CS6CNTL         0x34
#define CS7BASE         0x38
#define CS7CNTL         0x3c
#define CS8BASE         0x40
#define CS8CNTL         0x44

#define EBICONFIG       0x100 

/*
# CSxBASE settings
#   Size in low 4 bits
#   Base Address for match in upper 24 bits
*/
#define EBI_SIZE_8K         0
#define EBI_SIZE_16K        1
#define EBI_SIZE_32K        2
#define EBI_SIZE_64K        3
#define EBI_SIZE_128K       4
#define EBI_SIZE_256K       5
#define EBI_SIZE_512K       6
#define EBI_SIZE_1M         7
#define EBI_SIZE_2M         8
#define EBI_SIZE_4M         9
#define EBI_SIZE_8M         10
#define EBI_SIZE_16M        11
#define EBI_SIZE_32M        12
#define EBI_SIZE_64M        13
#define EBI_SIZE_128M       14
#define EBI_SIZE_256M       15

/* CSxCNTL settings */
#define EBI_MEMIO           0x80000000  /* .. Memory/IO */
#define EBI_TAWAIT          0x40000000
#define EBI_WP              0x20000000
#define EBI_WAIT_STATES     0x1f000000  /* .. mask for wait states */
#define ZEROWT              0x00000000  /* ..  0 WS */
#define ONEWT               0x01000000  /* ..  1 WS */
#define TWOWT               0x02000000  /* ..  2 WS */
#define THREEWT             0x03000000  /* ..  3 WS */
#define FOURWT              0x04000000  /* ..  4 WS */
#define FIVEWT              0x05000000  /* ..  5 WS */
#define SIXWT               0x06000000  /* ..  6 WS */
#define SEVENWT             0x07000000  /* ..  7 WS */
#define EBI_WTST_SHIFT      24          /* .. for shifting wait states */
#define EBI_THOLD           0x00f00000
#define EBI_THOLD_SHIFT     20
#define EBI_TSETUP          0x000f0000
#define EBI_TSETUP_SHIFT    16
#define EBI_CSHOLD          0x00008000
#define EBI_SPLITCS         0x00004000
#define EBI_MASKEN          0x00002000
#define EBI_NESAMPLE        0x00001000
#define EBI_M68K            0x00000800
#define EBI_REV_END         0x00000400  /* .. Reverse Endian */
#define EBI_RE              0x00000400  /* .. Reverse Endian */
#define EBI_FIFO            0x00000200  /* .. enable fifo */
#define EBI_TS_SEL          0x00000100  /* .. drive tsize, not bs_b */
#define EBI_TS_TA_MODE      0x00000080  /* .. use TS/TA mode */
#define EBI_POLARITY        0x00000040  /* .. set to invert chip select polarity */
#define EBI_WREN            0x00000020      /* .. enable posted writes */
#define EBI_WORD_WIDE       0x00000010  /* .. 16-bit peripheral, else 8 */
#define EBI_MSINH           0x00000008
#define EBI_MSINL           0x00000004
#define EBI_TSEN            0x00000002
#define EBI_ENABLE          0x00000001  /* .. enable this range */

/* EBICONFIG settings */
#define EBI_MASTER_ENABLE   0x80000000  /* allow external masters */
#define EBI_EXT_MAST_PRIO   0x40000000  /* maximize ext master priority */
#define EBI_CTRL_ENABLE     0x20000000
#define EBI_TA_ENABLE       0x10000000

/* EBI DMA control register settings */
#define EBI_FIXED_ADDR          0x01000000
#define EBI_TX_INV_IRQ_EN       0x00080000
#define EBI_RX_INV_IRQ_EN       0x00040000
#define EBI_TX_PKT_DN_IRQ_EN    0x00020000
#define EBI_RX_PKT_DN_IRQ_EN    0x00010000
#define EBI_TX_INV_CLR          0x00001000
#define EBI_RX_INV_CLR          0x00000800
#define EBI_CHAINING            0x00000400
#define EBI_EXT_MODE            0x00000200
#define EBI_HALF_WORD           0x00000100
#define EBI_TX_PKT_DN_CLR       0x00000080
#define EBI_RX_PKT_DN_CLR       0x00000040
#define EBI_TX_BUF_DN_CLR       0x00000020
#define EBI_RX_BUF_DN_CLR       0x00000010
#define EBI_TX_BUF_DN_IRQ_EN    0x00000008
#define EBI_RX_BUF_DN_IRQ_EN    0x00000004
#define EBI_TX_EN               0x00000002
#define EBI_RX_EN               0x00000001

/* EBI Rx lenght flag */
#define EBI_RX_EOP              0x00010000

/* EBI DMA status register settings */
#define EBI_TX_INV_DESC         0x00000020
#define EBI_RX_INV_DESC         0x00000010
#define EBI_TX_PKT_DN           0x00000008
#define EBI_RX_PKT_DN           0x00000004
#define EBI_TX_BUF_DN           0x00000002
#define EBI_RX_BUF_DN           0x00000001

#ifndef _ASMLANGUAGE

typedef struct EbiChipSelect {
  unsigned long base;                   /* base address in upper 24 bits */
  unsigned long config;
} EbiChipSelect;


typedef struct EbiRegisters {
  EbiChipSelect cs[9];                  /* size chip select configuration */
  unsigned long reserved[46];
  unsigned long ebi_config;             /* (0x100) configuration */
  unsigned long addr_page;				/* (0x104) address page register */
  unsigned long mbist_control;			/* (0x108) mbist control */
  unsigned long mbist_status;			/* (0x10c) mbist status */
  unsigned long reserverd1[60];
  unsigned long dma_control;			/* (0x200) dma control */
  unsigned long dma_rx1_addr;		/* (0x204) dma rx 1 start address */
  unsigned long dma_rx1_length;
  unsigned long dma_tx1_addr;		/* (0x20c) dma tx 1 start address */
  unsigned long dma_tx1_length;
  unsigned long dma_rx2_addr;		/* (0x214) dma rx 2 start address */
  unsigned long dma_rx2_length;
  unsigned long dma_tx2_addr;		/* (0x21c) dma tx 2 start address */
  unsigned long dma_tx2_length;
  unsigned long dma_status;			/* (0x224) dma status */
} EbiRegisters;

#define EBIC ((volatile EbiRegisters * const) EBI_ADR_BASE)

#endif /* _ASMLANGUAGE */

#ifdef _cplusplus
}
#endif

#endif
