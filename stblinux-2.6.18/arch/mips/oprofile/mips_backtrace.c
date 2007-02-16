/*
 * MIPS specific backtracing code for oprofile
 *
 * Copyright 2006 Boardcom Corp.
 *
 * Author: Troy Trammel <ttrammel@broadcom.com>
 *
 * Based on i386 oprofile backtrace code by John Levon, David Smith
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/oprofile.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <asm/ptrace.h>
#include <asm/uaccess.h>

/* check that the page(s) containing the frame head are present */
static int pages_present(unsigned long addr)
{
	struct mm_struct * mm = current->mm;

	/* FIXME: only necessary once per page */
	if (!check_user_page_readable(mm, (unsigned long)addr))
		return 0;

	return check_user_page_readable(mm, (unsigned long)(addr + 1));
}

static unsigned long *user_getsp32(unsigned long *sp, int is_first)
{
	unsigned int stack_frame[2];


	if(!pages_present(*sp)) return 0;
	if (!access_ok(VERIFY_READ, *sp, sizeof(stack_frame)))
		return 0;
  
   /*
	 * The most likely reason for this is that we returned -EFAULT,
	 * which means that we've done all that we can do from
	 * interrupt context.
	 */
	if (__copy_from_user_inatomic(stack_frame, (void *)sp,
					sizeof(stack_frame)))
	{
 		return 0;
	}

	if(!pages_present(stack_frame[1])) return 0;

 	if (!is_first)
	{
 		oprofile_add_trace(stack_frame[1]);
	}

	/*
	 * We do not enforce increasing stack addresses here because
	 * we may transition to a different stack, eg a signal handler.
	 */
	return ((unsigned long *) stack_frame[0]);
}

static unsigned long *kernel_getsp(unsigned long *sp, int is_first)
{
	unsigned long *stack_frame = (unsigned long *)sp;
	unsigned long addr;

	if (kstack_end(stack_frame))
		return 0;

	addr = *stack_frame++;
	if (!is_first && __kernel_text_address(addr))
		oprofile_add_trace(addr);
	/*
	 * We do not enforce increasing stack addresses here because
	 * we might be transitioning from an interrupt stack to a kernel
	 * stack. validate_sp() is designed to understand this, so just
	 * use it.
	 */
	return stack_frame;
}

void
op_mips_backtrace(struct pt_regs * const regs, unsigned int depth)
{
	int first_frame = 1;
	unsigned long *sp;

	sp = (unsigned long *) regs->regs[29];

	/* We ditch the top stackframe so need to loop through an extra time */
	depth += 1;

	if (!user_mode(regs)) {
		while (depth--) {
 			 sp = kernel_getsp(sp, first_frame);
			 if(!sp) break;
			 first_frame = 0;
 		 }
		return;
	}
	else
	{
    	while (depth--) {
 			sp = user_getsp32(sp, first_frame);
			if (!sp)
				break;
			first_frame = 0;
		}
	}



#if 0
#ifdef CONFIG_SMP
	if (!spin_trylock(&current->mm->page_table_lock))
		return;
#endif

	while (depth-- && stack && pages_present(stack))
		head = dump_backtrace(head);

#ifdef CONFIG_SMP
	spin_unlock(&current->mm->page_table_lock);
#endif
#endif
}
