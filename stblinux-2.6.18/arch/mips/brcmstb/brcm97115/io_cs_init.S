/*
 * arch/mips/brcmstb/brcm97110/io_cs_init.S
 *
 * Copyright (C) 2000-2005 Broadcom Corporation
 *
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

/*
#************************************************************************
#*     InitEbi: Initializes the EBI for proper chip-select operation    *
#*                                                                      *
#*      SYNTAX: void  InitEbi(void);                                    *
#*     RETURNS:                                                         *
#*                                                                      *
#*     Configuration                                                    *
#*       CS[0] - Boot Rom    @ 1fc0_0000  BASE_ADDR must already be set *
#*                                        because we have boot from rom *
#*       CS[1] - Flash       @ 0x1A000000                               *
#*       CS[2] - Flash       @ 0x1A800000                               *
#*       CS[3] - Security    @ 0x1B000000                               *
#*       CS[4] - Security    @ 0x1B000000                               *
#*       CS[5] - 7040        @ 0x1B100000                               *
#*       CS[6] - POD         @ 0x1B200000                               *
#*       CS[7] - 1394        @ 0x1B300000                               *
#*       CS[8] - 4413        @ 0x1B400000                               *
#*                                                                      *
#************************************************************************
*/
#include <asm/asm.h>
#include <asm/mipsregs.h>
#include <asm/regdef.h>
#include <asm/stackframe.h>

//#include "asm/brcmstb/brcm97115/bcm97115.h"
//#include "asm/brcmstb/brcm97115/bcm711x_common.h"

	.text
	.align	2


	.set    noreorder
  	.globl  InitEbi
  	.ent   InitEbi
	.set    mips2


InitEbi:

		li      t1,0xfffe7000  

		/* Chip Select # 0 */
		li      t2,0x1FC00000|0x08
		sw      t2,0x00(t1)       

		/* Chip Select # 1 */
		li      t2,0x1A000000|0x0b
		sw      t2,0x08(t1)       
		li      t2,0x07000000|0x00000010|0x00000001
		sw      t2,0x0c(t1)       

		/* Chip Select # 2 */
		li      t2,0x1B000000|0x09
		sw      t2,0x10(t1)       
		li      t2,0x07000000|0x00000010|0x00000001
		sw      t2,0x14(t1)        
#if 0
		/* Chip Select # 3 */
		li      t2,PHYS_SECURITY_BASE|EBI_SIZE_128K
		sw      t2,CS3BASE(t1)       
		li      t2,SEVENWT|EBI_WORD_WIDE|EBI_ENABLE
		sw      t2,CS3CNTL(t1)       

		/* Chip Select # 4 */
		li      t2,PHYS_SECURITY_BASE|EBI_SIZE_128K
		sw      t2,CS4BASE(t1)       
		li      t2,SEVENWT|EBI_WORD_WIDE|EBI_ENABLE
		sw      t2,CS4CNTL(t1)        

		/* Chip Select # 5 */
		li      t2,PHYS_BCM7040_BASE|EBI_SIZE_1M
		sw      t2,CS5BASE(t1)       
		li      t2,SEVENWT|EBI_WORD_WIDE|EBI_ENABLE
		sw      t2,CS5CNTL(t1)       

		/* Chip Select # 6 */
		li      t2,PHYS_POD_BASE|EBI_SIZE_1M
		sw      t2,CS6BASE(t1)       
		li      t2,SEVENWT|EBI_WORD_WIDE|EBI_ENABLE
		sw      t2,CS6CNTL(t1)       

		/* Chip Select # 7 */
		li      t2,PHYS_1394_BASE|EBI_SIZE_1M
		sw      t2,CS7BASE(t1)       
		li      t2,SEVENWT|EBI_WORD_WIDE|EBI_ENABLE
		sw      t2,CS7CNTL(t1)      		
#endif
		/* Chip Select # 8 */
		li      t2,0x1B400000|0x00
		sw      t2,0x40(t1)       
		li      t2,0x0c91 //EBI_WORD_WIDE|EBI_ENABLE|EBI_TS_TA_MODE|EBI_REV_END
		sw      t2,0x44(t1)       

        li      t2,0x00000400 
        sw      t2,0x100(t1)
        nop

	jr	ra
	nop

        .set    reorder

	.end   InitEbi
