/**
	@file	uart.c
	@brief	Yampp Uart library
	@author	Jesper Hansen 
	@date	2000
 
	$Id: uart.c,v 1.10 2007/06/04 15:12:17 brokentoaster Exp $
 
  Jesper Hansen <jesperh@telia.com>
  
  Original Author: Volker Oth <volkeroth@gmx.de>
  
  This file is part of the yampp system.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation, 
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


// Modified 2004,2005,2006 for butterflymp3 by Nick Lott
**/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>
#include "uart.h"
#include "main.h"

/* UART global variables */
volatile u08   UART_Ready;
volatile u08   UART_ReceivedChar;
volatile u08   UART_RxChar;
         u08*  pUART_Buffer;

/* end-of-line string = 'Line End' + 'Line Feed' character */
//prog_char UART_pszEndOfLine[3] = {0x0d,0x0a,0};

/* UART Receive Complete Interrupt Function */
SIGNAL(SIG_USART_RECV)      
{
    /* Indicate that the UART has received a character */
    UART_ReceivedChar = 1;
    /* Store received character */
    UART_RxChar = UDR;
}

void UART_SendByte(u08 Data)
{   
	  /* wait for UART to become available */
	while ( (UCSRA & (1<<UDRE)) != (1<<UDRE) ) ;

    /* Send character */
   UDR = Data;
}

u08 UART_ReceiveByte(void)
{
    /* wait for UART to indicate that a character has been received */
   // while(!UART_ReceivedChar);
	
    UART_ReceivedChar = 0;
    /* read byte from UART data buffer */
    return UART_RxChar;
}

void UART_PrintfProgStr(const s08* pBuf)
{ 
#ifdef SERIAL_ENABLE
	unsigned char b;

    pUART_Buffer = (u08 *) pBuf;
	
	do {
		b = pgm_read_byte(pUART_Buffer);
		pUART_Buffer++;
		if (b) 
			UART_SendByte(b);    
	} while (b);
#endif
}

void UART_PrintfEndOfLine(void)
{
	#ifdef SERIAL_ENABLE
	UART_SendByte(0x0d);
	UART_SendByte(0x0a);
	#endif	
}

void UART_PrintfU4(u08 Data)
{
	#ifdef SERIAL_ENABLE
    /* Send 4-bit hex value */
    u08 Character = Data&0x0f;
    if (Character>9)
    {
        Character+='A'-10;
    }
    else
    {
        Character+='0';
    }
    UART_SendByte(Character);
	#endif
}

void UART_Printfu08(u08 Data)
{
    /* Send 8-bit hex value */
    UART_PrintfU4(Data>>4);
    UART_PrintfU4(Data);
}

void UART_Printfu16(u16 Data)
{
    /* Send 16-bit hex value */
    UART_Printfu08(Data>>8);
    UART_Printfu08(Data);
}

void UART_Printfu32(u32 Data)
{
    /* Send 32-bit hex value */
    UART_Printfu16(Data>>16);
    UART_Printfu16(Data);
}

void UART_Init(void)
{
    UART_Ready        = 1;
    UART_ReceivedChar = 0;
    pUART_Buffer      = 0;

    // Enable 2x speed
    UCSRA = (1<<U2X);

    // Enable receiver and transmitter
    UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(0<<TXCIE);

    // Async. mode, 8N1
    UCSRC = (0<<UMSEL)|(0<<UPM0)|(0<<USBS)|(3<<UCSZ0)|(0<<UCPOL);
	
    /* set baud rate */
    UBRRL = UART_BAUD_SELECT;  


}

void UART_Shutdown(void)
{
	// disable 2x speed
    UCSRA = 0;
	
    // disable receiver and transmitter
    UCSRB =0;
	
    // disable Async. mode, 8N1
    UCSRC = 0;
}

unsigned char UART_HasChar(void)
{
	return UART_ReceivedChar;
}


void UART_Puts(u08* pBuf)
{
	while (*pBuf)
	{
		if (*pBuf == '\n')
			UART_SendByte('\r'); // for stupid terminal program
		UART_SendByte(*pBuf++);
	}
}

void UART_Putsln(u08* pBuf)
{
	UART_Puts(pBuf);
	UART_PrintfEndOfLine();
}

