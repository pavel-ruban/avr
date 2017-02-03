//**************************************************************
// ****** FUNCTIONS FOR SD RAW DATA TRANSFER *******
//**************************************************************
//Controller: ATmega8 (Clock: 8 Mhz-internal)
//Compiler: AVR-GCC
//Version : 2.0
//Author: CC Dharmani, Chennai (India)
// www.dharmanitech.com
//Date: 26 Feb 2009
//**************************************************************
//Link to the Post: http://www.dharmanitech.com/2009/01/sd-card-interfacing-with-atmega8-fat32.html

//**************************************************
// ***** SOURCE FILE : SD_routines.c ******
//**************************************************
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "spi.h"
#include "sd.h"
#include "tools.h"
#include <util/delay.h>
#include <stdio.h>

//******************************************************************
//Function: to initialize the SD card in SPI mode
//Arguments: none
//return: uint8_t; will be 0 if no error,
// otherwise the response byte will be sent
//******************************************************************
uint8_t SD_init(void)
{
	uint8_t i, response, retry=0 ;

	sd_select();
	do
	{
		for(i=0;i<10;i++)
			spi_transmit(0xff);
		response = SD_sendCommand(GO_IDLE_STATE, 0);//send 'reset & go idle' command
		retry++;

		if (retry>0xfe) {
			uart_str_send_P(PSTR("SD init fail..")); return 1;
		}//time out

	} while(response != 0x01);

	uint8_t bb[30];
	sprintf_P(bb, PSTR("\n\rresponse: %u\n\r"), response);
	uart_str_send(bb);
	sd_release();

	spi_transmit (0xff);
	spi_transmit (0xff);

	retry = 0;

	do
	{
		response = SD_sendCommand(SEND_OP_COND, 0); //activate card's initialization process

		//response = SD_sendCommand(SEND_OP_COND, 0); //resend command (for compatibility with some cards)
		retry++;
		if(retry>0xfe) {
			sprintf_P(bb, PSTR("\n\rresponse: %u\n\r"), response);
			uart_str_send(bb);
			uart_str_send_P(PSTR("SD init fail.. timeout")); return 1;
			return 1; //time out
		}
	}while(response);

	SD_sendCommand(CRC_ON_OFF, OFF); //disable CRC; deafault - CRC disabled in SPI mode
	SD_sendCommand(SET_BLOCK_LEN, 512); //set block size to 512

	return 0; //normal return
}

//******************************************************************
//Function: to send a command to SD card
//Arguments: uint8_t (8-bit command value)
// & unsigned long (32-bit command argument)
//return: uint8_t; response byte
//******************************************************************
uint8_t SD_sendCommand(uint8_t cmd, unsigned long arg)
{
	uint8_t response, retry=0;

	sd_select();;

	spi_transmit(cmd | 0x40); //send command, first two bits always '01'
	spi_transmit(arg>>24);
	spi_transmit(arg>>16);
	spi_transmit(arg>>8);
	spi_transmit(arg);
	spi_transmit(0x95);

	while((response = spi_receive()) == 0xff) //wait response
		if(retry++ > 0xfe) {
			uart_str_send_P(PSTR("command send failed - timeout\n\r"));
			break; //time out error
		}

	spi_receive(); //extra 8 CLK
	sd_release();;

	return response; //return state
}

//*****************************************************************
//Function: to erase specified no. of blocks of SD card
//Arguments: none
//return: uint8_t; will be 0 if no error,
// otherwise the response byte will be sent
//*****************************************************************
uint8_t SD_erase (unsigned long startBlock, unsigned long totalBlocks)
{
	uint8_t response;

	response = SD_sendCommand(ERASE_BLOCK_START_ADDR, startBlock<<9); //send starting block address
	if(response != 0x00) //check for SD status: 0x00 - OK (No flags set)
		return response;

	response = SD_sendCommand(ERASE_BLOCK_END_ADDR,(startBlock + totalBlocks - 1)<<9); //send end block address
	if(response != 0x00)
		return response;

	response = SD_sendCommand(ERASE_SELECTED_BLOCKS, 0); //erase all selected blocks
	if(response != 0x00)
		return response;

	return 0; //normal return
}

//******************************************************************
//Function: to read a single block from SD card
//Arguments: none
//return: uint8_t; will be 0 if no error,
// otherwise the response byte will be sent
//******************************************************************
uint8_t SD_readSingleBlock(unsigned long startBlock)
{
	uint8_t response;
	uint8_t i, retry=0;

	response = SD_sendCommand(READ_SINGLE_BLOCK, startBlock<<9); //read a Block command
	//block address converted to starting address of 512 byte Block
	if(response != 0x00) //check for SD status: 0x00 - OK (No flags set)
	{
		TX_NEWLINE;
		uart_str_send_P(PSTR("read single block failed status\r"));
		TX_NEWLINE;
		return response;
	}

	sd_select();;

	while(spi_receive() != 0xfe) //wait for start block token 0xfe (0x11111110)
		if(retry++ > 0xfffe)
		{
			sd_release();
			TX_NEWLINE;
			uart_str_send_P(PSTR("read single block failed token\r"));
			TX_NEWLINE;
			return 1;
		} //return if time-out

	for(i=0; i<512; i++) //read 512 bytes
		buffer[i] = spi_receive();

	spi_receive(); //receive incoming CRC (16-bit), CRC is ignored here
	spi_receive();

	spi_receive(); //extra 8 clock pulses
	sd_release();;

	return 0;
}

//******************************************************************
//Function: to write to a single block of SD card
//Arguments: none
//return: uint8_t; will be 0 if no error,
// otherwise the response byte will be sent
//******************************************************************
uint8_t SD_writeSingleBlock(unsigned long startBlock)
{
	uint8_t response;
	uint8_t i, retry=0;

	response = SD_sendCommand(WRITE_SINGLE_BLOCK, startBlock<<9); //write a Block command
	if(response != 0x00) //check for SD status: 0x00 - OK (No flags set)
		return response;

	sd_select();;

	spi_transmit(0xfe);     //Send start block token 0xfe (0x11111110)

	for(i=0; i<512; i++)    //send 512 bytes data
		spi_transmit(buffer[i]);

	spi_transmit(0xff);     //transmit dummy CRC (16-bit), CRC is ignored here
	spi_transmit(0xff);

	response = spi_receive();

	if( (response & 0x1f) != 0x05) //response= 0xXXX0AAA1 ; AAA='010' - data accepted
	{                              //AAA='101'-data rejected due to CRC error
		sd_release();;              //AAA='110'-data rejected due to write error
		return response;
	}

	while(!spi_receive()) //wait for SD card to complete writing and get idle
		if(retry++ > 0xfffe){sd_release();; return 1;}

	sd_release();;
	spi_transmit(0xff);   //just spend 8 clock cycle delay before reasserting the CS line
	sd_select();;         //re-asserting the CS line to verify if card is still busy

	while(!spi_receive()) //wait for SD card to complete writing and get idle
		if(retry++ > 0xfffe){sd_release();; return 1;}
	sd_release();;

	return 0;
}

#ifndef FAT_TESTING_ONLY
//***************************************************************************
//Function: to read multiple blocks from SD card & send every block to UART
//Arguments: none
//return: uint8_t; will be 0 if no error,
// otherwise the response byte will be sent
//****************************************************************************
uint8_t SD_readMultipleBlock (unsigned long startBlock, unsigned long totalBlocks)
{
	uint8_t response;
	uint8_t i, retry=0;

	retry = 0;

	response = SD_sendCommand(READ_MULTIPLE_BLOCKS, startBlock <<9); //read a Block command
	//block address converted to starting address of 512 byte Block
	if(response != 0x00) //check for SD status: 0x00 - OK (No flags set)
		return response;

	sd_select();;

	while( totalBlocks )
	{
		retry = 0;
		while(spi_receive() != 0xfe) //wait for start block token 0xfe (0x11111110)
			if(retry++ > 0xfffe){sd_release();; return 1;} //return if time-out

		for(i=0; i<512; i++) //read 512 bytes
			buffer[i] = spi_receive();

		spi_receive(); //receive incoming CRC (16-bit), CRC is ignored here
		spi_receive();

		spi_receive(); //extra 8 cycles
		TX_NEWLINE;
		uart_str_send_P(PSTR(" --------- "));
		TX_NEWLINE;

		for(i=0; i<512; i++) //send the block to UART
		{
			if(buffer[i] == '~') break;
			transmitByte ( buffer[i] );
		}

		TX_NEWLINE;
		uart_str_send_P(PSTR(" --------- "));
		TX_NEWLINE;
		totalBlocks--;
	}

	SD_sendCommand(STOP_TRANSMISSION, 0); //command to stop transmission
	sd_release();;
	spi_receive(); //extra 8 clock pulses

	return 0;
}

//***************************************************************************
//Function: to receive data from UART and write to multiple blocks of SD card
//Arguments: none
//return: uint8_t; will be 0 if no error,
// otherwise the response byte will be sent
//****************************************************************************
uint8_t SD_writeMultipleBlock(unsigned long startBlock, unsigned long totalBlocks)
{
	uint8_t response, data;
	uint8_t i, retry=0;
	unsigned long blockCounter=0, size;

	response = SD_sendCommand(WRITE_MULTIPLE_BLOCKS, startBlock<<9); //write a Block command
	if(response != 0x00) //check for SD status: 0x00 - OK (No flags set)
		return response;

	sd_select();;

	TX_NEWLINE;
	uart_str_send_P(PSTR(" Enter text (End with ~): "));
	TX_NEWLINE;

	while( blockCounter < totalBlocks )
	{
		i=0;
		do
		{
			data = receiveByte();
			if(data == 0x08)   //'Back Space' key pressed
			{
				if(i != 0)
				{
					transmitByte(data);
					transmitByte(' ');
					transmitByte(data);
					i--;
					size--;
				}
				continue;
			}
			transmitByte(data);
			buffer[i++] = data;
			if(data == 0x0d)
			{
				transmitByte(0x0a);
				buffer[i++] = 0x0a;
			}
			if(i == 512) break;
		}while (data != '~');

		TX_NEWLINE;
		uart_str_send_P(PSTR(" ---- "));
		TX_NEWLINE;

		spi_transmit(0xfc); //Send start block token 0xfc (0x11111100)

		for(i=0; i<512; i++) //send 512 bytes data
			spi_transmit( buffer[i] );

		spi_transmit(0xff); //transmit dummy CRC (16-bit), CRC is ignored here
		spi_transmit(0xff);

		response = spi_receive();
		if( (response & 0x1f) != 0x05) //response= 0xXXX0AAA1 ; AAA='010' - data accepted
		{        	                  //AAA='101'-data rejected due to CRC error
			sd_release();;             //AAA='110'-data rejected due to write error
			return response;
		}

		while(!spi_receive()) //wait for SD card to complete writing and get idle
			if(retry++ > 0xfffe){sd_release();; return 1;}

		spi_receive(); //extra 8 bits
		blockCounter++;
	}

	spi_transmit(0xfd); //send 'stop transmission token'

	retry = 0;

	while(!spi_receive()) //wait for SD card to complete writing and get idle
		if(retry++ > 0xfffe){sd_release();; return 1;}

	sd_release();;
	spi_transmit(0xff); //just spend 8 clock cycle delay before reasserting the CS signal
	sd_select();; //re assertion of the CS signal is required to verify if card is still busy

	while(!spi_receive()) //wait for SD card to complete writing and get idle
		if(retry++ > 0xfffe){sd_release();; return 1;}
	sd_release();;

	return 0;
}
//*********************************************
#endif

//******** END ****** www.dharmanitech.com *****



