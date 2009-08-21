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

	File: bcmuart.c

    Description: 
    Simple UART driver for 71xx

    History:
    -------------------------------------------------------------------------
    $Log: bcm71xx_uart.c,v $
    Revision 1.1  2001/09/28 01:33:57  cnovak
    Initial Revision

 ------------------------------------------------------------------------- */

 #include <linux/config.h>


typedef struct {
	unsigned long	uRxStatus;
	unsigned long	uRxData;
	unsigned long	UNUSED;
	unsigned long	uControl;
	unsigned long	uBaudRateHi;
	unsigned long	uBaudRateLo;
	unsigned long	uTxStatus;
	unsigned long	uTxData;
} 	Uart7320;

#ifdef CONFIG_MIPS_BCM3560_UARTC
#define UART7320_BASE	0xb04001c0	

#else
#define UART7320_BASE		0xb0400180
#endif

#define stUart ((volatile Uart7320 * const) UART7320_BASE)
#define stUartB ((volatile Uart7320 * const) 0xb04001a0)
#define stUartC ((volatile Uart7320 * const) 0xb04001c0)

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




uartC_putc(char c)
{
    // Wait for Tx Data Register Empty
    while (! (stUartC->uTxStatus & 0x1));

    stUartC->uTxData = c;
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

char
uartC_getc(void)
{
    char cData = 0;
    volatile unsigned long uStatus = stUartC->uRxStatus;

    if (uStatus & 0x4) {
        cData = stUartC->uRxData;

#if 0
        // Check for Frame & Parity errors
        if (uStatus & (0x10 | 0x20)) {
            cData = 0;
        }
#endif
    }
    return cData;
}


void
uartB_init(void)
{
// MUX for UARTB :
//    bits 27:25   Value 0x1  for Tx
//    bits 24:22   Value 0x1  for Rx
// Do this until CFE initializes it correctly

#define SUN_TOP_CTRL_PIN_MUX_CTRL_6     (0xb0404118)

        volatile unsigned long* pSunTopMuxCtrl6 = (volatile unsigned long*) SUN_TOP_CTRL_PIN_MUX_CTRL_6;

        *pSunTopMuxCtrl6 &= 0x3FFFFE07;  // Clear the bits
        *pSunTopMuxCtrl6 |= 0x00000090;  // set (27:25) to 1, (24:22) to 1
}


void
uartC_init(void)
{
// MUX for UARTB :
//    bits 29:27   Value 0x1  for Rx
//    bits 2:0   Value 0x1  for Tx
// Do this until CFE initializes it correctly

#define SUN_TOP_CTRL_PIN_MUX_CTRL_7     (0xb040411C)
        volatile unsigned long* pSunTopMuxCtrl7 = (volatile unsigned long*) SUN_TOP_CTRL_PIN_MUX_CTRL_7;

        *pSunTopMuxCtrl7 &= 0x3FFFF03F;  // Clear the bits
        *pSunTopMuxCtrl7 |= 0x00000480;  // set (2) to 1

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

#if defined( CONFIG_KGDB ) && !defined( CONFIG_SINGLE_SERIAL_PORT )
	(void) uartB_init();
#endif
        (void) uartC_init();

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
        stUartC->uBaudRateLo = (uBaudRate & 0xFF);
        stUartC->uBaudRateHi = ((uBaudRate >> 8) & 0xFF);

	stUart->uControl = 0x16;
	stUartB->uControl = 0x16;
        stUartC->uControl = 0x16;

        uartC_puts(" the 3rd UART port UARTC has been inited ..... \r\n");
}
