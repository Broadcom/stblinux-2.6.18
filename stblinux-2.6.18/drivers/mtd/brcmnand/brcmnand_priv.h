/*
 * drivers/mtd/brcmnand/brcmnand_priv.h
 *
 *  Copyright (c) 2005-2009 Broadcom Corp.
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
 * Data structures for Broadcom NAND controller
 * 
 * when		who		what
 * 20060729	tht		Original coding
 */


#ifndef _BRCMNAND_PRIV_H_
#define _BRCMNAND_PRIV_H_

#include <linux/config.h>
#include <linux/vmalloc.h>

#include <asm/brcmstb/common/bcmtypes.h>
#include <linux/mtd/brcmnand.h> 
#include <asm/brcmstb/common/brcmstb.h>

#ifdef CONFIG_MTD_BRCMNAND_USE_ISR
#include <linux/irq.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>

//#include "edu.h"
#endif

#if defined( CONFIG_MTD_BRCMNAND_EDU )
#define BRCMNAND_malloc(size) kmalloc(size, GFP_DMA)
#define BRCMNAND_free(addr) kfree(addr)
#else
#define BRCMNAND_malloc(size) vmalloc(size)
#define BRCMNAND_free(addr) vfree(addr)
#endif



#ifdef CONFIG_MTD_BRCMNAND_USE_ISR
// Level 1 interrupt where EDU INTR2 is hanging off
#define BCM_LINUX_CPU_INTR1_IRQ		(1+BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HIF_CPU_INTR_SHIFT)

#define BCM_BASE_ADDRESS				0xb0000000

/* CP0 hazard avoidance. */
#define BARRIER __asm__ __volatile__(".set noreorder\n\t" \
				     "nop; nop; nop; nop; nop; nop;\n\t" \
				     ".set reorder\n\t")


typedef struct eduIsrData {
	spinlock_t lock; // For SMP and future double buffering on Read.
	int cmd;	// 1 == Read, 0 == Write

	uint32_t mask;	/* Clear status mask */
	uint32_t expect;	/* Status on success */
	uint32_t error;	/* Status on error */
	uint32_t intr;		/* Interrupt bits */
	uint32_t status; 	/* Status read during ISR.  There may be several interrupts before completion */
	int opComplete;	/* Completion criterium */

	/* For debugging only */
	uint32_t flashAddr;
	uint32_t dramAddr;
} eduIsrData_t;

extern eduIsrData_t gEduIsrData;

void ISR_init(void);

uint32_t ISR_wait_for_completion(void);
uint32_t ISR_cache_is_valid(uint32_t clearMask);

static inline uint32_t ISR_volatileRead(uint32_t addr)
{
        volatile uint32_t* pAddr;
        
        pAddr = (volatile uint32_t *)addr;
        
        return *(uint32_t *)pAddr;
}

static inline void ISR_volatileWrite(uint32_t addr, uint32_t data)
{
        volatile uint32_t* pAddr;

        pAddr = (volatile uint32_t *)addr;
        *pAddr = (volatile uint32_t)data;
}

static inline void ISR_enable_irq(void)
{
	uint32_t intrMask; 
	//unsigned long flags;

	//spin_lock_irqsave(&gEduIsrData.lock, flags);
	
	// Clear status bits
	ISR_volatileWrite(BCM_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_CLEAR, gEduIsrData.mask);

#if 0
	// Disable everything that may screw us up
	intrMask = EDU_volatileRead(EDU_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_MASK_STATUS);
	EDU_volatileWrite(EDU_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_MASK_SET, ~intrMask);
PRINTK("%s-1: intrMask=%08x\n", __FUNCTION__, intrMask);

	BARRIER;
#endif

	// Enable interrupt
	ISR_volatileWrite(BCM_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_MASK_CLEAR, gEduIsrData.intr);

#if 0	
intrMask = EDU_volatileRead(EDU_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_MASK_STATUS);
PRINTK("%s-2: intrMask=%08x\n", __FUNCTION__, intrMask);
#endif
	//spin_unlock_irqrestore(&gEduIsrData.lock, flags);
}

static inline void ISR_disable_irq(uint32_t mask)
{

	/* Disable L2 interrupts */
	ISR_volatileWrite(BCM_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_MASK_SET, mask);

	/* Clear L2 interrupts */
	//EDU_volatileWrite(EDU_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_CLEAR, mask);
}

#endif




/**
 * brcmnand_scan - [BrcmNAND Interface] Scan for the BrcmNAND device
 * @param mtd		MTD device structure
 * @param maxchips	Number of chips to scan for
 *
 * This fills out all the not initialized function pointers
 * with the defaults.
 * The flash ID is read and the mtd/chip structures are
 * filled with the appropriate values.
 *
 * THT: For now, maxchips should always be 1.
 */
extern int brcmnand_scan(struct mtd_info *mtd , int maxchips );

/**
 * brcmnand_release - [BrcmNAND Interface] Free resources held by the BrcmNAND device
 * @param mtd		MTD device structure
 */
extern void brcmnand_release(struct mtd_info *mtd);

/* BrcmNAND BBT interface */
extern int brcmnand_scan_bbt(struct mtd_info *mtd, struct nand_bbt_descr *bd);
extern int brcmnand_default_bbt(struct mtd_info *mtd);

extern int brcmnand_update_bbt (struct mtd_info *mtd, loff_t offs);

extern void* get_brcmnand_handle(void);

extern void print_oobbuf(const unsigned char* buf, int len);
extern void print_databuf(const unsigned char* buf, int len);

#if CONFIG_MTD_BRCMNAND_CORRECTABLE_ERR_HANDLING
extern int brcmnand_cet_update(struct mtd_info *mtd, loff_t from, int *status);
extern int brcmnand_cet_prepare_reboot(struct mtd_info *mtd);
extern int brcmnand_cet_erasecallback(struct mtd_info *mtd, u_int32_t addr);
extern int brcmnand_create_cet(struct mtd_info *mtd);
#endif

#endif
