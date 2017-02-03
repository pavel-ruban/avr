/*
 *  testing.h
 *  BflyMP3
 *
 *  Created by Nick on 21/03/2007.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "types.h"
#include "uart.h"
#include "delay.h"
#include "vs1001.h"
#include "mmc.h"
#include "avrfat16.h"

//#include "fontset0.h"
#include "button.h"
//#include "font_ttuf1.h"
#include "pcd8544.h"
//#include "font_ttuf2.h"
#include "lcdgraphics.h"
//#define ASSUME_CONTIGOUS 1
#define FAT_TABLEBUFFERSIZE 128
extern uint8 mmc_sbuf[];
//extern uint8 FAT_buffer[]; // 512 byte buffer for sector reads/writes
//extern 	uint8 frameBuffer[fbCols][fbRows];	
extern		uint8 fbXY[];		

#ifndef ASSUME_CONTIGOUS
uint16 fat_sectors[FAT_TABLEBUFFERSIZE];
#endif
uint8 longfilename[260];

/***************************************************************************
*   Name:	dump_buffer
*	Description: Dumps the mmc_sbuf[] to the Uart
*	Parameters: <lines> # of lines (16 Bytes) to send starting from 0x00
*	Returns: 	none
***************************************************************************/
void dump_buffer(uint8 lines, uint8 buffer[]);

/***************************************************************************
*   Name:			MMC_tester
*	Description:	Check out basic MMC functions
*	Parameters:		none
*	Returns:	Status byte, non-zero for failure.
***************************************************************************/
uint8 MMC_tester(void);


/***************************************************************************
*   Name:			FAT_tester
*	Description:	Routines to test filesystem functions to the mmc
*	Parameters:		none
*	Returns:		error code
***************************************************************************/
uint8	FAT_tester(void);




