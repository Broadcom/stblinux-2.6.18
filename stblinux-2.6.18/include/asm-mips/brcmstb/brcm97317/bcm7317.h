/***********************************************************************/
/*                                                                     */
/*   MODULE: 7317_common.h                                             */
/*   DATE:    96/12/19                                                 */
/*   PURPOSE: Define addresses of major hardware components of         */
/*            BCM711X                                                  */
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
 ***********************************************************************/

#ifndef __BCM7317_H
#define __BCM7317_H

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

/**********************************************************************
 * Base addresses
 **********************************************************************/
#define MINITITAN_ADR_BASE	0xffe00000    /* Minititan 1MB, 0xfffe,0000-0xffef0000 */
#define IDE_ADR_BASE	0xfffb0000    /* IDE 64KB, 0xfffb,0000-0xfffb,ffff */
#define XPT_ADR_BASE	0xfffc0000    /* Transport 32KB, 0xfffc,0000-0xfffc,7fff */
#define VIDEO_ADR_BASE	0xfffc8000	  /* Video Display	*/
#define BGE_ADR_BASE	0xfffc8c00    /* Graphic 32KB, 0xfffc,8000-0xfffc,ffff */
#define PCM_AUDIO_ADR_BASE 0xfffc8800 /* PCM Audio	*/	
#define MEM_DMA_ADR_BASE 0xfffc8f00	  /* Memory DMA	*/
#define ADP_ADR_BASE	0xfffd0000    /* Audio 16KB, 0xfffd,0000-0xfffd,3fff */
#define BTSC_ADR_BASE	0xfffd4000    /* BTSC, 16KB, 0xfffd,4000-0xfffd,7fff*/

#define UPG_ADR_BASE	0xfffe0000    /* UPG, 512B, 0xfffe,0000-0xfffe,01ff */
#define QAM_ADR_BASE	0xfffe0000    /* QAM Base */
#define QAM_OB_ADR_BASE	0xfffe0200    /* QAM OB, 256B, 0xfffe,0200-0xfffe,02ff */
#define QAM_IB_ADR_BASE	0xfffe0300    /* QAM IB, 256B, 0xfffe,0300-0xfffe,03ff */
#define US_ADR_BASE		0xfffe0400    /* Upstream, 512B, 0xfffe,0400-0xfffe,05ff */
#define INTC_ADR_BASE	0xfffe0600    /* interrupts controller registers */
#define TIMR_ADR_BASE	0xfffe0700    /* timer registers */
#define BRIDGE_ADR_BASE	0xfffe0900    /* Bridge ACC, 256B, 0xfffe,0900-0xfffe,09ff */
#define DAVIC_ADR_BASE	0xfffe0a00    /* Davic MAC, 512B, 0xfffe,0a00-0xfffe,0bff */
#define DMA_ADR_BASE	0xfffe6000    /* DMA control registers */
#define EBI_ADR_BASE	0xfffe7000    /* EBI control registers */
#define TM_TOP_ADR_BASE	0xfffe8000    /* TM_TOP, 256B */
#define USB_HOST_ADR_BASE 0xfffe8100  /* USB Host, 256B */
#define USB_DEV_ADR_BASE  0xfffe8200  /* USB Device, 256B */
#define PID_FIL_ADR_BASE 0xfffe8400	  /* PID Filter	*/
#define MODEM_ADR_BASE	0xfffe8500    /* soft modem, 256B*/
#define PROBE_ADR_BASE	0xff200000    /* probe (ejata) */
#define DSU_ADR_BASE	0xff300000    /* dsu (ejtag) */

#define DDR_BASE_ADR_REG   MINITITAN_ADR_BASE    /* SDRAM base address reg */

/*******************************************************************
  BCM7315 Modules
 *******************************************************************/
#include "bcm7317_cpu.h"
#include "bcmupg.h"	/* definitions for UPG section of BCM711x */
#include "bcmqamds.h"
#include "bcmqamob.h"
#include "bcmqamus.h"
#include "bcmebi.h"
#include "bcmdma.h"
#include "bcmtimer.h"
#include "bcmintc.h"
#include "bcmpcmreg.h"		/* Pcm Registers */
#include "bcmvideoreg.h"	/* Video Registers */
#include "bcmbgereg.h"	/* Video Registers */


#ifdef _cplusplus
}
#endif

#endif
