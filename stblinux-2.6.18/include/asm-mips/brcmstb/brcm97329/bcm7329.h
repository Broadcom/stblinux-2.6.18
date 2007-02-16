/*                                                                     
 * Copyright (C) 2004 Broadcom Corporation
 *                    Ton Truong <ttruong@broadcom.com>
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

/***********************************************************************/
/*                                                                     */
/*   MODULE:  bcm7329.h                                                */
/*   PURPOSE: Define addresses of major hardware components of         */
/*            BCM7329                                                  */
/*                                                                     */
/***********************************************************************/
#ifndef BCM7329_H
#define BCM7329_H

#ifndef LANGUAGE_ASSEMBLY
#ifdef _cplusplus
extern "C" {
#endif
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


#include "bcm7329_cpu.h"

#define PHYS_MINITITAN_ADR_BASE 0x1ae00000
#define PHYS_XPT_ADR_BASE       0x1ae00000    /* Transport 32KB */
#define PHYS_VIDEO_ADR_BASE     0x1ae00000    /* Video display */
#define PHYS_BGE_ADR_BASE       0x1ae00000    /* BGE 32KB */
#define PHYS_PCM_AUDIO_ADR_BASE 0x1ae00000    /* PCM audio */
#define PHYS_MEM_DMA_ADR_BASE   0x1ae00000    /* Memory DMA */
#define PHYS_ADP_ADR_BASE       0x1afd0000    /* Audio 16KB */

#define PHYS_SDS_ADR_BASE       0x1afd4000    /* satellite downstream 16KB */
#define PHYS_UPG_ADR_BASE       0x1afe0000    /* UPG, 512B */
#define PHYS_USB_HOST2_ADR_BASE	0x1afe0200    /* USB Host #2, 256B */
#define PHYS_UHFR_ADR_BASE      0x1ae00000    /* UHF Receiver, 256B */
#define PHYS_INTC_ADR_BASE      0x1afe0600    /* interrupt controller 256B */
#define PHYS_TIMR_ADR_BASE      0x1afe0700    /* timers 256B */
#define PHYS_BRIDGE_ADR_BASE    0x1afe0900    /* Bridge ACC, 256B */
#define PHYS_DMA_ADR_BASE       0x1afe6000    /* DMA controller 4KB */
#define PHYS_EBI_ADR_BASE       0x1afe7000    /* EBI 4K */
#define PHYS_TM_TOP_ADR_BASE    0x1ae00000    /* TM_TOP, 256B */
#define PHYS_USB_HOST_ADR_BASE 0x1afe8100    /* USB Host, 256B */
#define PHYS_MODEM_ADR_BASE     0x1afe8500    /* soft modem 256B */
#define PHYS_RF_MOD_ADR_BASE    0x1afe8600    /* RF modulator 256B */
#define PHYS_PCI_CFG_ADR_BASE   0x1afe9000    /* PCI Config/Ctrl 512B */

#define USB_HOST_ADR_BASE PHYS_USB_HOST_ADR_BASE
#define USB_HOST2_ADR_BASE PHYS_USB_HOST2_ADR_BASE

#define MINITITAN_ADR_BASE      BCM_PHYS_TO_K1(PHYS_MINITITAN_ADR_BASE)
#define IDE_ADR_BASE            BCM_PHYS_TO_K1(PHYS_IDE_ADR_BASE)
#define XPT_ADR_BASE            BCM_PHYS_TO_K1(PHYS_XPT_ADR_BASE)
#define VIDEO_ADR_BASE          BCM_PHYS_TO_K1(PHYS_VIDEO_ADR_BASE)
#define BGE_ADR_BASE            BCM_PHYS_TO_K1(PHYS_BGE_ADR_BASE)
#define PCM_AUDIO_ADR_BASE      BCM_PHYS_TO_K1(PHYS_PCM_AUDIO_ADR_BASE)
#define MEM_DMA_ADR_BASE        BCM_PHYS_TO_K1(PHYS_MEM_DMA_ADR_BASE)
#define ADP_ADR_BASE            BCM_PHYS_TO_K1(PHYS_ADP_ADR_BASE)
#define SDS_ADR_BASE            BCM_PHYS_TO_K1(PHYS_SDS_ADR_BASE)
#define UPG_ADR_BASE            BCM_PHYS_TO_K1(PHYS_UPG_ADR_BASE)
#define UHFR_ADR_BASE           BCM_PHYS_TO_K1(PHYS_UHFR_ADR_BASE)
#define INTC_ADR_BASE           BCM_PHYS_TO_K1(PHYS_INTC_ADR_BASE)
#define TIMR_ADR_BASE           BCM_PHYS_TO_K1(PHYS_TIMR_ADR_BASE)
#define BRIDGE_ADR_BASE         BCM_PHYS_TO_K1(PHYS_BRIDGE_ADR_BASE)
#define DMA_ADR_BASE            BCM_PHYS_TO_K1(PHYS_DMA_ADR_BASE)
#define EBI_ADR_BASE            BCM_PHYS_TO_K1(PHYS_EBI_ADR_BASE)
#define TM_TOP_ADR_BASE         BCM_PHYS_TO_K1(PHYS_TM_TOP_ADR_BASE)
#define MODEM_ADR_BASE          BCM_PHYS_TO_K1(PHYS_MODEM_ADR_BASE)
#define RF_MOD_ADR_BASE         BCM_PHYS_TO_K1(PHYS_RF_MOD_ADR_BASE)
#define PROBE_ADR_BASE          BCM_PHYS_TO_K1(PHYS_PROBE_ADR_BASE)
#define DSU_ADR_BASE            BCM_PHYS_TO_K1(PHYS_DSU_ADR_BASE)
#define PCI_CFG_ADR_BASE        BCM_PHYS_TO_K1(PHYS_PCI_CFG_ADR_BASE)


#define DDR_BASE_ADR_REG        MINITITAN_ADR_BASE 

#include "bcmebi.h"
#include "bcmupg.h"
#include "bcmtimer.h"
#ifndef LANGUAGE_ASSEMBLY
#include "bcmintc.h"
#include "bcmdma.h"
#include "bcmsds.h"
#include "bcmbridge.h"
//#include "bme.h"
#endif

#ifndef LANGUAGE_ASSEMBLY
#ifdef _cplusplus
}
#endif
#endif

#endif
