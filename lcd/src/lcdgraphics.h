/**
	@file	lcdgraphics.h
	@brief	LCD MP3 Functions
	@author	Nick Lott 
	@date	September 2004
 
	$Id: lcdgraphics.h,v 1.9 2007/06/04 15:12:17 brokentoaster Exp $

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

#ifndef  LCDG_C
#define LCDG_C


#define SONG_TITLE_WIDTH      20
#define SONG_TITLE_PAUSE      4


void LCD_Initialize(void);
void LCD_Blank(void);
void LCD_PrintfU4(uint8 Data);
void LCD_PrintfU8(uint8 Data);
uint8 LCD_Time(uint16 data);
uint8 LCD_FileName(uint8 resetScroll);
void LCD_Play(void);
void LCD_Stop(void);
void LCD_Pause(void);
void LCD_Vol(uint8 volume, uint8 boostOn);
uint8 LCD_Tester(void);
void LCD_Sleep(void);
void LCD_Wake(void);
void LCD_Scanning(void);
void LCD_Battery(uint8 percentage);

#endif
