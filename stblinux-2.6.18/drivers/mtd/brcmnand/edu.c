/**
 * @par Copyright Information:
 *      Copyright (C) 2007, Broadcom Corporation.
 *      All Rights Reserved.
 *
 * @file edu.c
 * @author Jean Roberge
 *
 * @brief Prototypes for EDU Support Software
 */
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/jiffies.h>
#include <linux/delay.h>

#include <linux/byteorder/generic.h>

#include <asm/brcmstb/brcm97440b0/bchp_nand.h>
#include <asm/brcmstb/brcm97440b0/bchp_common.h>
#include <asm/brcmstb/brcm97440b0/bchp_hif_intr2.h>

#include <asm/io.h>
#include <asm/bug.h>

#include "edu.h"

#include <linux/mtd/brcmnand.h> 
#include "brcmnand_priv.h"

/*************************************** Internal Prototypes ******************************************/
uint32 EDU_poll(uint32_t, uint32_t, uint32_t);
void EDU_issue_command(uint32_t, uint32_t,uint8);
void get_edu_status(void);
uint32_t EDU_volatileRead(uint32_t);
void EDU_volatileWrite(uint32_t, uint32_t);
void EDU_init(void);
int EDU_write(uint32, uint32);
int EDU_read(uint32, uint32);

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
        uint32 rd_data=0, i=0; 
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
        uint32 rd_data=0, i=0; 
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

void EDU_checkRegistersValidity(uint32 external_physical_device_address)
{
       uint32 result;

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

uint16_t EDU_checkNandCacheAndBuffer(uint32 buffer, int length)
{
    uint32  k;
    uint16_t    error = 0; /* no error */    
    uint32  rd_addr, exp_data, act_data;

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

uint32 EDU_poll(uint32_t address, uint32_t expect, uint32_t mask)
{
        uint32 rd_data=0, i=0; 
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

        EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_CONFIG, EDU_CONFIG_VALUE);

        EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_LENGTH, EDU_LENGTH_VALUE);
 
        // Writing to PCI control register (Init PCI Window is now Here)

        EDU_volatileWrite(0xb0000128, 0x00000001);
        EDU_volatileWrite(0xb040600c, 0x00010000);

        // Clear the interrupt for next time
        EDU_volatileWrite(EDU_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_CLEAR, BCHP_HIF_INTR2_CPU_CLEAR_NAND_UNC_INTR_MASK); 
}

int EDU_write(uint32 virtual_addr_buffer, uint32 external_physical_device_address)
{
        uint32  phys_mem;
        // uint32  rd_data;

//printk("EDU_write: vBuff: %08x physDev: %08x\n", virtual_addr_buffer, external_physical_device_address);


        EDU_volatileWrite(EDU_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_CLEAR, HIF_INTR2_EDU_CLEAR);

        EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_DONE, 0x00000000); 
        EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_ERR_STATUS, 0x00000000); 

        phys_mem = virt_to_phys((void *)virtual_addr_buffer);
        dma_cache_wback(virtual_addr_buffer, 512);
        EDU_issue_command(phys_mem, external_physical_device_address, 0); /* 1: Is a Read, 0 Is a Write */

//      rd_data = EDU_poll(EDU_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_STATUS, HIF_INTR2_EDU_DONE, HIF_INTR2_EDU_DONE);
//      EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_DONE, 0x00000000);

        return 0;
}


int EDU_read(uint32 virtual_addr_buffer, uint32 external_physical_device_address)
{
        uint32  phys_mem;
        // uint32  rd_data;

        EDU_volatileWrite(EDU_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_CLEAR, HIF_INTR2_EDU_CLEAR);

        phys_mem = virt_to_phys((void *)virtual_addr_buffer);
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
        dma_cache_inv(virtual_addr_buffer, 512);

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
