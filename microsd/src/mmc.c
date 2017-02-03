/**
	@file	mmc.c
	@brief	MultiMedia Card low level Functions
	@author	Nick Lott brokentoaster@users.sf.net
	@date	September 2004
 
	$Id: mmc.c,v 1.17 2007/06/04 15:12:18 brokentoaster Exp $

	This file represents a convergence of a number of code snippets found 
	on the web, some of the Yampp system by Jesper Hansen and the work
	done by Sylvain.Bissonnette@microsyl.com. The goal is to produce an 
	adaptable library for doing low level MMC activities over the SPI bus.
	This code was written with the Atmega169V in mind (aka "Butterfly"). 
	http://butterflymp3.sf.net
	
	For details Concerning the MMC spec see www.sandisk.com. I refered to 
	"SanDisk MultiMediaCard and Reduced-Size MultiMediaCard Product Manual"
	Doc No. 80-36-00320 v1.0 during the writing of this code.
	
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
	
History:

24-26 Sep 2004:
	Initial write and port of functions collected over last few 
	months 
	
March 2005:		
	Add new improvements based around Circuit cellar article.
	Updated Comments for Doxygen

**/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#define MMC_C
#include "types.h"
#include "mmc.h"
#include "delay.h"
#include "main.h"
#include "uart.h"
#include "vs1001.h" 

uint8 mmc_sbuf[512];	///< 512 byte sector buffer in internal RAM
uint8 mmc_scratch[32]; 	///< 32 byte Scratch buffer for CSD/CID/ ops


/**
*	Setup Pin configuration of SPI bus for the AVR
**/
void SpiInit(void)
{
	uint8 dummy;
	
	SBI(SPI_DDR, SPI_SS);		// SS must be output for Master mode to work
	SBI(SPI_DDR, SPI_MOSI);		// set MOSI a output
	SBI(SPI_DDR, SPI_SCK);		// set SCK as output
	CBI(SPI_DDR, SPI_MISO); 	// MISO is input
	//CBI(SPI_PORT, SPI_SCK);		// set SCK low
	
	SBI(MMC_PORT,MMC_CS);
	SBI(MMC_DDR,MMC_CS);
	
	// hold dataflash in reset so it doesn't interfere with 
	// MMC or VS1001.
	DDRF |= (1<<PE7);	// PE7 is output.
	PORTF &= ~(1<<PE7); // hold PE7 low
	
	/* enable SPI as master, set clk divider fosc/4 */
	// Although the MMC card is capable of clock speeds up to 20Mhz
	// We are limited to 2Mhz for the VS1001k until we set the clock doubler 
	// after which we are limited to 4Mhz by the butterfly.
	/* later we will enable 2X mode once vS1001 has been initialized.*/
	SPCR = (0<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0);     
	SPSR = (0<<SPI2X);
	dummy = SPSR;	// clear status
	dummy = SPDR;
}


/**
* Main SPI routine
*  - transmits a byte and receives a byte simultaneously
*  - received byte is returned
*  - if you only want to read a byte, put a dummy
*    (say 0xff) in the transmit slot
*
*	@param byte Byte to transmit
* 	@return byte clocked in during transmit
**/
uint8 SpiByte(unsigned char byte)
{
	uint8 i,dummy;

	SPDR = byte;              	 	// put byte to send in SPDR, which initiates xmit  
	
	loop_until_bit_is_set(SPSR, SPIF); 	//wait for completion 
	
	//delay(5); //wait 5 microseconds after sending data to control port
	for (i=0;i<8;i++) // 2 + 5i clock cycles -> 42 clocks at 8 Mhz -> 5.25uS
		asm volatile("nop");
		
	i = SPSR;	// clear status
	dummy = SPDR;
	return dummy;               	// return with byte shifted in from slave
	
}


/**
*	Retrieve data from then MMC.
*	Pings the card until it gets a non-0xff value
*
*  	@return first byte of data usualy a data token.
**/
uint8 MMCGet(void)
{
	uint8 i,Byte;
	
	for (i=0;i<8;i++){                      
	   Byte = SpiByte(0xff);
	   if (Byte != 0xff) return Byte;
	}
	
	return Byte;
}

/**
* 	Send a control command to the MMC.
*	send one byte of 0xff, then issue command + params + (fake) crc
* 	eat up the one command of nothing after the CRC
*   Clears the MMC_CS and BSYNC_PIN lines.
*	MMC must be de-selected outside of this code !!
*
* 	@param command 	MMC Command to be sent
* 	@param px		first byte of command parameters
* 	@param py		second byte of command parameters
* 	@param pz		third byte of command parameters
*
**/
void MMCCommand(unsigned char command, uint8 px, uint8 py, uint8 pz)
{
	CBI(MMC_PORT,MMC_CS);	// Select card
	CBI( BSYNC_PORT,   BSYNC_PIN ); 	// byte sync lo
	SpiByte(0xff);
	SpiByte(command | 0x40);
	SpiByte(px);
	SpiByte(py);
	SpiByte(pz);
	SpiByte(0x00);
	SpiByte(0x95);            
	/* 
	correct CRC for first command in SPI          
	after that CRC is ignored, so no problem with 
	always sending 0x95
    */
	/*
	 the reason I did not re set the MMC_CS line was because I didn't want to 
	 de-assert the line after the command while waiting for a response.  
	 The card is selected here as a backup to make sure it is selected. 
	 I might go through and remove places where it is redundant when 
	 i get a chance.  
		
	The BSYNC line is held low to ensure that the spi data is sent as a command 
	 to the MMC and not as data to teh vs1001 as I have combinned both the 
	 interfaces on the vs1001 to save pins. 
		
	Any data on the spi lines is read as data by the vs1001 when the
	 BSYNC line is high.
	*/
}


/**
*	Perform a Hardware then software reset of the MMC.
*
*	@return 00 if reset successful
*	@return FF if reset unsuccessful
**/
uint8 MMC_Reset(void)			
{
	unsigned char Byte;
	unsigned int i;

	SBI(MMC_PORT,MMC_CS);
	//CBI( BSYNC_PORT,   BSYNC_PIN ); 	// byte sync lo
	
	/* start off with 80 bits of high data with card deselected */
	for(i=0;i<10;i++)
		Byte = SpiByte(0xff);
		
	/* now send CMD0 - go to idle state, try up to 100 times */
	MMCCommand(0,0,0,0);  /* software reset command */

	if (MMCGet() != 1)
	  {
		  SBI(MMC_PORT ,MMC_CS);
		  SpiByte(0xff);
		  return -1;  // MMC Not detected
	  }
	
	 
	/* send CMD1 until we get a 0 back, indicating card is done initializing */
	i =0xffff;    /* but only do it up to 1000 times */
	Byte = 1;
	
	while (Byte && i--){
	   MMCCommand(1,0,0,0);
	   Byte = MMCGet();
	   }
	
	SBI(MMC_PORT,MMC_CS);
	SpiByte(0xff); // Clear SPI
	
	if (Byte){
	  return -2;  // Init Fail
	  }
	  

	return 0;
}

/**
 *	Checks if there is a memory card in the slot
 *	This requires a pull down resistor on the CS 
 *  line of about 100K.
 *
 *	@return	FALSE		card not detected
 *	@return TRUE		car detected
 **/
uint8 MMC_Detect(void)
{
	uint8 i;
	#if (REV >='B') // MMC detect only available on rev_b boards.
	MMC_DDR &= ~(1<<MMC_CS); //
	MMC_PORT &= ~(1<<MMC_CS); // disable pullup
	
	//wait 18 microseconds after sending data to control port
	for (i=0;i<29;i++) // 2 + 5i clock cycles -> 147 clocks at 8 Mhz -> 18.375uS
		asm volatile("nop");
		
	if((MMC_PIN & (1<<MMC_CS)) == 0)	
	{
		MMC_PORT |= (1<<MMC_CS); 
		MMC_DDR |= (1<<MMC_CS);
		return FALSE;
	}
	MMC_PORT |= (1<<MMC_CS); 
	MMC_DDR |= (1<<MMC_CS);	
	#endif
	return TRUE;
}


/**
*	Send the get status command to the MMC and returns the 
*	result
*
*	@return 2 byte value from MMC status register
**/
uint16 MMC_Check(void)
{
	uint16 word;

	MMCCommand(MMC_SEND_STATUS,0,0,0); // check card ststus
	word = MMCGet();
	word = (word<<8) + SpiByte(0xff); // NOTE spibyte and not MMCGET!!!
	
	SBI(MMC_PORT,MMC_CS); // Deselect card
	SpiByte(0xff); // Clear SPI
	
	return word;
}

/**
*	Grab the Serial number & info from the card.
*	Returns status response from card.
*	if successful CID is in mmc_scratch[1..16]
*	
*	@return byte response from the CID command
*
**/
/*
uint8 MMC_Identify(void)
{
	uint8 byte,data,i;
	

	MMCCommand(MMC_SEND_CID,0,0,0); // send cmd
	byte = MMCGet(); // check reponse
	
	if (byte==0){ // get the CID...
		for (i=0;i<32;i++){
			data = SpiByte(0xff); 
			mmc_scratch[i]=data;
		}
	}

	SBI(MMC_PORT,MMC_CS); // Deselect card
	SpiByte(0xff); // Clear SPI
	
	return byte;
}
*/

/**
*	Retrieves the CSD Register from the mmc and stores it in
*	mmc_scratch[1..17]
*
*	@return		Status response from cmd
**/
/*
uint8 MMC_CardType(void)
{
	uint8 byte,data,i;
	
	MMCCommand(MMC_SEND_CSD,0,0,0); // send cmd
	byte = MMCGet(); // check reponse
	
	if (byte==0){ // get the CID...
		for (i=0;i<32;i++){
			data = SpiByte(0xff); 
			mmc_scratch[i]=data;
		}
	}

	SBI(MMC_PORT,MMC_CS); // Deselect card
	SpiByte(0xff); // Clear SPI
	
	return byte;
}
*/
/**
*	Calculates the capacity of the MMC in blocks
*
*	@return	uint32 capacity of MMC in blocks or -1 in error;
**/
//
//uint32 MMC_Capacity(void)
//{
//	uint8 byte,data,multi,blk_len;
//	uint32 c_size;
//	uint32 capacity;
//	byte = MMC_CardType();
//	if (byte==0) {// got info okay
//		blk_len = 0x0F & mmc_scratch[6]; // this should equal 9 -> 512 bytes
//		/*	; get size into reg 
//			;	  7				8			9
//			; xxxx xxxx    xxxx xxxx    xxxx xxxx
//			;        ^^    ^^^^ ^^^^    ^^ 
//		*/
//		data =(mmc_scratch[7] & 0x03)<<6;
//		data |= (mmc_scratch[8] >> 2);
//		c_size = data << 4;
//		data =(mmc_scratch[8] << 2 ) | ((mmc_scratch[9] & 0xC0)>>6 );
//		c_size |= data;
//		
//		/*	; get multiplier
//			;	10			11
//			; xxxx xxxx    xxxx xxxx
//			;        ^^    ^
//		*/
//		multi = ((mmc_scratch[10] & 0x03 ) << 1 ) ;
//		multi |= ((mmc_scratch[11] & 0x80) >> 7);
//		// sectors = (size+1)<<(multiplier+2)
//		capacity = (c_size + 1)<<(multi + blk_len + 2);
//
//		return capacity;
//	}
//	return 0xFFFFFFFF;
//}
//

/**
*	Read the OEM/Manufcatures MMC Name into the scratchpad.
*
*	@return		status of MMC from SEND_CID cmd.
**/
//uint8 MMC_Name(void)
//{
//	uint8 byte,i;
//	
//	byte = MMC_Identify(); // Grab CID into mmc_scratch 
//
//	if (byte==0){
//		for (i=0;i<6;i++)
//			mmc_scratch[i]=mmc_scratch[i+4];
//		mmc_scratch[6]=0;
//	}
//	
//	return byte;
//}
    
	
/***************************************************************************
*	MMC_Read	
*	Read one sector from the memory card into mmc_sbuf[]
*	Note this assumes block length of 512 Bytes
*	@Param		<lba> the number of the sector to read
*	@Return		Status from the read call.
***************************************************************************/
uint8 MMC_Read(uint32 lba)
{
	uint8 byte,data,px,py,pz;
	uint16 sec,i;
	uint16 word;
#ifdef USE_MMC_CACHE
	static uint32 last_read_lba=0xffffffff; // remember last sector for caching purposes

	if (last_read_lba == lba){ // exit ASSUMING BUFFER HAS NOT BEEN ALTERED
		return 0x00; // OK
	}
#endif
	
	SpiByte(0xff);
	CBI(MMC_PORT,MMC_CS);	// Select card
	MMCCommand(MMC_SEND_STATUS,0,0,0); // check card status
	word = MMCGet();
	word = (word<<8) + SpiByte(0xff);
	
	
	lba <<=9; // * 512
	pz = (lba>>8 & 0xff);
	py =( lba>>16 & 0xff);
	px =( lba>>24 & 0xff);
	
	SpiByte(0xff);
	CBI(MMC_PORT,MMC_CS);	// Select card
	MMCCommand(MMC_READ_SINGLE_BLOCK, px , py, pz);
	byte = MMCGet(); // check reponse
	
	//TODO:  if i get a byte == 0x05 should I resend?
	
	if (byte==0){ // then everything okay
		i=500; // wait up to 255 bytes for response
		data = SpiByte(0xff);
		
		while ((data != MMC_STARTBLOCK_READ) && (i--)){
			data = SpiByte(0xff);
		}
		
		if (i!=0){ // read okay. start grabbing the data
			for(sec=0;sec<512;sec++)
				mmc_sbuf[sec] = SpiByte(0xff);
			SpiByte(0xff); // flush MMC CRC 
			SpiByte(0xff); // 
			SpiByte(0xff); //Clear SPI
			SpiByte(0xff); //
		}else{
			byte = 0xff;// signal MMC Coms error
		}
	//}else{
	//	PRINT("MMC_READ_SINGLE_BLOCK\t");
	//	PRINT("\t");UART_Printfu32(lba<<9);
	//	PRINT("\t");UART_Printfu16(word);
	//	PRINT("\t");UART_Printfu08(byte);
	//	PRINT("\t");UART_Printfu08(SpiByte(0xff));
	//	EOL();
		
	}

	SBI(MMC_PORT,MMC_CS); // Deselect card
	SpiByte(0xff); // Clear SPI

#ifdef USE_MMC_CACHE
	if (byte == 0){
			last_read_lba = lba;// save for next time.
	}
#endif
	return byte;
}

 /*************************************************************************
 *	uint8 MMCWriteSector(uint32lba)
 *
 * - writes a sector to the card (512 bytes)
 * - takes sector # as param
 * - data is in mmc_sbuf[] 
 * - returns success/failure status of operation.
 **************************************************************************/
 // ****** UNTESTED ******
//uint8 MMC_Write(uint32 lba)
//{
//	unsigned char px,py,pz;
//	unsigned int i;
//	lba <<=1;
//	pz = (lba & 0xff);
//	lba >>= 8;
//	py =( lba & 0xff);
//	lba >>= 8;
//	px =( lba & 0xff);
//	
//	CBI(MMC_PORT,MMC_CS);	// Select card
////	MMCCommand(MMC_WRITE_BLOCK, (lba>>7)& 0xffff, (lba<<9)& 0xffff);
//	if (MMCGet() == 0xff) return 0xff;
//	
//	SpiByte(0xfe);  // Send Start Byte
//	
//	for (i=0;i<512;i++)       // read the sector 
//	   {
//		SpiByte(mmc_sbuf[i]);
//	   }
//	SpiByte(0xff);          // checksum -> don't care about it for now 
//	SpiByte(0xff);       // checksum -> don't care about it for now 
//	SpiByte(0xff);       // Read "data response byte"                 
//	
//	if (MMCGet() == 0xff) return 0xff;
//	
//	SBI(MMC_PORT,MMC_CS); // Deselect card
//	SpiByte(0xff); // Clear SPI
//	
//	return 0;
//}

//void MMC_Flush(void)
//{
//	uint8 i;
//	SBI(MMC_PORT,MMC_CS);
//	/* start off with 80 bits of high data with card deselected */
//	for(i=0;i<10;i++)
//		SpiByte(0xff);
//	//CBI(MMC_PORT,MMC_CS);        /* select card */
//}


/***************************************************************************
*   Name:			MMC_WriteProtect
*	Description:	Set or clear the MMC Writeprotect
*	Parameters:		<set> determines weather to set or clear the write protection
*	Returns:		error code.
***************************************************************************/
//uint8 MMC_WriteProtect(uint8 set){
//	uint8 byte;
//	
//	if (set){
//		MMCCommand(MMC_SET_WRITE_PROT,0,0,0); // send cmd
//	}else{
//		MMCCommand(MMC_CLR_WRITE_PROT,0,0,0); // send cmd
//	}
//	byte = MMCGet(); // check reponse
//
//	SBI(MMC_PORT,MMC_CS); // Deselect card
//	SpiByte(0xff); // Clear SPI
//	
//	return byte;
//}
