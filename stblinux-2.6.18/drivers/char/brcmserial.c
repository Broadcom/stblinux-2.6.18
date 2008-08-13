/*
 *  linux/drivers/char/brcmserial.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *  Copyright (C) 1992, 1993, 1994, 1995, 1996, 1997, 
 * 		1998, 1999  Theodore Ts'o
 * Copyright (C) 2002-2006 Broadcom Corporation
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
 *  12/00 Ported to Broadcom BCM93725B/C SetTop platform with BCM3250
 *  UPG serial ports by Mike Steiger <msteiger@broadcom.com>.
 *
 *  Extensively rewritten by Theodore Ts'o, 8/16/92 -- 9/14/92.  Now
 *  much more extensible to support other serial cards based on the
 *  16450/16550A UART's.  Added support for the AST FourPort and the
 *  Accent Async board.  
 *
 *  set_serial_info fixed to set the flags, custom divisor, and uart
 * 	type fields.  Fix suggested by Michael K. Johnson 12/12/92.
 *
 *  11/95: TIOCMIWAIT, TIOCGICOUNT by Angelo Haritsis <ah@doc.ic.ac.uk>
 *
 *  03/96: Modularised by Angelo Haritsis <ah@doc.ic.ac.uk>
 *
 *  rs_set_termios fixed to look also for changes of the input
 *      flags INPCK, BRKINT, PARMRK, IGNPAR and IGNBRK.
 *                                            Bernd Anhupl 05/17/96.
 *
 *  1/97:  Extended dumb serial ports are a config option now.  
 *         Saves 4k.   Michael A. Griffith <grif@acm.org>
 * 
 *  8/97: Fix bug in rs_set_termios with RTS
 *        Stanislav V. Voronyi <stas@uanet.kharkov.ua>
 *
 *  3/98: Change the IRQ detection, use of probe_irq_o*(),
 *	  supress TIOCSERGWILD and TIOCSERSWILD
 *	  Etienne Lorrain <etienne.lorrain@ibm.net>
 *
 *  4/98: Added changes to support the ARM architecture proposed by
 * 	  Russell King
 *
 *  5/99: Updated to include support for the XR16C850 and ST16C654
 *        uarts.  Stuart MacDonald <stuartm@connecttech.com>
 *
 *  8/99: Generalized PCI support added.  Theodore Ts'o
 * 
 *  3/00: Rid circular buffer of redundant xmit_cnt.  Fix a
 *	  few races on freeing buffers too.
 *	  Alan Modra <alan@linuxcare.com>
 *
 *  5/00: Support for the RSA-DV II/S card added.
 *	  Kiyokazu SUTO <suto@ks-and-ks.ne.jp>
 * 
 *  6/00: Remove old-style timer, use timer_list
 *        Andrew Morton <andrewm@uow.edu.au>
 *
 *  7/00: Support Timedia/Sunix/Exsys PCI cards
 *
 *  7/05: Fix for ttyS0/ttyS1 baud rate setting problem
 *        Richard Hsu <ryhsu@broadcom.com>
 *
 * This module exports the following rs232 io functions:
 *
 *	int rs_init(void);
 */



static char *serial_version = "1.00";
static char *serial_revdate = "2000-11-09";

char _str_[128];
/*
 * Serial driver configuration section.  Here are the various options:
 *
 * CONFIG_HUB6
 *		Enables support for the venerable Bell Technologies
 *		HUB6 card.
 *
 * CONFIG_SERIAL_MANY_PORTS
 * 		Enables support for ports beyond the standard, stupid
 * 		COM 1/2/3/4.
 *
 * CONFIG_SERIAL_MULTIPORT
 * 		Enables support for special multiport board support.
 *
 * CONFIG_SERIAL_DETECT_IRQ
 *		Enable the autodetection of IRQ on standart ports
 *
 * SERIAL_PARANOIA_CHECK
 * 		Check the magic number for the async_structure where
 * 		ever possible.
 */

#include <linux/config.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
	#define MOD_DEC_USE_COUNT
	#define MOD_INC_USE_COUNT
#endif

#undef SERIAL_PARANOIA_CHECK
#define CONFIG_SERIAL_NOPAUSE_IO
#define SERIAL_DO_RESTART


#if 0
/* These defines are normally controlled by the autoconf.h */
#define CONFIG_SERIAL_MANY_PORTS
#define CONFIG_SERIAL_SHARE_IRQ
#define CONFIG_SERIAL_DETECT_IRQ
#define CONFIG_SERIAL_MULTIPORT
#define CONFIG_HUB6
#endif

#ifdef CONFIG_PCI
#define ENABLE_SERIAL_PCI
#ifndef CONFIG_SERIAL_SHARE_IRQ
#define CONFIG_SERIAL_SHARE_IRQ
#endif
#ifndef CONFIG_SERIAL_MANY_PORTS
#define CONFIG_SERIAL_MANY_PORTS
#endif
#endif

#if defined(CONFIG_ISAPNP)|| (defined(CONFIG_ISAPNP_MODULE) && defined(MODULE))
#ifndef ENABLE_SERIAL_PNP
#define ENABLE_SERIAL_PNP
#endif
#endif

/* Set of debugging defines */

#undef  SERIAL_DEBUG_INTR
#undef  SERIAL_DEBUG_OPEN
#undef  SERIAL_DEBUG_FLOW
#undef  SERIAL_DEBUG_RS_WAIT_UNTIL_SENT
#undef  SERIAL_DEBUG_PCI
#undef  SERIAL_DEBUG_AUTOCONF

#if defined( SERIAL_DEBUG_OPEN ) || defined( SERIAL_DEBUG_INTR )
#define dbg_print(msg) uartB_puts(msg)
#endif

/* Sanity checks */

#ifdef CONFIG_SERIAL_SHARE_IRQ
#undef CONFIG_SERIAL_SHARE_IRQ
#endif

#define RS_STROBE_TIME (10*HZ)
#define RS_ISR_PASS_LIMIT 256

#if defined(__i386__) && (defined(CONFIG_M386) || defined(CONFIG_M486))
#define SERIAL_INLINE
#endif
  
/*
 * End of serial driver configuration section.
 */

#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <linux/types.h>
#ifdef LOCAL_HEADERS
#include "serial_local.h"
#else
#include <linux/serial.h>
#include <linux/serialP.h>
#include <linux/serial_reg.h>
#include <asm/serial.h>
#define LOCAL_VERSTRING ""
#endif

#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/major.h>
#include <linux/string.h>
#include <linux/fcntl.h>
#include <linux/ptrace.h>
#include <linux/ioport.h>
#include <linux/mm.h>
#include <linux/slab.h>
#if (LINUX_VERSION_CODE >= 131343)
#include <linux/init.h>
#endif
#if (LINUX_VERSION_CODE >= 131336)
#include <asm/uaccess.h>
#endif
#include <linux/delay.h>
#ifdef CONFIG_SERIAL_CONSOLE
#include <linux/console.h>
#endif
#ifdef ENABLE_SERIAL_PCI
#include <linux/pci.h>
#endif
#ifdef ENABLE_SERIAL_PNP
#include <linux/isapnp.h>
#endif
#ifdef CONFIG_MAGIC_SYSRQ
#include <linux/sysrq.h>
#endif

/*
 * All of the compatibilty code so we can compile serial.c against
 * older kernels is hidden in serial_compat.h
 */
#if defined(LOCAL_HEADERS) || (LINUX_VERSION_CODE < 0x020317) /* 2.3.23 */
#include "serial_compat.h"
#endif

#ifdef CONFIG_MIPS_BRCM97XXX
#include <asm/brcmstb/common/serial.h>
#endif

#include <asm/system.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/bitops.h>

#ifdef CONFIG_MAC_SERIAL
#define SERIAL_DEV_OFFSET	2
#else
#define SERIAL_DEV_OFFSET	0
#endif

#ifdef SERIAL_INLINE
#define _INLINE_ inline
#else
#define _INLINE_
#endif

static char *serial_name = "Broadcom serial driver";


static struct tty_driver* serial_driver;
//static int serial_refcount;

static struct timer_list serial_timer;

/* serial subtype definitions */
#ifndef SERIAL_TYPE_NORMAL
#define SERIAL_TYPE_NORMAL	1
#define SERIAL_TYPE_CALLOUT	2
#endif

/* number of characters left in xmit buffer before we ask for more */
#define WAKEUP_CHARS 256

/*
 * This event is scheduled to run at timer-interrupt
 * time, instead of at rs interrupt time.
 */

#define BRCMSERIAL_RECEIVE      3

/*
 * IRQ_timeout		- How long the timeout should be for each IRQ
 * 				should be after the IRQ has been active.
 */

static struct async_struct *IRQ_ports[NR_IRQS];
static int IRQ_timeout[NR_IRQS];

/*
 * ADTADT
 * Byte FIFO for the two serial port, 256 bytes deep for now
 */
#define BRCMSERIAL_FIFO_DEPTH              256

static char *brcmserial_fifo[NR_IRQS];
static char *brcmserial_fifo_stat[NR_IRQS];
static int  brcmserial_fifo_readptr[NR_IRQS];
static int  brcmserial_fifo_writeptr[NR_IRQS];


#ifdef CONFIG_SERIAL_CONSOLE
static struct console sercons;

#endif

static void change_speed(struct async_struct *info, struct termios *old);
static void rs_wait_until_sent(struct tty_struct *tty, int timeout);
static void autoconfig(struct serial_state * state);
/*
 * Here we define the default xmit fifo size used for each type of
 * UART
 */
static struct serial_uart_config uart_config[] = {
	{ "unknown", 1, 0 }, 
	{ "8250", 1, 0 }, 
	{ "16450", 1, 0 }, 
	{ "16550", 1, 0 }, 
	{ "16550A", 16, UART_CLEAR_FIFO | UART_USE_FIFO }, 
	{ "cirrus", 1, 0 }, 	/* usurped by cyclades.c */
	{ "ST16650", 1, UART_CLEAR_FIFO |UART_STARTECH }, 
	{ "ST16650V2", 32, UART_CLEAR_FIFO | UART_USE_FIFO |
		  UART_STARTECH }, 
	{ "TI16750", 64, UART_CLEAR_FIFO | UART_USE_FIFO},
	{ "Startech", 1, 0},	/* usurped by cyclades.c */
	{ "16C950/954", 128, UART_CLEAR_FIFO | UART_USE_FIFO},
	{ "ST16654", 64, UART_CLEAR_FIFO | UART_USE_FIFO |
		  UART_STARTECH }, 
	{ "XR16850", 128, UART_CLEAR_FIFO | UART_USE_FIFO |
		  UART_STARTECH },
	{ "RSA", 2048, UART_CLEAR_FIFO | UART_USE_FIFO }, 
	{ 0, 0}
};

static struct serial_state rs_table[RS_TABLE_SIZE] = {
	SERIAL_PORT_DFNS	/* Defined in serial.h */
};

#define NR_PORTS	(sizeof(rs_table)/sizeof(struct serial_state))

#if (defined(ENABLE_SERIAL_PCI) || defined(ENABLE_SERIAL_PNP))
#define NR_PCI_BOARDS	8
/* We don't unregister PCI boards right now */

#ifndef IS_PCI_REGION_IOPORT
#define IS_PCI_REGION_IOPORT(dev, r) (pci_resource_flags((dev), (r)) & \
				      IORESOURCE_IO)
#endif
#ifndef PCI_IRQ_RESOURCE
#define PCI_IRQ_RESOURCE(dev, r) ((dev)->irq_resource[r].start)
#endif
#ifndef pci_get_subvendor
#define pci_get_subvendor(dev) ((dev)->subsystem_vendor)
#define pci_get_subdevice(dev)  ((dev)->subsystem_device)
#endif
#endif	/* ENABLE_SERIAL_PCI || ENABLE_SERIAL_PNP  */

#ifndef PREPARE_FUNC
#define PREPARE_FUNC(dev)  (dev->prepare)
#define ACTIVATE_FUNC(dev)  (dev->activate)
#define DEACTIVATE_FUNC(dev)  (dev->deactivate)
#endif

#define HIGH_BITS_OFFSET ((sizeof(long)-sizeof(int))*8)

static struct tty_struct *serial_table[NR_PORTS];
static struct termios *serial_termios[NR_PORTS];
static struct termios *serial_termios_locked[NR_PORTS];


#if defined(MODULE) && defined(SERIAL_DEBUG_MCOUNT)
#define DBG_CNT(s) printk("(%s): [%x] refc=%d, serc=%d, ttyc=%d -> %s\n", \
 kdevname(tty->device), (info->flags), 0, info->count,tty->count,s)
#else
#define DBG_CNT(s)
#endif

/*
 * tmp_buf is used as a temporary buffer by serial_write.  We need to
 * lock it in case the copy_from_user blocks while swapping in a page,
 * and some other program tries to do a serial write at the same time.
 * Since the lock will only come under contention when the system is
 * swapping and available memory is low, it makes sense to share one
 * buffer across all the serial ports, since it significantly saves
 * memory if large numbers of serial ports are open.
 */
static unsigned char *tmp_buf;
#ifdef DECLARE_MUTEX
static DECLARE_MUTEX(tmp_buf_sem);
#else
static struct semaphore tmp_buf_sem = MUTEX;
#endif

#ifdef CONFIG_KGDB
extern void breakpoint();
extern int kgdb_detached;    /* RYH */
#endif

static inline int serial_paranoia_check(struct async_struct *info,
					char* kname, const char *routine)
{
#ifdef SERIAL_PARANOIA_CHECK
	static const char *badmagic =
		"Warning: bad magic number for serial struct (%s) in %s\n";
	static const char *badinfo =
		"Warning: null async_struct for (%s) in %s\n";

	if (!info) {
		printk(badinfo, kname, routine);
		return 1;
	}
	if (info->magic != SERIAL_MAGIC) {
		printk(badmagic, kname, routine);
		return 1;
	}
#endif
	return 0;
}

static _INLINE_ unsigned int serial_in(struct async_struct *info, int offset)
{
/* PR12020 - Most non-pci systems are happy with no changes for endianess, Pci based Bcm97038 needs this though */
#if !defined(CONFIG_CPU_LITTLE_ENDIAN) && defined(CONFIG_PCI)
      return readb(((unsigned long *) ((unsigned long)info->iomem_base +
	     ((offset<<info->iomem_reg_shift)^3))));
#else
      return readb(((unsigned long *) ((unsigned long)info->iomem_base +
	     (offset<<info->iomem_reg_shift))));
#endif
}
static _INLINE_ void serial_out(struct async_struct *info, int offset,
				int value)
{
/* PR12020 - Most non-pci systems are happy with no changes for endianess, Pci based Bcm97038 needs this though */
#if !defined(CONFIG_CPU_LITTLE_ENDIAN) && defined(CONFIG_PCI)
	writeb(value, ((unsigned long *) ((unsigned long)info->iomem_base +
		((offset<<info->iomem_reg_shift)^3))));
#else
	writeb(value, ((unsigned long *) ((unsigned long)info->iomem_base +
		(offset<<info->iomem_reg_shift))));
#endif
}

/*
 * We used to support using pause I/O for certain machines.  We
 * haven't supported this for a while, but just in case it's badly
 * needed for certain old 386 machines, I've left these #define's
 * in....
 */
#define serial_inp(info, offset)		serial_in(info, offset)
#define serial_outp(info, offset, value)	serial_out(info, offset, value)

/*
 * ------------------------------------------------------------
 * rs_stop() and rs_start()
 *
 * This routines are called before setting or resetting tty->stopped.
 * They enable or disable transmitter interrupts, as necessary.
 * ------------------------------------------------------------
 */
static void rs_stop(struct tty_struct *tty)
{
	struct async_struct *info = (struct async_struct *)tty->driver_data;
	unsigned long flags;
	if (serial_paranoia_check(info, tty->name, "rs_stop"))
		return;
	
	local_irq_save(flags);
	
	serial_out(info, UART_XMIT_STATUS, 0);
	
	local_irq_restore(flags);
}

static void rs_start(struct tty_struct *tty)
{
	struct async_struct *info = (struct async_struct *)tty->driver_data;
	unsigned long flags;
	unsigned char ctrl;
	
	if (serial_paranoia_check(info, tty->name, "rs_start"))
		return;
	
	local_irq_save(flags);
	
	ctrl = serial_inp(info, UART_CONTROL);
	ctrl |= (UART_RE | UART_TE);
	serial_out(info, UART_CONTROL, ctrl);
	
	serial_out(info, UART_XMIT_STATUS, UART_TIE);
	serial_out(info, UART_RECV_STATUS, UART_RIE);
	
	local_irq_restore(flags);
}

/*
 * ----------------------------------------------------------------------
 *
 * Here starts the interrupt handling routines.  All of the following
 * subroutines are declared as inline and are folded into
 * rs_interrupt().  They were separated out for readability's sake.
 *
 * Note: rs_interrupt() is a "fast" interrupt, which means that it
 * runs with interrupts turned off.  People who may want to modify
 * rs_interrupt() should try to keep the interrupt handler as fast as
 * possible.  After you are done making modifications, it is not a bad
 * idea to do:
 * 
 * gcc -S -DKERNEL -Wall -Wstrict-prototypes -O6 -fomit-frame-pointer serial.c
 *
 * and look at the resulting assemble code in serial.s.
 *
 * 				- Ted Ts'o (tytso@mit.edu), 7-Mar-93
 * -----------------------------------------------------------------------
 */

/*
 * This routine is used by the interrupt handler to schedule
 * processing in the software interrupt portion of the driver.
 */
static _INLINE_ void rs_sched_event(struct async_struct *info,
				  int event)
{
	info->event |= 1 << event;
	//schedule_work(&info->work);
	tasklet_schedule(&info->tlet);
}
#if 0
static _INLINE_ void receive_chars(struct async_struct *info,
				 int *status, struct pt_regs * regs)
{
	struct tty_struct *tty = info->tty;
	unsigned char ch;
	int ignored = 0;
	struct	async_icount *icount;
	void dump_INTC_regs(void);
	void dump_ide(void);

	icount = &info->state->icount;
	do
	{
		ch = serial_inp(info, UART_RECV_DATA);
 		tty_insert_flip_char(tty,ch,TTY_NORMAL);
		icount->rx++;

#ifdef SERIAL_DEBUG_INTR
/*		printk("DR%02x:%02x...", ch, *status);*/
		sprintf(_str_, "DR%02x:%02x...\r\n", ch, *status);
		dbg_print(_str_);
#endif
		/*
		 * For statistics only
		 */
		if (*status & UART_PE)
			icount->parity++;
		else if (*status & UART_FE)
			icount->frame++;
		if (*status & UART_OVRN)
			icount->overrun++;
		/*
		 * Now check to see if character should be
		 * ignored, and mask off conditions which
		 * should be ignored.
		 */
		if (*status & info->ignore_status_mask)
		{
			if (++ignored > 100)
				break;
#ifdef SERIAL_DEBUG_INTR
	    sprintf(_str_, "ignoring:%c ignore mask: %x\r\n",
			ch, info->ignore_status_mask);
		dbg_print(_str_);
#endif
			goto ignore_char;
		}
		*status &= info->read_status_mask;
#ifdef CONFIG_KGDB
	        if ( info->line == 1 )
		{
			set_debug_traps();
			breakpoint();
        	        goto goout;
	        }

                if (ch == 0xb) /* CTRL-K breakpoint */
                {
			printk("\nkgdb enter> Please Start a Gdb Session To UART-B With 115200");
			set_debug_traps();
                   	breakpoint();
                   	goto goout;
                }
#endif
		   /* Intercept CTRL-W - only when not debugging on 2nd serial port */
                if (tty->index == 0 && ch == 0x17) /* CTRL-W  */
                {
			//dump_softirq();
		     	show_regs(regs);
				// THT 4/05/06: Display stack to debug PR20442
				show_stack(current, NULL);
			printk(" ========== Done Dumping INTC & MIPS registers\n");
             		//printk(" ========== Dumping TLB\n");
	     		//dump_tlb_all();
             		//dump_tlb_wired();
#if defined( CONFIG_BCMINTEMAC ) || defined(CONFIG_BCMINTEMAC_7038)
		     printk(" ========== Dumping EMAC\n");
		     //dump_emac();
#endif

#ifdef CONFIG_IDE
		     printk(" ========== Dumping IDE\n");
		     //dump_ide();
#endif
			 
		     
		     printk(" ========== Done with dump\n");
                   goto goout;

                }
		if (ch == 0x03)	/* break */
		{

#ifdef SERIAL_DEBUG_INTR
/*			printk("handling break....");*/
			sprintf(_str_, "handling break....\r\n");
			dbg_print (_str_);

#endif
			icount->brk++;
			*tty->buf.tail->flag_buf_ptr = TTY_BREAK;
			if (info->flags & ASYNC_SAK)
				do_SAK(tty);
		}
		else if (*status & UART_PE)
			*tty->buf.tail->flag_buf_ptr = TTY_PARITY;
		else if (*status & UART_FE)
			*tty->buf.tail->flag_buf_ptr = TTY_FRAME;

		if (*status & UART_OVRN)
		{
			/*
			 * Overrun is special, since it's
			 * reported immediately, and doesn't
			 * affect the current character
			 */
			//tty->buf.tail->used++;
			//tty->buf.tail->flag_buf_ptr++;
			//tty->buf.tail->char_buf_ptr++;
			*tty->buf.tail->flag_buf_ptr = TTY_OVERRUN;
			if (tty->buf.tail->used >= TTY_FLIPBUF_SIZE)
				goto ignore_char;
		}
		//tty->buf.tail->flag_buf_ptr++;
		//tty->buf.tail->char_buf_ptr++;
		//tty->buf.tail->used++;

		ignore_char:

		*status = serial_inp(info, UART_RECV_STATUS);
	}
	while (*status & UART_RDRF);

#if (LINUX_VERSION_CODE > 131394) /* 2.1.66 */
	tty_flip_buffer_push(tty);
#else
	queue_task_irq_off(&tty->buf.tqueue, &tq_timer);
#endif	
	goout:
		return;
}
#endif //0

static _INLINE_ void transmit_chars(struct async_struct *info, int *intr_done)
{

	serial_out(info, UART_CONTROL, serial_inp(info, UART_CONTROL) | UART_TE );
	
	if (info->x_char) {
		serial_out(info, UART_XMIT_DATA, info->x_char);
		info->state->icount.tx++;
		info->x_char = 0;
		if (intr_done)
			*intr_done = 0;
		return;
	}
	if (info->xmit.head == info->xmit.tail
	    || info->tty->stopped
	    || info->tty->hw_stopped) {
		serial_out(info, UART_XMIT_STATUS, 0);
		return;
	}
	
#ifdef SERIAL_DEBUG_INTR
/*	printk("transmit_chars %x\r\n", info->xmit.buf[info->xmit.tail]);*/
	sprintf(_str_, "transmit_chars %x\r\n", info->xmit.buf[info->xmit.tail]);
			dbg_print (_str_);

#endif	
	serial_out(info, UART_XMIT_DATA, info->xmit.buf[info->xmit.tail]);
	info->xmit.tail = (info->xmit.tail + 1) & (SERIAL_XMIT_SIZE-1);
	info->state->icount.tx++;
	
	if (CIRC_CNT(info->xmit.head,
		     info->xmit.tail,
		     SERIAL_XMIT_SIZE) < WAKEUP_CHARS)
		rs_sched_event(info, RS_EVENT_WRITE_WAKEUP);

#ifdef SERIAL_DEBUG_INTR
/*	printk("THRE...");*/
	sprintf(_str_, "THRE...\r\n");
			dbg_print (_str_);

#endif
	if (intr_done)
		*intr_done = 0;

	if (info->xmit.head == info->xmit.tail) {
		serial_out(info, UART_XMIT_STATUS, 0);
	}
}

/*
 * This is the serial driver's interrupt routine for a single port
 */
static irqreturn_t rs_interrupt_single(int irq, void *dev_id, struct pt_regs * regs)
{
	int status;
	int pass_counter = 0;
	unsigned char ch;
	int           *writeptr;
	struct async_struct * info;
	int           sched_receive = 0;
	
#ifdef SERIAL_DEBUG_INTR
/*	printk("rs_interrupt_single(%d)...", irq);*/
	sprintf(_str_, "rs_interrupt_single(%d)...\r\n", irq);
	dbg_print (_str_);

#endif

	writeptr = &(brcmserial_fifo_writeptr[irq]);
	info = IRQ_ports[irq];
	if (!info || !info->tty)
		return IRQ_NONE;

	do {
		status = serial_inp(info, UART_RECV_STATUS);
#ifdef SERIAL_DEBUG_INTR
/*		printk("rx status = %x...", status);*/
		sprintf(_str_, "rx status = %x...\r\n", status);
		dbg_print (_str_);

#endif
		if (status & (UART_RDRF|UART_OVRN|UART_FE|UART_PE))
		{
			ch = serial_inp(info, UART_RECV_DATA);

#ifdef CONFIG_KGDB
#ifdef CONFIG_SINGLE_SERIAL_PORT_FORCE
			if ( ch == 0x0 ) /* This is serial break signal */
			{
				if ( kgdb_detached ) 
				{
					kgdb_detached = 0;
					set_debug_traps();
				}

				breakpoint();
				goto goout;
			}
#else /* assume serial port is line 1 for kgdb */
			if ( info->line == 1 )
			{
				if ( kgdb_detached ) 
				{
					kgdb_detached = 0;
					set_debug_traps();
				}

				breakpoint();
				goto goout;
			}
#endif /* CONFIG_SINGLE_SERIAL_PORT */
#if 0
			if (ch == 0x12) /* CTRL-R dumps register */
			{
				show_regs(regs);
				goto goout;
			}
#endif
			if (ch == 0xb) /* CTRL-K breakpoint */
			{
				printk("\nkgdb enter> Please Start a Gdb Session To UART-B With 115200");
				if ( kgdb_detached ) 
				{
					kgdb_detached = 0;
					set_debug_traps();
				}

				breakpoint();
				goto goout;

			}
#endif
                   /* Intercept CTRL-W - only when not debugging on 2nd serial port */
#if 0
			if (tty->index == 0 && ch == 0x17) /* CTRL-W  */
#else
			if(0)
#endif
			{
				//dump_softirq();
				show_regs(regs);
				// THT 4/05/06: Display stack to debug PR20442
				show_stack(current, NULL);
				printk(" ========== Done Dumping INTC & MIPS registers\n");
				//printk(" ========== Dumping TLB\n");
				//dump_tlb_all();
				//dump_tlb_wired();
				printk(" ========== Done with dump\n");
				goto goout;

			}

			brcmserial_fifo[irq][*writeptr]      = ch;
                        brcmserial_fifo_stat[irq][*writeptr] = status;
			*writeptr = (*writeptr + 1) & (BRCMSERIAL_FIFO_DEPTH - 1);
                        sched_receive = 1;

		}

		goout:

		status = serial_inp(info, UART_XMIT_STATUS);
		
		if (status & UART_TDRE)
			transmit_chars(info, 0);

#ifdef SERIAL_DEBUG_INTR
/*		printk("tx status = %x...", status);*/
		sprintf(_str_, "tx status = %x...\r\n", status);
			dbg_print (_str_);

#endif
		if (pass_counter++ > RS_ISR_PASS_LIMIT) {
			break;
		}
	} while ((serial_inp(info, UART_RECV_STATUS) & UART_RDRF) ||
		 (serial_inp(info, UART_XMIT_STATUS) & (UART_TDRE|UART_TIE))
			  == (UART_TDRE|UART_TIE));
	info->last_active = jiffies;
	if (sched_receive)
                rs_sched_event(info, BRCMSERIAL_RECEIVE);


#ifdef SERIAL_DEBUG_INTR
/*	printk("end.\n");*/
	sprintf(_str_, "end.\r\n");
	dbg_print (_str_);

#endif

#if 0
	if (irq == BRCM_SERIAL2_IRQ)
		(*(volatile unsigned char *) 0xb500074f) |= UBIRQ;
	else if (irq == BRCM_SERIAL1_IRQ)
		(*(volatile unsigned char *) 0xb500074f) |= UAIRQ;
#endif
	return IRQ_HANDLED;
}


/*
 * -------------------------------------------------------------------
 * Here ends the serial interrupt routines.
 * -------------------------------------------------------------------
 */

static _INLINE_ void sched_receive_chars(struct async_struct *info)
{
        struct tty_struct *tty = info->tty;
        unsigned char ch;
        unsigned char status;
        int ignored = 0;
        struct  async_icount *icount;
        char          *fifo = brcmserial_fifo[info->state->irq];
        char          *fifostat = brcmserial_fifo_stat[info->state->irq];
        int           *readptr  = &(brcmserial_fifo_readptr[info->state->irq]);
        int           writeptr;
        unsigned long flags;
		char flag; //vindent

        local_irq_save(flags);
        writeptr = brcmserial_fifo_writeptr[info->state->irq];
        local_irq_restore(flags);
        icount = &info->state->icount;
        while (*readptr != writeptr)
        {
                ch = fifo[*readptr];
                status = fifostat[*readptr];
				if (tty_buffer_request_room(tty, 1) == 0) goto ignore_char;
                if (tty->buf.tail->used >= TTY_FLIPBUF_SIZE)
                {
#ifdef SERIAL_DEBUG_INTR
                        uart_putc('+');
#endif
			goto ignore_char;  
                }

                icount->rx++;

#ifdef SERIAL_DEBUG_INTR
/*              printk("DR%02x:%02x...", ch, *status);*/
                sprintf(_str_, "DR%02x:%02x...\r\n", ch, status);
                dbg_print(_str_);
#endif

                /*
                 * For statistics only
                 */

                if (ch == 0x03)   /* break   */
                {
                        icount->brk++;
                        if (info->flags & ASYNC_SAK)
                                do_SAK(tty);
                }

                else if (status & UART_PE)
                        icount->parity++;
                else if (status & UART_FE)
                        icount->frame++;
                else if (status & UART_OVRN)
                {
                        icount->overrun++;
/*                      printk("OVRN....\r\n");*/
#ifdef SERIAL_DEBUG_INTR
                        uart_putc('x');
#endif
                }
                /*
                 * Now check to see if character should be
                 * ignored, and mask off conditions which
                 * should be ignored.
                 */
                if (status & info->ignore_status_mask)
                {
                        if (++ignored > 100)
                                break;
#ifdef SERIAL_DEBUG_INTR
			sprintf(_str_, "ignoring:%c ignore mask: %x\r\n",
			              ch, info->ignore_status_mask);
			dbg_print(_str_);
#endif
                        goto ignore_char;
                }
		

                status &= info->read_status_mask;
                flag = TTY_NORMAL;


                if (ch == 0x03) /* break */
                {

#ifdef SERIAL_DEBUG_INTR
/*                      printk("handling break....");*/
                        sprintf(_str_, "handling break....\r\n");
                        dbg_print (_str_);

#endif
                        flag = TTY_BREAK;
                }
                else if (status & UART_PE)
                        flag = TTY_PARITY;
                else if (status & UART_FE)
                        flag = TTY_FRAME;

                if (status & UART_OVRN) 
                {
                        /*
                         * Overrun is special, since it's
                         * reported immediately, and doesn't
                         * affect the current character
                         */
						tty_insert_flip_char(tty, ch, TTY_OVERRUN);
                    	if (tty->buf.tail->used>= TTY_FLIPBUF_SIZE)
							goto ignore_char;
                }
				tty_insert_flip_char(tty, ch, flag);

		ignore_char:

                *readptr = (*readptr + 1) & (BRCMSERIAL_FIFO_DEPTH - 1);
                local_irq_save(flags);
                writeptr = brcmserial_fifo_writeptr[info->state->irq];
                local_irq_restore(flags);
        }

        tty_flip_buffer_push(tty);
/*      sprintf(_str_, "%d\r\n",icount->rx );
        dbg_print(_str_);*/
/*      printk("%d\r\n",icount->rx ); */
#ifdef SERIAL_DEBUG_INTR
        if ((icount->rx % 100) == 0)
        {
                uart_putc('\r');
                uart_putc('\n');
        }
        uart_putc('.');
#endif

}



#if 0
/*
 * This routine is used to handle the "bottom half" processing for the
 * serial driver, known also the "software interrupt" processing.
 * This processing is done at the kernel interrupt level, after the
 * rs_interrupt() has returned, BUT WITH INTERRUPTS TURNED ON.  This
 * is where time-consuming activities which can not be done in the
 * interrupt driver proper are done; the interrupt driver schedules
 * them using rs_sched_event(), and they get done here.
 */
static void do_serial_bh(void)
{
	run_task_queue(&tq_serial);
}
#endif

static void do_softint(unsigned long private_)
{
	struct async_struct	*info = (struct async_struct *) private_;
	struct tty_struct	*tty;
	
	tty = info->tty;
	if (!tty)
		return;

	if (test_and_clear_bit(RS_EVENT_WRITE_WAKEUP, &info->event)) {
		tty_wakeup(tty);
		wake_up_interruptible(&tty->write_wait);
#ifdef SERIAL_HAVE_POLL_WAIT
		wake_up_interruptible(&tty->poll_wait);
#endif
	}
	if (test_and_clear_bit(BRCMSERIAL_RECEIVE, &info->event))
        {
                sched_receive_chars(info);
        }

}

/*
 * This subroutine is called when the RS_TIMER goes off.  It is used
 * by the serial driver to handle ports that do not have an interrupt
 * (irq=0).  This doesn't work very well for 16450's, but gives barely
 * passable results for a 16550A.  (Although at the expense of much
 * CPU overhead).
 */
static void rs_timer(unsigned long dummy)
{
	static unsigned long last_strobe = 0;
	struct async_struct *info;
	unsigned int	i;
	unsigned long flags;

	if ((jiffies - last_strobe) >= RS_STROBE_TIME) {
		for (i=0; i < NR_IRQS; i++) {
			info = IRQ_ports[i];
			if (!info)
				continue;
			local_irq_save(flags);
			rs_interrupt_single(i, NULL, NULL);
			local_irq_restore(flags);
		}
	}
	last_strobe = jiffies;
	mod_timer(&serial_timer, jiffies + RS_STROBE_TIME);

	if (IRQ_ports[0]) {
		unsigned long next;
		local_irq_save(flags);
		rs_interrupt_single(0, NULL, NULL);
		next = jiffies + IRQ_timeout[0] - 2;
		if (next < jiffies + 1)
			next = jiffies + 1;
		mod_timer(&serial_timer, next);
		local_irq_restore(flags);
	}
}

/*
 * ---------------------------------------------------------------
 * Low level utility subroutines for the serial driver:  routines to
 * figure out the appropriate timeout for an interrupt chain, routines
 * to initialize and startup a serial port, and routines to shutdown a
 * serial port.  Useful stuff like that.
 * ---------------------------------------------------------------
 */

/*
 * This routine figures out the correct timeout for a particular IRQ.
 * It uses the smallest timeout of all of the serial ports in a
 * particular interrupt chain.  Now only used for IRQ 0....
 */
static void figure_IRQ_timeout(int irq)
{
	struct	async_struct	*info;
	int	timeout = 60*HZ;	/* 60 seconds === a long time :-) */

	info = IRQ_ports[irq];
	if (!info) {
		IRQ_timeout[irq] = 60*HZ;
		return;
	}
	while (info) {
		if (info->timeout < timeout)
			timeout = info->timeout;
		info = info->next_port;
	}
	if (!irq)
		timeout = timeout / 2;
	IRQ_timeout[irq] = timeout ? timeout : 1;
}

static int startup(struct async_struct * info)
{
	unsigned long flags;
	int	retval=0;
	irqreturn_t(*handler)(int, void *, struct pt_regs *);
	struct serial_state *state= info->state;
	unsigned long page;
	
	page = get_zeroed_page(GFP_KERNEL);
	if (!page)
		return -ENOMEM;

//vincent	local_irq_save(flags);

	if (info->flags & ASYNC_INITIALIZED) {
		free_page(page);
		goto errout;
	}

	if (!CONFIGURED_SERIAL_PORT(state) || !state->type) {
		if (info->tty)
			set_bit(TTY_IO_ERROR, &info->tty->flags);
		free_page(page);
		goto errout;
	}
	if (info->xmit.buf)
		free_page(page);
	else
		info->xmit.buf = (unsigned char *) page;

#ifdef SERIAL_DEBUG_OPEN
/*	printk("starting up ttys%d (irq %d)...", info->line, state->irq);*/
	sprintf(_str_, "starting up ttys%d (irq %d)...\r\n", info->line, state->irq);
			dbg_print (_str_);

#endif

	/*
	 * Clear the FIFO buffers and disable them
	 * (they will be reenabled in change_speed())
	 */
	
	/*
	 * Clear the interrupt registers.
	 */
	serial_out(info, UART_RECV_STATUS, 0x00);       /* disable all intrs */
	serial_out(info, UART_XMIT_STATUS, 0x00);       /* disable all intrs */
	
	/*
	 * Allocate the IRQ if necessary
	 */
	if (state->irq && (!IRQ_ports[state->irq] ||
			  !IRQ_ports[state->irq]->next_port)) {

#if 1
		 /*
		  * THT: 6/16/06: There is a race condition here, that may lead to spurious interrupt.
		  * Here we call request_irq() which starts the Interrupt handler, but we have NOT 
		  * initialized the info struct yet, or linked it into the IRQ_ports chain.  The interrupt
		  * handler, rs_interrupt_single() depends on the chain being initialized prior to the 
		  * interrupt.
		  * As a result, when rs_interrupt_single is called, it causes an endless chain
		  * of interrupts that cannot be handled.
		  * The fix is then to go ahead and initialize the IRQ_port chain before initializing the  
		  * interrupt, but reset it to NULL if the request_irq call fails.
		  */
/* Part 1 of 4 */			  
		struct async_struct* prev = info->prev_port;
		struct async_struct* next = info->next_port;
#endif

		if (IRQ_ports[state->irq]) {      
			retval = -EBUSY;
			goto errout;

		} else 
			handler = rs_interrupt_single;
#if 1
/* Part 2 of 4 */
		/*
		 * Insert serial port into IRQ chain.
		 */
		info->prev_port = 0;
		info->next_port = IRQ_ports[state->irq];
		if (info->next_port)
			info->next_port->prev_port = info;
		IRQ_ports[state->irq] = info;
#endif

		retval = request_irq(state->irq, handler, SA_INTERRUPT,
				     "serial", &IRQ_ports[state->irq]);
		if (retval) {
#if 0
/* Verbose when 2nd Serial port is not handled */
			printk ("request_irq(%d) failed.\r\n", state->irq);
#endif
#if 1
/* Part 3 of 4 */
			/* THT: Restore NULL value on failure */
			IRQ_ports[state->irq] = NULL;
			info->next_port = next;
			info->prev_port = prev;
#endif
			if (capable(CAP_SYS_ADMIN)) {
				if (info->tty)
					set_bit(TTY_IO_ERROR,
						&info->tty->flags);
				retval = 0;
			}
			goto errout;
		}

#if 0		
		INTC->IrqMask |= UPG_IRQ;
		if (info->iomem_base == (void *)BRCM_SERIAL1_BASE)
			UPG_INTC->irqen_l |= UPG_UA_IRQ;
		else if (info->iomem_base == (void *)BRCM_SERIAL2_BASE)
			UPG_INTC->irqen_l |= UPG_UB_IRQ;
		*((volatile unsigned short *)0xb5000050) |= 0x04;
		if (info->iomem_base == (void *)BRCM_SERIAL1_BASE)
			*((volatile unsigned char *)0xb500074f) |= 0x10;
		else if (info->iomem_base == (void *)BRCM_SERIAL2_BASE)
			*((volatile unsigned char *)0xb500074f) |= 0x08;
#endif

/*		printk ("request_irq passed.\r\n");*/
	}

	
#if 0
/* Part 4 of 4, already done in Part 1 */
	/*
	 * Insert serial port into IRQ chain.
	 */
	info->prev_port = 0;
	info->next_port = IRQ_ports[state->irq];
	if (info->next_port)
		info->next_port->prev_port = info;
	IRQ_ports[state->irq] = info;
#endif
	figure_IRQ_timeout(state->irq);

        /*
         * ADTADT FIFO init
         */

        if (!brcmserial_fifo[state->irq])
                brcmserial_fifo[state->irq] =
                     (char *)kmalloc(BRCMSERIAL_FIFO_DEPTH, GFP_KERNEL);
        if (!brcmserial_fifo_stat[state->irq])
                brcmserial_fifo_stat[state->irq] =
                     (char *)kmalloc(BRCMSERIAL_FIFO_DEPTH, GFP_KERNEL);
        brcmserial_fifo_readptr[state->irq] = 0;
        brcmserial_fifo_writeptr[state->irq] = 0;



	/*
	 * Now, initialize the UART 
	 */

	if (info->tty)
		clear_bit(TTY_IO_ERROR, &info->tty->flags);
	info->xmit.head = info->xmit.tail = 0;

	/*
	 * Set up serial timers...
	 */
	mod_timer(&serial_timer, jiffies + 2*HZ/100);

	/*
	 * Set up the tty->alt_speed kludge
	 */
#if (LINUX_VERSION_CODE >= 131394) /* Linux 2.1.66 */
	if (info->tty) {
		if ((info->flags & ASYNC_SPD_MASK) == ASYNC_SPD_HI)
			info->tty->alt_speed = 57600;
		if ((info->flags & ASYNC_SPD_MASK) == ASYNC_SPD_VHI)
			info->tty->alt_speed = 115200;
		if ((info->flags & ASYNC_SPD_MASK) == ASYNC_SPD_SHI)
			info->tty->alt_speed = 230400;
		if ((info->flags & ASYNC_SPD_MASK) == ASYNC_SPD_WARP)
			info->tty->alt_speed = 460800;
	}
#endif
	
	/*
	 * and set the speed of the serial port
	 */
	change_speed(info, 0);

	info->flags |= ASYNC_INITIALIZED;
//vincent	local_irq_restore(flags);
	return 0;
	
errout:
//vincent	local_irq_restore(flags);
	return retval;
}

/*
 * This routine will shutdown a serial port; interrupts are disabled, and
 * DTR is dropped if the hangup on close termio flag is on.
 */
static void shutdown(struct async_struct * info)
{
	unsigned long	flags;
	struct serial_state *state;
	int		retval;
	unsigned char   ctrl;

	if (!(info->flags & ASYNC_INITIALIZED))
		return;

	state = info->state;

#ifdef SERIAL_DEBUG_OPEN
/*	printk("Shutting down serial port %d (irq %d)....", info->line, state->irq);*/
	sprintf(_str_, "Shutting down serial port %d (irq %d)....\r\n", info->line, state->irq);
			dbg_print (_str_);

#endif
	
	local_irq_save(flags); /* Disable interrupts */

	/*
	 * clear delta_msr_wait queue to avoid mem leaks: we may free the irq
	 * here so the queue might never be waken up
	 */
	wake_up_interruptible(&info->delta_msr_wait);
	
	/*
	 * First unlink the serial port from the IRQ chain...
	 */
	if (info->next_port)
		info->next_port->prev_port = info->prev_port;
	if (info->prev_port)
		info->prev_port->next_port = info->next_port;
	else
		IRQ_ports[state->irq] = info->next_port;
	figure_IRQ_timeout(state->irq);
	
	/*
	 * Free the IRQ, if necessary
	 */
	if (state->irq && (!IRQ_ports[state->irq] ||
			  !IRQ_ports[state->irq]->next_port)) {
		if (IRQ_ports[state->irq]) {
			free_irq(state->irq, &IRQ_ports[state->irq]);
			retval = request_irq(state->irq, rs_interrupt_single,
					     SA_INTERRUPT, "serial",
					     &IRQ_ports[state->irq]);
			
			if (retval)
				printk("serial shutdown: request_irq: error %d"
				       "  Couldn't reacquire IRQ.\n", retval);
		} else
		{
#if 0
			// when disabling IRQ, we do NOT want to disable all UPG int!!!
			//INTC->IrqMask &= (~(UPG_IRQ));
			if (info->iomem_base == (void *)BRCM_SERIAL1_BASE)
				UPG_INTC->irqen_l &= (~(UPG_UA_IRQ));
			else if (info->iomem_base == (void *)BRCM_SERIAL2_BASE)
				UPG_INTC->irqen_l &= (~(UPG_UB_IRQ));
			*((volatile unsigned short *)0xb5000050) &= (~0x04);
			if (info->iomem_base == (void *)BRCM_SERIAL1_BASE)
				*((volatile unsigned char *)0xb500074f) &= (~0x10);
			else if (info->iomem_base == (void *)BRCM_SERIAL2_BASE)
				*((volatile unsigned char *)0xb500074f) &= (~0x08);
#endif
			free_irq(state->irq, &IRQ_ports[state->irq]);
		}
	}

	if (info->xmit.buf) {
		unsigned long pg = (unsigned long) info->xmit.buf;
		info->xmit.buf = 0;
		free_page(pg);
	}

        /*
         * reset pointers for  receive FIFO
         */

        brcmserial_fifo_readptr[state->irq] = 0;
        brcmserial_fifo_writeptr[state->irq] = 0;

        /*
         * ADT Disable receiver and transmitter
         */
        ctrl = serial_inp(info, UART_CONTROL);
        ctrl &= ~(UART_RE | UART_TE);
        serial_out(info, UART_CONTROL, ctrl);

        /* flush out the hardware 4-byte receive FIFO */
        while (serial_inp(info, UART_RECV_STATUS) & UART_RDRF)
                ctrl = serial_in(info, UART_RECV_DATA);

	info->IER = 0;
	serial_out(info, UART_RECV_STATUS, 0x00);	/* disable all intrs */
	serial_out(info, UART_XMIT_STATUS, 0x00);	/* disable all intrs */

	(void)serial_in(info, UART_RECV_DATA);    /* read data port to reset things */
	
	if (info->tty)
		set_bit(TTY_IO_ERROR, &info->tty->flags);

	info->flags &= ~ASYNC_INITIALIZED;
	local_irq_restore(flags);
}

/* #if (LINUX_VERSION_CODE < 131394) */ /* Linux 2.1.66 */
#if 0
static int baud_table[] = {
	0, 50, 75, 110, 134, 150, 200, 300,
	600, 1200, 1800, 2400, 4800, 9600, 19200,
	38400, 57600, 115200, 230400, 460800, 0 };

static int tty_get_baud_rate(struct tty_struct *tty)
{
	struct async_struct * info = (struct async_struct *)tty->driver_data;
	unsigned int cflag, i;

	cflag = tty->termios->c_cflag;

	i = cflag & CBAUD;
	if (i & CBAUDEX) {
		i &= ~CBAUDEX;
		if (i < 1 || i > 2) 
			tty->termios->c_cflag &= ~CBAUDEX;
		else
			i += 15;
	}
	if (i == 15) {
		if ((info->flags & ASYNC_SPD_MASK) == ASYNC_SPD_HI)
			i += 1;
		if ((info->flags & ASYNC_SPD_MASK) == ASYNC_SPD_VHI)
			i += 2;
	}
	/* printk ("tty_get_baud_rate: %d\r\n", baud_table[i]);    RYH */
	return baud_table[i];
}
#endif

/*
 * This routine is called to set the UART divisor registers to match
 * the specified baud rate for a serial port.
 */
static void change_speed(struct async_struct *info,
			 struct termios *old_termios)
{
	int	quot = 0, baud_base, baud;
	unsigned cflag, cval;
	int	bits;
	unsigned long	flags;

#ifdef SERIAL_DEBUG_OPEN
	sprintf(_str_, "change_speed info = %p\r\n", info);
			dbg_print (_str_);
#endif
	
	if (!info->tty || !info->tty->termios)
		return;
	cflag = info->tty->termios->c_cflag;
#ifdef SERIAL_DEBUG_OPEN
	sprintf(_str_, "cflag = %x\r\n", cflag);
			dbg_print (_str_);
#endif
	if (!CONFIGURED_SERIAL_PORT(info))
		return;

	/* byte size and parity */
	switch (cflag & CSIZE) {
	      case CS7: cval = 0x06; bits = 9; break;
	      case CS8: cval = 0x16; bits = 10; break;
	      /* Never happens, but GCC is too dumb to figure it out */
	      default:  cval = 0x06; bits = 7; break;
	      }
	if (cflag & PARENB) {
		cval |= UART_PAREN;
		bits++;
	}
	if (!(cflag & PARODD))
		cval |= UART_PODD;

/* Determine divisor based on baud rate */
	baud = tty_get_baud_rate(info->tty);
	if (!baud)
		baud = 115200;	/* B0 transition handled in rs_set_termios */
	baud_base = info->state->baud_base;
	
	quot = baud_base / baud;	/* RYH */
	

	/* If the quotient is zero refuse the change */
	if (!quot && old_termios) {
		info->tty->termios->c_cflag &= ~CBAUD;
		info->tty->termios->c_cflag |= (old_termios->c_cflag & CBAUD);
		baud = tty_get_baud_rate(info->tty);
		if (!baud)
			baud = 115200;
		else if (baud)
			quot = baud_base / baud;
	}

	/* As a last resort, if the quotient is zero, default to 115200 bps */
	if (!quot)
		quot = baud_base / 115200;
	
/* printk("BAUD : %d      QUOT : %d\n", baud, quot);  RYH */

	info->quot = quot;
	info->timeout = ((HZ*bits*quot) / baud_base);
	info->timeout += HZ/50;		/* Add .02 seconds of slop */

	/* CTS flow control flag and modem status interrupts */
	info->flags &= ~ASYNC_CTS_FLOW;
	info->flags &= ~ASYNC_CHECK_CD;
	
	/*
	 * Set up parity check flag
	 */
#define RELEVANT_IFLAG(iflag) (iflag & (IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK))

	info->read_status_mask = UART_OVRN | UART_RDRF;
	if (I_INPCK(info->tty))
		info->read_status_mask |= UART_FE | UART_PE;
	
	/*
	 * Characters to ignore
	 */
#ifdef SERIAL_DEBUG_OPEN
	sprintf(_str_, "Setting ignore mask\r\n");
			dbg_print (_str_);
#endif
	info->ignore_status_mask = 0;
	if (I_IGNPAR(info->tty))
		info->ignore_status_mask |= UART_PE | UART_FE;
	if (I_IGNBRK(info->tty)) {
		/*
		 * If we're ignore parity and break indicators, ignore 
		 * overruns too.  (For real raw support).
		 */
		if (I_IGNPAR(info->tty))
			info->ignore_status_mask |= UART_OVRN;
	}
	/*
	 * !!! ignore all characters if CREAD is not set
	 */


	if ((cflag & CREAD) == 0)
		info->ignore_status_mask |= UART_RDRF;	
	local_irq_save(flags);
	
	serial_out(info, UART_BAUDRATE_LO, (info->quot) & 0xff);	/* LS of divisor */
	serial_out(info, UART_BAUDRATE_HI, (info->quot) >> 8);		/* MS of divisor */
	serial_out(info, UART_CONTROL, cval);
	
#ifdef SERIAL_DEBUG_OPEN
	sprintf(_str_, "Ignore mask: %x\r\n", info->ignore_status_mask);
			dbg_print (_str_);
#endif
	local_irq_restore(flags);
}

static void rs_put_char(struct tty_struct *tty, unsigned char ch)
{
	struct async_struct *info = (struct async_struct *)tty->driver_data;
	unsigned long flags;

	if (!tty || !info->xmit.buf)
		return;
	
#ifdef SERIAL_DEBUG_INTR
/*	printk("rs_put_char %x\r\n", ch);*/
	sprintf(_str_, "rs_put_char %x\r\n", ch);
			dbg_print (_str_);

#endif
	local_irq_save(flags);
	if (CIRC_SPACE(info->xmit.head,
		       info->xmit.tail,
		       SERIAL_XMIT_SIZE) == 0) {
		local_irq_restore(flags);
		return;
	}

	info->xmit.buf[info->xmit.head] = ch;
	info->xmit.head = (info->xmit.head + 1) & (SERIAL_XMIT_SIZE-1);
	serial_out(info, UART_XMIT_STATUS, UART_TIE);
	local_irq_restore(flags);
}

static void rs_flush_chars(struct tty_struct *tty)
{
	struct async_struct *info = (struct async_struct *)tty->driver_data;
				
	if (serial_paranoia_check(info, tty->name, "rs_flush_chars"))
		return;

	if (info->xmit.head == info->xmit.tail
	    || tty->stopped
	    || tty->hw_stopped
	    || !info->xmit.buf)
		return;
}

static int rs_write(struct tty_struct * tty, const unsigned char *buf, int count)
{
	int	c, ret = 0;
	struct async_struct *info = (struct async_struct *)tty->driver_data;
	unsigned long flags;	

	if (serial_paranoia_check(info, tty->name, "rs_write"))
		return 0;

	if (!tty || !info->xmit.buf || !tmp_buf)
		return 0;

	local_save_flags(flags);
	local_irq_disable();

	while (1) {
		c = CIRC_SPACE_TO_END(info->xmit.head,
				      info->xmit.tail,
				      SERIAL_XMIT_SIZE);
		if (count < c)
			c = count;
		if (c <= 0) {
			break;
		}
		memcpy(info->xmit.buf + info->xmit.head, buf, c);
		info->xmit.head = ((info->xmit.head + c) &
				   (SERIAL_XMIT_SIZE-1));
		buf += c;
		count -= c;
		ret += c;
	}

	local_irq_restore(flags);

	if (info->xmit.head != info->xmit.tail
	    && !tty->stopped
	    && !tty->hw_stopped) {

		serial_out(info, UART_XMIT_STATUS, UART_TIE);

	}
	return ret;
}

static int rs_write_room(struct tty_struct *tty)
{
	struct async_struct *info = (struct async_struct *)tty->driver_data;
	return CIRC_SPACE(info->xmit.head, info->xmit.tail, SERIAL_XMIT_SIZE);
}

static int rs_chars_in_buffer(struct tty_struct *tty)
{
	struct async_struct *info = (struct async_struct *)tty->driver_data;
	return CIRC_CNT(info->xmit.head, info->xmit.tail, SERIAL_XMIT_SIZE);
}

static void rs_flush_buffer(struct tty_struct *tty)
{
	struct async_struct *info = (struct async_struct *)tty->driver_data;
	unsigned long flags;

	/* XXX Would the write semaphore do? */
	local_irq_save(flags);
	info->xmit.head = info->xmit.tail = 0;
	local_irq_restore(flags);
	wake_up_interruptible(&tty->write_wait);
	tty_wakeup(tty);
}

/*
 * This function is used to send a high-priority XON/XOFF character to
 * the device
 */
static void rs_send_xchar(struct tty_struct *tty, char ch)
{
	struct async_struct *info = (struct async_struct *)tty->driver_data;

	info->x_char = ch;
	if (ch) {
		/* Make sure transmit interrupts are on */
		serial_out(info, UART_XMIT_STATUS, UART_TIE);
	}
}

/*
 * ------------------------------------------------------------
 * rs_throttle()
 * 
 * This routine is called by the upper-layer tty layer to signal that
 * incoming characters should be throttled.
 * ------------------------------------------------------------
 */
static void rs_throttle(struct tty_struct * tty)
{
	struct async_struct *info = (struct async_struct *)tty->driver_data;
#ifdef SERIAL_DEBUG_THROTTLE
	char	buf[64];
	
	printk("throttle %s: %d....\n", tty_name(tty, buf),
	       tty->ldisc.chars_in_buffer(tty));
#endif

	if (I_IXOFF(tty))
		rs_send_xchar(tty, STOP_CHAR(tty));

	if (tty->termios->c_cflag & CRTSCTS)
		info->MCR &= ~UART_MCR_RTS;
}

static void rs_unthrottle(struct tty_struct * tty)
{
	struct async_struct *info = (struct async_struct *)tty->driver_data;
#ifdef SERIAL_DEBUG_THROTTLE
	char	buf[64];
	
	printk("unthrottle %s: %d....\n", tty_name(tty, buf),
	       tty->ldisc.chars_in_buffer(tty));
#endif

	if (I_IXOFF(tty)) {
		if (info->x_char)
			info->x_char = 0;
		else
			rs_send_xchar(tty, START_CHAR(tty));
	}
}

/*
 * ------------------------------------------------------------
 * rs_ioctl() and friends
 * ------------------------------------------------------------
 */

static int get_serial_info(struct async_struct * info,
			   struct serial_struct * retinfo)
{
	struct serial_struct tmp;
	struct serial_state *state = info->state;
   
	if (!retinfo)
		return -EFAULT;
	memset(&tmp, 0, sizeof(tmp));
	tmp.type = state->type;
	tmp.line = state->line;
	tmp.port = state->port;
	if (HIGH_BITS_OFFSET)
		tmp.port_high = state->port >> HIGH_BITS_OFFSET;
	else
		tmp.port_high = 0;
	tmp.irq = state->irq;
	tmp.flags = state->flags;
	tmp.xmit_fifo_size = state->xmit_fifo_size;
	tmp.baud_base = state->baud_base;
	tmp.close_delay = state->close_delay;
	tmp.closing_wait = state->closing_wait;
	tmp.custom_divisor = state->custom_divisor;
	tmp.hub6 = state->hub6;
	tmp.io_type = state->io_type;
	if (copy_to_user(retinfo,&tmp,sizeof(*retinfo)))
		return -EFAULT;
	return 0;
}

static int set_serial_info(struct async_struct * info,
			   struct serial_struct * new_info)
{
	struct serial_struct new_serial;
 	struct serial_state old_state, *state;
	unsigned int		i,change_irq,change_port;
	int 			retval = 0;
	unsigned long		new_port;

	if (copy_from_user(&new_serial,new_info,sizeof(new_serial)))
		return -EFAULT;
	state = info->state;
	old_state = *state;

	new_port = new_serial.port;
	if (HIGH_BITS_OFFSET)
		new_port += (unsigned long) new_serial.port_high << HIGH_BITS_OFFSET;

	change_irq = new_serial.irq != state->irq;
	change_port = (new_port != ((int) state->port)) ||
		(new_serial.hub6 != state->hub6);
  
	if (!capable(CAP_SYS_ADMIN)) {
		if (change_irq || change_port ||
		    (new_serial.baud_base != state->baud_base) ||
		    (new_serial.type != state->type) ||
		    (new_serial.close_delay != state->close_delay) ||
		    (new_serial.xmit_fifo_size != state->xmit_fifo_size) ||
		    ((new_serial.flags & ~ASYNC_USR_MASK) !=
		     (state->flags & ~ASYNC_USR_MASK)))
			return -EPERM;
		state->flags = ((state->flags & ~ASYNC_USR_MASK) |
			       (new_serial.flags & ASYNC_USR_MASK));
		info->flags = ((info->flags & ~ASYNC_USR_MASK) |
			       (new_serial.flags & ASYNC_USR_MASK));
		state->custom_divisor = new_serial.custom_divisor;
		goto check_and_exit;
	}

	new_serial.irq = irq_canonicalize(new_serial.irq);

	if ((new_serial.irq >= NR_IRQS) || 
	    (new_serial.baud_base < 9600)|| (new_serial.type < PORT_UNKNOWN) ||
	    (new_serial.type > PORT_MAX) || (new_serial.type == PORT_CIRRUS) ||
	    (new_serial.type == PORT_STARTECH)) {
		return -EINVAL;
	}

	if ((new_serial.type != state->type) ||
	    (new_serial.xmit_fifo_size <= 0))
		new_serial.xmit_fifo_size =
			uart_config[new_serial.type].dfl_xmit_fifo_size;

	/* Make sure address is not already in use */
	if (new_serial.type) {
		for (i = 0 ; i < NR_PORTS; i++)
			if ((state != &rs_table[i]) &&
			    (rs_table[i].port == new_port) &&
			    rs_table[i].type)
				return -EADDRINUSE;
	}

	if ((change_port || change_irq) && (state->count > 1))
		return -EBUSY;

	/*
	 * OK, past this point, all the error checking has been done.
	 * At this point, we start making changes.....
	 */

	state->baud_base = new_serial.baud_base;
	state->flags = ((state->flags & ~ASYNC_FLAGS) |
			(new_serial.flags & ASYNC_FLAGS));
	info->flags = ((state->flags & ~ASYNC_INTERNAL_FLAGS) |
		       (info->flags & ASYNC_INTERNAL_FLAGS));
	state->custom_divisor = new_serial.custom_divisor;
	state->close_delay = new_serial.close_delay * HZ/100;
	state->closing_wait = new_serial.closing_wait * HZ/100;
#if (LINUX_VERSION_CODE > 0x20100)
	info->tty->low_latency = (info->flags & ASYNC_LOW_LATENCY) ? 1 : 0;
#endif
	info->xmit_fifo_size = state->xmit_fifo_size =
		new_serial.xmit_fifo_size;

	if ((state->type != PORT_UNKNOWN) && state->port) {
#ifdef CONFIG_SERIAL_RSA
		if (old_state.type == PORT_RSA)
			release_region(state->port + UART_RSA_BASE, 16);
		else
#endif
		release_region(state->port,8);
	}
	state->type = new_serial.type;
	if (change_port || change_irq) {
		/*
		 * We need to shutdown the serial port at the old
		 * port/irq combination.
		 */
		shutdown(info);
		state->irq = new_serial.irq;
		info->port = state->port = new_port;
		info->hub6 = state->hub6 = new_serial.hub6;
		if (info->hub6)
			info->io_type = state->io_type = SERIAL_IO_HUB6;
		else if (info->io_type == SERIAL_IO_HUB6)
			info->io_type = state->io_type = SERIAL_IO_PORT;
	}
	if ((state->type != PORT_UNKNOWN) && state->port) {
		request_region(state->port,8,"serial(set)");
	}

	
check_and_exit:
	if (!state->port || !state->type)
		return 0;
	if (info->flags & ASYNC_INITIALIZED) {
		if (((old_state.flags & ASYNC_SPD_MASK) !=
		     (state->flags & ASYNC_SPD_MASK)) ||
		    (old_state.custom_divisor != state->custom_divisor)) {
#if (LINUX_VERSION_CODE >= 131394) /* Linux 2.1.66 */
			if ((state->flags & ASYNC_SPD_MASK) == ASYNC_SPD_HI)
				info->tty->alt_speed = 57600;
			if ((state->flags & ASYNC_SPD_MASK) == ASYNC_SPD_VHI)
				info->tty->alt_speed = 115200;
			if ((state->flags & ASYNC_SPD_MASK) == ASYNC_SPD_SHI)
				info->tty->alt_speed = 230400;
			if ((state->flags & ASYNC_SPD_MASK) == ASYNC_SPD_WARP)
				info->tty->alt_speed = 460800;
#endif
			change_speed(info, NULL);
		}
	} else
		retval = startup(info);
	return retval;
}

static int get_modem_info(struct async_struct * info, unsigned int *value)
{
	unsigned int result;
	result =  0;

	if (copy_to_user(value, &result, sizeof(int)))
		return -EFAULT;
	return 0;
}

static int set_modem_info(struct async_struct * info, unsigned int cmd,
			  unsigned int *value)
{
	return -ENOSYS;
}

static int do_autoconfig(struct async_struct * info)
{
	int			retval;
	
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;
	
	if (info->state->count > 1)
		return -EBUSY;
	
	shutdown(info);

	autoconfig(info->state);
	
	retval = startup(info);
	if (retval)
		return retval;
	return 0;
}

/*
 * rs_break() --- routine which turns the break handling on or off
 */
#if (LINUX_VERSION_CODE < 131394) /* Linux 2.1.66 */
static void send_break(	struct async_struct * info, int duration)
{
	return;
}
#else
static void rs_break(struct tty_struct *tty, int break_state)
{
	return;
}
#endif

static int rs_ioctl(struct tty_struct *tty, struct file * file,
		    unsigned int cmd, unsigned long arg)
{
	struct async_struct * info = (struct async_struct *)tty->driver_data;
	struct async_icount cnow;	/* kernel counter temps */
	struct serial_icounter_struct icount;
	unsigned long flags;
#if (LINUX_VERSION_CODE < 131394) /* Linux 2.1.66 */
	int retval, tmp;
#endif
	
	if ((cmd != TIOCGSERIAL) && (cmd != TIOCSSERIAL) &&
	    (cmd != TIOCSERCONFIG) && (cmd != TIOCSERGSTRUCT) &&
	    (cmd != TIOCMIWAIT) && (cmd != TIOCGICOUNT)) {
		if (tty->flags & (1 << TTY_IO_ERROR))
		    return -EIO;
	}
	
	switch (cmd) {
#if (LINUX_VERSION_CODE < 131394) /* Linux 2.1.66 */
		case TCSBRK:	/* SVID version: non-zero arg --> no break */
			retval = tty_check_change(tty);
			if (retval)
				return retval;
			tty_wait_until_sent(tty, 0);
			if (signal_pending(current))
				return -EINTR;
			if (!arg) {
				send_break(info, HZ/4);	/* 1/4 second */
				if (signal_pending(current))
					return -EINTR;
			}
			return 0;
		case TCSBRKP:	/* support for POSIX tcsendbreak() */
			retval = tty_check_change(tty);
			if (retval)
				return retval;
			tty_wait_until_sent(tty, 0);
			if (signal_pending(current))
				return -EINTR;
			send_break(info, arg ? arg*(HZ/10) : HZ/4);
			if (signal_pending(current))
				return -EINTR;
			return 0;
		case TIOCGSOFTCAR:
			tmp = C_CLOCAL(tty) ? 1 : 0;
			if (copy_to_user((void *)arg, &tmp, sizeof(int)))
				return -EFAULT;
			return 0;
		case TIOCSSOFTCAR:
			if (copy_from_user(&tmp, (void *)arg, sizeof(int)))
				return -EFAULT;

			tty->termios->c_cflag =
				((tty->termios->c_cflag & ~CLOCAL) |
				 (tmp ? CLOCAL : 0));
			return 0;
#endif
		case TIOCMGET:
			return get_modem_info(info, (unsigned int *) arg);
		case TIOCMBIS:
		case TIOCMBIC:
		case TIOCMSET:
			return set_modem_info(info, cmd, (unsigned int *) arg);
		case TIOCGSERIAL:
			return get_serial_info(info,
					       (struct serial_struct *) arg);
		case TIOCSSERIAL:
			return set_serial_info(info,
					       (struct serial_struct *) arg);
		case TIOCSERCONFIG:
			return do_autoconfig(info);

		case TIOCSERGETLSR: /* Get line status register */
			return -ENODATA;

		case TIOCSERGSTRUCT:
			if (copy_to_user((struct async_struct *) arg,
					 info, sizeof(struct async_struct)))
				return -EFAULT;
			return 0;
				
			/* NOTREACHED */

		/* 
		 * Get counter of input serial line interrupts (DCD,RI,DSR,CTS)
		 * Return: write counters to the user passed counter struct
		 * NB: both 1->0 and 0->1 transitions are counted except for
		 *     RI where only 0->1 is counted.
		 */
		case TIOCGICOUNT:
			local_irq_save(flags);
			cnow = info->state->icount;
			local_irq_restore(flags);
			icount.cts = cnow.cts;
			icount.dsr = cnow.dsr;
			icount.rng = cnow.rng;
			icount.dcd = cnow.dcd;
			icount.rx = cnow.rx;
			icount.tx = cnow.tx;
			icount.frame = cnow.frame;
			icount.overrun = cnow.overrun;
			icount.parity = cnow.parity;
			icount.brk = cnow.brk;
			icount.buf_overrun = cnow.buf_overrun;
			
			if (copy_to_user((void *)arg, &icount, sizeof(icount)))
				return -EFAULT;
			return 0;
		case TIOCSERGWILD:
		case TIOCSERSWILD:
			/* "setserial -W" is called in Debian boot */
			printk ("TIOCSER?WILD ioctl obsolete, ignored.\n");
			return 0;

		default:
			return -ENOIOCTLCMD;
		}
	return 0;
}

static void rs_set_termios(struct tty_struct *tty, struct termios *old_termios)
{
	struct async_struct *info = (struct async_struct *)tty->driver_data;
	
	unsigned int cflag = tty->termios->c_cflag;
	
//printk("$$$$$$Set termios to = %x orig\n", tty->termios->c_cflag);	
//tty->termios->c_cflag |= CREAD;
//printk("$$$$$$Set termios to = %x\n", tty->termios->c_cflag);	
	
	if (   (cflag == old_termios->c_cflag)
	    && (   RELEVANT_IFLAG(tty->termios->c_iflag) 
		== RELEVANT_IFLAG(old_termios->c_iflag)))
	  return;

	change_speed(info, old_termios);
}

/*
 * ------------------------------------------------------------
 * rs_close()
 * 
 * This routine is called when the serial port gets closed.  First, we
 * wait for the last remaining data to be sent.  Then, we unlink its
 * async structure from the interrupt chain if necessary, and we free
 * that IRQ if nothing is left in the chain.
 * ------------------------------------------------------------
 */
static void rs_close(struct tty_struct *tty, struct file * filp)
{
	struct async_struct * info = (struct async_struct *)tty->driver_data;
	struct serial_state *state;
	unsigned long flags;
	state = info->state;

	local_irq_save(flags);

	if (tty_hung_up_p(filp)) {
		local_irq_restore(flags);
		return;
	}
#ifdef SERIAL_DEBUG_OPEN
/*	printk("rs_close ttys%d, count = %d\n", info->line, state->count);*/
	sprintf(_str_, "rs_close ttys%d, count = %d\r\n", info->line, state->count);
			dbg_print (_str_);

#endif

	if ((tty->count == 1) && (state->count != 1)) {
		/*
		 * Uh, oh.  tty->count is 1, which means that the tty
		 * structure will be freed.  state->count should always
		 * be one in these conditions.  If it's greater than
		 * one, we've got real problems, since it means the
		 * serial port won't be shutdown.
		 */
		printk("rs_close: bad serial port count; tty->count is 1, "
		       "state->count is %d\n", state->count);
		state->count = 1;
	}
	if (--state->count < 0) {
		printk("rs_close: bad serial port count for ttys%d: %d\n",
		       info->line, state->count);
		state->count = 0;
	}
	if (state->count) {
		DBG_CNT("before DEC-2");
		local_irq_restore(flags);
		return;
	}
	info->flags |= ASYNC_CLOSING;
	local_irq_restore(flags);
#if 0
	/*
	 * Save the termios structure, since this port may have
	 * separate termios for callout and dialin.
	 */
	if (info->flags & ASYNC_NORMAL_ACTIVE)
		info->state->normal_termios = *tty->termios;
	if (info->flags & ASYNC_CALLOUT_ACTIVE)
		info->state->callout_termios = *tty->termios;
#endif
	/*
	 * Now we wait for the transmit buffer to clear; and we notify 
	 * the line discipline to only process XON/XOFF characters.
	 */
	tty->closing = 1;
	/* if (port->closing_wait != ASYNC_CLOSING_WAIT_NONE)
	   tty_wait_until_sent(tty, port->closing_wait); */

	/*
	 * At this point we stop accepting input.  To do this, we
	 * disable the receive line status interrupts, and tell the
	 * interrupt driver to stop checking the data ready bit in the
	 * line status register.
	 */

	serial_out(info, UART_RECV_STATUS, 0);

	/* close has no way of returning "EINTR", so discard return value */
	if (info->closing_wait != ASYNC_CLOSING_WAIT_NONE)
		rs_wait_until_sent(tty, info->timeout);

	shutdown(info);
	if (tty->driver->flush_buffer)
		tty->driver->flush_buffer(tty);
	tty_ldisc_flush(tty);
	tty->closing = 0;
	info->event = 0;
	info->tty = NULL;
	if (info->blocked_open) {
		if (info->close_delay) {
			msleep_interruptible(jiffies_to_msecs(info->close_delay));
		}
		wake_up_interruptible(&info->open_wait);
	}
	info->flags &= ~(ASYNC_NORMAL_ACTIVE|ASYNC_CLOSING);
	wake_up_interruptible(&info->close_wait);
	local_irq_restore(flags);
}

/*
 * rs_wait_until_sent() --- wait until the transmitter is empty
 */
static void rs_wait_until_sent(struct tty_struct *tty, int timeout)
{
	struct async_struct * info = (struct async_struct *)tty->driver_data;
	unsigned long orig_jiffies, char_time;

	orig_jiffies = jiffies;
	/*
	 * Set the check interval to be 1/5 of the estimated time to
	 * send a single character, and make it at least 1.  The check
	 * interval should also be less than the timeout.
	 * 
	 * Note: we have to use pretty tight timings here to satisfy
	 * the NIST-PCTS.
	 */
	char_time = (info->timeout - HZ/50) / info->xmit_fifo_size;
	char_time = char_time / 5;
	if (char_time == 0)
		char_time = 1;
	if (timeout)
	  char_time = min_t(unsigned long, char_time, timeout);
	/*
	 * If the transmitter hasn't cleared in twice the approximate
	 * amount of time to send the entire FIFO, it probably won't
	 * ever clear.  This assumes the UART isn't doing flow
	 * control, which is currently the case.  Hence, if it ever
	 * takes longer than info->timeout, this is probably due to a
	 * UART bug of some kind.  So, we clamp the timeout parameter at
	 * 2*info->timeout.
	 */
	if (!timeout || timeout > 2*info->timeout)
		timeout = 2*info->timeout;
#ifdef SERIAL_DEBUG_RS_WAIT_UNTIL_SENT
/*	printk("In rs_wait_until_sent(%d) check=%lu...", timeout, char_time);*/
	sprintf(_str_, "In rs_wait_until_sent(%d) check=%lu...\r\n", timeout, char_time);
			dbg_print (_str_);

/*	printk("jiff=%lu...", jiffies);*/
	sprintf(_str_, "jiff=%lu...\r\n", jiffies);
			dbg_print (_str_);

#endif
	while (!(serial_inp(info, UART_XMIT_STATUS) & UART_TDRE)) {
#ifdef SERIAL_DEBUG_RS_WAIT_UNTIL_SENT
/*		printk("jiff=%lu...", jiffies);*/
		sprintf(_str_, "jiff=%lu...\r\n", jiffies);
			dbg_print (_str_);

#endif
		msleep_interruptible(jiffies_to_msecs(char_time));
		if (signal_pending(current))
			break;
		if (timeout && time_after(jiffies, orig_jiffies + timeout))
			break;
	}
	set_current_state(TASK_RUNNING);
#ifdef SERIAL_DEBUG_RS_WAIT_UNTIL_SENT
/*	printk("jiff=%lu...done\n", jiffies);*/
	sprintf(_str_, "jiff=%lu...done\r\n", jiffies);
			dbg_print (_str_);

#endif
}

/*
 * rs_hangup() --- called by tty_hangup() when a hangup is signaled.
 */
static void rs_hangup(struct tty_struct *tty)
{
	struct async_struct * info = (struct async_struct *)tty->driver_data;
	struct serial_state *state = info->state;
	
	state = info->state;
	
	rs_flush_buffer(tty);
	if (info->flags & ASYNC_CLOSING)
		return;
	shutdown(info);
	info->event = 0;
	state->count = 0;
	info->flags &= ~(ASYNC_NORMAL_ACTIVE /*|ASYNC_CALLOUT_ACTIVE*/);
	info->tty = 0;
	wake_up_interruptible(&info->open_wait);
}

/*
 * ------------------------------------------------------------
 * rs_open() and friends
 * ------------------------------------------------------------
 */
static int block_til_ready(struct tty_struct *tty, struct file * filp,
			   struct async_struct *info)
{
	DECLARE_WAITQUEUE(wait, current);
	struct serial_state *state = info->state;
	int		retval;
	int		do_clocal = 0, extra_count = 0;
	unsigned long	flags;

	/*
	 * If the device is in the middle of being closed, then block
	 * until it's done, and then try again.
	 */
	if (tty_hung_up_p(filp) ||
	    (info->flags & ASYNC_CLOSING)) {
		if (info->flags & ASYNC_CLOSING)
			interruptible_sleep_on(&info->close_wait);
#ifdef SERIAL_DO_RESTART
		return ((info->flags & ASYNC_HUP_NOTIFY) ?
			-EAGAIN : -ERESTARTSYS);
#else
		return -EAGAIN;
#endif
	}

#if 0

	/*
	 * If this is a callout device, then just make sure the normal
	 * device isn't being used.
	 */
	if (tty->driver.subtype == SERIAL_TYPE_CALLOUT) {
		if (info->flags & ASYNC_NORMAL_ACTIVE)
			return -EBUSY;
		if ((info->flags & ASYNC_CALLOUT_ACTIVE) &&
		    (info->flags & ASYNC_SESSION_LOCKOUT) &&
		    (info->session != current->session))
		    return -EBUSY;
		if ((info->flags & ASYNC_CALLOUT_ACTIVE) &&
		    (info->flags & ASYNC_PGRP_LOCKOUT) &&
		    (info->pgrp != current->pgrp))
		    return -EBUSY;
		info->flags |= ASYNC_CALLOUT_ACTIVE;
		return 0;
	}
#endif
	
	/*
	 * If non-blocking mode is set, or the port is not enabled,
	 * then make the check up front and then exit.
	 */
	if ((filp->f_flags & O_NONBLOCK) ||
	    (tty->flags & (1 << TTY_IO_ERROR))) {
#if 0
		if (info->flags & ASYNC_CALLOUT_ACTIVE)
			return -EBUSY;
#endif
		info->flags |= ASYNC_NORMAL_ACTIVE;
		return 0;
	}

#if 0
	if (info->flags & ASYNC_CALLOUT_ACTIVE) {
		if (state->normal_termios.c_cflag & CLOCAL)
			do_clocal = 1;
	} else {
		if (tty->termios->c_cflag & CLOCAL)
			do_clocal = 1;
	}
#else
	/* ADT ADT will remove this #if later to include the fix for all platforms */
        if (tty->termios->c_cflag & CLOCAL)
              do_clocal = 1;
#endif
	
	/*
	 * Block waiting for the carrier detect and the line to become
	 * free (i.e., not in use by the callout).  While we are in
	 * this loop, state->count is dropped by one, so that
	 * rs_close() knows when to free things.  We restore it upon
	 * exit, either normal or abnormal.
	 */
	retval = 0;
	add_wait_queue(&info->open_wait, &wait);
#ifdef SERIAL_DEBUG_OPEN
/*	printk("block_til_ready before block: ttys%d, count = %d\n",   state->line, state->count);*/
	sprintf(_str_, "block_til_ready before block: ttys%d, count = %d\r\n",   state->line, state->count);
			dbg_print (_str_);

#endif
	local_irq_save(flags);
	if (!tty_hung_up_p(filp)) {
		extra_count = 1;
		state->count--;
	}
	local_irq_restore(flags);
	info->blocked_open++;
	while (1) {
#if 0
		local_irq_save(flags);
		if (tty->termios->c_cflag & CBAUD)
			rtsdtr_ctrl(SER_DTR|SER_RTS);
		local_irq_restore(flags);
#endif
		set_current_state(TASK_INTERRUPTIBLE);
		if (tty_hung_up_p(filp) ||
		    !(info->flags & ASYNC_INITIALIZED)) {
#ifdef SERIAL_DO_RESTART
			if (info->flags & ASYNC_HUP_NOTIFY)
				retval = -EAGAIN;
			else
				retval = -ERESTARTSYS;	
#else
			retval = -EAGAIN;
#endif
			break;
		}
		if (/*!(info->flags & ASYNC_CALLOUT_ACTIVE) &&*/
		    !(info->flags & ASYNC_CLOSING) &&
		    do_clocal)
			break;
		if (signal_pending(current)) {
			retval = -ERESTARTSYS;
			break;
		}
#ifdef SERIAL_DEBUG_OPEN
/*		printk("block_til_ready blocking: ttys%d, count = %d\n", info->line, state->count);*/
		sprintf(_str_, "block_til_ready blocking: ttys%d, count = %d\r\n", info->line, state->count);
			dbg_print (_str_);

#endif
		schedule();
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&info->open_wait, &wait);
	if (extra_count)
		state->count++;
	info->blocked_open--;
#ifdef SERIAL_DEBUG_OPEN
/*	printk("block_til_ready after blocking: ttys%d, count = %d\n", info->line, state->count);*/
	sprintf(_str_, "block_til_ready after blocking: ttys%d, count = %d\r\n", info->line, state->count);
			dbg_print (_str_);

#endif
	if (retval)
		return retval;
	info->flags |= ASYNC_NORMAL_ACTIVE;
	return 0;
}

static int get_async_struct(int line, struct async_struct **ret_info)
{
	struct async_struct *info;
	struct serial_state *sstate;

	sstate = rs_table + line;
	sstate->count++;
	if (sstate->info) {
		*ret_info = sstate->info;
		return 0;
	}
	info = kmalloc(sizeof(struct async_struct), GFP_KERNEL);
	if (!info) {
		sstate->count--;
		return -ENOMEM;
	}
	memset(info, 0, sizeof(struct async_struct));
#ifdef DECLARE_WAITQUEUE
	init_waitqueue_head(&info->open_wait);
	init_waitqueue_head(&info->close_wait);
	init_waitqueue_head(&info->delta_msr_wait);
#endif
	info->magic = SERIAL_MAGIC;
	info->port = sstate->port;
	info->flags = sstate->flags;
	info->io_type = sstate->io_type;
	info->iomem_base = sstate->iomem_base;
	info->iomem_reg_shift = sstate->iomem_reg_shift;
	info->xmit_fifo_size = sstate->xmit_fifo_size;
	info->line = line;
	//info->tqueue.routine = do_softint;
		//info->tqueue.data = info;
	tasklet_init(&info->tlet, do_softint, (unsigned long)info);
	info->state = sstate;
	if (sstate->info) {
		kfree(info);
		*ret_info = sstate->info;
		return 0;
	}
	*ret_info = sstate->info = info;
	return 0;
}

/*
 * This routine is called whenever a serial port is opened.  It
 * enables interrupts for a serial port, linking in its async structure into
 * the IRQ chain.   It also performs the serial-specific
 * initialization for the tty structure.
 */
static int rs_open(struct tty_struct *tty, struct file * filp)
{
	struct async_struct	*info;
	int 			retval, line;
	unsigned long		page;

	MOD_INC_USE_COUNT;
	line = tty->index;
	if ((line < 0) || (line >= NR_PORTS)) {
		MOD_DEC_USE_COUNT;
		return -ENODEV;
	}
	retval = get_async_struct(line, &info);
	if (retval) {
		MOD_DEC_USE_COUNT;
		return retval;
	}
	tty->driver_data = info;
	info->tty = tty;

#ifdef SERIAL_DEBUG_OPEN
/*	printk("rs_open %s%d, count = %d\n", tty->driver.name, info->line, info->state->count);*/
	sprintf(_str_, "rs_open %s%d, count = %d\r\n", tty->name, info->line, info->state->count);
	dbg_print (_str_);

#endif
#if (LINUX_VERSION_CODE > 0x20100)
	info->tty->low_latency = (info->flags & ASYNC_LOW_LATENCY) ? 1 : 0;
#endif

	if (!tmp_buf) {
		page = get_zeroed_page(GFP_KERNEL);
		if (!page) {
			return -ENOMEM;
		}
		if (tmp_buf)
			free_page(page);
		else
			tmp_buf = (unsigned char *) page;
	}

	/*
	 * If the port is the middle of closing, bail out now
	 */
	if (tty_hung_up_p(filp) ||
	    (info->flags & ASYNC_CLOSING)) {
		if (info->flags & ASYNC_CLOSING)
			interruptible_sleep_on(&info->close_wait);
#ifdef SERIAL_DO_RESTART
		return ((info->flags & ASYNC_HUP_NOTIFY) ?
			-EAGAIN : -ERESTARTSYS);
#else
		return -EAGAIN;
#endif
	}

	/*
	 * Start up serial port
	 */
	retval = startup(info);
	if (retval) {
		return retval;
	}

	retval = block_til_ready(tty, filp, info);
	if (retval) {
#ifdef SERIAL_DEBUG_OPEN
/*		printk("rs_open returning after block_til_ready with %d\n", retval);*/
		sprintf(_str_, "rs_open returning after block_til_ready with %d\r\n", retval);
		dbg_print (_str_);

#endif
		return retval;
	}

#if 0

	if ((info->state->count == 1) &&
	    (info->flags & ASYNC_SPLIT_TERMIOS)) {
		if (tty->driver.subtype == SERIAL_TYPE_NORMAL)
			*tty->termios = info->state->normal_termios;
		else 
			*tty->termios = info->state->callout_termios;
		change_speed(info, 0);
	}
#ifdef CONFIG_SERIAL_CONSOLE
	if (sercons.cflag && sercons.index == line) {
		tty->termios->c_cflag = sercons.cflag;
		sercons.cflag = 0;
		change_speed(info, 0);
	}
#endif
	info->session = current->session;
	info->pgrp = current->pgrp;
#endif

#ifdef SERIAL_DEBUG_OPEN
/*	printk("rs_open ttys%d successful...", info->line);*/
	sprintf(_str_, "rs_open %s successful...\r\n", tty->name);
			dbg_print (_str_);

#endif
	serial_out(info, UART_XMIT_STATUS, UART_TIE);
	serial_out(info, UART_RECV_STATUS, UART_RIE);
	return 0;
}

/*
 * /proc fs routines....
 */

static inline int line_info(char *buf, struct serial_state *state)
{
	struct async_struct *info = state->info, scr_info;
	char	stat_buf[30], status;
	int	ret;
	unsigned long flags;

	ret = sprintf(buf, "%d: uart:%s port:%lX irq:%d",
		      state->line, uart_config[state->type].name, 
		      state->port, state->irq);

	if (!state->port || (state->type == PORT_UNKNOWN)) {
		ret += sprintf(buf+ret, "\r\n");
		return ret;
	}

	/*
	 * Figure out the current RS-232 lines
	 */
	if (!info) {
		info = &scr_info;	/* This is just for serial_{in,out} */

		info->magic = SERIAL_MAGIC;
		info->port = state->port;
		info->flags = state->flags;
		info->quot = 0;
		info->tty = 0;
	}
	local_irq_save(flags);
	status = serial_in(info, UART_MSR);
	local_irq_restore(flags);
	stat_buf[0] = 0;
	stat_buf[1] = 0;
	if (info->quot) {
		ret += sprintf(buf+ret, " baud:%d",
			       state->baud_base / info->quot);
	}

	ret += sprintf(buf+ret, " tx:%d rx:%d",
		      state->icount.tx, state->icount.rx);

	if (state->icount.frame)
		ret += sprintf(buf+ret, " fe:%d", state->icount.frame);
	
	if (state->icount.parity)
		ret += sprintf(buf+ret, " pe:%d", state->icount.parity);
	
	if (state->icount.overrun)
		ret += sprintf(buf+ret, " oe:%d", state->icount.overrun);

	/*
	 * Last thing is the RS-232 status lines
	 */
	ret += sprintf(buf+ret, " %s\r\n", stat_buf+1);
	return ret;
}

int rs_read_proc(char *page, char **start, off_t off, int count,
		 int *eof, void *data)
{
	int i, len = 0, l;
	off_t	begin = 0;

	len += sprintf(page, "serinfo:1.0 driver:%s%s revision:%s\r\n",
		       serial_version, LOCAL_VERSTRING, serial_revdate);
	for (i = 0; i < NR_PORTS && len < 4000; i++) {
		l = line_info(page + len, &rs_table[i]);
		len += l;
		if (len+begin > off+count)
			goto done;
		if (len+begin < off) {
			begin += len;
			len = 0;
		}
	}
	*eof = 1;
done:
	if (off >= len+begin)
		return 0;
	*start = page + (off-begin);
	return ((count < begin+len-off) ? count : begin+len-off);
}

/*
 * ---------------------------------------------------------------------
 * rs_init() and friends
 *
 * rs_init() is called at boot-time to initialize the serial driver.
 * ---------------------------------------------------------------------
 */

/*
 * This routine prints out the appropriate serial driver version
 * number, and identifies which options were configured into this
 * driver.
 */
static char serial_options[] __initdata =
#ifdef CONFIG_SERIAL_SHARE_IRQ
       " SHARE_IRQ"
#define SERIAL_OPT
#endif
#ifdef CONFIG_SERIAL_DETECT_IRQ
       " DETECT_IRQ"
#define SERIAL_OPT
#endif
#ifdef ENABLE_SERIAL_PCI
       " SERIAL_PCI"
#define SERIAL_OPT
#endif
#ifdef ENABLE_SERIAL_PNP
       " ISAPNP"
#define SERIAL_OPT
#endif
#ifdef SERIAL_OPT
       " enabled\n";
#else
       " no serial options enabled\n";
#endif
#undef SERIAL_OPT

static _INLINE_ void show_serial_version(void)
{
 	printk(KERN_INFO "%s version %s%s (%s) with%s", serial_name,
	       serial_version, LOCAL_VERSTRING, serial_revdate,
	       serial_options);
}

/*
 * This routine detect the IRQ of a serial port by clearing OUT2 when
 * no UART interrupt are requested (IER = 0) (*GPL*). This seems to work at
 * each time, as long as no other device permanently request the IRQ.
 * If no IRQ is detected, or multiple IRQ appear, this function returns 0.
 * The variable "state" and the field "state->port" should not be null.
 */
static unsigned detect_uart_irq (struct serial_state * state)
{
	if (state->iomem_base == (void *)BRCM_SERIAL1_BASE)
		return BRCM_SERIAL1_IRQ;
	
	if (state->iomem_base == (void *)BRCM_SERIAL2_BASE)
		return BRCM_SERIAL2_IRQ;
	
	return 0;
}

/*
 * This routine is called by rs_init() to initialize a specific serial
 * port.  It determines what type of UART chip this serial port is
 * using: 8250, 16450, 16550, 16550A.  The important question is
 * whether or not this UART is a 16550A or not, since this will
 * determine whether or not we can use its FIFO features or not.
 */
static void autoconfig(struct serial_state * state)
{
	struct async_struct *info, scr_info;
	unsigned long flags;

#ifdef SERIAL_DEBUG_AUTOCONF
/*	printk("Testing ttyS%d (0x%04lx, 0x%04x)...\n", state->line, state->port, (unsigned) state->iomem_base);*/
	sprintf(_str_, "Testing ttyS%d (0x%04lx, 0x%04x)...\r\n", state->line, state->port, (unsigned) state->iomem_base);
			dbg_print (_str_);

#endif
	
	if (!CONFIGURED_SERIAL_PORT(state))
		return;
	
	local_irq_save(flags);
		
	info = &scr_info;	/* This is just for serial_{in,out} */

	info->magic = SERIAL_MAGIC;
	info->state = state;
	info->port = state->port;
	info->flags = state->flags;
	info->io_type = state->io_type;
	info->iomem_base = state->iomem_base;
	info->iomem_reg_shift = state->iomem_reg_shift;

	state->type = PORT_BCM3250;
	/*
	 * Reset the UART.
	 */
#if 0
	serial_out (info, UART_CONTROL, 0);
	serial_out (info, UART_XMIT_STATUS, 0);
	serial_out (info, UART_RECV_STATUS, 0);
	(void) serial_in (info, UART_RECV_DATA);
#endif
	local_irq_restore(flags);
}

#if !defined( CONFIG_MIPS_BCM7401B0 ) && !defined( CONFIG_MIPS_BCM7402 ) && \
    !defined( CONFIG_MIPS_BCM7401C0 ) && !defined( CONFIG_MIPS_BCM7403A0 ) && \
    !defined( CONFIG_MIPS_BCM7452A0 )
/* Already defined in 8250.c */

int register_serial(struct serial_struct *req);
void unregister_serial(int line);

#if (LINUX_VERSION_CODE > 0x20100)
//EXPORT_SYMBOL(register_serial);
//EXPORT_SYMBOL(unregister_serial);
#else
static struct symbol_table serial_syms = {
#include <linux/symtab_begin.h>
	X(register_serial),
	X(unregister_serial),
#include <linux/symtab_end.h>
};
#endif

int register_serial(struct serial_struct *req)
{
	return -1;
}
void unregister_serial(int line)
{
	return;
}

#endif /* !7401B0 */


static struct tty_operations serial_ops = {
	.open = rs_open,
	.close = rs_close,
	.write = rs_write,
	.put_char = rs_put_char,
	.flush_chars = rs_flush_chars,
	.write_room = rs_write_room,
	.chars_in_buffer = rs_chars_in_buffer,
	.flush_buffer = rs_flush_buffer,
	.ioctl = rs_ioctl,
	.throttle = rs_throttle,
	.unthrottle = rs_unthrottle,
	.set_termios = rs_set_termios,
	.stop = rs_stop,
	.start = rs_start,
	.hangup = rs_hangup,
	.break_ctl = rs_break,
	.send_xchar = rs_send_xchar,
	.wait_until_sent = rs_wait_until_sent,
	.read_proc = rs_read_proc,
	//.tiocmget = rs_tiocmget,
	//.tiocmset = rs_tiocmset,
};
/*
 * The serial driver boot-time initialization code!
 */
static int __init rs_init(void)
{
	int i;
	struct serial_state * state;



#if defined( CONFIG_MIPS_BCM7401B0 ) || defined( CONFIG_MIPS_BCM7402 ) || \
    defined( CONFIG_MIPS_BCM7401C0 ) || defined( CONFIG_MIPS_BCM7403A0 ) || \
    defined( CONFIG_MIPS_BCM7452A0 )
    /* 
     * Currently this module is called before 8250 is called
     * but we want to be deadsure
     */
	static int rs_initiated;

	if (rs_initiated) {
		return 0;
	}
	rs_initiated = 1;
#endif

	serial_driver = alloc_tty_driver(NR_PORTS);
	if (!serial_driver) 
		return -ENOMEM;
	
	if (serial_timer.function) {
		uart_puts("RS_TIMER already set, another serial driver already loaded?\n");
		return -EBUSY;
	}

	//init_bh(SERIAL_BH, do_serial_bh);
	init_timer(&serial_timer);
	serial_timer.function = rs_timer;
	mod_timer(&serial_timer, jiffies + RS_STROBE_TIME);

	for (i = 0; i < NR_IRQS; i++) {
		IRQ_ports[i] = 0;
		IRQ_timeout[i] = 0;
		/* ADTADT */
                brcmserial_fifo[i] = 0;
                brcmserial_fifo_stat[i] = 0;
                brcmserial_fifo_readptr[i] = 0;
                brcmserial_fifo_writeptr[i] = 0;

	}

#ifdef CONFIG_SERIAL_CONSOLE
//#if 1
	/*
	 *	The interrupt of the serial console port
	 *	can't be shared.
	 */
	if (sercons.flags & CON_CONSDEV) {
		for(i = 0; i < NR_PORTS; i++)
			if (i != sercons.index &&
			    rs_table[i].irq == rs_table[sercons.index].irq)
				rs_table[i].irq = 0;
	}
#endif
	show_serial_version();

	/* Initialize the tty_driver structure */

	//memset(&serial_driver, 0, sizeof(struct tty_driver));
	//serial_driver.magic = TTY_DRIVER_MAGIC;
#if (LINUX_VERSION_CODE > 0x20100)
	serial_driver->driver_name ="serial";
#endif
#if (LINUX_VERSION_CODE > 0x2032D && defined(CONFIG_DEVFS_FS))
	serial_driver->name = "tts/%d";
#else
	serial_driver->name = "ttyS";
#endif
	serial_driver->major = TTY_MAJOR;
	serial_driver->minor_start = 64 + SERIAL_DEV_OFFSET;
	serial_driver->num = NR_PORTS;
	serial_driver->type = TTY_DRIVER_TYPE_SERIAL;
	serial_driver->subtype = SERIAL_TYPE_NORMAL;
	serial_driver->init_termios = tty_std_termios;
	serial_driver->init_termios.c_cflag =
		B115200 | CS8 | CREAD | CLOCAL;
	serial_driver->flags = TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV;
	serial_driver->refcount = 0;
	serial_driver->ttys = serial_table;
	serial_driver->termios = serial_termios;
	serial_driver->termios_locked = serial_termios_locked;

#if 0
	serial_driver->open = rs_open;
	serial_driver->close = rs_close;
	serial_driver->write = rs_write;
	serial_driver->put_char = rs_put_char;
	serial_driver->flush_chars = rs_flush_chars;
	serial_driver->write_room = rs_write_room;
	serial_driver->chars_in_buffer = rs_chars_in_buffer;
	serial_driver->flush_buffer = rs_flush_buffer;
	serial_driver->ioctl = rs_ioctl;
	serial_driver->throttle = rs_throttle;
	serial_driver->unthrottle = rs_unthrottle;
	serial_driver->set_termios = rs_set_termios;
	serial_driver->stop = rs_stop;
	serial_driver->start = rs_start;
	serial_driver->hangup = rs_hangup;
#if (LINUX_VERSION_CODE >= 131394) /* Linux 2.1.66 */
	serial_driver->break_ctl = rs_break;
#endif
#if (LINUX_VERSION_CODE >= 131343)
	serial_driver->send_xchar = rs_send_xchar;
	serial_driver->wait_until_sent = rs_wait_until_sent;
	serial_driver->read_proc = rs_read_proc;
#endif
#endif

	tty_set_operations(serial_driver, &serial_ops);

#if 0
	
	/*
	 * The callout device is just like normal device except for
	 * major number and the subtype code.
	 */
	callout_driver = serial_driver;
#if (LINUX_VERSION_CODE > 0x2032D && defined(CONFIG_DEVFS_FS))
	callout_driver.name = "cua/%d";
#else
	callout_driver.name = "cua";
#endif
	callout_driver.major = TTYAUX_MAJOR;
	callout_driver.subtype = SERIAL_TYPE_CALLOUT;
#if (LINUX_VERSION_CODE >= 131343)
	callout_driver.read_proc = 0;
	callout_driver.proc_entry = 0;
#endif
#endif

#ifdef SERIAL_DEBUG_AUTOCONF
/*	printk ("Calling tty_register_driver/serial\r\n");*/
	sprintf( _str_,"Calling tty_register_driver/serial\r\n");
			dbg_print (_str_);

#endif	
	if (tty_register_driver(serial_driver)) {
		//printk(KERN_ERR "Couldn't register serial driver\n");
		uart_puts("Couldn't register serial driver\n");
		return -ENOMEM;
	}

#ifdef SERIAL_DEBUG_AUTOCONF
/*	printk ("Calling tty_register_driver/callout\r\n");*/
	sprintf( _str_, "Calling tty_register_driver/callout\r\n");
			dbg_print (_str_);

#endif	
#if 0
	if (tty_register_driver(&callout_driver))
		panic("Couldn't register callout driver\n");
#endif

	for (i = 0, state = rs_table; i < NR_PORTS; i++,state++) {
		state->magic = SSTATE_MAGIC;
		state->line = i;
		state->type = PORT_UNKNOWN;
		state->custom_divisor = 0;
		state->close_delay = 5*HZ/10;
		state->closing_wait = 30*HZ;
//		state->callout_termios = callout_driver.init_termios;
//		state->normal_termios = serial_driver.init_termios;
		state->icount.cts = state->icount.dsr = 
			state->icount.rng = state->icount.dcd = 0;
		state->icount.rx = state->icount.tx = 0;
		state->icount.frame = state->icount.parity = 0;
		state->icount.overrun = state->icount.brk = 0;
		state->irq = irq_canonicalize(state->irq);
		if (state->hub6)
			state->io_type = SERIAL_IO_HUB6;

		if (state->port && check_mem_region(state->port,8))
			continue;
		
		if (state->flags & ASYNC_BOOT_AUTOCONF)
			autoconfig(state);
	}
	for (i = 0, state = rs_table; i < NR_PORTS; i++,state++) {
		if (state->type == PORT_UNKNOWN)
			continue;
		if (   (state->flags & ASYNC_BOOT_AUTOCONF)
		    && (state->flags & ASYNC_AUTO_IRQ)
		    && (state->port != 0)) {
			state->irq = detect_uart_irq(state);
		}
/*
		printk(KERN_INFO "ttyS%02d%s at 0x%04lx (irq = %d) is a %s\n",
		       state->line + SERIAL_DEV_OFFSET,
		       (state->flags & ASYNC_FOURPORT) ? " FourPort" : "",
		       state->port, state->irq,
		       uart_config[state->type].name);
*/		
	}
	return 0;
}


#if defined( CONFIG_MIPS_BCM7401B0 ) || defined( CONFIG_MIPS_BCM7402 ) || \
    defined( CONFIG_MIPS_BCM7401C0 ) || defined( CONFIG_MIPS_BCM7403A0 ) || \
    defined( CONFIG_MIPS_BCM7452A0 )
/*
 * THT: On BCM7401, where both the bcm3250 style UART and the 16550A style UART
 * are present, we have to make sure that the bcm3250 UARTS are initialized first.
 */
void __init bcm3250_uart_init(void)
{
	rs_init();
}

#endif // BCM7401B0

static void __exit rs_fini(void) 
{
	int e1;
	int i;
	struct async_struct *info = NULL;

	/* printk("Unloading %s: version %s\n", serial_name, serial_version); */
	del_timer_sync(&serial_timer);
	//save_flags(flags); cli();
        //remove_bh(SERIAL_BH);
        tasklet_kill(&info->tlet);
	if ((e1 = tty_unregister_driver(serial_driver)))
		printk("serial: failed to unregister serial driver (%d)\n",
		       e1);
        put_tty_driver(serial_driver);
#if 0
	if ((e2 = tty_unregister_driver(&callout_driver)))
		printk("serial: failed to unregister callout driver (%d)\n", 
		       e2);
#endif
	//restore_flags(flags);

	for (i = 0; i < NR_PORTS; i++) {
		if ((info = rs_table[i].info)) {
			rs_table[i].info = NULL;
			kfree(info);
		}
		if ((rs_table[i].type != PORT_UNKNOWN) && rs_table[i].port) {
				release_mem_region(rs_table[i].port, 8);
		}
	}
	if (tmp_buf) {
		unsigned long pg = (unsigned long) tmp_buf;
		tmp_buf = NULL;
		free_page(pg);
	}
}


module_init(rs_init);
module_exit(rs_fini);



/*
 * ------------------------------------------------------------
 * Serial console driver
 * ------------------------------------------------------------
 */
#ifdef CONFIG_SERIAL_CONSOLE

static struct async_struct async_sercons;

static struct tty_driver* serial_console_device(struct console *c, int *index)
{
	//return MKDEV(TTY_MAJOR, 64 + c->index);
	*index = 0;
	return serial_driver;
}

/*******************************************************************/
/***************** Broadcom Specific Routines **********************/
/*******************************************************************/

static inline void brcm_wait_for_xmitr(struct async_struct *info)
{
	unsigned int tmout = 1000000;

	while (--tmout &&
	       (!(serial_in(info, UART_XMIT_STATUS) & UART_TDRE)));
}

/*
 *	Print a string to the serial port trying not to disturb
 *	any possible real use of the port...
 *
 *	The console_lock must be held when we get here.
 */
static void brcm_serial_console_write(struct console *co, const char *s,
				unsigned count)
{
	static struct async_struct *info = &async_sercons;
	int ier;
	unsigned i;


	/*
	 *	First save the IER then disable the interrupts
	 */
	ier = serial_in(info, UART_XMIT_STATUS);

	serial_out(info, UART_XMIT_STATUS, 0x00);

	/*
	 *	Now, do each character
	 */
	for (i = 0; i < count; i++, s++) {
		brcm_wait_for_xmitr(info);

		/*
		 *	Send the character out.
		 *	If a LF, also do CR...
		 */
		serial_out(info, UART_XMIT_DATA, *s);

		if (*s == 10) {
			brcm_wait_for_xmitr(info);
			serial_out(info, UART_XMIT_DATA, 13);
		}
	}

	/*
	 *	Finally, Wait for transmitter & holding register to empty
	 * 	and restore the IER
	 */

	brcm_wait_for_xmitr(info);

	serial_out(info, UART_XMIT_STATUS, ier & UART_TIE);

}

/*
 *	Receive character from the serial port
 */

#if 0
static int brcm_serial_console_wait_key(struct console *co)
{
	static struct async_struct *info;
	int c;
	int ier;

	info = &async_sercons;

	/*
	 *	First save the IER then disable the interrupts so
	 *	that the real driver for the port does not get the
	 *	character.
	 */
	ier = serial_in(info, UART_RECV_STATUS);
	serial_out(info, UART_RECV_STATUS, 0x00);

	while ((serial_in(info, UART_RECV_STATUS) & UART_RDRF) == 0);
	c = serial_in(info, UART_RECV_DATA);

	/*
	 *	Restore the interrupts
	 */
	serial_out(info, UART_RECV_STATUS, ier & UART_RIE);

	return c;
}
#endif

/*
 *	Setup initial baud/bits/parity. We do two things here:
 *	- construct a cflag setting for the first rs_open()
 *	- initialize the serial port
 *	Return non-zero if we didn't find a serial port.
 */
static int __init brcm_serial_console_setup(struct console *co, char *options)
{
	static struct async_struct *info;
	struct serial_state *state;
	unsigned cval = (UART_TE|UART_RE);
	int	baud = 115200;
	int	bits = 8;
	int	parity = 'n';
	int	cflag = CREAD | CLOCAL;
	int	quot = 0;
	char	*s;

	if (options) {
		baud = simple_strtoul(options, NULL, 10);
		s = options;
		while(*s >= '0' && *s <= '9')
			s++;
		if (*s) parity = *s++;
		if (*s) bits   = *s - '0';
	}

	/*
	 *	Now construct a cflag setting.
	 */
	switch(baud) {
		case 1200:
			cflag |= B1200;
			break;
		case 2400:
			cflag |= B2400;
			break;
		case 4800:
			cflag |= B4800;
			break;
		case 19200:
			cflag |= B19200;
			break;
		case 38400:
			cflag |= B38400;
			break;
		case 57600:
			cflag |= B57600;
			break;
		case 115200:
			cflag |= B115200;
			break;
		case 9600:
		default:
			cflag |= B9600;
			break;
	}

	switch(bits) {
		case 7:
			cflag |= CS7;
			break;
		default:
		case 8:
			cflag |= CS8;
			break;
	}

	switch(parity) {
		case 'o': case 'O':
			cflag |= (PARODD | PARENB);
			break;
		case 'e': case 'E':
			cflag |= PARENB;
			break;
	}
	co->cflag = cflag;

	/*
	 *	Divisor, bytesize and parity
	 */
	state = rs_table + co->index;


	info = &async_sercons;
	info->magic = SERIAL_MAGIC;
	info->state = state;
	info->port = state->port;
	info->flags = state->flags;
	info->io_type = state->io_type;
	info->iomem_base = state->iomem_base;
	info->iomem_reg_shift = state->iomem_reg_shift;
	quot = (state->baud_base / baud);

	if (cflag & CS8)
		cval |= UART_BIT8M;
	if (cflag & PARENB)
		cval |= UART_PAREN;
	if (cflag & PARODD)
		cval |= UART_PODD;

	/*
	 *	Disable UART interrupts, set DTR and RTS high
	 *	and set speed.
	 */

	serial_out(info, UART_BAUDRATE_LO, quot & 0xff);	/* LS of divisor */

	serial_out(info, UART_BAUDRATE_HI, quot >> 8);		/* MS of divisor */

	serial_out(info, UART_CONTROL, cval);

	serial_out(info, UART_RECV_STATUS, UART_RIE);
	serial_out(info, UART_XMIT_STATUS, UART_TIE);

	return 0;
}

static struct console sercons = {
	.name	= "ttyS",
	.write   	= brcm_serial_console_write,
	.device 	= serial_console_device,
	.setup 	= brcm_serial_console_setup,
	.flags 	= CON_PRINTBUFFER,
	.index 	= -1,
	.cflag 	= 0,
};
/**************************************************/
/*********** End Broadcom Specific ****************/
/**************************************************/

#if defined( CONFIG_MIPS_BCM7401B0 ) || defined( CONFIG_MIPS_BCM7402 ) || \
    defined( CONFIG_MIPS_BCM7401C0 ) || defined( CONFIG_MIPS_BCM7403A0 ) || \
    defined( CONFIG_MIPS_BCM7452A0 )
extern int console_initialized;
extern int brcm_console_initialized(void);

#else
static int console_initialized;
int brcm_console_initialized(void)
{
	return console_initialized;
}
EXPORT_SYMBOL(brcm_console_initialized);
#endif

/*
 *	Register console.
 */
static int __init brcm_serial_console_init(void)
{
// printk will do the right thing even with console unitialized.	
printk("################## brcm_serial_console_init, sercon=%p\n", &sercons);
	
	register_console(&sercons);
	// New in 2.6.8: Without this we will get /dev/tty0 instead.
	add_preferred_console(sercons.name, 0, "115200");


#if 0 //defined( CONFIG_MIPS_BCM7401B0 ) && defined(  CONFIG_SERIAL_8250 )
	serial8250_start_console(uart,0);
#endif

	console_initialized = 1;
	return 0;
}
console_initcall(brcm_serial_console_init);
#endif

/*
  Local variables:
  compile-command: "gcc -D__KERNEL__ -I../../include -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer -fno-strict-aliasing -pipe -fno-strength-reduce -march=i586 -DMODULE -DMODVERSIONS -include ../../include/linux/modversions.h   -DEXPORT_SYMTAB -c brcmserial.c"
  End:
*/
