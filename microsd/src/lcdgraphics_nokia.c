/**
        @file	lcdgraphics_nokia.c
	@brief	NOKIA LCD MP3 Functions
	
	@author	Nick Lott
	@date	September 2004
 
	$Id: lcdgraphics_nokia.c,v 1.1 2007/06/04 15:10:14 brokentoaster Exp $
 
	This file represents a convergence of a number of code snippets found
	on the web, some of the Yampp system by Jesper Hansen and the work
	done by Sylvain.Bissonnette@microsyl.com. The goal of this code is to
	run the graphical LCD for the Butterfly MP3 project.
	http://butterflymp3.sourceforge.net/
	
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

#ifdef NOKIA

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "main.h"
#include "types.h"
#include "lcdgraphics.h"
#include "BCD.h"
#include "avrfat16.h"			// Included for long filename / ID3 stuff.


#include "pcd8544.h"
#include "fontset0.h"
#include "font_ttuf1.h"
#include "font_ttuf2.h"



//
#include "uart.h"





//Butterfly MP3 title graphic
// static unsigned char 
__attribute__((progmem))
static const char __attribute__((progmem)) bfMP3Title[] =
{
	0x41, 0x51, 0x47, //'B'
	0x7F,
	0x41, 0x5F, 0x41, //'U'
	0x7F,
	0x7D, 0x41, 0x7D, //'T'
	0x7F,
	0x7D, 0x41, 0x7D, //'T'
	0x7F,
	0x41, 0x55, 0x5D, //'E'
	0x7F,
	0x41, 0x75, 0x4B, //'R'
	0x7F,
	0x41, 0x75, 0x7D, //'F'
	0x7F,
	0x41, 0x5F, 0x5F, //'L'
	0x7F,
	0x71, 0x47, 0x71, //'Y'
	0x7F,
	0x7F, 0x7F, 0x7F, //' '
	0x7F,
	0x41, 0x79, 0x41, //'M'
	0x7F,
	0x41, 0x75, 0x71, //'P'
	0x7F,
	0x5D, 0x55, 0x41, //'3'
	
};


/**
*	Initialize hardware
 **/
void LCD_Initialize(void)
{
	lcdInit();
	
}


/**
*	clear LCD
 **/
void LCD_Blank(void)
{
	
	uint8           i;
	
	lcdClr();
	lcdXY(0, 0);
	lcdModeData;
	
	for (i = 0; i < 16; i++)
		lcdByte(0x7F);
	for (i = 0; i < sizeof(bfMP3Title); i++) {
		lcdByte(pgm_read_byte(bfMP3Title + i));
	}
	for (i = 0; i < 17; i++)
		lcdByte(0x7F);
	
	
}


/**
*	Put LCD into powersave mode
 **/
void LCD_Sleep(void)
{
	lcdClr();
	lcdSuspend;
	lcdReset;
}


/**
*	Wake LCD from powersave mode
 **/
void LCD_Wake(void)
{
	extern uint8    gLCD_Refresh;
	lcdResume;
	lcdInit();
	LCD_Blank();
	//Make sure backgound image only is set
	
	gLCD_Refresh = TRUE;
}


/**
*	Print a single 4bit digit
 *
 *	@param Data 8bit int to print, upper nibble is ignored
 **/
void LCD_PrintfU4(uint8 Data)
{
	/* Send 4-bit hex value */
	uint8           Character[2];
	Character[0] = Data & 0x0f;
	if (Character[0] > 9) {
		Character[0] += 'A' - 10;
	} else {
		Character[0] += '0';
	}
	
	//load a pointer to the font
	uint8 * font = (uint8 *) fontSet0;
	//uint8 * font2 = (uint8 *) fontSet1;
	//uint8 * font2 = (uint8 *) fontSet1;
	
	//uint8 fontSize = pgm_read_byte(font + 0);
	//chars in this font
	// uint8 fontX = pgm_read_byte(font + 1);
	//font width
	// uint8 fontY = pgm_read_byte(font + 2);
	//font hieght
	
	Character[1] = 0x00;
	lcdWrite(font, Character, 0, 1);
	
	
}

/**
Print two 4bit Digits
	@param Data 8bit int to print
 **/
void LCD_PrintfU8(uint8 Data)
{
	/* Send 8-bit hex value */
	LCD_PrintfU4(Data >> 4);
	LCD_PrintfU4(Data);
}






/**
Print Play time on display
	@param data[] 16bit integer array containting play time information.
	@todo	- better play time indicator
	@todo	- decimal time
	@todo	- song position.
 **/
uint8 LCD_Time(uint16 data)
{
	
	uint8          *font = (uint8 *) fontSet1;
	uint8           cBuff[6];
	
	lcdModeData;
	
	//fbXY[0] = 43;
	//x
	// fbXY[1] = 2;
	//y
	// LCD_PrintfU8(data >> 8);
	//LCD_PrintfU8(data & 0xFF);
	
	fbXY[0] = 65;
	//x
	fbXY[1] = 3;
	//y
	
	cBuff[0] = (data / 600);
	data -= 600 * cBuff[0];
	cBuff[1] = (data / 60);
	data -= 60 * cBuff[1];
	
	cBuff[2] = 58;
	//':'
	
	cBuff[3] = (data / 10);
	data -= 10 * cBuff[3];
	cBuff[4] = (data);
	
	cBuff[5] = '\0';
	
	cBuff[0] += 0x30;
	cBuff[1] += 0x30;
	cBuff[3] += 0x30;
	cBuff[4] += 0x30;
	
	lcdWrite(font, cBuff, 0, 0xFF);
	
	return 0;
}

/**
Print long filename on display
	@param name[] null terminated string containing filename
 **/

uint8 LCD_FileName(uint8 resetScroll)
{
	static uint8    byte = 0;
	static uint8    textIndex = LFN_TITLE_INDEX;
	static uint8   *textLen = &FAT16_LFNTitleLen;
	uint8          *font = (uint8 *) fontSet1;
	
	if (resetScroll) {
		byte = 0;
		textIndex = LFN_TITLE_INDEX;
		textLen = &FAT16_LFNTitleLen;
	}
	fbXY[0] = 0;
	//x
	fbXY[1] = 2;
	//y
	
	// Blank out the whole line(artist could be shorter than track and
	// vice - versa)
	lcdClrLine(2);
	
	lcdModeData;
	if (byte < SONG_TITLE_PAUSE)
		lcdWrite(font, FAT16_longfilename + textIndex, 0, SONG_TITLE_WIDTH);
	else {
		lcdWrite(font,
                         FAT16_longfilename + textIndex + byte - SONG_TITLE_PAUSE,
                         0,
                         SONG_TITLE_WIDTH);
	}
	
	byte++;
	
	if (*textLen >= SONG_TITLE_WIDTH)
		//We are scrolling
	{
		if (byte > *textLen - SONG_TITLE_WIDTH + SONG_TITLE_PAUSE)
			goto toggleText;
	} else
		//We are not scrolling
	{
		if (byte > SONG_TITLE_PAUSE)
			goto toggleText;
	}
	
	return 0;
	
	//Only efficient way of doing(without yet another function call with a lot
	// of parameters was a goto
toggleText:
	{
		byte = 0;
		
		//Toggle between displaying artist and title
		if ((FAT16_longfilename[LFN_TYPE_INDEX] == LFN_TYPE_FILENAME) ||
                    (textIndex == LFN_ARTIST_INDEX)) {
			textIndex = LFN_TITLE_INDEX;
			textLen = &FAT16_LFNTitleLen;
		} else if ((FAT16_longfilename[LFN_TYPE_INDEX] & LFN_TYPE_ID3_ARTIST)
                           == LFN_TYPE_ID3_ARTIST) {
			textIndex = LFN_ARTIST_INDEX;
			textLen = &FAT16_LFNArtistLen;
		}
		return 0;
	}
	
	
}

/**
Display the "playing" symbol
 **/
void LCD_Play(void)
{
	lcdXY(0, 3);
	lcdModeData;
	lcdByte(0x1F);
	lcdByte(0x1F);
	lcdByte(0x0E);
	lcdByte(0x0E);
	lcdByte(0x04);
	lcdByte(0x04);
	
}


/**
Display the "stopped" symbol
 **/
void LCD_Stop(void)
{
	lcdXY(0, 3);
	lcdModeData;
	lcdByte(0x1F);
	lcdByte(0x1F);
	lcdByte(0x1F);
	lcdByte(0x1F);
	lcdByte(0x1F);
	
}


/**
Display the "Paused" symbol
 **/
void LCD_Pause(void)
{
	
	lcdXY(0, 3);
	lcdModeData;
	lcdByte(0x1F);
	lcdByte(0x1F);
	lcdByte(0);
	lcdByte(0x1F);
	lcdByte(0x1F);
	
	
}

/**
Show the volume
	@param uint8 containing the volumt lower nibble is used.
 **/
void LCD_Vol(uint8 volume, uint8 boostOn)
{
	uint8           i;
	uint8           level;
	
	lcdXY(0, 5);
	lcdModeData;
	
	//draw speaker icon
	lcdByte(60);
	lcdByte(60);
	lcdByte(126);
	lcdByte(255);
	lcdByte(0);
	lcdByte(0);
	
	//draw volume graphic
	for (i = 0; i < 10; i++) {
		if (volume >= i) {
			if (i < 8) {
				level = 0xFF << (7 - i);
				if (boostOn)
					level |= 0x01 << (5 - i);
			} else
				level = 0xFF;
			
			
			lcdByte(level);
			lcdByte(level);
			lcdByte(level);
			lcdByte(0x00);
		} else {
			lcdByte(0x00);
			lcdByte(0x00);
			lcdByte(0x00);
			lcdByte(0x00);
		}
	}
	
	//draw volume level
	// fbXY[0] = 10;
	//x
	// fbXY[1] = 4;
	//y
	// LCD_PrintfU4(volume);
	
	
}

/**
Print the startup message to the LCD screen
	@return		uint8 status code (always zero)
 **/
uint8 LCD_Tester(void)
{
	
	
	//load a pointer to the font
	uint8 * font = (uint8 *) fontSet1;
	//uint8 * font2 = (uint8 *) fontSet2;
	
	//uint8 fontSize = pgm_read_byte(font + 0);
	//chars in this font
	// uint8 fontX = pgm_read_byte(font + 1);
	//font width
	// uint8 fontY = pgm_read_byte(font + 2);
	//font hieght
	
	
	lcdModeData;
	//
	
	fbXY[0] = 0;
	//x
	fbXY[1] = 1;
	//y
	lcdWrite(font,(uint8 *)  VERSIONINFO2, 1, 0xFF);
	//lcdWrite(font, PSTR(__TIME__), 1, 0xFF);
	//fbXY[0] = 0;
	//x
	// fbXY[1] = 2;
	//y
	// lcdWrite(font, (uint8 *) PSTR(__DATE__), 1, 0xFF);
	
	fbXY[0] = 0;
	//x
	fbXY[1] = 3;
	//y
	lcdWrite(font, (uint8 *) VERSIONINFO1, 1, 0xFF);
	
	fbXY[0] = 0;
	//x
	fbXY[1] = 5;
	//y
	lcdWrite(font, (uint8 *) PSTR("NO MMC\0"), 1, 0xFF);
	
	
	return 0;
}



/**
Print Sanning message while searching MMC
 **/
void LCD_Scanning(void)
{
	//load a pointer to the font
	uint8 * font = (uint8 *) fontSet1;
	
	lcdModeData;
	
	fbXY[0] = 0;
	//x
	fbXY[1] = 5;
	//y
	lcdWrite(font, (uint8 *) PSTR("SCANNING\0"), 1, 0xFF);
	
}

/**
Print Battery Status on LCD
 **/
void LCD_Battery(uint8 percentage)
{
	uint8           i;
	
	lcdXY(71, 5);
	
	lcdModeData;
	//fbXY[0] = 10;
	//x
	// fbXY[1] = 5;
	//y
	
	// LCD_PrintfU8(percentage);
	//lcdCh(font, 04);
	//'%'
	
	// Draw battery graphic
	
	lcdByte(0x7E);
	//lcdByte(0x42);
	
	for (i = 0; i < 100; i += 10) {
		if (percentage > i) {
			lcdByte(0x5A);
		} else {
			lcdByte(0x42);
		}
	}
	
	lcdByte(0x42);
	lcdByte(0x3C);
	
	
	
}
#endif
