/**
 * @par Copyright Information:
 *      Copyright (C) 2007, Broadcom Corporation.
 *      All Rights Reserved.
 *
 * @file eduproto.h
 * @author Jean Roberge
 *
 * @brief Prototypes for EDU Support Software
 */

#ifndef _EDUPROTO_H_
#define _EDUPROTO_H_

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/jiffies.h>

#include <linux/byteorder/generic.h>
#include <linux/timer.h>

#include <asm/brcmstb/brcm97440b0/bchp_nand.h>
#include <asm/brcmstb/brcm97440b0/bchp_common.h>
#include <asm/brcmstb/brcm97440b0/bchp_hif_intr2.h>

#include <asm/io.h>
#include <asm/bug.h>

#include <linux/mtd/brcmnand.h> 

/******************************************************************************
* Prototyping
*******************************************************************************/
extern uint32_t tb_r(uint32_t);
extern void tb_w(uint32_t, uint32_t);
extern uint32_t tb_poll(uint32_t, uint32_t, uint32_t);
extern int byteSmoosh(int);

extern void issue_command(uint32_t, uint32_t, uint8_t);
extern void get_edu_status(void);
extern void edu_check_data(uint32_t, uint32_t, uint32_t);


extern void EDU_initialize_timer(void);
extern void EDU_start_timer(void);
extern void EDU_stop_timer(void);
extern void EDU_print_results(unsigned long data_size);
extern void EDU_get_status(void);

extern uint32_t EDU_volatileRead(uint32_t);
extern void EDU_volatileWrite(uint32_t, uint32_t);

extern uint32_t EDU_poll(uint32_t, uint32_t, uint32_t);

extern void EDU_init(void);
extern int EDU_write(volatile const void*, uint32_t);
extern int EDU_read(volatile void*, uint32_t);

extern uint32_t EDU_get_error_status_register(void);

#endif // _EDUPROTO_H_
