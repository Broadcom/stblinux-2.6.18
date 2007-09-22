/*
 * arch/mips/brcmstb/brcm97405a0/racinit.S
 *
 * Copyright (C) 2004-2005 Broadcom Corporation
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
 *
 *
 * Board Specific routines for Broadcom eval boards
 *
 * when       who    what
 * ----       ---    ----
 * 10-21-2005   THT  Created
 */
 
#include <linux/config.h> // For CONFIG_SMP
#include <asm/asm.h>
#include <asm/mipsregs.h>
#include <asm/regdef.h>
#include <asm/stackframe.h>
#include <asm/cacheops.h>

LEAF(_RAC_init)
    .set    noreorder
    .set push
    .set mips32
   

	# Save ra.  LOADREL can stomp on it.
	move t3, ra

    /*Init code for bf1 space without TLB*/
    li $2,0x11f0000C        //Set up CBR to 0x11fx_xxxx: mask bits: [5:2] must be 0011.
    mtc0 $2, $22,6
    nop

    /*Enable SYNC MODE*/
    mfc0 $3, $22, 5
    nop
    li   $2, 0xefffffff
    and  $3, $2, $3
    mtc0 $3, $22, 5
    nop

    li $2, 0xb1f0001c       /*enable LLMB*/
    lw $3, 0x0($2)
    li $4, 0x1
    or $3, $3, $4
    sw $3, 0x0($2)

#if 0 // THT: Moved to RAC_enable
    li $2, 0xb1f00004
    li $3, 0x2fff0000
    sw $3, 0x0($2)


    li $2, 0xb1f00000       /*enable D and I RAC for Thread 0*/
    lw $4, 0x0($2)
    li $3, 0xf
    or $4, $4, $3
    li $3, 0xffffffdf       /*Write 0 to bit 5*/
    and $4, $4, $3
    sw $4, 0x0($2)

    li $2, 0xb1f00008       /*enable D and I RAC for Thread  1*/
    lw $4, 0x0($2)
    li $3, 0xf
    or $4, $4, $3
    sw $4, 0x0($2)
#endif


# Restore ra
	move ra, t3

	j ra
	nop

    .set mips0
	.set 	pop
    .set    reorder
	.end    _RAC_init


LEAF(_RAC_enable)
    .set    noreorder
    .set push
    .set mips32
   

	# Save ra.  LOADREL can stomp on it.
	move t3, ra

	li $2, 0xb1f00004		/*To set the address range of the RAC*/
    li $3, 0x2fff0000
    sw $3, 0x0($2)


    li $2, 0xb1f00000       /*enable D and I RAC for Thread 0*/
    lw $4, 0x0($2)
    li $3, 0xf
    or $4, $4, $3
    li $3, 0xffffffdf       /*Write 0 to bit 5*/
    and $4, $4, $3
    sw $4, 0x0($2)

#ifdef CONFIG_SMP
    li $2, 0xb1f00008       /*enable D and I RAC for Thread  1*/
    lw $4, 0x0($2)
    li $3, 0xf
    or $4, $4, $3
    sw $4, 0x0($2)
#endif

# Restore ra
	move ra, t3

	j ra
	nop

    .set mips0
	.set 	pop
    .set    reorder
	.end    _RAC_enable

