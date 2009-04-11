/*
<:copyright-gpl 
 Copyright 2005-2006 Broadcom Corp. All Rights Reserved. 
 
 This program is free software; you can distribute it and/or modify it 
 under the terms of the GNU General Public License (Version 2) as 
 published by the Free Software Foundation. 
 
 This program is distributed in the hope it will be useful, but WITHOUT 
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
 for more details. 
 
 You should have received a copy of the GNU General Public License along 
 with this program; if not, write to the Free Software Foundation, Inc., 
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA. 
:>
*/

/*
 * These are the platform specific functions that need to be implemented
 * when SMP is enabled in the kernel build.
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/smp.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <asm/io.h>
#include <asm/pgtable.h>
#include <asm/processor.h>
#include <asm/reboot.h>
#include <asm/system.h>
#include <asm/bootinfo.h>
#include <asm/pmon.h>
#include <asm/time.h>
#include <asm/cpu.h>
#include <asm/smp.h>
#include <asm/cacheflush.h>
#include <asm/mipsregs.h>
#include <asm/brcmstb/common/brcmstb.h>

// used for passing boot arguments to TP1
struct BootConfig {
	unsigned long signature;
	unsigned long ncpus;
	unsigned long func_addr;
	unsigned long sp;
	unsigned long gp;
	unsigned long arg;
} g_boot_config;
struct BootConfig * boot_config = (struct BootConfig * ) &g_boot_config;

extern cpumask_t phys_cpu_present_map;		/* Bitmask of available CPUs */
extern void brcm_timerx_setup(struct irqaction *);
extern irqreturn_t timerx_interrupt(int, void *, struct pt_regs *);

static int tp1_running = 0;

void __init plat_smp_setup(void)
{
	int i, num, lim;

	cpus_clear(phys_cpu_present_map);
	cpu_set(0, phys_cpu_present_map);
	__cpu_number_map[0] = 0;
	__cpu_logical_map[0] = 0;

	lim = brcm_smp_enabled ? NR_CPUS : 1;

	for (i = 1, num = 0; i < lim; i++) {
		cpu_set(i, phys_cpu_present_map);
		__cpu_number_map[i] = ++num;
		__cpu_logical_map[num] = i;
	}
}

void prom_init_secondary(void)
{
	// don't do anything...
}

// Handle interprocessor messages
static irqreturn_t brcm_smp_call_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	/* this clears the interrupt right before acknowledging it */
	smp_call_function_interrupt();
	return IRQ_HANDLED;
}

#if defined(CONFIG_BMIPS4380)
static DEFINE_SPINLOCK(ipi_lock);
#endif

static irqreturn_t brcm_reschedule_call_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
#if defined(CONFIG_BMIPS4380)
	unsigned long flags;

	spin_lock_irqsave(&ipi_lock, flags);
	clear_c0_cause(C_SW1);
	spin_unlock_irqrestore(&ipi_lock, flags);
#elif defined(CONFIG_BMIPS5000)
	write_c0_brcm_action(0x2000 | (raw_smp_processor_id() << 9) | (1 << 8));
#endif
	return IRQ_HANDLED;
}

static struct irqaction brcm_ipc_action0 = {
	.handler	= brcm_smp_call_interrupt,
	.flags		= IRQF_DISABLED,
	.mask		= CPU_MASK_NONE,
	.name		= "ipc0",
	.next		= NULL,
	.dev_id		= brcm_smp_call_interrupt,
};

static struct irqaction brcm_ipc_action1 = {
	.handler	= brcm_reschedule_call_interrupt,
	.flags		= IRQF_DISABLED,
	.mask		= CPU_MASK_NONE,
	.name		= "ipc1",
	.next		= NULL,
	.dev_id		= brcm_reschedule_call_interrupt,
};

static struct irqaction dummy_irq = {
	.handler	= timerx_interrupt,
	.flags		= SA_INTERRUPT,
	.mask		= CPU_MASK_NONE,
	.name		= "timer TP1",
	.next		= NULL,
};

// for DT-MIPS this always runs from 2nd CPU
// FIX this later so it can run from 3rd/4th, etc. CPU...
void prom_smp_finish(void)
{
	setup_irq(BCM_LINUX_IPC_0_IRQ, &brcm_ipc_action0);
	setup_irq(BCM_LINUX_IPC_1_IRQ, &brcm_ipc_action1);
	set_c0_status(IE_SW0 | IE_SW1 | IE_IRQ1 | ST0_IE);

	brcm_timerx_setup(&dummy_irq);
}

void prom_cpus_done(void)
{
	// Enable interrupts on TP0
	set_c0_status(IE_SW0 | IE_SW1 | ST0_IE);
}

void __init plat_prepare_cpus(unsigned int max_cpus)
{
	boot_config->ncpus = 1;

	if (!boot_config->ncpus) {
		printk("plat_prepare_cpus: leaving 2nd thread disabled...\n");
	} else {
		printk("plat_prepare_cpus: ENABLING 2nd Thread...\n");
		cpu_set(1, phys_cpu_present_map);
		cpu_set(1, cpu_present_map);
	}
}

#if defined(CONFIG_BMIPS4380)
void core_send_ipi(int cpu, unsigned int action)
{
	unsigned long flags;

	spin_lock_irqsave(&ipi_lock, flags);

	switch (action) {
		case SMP_CALL_FUNCTION:
			set_c0_cause(C_SW0);
			break;
		case SMP_RESCHEDULE_YOURSELF:
			set_c0_cause(C_SW1);
			break;
		default:
			BUG();
			break;
	}

	spin_unlock_irqrestore(&ipi_lock, flags);
}
#elif defined(CONFIG_BMIPS5000)
void core_send_ipi(int cpu, unsigned int action)
{
	switch (action) {
		case SMP_CALL_FUNCTION:
			write_c0_brcm_action(0x3000 | (0 << 8) | (cpu << 9));
			break;
		case SMP_RESCHEDULE_YOURSELF:
			write_c0_brcm_action(0x3000 | (1 << 8) | (cpu << 9));
			break;
		default:
			BUG();
			break;
	}
}
#endif

static void tp1_entry(void)
{
	unsigned long tmp0, tmp1;

	__asm__ __volatile__(
	"	.set	push\n"
	"	.set	reorder\n"

	/* enable FPU and initialize FPU registers */
	"	mfc0	%0, $12\n"
	"	li	%1, 0x20000000\n"
	"	or	%0, %1\n"
	"	mtc0	%0, $12\n"
	"	ssnop\n"
	"	ssnop\n"
	"	ssnop\n"
	"	mtc1	$0, $f0\n"
	"	mtc1	$0, $f1\n"
	"	mtc1	$0, $f2\n"
	"	mtc1	$0, $f3\n"
	"	mtc1	$0, $f4\n"
	"	mtc1	$0, $f5\n"
	"	mtc1	$0, $f6\n"
	"	mtc1	$0, $f7\n"
	"	mtc1	$0, $f8\n"
	"	mtc1	$0, $f9\n"
	"	mtc1	$0, $f10\n"
	"	mtc1	$0, $f11\n"
	"	mtc1	$0, $f12\n"
	"	mtc1	$0, $f13\n"
	"	mtc1	$0, $f14\n"
	"	mtc1	$0, $f15\n"
	"	mtc1	$0, $f16\n"
	"	mtc1	$0, $f17\n"
	"	mtc1	$0, $f18\n"
	"	mtc1	$0, $f19\n"
	"	mtc1	$0, $f20\n"
	"	mtc1	$0, $f21\n"
	"	mtc1	$0, $f22\n"
	"	mtc1	$0, $f23\n"
	"	mtc1	$0, $f24\n"
	"	mtc1	$0, $f25\n"
	"	mtc1	$0, $f26\n"
	"	mtc1	$0, $f27\n"
	"	mtc1	$0, $f28\n"
	"	mtc1	$0, $f29\n"
	"	mtc1	$0, $f30\n"
	"	mtc1	$0, $f31\n"

#ifdef CONFIG_BMIPS4380
	/* initialize TP1's local I-cache */
	"	li	%0, 0x80000000\n"
	"	li	%1, 0x80008000\n"
	"	mtc0	$0, $28\n"
	"	mtc0	$0, $28, 1\n"
	"	nop\n"
	"	nop\n"
	"	nop\n"

	"1:	cache	0x08, 0(%0)\n"
	"	addiu	%0, 64\n"
	"	bne	%0, %1, 1b\n"
#endif

#ifdef CONFIG_BMIPS5000
	/* set local CP0 CONFIG to make kseg0 cacheable, write-back */
	"	mfc0	%0, $16\n"
	"	ori	%0, 0x7\n"
	"	xori	%0, 0x4\n"
	"	mtc0	%0, $16\n"
	"	ssnop\n"
	"	ssnop\n"
	"	ssnop\n"
#endif

	/* jump to kernel_entry */
	"	la	%0, kernel_entry\n"
	"	jr	%0\n"
	"	.set	pop\n"
	: "=&r" (tmp0), "=&r" (tmp1)
	:
	: "memory");
}

/*
 * Setup the PC, SP, and GP of a secondary processor and start it
 * running!
 */
void prom_boot_secondary(int cpu, struct task_struct *idle)
{
	unsigned long entry;

	if(tp1_running) {
		// TP1 was booted and then shut down; trigger SW1 to wake it up
		core_send_ipi(raw_smp_processor_id() ^ 1,
			SMP_RESCHEDULE_YOURSELF);
		return;
	}

#if defined(CONFIG_BMIPS4380)
	// PR22809: NBK and weak order flags
	set_c0_brcm_config_0(0x30000);

	// Configure the s/w interrupt that TP0 will use to kick TP1.  By
	// default, s/w interrupt 0 will be vectored to the TP that generated
	// it, so we need to set the interrupt routing bit to make it cross
	// over to the other TP.  This is bit 15 (SIR) of CP0 Reg 22 Sel 1.
	// We also want to make h/w IRQ 1 go to TP1. To do this, we need to
	// set the appropriate bit in the h/w interrupt crossover. To set
	// the h/w interrupt crossover, we need to set a different bit in
	// that same CP0 register.  The XIR bits (31:27) control h/w IRQ 4..0,
	// respectively.

	change_c0_brcm_cmt_intr(0xf8018000, (0x02 << 27) | (0x03 << 15));
#elif defined(CONFIG_BMIPS5000)
	/* enable raceless SW interrupts */
	set_c0_brcm_config(0x03 << 22);
	/* send HW interrupt 0 to TP0, HW interrupt 1 to TP1 */
	change_c0_brcm_mode(0x1f << 27, 0x02 << 27);
#endif

	// first set up the TP1 boot data...
	boot_config->func_addr	= (unsigned long) smp_bootstrap;
	boot_config->sp		= (unsigned long) __KSTK_TOS(idle);
	boot_config->gp		= (unsigned long) idle->thread_info;
	boot_config->arg	= 0;

	/*
	 * NOTE: TP0/TP1 reset and exception vectors have been relocated
	 * TP1 will start executing the jump code (below) from a000_0000.
	 * Changes are in include/asm/mach-brcmstb/kernel-entry-init.h and
	 * arch/mips/kernel/traps.c (search for BMIPS4380).
	 */

	entry = KSEG1ADDR((u32)&tp1_entry);
	DEV_WR(KSEG0 + 0, 0x3c1a0000 | (entry >> 16));	  // lui k0, HI(entry)
	DEV_WR(KSEG0 + 4, 0x375a0000 | (entry & 0xffff)); // ori k0, LO(entry)
	DEV_WR(KSEG0 + 8, 0x03400008);			  // jr k0
	DEV_WR(KSEG0 + 12, 0x00000000);			  // nop

	dma_cache_wback(KSEG0, 16);
	flush_icache_range(KSEG0, KSEG0 + 16);

	printk("TP%d: prom_boot_secondary: Kick off 2nd CPU...\n",
		smp_processor_id());
#if defined(CONFIG_BMIPS4380)
	set_c0_brcm_cmt_ctrl(0x01);
#elif defined(CONFIG_BMIPS5000)
	write_c0_brcm_action(0x9);
#endif

	tp1_running = 1;
}

#ifdef CONFIG_HOTPLUG_CPU

static DECLARE_COMPLETION(cpu_killed);

int mach_cpu_disable(unsigned int cpu)
{
	/* not allowed to shut down TP0 */
	return cpu == 0 ? -EPERM : 0;
}

/* runs on living CPU */
int platform_cpu_kill(unsigned int cpu)
{
	return(wait_for_completion_timeout(&cpu_killed, 5000));
}

/* runs on dying CPU */
void platform_cpu_die(unsigned int cpu)
{
	complete(&cpu_killed);
	mb();

	/* enable SW1 for wakeup, disable all other interrupts */
	write_c0_status((read_c0_status() & ~0xff00) | IE_SW1 | ST0_IE);
	mb();

	/* wait for wakeup IPI from TP0 */
	__asm__ __volatile__(
		".set	push\n"
		".set	mips3\n"
		"wait\n"
		".set	pop\n");

	/* reset stack pointer and __current_thread_info ($gp) */
	local_irq_disable();
	mb();

	__asm__ __volatile__(
		".extern g_boot_config\n"
		"la	$8, g_boot_config\n"
		"lw	$29, 12($8)\n"
		"lw	$28, 16($8)\n"
		);
	
	/* unmask the usual interrupts */
	set_c0_status(IE_SW0 | IE_SW1 | IE_IRQ1 | IE_IRQ5);
	mb();

	__asm__ __volatile__("j restart_secondary\n");
}

#endif /* CONFIG_HOTPLUG_CPU */
