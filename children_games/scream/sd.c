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
#include <avr/interrupt.h>
#include "spi.h"
#include "sd.h"
#include "tools.h"
#include <util/delay.h>
#include <stdio.h>

#define MMC_C

uint8_t mmc_sbuf[512];	///< 512 byte sector buffer in internal RAM
uint8_t mmc_scratch[32]; 	///< 32 byte Scratch buffer for CSD/CID/ ops
uint8_t mmc_r1, mmc_status;
uint8_t mmc_card_type;

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
uint8_t mmc_command(unsigned char command, uint32_t arg)
{
	uint8_t retry;

	sd_select();

	spi_transmit(0xff);
	spi_transmit(command | 0x40);
	spi_transmit(arg >> 24);
	spi_transmit(arg >> 16);
	spi_transmit(arg >> 8);
	spi_transmit(arg);

	switch(command)
	{
		case MMC_GO_IDLE_STATE:
			spi_transmit(0x95);
			break;
		case MMC_SEND_IF_COND:
			spi_transmit(0x87);
			break;
		default:
			spi_transmit(0xff);
			break;
	}

	retry = 0;
	mmc_r1 = 0xFF;

	while ((mmc_r1 = spi_transmit(0xff)) == 0xff)
	{
		++retry;
		if (retry > 10) break;
	}

	sd_release();

	return mmc_r1;
}

/**
*	Perform a Hardware then software reset of the MMC.
*
*	@return 00 if reset successful
*	@return FF if reset unsuccessful
**/
uint8_t mmc_init(void)
{
	uint16_t retry, i;

	sd_release();

	/* start off with 80 bits of high data with card deselected */
	for(i = 0; i < 10; i++)
		mmc_r1 = spi_transmit(0xff);

	sd_select();

	mmc_card_type == MMC_CARD;
	mmc_status = MMC_KO;

	retry = 0;

	/* now send CMD0 - go to idle state, try up to 100 times */
	mmc_r1 = mmc_command(MMC_GO_IDLE_STATE, 0x00);  /* software reset command */

	if (mmc_r1 != 0x01)
	{
		sd_release();
		spi_transmit(0xff);
		uart_str_send_P(PSTR("mmc init go idle state failed 1\n\r"));
		return -1;  // MMC Not detected
	}

	// Send CMD8. Check for SDC ver.2+ card.
	mmc_r1 = mmc_command(MMC_SEND_IF_COND, 0x01AA);

	if (mmc_r1 == 0x01)
	{
		// Card type is SDC ver.2+
		// Check the card support Vdd 2.7-3.6V
		uart_str_send_P(PSTR("Card type is SDC ver.2+\n\r"));

		// @todo read r7 & do the check.
		// Check the card support Vdd 2.7-3.6V.
		if (1)
		{
			retry = 0x7FF;
			do
			{
				mmc_command(55, 0x0);
				mmc_r1 = mmc_command(41, (unsigned long) 1 << 30);
			}
			while (mmc_r1 && retry--);

			if (mmc_r1 == 0x00)
			{
				retry = 0x7FF;
				do
				{
					mmc_r1 = mmc_command(MMC_READ_OCR, 0x0);
				}
				while (mmc_r1 && retry--);

				if (mmc_r1 == 0x00)
				{
					if (1)
					{
						mmc_card_type = MMC_SDHC;
					}
					else {
						mmc_card_type = MMC_SDCv2x;
					}
				}

			}
		}
	}
	else
	{
		// Card is MMC or SDC v1.x
		retry = 0x7FF;
		//  ACMD41
		do {
			mmc_command(55, 0x00);
			mmc_r1 = mmc_command(MMC_CMD_SD_SEND_OP_COND, 0x00);
		} while (mmc_r1 && retry--);

		// Check for SDC v1.0 card
		if (mmc_r1 == 0x00)
		{
			// Card is SDC v1.0
			mmc_status = MMC_OK;
			mmc_card_type = MMC_SDCv1x;
			uart_str_send_P(PSTR("Card type is SDC ver. 1.0\n\r"));
		}
		else {
			retry = 0x7FF;
			do {
				mmc_r1 = mmc_command(MMC_SEND_OP_COND, 0x00);
			} while (mmc_r1 && retry--);

			if (0x00 == mmc_r1)
			{
				// Card is MMC
				mmc_status = MMC_OK;
				mmc_card_type = MMC_MMC;
				uart_str_send_P(PSTR("Card type is MMC\n\r"));
			}
		}


	}

	if (mmc_status != MMC_OK){
		uart_str_send_P(PSTR("mmc init failed 2\n\r"));
		return -2;  // Init Fail
	}

	uart_str_send_P(PSTR("mmc init ok\n\r"));
	return 0;
}

/**
 *	Send the get status command to the MMC and returns the
 *	result
 *
 *	@return 2 byte value from MMC status register
 **/
uint16_t mmc_check(void)
{
	uint16_t word;

	word = mmc_command(MMC_SEND_STATUS, 0x00); // check card ststus
	sd_select();
	word = (word << 8) + spi_transmit(0xff); // NOTE spi_transmit and not MMCGET!!!

	sd_release();
	spi_transmit(0xff); // Clear SPI

	return word;
}

/**
*	Retrieve data from then MMC.
*	Pings the card until it gets a non-0xff value
*
*  	@return first byte of data usualy a data token.
**/
uint8_t MMCGet(void)
{
	uint8_t i, retval;

	for (i=0; i < 8; i++) {
		retval = spi_transmit(0xff);
		if (retval != 0xff) return retval;
	}

	return retval;
}

/***************************************************************************
 *	MMC_Read
 *	Read one sector from the memory card into mmc_sbuf[]
 *	Note this assumes block length of 512 Bytes
 *	@Param		<lba> the number of the sector to read
 *	@Return		Status from the read call.
 ***************************************************************************/
uint8_t mmc_read(uint32_t lba)
{
	uint8_t byte,data,px,py,pz;
	uint16_t sec,i;
	uint16_t word;
#ifdef USE_MMC_CACHE
//	static uint32_t last_read_lba=0xffffffff; // remember last sector for caching purposes
//
//	if (last_read_lba == lba){ // exit ASSUMING BUFFER HAS NOT BEEN ALTERED
//		return 0x00; // OK
//	}
#endif

	spi_transmit(0xff);
	sd_select();
	word = mmc_command(MMC_SEND_STATUS, 0x00); // check card status
	word = (word << 8) + spi_transmit(0xff);

	lba <<=9; // * 512
	pz = (lba>>8 & 0xff);
	py =( lba>>16 & 0xff);
	px =( lba>>24 & 0xff);

	spi_transmit(0xff);
	sd_select();
	mmc_r1 = mmc_command(MMC_READ_SINGLE_BLOCK, lba);

	//TODO:  if i get a byte == 0x05 should I resend?

	if (mmc_r1 == 0x0) { // then everything okay
		i = 500; // wait up to 255 bytes for response
		data = spi_transmit(0xff);

		while ((data != MMC_STARTBLOCK_READ) && (i--)) {
			data = spi_transmit(0xff);
		}

		if (i != 0) { // read okay. start grabbing the data
			for(sec = 0; sec < 512; sec++)
				mmc_sbuf[sec] = spi_transmit(0xff);

			spi_transmit(0xff); // flush MMC CRC
			spi_transmit(0xff); //
			spi_transmit(0xff); //Clear SPI
			spi_transmit(0xff); //
		}else{
			mmc_r1 = 0xff;// signal MMC Coms error
		}

	}

	sd_release();
	spi_transmit(0xff); // Clear SPI

#ifdef USE_MMC_CACHE
//	if (mmc_r1 == 0){
//		last_read_lba = lba;// save for next time.
//	}
#endif
	return mmc_r1;
}
