/*---------------------------------------------------------------------------

    Copyright (c) 2001-2006 Broadcom Corporation                     /\
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
    Simple UART driver for 71xx (UARTA & B)
    Simple UART driver for 7400 16550V2 style UART (UARTC)

    when	who what
-----	---	----
051011	tht	Original coding
 ------------------------------------------------------------------------- */

#define DFLT_BAUDRATE   115200

/*
 * On the 7401C0, UARTB is the default, with UARTC being the 2nd serial port.  UARTA is not used,
 * so we have the following mapping
 *
 *  Hardware			Linux
 *  UARTA				Not used
 *  UARTB				ttyS0
 * 	UARTC				ttyS1  - 16550A style UART 
 */

typedef struct {
	unsigned long/*char*/	uRxStatus;
	unsigned long/*char*/	uRxData;
	unsigned long/*char*/	UNUSED;
	unsigned long/*char*/	uControl;
	unsigned long/*char*/	uBaudRateHi;
	unsigned long/*char*/	uBaudRateLo;
	unsigned long/*char*/	uTxStatus;
	unsigned long/*char*/	uTxData;
} 	Uart7401;

#define UART7401_UARTB_BASE		0xb04001a0  
#define stUart ((volatile Uart7401 * const) UART7401_UARTB_BASE)

#define DFLT_BAUDRATE   115200

/* --------------------------------------------------------------------------
    Name: PutChar
 Purpose: Send a character to the UART
-------------------------------------------------------------------------- */
void 
//PutChar(char c)
uart_putc(char c)
{

#if 0
    // Wait for Tx Data Register Empty
    while (! (stUart->uTxStatus & 0x1));

    stUart->uTxData = c;
#else
	while (!(*((volatile unsigned long*) 0xb04001b8) & 1));

	*((volatile unsigned long*) 0xb04001bc) = c;
#endif
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


/* --------------------------------------------------------------------------
    Name: GetChar
 Purpose: Get a character from the UART. Non-blocking
-------------------------------------------------------------------------- */
char 
uart_getc(void)
{
    	char cData = 0;
	unsigned long uStatus = stUart->uRxStatus;

    	if (uStatus & 0x4) {
        	cData = stUart->uRxData;

		// Check for Frame & Parity errors
		if (uStatus & (0x10 | 0x20)) {
           	 cData = 0;
        	}
    	}

	return cData;
}



/* --------------------------------------------------------------------------
    Name: bcm71xx_uart_init
 Purpose: Initalize the UARTB abd UARTC
 (Linux knows them as UARTA and UARTB respectively)
-------------------------------------------------------------------------- */
void 
//bcm71xx_uart_init(uint32 uClock)
serial_bcm_init(unsigned long uClock)
{
    unsigned long uBaudRate;

    // Make sure clock is ticking
    //INTC->blkEnables = INTC->blkEnables | UART_CLK_EN;

	// Calculate BaudRate register value => PeriphClk / UartBaud / 16
    uBaudRate = uClock / (DFLT_BAUDRATE * 16);
	//uBaudRate++;

	// Set the BAUD rate
	stUart->uBaudRateLo = (uBaudRate & 0xFF);
	stUart->uBaudRateHi = ((uBaudRate >> 8) & 0xFF);
//	stUartB->uBaudRateLo = (uBaudRate & 0xFF);
//	stUartB->uBaudRateHi = ((uBaudRate >> 8) & 0xFF);

	// Enable the UART, 8N1, Tx & Rx enabled
	stUart->uControl = 0x16;
//	stUartB->uControl = 0x16;
}
