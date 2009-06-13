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
#include <linux/list.h>

//#include "edu.h"
#endif

#define BRCMNAND_CORRECTABLE_ECC_ERROR		(1)
#define BRCMNAND_SUCCESS						(0)
#define BRCMNAND_UNCORRECTABLE_ECC_ERROR	(-1)
#define BRCMNAND_FLASH_STATUS_ERROR			(-2)
#define BRCMNAND_TIMED_OUT					(-3)

#ifdef CONFIG_MTD_BRCMNAND_EDU
#define BRCMEDU_CORRECTABLE_ECC_ERROR        	(4)
#define BRCMEDU_UNCORRECTABLE_ECC_ERROR      (-4)

#define  BRCMEDU_MEM_BUS_ERROR				(-5)


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

/* 
 * Right now we submit a full page Read for queueing, so with a 8KB page,
 * and an ECC step of 512B, the queue depth is 16. Add 2 for dummy elements
 * during EDU WAR
 */
#if CONFIG_MTD_BRCMNAND_VERSION <=  CONFIG_MTD_BRCMNAND_VERS_3_3
#define MAX_NAND_PAGE_SIZE	(4<<10)

#else
#define MAX_NAND_PAGE_SIZE	(8<<10)
#endif

/* Max queue size is (PageSize/512B_ECCSize)+2 spare for WAR */
#define MAX_JOB_QUEUE_SIZE	((MAX_NAND_PAGE_SIZE>>9))

typedef enum {
	ISR_OP_QUEUED = 0, 
	ISR_OP_SUBMITTED = 1, 
	ISR_OP_NEED_WAR = 2,
	ISR_OP_COMPLETED = 3, 
	ISR_OP_TIMEDOUT = 4
} isrOpStatus_t;

typedef struct eduIsrNode {
	struct list_head list;
	spinlock_t lock; // per Node update lock
	// int cmd;	// 1 == Read, 0 == Write

	// ISR stuffs
	uint32_t mask;	/* Clear status mask */
	uint32_t expect;	/* Status on success */
	uint32_t error;	/* Status on error */
	uint32_t intr;		/* Interrupt bits */
	uint32_t status; 	/* Status read during ISR.  There may be several interrupts before completion */
	isrOpStatus_t opComplete;	/* Completion status */

	/* Controller Level params (for queueing)  */
	struct mtd_info* mtd;
	void* 	buffer;
	u_char* 	oobarea;
	loff_t 	offset;
	int		ret;
	int		needBBT;

	/* EDU level params (for ISR) */
	uint32_t edu_ldw;
	uint32_t physAddr;
	uint32_t hif_intr2;
	uint32_t edu_status;

	int refCount;		/* Marked for re-use when refCount=0 */
	unsigned long expired; /* Time stamp for expiration, 3 secs from submission */
} eduIsrNode_t;

/*
 * Read/Write Job Q.
 * Process one page at a time, and queue 512B sector Read or Write EDU jobs.
 * ISR will wake up the process context thread iff
 * 1-EDU reports an error, in which case the process context thread need to be awaken
 *  		in order to do WAR
 * 2-Q is empty, in which case the page read/write op is complete.
 */
typedef struct jobQ_t {
	struct list_head 	jobQ;		/* Nodes queued for EDU jobs */
	struct list_head 	availList;	/* Free Nodes */
	spinlock_t		lock; 		/* Queues guarding spin lock */
	int 				needWakeUp;	/* Wake up Process context thread to do EDU WAR */
	int 				cmd; 		/* 1 == Read, 0 == Write */
} isrJobQ_t;

extern isrJobQ_t gJobQ; 

void ISR_init(void);

/*
 * Submit the first entry that is in queued state,
 * assuming queue lock has been held by caller.
 * 
 * @doubleBuffering indicates whether we need to submit just 1 job or until EDU is full (double buffering)
 * Return the number of job submitted for read.
 *
 * In current version (v3.3 controller), since EDU only have 1 register for EDU_ERR_STATUS,
 * we can't really do double-buffering without losing the returned status of the previous read-op.
 */
#undef EDU_DOUBLE_BUFFER_READ

int brcmnand_isr_submit_job(void);

eduIsrNode_t*  ISR_queue_read_request(struct mtd_info *mtd,
        void* buffer, u_char* oobarea, loff_t offset);
eduIsrNode_t* ISR_queue_write_request(struct mtd_info *mtd,
        const void* buffer, const u_char* oobarea, loff_t offset);
eduIsrNode_t*  ISR_push_request(struct mtd_info *mtd,
        void* buffer, u_char* oobarea, loff_t offset);


int brcmnand_edu_read_completion(struct mtd_info* mtd, 
        void* buffer, u_char* oobarea, loff_t offset, uint32_t intr_status);

int brcmnand_edu_read_comp_intr(struct mtd_info* mtd, 
        void* buffer, u_char* oobarea, loff_t offset, uint32_t intr_status);

#ifdef CONFIG_MTD_BRCMNAND_ISR_QUEUE
int brcmnand_edu_write_completion(struct mtd_info *mtd,
        const void* buffer, const u_char* oobarea, loff_t offset, uint32_t intr_status, 
        int needBBT);
#endif
eduIsrNode_t* ISR_find_request( isrOpStatus_t opStatus);

uint32_t ISR_wait_for_completion(void);

/*
 *  wait for completion with read/write Queue
 */
int ISR_wait_for_queue_completion(void);

int ISR_cache_is_valid(void);

static __inline__ uint32_t ISR_volatileRead(uint32_t addr)
{
        volatile uint32_t* pAddr;
        
        pAddr = (volatile uint32_t *)addr;
        
        return *(uint32_t *)pAddr;
}

static __inline__ void ISR_volatileWrite(uint32_t addr, uint32_t data)
{
        volatile uint32_t* pAddr;

        pAddr = (volatile uint32_t *)addr;
        *pAddr = (volatile uint32_t)data;
}

static __inline__ void ISR_enable_irq(eduIsrNode_t* req)
{
	uint32_t intrMask; 
	//unsigned long flags;

	//spin_lock_irqsave(&gEduIsrData.lock, flags);
	
	// Clear status bits
	ISR_volatileWrite(BCM_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_CLEAR, req->mask);

	// Enable interrupt
	ISR_volatileWrite(BCM_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_MASK_CLEAR, req->intr);

	//spin_unlock_irqrestore(&gEduIsrData.lock, flags);
}

static __inline__ void ISR_disable_irq(uint32_t mask)
{

	/* Disable L2 interrupts */
	ISR_volatileWrite(BCM_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_MASK_SET, mask);

}


/*
 * For debugging
 */

#ifdef DEBUG_ISR

static void __inline__
ISR_print_queue(void)
{
	eduIsrNode_t* req;
	//struct list_head* node;
	int i = 0;

	list_for_each_entry(req, &gJobQ.jobQ, list) {
		
		printk("i=%d, cmd=%d, offset=%08llx, flashAddr=%08x, opComp=%d, status=%08x\n",
			i, gJobQ.cmd, req->offset, req->edu_ldw,req->opComplete, req->status);
		i++;
	}	
}

static void __inline__
ISR_print_avail_list(void)
{
	eduIsrNode_t* req;
	//struct list_head* node;
	int i = 0;

	printk("AvailList=%p, next=%p\n", &gJobQ.availList, gJobQ.availList.next);
	list_for_each_entry(req, &gJobQ.availList, list) {
		printk("i=%d, req=%p, list=%p\n", i, req, &req->list);
		i++;
	}	
}
#else
#define IS_print_queue()
#define ISR_print_avail_list()
#endif // DEBUG_ISR


#endif // CONFIG_MTD_BRCMNAND_USE_ISR




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
