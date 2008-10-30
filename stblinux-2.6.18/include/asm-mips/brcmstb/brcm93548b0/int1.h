#ifndef _INT1_H
#define _INT1_H

#include "bcmmips.h"
#include "bchp_common.h"
#include "bchp_hif_cpu_intr1.h"
#include "bchp_hif_cpu_tp1_intr1.h"
#include "int1_api.h"


#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
 *  BCM Interrupt Level 1 Base Address
 **************************************************************************/
#define CPUINT1C_ADR_BASE  \
	BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_HIF_CPU_INTR1_INTR_W0_STATUS)

#define CPUINT1C ((volatile Int1Control * const)(CPUINT1C_ADR_BASE))

#define CPUINT1C_TP1_ADR_BASE  \
	BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_HIF_CPU_TP1_INTR1_INTR_W0_STATUS)

#define CPUINT1C_TP1 ((volatile Int1Control * const)(CPUINT1C_TP1_ADR_BASE))

#ifdef __cplusplus
}
#endif

#endif /* _INT1_H */

