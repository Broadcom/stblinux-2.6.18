/**
 * @par Copyright Information:
 *      Copyright (C) 2007, Broadcom Corporation.
 *      All Rights Reserved.
 *
 * @file edu.c
 * @author Jean Roberge
 *
 * @brief Prototypes for EDU Support Software
 *
 * LOG
 * 7/30/08 tht Add handling of VM allocated buffers.
 */
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/jiffies.h>
#include <linux/delay.h>

#include <linux/byteorder/generic.h>

/* THT: Needed for VM addresses */


#include <linux/mm.h>
#include <asm/page.h>



#include <asm/brcmstb/brcm97440b0/bchp_nand.h>
#include <asm/brcmstb/brcm97440b0/bchp_common.h>
#include <asm/brcmstb/brcm97440b0/bchp_hif_intr2.h>

#include <asm/io.h>
#include <asm/bug.h>

#include "edu.h"
#include "eduproto.h"

#include <linux/mtd/brcmnand.h> 
#include "brcmnand_priv.h"



/*
 * THT: Just calling virt_to_phys() will not work on VM allocated addresses (0xC000_0000-0xCFFF_FFFF)
 * On the other hand, we cannot call kmalloc to allocate the BBT, because of its size.
 */

static unsigned long EDU_virt_to_phys(volatile void* vaddr)
{
	unsigned long addr = (unsigned long) vaddr;
	
	if (!(addr & KSEG0)) { 
		printk(KERN_ERR "brcmnand EDU: User Space buffers %08lx are not currently supported\n", addr);
		/* THT: Note to self: http://lwn.net/Articles/28548/ */
		BUG();
		goto error_out;
	}
	
	/* If not VM addresses, use the regular function */
	else if (addr < VMALLOC_START || addr > VMALLOC_END) {
		return virt_to_phys(vaddr);
	}
	
	// Buffers allocated by vmalloc(): We have to find the physical page */
	else {
		unsigned long start; // Page start address
		unsigned long pa = 0UL;
		unsigned long pageOffset; // Offset from page start address
		struct vm_area_struct* vma;
		pgd_t *pgd;
		pud_t *pud;
		pmd_t *pmd;
		pte_t *pte;
		struct page *page;
		unsigned long pfn;
		
		//unsigned long flags;

		start = (addr & PAGE_MASK);
		pageOffset = addr - start;
//printk("Calling find_extend_vma, start=%08lx\n", start);
//		vma = find_vma(current->mm, start);
		//local_irq_save(flags);
//printk("find_vma returns %p\n", vma);

//printk("Calling pgd_offset, current-=%p\n", current);
//printk("Calling pgd_offset, current->mm=%p\n", current->mm);
		if (current->mm) {
			pgd = pgd_offset(current->mm, start);
		}
		else {
			// If not find it in the kernel page table
			pgd = pgd_offset_k(start);
		}
//printk("pgd=%08x\n", pgd->pgd);
		pud = pud_offset(pgd, start);
//printk("pud=%08x\n", pud->pgd);
		pmd = pmd_offset(pud, start);
//printk("pnd=%08x\n", pmd->pud.pgd);
		pte = pte_offset(pmd, start);

#if 0
printk("Calling vm_normal_page, pte=%08lx\n", pte->pte);
		page = vm_normal_page(vma, start, *pte);

printk("page=%p\n", page);
		if (page) {
			// get_page(page);
			pa = page_to_phys(page);
printk("PA=%08lx\n", pa);
		}
		else {
			printk(KERN_ERR "brcmnand EDU: Unable to find page mapped to %08lx\n", addr);
			goto error_out;
		}
#else
//printk("Calling pte_pfn(pte=%08lx)\n", pte->pte);
		pfn = pte_pfn(*pte);
//printk("pfn=%08lx\n", pfn);
		pa = pfn << PAGE_SHIFT;
#endif

		//local_irq_restore(flags);

		return (pa + pageOffset);
	}

error_out:
	return 0UL;
}



/*************************************** Internals *******************************************/

void EDU_get_status(void)
{
    uint32_t rd_data;
    rd_data = 0;

    rd_data = EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_DONE);
    printk("INFO: Done count = 0x%08x\n",rd_data);

    rd_data = EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_STATUS);
    printk("INFO: EDU status = 0x%08x\n",rd_data);
    printk("INFO: \t bit1 = pending\n");
    printk("INFO: \t bit0 = active\n");

    rd_data = EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_STOP);
    printk("INFO: Stop reg = 0x%08x\n",rd_data);

    rd_data = EDU_volatileRead(EDU_BASE_ADDRESS + EDU_ERR_STATUS);
    printk("INFO: EDU ERR status = 0x%08x\n",rd_data);
    printk("INFO: \t bit3 = NandWrErr\n");
    printk("INFO: \t bit2 = NandEccUncor\n");
    printk("INFO: \t bit1 = NandEccCor\n");
    printk("INFO: \t bit0 = ErrAck\n");

    rd_data = EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_ECC_CORR_ADDR);
    printk("INFO: NAND CTRL BCHP_NAND_ECC_CORR_ADDR = 0x%08x\n",rd_data);

    rd_data = EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_ECC_UNC_ADDR);
    printk("INFO: NAND CTRL BCHP_NAND_ECC_UNC_ADDR = 0x%08x\n",rd_data);

    rd_data = EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_INTFC_STATUS) & 0xf00000ff;
    printk("INFO: NAND CTRL BCHP_NAND_INTFC_STATUS = 0x%08x\n",rd_data);
}

#if 0  // DO NOT DELETE, MAY BE USEFUL!!!

void EDU_poll_for_done()
{
        uint32_t rd_data=0, i=0; 
        unsigned long timeout;
        
        __sync();
        rd_data = EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_DONE);

        timeout = jiffies + msecs_to_jiffies(3000); // 3 sec timeout for now (testing)
        while ((rd_data & 0x00000003) == 0x00000000) 
        {
         
                __sync(); //PLATFORM_IOFLUSH_WAR();
                rd_data = EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_DONE);
                i++;
                if(!time_before(jiffies, timeout))
                {
                   printk("EDU_poll_for_done timeout at 3 SECONDS with i= 0x%.08x!\n", (int)i);
                   return;
                }
        }
        return;
}


void EDU_waitForNoPendingAndActiveBit()
{
        uint32_t rd_data=0, i=0; 
        unsigned long timeout;
        
        //printk("Start Polling!\n");
        __sync();
        rd_data = EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_STATUS);

        timeout = jiffies + msecs_to_jiffies(3000); // 3 sec timeout for now (testing)
        while ((rd_data & 0x00000003) != 0x00000000) /* && (i<cnt) */ 
        {
         
                __sync(); //PLATFORM_IOFLUSH_WAR();
                rd_data = EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_STATUS);
                i++;
                if(!time_before(jiffies, timeout))
                {
                   printk("EDU_waitForNoPendingAndActiveBit timeout at 3 SECONDS with i= 0x%.08x!\n", (int)i);
                   return;
                }
        }
        return;
}

void EDU_checkRegistersValidity(uint32_t external_physical_device_address)
{
       uint32_t result;

        result = EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_INTFC_STATUS);
        
        result = result & 0xf0000000;        

        if( result != 0xf0000000)
        {
                printk("NAND Status bits are NOT VALID!!\n");
        }

        else if( EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_CMD_ADDRESS) != external_physical_device_address)
        {
                printk("BCHP_NAND_CMD_ADDRESS not good!\n");
        }

        else if( EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_CMD_START) != 0x01000000)
        {
                printk("BCHP_NAND_CMD_START not good!\n");
        }

        else if( EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_SEMAPHORE) != 0x00000000)
        {
                printk("BCHP_NAND_SEMAPHORE not good!\n");
        }

        result = EDU_volatileRead(EDU_BASE_ADDRESS + EDU_ERR_STATUS) & 0x0000000f;
       
        if((result != 0x00000004) && (result != 0x00000000)) 
        {
                printk("EDU_ERR_STATUS = 0x%.08x NOT GOOD\n", result);
        }
}

uint16_t EDU_checkNandCacheAndBuffer(uint32_t buffer, int length)
{
    uint32_t  k;
    uint16_t    error = 0; /* no error */    
    uint32_t  rd_addr, exp_data, act_data;

       for (k=0; k < length; k=k+4) {
                rd_addr = buffer + k;
                if(EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_FLASH_CACHEi_ARRAY_BASE + k) != EDU_volatileRead(rd_addr))
                {
                        error = 1;
                }
        }
        
     if(error == 1)
     {
         printk("ERROR: BAD DRAM EDU_checkNandCacheAndBuffer at address 0x%8x!!\n", (unsigned int)buffer);
         EDU_get_status();

     }
     else
     {
         //printk("TEST PASSED byteSmoosh at address 0x%8x!!\n", buffer);
     }   

     return error;
}

#endif

// 32-bit register polling
// Poll a register until the reg has the expected value.
// a timeout read count. The value reflects how many reads
// the routine check the register before is gives up.

uint32_t EDU_poll(uint32_t address, uint32_t expect, uint32_t mask)
{
        uint32_t rd_data=0, i=0; 
        unsigned long timeout;
        
        //printk("Start Polling!\n");
        __sync();
        rd_data = EDU_volatileRead(address);

        timeout = jiffies + msecs_to_jiffies(3000); // 3 sec timeout for now (testing)
        while ((rd_data & mask) != (expect & mask)) /* && (i<cnt) */ 
        {
         
                __sync(); //PLATFORM_IOFLUSH_WAR();
                rd_data = EDU_volatileRead(address);
                
                // JR+ 2008-02-01 Allow other tasks to run while waiting
                cond_resched();
                // JR- 2008-02-01 Allow other tasks to run while waiting
                
                i++;
                if(!time_before(jiffies, timeout))
                {
                   printk("EDU_poll timeout at 3 SECONDS (just for testing) with i= 0x%.08x!\n", (int)i);
                   printk("DBG> EDU_poll (0x%.08x, 0x%.08x, 0x%.08x);\n",address, expect, mask);
                   return(rd_data);
                }
        }
  
        //printk("DBG> EDU_poll (0x%.08x, 0x%.08x, 0x%.08x);\n",address, expect, mask);
        //printk("DBG> EDU_poll i= 0x%.08x!\n", i);
        //printk("\n");
        //printk("End Polling! Number of passes: %d\n", i);
   
        return(rd_data);
}


void EDU_issue_command(uint32_t dram_addr, uint32_t ext_addr,uint8 cmd)
{

    EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_DRAM_ADDR, dram_addr);
    //EDU_volatileWrite(EDU_PATCH_GLOBAL_REG_RBUS_START + CPU_REGISTER_ADDRESS + EDU_DRAM_ADDR, dram_addr);        
    //printk("\tINFO: EDU_DRAM_ADDR = 0x%08x\n",dram_addr);

    EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_EXT_ADDR, ext_addr);
    //EDU_volatileWrite(EDU_PATCH_GLOBAL_REG_RBUS_START + CPU_REGISTER_ADDRESS + EDU_EXT_ADDR, ext_addr);
    //printk("\tINFO: EDU_EXT_ADDR = 0x%08x\n",ext_addr);

    EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_CMD, cmd);
    //EDU_volatileWrite(EDU_PATCH_GLOBAL_REG_RBUS_START + CPU_REGISTER_ADDRESS + EDU_CMD, cmd);
    //if (cmd == 1)
        //printk("\tINFO: EDU_CMD = READ operation\n");
    //if (cmd == 0)
       //printk("\tINFO: EDU_CMD = WRITE operation\n");
}


uint32_t EDU_volatileRead(uint32_t addr)
{
        volatile uint32_t* pAddr;
        
        pAddr = (volatile uint32_t *)addr;
        
        return *(uint32_t *)pAddr;
}

void EDU_volatileWrite(uint32_t addr, uint32_t data)
{
        volatile uint32_t* pAddr;

        pAddr = (volatile uint32_t *)addr;
        *pAddr = (volatile uint32_t)data;
       
}

uint32_t EDU_get_error_status_register(void)
{
        uint32_t valueOfReg = EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_ERR_STATUS);  

        // Clear the error
        EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_ERR_STATUS, 0x00000000);   

        return(valueOfReg);
}

void EDU_init(void)
{
	
printk("-->%s:\n", __FUNCTION__);

        EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_CONFIG, EDU_CONFIG_VALUE);

        EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_LENGTH, EDU_LENGTH_VALUE);
 
        // Writing to PCI control register (Init PCI Window is now Here)

        EDU_volatileWrite(0xb0000128, 0x00000001);
        EDU_volatileWrite(0xb040600c, 0x00010000);

        // Clear the interrupt for next time
        EDU_volatileWrite(EDU_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_CLEAR, BCHP_HIF_INTR2_CPU_CLEAR_NAND_UNC_INTR_MASK); 

	//external_physical_device_address = EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_CMD_ADDRESS);
	//return external_physical_device_address;
}

/*
 * THT: 07/31/08: This does not work.  One has to write the 512B Array from the NAND controller into 
 * the EXT registers for it to work.  Will fix it when I come back.
 */
int EDU_write(volatile const void* virtual_addr_buffer, uint32_t external_physical_device_address)
{
	uint32_t  phys_mem;
	// uint32_t  rd_data;

	phys_mem = EDU_virt_to_phys((void *)virtual_addr_buffer);
	if (!phys_mem) {
		return (-1);
	}
	
//printk("EDU_write: vBuff: %p physDev: %08x, PA=%08x\n", 
//virtual_addr_buffer, external_physical_device_address, phys_mem);


	EDU_volatileWrite(EDU_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_CLEAR, HIF_INTR2_EDU_CLEAR);

	EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_DONE, 0x00000000); 
	EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_ERR_STATUS, 0x00000000); 
	
	dma_cache_wback_inv((unsigned long) virtual_addr_buffer, 512);
	EDU_issue_command(phys_mem, external_physical_device_address, 0); /* 1: Is a Read, 0 Is a Write */

//      rd_data = EDU_poll(EDU_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_STATUS, HIF_INTR2_EDU_DONE, HIF_INTR2_EDU_DONE);
//      EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_DONE, 0x00000000);

	return 0;
}


/*
 * THT: 07/31/08: This does not work.  One has to write the 512B Array from the NAND controller into 
 * the EXT registers for it to work.  Will fix it when I come back.
 */
int EDU_read(volatile void* virtual_addr_buffer, uint32_t external_physical_device_address)
{
        uint32_t  phys_mem;
        // uint32_t  rd_data;

	phys_mem = EDU_virt_to_phys((void *)virtual_addr_buffer);
	if (!phys_mem) {
		return (-1);
	}

//printk("EDU_read: vBuff: %p physDev: %08x, PA=%08x\n", 
//virtual_addr_buffer, external_physical_device_address, phys_mem);

        EDU_volatileWrite(EDU_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_CLEAR, HIF_INTR2_EDU_CLEAR);

#if 0

        if( (EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_DONE) && 0x00000003) > 1)
        {
                printk("EDU_DONE > 1!!!\n");
        }
#endif
        EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_DONE, 0x00000000);

#if 0
        if( (EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_DONE) && 0x00000003) != 0)
        {
                printk("EDU_DONE != 0!!!\n");
        }
#endif
        EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_ERR_STATUS, 0x00000000);
#if 0
        if( EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_ERR_STATUS) != 0)
        {
                printk("EDU_ERR_STATUS != 0!!!\n");
        }

#endif
        dma_cache_inv((unsigned long) virtual_addr_buffer, 512);

        EDU_issue_command(phys_mem, external_physical_device_address, 1); /* 1: Is a Read, 0 Is a Write */

        EDU_poll(EDU_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_STATUS, HIF_INTR2_EDU_DONE, HIF_INTR2_EDU_DONE);

#if 0
// TEST TEST TEST TEST
        EDU_checkRegistersValidity(external_physical_device_address);
        EDU_checkNandCacheAndBuffer(virtual_addr_buffer, 512);
// TEST TEST TEST TEST
#endif
        return 0;
} 
