/**
	@file	button.h
	@brief	Butterfly MP3 Button Handling
	@author	Nick Lott 
	@date	January 2005

	Copyright (C) 2005 Nick Lott <nick_Lott@bigfoot.com>
	
	This is a simple MP3 player based around the AVR Butterfly.It currently 
	uses a VS1001 mp3 decoder, MMC card and Nokia LCD. It has been heavily 
	influenced by the Yampp system by Jesper Hansen <jesperh@telia.com>.
	
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


	Target(s)...: ATmega169

	Compiler....: AVR-GCC 3.3.1; avr-libc 1.0
	
	$Id: button.h,v 1.5 2007/06/04 15:12:18 brokentoaster Exp $
**/

/// Button masks for pin change interupt handling.
#define PINB_MASK ((1<<PINB4)|(1<<PINB6)|(1<<PINB7))
#define PINE_MASK ((1<<PINE2)|(1<<PINE3))

/// Button Hardware definitions
#define BUTTON_A    6   ///< UP
#define BUTTON_B    7   ///< DOWN
#define BUTTON_C    2   ///< LEFT
#define BUTTON_D    3   ///< RIGHT
#define BUTTON_O    4   ///< PUSH

/// Button logical definitions ( virtual keys)
#define KEY_NULL    0   ///< No key has been pressed
#define KEY_ENTER   1	///< Center click detected
#define KEY_NEXT    2	///< Right click detected
#define KEY_PREV    3	///< left click detected
#define KEY_PLUS    4	///< up click detected
#define KEY_MINUS   5   ///< down click detected

void PinChangeInterrupt(void);
void Button_Init(void);
char getkey(void);
char ButtonBouncing(void);
