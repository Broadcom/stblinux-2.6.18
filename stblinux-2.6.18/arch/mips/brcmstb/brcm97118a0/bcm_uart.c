/*---------------------------------------------------------------------------

    Copyright (c) 2001-2005 Broadcom Corporation                     /\
                                                              _     /  \     _
    _____________________________________________________/ \   /    \   / \_
                                                            \_/      \_/  
    
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 as
 published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    File: bcm_uart.c

    Description: 
    Simple UART driver for 7118 16550V2 style UART

when	who what
-----	---	----
051011	tht	Original coding
 ------------------------------------------------------------------------- */

#define DFLT_BAUDRATE   115200

#define SER_DIVISOR(_x, clk)		(((clk) + (_x) * 8) / ((_x) * 16))
#define DIVISOR_TO_BAUD(div, clk)	((clk) / 16 / (div))


#include <linux/config.h>
#include <linux/types.h>
#include "asm/brcmstb/common/serial.h"
#include <linux/serial.h>
#include <linux/serial_reg.h>
#include <asm/serial.h>
#include <asm/io.h>
#include <linux/module.h>

static int shift = 2;

/*
 * At this point, the serial driver is not initialized yet, but we rely on the
 * bootloader to have initialized UARTA
 */

#if 0
/* 
* InitSerial() - initialize UART-A to 115200, 8, N, 1
*/
asm void InitSerial(void)
{
	.set noreorder

	/* t3 contains the UART BAUDWORD */
	li		t3,0x0e
	li	t0, 0xb0400b00
	li	t1,0x83  // DLAB, 8bit
	sw	t1,0x0c(t0)
	sw	t3,0x00(t0)
	sw	zero,0x04(t0)
	li	t1,0x03
	sw	t1,0x0c(t0)
	sw	zero,0x08(t0) //disable FIFO.
	
	jr	ra
	nop
	
	.set reorder
}
#endif

unsigned long serial_init(int chan, void *ignored)
{
#if 0
	struct serial_struct req;

	/* UARTA has already been initialized by the bootloader */
	if (chan > 0) {
		memset(&req, 0, sizeof(struct serial_struct));
		req.port = chan;
		req.iomem_base = UARTA_ADR_BASE + (0x40 * chan);
		req.irq = BRCM_SERIAL1_IRQ + chan;  // FOr now, assume all 3 irqs are consecutive
		req.baud_base = BRCM_BASE_BAUD;
		req.xmit_fifo_size = 32;

		/* How far apart the registers are. */
		req.iomem_reg_shift = shift = 2;  /* Offset by 4 bytes, UART_SDW_LCR=0c offset, UART_LCR=3 */
		
		req.io_type = SERIAL_IO_MEM;
		req.flags = STD_COM_FLAGS;
		req.iomap_base = chan;

		register_serial(&req);

	}
	
#endif

	unsigned long uartBaseAddr = UARTA_ADR_BASE + (0x40 * chan); 
	void uartB_puts(const char *s);

#ifdef CONFIG_MIPS_BRCM_IKOS
  #define DIVISOR (14)
#else
  #define DIVISOR (44)
#endif

	shift = 2;


#if 1 /* Enable UARTB */
if (chan == 1) {
// MUX for UARTB is: RX: ctrl3: bit 29:27 (001'b) and TX: ctrl4: bit 02:00 (001'b)

#define SUN_TOP_CTRL_PIN_MUX_CTRL_3	(0xb04040a0)
	volatile unsigned long* pSunTopMuxCtrl3 = (volatile unsigned long*) SUN_TOP_CTRL_PIN_MUX_CTRL_3;
#define SUN_TOP_CTRL_PIN_MUX_CTRL_4	(0xb04040a4)
	volatile unsigned long* pSunTopMuxCtrl4 = (volatile unsigned long*) SUN_TOP_CTRL_PIN_MUX_CTRL_4;
	
	*pSunTopMuxCtrl3 &= 0xc7ffffff;	// Clear it
	*pSunTopMuxCtrl3 |= 0x08000000;  // Write 001'b at 27:29

	*pSunTopMuxCtrl4 &= 0xfffffff8;	// Clear it
	*pSunTopMuxCtrl4 |= 0x00000001;  // Write 001'b  at 00:02
}
#endif

	/* UARTA has already been initialized by the bootloader */
	if (chan > 0 ) {
		// Write DLAB, and (8N1) = 0x83
		writel(UART_LCR_DLAB|UART_LCR_WLEN8, uartBaseAddr + (UART_LCR << shift));
		// Write DLL to 0xe
		writel(DIVISOR, uartBaseAddr + (UART_DLL << shift));
		writel(0, uartBaseAddr + (UART_DLM << shift));

		// Clear DLAB
		writel(UART_LCR_WLEN8, uartBaseAddr + (UART_LCR << shift));

		// Disable FIFO
		writel(0, uartBaseAddr + (UART_FCR << shift));

		if (chan == 1) {
			uartB_puts("Done initializing UARTB\n");
		}
	}
	return (uartBaseAddr);
}

#if 0

unsigned long 
my_readl(unsigned long addr)
{
	return *((volatile unsigned long*) addr);
}

void
my_writel(unsigned char c, unsigned long addr)
{
	*((volatile unsigned long*) addr) = c;
}

#endif

void
serial_putc(unsigned long com_port, unsigned char c)
{
	while ((readl(com_port + (UART_LSR << shift)) & UART_LSR_THRE) == 0)
		;
	writel(c, com_port);
}

unsigned char
serial_getc(unsigned long com_port)
{
	while ((readl(com_port + (UART_LSR << shift)) & UART_LSR_DR) == 0)
		;
	return readl(com_port);
}

int
serial_tstc(unsigned long com_port)
{
	return ((readl(com_port + (UART_LSR << shift)) & UART_LSR_DR) != 0);
}

/* Old interface, for compatibility */

/* --------------------------------------------------------------------------
    Name: PutChar
 Purpose: Send a character to the UART
-------------------------------------------------------------------------- */
void 
//PutChar(char c)
uart_putc(char c)
{
	serial_putc(UARTA_ADR_BASE, c);
}

void 
//PutChar(char c)
uartB_putc(char c)
{
    serial_putc(UARTB_ADR_BASE, c);
}
/* --------------------------------------------------------------------------
    Name: PutString
 Purpose: Send a string to the UART
-------------------------------------------------------------------------- */
void 
//PutString(const char *s)
uart_puts(const char *s)
{
    while (*s) {
        if (*s == '\n') {
            uart_putc('\r');
        }
    	uart_putc(*s++);
    }
}

void 
//PutString(const char *s)
uartB_puts(const char *s)
{
    while (*s) {
        if (*s == '\n') {
            uartB_putc('\r');
        }
    	uartB_putc(*s++);
    }
}
/* --------------------------------------------------------------------------
    Name: GetChar
 Purpose: Get a character from the UART. Non-blocking
-------------------------------------------------------------------------- */
char 
uart_getc(void)
{
	return serial_getc(UARTA_ADR_BASE);
}

char
uartB_getc(void)
{
	return serial_getc(UARTB_ADR_BASE);
}



/**************************************************/
/*********** End Broadcom Specific ****************/
/**************************************************/
int console_initialized;
int brcm_console_initialized(void)
{
	return console_initialized;
}
EXPORT_SYMBOL(brcm_console_initialized);

/* --------------------------------------------------------------------------
    Name: bcm71xx_uart_init
 Purpose: Initalize the UARTB abd UARTC
 (Linux knows them as UARTA and UARTB respectively)
-------------------------------------------------------------------------- */
void 
uart_init(unsigned long ignored)
{
	serial_init(0, NULL);
	serial_init(1, NULL);		/* Uart B */
	//serial_init(2, NULL);		/* Uart C */
	//console_initialized = 1;
}

