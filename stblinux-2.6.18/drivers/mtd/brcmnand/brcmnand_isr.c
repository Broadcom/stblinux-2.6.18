/*
 * drivers/mtd/brcmnand/brcmnand_isr.c
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
 * Implement Interrupt Service Routine
 * 
 * when		who		what
 * 20090318	tht		Original coding
 */

//#define ISR_DEBUG_SMP
#undef ISR_DEBUG_SMP

#ifdef ISR_DEBUG_SMP
#include <asm/atomic.h>
#endif

#include "brcmnand_priv.h"
#include "edu.h"

#define PRINTK(...)
//#define PRINTK printk

#ifdef ISR_DEBUG_SMP
static atomic_t v = ATOMIC_INIT(1);
#define PRINTK1(...) if (!atomic_dec_and_test(&v)) printk("<")
#define PRINTK2(...) atomic_inc(&v)  //, printk(">"))
#define PRINTK5(...) if (!atomic_dec_and_test(&v))  printk("+");
#define PRINTK6(...) atomic_inc(&v)  // printk("-");
#define PRINTK3(...) if (!atomic_dec_and_test(&v)) printk("[");
#define PRINTK4(...) atomic_inc(&v) // printk("]");

#else
#define PRINTK1(...)
#define PRINTK2(...)
#define PRINTK3(...)
#define PRINTK4(...)
#define PRINTK5(...)
#define PRINTK6(...)
#endif
 

 // Wakes up the sleeping calling thread.
static DECLARE_WAIT_QUEUE_HEAD(gEduWaitQ);

eduIsrData_t gEduIsrData;

static irqreturn_t ISR_isr(int irq, void *devid, struct pt_regs *regs)
{
	uint32_t status, rd_data;
	uint32_t intrMask;  

	/*
	 * Not mine
	 */
	if (devid != (void*) &gEduIsrData) {
		return IRQ_NONE;
	}
	
	/*
	 * Remember the status, as there can be several L1 interrupts before completion.
	 * Grab the lock first, we don't want any race condition.
	 */
	down(&gEduIsrData.lock);
	intrMask = ISR_volatileRead(BCM_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_MASK_STATUS);
	rd_data = ISR_volatileRead(BCM_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_STATUS);
	
PRINTK1("==> %s: Awaken rd_data=%08x, intrMask=%08x, cmd=%d, flashAddr=%08x\n", __FUNCTION__, 
	rd_data, intrMask, gEduIsrData.cmd, gEduIsrData.flashAddr);

	gEduIsrData.status |= rd_data;
	status = gEduIsrData.status & gEduIsrData.mask;
	
	/*
	 * Evaluate exit/completion condition.  On an SMP system,
	 * it is too complicated to be evaluated atomically without a semaphore.
	 */
	switch (gEduIsrData.cmd) {
	case EDU_READ:
	case NAND_CTRL_READY:
		gEduIsrData.opComplete = ((gEduIsrData.expect == (gEduIsrData.status & gEduIsrData.expect)) || 
								(gEduIsrData.status & gEduIsrData.error));
		break;
		
	case EDU_WRITE:
		/* 
		 * We wait for both DONE|ERR +CTRL_READY
		 */
		gEduIsrData.opComplete = ((gEduIsrData.expect == (gEduIsrData.status & gEduIsrData.expect) ||
									(gEduIsrData.status & gEduIsrData.error))
								&&
								(gEduIsrData.status & HIF_INTR2_CTRL_READY));
		break;							
	}
	if (gEduIsrData.opComplete) {
		ISR_disable_irq(gEduIsrData.intr);
		//wake_up_interruptible(&gEduWaitQ);
		wake_up(&gEduWaitQ);
	}
	else {
		/* Ack only the ones that show */
		uint32_t ack = gEduIsrData.status & gEduIsrData.intr;
		
PRINTK("%s: opComp=0, intr=%08x, mask=%08x, expect=%08x, err=%08x, status=%08x, rd_data=%08x, intrMask=%08x, flashAddr=%08x, DRAM=%08x\n", __FUNCTION__, 
gEduIsrData.intr, gEduIsrData.mask, gEduIsrData.expect, gEduIsrData.error, gEduIsrData.status, rd_data, intrMask, gEduIsrData.flashAddr, gEduIsrData.dramAddr);

		// Just disable the ones that are triggered
		ISR_disable_irq(ack);
		gEduIsrData.intr &= ~ack;

		if (gEduIsrData.intr) {
			// Re-arm
			ISR_enable_irq();
		}
		else {
			printk(KERN_ERR "%s: Lost interrupt\n", __FUNCTION__);
			BUG();
		}
	}
	up(&gEduIsrData.lock);
PRINTK2("<== %s: \n", __FUNCTION__);
	return IRQ_HANDLED;
}

uint32_t ISR_wait_for_completion(void)
{
	//uint32_t rd_data;
//volatile unsigned int c = 0xfedeadad;
	int ret = -ERESTARTSYS;
	unsigned long to_jiffies = 3*HZ; /* 3 secs */
	//unsigned long cur_jiffies = jiffies;
	unsigned long expired = jiffies + to_jiffies;
	int cmd;
	int retries = 2;
	//unsigned long flags;
//volatile unsigned int counter = 0xAABBCCDD;
//static int erestartsys = 0;

	
	while (ret == -ERESTARTSYS ) {
//printk("%s: jiffies=%08lx, expired=%08lx\n", __FUNCTION__, jiffies, expired);
		if (((retries--) < 0) || time_after(jiffies, expired)) {
			ret = 0; // Timed out
			return ERESTARTSYS;
		}
		else  {
			// Recalculate TO, for retries
			to_jiffies = expired - jiffies;
			//ret = wait_event_interruptible_timeout(gEduWaitQ, gEduIsrData.opComplete, to_jiffies);
			ret = wait_event_timeout(gEduWaitQ, gEduIsrData.opComplete, to_jiffies);
		}

PRINTK3("==>%s\n", __FUNCTION__);
		down(&gEduIsrData.lock);

		cmd = gEduIsrData.cmd;
		gEduIsrData.cmd = -1;

		if (!gEduIsrData.opComplete && ret <= 0) {
			ISR_disable_irq(gEduIsrData.intr);

			if (ret == -ERESTARTSYS) {
				up(&gEduIsrData.lock);

//if (5 >= erestartsys++)
//printk("Pending signals: %08lx-%08lx-%08lx-%08lx\n", 
//current->pending.signal.sig[0], current->pending.signal.sig[1],current->pending.signal.sig[2], current->pending.signal.sig[3]);
				continue;
			}	
			else if (ret == 0) { 
				//gEduIsrData.opComplete = 1;
				PRINTK("%s: DMA timedout\n", __FUNCTION__);

				up(&gEduIsrData.lock);
//printk("<==%s, ret=0 TimeOut\n", __FUNCTION__);
PRINTK4("<==%s, ret=0 TimeOut\n", __FUNCTION__);

				return 0; // Timed Out
			}

			
			
			// DMA completes on Done or Error.
			//rd_data = ISR_volatileRead(BCM_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_STATUS);
		
PRINTK("%s: EDU completes but Status is %08x\n", __FUNCTION__, gEduIsrData.status);
			//rd_data = 0; // Treat as a timeout
		}

		up(&gEduIsrData.lock);
	}

	return gEduIsrData.status;
}


uint32_t ISR_cache_is_valid(uint32_t clearMask)
{
	uint32_t rd_data = ISR_volatileRead(BCM_BASE_ADDRESS+BCHP_HIF_INTR2_CPU_STATUS);
	//unsigned long flags;

	/*
	 * Already there, no need to wait
	 */
	if (rd_data & HIF_INTR2_CTRL_READY)
		return rd_data;

	// Clear existing interrupt
	ISR_volatileWrite(BCM_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_MASK_SET, clearMask);
	
	 do {
PRINTK5("==>%s\n", __FUNCTION__);
		down(&gEduIsrData.lock);
	 	gEduIsrData.flashAddr = 0;
	 	gEduIsrData.dramAddr = 0;
		
		/*
		 * Enable L2 Interrupt
		 */
		gEduIsrData.cmd = NAND_CTRL_READY;
		gEduIsrData.opComplete = 0;
		gEduIsrData.status = 0;
		
		gEduIsrData.mask = HIF_INTR2_CTRL_READY;
		gEduIsrData.expect = HIF_INTR2_CTRL_READY;
		gEduIsrData.error = 0;
		gEduIsrData.intr = HIF_INTR2_CTRL_READY;

		up(&gEduIsrData.lock);
PRINTK6("<==%s\n", __FUNCTION__);
		ISR_enable_irq();
	
		rd_data = ISR_wait_for_completion();
	} while (rd_data != 0 && !(rd_data & HIF_INTR2_CTRL_READY));
	return rd_data;

}

void ISR_init(void)
{
	int ret;
	uint32_t intrMask;

	init_MUTEX(&gEduIsrData.lock);
	
	// Mask all L2 interrupts
	intrMask = ISR_volatileRead(BCM_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_MASK_STATUS);
	ISR_volatileWrite(BCM_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_MASK_SET, ~intrMask);
	BARRIER;

	ret = request_irq(BCM_LINUX_CPU_INTR1_IRQ, ISR_isr, SA_SHIRQ, "brcmnand EDU", &gEduIsrData);
	if (ret) {
		printk(KERN_INFO "%s: request_irq(BCM_LINUX_CPU_INTR1_IRQ) failed ret=%d.  Someone not sharing?\n", 
			__FUNCTION__, ret);
	}
	
}


 
