
/*
 * arch/mips/brcmstb/common/cfe_entry.S
 *
 * Copyright (C) 2001 Broadcom Corporation
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Low level interrupt code for Broadcom eval boards
 *
 */
#include <asm/asm.h>
#include <asm/mipsregs.h>
#include <asm/regdef.h>
#include <asm/stackframe.h>


#define CFE_SEAL	0x43464531


	.text
	.set	push
	.set	noreorder
	.set	noat
	.align	5



		EXPORT(firmhandl)
firmhandl:
		.word  0
			
		EXPORT(firmentry)
firmentry:
		.word  0
			
		EXPORT(cfe_seal)
cfe_seal:
		.word	0

		.text
###################################################################################
		LEAF(cfe_start)
		.set	at
#ifdef CONFIG_KGDB
	# PR33365: enable JTAG interface for hot attach
# jipeng - check DEBUG.DM to ensure dseg is accessible 
		mfc0 t0, CP0_DEBUG
		srl  t0, t0, 16
		andi t0, t0, 0x4000		
		beqz t0, patch_done	
		lui k0, 0xff30
		li  k1, 0x1a
		sw  k1, 0(k0)
patch_done:
#endif
		
	#Check to see if we were really launched by CFE
		sw  a3, cfe_seal
		li  t0, CFE_SEAL
		bne t0, a3, done
		nop

	# Store the parameters sent to us by CFE.

		sw  a0, firmhandl
		sw  a2, firmentry

done:    
		j	ra
		nop
		END(cfe_start)
###################################################################################
		NESTED(cfe_call, 64, sp)

		addiu	sp, sp, -64

		sw	s0, 16(sp)
		sw	s1, 20(sp)
		sw	s2, 24(sp)
		sw	s3, 28(sp)
		sw	s4, 32(sp)
		sw	s5, 36(sp)
		sw	s6, 40(sp)
		sw	s7, 44(sp)
		sw	s8, 48(sp)
		sw	gp, 52(sp)
		sw	ra, 56(sp)

		move	a1, a0
		lw   	a0, firmhandl
		lw   	t0, firmentry

		beqz	t0, 1f
		li	v0, -1

		jalr	t0
		nop

1:		lw	ra, 56(sp)
		lw	gp, 52(sp)
		lw	s8, 48(sp)
		lw	s7, 44(sp)
		lw	s6, 40(sp)
		lw	s5, 36(sp)
		lw	s4, 32(sp)
		lw	s3, 28(sp)
		lw	s2, 24(sp)
		lw	s1, 20(sp)
		lw	s0, 16(sp)

		jr	ra
		addiu	sp, sp, 64

		END(cfe_call)

		.set	pop

