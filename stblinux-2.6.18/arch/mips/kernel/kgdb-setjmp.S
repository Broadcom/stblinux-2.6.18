/*
 * arch/mips/kernel/kgdb-jmp.c
 *
 * Save and restore system registers so that within a limited frame we
 * may have a fault and "jump back" to a known safe location.
 *
 * Copyright (C) 2005 by MontaVista Software.
 * Author: Manish Lachwani (mlachwani@mvista.com)
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program as licensed "as is" without any warranty of
 * any kind, whether express or implied.
 */

#include <asm/asm.h>
#include <asm/mipsregs.h>
#include <asm/regdef.h>
#include <asm/stackframe.h>

	.ent	kgdb_fault_setjmp,0
ENTRY (kgdb_fault_setjmp)
	move    a1, sp
	move	a2, fp
#ifdef CONFIG_MIPS64
	nop
#endif
	j	kgdb_fault_setjmp_aux
	.end	kgdb_fault_setjmp

#ifdef CONFIG_MTI_R34K
LEAF(kgdb_show_wplo)
	mfc0	v0, CP0_WATCHLO
	jr	ra
	nop
	END(kgdb_show_wplo)

LEAF(kgdb_show_wphi)
	mfc0	v0, CP0_WATCHHI
	jr	ra
	nop
	END(kgdb_show_wphi)

LEAF(kgdb_show_wplo1)
	mfc0	v0, CP0_WATCHLO, 1
	jr	ra
	nop
	END(kgdb_show_wplo1)

LEAF(kgdb_show_wphi1)
	mfc0	v0, CP0_WATCHHI, 1
	jr	ra
	nop
	END(kgdb_show_wphi1)

LEAF(kgdb_show_wplo2)
	mfc0	v0, CP0_WATCHLO, 2
	jr	ra
	nop
	END(kgdb_show_wplo2)

LEAF(kgdb_show_wphi2)
	mfc0	v0, CP0_WATCHHI, 2
	jr	ra
	nop
	END(kgdb_show_wphi2)

LEAF(kgdb_show_wplo3)
	mfc0	v0, CP0_WATCHLO, 3
	jr	ra
	nop
	END(kgdb_show_wplo3)

LEAF(kgdb_show_wphi3)
	mfc0	v0, CP0_WATCHHI, 3
	jr	ra
	nop
	END(kgdb_show_wphi3)

LEAF(kgdb_set_wp)
	mtc0	a0, CP0_WATCHLO, 2
	nop
	mtc0	a1, CP0_WATCHHI, 2
	jr	ra
	li	v0, 0
	END(kgdb_set_wp)
#endif // CONFIG_MTI_R34K

