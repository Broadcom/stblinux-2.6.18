/***************************************************************************
 *     Copyright (c) 1999-2007, Broadcom Corporation
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


#ifndef _INT1_H
#define _INT1_H

#include "bcmmips.h"
#include "bchp_common.h"
#include "bchp_hif_cpu_intr1.h"
#include "int1_api.h"


#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
 *  BCM7038 Interrupt Level 1 Base Address
 **************************************************************************/
#define CPUINT1C_ADR_BASE  \
            BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_HIF_CPU_INTR1_INTR_W0_STATUS)

#define CPUINT1C ((Int1Control * const)(CPUINT1C_ADR_BASE))



#ifdef __cplusplus
}
#endif

#endif /* _INT1_H */

