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
// ***** HEADER FILE : SD_routines.h ******
//**************************************************
#ifndef _SD_ROUTINES_H_
#define _SD_ROUTINES_H_

#define FAT_TESTING_ONLY

#define GO_IDLE_STATE            0
#define SEND_OP_COND     	    1
#define SEND_CSD                 9
#define STOP_TRANSMISSION        12
#define SEND_STATUS              13
#define SET_BLOCK_LEN            16
#define READ_SINGLE_BLOCK        17
#define READ_MULTIPLE_BLOCKS     18
#define WRITE_SINGLE_BLOCK       24
#define WRITE_MULTIPLE_BLOCKS    25
#define ERASE_BLOCK_START_ADDR   32
#define ERASE_BLOCK_END_ADDR     33
#define ERASE_SELECTED_BLOCKS    38
#define CRC_ON_OFF               59

#define ON     1
#define OFF    0

extern volatile unsigned long startBlock;
extern volatile unsigned long totalBlocks;
extern uint8_t buffer[48];

uint8_t SD_init(void);
uint8_t SD_sendCommand(uint8_t cmd, unsigned long arg);
uint8_t SD_readSingleBlock(unsigned long startBlock);
uint8_t SD_writeSingleBlock(unsigned long startBlock);
uint8_t SD_readMultipleBlock (unsigned long startBlock, unsigned long totalBlocks);
uint8_t SD_writeMultipleBlock(unsigned long startBlock, unsigned long totalBlocks);
uint8_t SD_erase (unsigned long startBlock, unsigned long totalBlocks);

#endif
