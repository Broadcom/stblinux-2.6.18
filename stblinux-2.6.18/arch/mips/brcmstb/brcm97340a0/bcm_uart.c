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
    Simple UART driver for 7400 16550V2 style UART

when	who what
-----	---	----
051011	tht	Original coding
030509  VW  	port to 7340a0
 ------------------------------------------------------------------------- */

//#define DFLT_BAUDRATE   115200



#include <linux/config.h>
#include <linux/types.h>
#include "asm/brcmstb/common/serial.h"
#include <linux/serial.h>
#include <linux/serial_reg.h>
#include <asm/serial.h>
#include <asm/io.h>
#include <linux/module.h>

static int shift = 2;

#define SUN_TOP_CTRL_PIN_MUX_CTRL_17	0xb0404144
#define SUN_TOP_CTRL_PIN_MUX_CTRL_18	0xb0404148

static void
turn_on_mux(int chan)
{
	volatile unsigned long* pvul;
	
	if (chan == 1) { // UartB
		/*
		 * uart_rxdb is bit 03:00 value 0 PIN_MUX_CONTROL_18
		 * uart_txdb is bit 07:04 value 0 PIN_MUX_CONTROL_18
		 */
		 pvul = (volatile unsigned long*) SUN_TOP_CTRL_PIN_MUX_CTRL_18;
		*pvul &= 0xffffff00; // CLear it

//		pvul = (volatile unsigned long*) SUN_TOP_CTRL_PIN_MUX_CTRL_18;
//		*pvul &= 0xfffffffc; // CLear it
	}
}

/*
 * At this point, the serial driver is not initialized yet, but we rely on the
 * bootloader to have initialized UARTA
 */


unsigned long serial_init(int chan, void *ignored)
{
	unsigned long uartBaseAddr = UARTA_ADR_BASE + (0x40 * chan);
	void uartB_puts(const char *s);
#ifdef CONFIG_MIPS_BRCM_IKOS
#define DIVISOR (14)
#else
#define DIVISOR (44)
#endif

	shift = 2;

	/* UARTA has already been initialized by the bootloader */
	if (chan > 0) {

		turn_on_mux(chan);
		
		// Write DLAB, and (8N1) = 0x83
		writel(UART_LCR_DLAB|UART_LCR_WLEN8, (void *)(uartBaseAddr + (UART_LCR << shift)));
		// Write DLL to 0xe
		writel(DIVISOR, (void *)(uartBaseAddr + (UART_DLL << shift)));
		writel(0, (void *)(uartBaseAddr + (UART_DLM << shift)));

		// Clear DLAB
		writel(UART_LCR_WLEN8, (void *)(uartBaseAddr + (UART_LCR << shift)));

		// Disable FIFO
		writel(0, (void *)(uartBaseAddr + (UART_FCR << shift)));

		if (chan == 1) {
			uartB_puts("Done initializing UARTB\n");
		}
	}
	return (uartBaseAddr);
}

void
serial_putc(unsigned long com_port, unsigned char c)
{
	while ((readl((void *)(com_port + (UART_LSR << shift))) & UART_LSR_THRE) == 0)
		;
	writel(c, (void *)com_port);
}

unsigned char
serial_getc(unsigned long com_port)
{
	while ((readl((void *)(com_port + (UART_LSR << shift))) & UART_LSR_DR) == 0)
		;
	return readl((void *)com_port);
}

int
serial_tstc(unsigned long com_port)
{
	return ((readl((void *)(com_port + (UART_LSR << shift))) & UART_LSR_DR) != 0);
}

/* Old interface, for compatibility */

void
uart_test_interrupt(void)
{
	*(volatile unsigned long *)(UARTA_ADR_BASE+(UART_FCR<<shift)) = 1;
	*(volatile unsigned long *)(UARTA_ADR_BASE+(UART_IER<<shift)) = UART_IER_THRI;
}

/* --------------------------------------------------------------------------
    Name: PutChar
 Purpose: Send a character to the UART
-------------------------------------------------------------------------- */
void 
//PutChar(char c)
uart_putc(char c)
{
	//void uartB_putc(char c);
	
	serial_putc(UARTA_ADR_BASE, c);
	//uartB_putc(c);
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

