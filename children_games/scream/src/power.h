/**
 @file   power.h
 @brief  Butterfly MP3 Power Routines
 @author Nick Lott 
 @date	 January 2006

 $Id: power.h,v 1.5 2007/06/02 21:34:17 brokentoaster Exp $

 
 Copyright (C) 2004 Nick Lott <brokentoaster@users.sf.net>
 http://butterflymp3.sf.net/
 
 This is the power control software for the ButterflyMP3 project.
 This file contains all software responsible for controlling the
 power save modes for the mp3 player.
 
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
 
 Compiler....: AVR-GCC 3.3.1; avr-libc 1.2
 
 **/

#ifndef POWER_H
#define POWER_H
#include "types.h"
/*
* return accurate percentage from experimental data
*
*	mins	%	ADC	ADC	~Voltage
*	0	100	655	0x28f	4.2
*	400	66	608	0x260	3.9	
*	1050	12	572	0x23c	3.6
*	1200	0	445	0x1BD	2.8
*/
#define PWR_MAX_V		0x28f ///< 4.2V
#define PWR_GOOD_V		0x260 ///< 3.85V
#define PWR_LOW_V		0x23c ///< 3.6V average voltage
#define PWR_MIN_V		0x1D9 ///< 3.0V
#define PWR_ABS_MIN_V		0x1C0 ///< 2.8V absolute minimum allowed battery voltage

volatile uint8 	gPowerSaveTimer;  	// used for sleep mode power down

void	Power_Init(void);
void 	Power_idle(void);
void 	Power_save(void);
void 	Power_off(void);
uint16	Power_check(void);
uint16	Power_pcnt(uint16 voltage); // return power left as percent


#endif
