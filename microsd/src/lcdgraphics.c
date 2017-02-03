/**
	@file	lcdgraphics.c
	@brief	Template for LCD MP3 Functions
	
	@author	Nick Lott 
	@date	September 2004
	
	This file is a dummy template for implementing a display on the mp3 
	player.	It is included when no other display is chosen during compile 
	time. You can use this as a basis for future dislpays such as serial or 
	other LCD systems.
 
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

//#define LCDG_C

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "types.h"
#include "lcdgraphics.h"
//#include "main.h"
//#include "BCD.h"
//#include "avrfat16.h"			// Included for long filename / ID3 stuff.
	



/**
*	Initialize hardware 
**/
void LCD_Initialize(void)
{
	return;
}


/** 
*	clear LCD
**/
void LCD_Blank(void)
{
	return;
}


/** 
*	Put LCD into powersave mode
**/
void LCD_Sleep(void)
{
	return;
}


/**
*	Wake LCD from powersave mode
**/
void LCD_Wake(void)
{
	//extern uint8 gLCD_Refresh;

	//gLCD_Refresh=TRUE;
	return;
}


/**
 *	Print a single 4bit digit
 *
 *	@param Data 8bit int to print, upper nibble is ignored 
 **/
void LCD_PrintfU4(uint8 Data)
{
	/* Send 4-bit hex value */
//    uint8 Character[2];
//	Character[0] = Data&0x0f;
//    if (Character[0]>9)
//    {
//        Character[0]+='A'-10;
//    }
//    else
//    {
//        Character[0]+='0';
//    }
	

	return;
}

/**
	Print two 4bit Digits
	@param Data 8bit int to print
**/
void LCD_PrintfU8(uint8 Data)
{
//    /* Send 8-bit hex value */
//    LCD_PrintfU4(Data>>4);
//    LCD_PrintfU4(Data);
	return;
}






/**
	Print Play time on display
	@param data[] 16bit integer array containting play time information.
**/
uint8 LCD_Time(uint16 data)
{
	return 0;
}

/**
	Print long filename on display
	@param name[] null terminated string containing filename
**/

uint8 LCD_FileName(uint8 resetScroll)
{
	return 0;
}

/**
	Display the "playing" symbol 
**/
void LCD_Play(void)
{	
	return;
}


/** 
	Display the "stopped" symbol
**/
void LCD_Stop(void)
{	
	return;
}


/** 
	Display the "Paused" symbol
**/
void LCD_Pause(void)
{	
	return;
}

/** 
	Show the volume
	@param uint8 containing the volumt lower nibble is used.
**/
void LCD_Vol(uint8 volume, uint8 boostOn)
{	
	return;
}

/**
	Print the startup message to the LCD screen
	@return		uint8 status code (always zero)
**/
uint8 LCD_Tester(void)
{
	return 0;
}	



/**
	Print Sanning message while searching MMC
**/
void LCD_Scanning(void)
{	
	return;
}

/**
	Print Battery Status on LCD
 **/
void LCD_Battery(uint8 percentage)
{
	return;
}

