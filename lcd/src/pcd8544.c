/**
 * PCD8544 Driver 
 *
 *	@file		pcd8544.h
 *	@brief		AVR-GCC Driver for the PCD8544 LCD Controller
 *	@author		Muhammad J. A. Galadima
 *	@date		2004 / 01 / 27
 *
 *	<pre>
 *	Revised		: 2004 / 02 / 05
 *	Version		: 0.7
 *	Target MCU	: Atmel AVRs w/1k+ RAM
 *	
 *	Modified : September 2004 by Nick Lott for AVR Butterfly.
 *	</pre>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version 2
 *	of the License, or (at your option) any later version.
 *	
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software Foundation, 
 *	Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *	
 *	$Id: pcd8544.c,v 1.10 2007/06/04 15:12:18 brokentoaster Exp $
 ************************************************************************
 **/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "types.h"
#include "pcd8544.h"


//extern 	uint8 frameBuffer[fbCols][fbRows];	
extern		uint8 fbXY[];			

/*** lcdByte ******************************************************************
 *	
 *	Send one byte to the LCD
 *	
 *	shift out given byte, MSB fisrt; dc is 1 (data) or 0 (command) 
 *	(lcdModeData/lcdModeCmd) and is set in calling function
 *	
 **************************************************************************///{
void lcdByte(uint8 data) {	// 
	uint8 i;
	
	lcdResume;
	
	for(i=0; i<8; i++) {		// clock out the byte, MSB first
		if(bit_is_set(data, 7))
			lcdPort |= (unsigned char) _BV(SDIN);
		else
			lcdPort &= (unsigned char)~_BV(SDIN);
		
		// clock out the bit
		lcdPort &= (unsigned char) ~_BV(SCLK);
		lcdPort |= (unsigned char) _BV(SCLK);

		data <<= 1;
	}
	
	lcdSuspend;
}
//}
//	void lcdByte(uint8 data)

/*** lcdDot *******************************************************************
 *	
 *	Draw a dot to the frame buffer
 *	
 **************************************************************************///{
void lcdDot(uint8 x, uint8 yLine, uint8 color) {
	uint8 yBit;			// 00LL LBBB
	
	// f x is more than screen x-res, wrap
	while(x>=fbCols)
		x		-= fbCols;
	// if yLine is more than screen y-res, wrap
	while(yLine>=(fbRows*8))
		yLine	-= (fbRows*8);
	
	
	// separate line number (0-5) from atual pixel in line (0-7)
	//	this is done by divide
	yBit	= yLine & 7;	// 0b0000 0bbb
	yLine	= yLine >> 3;	// yline / 8 ; 0b00l ll00 => 0b0000 0lll

// if color is diff from frameBuffer, toggle; if color is toggle (2) expression is always true
//	if( color ^ (frameBuffer[x][yLine] & _BV(yBit))>>yBit ) {
//		frameBuffer[x][yLine] ^= _BV(yBit);
//	}
}
//}
//	void lcdDot(uint8 x, uint8 yLine, uint8 color)

/*** lcdCh *******************************************************************
 *	
 *	Write predefined fonts to screen at set location
 *	
 **************************************************************************///{
void lcdCh(uint8* fontSet, uint8 ch) {
	uint8 i;
	uint8 k = pgm_read_byte(fontSet);
	uint8 x = pgm_read_byte(fontSet+1);
	//uint8 y = pgm_read_byte(fontSet+2);

	lcdModeData;
	
	if(ch < k) {
		
		
		for(i=0; i<x; i++) {
			lcdByte(pgm_read_byte(fontSet+(x*ch+3)+i));
		//	lcdByte(0);
		/*
			for(j=0; j<y; j++) {
				k=0;
				if(pgm_read_byte(fontSet+(x*ch+3)+i) & _BV(j)) {
					k=1;
				}
				
				lcdDot(fbXY[0],fbXY[1]+j,k);
			}
		*/	fbXY[0]++;
		}
	}
	else {
		//for(j=0; j<y; j++) {
		//	lcdDot(fbXY[0],fbXY[1]+j,0);		// show empty if out of range
		//}

		for(i=0; i<x; i++) {
		   lcdByte(0);
		   fbXY[0]++;
		}
	}
}
//}
//	void lcdCh(uint8* fontSet, uint8 ch)

/*** lcdWrite *****************************************************************
 *	
 *	Translate input to selected font, Write to display at set location
 *	
 **************************************************************************///{
void lcdWrite(uint8* fontSet, uint8* buf, uint8 is_pstr, uint8 len) {
	uint8 i=0, offset, ch;

	// check if string is in flash/sram and read appropriately
	if(is_pstr) {
		ch = pgm_read_byte(buf+i);
	}
	else {
		ch = buf[i];
	}

	lcdXY(fbXY[0],fbXY[1]);
	do {
		if(ch > 32) {
			offset = 33;
			
			if( (ch>96) && (ch<123) ) {
				#ifdef LOwERCASE_STRINGS
				offset = 33;
				#else
				offset = 65;
				#endif
			}
			else if(ch>122) {
				offset = 60;
			}
	
			lcdCh(fontSet, (ch-offset));
		}
		else {
			lcdCh(fontSet, 255);	// empty (not recognized)
		}
		fbXY[0]++;	// char spacing
		lcdByte(0);
		i++;
		
		// check if string is in flash/sram and read appropriately
		if(is_pstr) {
			ch = pgm_read_byte(buf+i);
		}
		else {
			ch = buf[i];
		}

	} while((ch != '\0') && (i<=len));
   
}
//}
//	void lcdWrite(uint8* fontSet, uint8* buf, uint8 is_pstr)

/*** fbClr ********************************************************************
 *	
 *	Clear the frame buffer
 *	
 **************************************************************************///{
void fbClr(void) {
	/*	
	uint8 x, yLine;

	for(yLine=0; yLine<fbRows; yLine++) {
		for(x=0; x<fbCols; x++) {
			frameBuffer[x][yLine] = 0x00;
		}
	}*/
	lcdClr();
}
//}
//	void fbClr(void)

/*** lcdClr *******************************************************************
 *	
 *	Clears the LCD. Call this first to get rid of junk data on the screen.
 *	
 **************************************************************************///{
void lcdClr(void) {
	uint16 i;

	lcdModeData;
	for(i=0; i<504; i++) {
		lcdByte(0x00);
	}
}

/*** lcdClrLine *******************************************************************
 *	
 *	Clears an 8x84 line on the LCD. 
 *	
 **************************************************************************///{
void lcdClrLine(uint8 lineNo) {
	uint16 i;

   lcdXY(0,lineNo);

	lcdModeData;
	for(i=0; i<84; i++) {
		lcdByte(0x00);
	}
}

//}
//	void lcdClr(void)

/*** lcdInit ******************************************************************
 *	
 *	Initialize the LCD, get it ready to be used; returns in cmd mode
 *	
 **************************************************************************///{
void lcdInit(void) {
	// set the direction on these pins to output
	lcdPortD |= (unsigned char)(_BV(SCLK) | _BV(SDIN) | _BV(DC) | _BV(SCE_) | _BV(RES_));
	// set RES: disable reset; IC ready to run
	lcdPort  |= (unsigned char)(_BV(SCLK) | _BV(SDIN) | _BV(DC) | _BV(SCE_) | _BV(RES_));
	// set all (high); RESET complete, LCD in suspend

	//	lcdReset;
	lcdClr();

	// init settings
	lcdModeCmd;		// set command mode
	lcdByte(0x21);	// extended instruction set
	
//	lcdByte(0x90);	// set VOP
	lcdByte(0xC8);	// set VOP
	lcdByte(0x06);	// set temperature co-eff
	lcdByte(0x13);	// bias (1:48)
	
	lcdByte(0x20);	// normal instruction set: PD= 0, V= 0
//	lcdByte(0x22);	// normal instruction set: PD= 0, V= 1
	lcdByte(0x0c);	// normal mode: d=1, e=0
}
//}
//	void lcdInit(void)

/*** lcdXY ********************************************************************
 *	
 *	Set cursor to location x:y (0-83:0-47).
 *	
 **************************************************************************///{
void lcdXY(uint8 x, uint8 y) {
	lcdModeCmd;
	lcdByte(x|0x80);	// 0b1xxx xxxx
	lcdByte(y|0x40);	// 0b0100 0xxx
}
//}
//	void lcdXY(uint8 x, uint8 y)


/*--------------------------------------------------------------------------------------------------

  Name         :  lcdLine

  Description  :  Draws a line between two points on the display.

  Argument(s)  :  x1, y1 -> Absolute pixel coordinates for line origin.
                  x2, y2 -> Absolute pixel coordinates for line end.
                  mode   -> Off, On or Xor. See enum.

  Return value :  None.

--------------------------------------------------------------------------------------------------*/
/*
void lcdLine ( uint8 x1, uint8 y1, uint8 x2, uint8 y2, uint8 mode )
{
    int dx, dy, stepx, stepy, fraction;

    dy = y2 - y1;
    dx = x2 - x1;

    if ( dy < 0 )
    {
        dy    = -dy;
        stepy = -1;
    }
    else
    {
        stepy = 1;
    }

    if ( dx < 0 )
    {
        dx    = -dx;
        stepx = -1;
    }
    else
    {
        stepx = 1;
    }

    dx <<= 1;
    dy <<= 1;

    lcdDot( x1, y1, mode );

    if ( dx > dy )
    {
        fraction = dy - (dx >> 1);
        while ( x1 != x2 )
        {
            if ( fraction >= 0 )
            {
                y1 += stepy;
                fraction -= dx;
            }
            x1 += stepx;
            fraction += dy;
            lcdDot( x1, y1, mode );
        }
    }
    else
    {
        fraction = dx - (dy >> 1);
        while ( y1 != y2 )
        {
            if ( fraction >= 0 )
            {
                x1 += stepx;
                fraction -= dy;
            }
            y1 += stepy;
            fraction += dx;
            lcdDot( x1, y1, mode );
        }
    }

  //  UpdateLcd = TRUE;
}
*/

