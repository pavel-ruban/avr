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
 *	$Id: pcd8544.h,v 1.11 2007/06/04 15:12:18 brokentoaster Exp $
 ************************************************************************
 **/
#ifndef PCD8544_H
#define PCD8544_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "types.h"


/*** LCD defines **************************************************************
 *	
 *	These are the port/pins I used; change if necessary them to suit your
 *	circuit.
 *	
 **************************************************************************///{


// for AVR Butterfly
//#define NOKIA_ALTPINOUT
#ifdef NOKIA_ALTPINOUT 
	#define lcdPort		PORTC	// port that LCD is on; needs 5 free pins
	#define lcdPortD	DDRC	//
	#define	SCLK		PC3
	#define	SDIN		PC2
	#define	DC			PC7
	#define	SCE_		PC6
	#define	RES_		PC4
#else
	#define lcdPort		PORTC	// port that LCD is on; needs 5 free pins
	#define lcdPortD	DDRC	//
	#define	SCLK		PC3
	#define	SDIN		PC5
	#define	DC			PC2
	#define	SCE_		PC1
	#define	RES_		PC4
#endif

//	LCD Defines
//#define LOwERCASE_STRINGS

/*** LCD Simple Controls ******************************************************
 *	
 *	Some low level LCD controls
 *	
 **************************************************************************///{
	#define lcdInvert1	lcdModeCmd;lcdByte(0x0d)					// invert
	#define lcdInvert0	lcdModeCmd;lcdByte(0x0c)					// normal
	
	#define	lcdModeCmd	lcdPort &= (unsigned char)~_BV(DC)
	#define	lcdModeData	lcdPort |= (unsigned char)_BV(DC)
	
	#define	lcdResume	lcdPort &= (unsigned char)~_BV(SCE_)
	#define	lcdSuspend	lcdPort |= (unsigned char)_BV(SCE_)
	
	#define	lcdReset	lcdPort &= (unsigned char)~_BV(RES_);lcdPort |= (unsigned char) _BV(RES_)	// reset the LCD
//}
//	LCD Simple Controls

/*** LCD Size *****************************************************************
 *	
 *	Used mainly during testing on an 8515/stk500. The 8515 has 512 bytes RAM,
 *	and the frame buffer is 6x84=504 bytes leaving too little (8b) for other
 *	stuff. My solution was to decrease the vertical lines available (temporarily
 *	of course) to let me use 5x84=420 bytes, and have enough for other stuff.
 *
 *	If you are using a processor with less than 1K of RAM, you can decrease the
 *	'fbRows' value to 5. You can draw to the last LCD row without the frame
 *	buffer by calling "lcdXY(x,5)" (where 'x' is 0 to 84) and then writing a
 *	byte by calling "lcdByte(value)" to fill in one column at a time.
 *	
 **************************************************************************///{
	#define fbCols		84
	#define fbRows		2
//}
//	LCD Size

/*** Global Vars **************************************************************
 *	
 *	Just the framebuffer and frame buffer cursor
 *	
 **************************************************************************///{
//	uint8 frameBuffer[fbCols][fbRows];	// __attribute__ ((section (".noinit")));
	uint8 fbXY[2];						// frame buffer cursor location
//}
//	Global Variables


/*** LCD Functions *********************************************************
*
*	User functions for manipulating the LCD.
*
***************************************************************************/
void lcdByte(uint8 data);
void lcdDot(uint8 x, uint8 yLine, uint8 color);
void lcdCh(uint8* fontSet, uint8 ch);
void lcdWrite(uint8* fontSet, uint8* buf, uint8 is_pstr, uint8 len);
void fbClr(void); 
void lcdClr(void);
void lcdClrLine(uint8 lineNo);
void lcdInit(void);
void lcdXY(uint8 x, uint8 y);

//void lcdLine ( uint8 x1, uint8 y1, uint8 x2, uint8 y2, uint8 mode );

//} LCD Functions 
#endif
