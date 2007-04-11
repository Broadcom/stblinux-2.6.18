/***********************************************************************/
/*                                                                     */
/*   MODULE:  bcm7319.h                                                */
/*   DATE:    96/12/19                                                 */
/*   PURPOSE: Define addresses of major hardware components of         */
/*            BCM7319                                                  */
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


#ifndef BCM7320_H
#define BCM7320_H

#ifdef _cplusplus
extern "C" {
#endif


/**********************************************************************
 *generic GET_FIELD & SET_FIELD
 **********************************************************************/
/**
 * m = memory, c = core, r = register, f = field, d = data.
 */
#if !defined(GET_FIELD) && !defined(SET_FIELD)
#define BRCM_ALIGN(c,r,f)   c##_##r##_##f##_ALIGN
#define BRCM_BITS(c,r,f)    c##_##r##_##f##_BITS
#define BRCM_MASK(c,r,f)    c##_##r##_##f##_MASK
#define BRCM_SHIFT(c,r,f)   c##_##r##_##f##_SHIFT

#define GET_FIELD(m,c,r,f) \
	((((m) & BRCM_MASK(c,r,f)) >> BRCM_SHIFT(c,r,f)) << BRCM_ALIGN(c,r,f))

#define SET_FIELD(m,c,r,f,d) \
	((m) = (((m) & ~BRCM_MASK(c,r,f)) | ((((d) >> BRCM_ALIGN(c,r,f)) << \
	 BRCM_SHIFT(c,r,f)) & BRCM_MASK(c,r,f))) \
	)

#define SET_TYPE_FIELD(m,c,r,f,d) SET_FIELD(m,c,r,f,c##_##d)

#define SET_NAME_FIELD(m,c,r,f,d) SET_FIELD(m,c,r,f,c##_##r##_##f##_##d)

#define SET_VALUE_FIELD(m,c,r,f,d) SET_FIELD(m,c,r,f,d)

#endif /* GET & SET */


#define MINITITAN_ADR_BASE	0xbae00000    /* Minititan 1MB */
#define IDE_ADR_BASE		0xbafa0000    /* first IDE 64KB */
/* #define IDE2_ADR_BASE		0xbafb0000  */  /* first IDE 64KB */
#define XPT_ADR_BASE		0xbafc0000    /* Transport 32KB */

#define VIDEO_ADR_BASE          0xbafc8000    /* Video display */
#define BGE_ADR_BASE		0xbafc8400    /* BGE 32KB */
#define PCM_AUDIO_ADR_BASE      0xbafc8800    /* PCM audio */
#define MEM_DMA_ADR_BASE        0xbafc8f00    /* Memory DMA */
#define ADP_ADR_BASE		0xbafd0000    /* Audio 16KB */
/* #define BTSC_ADR_BASE           0xbafd4000 */

#define SDS_ADR_BASE		0xbafd4000    /* satellite downstream 16KB */
#define UPG_ADR_BASE		0xbafe0000    /* UPG, 512B */
#define USB_HOST2_ADR_BASE	0x1afe0200    /* USB Host #2, 256B */
#define INTC_ADR_BASE		0xbafe0600    /* interrupt controller 256B */
#define TIMR_ADR_BASE		0xbafe0700    /* timers 256B */
#define BRIDGE_ADR_BASE		0xbafe0900    /* Bridge ACC, 256B */
#define DMA_ADR_BASE		0xbafe6000    /* DMA controller 4KB */
#define EBI_ADR_BASE		0xbafe7000    /* EBI 4K */
#define TM_TOP_ADR_BASE		0xbafe8000    /* TM_TOP, 256B */
#define USB_HOST_ADR_BASE	0x1afe8100    /* USB Host, 256B */
#define MODEM_ADR_BASE		0xbafe8500    /* soft modem 256B */
#define RF_MOD_ADR_BASE		0xbafe8600    /* RF modulator 256B */
#define PROBE_ADR_BASE		0xba200000    /* probe (ejtag) */
#define DSU_ADR_BASE		0xba300000    /* dsu (ejtag) */

#define DDR_BASE_ADR_REG	MINITITAN_ADR_BASE 

#include "bcmebi.h"
#include "bcmupg.h"
#include "bcmtimer.h"
#include "bcmintc.h"
#include "bcmdma.h"
#include "bcmsds.h"
#include "bcmbridge.h"


#ifdef _cplusplus
}
#endif

#endif
