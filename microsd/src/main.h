/**
	@file	main.h
	@brief	Butterfly MP3 main
	@author	Nick Lott 
	@date	November 2004

	$Id: main.h,v 1.32 2007/06/02 21:34:17 brokentoaster Exp $

	Copyright (C) 2004 Nick Lott <nick_Lott@bigfoot.com>
	http://butterflymp3.sf.net/
	
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

**/

#ifndef MAIN_H
#define MAIN_H

#define USE_MMC_CACHE

#define	MODE_IDLE 	0
#define	MODE_SLEEP 	1
#define MODE_PLAY 	2
#define MODE_PAUSE 	3
#define MODE_STOP 	4
#define MODE_POWEROFF	5

// Timers 
// Note undefine PWR_CHK_INTERVAL to remove any Battery voltage checking
#define AUTOPOWEROFF_TIMEOUT	30	///< define Number of seconds till power Off when idle. 0-255
#define PWR_CHK_INTERVAL	60	///< number of seconds before checking Battery voltage
#define STAT_DISPLAY_TIME	2	///< number of seconds to display stats 

#define DDR(x) ((x)-1)    /* address of data direction register of port x */
#define PIN(x) ((x)-2)    /* address of input register of port x */

// player behaviour
#define SCAN_JUMPTOSTART 1	///< Return to first song after last when searching throught songs/
#define PLAY_NEXT_SONG 1 	///< Automatically select next song when finished.
#define PLAY_REPEAT_ALL 1 	///< Automatically loop to start of MMC when end is reached.
#define RECURSE_DIRECTORIES 0	///< Automatically recurse directories
 
// Hardware Definitions
//
#define REV 'D'			///< define Hardware PCB version
// DISPLAY IS NOW DEFINED IN THE MAKEFILE
//#define ALPHA			///< define Display type use ALPHA for standard NOKIA for Graphical
#define FAT_ON_MMC		///< define storage hardware MMC or ATA (comming soon...)
#define HW_MMC_IDENTIFY		///< define MMC identification method HW or SW.
#define SERIAL_ENABLE		///< define Dubug output on Serial port

// EEPROM Location Definitions
#define EEPROM_BFFRSZE		0x0A ///< size of ring buffer

#define EEPROM_OPTIONS_INDEX	0xA0 ///< ring buffer index for player options
#define EEPROM_VOL		0x10
#define EEPROM_STATE_INDEX	0x90 ///< ring buffer index for player state.
#define EEPROM_FILE		0x20
#define EEPROM_MODE		0x30
#define EEPROM_SECTOR		0x40
#define EEPROM_PLAYED		0x50
#define EEPROM_VALID		0x60
#define EEPROM_TIME		0x70
#define USE_EEPROM		///< Enable eeprom usaage
#define ENDURANCE_EEPROM	///< Use a ringbuffer in eeprom

/*
VS1001 Connections Defined in "vs1001.h"
	DREQ		PE6
	BSYNC		PE5	
	CS		PB0 
	RESET		PB5 
	SI/SDATA	PB2
	SO 		PB3
	SCK/DCLK	PB1 

MMC Connections	Defined in "mmc.h"
	MMC_cs		PE4
	MOSI		PB2
	MISO		PB3
	SCK 		PB1

The NOKIA 3310 LCD is currently configured on PORTC with a Bit Banged SPI interface.
NOTE: This changes with the NOKIA_ALTPINOUT definition

	Defined in "pcd8544.h"
#ifdef NOKIA_ALTPINOUT 
	lcdPort		PORTC	// port that LCD is on; needs 5 free pins
	lcdPortD	DDRC	//
	SCLK		PC3
	SDIN		PC2
	DC		PC7
	SCE_		PC6
	RES_		PC4
#else
	lcdPort		PORTC	// port that LCD is on; needs 5 free pins
	lcdPortD	DDRC	//
	SCLK		PC3
	SDIN		PC5
	DC		PC2
	SCE_		PC1
	RES_		PC4
#endif
 
	
Other connections
 	DataFlash CS	PB0 // DataFlash is held in reset 
*/



extern const char VERSIONINFO1[];
extern const char VERSIONINFO2[];


// SANITY CHECK!!
#ifdef NOKIA
	#ifdef ALPHA
		#error: Too many display definitions. You can't have both
	#endif
#endif

#ifndef NOKIA
	#ifndef ALPHA
		#warning: No dislpay defined
	#endif
#endif

#endif
