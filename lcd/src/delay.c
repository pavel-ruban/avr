/**
	@file	delay.c
	@brief	Delay functions
	@author	Nick Lott 
	@date	September 2004
	
	This file contains short delay routines as used by the butterflyMP3
	project http://butterflymp3.sf.net
	
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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>
#include "types.h"
#include "delay.h"
#include "uart.h"


/**
   Millisecond Delay-loop

   @param    millisec 16bit number of milliseconds to wait
**/
void Delay(uint16 millisec)
{
	// this delay is calibrated for 8MHz clock.
    uint16 i;
    while (millisec--)
        for (i=0; i<1325; i++)
			asm("nop");
}
