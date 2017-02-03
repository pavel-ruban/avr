/**
	@file	vs1001.c
	@brief	VS1001 interface library
	@author	Jesper Hansen 
	@date	2000
 
	$Id: vs1001.c,v 1.10 2007/06/04 15:12:18 brokentoaster Exp $
 
  Copyright (C) 2000 Jesper Hansen <jesperh@telia.com>.

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
**/

/**
 * Modified on 13/07/2004 for ButterflyMP3 project ( atMega169 )
 * by Nick Lott.
 **/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "types.h"
#include "delay.h"
#include "vs1001.h"
#include "mmc.h" // for SPI routines

//
// VS1001 commands
//
#define VS1001_READ	0x03
#define VS1001_WRITE	0x02

#define write_byte_spi(data) (SpiByte(data))

/*
uint8 write_byte_spi(uint8 data)
{
	outp(data, SPDR);
	loop_until_bit_is_set(SPSR, SPIF);    
	
	return inp(SPDR);
}	

*/

///
/// read one or more word(s) from the VS1001 Control registers
///
void vs1001_read(uint8 address, uint16 count, uint16 *pData)
{
	uint8 i;
	CBI( MP3_PORT, MP3_PIN);	// xCS lo
	CBI( BSYNC_PORT,   BSYNC_PIN ); 	// byte sync lo
	
	write_byte_spi(VS1001_READ);
	write_byte_spi(address);

	while (count--)
	{
		*pData = write_byte_spi(0) << 8;
		*pData++ |= write_byte_spi(0);
	}

	SBI( MP3_PORT, MP3_PIN);	// xCS hi

	//this is absolutely neccessary!
	//delay(5); //wait 5 microseconds after sending data to control port
	for (i=0;i<8;i++)
		asm volatile("nop");

}



///
/// write one or more word(s) to the VS1001 Control registers
///
void vs1001_write(uint8 address, uint16 count, uint16 *pData)
{
	uint8 i;
	CBI( MP3_PORT, MP3_PIN);	// xCS lo
	CBI( BSYNC_PORT,   BSYNC_PIN ); 	// byte sync lo
	
	write_byte_spi(VS1001_WRITE);
	write_byte_spi(address);

	while (count--)
	{
		write_byte_spi((uint8)((*pData) >> 8));
		write_byte_spi((uint8)*pData);
		pData++;
	}
	
	SBI( MP3_PORT, MP3_PIN);	// xCS hi

	//this is absolutely neccessary!
	//delay(5); //wait 5 microseconds after sending data to control port
	for (i=0;i<8;i++)
		asm volatile("nop");
}


/****************************************************************************
**
** MPEG Data Stream
**
****************************************************************************/


/* !!! WARNING !!!
	
ALL data on the spi lines is read as
input to the MPEG Stream by the vs1001 when the BSYNC line is high.

*/

///
/// send a byte to the VS1001 MPEG stream
///
inline void vs1001_send_data(unsigned char b)
{
	char i;
	SBI( BSYNC_PORT,   BSYNC_PIN ); 	// byte sync hi

	//	outp(b, SPDR);			// send data
	SPDR = b;				// send data

	// wait for data to be sent
	loop_until_bit_is_set(SPSR, SPIF); 
	
	CBI( BSYNC_PORT,   BSYNC_PIN ); 	// byte sync lo
	i = SPDR; 				// clear SPIF
	

}


///
/// send a burst of 32 data bytes to the VS1001 MPEG stream
///
inline void vs1001_send_32(unsigned char *p)
{
	int j;

	
	for (j=0;j<32;j++)
	{
		SBI( BSYNC_PORT,   BSYNC_PIN ); 		// byte sync hi
		SPDR = *p++ ;		// send data
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		CBI( BSYNC_PORT,   BSYNC_PIN ); 		// byte sync lo
		
		// wait for data to be sent
		loop_until_bit_is_set(SPSR, SPIF);   
	}
	
	j = SPDR; // clear SPIF
}



/****************************************************************************
**
** Init and helper functions
**
****************************************************************************/


/// setup I/O pins and directions for
/// communicating with the VS1001
void vs1001_init_io(void)
{
	// setup BSYNC
	SBI( BSYNC_DDR , BSYNC_PIN );		// pin is output for BSYNC
	CBI( BSYNC_PORT,   BSYNC_PIN ); 	// output low

	// set the MP3/ChipSelect pin hi
	SBI( MP3_DDR , MP3_PIN); 		// pin output for xCS
	SBI( MP3_PORT, MP3_PIN); 		// output hi (select MP3)

	// set the /Reset pin hi
	SBI( RESET_DDR , RESET_PIN); 		// pin output 
	SBI( RESET_PORT, RESET_PIN); 		// output hi

	// Setup DREQ Pin
	CBI(DREQ_DDR , DREQ_PIN); 		// pin input
	CBI(DREQ_PORT, DREQ_PIN);		// no pullup

}


/// setup the VS1001 chip for decoding
void vs1001_init_chip(void)
{
	//we use a hardware reset, works much better 
	//than software rest, but makes a click noise.

	Delay(3);
	vs1001_reset(HARD_RESET);

	Delay(3);
	vs1001_nulls(32);
	vs1001_reset(SOFT_RESET);

}



/// reset the VS1001
void vs1001_reset(reset_e r)
{

	uint16 buf[2];

	if (r == SOFT_RESET)
	{
		
	//	Delay(200);		// 200 mS
		SPSR = (0<<SPI2X);			//set spi to Fosc/4
		
		// set SW reset bit	
		buf[0] = 0x04;
		vs1001_write(0,1,buf);	// set bit 2

		Delay(2);		// 2 mS

		while( !((DREQ_PORT) & (1<<DREQ_PIN)) ); //wait for DREQ

		// set CLOCKF for 24.576 MHz
		// change to doubler //nick 7/7/04
		buf[0] = 0x9800;
		vs1001_write(SCI_CLOCKF,1,buf);	
		
		// Force clock doubler see pg32 of VS10XX appl.notes
		buf[0] = 0x8008;
		vs1001_write(SCI_INT_FCTLH,1,buf);

		vs1001_nulls(32);
	    
		SPSR = (1<<SPI2X);			//set spi to Fosc/2
	}
	else if (r == HARD_RESET)
	{
		CBI(RESET_PORT, RESET_PIN);	// RESET- lo
		Delay(1);	// 1 mS	    
		SBI(RESET_PORT, RESET_PIN);	// RESET- hi
		Delay(5);	// 5 mS	    
	}
}

///
/// send a number of zero's to the VS1001
///
void vs1001_nulls(unsigned int nNulls)
{
	while (nNulls--)
		vs1001_send_data(0);
}

///
/// Set the VS1001 volume
///
void vs_1001_setvolume(unsigned char left, unsigned char right)
{
	uint16 buf[2];

	buf[0] = (((uint16)left) << 8) | (uint16)right;

	vs1001_write(SCI_VOL, 1, buf);
}


