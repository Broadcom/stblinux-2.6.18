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
 ------------------------------------------------------------------------- */

#define DFLT_BAUDRATE   115200



#include <linux/config.h>
#include <linux/types.h>
#include "asm/brcmstb/common/serial.h"
#include <linux/serial.h>
#include <linux/serial_reg.h>
#include <asm/serial.h>
#include <asm/io.h>
#include <linux/module.h>

static int shift = 2;

#define SUN_TOP_CTRL_PIN_MUX_CTRL_10	(0xb04040bc)
#define SUN_TOP_CTRL_PIN_MUX_CTRL_11	(0xb04040c0)
#define SUN_TOP_CTRL_PIN_MUX_CTRL_7		(0xb04040b0)

void uartC_puts(const char *s);
/* 
 * UART IP pin assignments from the 7403A0 RDB SUN_TOP_CTRL_PIN_MUX_CTRL.
 *  bits field is 3 bits wide
 *
 *      UART    GPIO    MUX     bits    value
 *      ______________________________________
 *      0 tx    49      11      02:00   1
 *      0 rx    50      11      05:03   1
 *      ______________________________________
 *      1 tx     9       7      02:00   1
 *      1 rx    10       7      05:03   1
 *      ______________________________________
 *      2 tx    42      10      11:09   1
 *      2 rx    39      10      02:00   1
 *      2 rtsb  41      10      08:06   3
 *      2 ctsb  40      10      05:03   3
 *      ______________________________________
 *
 */

static void handle_gpio_mux(int uartNo)
{
	volatile unsigned long* pSunTopMuxCtrl11; 
	volatile unsigned long* pSunTopMuxCtrl10;

	switch(uartNo) {
	case 0:
//UART_B		volatile unsigned long* pSunTopMuxCtrl7 = (volatile unsigned long*) SUN_TOP_CTRL_PIN_MUX_CTRL_7;
//UART_B		*pSunTopMuxCtrl7 &= 0xffffffc0;	// Clear it
//UART_B		*pSunTopMuxCtrl7 |= 0x00000009;  // Write 001'b and 001'b at 11:9 and 20:18
		break; //CFE already done this

	case 1:
		// MUX for UARTA 
		pSunTopMuxCtrl11 = (volatile unsigned long*) SUN_TOP_CTRL_PIN_MUX_CTRL_11;

		*pSunTopMuxCtrl11 &= 0xffffffc0;	// Clear it UART_0
		*pSunTopMuxCtrl11 |= 0x00000009;  // Write 001'b and 001'b at 11:9 and 20:18

		break;
	case 2:
		// MUX for UARTC are uart_txd_2: bits 18:20 (001'b),  uart_rxd_2 bits 09:11 (001'b)
		// uart_rtsb_2 bits 15-17 (011'b) and uart_ctsb_2 bits 12-14 (011'b) not MUX'ed
		// from SUN_TOP_CTRL_PIN_MUX_CTRL_10
		pSunTopMuxCtrl10 = (volatile unsigned long*) SUN_TOP_CTRL_PIN_MUX_CTRL_10;
		*pSunTopMuxCtrl10 &= 0xfffff000;	// Clear it
		*pSunTopMuxCtrl10 |= 0x000002d9; 	// and 011'b and 011'b at 15:17 & 12:14
		break;
	default:
		break;
	}
		return;
}

static unsigned long serial_8250_init(int chan, void *ignored)
{


	unsigned long uartBaseAddr = UARTC_ADR_BASE;
	void uart_puts(const char *s);
	void uartB_puts(const char *s);
	char msg[40];

	shift = 2;

	/* UARTA has already been initialized by the bootloader */
	if (chan == 1) {
		handle_gpio_mux(2); // ttyS1 is mapped to UARTC
		// Write DLAB, and (8N1) = 0x83
		writel(UART_LCR_DLAB|UART_LCR_WLEN8, uartBaseAddr + (UART_LCR << shift));
		// Write DLL to 0xe
		writel(SERIAL_DIVISOR_LSB, uartBaseAddr + (UART_DLL << shift));
		writel(SERIAL_DIVISOR_MSB, uartBaseAddr + (UART_DLM << shift));

		// Clear DLAB
		writel(UART_LCR_WLEN8, uartBaseAddr + (UART_LCR << shift));

		// Disable FIFO
		writel(0, uartBaseAddr + (UART_FCR << shift));

		sprintf(msg, "Done initializing UARTC at %08x\n", (u32) uartBaseAddr);
		uart_puts(msg);
		uartC_puts(msg);
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
uartA_putc(char c)
{
	void uartA_putc(char c);
	
	while (!(*((volatile unsigned long*) 0xb0400198) & 1));

	*((volatile unsigned long*) 0xb040019c) = c;

}


/* --------------------------------------------------------------------------
    Name: PutString
 Purpose: Send a string to the UART
-------------------------------------------------------------------------- */

void 
//PutString(const char *s)
uartA_puts(const char *s)
{
    while (*s) {
        if (*s == '\n') {
            uartA_putc('\r');
        }
    	uartA_putc(*s++);
    }
}
/* --------------------------------------------------------------------------
    Name: GetChar
 Purpose: Get a character from the UART. Non-blocking
-------------------------------------------------------------------------- */

char
uartA_getc(void)
{
    	char cData = 0;
	unsigned long uStatus = *((volatile unsigned long*) 0xb0400180);

    	if (uStatus & 0x4) {
        	cData = *((volatile unsigned long*) 0xb0400184);

		// Check for Frame & Parity errors
		if (uStatus & (0x10 | 0x20)) {
           	 cData = 0;
        	}
    	}

	return cData;

}
void 
//PutChar(char c)
uartC_putc(char c)
{
	//void uartB_putc(char c);
	
	serial_putc(UARTC_ADR_BASE, c);
	//uartB_putc(c);
}


/* --------------------------------------------------------------------------
    Name: PutString
 Purpose: Send a string to the UART
-------------------------------------------------------------------------- */

void 
//PutString(const char *s)
uartC_puts(const char *s)
{
    while (*s) {
        if (*s == '\n') {
            uartC_putc('\r');
        }
    	uartC_putc(*s++);
    }
}
/* --------------------------------------------------------------------------
    Name: GetChar
 Purpose: Get a character from the UART. Non-blocking
-------------------------------------------------------------------------- */

char
uartC_getc(void)
{
	return serial_getc(UARTC_ADR_BASE);
}



void uartB_putc(char c)
{
	
	uartA_putc(c);
}

char uartB_getc(void)
{
	return uartA_getc();
}


#if 1 /* Already defined elsewhere with the old bcm3250 serial driver */

/**************************************************/
/*********** End Broadcom Specific ****************/
/**************************************************/
int console_initialized;
int brcm_console_initialized(void)
{
	return console_initialized;
}
EXPORT_SYMBOL(brcm_console_initialized);
#endif

/* --------------------------------------------------------------------------
    Name: bcm71xx_uart_init
 Purpose: Initalize the UARTB and UARTC
 (Linux knows them as UARTA/ttyS0 and UARTB/ttyS1 respectively)
-------------------------------------------------------------------------- */
void 
uart_init(unsigned long ignored)
{
//	handle_gpio_mux(0); // ttyS1 is mapped to UARTC
	handle_gpio_mux(1); // ttyS1 is mapped to UARTC
	serial_bcm_init(0, 27000000);		/* Uart B */
	serial_bcm_init(1, 27000000);		/* Uart A */
#ifdef CONFIG_SERIAL_8250
	serial_8250_init(1, NULL);				/* Uart C */
#endif
}

