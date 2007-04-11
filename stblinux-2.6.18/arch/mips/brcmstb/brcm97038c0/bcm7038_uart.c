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

    File: bcm71xx_uart.c

    Description: 
    Simple UART driver for 71xx

    History:
    -------------------------------------------------------------------------
    $Log: bcm71xx_uart.c,v $
    Revision 1.1  2001/09/28 01:33:57  cnovak
    Initial Revision

 ------------------------------------------------------------------------- */


typedef struct {
	unsigned long/*char*/	uRxStatus;
	unsigned long/*char*/	uRxData;
	unsigned long/*char*/	UNUSED;
	unsigned long/*char*/	uControl;
	unsigned long/*char*/	uBaudRateHi;
	unsigned long/*char*/	uBaudRateLo;
	unsigned long/*char*/	uTxStatus;
	unsigned long/*char*/	uTxData;
} 	Uart7320;

#define UART7320_BASE		0xb0400180  /*xBAFE00B0*/
#define stUart ((volatile Uart7320 * const) UART7320_BASE)
#define stUartB ((volatile Uart7320 * const) 0xb04001a0 /*0xBAFE00C0*/)

#define DFLT_BAUDRATE   115200

/* --------------------------------------------------------------------------
    Name: PutChar
 Purpose: Send a character to the UART
-------------------------------------------------------------------------- */
void 
//PutChar(char c)
uart_putc(char c)
{
    // Wait for Tx Data Register Empty
    while (! (stUart->uTxStatus & 0x1));

    stUart->uTxData = c;
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
#if 0
		// Check for Frame & Parity errors
        if (uStatus & (0x10 | 0x20)) {
            cData = 0;
        }
#endif
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
 Purpose: Initalize the UART
-------------------------------------------------------------------------- */
void 
//bcm71xx_uart_init(uint32 uClock)
uart_init(unsigned long uClock)
{
    unsigned long uBaudRate;

#if 1
// MUX for UARTB is bit 23:31 (010'b) and bit 20:18 (010'b)
// Do this until CFE initializes it correctly
#define SUN_TOP_CTRL_PIN_MUX_CTRL_9	(0xb0404094)
	volatile unsigned long* pSunTopMuxCtrl9 = (volatile unsigned long*) SUN_TOP_CTRL_PIN_MUX_CTRL_9;

	*pSunTopMuxCtrl9 &= 0xff03ffff;	// Clear it
	*pSunTopMuxCtrl9 |= 0x00480000;  	// Write 010'b and 010'b at 23:21 and 20:18
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
