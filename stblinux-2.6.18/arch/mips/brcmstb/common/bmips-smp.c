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
    These are the platform specific functions that need to be implemented when SMP
    is enabled in the kernel build.

when	who what
-----	---	----
3/10/06	TDT	Created based on Bcm4350 FPGA SMP Linux
4/12/06 TDT Rewrote logic to decouple IPC messages by using seperate interrupts
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

#include <asm/brcmstb/common/brcmstb.h>

// Marcos that we use on the 7400
#define read_c0_brcm_config_0()	__read_32bit_c0_register($22, 0)
#define write_c0_brcm_config_0(val) __write_32bit_c0_register($22, 0, val)

#define read_c0_cmt_interrupt()		__read_32bit_c0_register($22, 1)
#define write_c0_cmt_interrupt(val)	__write_32bit_c0_register($22, 1, val)

#define read_c0_cmt_control()		__read_32bit_c0_register($22, 2)
#define write_c0_cmt_control(val)	__write_32bit_c0_register($22, 2, val)

// Debug macros
#define read_other_tp_status()		__read_32bit_c0_register($12, 7)
#define read_other_tp_cause()		__read_32bit_c0_register($13, 7)

// used for passing boot arguments...
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
       int i, num;

        cpus_clear(phys_cpu_present_map);
        cpu_set(0, phys_cpu_present_map);
        __cpu_number_map[0] = 0;
        __cpu_logical_map[0] = 0;

        for (i = 1, num = 0; i < NR_CPUS; i++) {
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

static irqreturn_t brcm_reschedule_call_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	clear_c0_cause(C_SW1);
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
	printk("TP%d: prom_smp_finish enabling sw_irq\n", smp_processor_id());

	setup_irq(BCM_LINUX_IPC_0_IRQ, &brcm_ipc_action0);
	setup_irq(BCM_LINUX_IPC_1_IRQ, &brcm_ipc_action1);
	set_c0_status(IE_SW0 | IE_SW1 | IE_IRQ1 | ST0_IE);

	printk("TP%d: prom_smp_finish enabling local timer_interrupt\n",
		smp_processor_id());
        brcm_timerx_setup(&dummy_irq);
}

void prom_cpus_done(void)
{
	// Enable interrupts on TP0
	set_c0_status(IE_SW0 | IE_SW1 | ST0_IE);
}

void __init plat_prepare_cpus(unsigned int max_cpus)
{
	/* TODO - Currently 7400 has no boot options for 1 or 2 threads, this needs to come from CFE */
	boot_config->ncpus = 1;
  	if (!boot_config->ncpus) {
        printk("plat_prepare_cpus: leaving 2nd thread disabled...\n");
    } else {
        printk("plat_prepare_cpus: ENABLING 2nd Thread...\n");
        cpu_set(1, phys_cpu_present_map);
        cpu_set(1, cpu_present_map);
    }

#if defined(CONFIG_MIPS_7400A0)
    /* 7400A0 CMT "uncached assesses blocking d-cache from other TP" workaround */
	{
        volatile unsigned long * pCfg = (unsigned long * ) 0xbfa0001c;
        printk("enable 7400A0 CMT 'uncached assesses blocking d-cache from other TP' workaround\n");
		printk("0x2000000=>0xBFA0001C\n");
		*pCfg |= (1<<25);
     }
#endif

}

static DEFINE_SPINLOCK(ipi_lock);

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

/*
 * Try to detect a CFE image by looking for 8 consecutive jumps to the
 * same address, e.g.
 *
 *       8c:       1000038e        b       0xec8
 *       90:       00000000        nop
 *       94:       1000038c        b       0xec8
 *       98:       00000000        nop
 *       9c:       1000038a        b       0xec8
 *       a0:       00000000        nop
 *       ...
 *
 * These correspond to the RVECENT() macros in reset.s and exist in all
 * recent CFE images.
 */

static int cfecheck(uint32_t *buf, unsigned int len)
{
	unsigned int i, count = 0, last_dst = 0;

	len >>= 2;

	for(i = 0; i < (len - 1); i++) {
		if(((buf[i] & 0xffff0000) == 0x10000000) &&
			(buf[i + 1] == 0x0)) {
			unsigned int dst = buf[i] & 0xffff;

			dst = (dst + i + 1) << 2;
			if(! dst || (dst > 0x8000))
				continue;
			if(dst == last_dst) {
				count++;
				if(count == 8)
					return(0);
			} else {
				last_dst = dst;
				count = 1;
			}
		}
	}
	return(-1);
}

/*
 * Setup the PC, SP, and GP of a secondary processor and start it
 * running!
 */
void prom_boot_secondary(int cpu, struct task_struct *idle)
{
    register unsigned long temp;

    if(tp1_running) {
	// TP1 was booted and then shut down - trigger SW1 to wake it up
	set_c0_cause(C_SW1);
	return;
    }

    // disable non-blocking mode
    temp = read_c0_brcm_config_0();
    // PR22809 Add NBK and weak order flags
	temp |= 0x30000;
    write_c0_brcm_config_0(temp);
  
    // Configure the s/w interrupt that TP0 will use to kick TP1.  By default,
    // s/w interrupt 0 will be vectored to the TP that generated it, so we need
    // to set the interrupt routing bit to make it cross over to the other TP.
    // This is bit 15 (SIR) of CP0 Reg 22 Sel 1. We also want to make h/w IRQ 1
    // go to TP1. To do this, we need to set the appropriate bit in the h/w
    // interrupt crossover. To set the h/w interrupt crossover, we need to set 
    // a different bit in that same CP0 register.  The XIR bits (31:27) 
    // control h/w IRQ 4..0, respectively.
    
    temp = read_c0_cmt_interrupt();
    temp |= 0x10018000;
    write_c0_cmt_interrupt(temp);

    //printk("TP%d: prom_boot_secondary: c0 22,1=%lx\n", smp_processor_id(),temp);
    
	// first save the boot data...
    boot_config->func_addr = (unsigned long) smp_bootstrap;      // function 
    boot_config->sp        = (unsigned long) __KSTK_TOS(idle);    // sp
    boot_config->gp        = (unsigned long) idle->thread_info;   // gp
    boot_config->arg       = 0;

    // TP1 always starts executing from bfc0_0000 (MIPS reset vector)
    // If CFE has been remapped to somewhere else (e.g. by changing EBI
    // settings), TP1 will not start.

    if(cfecheck((void *)0xbfc00000, 0x400) == -1)
	    printk("WARNING: CFE image not detected at reset vector; TP1 may not start\n");
     
    printk("TP%d: prom_boot_secondary: Kick off 2nd CPU...\n", smp_processor_id());
    temp = read_c0_cmt_control();

	// The following flags can be adjusted to suit performance and debugging needs
    //temp |= (1<<4);           // TP0 priority
    //temp |= (1<<5);           // TP1 priority

//	temp |= (0x01 << 30);	// Debug and profile TP1 thread

    //printk("TP%d: prom_boot_secondary: cp0 22,2 => %08lx\n", smp_processor_id(), temp);
    temp |= 0x01;           // Takes second CPU out of reset
    write_c0_cmt_control(temp);

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
