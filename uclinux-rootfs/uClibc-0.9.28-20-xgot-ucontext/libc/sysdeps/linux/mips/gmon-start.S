#undef sp
#define sp 29
	.file	1 "gmon-start.c"
	.section .mdebug.abi32
	.previous
	.abicalls
	.local	called.0
	.comm	called.0,4,4
	.text
	.align	2
	.globl	__gmon_start__
	.ent	__gmon_start__
	.type	__gmon_start__, @function
__gmon_start__:
	.frame	$sp,32,$31		# vars= 0, regs= 1/0, args= 16, gp= 8
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-32
	sw	$31,24($sp)
	.cprestore	16
	lw	$3,%got(called.0)($28)
	lw	$25,%call16(monstartup)($28)
	lw	$4,%got(__start)($28)
	lw	$2,%lo(called.0)($3)
	bne	$2,$0,$L1
	lw	$5,%got(etext)($28)

	li	$2,1			# 0x1
	jalr	$25
	sw	$2,%lo(called.0)($3)

	lw	$28,16($sp)
	lw	$31,24($sp)
	addiu	$sp,$sp,32
	lw	$25,%call16(atexit)($28)
	jr	$25
	lw	$4,%got(_mcleanup)($28)

$L1:
	lw	$31,24($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	__gmon_start__
	.ident	"GCC: (GNU) 3.4.6"
