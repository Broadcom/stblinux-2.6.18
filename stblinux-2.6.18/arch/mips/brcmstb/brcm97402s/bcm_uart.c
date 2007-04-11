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
    Simple UART driver for 71xx

    History:
    -------------------------------------------------------------------------
    $Log: bcm71xx_uart.c,v $
    Revision 1.1  2001/09/28 01:33:57  cnovak
    Initial Revision

 ------------------------------------------------------------------------- */


/*
 * On the 7401, UARTB is the default, with UARTC being the 2nd serial port.  UARTA is not used,
 * so we have the following mapping
 *
 *  Hardware			Linux
 *  UARTA				Not used
 *  UARTB				UARTA
 * 	UARTC				UARTB
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

#define UART7401_UARTC_BASE		0xb04001c0
#define stUartB ((volatile Uart7401 * const) UART7401_UARTC_BASE )

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

void 
//PutChar(char c)
uartB_putc(char c)
{
    // Wait for Tx Data Register Empty
    while (! (stUartB->uTxStatus & 0x1));

    stUartB->uTxData = c;
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

char
uartB_getc(void)
{
    char cData = 0;
    unsigned long uStatus = stUartB->uRxStatus;

    if (uStatus & 0x4) {
        cData = stUartB->uRxData;
#if 0
	// Check for Frame & Parity errors
        if (uStatus & (0x10 | 0x20)) {
            cData = 0;
        }
#endif
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
uart_init(unsigned long uClock)
{
    unsigned long uBaudRate;

#if 1
// MUX for UARTC is bit 11:9 (001'b) and bit 20:18 (001'b)
// Do this until CFE initializes it correctly.  UARTB initialization is done by the CFE
#define SUN_TOP_CTRL_PIN_MUX_CTRL_8	(0xb04040a4)
	volatile unsigned long* pSunTopMuxCtrl8 = (volatile unsigned long*) SUN_TOP_CTRL_PIN_MUX_CTRL_8;

	*pSunTopMuxCtrl8 &= 0xffe3f1ff;	// Clear it
	*pSunTopMuxCtrl8 |= 0x00040200;  // Write 001'b and 001'b at 11:9 and 20:18
#endif

    // Make sure clock is ticking
    //INTC->blkEnables = INTC->blkEnables | UART_CLK_EN;

	// Calculate BaudRate register value => PeriphClk / UartBaud / 16
        uBaudRate = uClock / (DFLT_BAUDRATE * 16);
	//uBaudRate++;

	// Set the BAUD rate
	stUart->uBaudRateLo = (uBaudRate & 0xFF);
	stUart->uBaudRateHi = ((uBaudRate >> 8) & 0xFF);
  	stUartB->uBaudRateLo = (uBaudRate & 0xFF);
  	stUartB->uBaudRateHi = ((uBaudRate >> 8) & 0xFF);

	// Enable the UART, 8N1, Tx & Rx enabled
	stUart->uControl = 0x16;
  	stUartB->uControl = 0x16;
}
