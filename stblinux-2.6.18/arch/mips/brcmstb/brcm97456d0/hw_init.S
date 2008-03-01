/**********************************************************************
*
*
*	File:         hw_init.S
* Copyright (C) 1998-2005 Broadcom Corporation
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
**	Description: HW init.
**	Created:      QY
**
**
**
**	REVISION:
**
**		
**
**
**********************************************************************/
#include <asm/asm.h>
#include <asm/mipsregs.h>
#include <asm/regdef.h>
#include <asm/stackframe.h>
#include <asm/cacheops.h>

	.text
	.align	2

//#define C0_BRCM			    $22
#define C0_CONFIG		    $16

/* 3350/3352/1100 */
/*#define ICACHE_SIZE         0x1000      # 4K*/
/* 3345 */
/*#define ICACHE_SIZE         0x2000      # 8K*/
/* 7100 */
#define ICACHE_SIZE         0x8000      # 32K
#define ICACHE_LINESIZE     0x40

#define DCACHE_SIZE         0x10000      # 64K
#define DCACHE_LINESIZE     0x40

/*---------------------------------------------------------------------------
    Name: _cacheInit()
 Purpose: Initialize the I & D cache
    Uses: a0, a1
---------------------------------------------------------------------------*/

    .set    noreorder
  	.globl  cacheALibInit
  	.ent    cacheALibInit
//    .set    mips3

cacheALibInit:
        /* force kseg1 */
        la      t0, 1f
        or      t0, 0xa0000000
        j       t0
        nop
1:

    move    a1, ra              # Save return address
	mfc0    t0, C0_CONFIG
    li      t1, ~0x00000007
    and     t0, t1              # Clear existing cache enable state

#    or      t0, 0x01            # Enable WT dcache
#    or      t0, 0x02            # Disable dcache
    or      t0, 0x03            # Enable WB dcache
	mtc0    t0, C0_CONFIG

//	li      t0, 0xC2000000		# enabled caches
//	li      t0, 0xC0000000		# enabled caches
//	mtc0    t0, C0_BRCM 

	mfc0	t1, $22		// C0_BRCM			
	nop
	li      t0, 0xC0000000		# enabled caches
	or		t1,t0,t1
	mtc0	t1, $22		// C0_BRCM

    jal     _cacheIflush
    nop

    jal     _cacheDflush
    nop

    move    ra, a1
  	j	    ra
  	nop

    .set    reorder
	.end    cacheALibInit


/*---------------------------------------------------------------------------
    Name: _cacheIflush()
 Purpose: Flush the Instruction Cache
    Uses: gp, t0
---------------------------------------------------------------------------*/

    .set    noreorder
  	.globl  _cacheIflush
  	.ent    _cacheIflush

_cacheIflush:
        /* force kseg1 */
        la      t0, 1f
        or      t0, 0xa0000000
        j       t0
        nop
1:

        mtc0    zero, CP0_TAGLO
        nop
        nop

        .word 0x4080e001    # mtc0 zero,C0_TAGLO,1 - unsupported MIPS32 Instr
        nop
        nop

	li      t0, ICACHE_SIZE 
  	sub     t0, ICACHE_LINESIZE

iFlushLoop:
	nop
	nop
	nop
	nop
  	.word 0xbd080000    //cache   0x8, 0x0(t0)    # invalidate set
  	nop
  	nop
  	nop
  	nop
    bne     t0, zero, iFlushLoop
  	sub     t0, ICACHE_LINESIZE

  	j	    ra
  	nop

    .set    reorder
	.end    _cacheIflush

/*---------------------------------------------------------------------------
    Name: _cacheDflush()
 Purpose: Flush the Data Cache
    Uses: gp, t0, t1
---------------------------------------------------------------------------*/

  	.globl  _cacheDflush
  	.ent    _cacheDflush
	.set    noreorder

_cacheDflush:
        mtc0    zero, CP0_TAGLO
        nop
        nop

        .word 0x4080e001    # mtc0 zero,C0_TAGLO,1 - unsupported MIPS32 Instr
        nop
        nop

    la	    t0, cachedAddress	# jump cacheable
	li	    t1, ~0x20000000
	and	    t0, t1
	jr	    t0
	nop

cachedAddress:
  	li      t0, DCACHE_SIZE
  	sub     t0, t0, DCACHE_LINESIZE

dFlushLoop:
	nop
	nop
	nop
	nop
  	.word 0xbd090000    //cache   0x9, 0x0(t0)        # invalidate set
	nop
	nop
	nop
	nop

  	bne     t0, zero, dFlushLoop
  	sub     t0, DCACHE_LINESIZE

    la	    t0, exitDFlush		# return uncacheable
	li	    t1, 0x20000000
	or	    t0, t1
	jr	    t0
	nop

exitDFlush:
  	j	    ra
  	nop

    .set    reorder
	.end    _cacheDflush





#if 0

#include <asm/brcm/6352_cpu.h>

#define dcache_size        0x1000
#define icache_size       0x2000
#define dcache_linesize	   0x10
#define icache_linesize	   0x10


/************************************************************************
 * Macros used in cache functions										*
 *************************************************************************/
#define cacheop(kva, size, linesize, op) \
        .set    noreorder;       \
        .set 	mips3;				\
        addu    t1, kva, size;   \
        subu    t2, linesize, 1; \
        not     t2;              \
        and     t0, kva, t2;     \
        addu    t1, -1;          \
        and     t1, t2;          \
10:     cache   op, 0x0(t0);       \
	    cache   op, 0x1(t0);       \
        bne     t0, t1, 10b;     \
        addu    t0, linesize;    \
11:                              \
        .set 	mips2;				\
        .set    reorder


        .text
        .align 4

/*
#************************************************************************
#* 
#*     cacheALibInit: Initializes the caches for proper op              *
#*                                                                      *
#*      SYNTAX: void  cacheALibInit(void);                              *
#*     RETURNS:                                                         *
#*  Note:  We must be running UnCached since cache is not properly init *
#************************************************************************
*/
        .globl cacheALibInit
        .ent cacheALibInit

cacheALibInit:
        .set    noreorder

        /* force kseg1 */
        la      t0, 1f
        or      t0, 0xa0000000
        j       t0
        nop
1:

        /*--------------------------------------------------------------------
        # The caches may be in an indeterminate state, so we force good parity
        # into them by doing an invalidate, load/fill, inval. for each line.
        #---------------------------------------------------------------------*/

        /*--------------------------------------------------------------------
        #* Initialize TAGLO register
        #---------------------------------------------------------------------*/
        mtc0    zero, CP0_TAGLO
        nop
        nop

        /*--------------------------------------------------------------------
        #* Initialize DataLo register
        #---------------------------------------------------------------------*/
        .word 0x4080e001    # mtc0 zero,C0_TAGLO,1 - unsupported MIPS32 Instr
        nop
        nop

        /* Enable Caches before Clearing. If the caches are disabled
         * then the cache operations to clear the cache will be ignored
         */
        mfc0    t1, BRCM_CONFIG
		nop
        or      t1, CP0_CFG_ICSHEN|CP0_CFG_DCSHEN
        mtc0    t1, BRCM_CONFIG

        mfc0    t1, BCM6352_PROCCFG 
		nop
        and     t1, ~CP0_K0Coherency
        or      t1, CP0_K0WriteThrough
        mtc0    t1, BCM6352_PROCCFG

        mfc0    t1, BCM6352_STATUS
		nop
        and     t1, ~ST0_ERL
        mtc0    t1, BCM6352_STATUS

        /*--------------------------------------------------------------------
        # Initialise primary instruction cache.
        #---------------------------------------------------------------------*/
        li      t4, K0BASE
        li      t5, icache_size
        li      t6, icache_linesize
        cacheop(t4, t5, t6, Index_Store_Tag_I)

        /*--------------------------------------------------------------------
        # Initialise primary data cache.
        #---------------------------------------------------------------------*/
        li      t4, K0BASE
        li      t5, dcache_size
        li      t6, dcache_linesize
        cacheop(t4, t5, t6, Index_Store_Tag_D)

        mfc0    t1, BRCM_CONFIG
		nop
        and     t1, ~CP0_CFG_ICSHEN
        and     t1, ~CP0_CFG_DCSHEN
        mtc0    t1, BRCM_CONFIG

        mfc0    t1, BCM6352_PROCCFG 
		nop
        and     t1, ~CP0_K0Coherency
        or      t1, CP0_K0Uncached
        mtc0    t1, BCM6352_PROCCFG

        j       ra
        nop



        j       ra
        nop
        .set    reorder
        .end cacheALibInit


/*
#************************************************************************
# cacheALibEnable_I: Enable instruction cache.                          *
#                                                                       *
#       SYNTAX: void cacheALibEnable_I(void);                           *
#                                                                       *
#       INPUTS: None                                                    *
#                                                                       *
#       OUTPUTS: None                                                   *
#                                                                       *
#************************************************************************
*/
        .globl cacheALibEnable_I
        .ent cacheALibEnable_I
cacheALibEnable_I:
        .set    noreorder

        /* force kseg1 */
        la      t0, 1f
        or      t0, 0xa0000000
        j       t0
        nop
1:
        mfc0    t1, BRCM_CONFIG
		nop
        or      t1, CP0_CFG_ICSHEN
        mtc0    t1, BRCM_CONFIG
        j       ra
        nop
        .set    reorder
        .end    cacheALibEnable_I



/*
#************************************************************************
# cacheALibEnable_D: Enable data cache.                                 *
#                                                                       *
#       SYNTAX: void cacheALibEnable_D(void);                           *
#                                                                       *
#       INPUTS: None                                                    *
#                                                                       *
#*      OUTPUTS: None                                                   *
#*                                                                      *
#************************************************************************
*/
        .globl cacheALibEnable_D
        .ent cacheALibEnable_D
cacheALibEnable_D:
        .set    noreorder
        mfc0    t1, BRCM_CONFIG
		nop
        or      t1, CP0_CFG_DCSHEN
        mtc0    t1, BRCM_CONFIG

		li		t3, CP0_K0Uncached

        mfc0    t1, BCM6352_PROCCFG 
		nop
        and     t1, ~CP0_K0Coherency
        or      t1, t3
        mtc0    t1, BCM6352_PROCCFG

        j       ra
        nop
        .set    reorder
        .end    cacheALibEnable_D


/*
#************************************************************************
# cacheALibInvalidateAll_I: Invalidates entire instruction cache.       *
#                                                                       *
# SYNTAX: void cacheALibInvalidateAll_I(void);                          *
#                                                                       *
# INPUT:  None                                                          *
#                                                                       *
# OUTPUT: None                                                          *
#                                                                       *
#************************************************************************
*/
        .globl cacheALibInvalidateAll_I
        .ent cacheALibInvalidateAll_I
cacheALibInvalidateAll_I:
        .set    noreorder

        /* force kseg1 */
        la      t0, 1f
        or      t0, 0xa0000000
        j       t0
        nop
1:
        li      t4, K0BASE
        li      t5, icache_size
        li      t6, icache_linesize
        cacheop(t4, t5, t6, Index_Invalidate_I)
        nop
        nop
        nop
        nop
        nop
        nop
        j       ra
        nop
        .set    reorder
        .end    cacheALibInvalidateAll_I


/*
#************************************************************************
# cacheALibClearAll_D: Clears entire data cache.                        *
#                                                                       *
# SYNTAX: void cacheALibClearAll_D(void);                               *
#                                                                       *
# INPUT:  None                                                          *
#                                                                       *
# OUTPUT: None                                                          *
#                                                                       *
#************************************************************************
*/
        .globl cacheALibClearAll_D
        .ent cacheALibClearAll_D
cacheALibClearAll_D:
        .set    noreorder
        li      t4, K0BASE
        li      t5, dcache_size
        li      t6, dcache_linesize
        cacheop(t4, t5, t6, Index_Writeback_Inv_D)
        j       ra
        nop
        .set    reorder

        .end    cacheALibClearAll_D

#endif

