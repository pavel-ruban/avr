/**
	@file	delay.h
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

#ifndef DELAY_H
#define DELAY_H

/* constants/macros */

// F_CPU now defined in the makefile
//#define F_CPU        	11059200               		/* Processor Speed */
//#define F_CPU        	8000000               		/* Processor Speed */
#define CYCLES_PER_US 	((F_CPU+500000)/1000000) 	/* cpu cycles per microsecond */

/* prototypes */
void Delay(uint16 ms);
#endif
