/**
	@file	lcdgraphics_alpha.c
	@brief	ALPHA LCD MP3 Functions
	
	@author	Nick Lott 
	@date	September 2004
 
	$Id: lcdgraphics_alpha.c,v 1.1 2007/06/04 15:10:14 brokentoaster Exp $

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

#ifdef ALPHA

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "main.h"
#include "types.h"
#include "lcdgraphics.h"
#include "BCD.h"
#include "avrfat16.h"			// Included for long filename / ID3 stuff.
	

	#include "LCD_driver.h"
	#include "LCD_functions.h"
//	volatile char gLCD_Update_Required;
//	char LCD_Data[LCD_REGISTER_COUNT];
//	char gTextBuffer[TEXTBUFFER_SIZE];
//	volatile char gScrollMode;
//	char gFlashTimer;
//	char gColon;
//	volatile signed char gScroll;
	uint8	ALPHA_textBufferPos;
extern uint8	gDisplayTimeOut; // from main.c to set # of seconds to show info

#include "uart.h"





/**
*	Initialize hardware 
**/
void LCD_Initialize(void)
{
		LCD_Init();
}


/** 
*	clear LCD
**/
void LCD_Blank(void)
{

		LCD_Clear();

}


/** 
*	Put LCD into powersave mode
**/
void LCD_Sleep(void)
{

		LCD_Clear();
		//CBI(LCDCRA, 7);             // disable LCD
		//Enable LCD start of frame interrupt
		LCDCRA &= ~( (1<<LCDEN));
}


/**
*	Wake LCD from powersave mode
**/
void LCD_Wake(void)
{
	extern uint8 gLCD_Refresh;


	LCD_Init();
	//SBI(LCDCRA, 7);             // enable LCD
	LCDCRA |=  (1<<LCDEN) ;
	gLCD_Refresh=TRUE;
}


/**
 *	Print a single 4bit digit
 *
 *	@param Data 8bit int to print, upper nibble is ignored 
 **/
void LCD_PrintfU4(uint8 Data)
{
	/* Send 4-bit hex value */
    uint8 Character[2];
	Character[0] = Data&0x0f;
    if (Character[0]>9)
    {
        Character[0]+='A'-10;
    }
    else
    {
        Character[0]+='0';
    }

	gTextBuffer[ALPHA_textBufferPos++] = Character[0];
	gTextBuffer[ALPHA_textBufferPos] =0;

	
}

/**
	Print two 4bit Digits
	@param Data 8bit int to print
**/
void LCD_PrintfU8(uint8 Data)
{
    /* Send 8-bit hex value */
    LCD_PrintfU4(Data>>4);
    LCD_PrintfU4(Data);
}






/**
	Print Play time on display
	@param data[] 16bit integer array containting play time information.
	@todo		- better play time indicator
	@todo		- decimal time
	@todo		- song position.
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
	uint8 i;
	//extern char gScroll;
	//extern char gScrollMode;

	extern char gLCD_Start_Scroll_Timer;
 
	gDisplayTimeOut = 0;
	gShowFilename = TRUE;
	for (i=0;FAT16_longfilename[i]&&i<=LFN_BUFFER_LENGTH;i++); // find name length
	if ((i>6)){ // scroll if larger than display
		gLCD_Start_Scroll_Timer=3;
		gScroll=0;
		LCD_UpdateRequired(TRUE,TRUE);
	} else{
		gScroll = 0;
		LCD_UpdateRequired(TRUE,FALSE);
	}
	
	return 0;
}

/**
	Display the "playing" symbol 
**/
void LCD_Play(void)
{
	//gShowFilename =FALSE; 
	LCD_puts_f(PSTR("PLAY\0"), 1);
	gDisplayTimeOut = STAT_DISPLAY_TIME+1;
	//LCD_UpdateRequired(TRUE,TRUE);

}


/** 
	Display the "stopped" symbol
**/
void LCD_Stop(void)
{
	LCD_puts_f(PSTR("STOP\0"), 1);
	gDisplayTimeOut = STAT_DISPLAY_TIME+1;

}


/** 
	Display the "Paused" symbol
**/
void LCD_Pause(void)
{

	LCD_puts_f(PSTR("PAUSE\0"), 1);
	gDisplayTimeOut = STAT_DISPLAY_TIME+1;

}

/** 
	Show the volume
	@param uint8 containing the volumt lower nibble is used.
**/
void LCD_Vol(uint8 volume, uint8 boostOn)
{
		gTextBuffer[0]='V';
		gTextBuffer[1]='O';
		gTextBuffer[2]='L';
		gTextBuffer[3]=' ';
		ALPHA_textBufferPos =4;	
		LCD_PrintfU4(volume);

		gDisplayTimeOut = STAT_DISPLAY_TIME+1;
		LCD_puts(gTextBuffer,1);
}

/**
	Print the startup message to the LCD screen
	@return		uint8 status code (always zero)
**/
uint8 LCD_Tester(void)
{
		

	uint8 i;
	gShowFilename = TRUE;

	for (i = 0; pgm_read_byte(&VERSIONINFO1[i]) && i < 25; i++){
		FAT16_longfilename[i] = pgm_read_byte(&VERSIONINFO1[i]);
    }
	FAT16_longfilename[i] = 0;
	LCD_UpdateRequired(TRUE,TRUE);
	

	return 0;
}	



/**
	Print Sanning message while searching MMC
**/
void LCD_Scanning(void)
{

	LCD_puts_f( PSTR("SCANNING...\0"),1);
	gDisplayTimeOut = 0;

	
}

/**
	Print Battery Status on LCD
 **/
void LCD_Battery(uint8 percentage)
{

	// TODO: make use of the flat battery LCD segment
	if (percentage <100){
		ALPHA_textBufferPos =0;	
		LCD_PrintfU8(CHAR2BCD2(percentage));
	}else{
		gTextBuffer[0]='1';
		gTextBuffer[1]='0';
		gTextBuffer[2]='0';
		ALPHA_textBufferPos =3;	
	}
	gTextBuffer[ALPHA_textBufferPos]=' ';
	gTextBuffer[ALPHA_textBufferPos+1]='*';//'%'
	gTextBuffer[ALPHA_textBufferPos+2]='/';
	gTextBuffer[ALPHA_textBufferPos+3]=':';
	gTextBuffer[ALPHA_textBufferPos+4]=' ';
	gTextBuffer[ALPHA_textBufferPos+5]='B';
	gTextBuffer[ALPHA_textBufferPos+6]='T';
	gTextBuffer[ALPHA_textBufferPos+7]='R';
	gTextBuffer[ALPHA_textBufferPos+8]='Y';
	gTextBuffer[ALPHA_textBufferPos+9]=0x00;
	LCD_puts(gTextBuffer,1);
	gDisplayTimeOut = STAT_DISPLAY_TIME+1;
	
}

#endif
