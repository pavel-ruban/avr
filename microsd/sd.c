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
#include <avr/sleep.h>
#include <inttypes.h>
#include <string.h>

#define MMC_C

//typedef struct
//{
//	unsigned busy:1;
//	unsigned CCS:1;
//	unsigned UHSIICS:1;
//	unsigned reserved_bits:4;
//	unsigned S18A:1;
//	unsigned 3_5__3_6:1;
//	unsigned 3_4__3_5:1;
//	unsigned 3_3__3_4:1;
//	unsigned 3_2__3_3:1;
//	unsigned 3_1__3_2:1;
//	unsigned 3_0__3_1:1;
//	unsigned 2_9__3_0:1;
//	unsigned 2_8__2_9:1;
//	unsigned 2_7__2_8:1;
//	unsigned reserved_bits:15;
//
//} OCR;

//typedef struct
//{
//	unsigned idle_state:1;
//	unsigned erase_reset:1;
//	unsigned illegal_command:1;
//	unsigned com_crc_error:1;
//	unsigned erase_seq_error:1;
//	unsigned address_error:1;
//	unsigned param_error:1;
//	unsigned start_bit:1;
//} R1;

//typedef struct
//{
//	uint8_t *r1;
//	uint32_t ocr;
//} R3;
//
//typedef struct
//{
//	uint8_t echo_back;
//	unsigned voltage_accepted:4;
//	uint16_t reserved_bits;
//	unsigned cmd_ver:4;
//	uint8_t *r1;
//} R7;

//uint8_t mmc_sbuf[512];	///< 512 byte sector buffer in internal RAM
//uint8_t mmc_scratch[32]; 	///< 32 byte Scratch buffer for CSD/CID/ ops
//uint8_t mmc_r1, mmc_status;
//uint8_t mmc_card_type;
//uint8_t r1;
//R3 r3;
//R7 r7;

void mmc_print_r1(uint8_t r1)
{
	uint8_t buffer[400];
	sprintf_P(
		buffer,
		PSTR(
			"R1 response debug information:\n\r"
			"idle state: %u\n\r"
			"erase_reset: %u\n\r"
			"illegal_command: %u\n\r"
			"com_crc_error: %u\n\r"
			"erase_seq_error: %u\n\r"
			"address_error: %u\n\r"
			"param_error: %u\n\r"
			"start_bit: %u\n\r"
		),
		r1 & 0b00000001,
		(r1 & 0b00000010) >> 1,
		(r1 & 0b00000100) >> 2,
		(r1 & 0b00001000) >> 3,
		(r1 & 0b00010000) >> 4,
		(r1 & 0b00100000) >> 5,
		(r1 & 0b01000000) >> 6,
		(r1 & 0b10000000) >> 7
	);
	uart_str_send(buffer);
}
//
//void mmc_print_r3()
//{
//	sprintf_P(
//		mmc_sbuf,
//		PSTR(
//			"R3 response debug information:\n\r"
//			"=============R1 section==========\n\r"
//			"idle state: %u\n\r"
//			"erase_reset: %u\n\r"
//			"illegal_command: %u\n\r"
//			"com_crc_error: %u\n\r"
//			"erase_seq_error: %u\n\r"
//			"address_error: %u\n\r"
//			"param_error: %u\n\r"
//			"start_bit: %u\n\r"
//			"=============R3 OCR section==========\n\r"
//			"OCR %#032x\n\r"
//		),
//		r1 & 0b00000001,
//		(r1 & 0b00000010) >> 1,
//		(r1 & 0b00000100) >> 2,
//		(r1 & 0b00001000) >> 3,
//		(r1 & 0b00010000) >> 4,
//		(r1 & 0b00100000) >> 5,
//		(r1 & 0b01000000) >> 6,
//		(r1 & 0b10000000) >> 7,
//		r3.ocr
//	);
//	uart_str_send(mmc_sbuf);
//}
//
//void mmc_print_r7()
//{
//	sprintf_P(
//		mmc_sbuf,
//		PSTR(
//			"R7 response debug information:\n\r"
//			"=============R1 section==========\n\r"
//			"idle state: %u\n\r"
//			"erase_reset: %u\n\r"
//			"illegal_command: %u\n\r"
//			"com_crc_error: %u\n\r"
//			"erase_seq_error: %u\n\r"
//			"address_error: %u\n\r"
//			"param_error: %u\n\r"
//			"start_bit: %u\n\r"
//			"=============R7 section==========\n\r"
//			"cmd_ver: %#04x\n\r"
//			"reserved_bits: %#016x\n\r"
//			"voltage_accepted: %#04x\n\r"
//			"echo_back: %#08x\n\r"
//		),
//		r1 & 0b00000001,
//		(r1 & 0b00000010) >> 1,
//		(r1 & 0b00000100) >> 2,
//		(r1 & 0b00001000) >> 3,
//		(r1 & 0b00010000) >> 4,
//		(r1 & 0b00100000) >> 5,
//		(r1 & 0b01000000) >> 6,
//		(r1 & 0b10000000) >> 7,
//		r7.cmd_ver,
//		r7.reserved_bits,
//		r7.voltage_accepted,
//		r7.echo_back
//	);
//	uart_str_send(mmc_sbuf);
//}

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
	uint8_t byte;
	uint8_t response_type = MMC_RESPONSE_TYPE_UNDEFINED;

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
		// CMD8.
		case MMC_SEND_IF_COND:
			spi_transmit(0x87);
			response_type = MMC_RESPONSE_TYPE_R7;
			break;
		// CMD58.
		case MMC_READ_OCR:
			spi_transmit(0xff);
			response_type = MMC_RESPONSE_TYPE_R3;
			break;
		default:
			spi_transmit(0xff);
			break;
	}

	retry = 0;
	byte = 0xFF;

	while ((byte = spi_transmit(0xff)) == 0xff)
	{
		++retry;
		if (retry > 10) break;
	}

	if (retry < 10)
	{
		switch (response_type)
		{
			case MMC_RESPONSE_TYPE_R3:
	//			r3.ocr = 0;
	//			for (int i = 0; i < 4; ++i)
	//			{
	//				r3.ocr |= ((uint32_t) spi_transmit(0xff)) << (24 - i * 8);
	//			}
				return byte;

			case MMC_RESPONSE_TYPE_R7:
	//			byte = spi_transmit(0xff);
	//	//		sprintf_P(mmc_scratch, PSTR("parsing r7 byte %#08x\n\r"), byte);
	//	//		uart_str_send(mmc_scratch);
	//			r7.cmd_ver = byte >> 4;
	//			r7.reserved_bits = (uint16_t) byte << 12;
	//			byte = spi_transmit(0xff);
	//	//		sprintf_P(mmc_scratch, PSTR("parsing r7 byte %#08x\n\r"), byte);
	//	//		uart_str_send(mmc_scratch);
	//			r7.reserved_bits |= ((uint16_t) byte << 4);
	//			byte = spi_transmit(0xff);
	//	//		sprintf_P(mmc_scratch, PSTR("parsing r7 byte %#08x\n\r"), byte);
	//	//		uart_str_send(mmc_scratch);
	//			r7.reserved_bits |= 0x0F & ((uint16_t) byte >> 4);
	//			r7.voltage_accepted = 0x0F & byte;
	//			byte = spi_transmit(0xff);
	//	//		sprintf_P(mmc_scratch, PSTR("parsing r7 byte %#08x\n\r"), byte);
	//	//		uart_str_send(mmc_scratch);
	//			r7.echo_back = byte;
				return byte;
		}

	}

	return byte;
}

/**
*	Perform a Hardware then software reset of the MMC.
*
*	@return 00 if reset successful
*	@return FF if reset unsuccessful
**/
uint8_t mmc_init()
{
	uint16_t retry, i;
	uint8_t r1, mmc_status = MMC_KO;

	sd_release();

	/* start off with 80 bits of high data with card deselected */
	for(i = 0; i < 10; i++)
		r1 = spi_transmit(0xff);

	sd_select();
	//_delay_ms(1000);
	//uart_str_send_P(PSTR("after delay\n\r"));

	retry = 0;

	/* now send CMD0 - go to idle state, try up to 100 times */
	retry = 10;
	while (r1 != 0x01 && retry)
	{
		r1 = mmc_command(MMC_GO_IDLE_STATE, 0x00);  /* software reset command */
		--retry;
	}

	//mmc_print_r1(r1);
	sd_release();
//	sprintf_P(mmc_scratch, PSTR("r1 debug %#08x\n\r"), r1);
	//uart_str_send(mmc_scratch);
//	mmc_print_r1();

	if (r1 != 0x01)
	{
		spi_transmit(0xff);
		uart_str_send_P(PSTR("mmc init go idle state failed 1\n\r"));
		return -1;  // MMC Not detected
	}

	// Send CMD8. Check for SDC ver.2+ card.
	r1 = mmc_command(MMC_SEND_IF_COND, 0x01AA);

	if (r1 == 0x01)
	{
		// Card type is SDC ver.2+
		// Check the card support Vdd 2.7-3.6V
		uart_str_send_P(PSTR("SDCv2+\n\r"));
//		uart_str_send_P(PSTR("start checking CMD8...\n\r"));

	//	mmc_print_r7();

	//	sprintf_P(mmc_scratch, PSTR("CMD8 voltage respoinse: %#01x\n\r"), r7.voltage_accepted);
	//	uart_str_send(mmc_scratch);

		uint8_t rn;
		for (i = 0; i < 3; ++i)
			rn = spi_transmit(0xff);

		sd_release();
		// Check the card support Vdd 2.7-3.6V.
		if ((0x0F & rn) == 0x1)
		{
			mmc_command(MMC_APP_CMD, 0x0);
			sd_release();

	//		sprintf_P(mmc_scratch, PSTR("CMD55 r1:\n\r"));
	//		uart_str_send(mmc_scratch);
	//		mmc_print_r1();

			r1 = mmc_command(MMC_CMD_SD_SEND_OP_COND, (uint32_t) 1 << 30);
			sd_release();

	//		sprintf_P(mmc_scratch, PSTR("ACMD41 r1:\n\r"));
	//		uart_str_send(mmc_scratch);
	//		mmc_print_r1();

			retry = 0x7FF;
			while (r1 && retry--)
			{
				mmc_command(MMC_APP_CMD, 0x0);
				sd_release();

	//			sprintf_P(mmc_scratch, PSTR("nCMD55 r1:\n\r"));
	//			uart_str_send(mmc_scratch);
	//			mmc_print_r1();
				r1 = mmc_command(MMC_CMD_SD_SEND_OP_COND, 0x0);
				sd_release();

	//			sprintf_P(mmc_scratch, PSTR("nACMD41 r1:\n\r"));
	//			uart_str_send(mmc_scratch);
	//			mmc_print_r1();
			}

			if (r1 == 0x00)
			{
				mmc_status = MMC_OK;
//				sprintf_P(mmc_scratch, PSTR("Card is initialized\n\r"));
//				uart_str_send(mmc_scratch);

				retry = 0x7FF;
				do
				{
					r1 = mmc_command(MMC_READ_OCR, 0x0);
					sd_release();
				}
				while (r1 && retry--);

				if (r1 == 0x00)
				{
	//				mmc_print_r3();
					rn = spi_transmit(0xff);
					if (1 & (rn >> 6))
					{
					//	mmc_card_type = MMC_SDHC;
						uart_str_send_P(PSTR("SDHC\n\r"));
					}
					else {
					//	mmc_card_type = MMC_SDCv2x;
	//					uart_str_send_P(PSTR("SDSC card type\n\r"));
						uart_str_send_P(PSTR("SDSC\n\r"));
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
			sd_release();
			r1 = mmc_command(MMC_CMD_SD_SEND_OP_COND, 0x00);
			sd_release();
		} while (r1 && retry--);

		// Check for SDC v1.0 card
		if (r1 == 0x00)
		{
			// Card is SDC v1.0
			mmc_status = MMC_OK;
			//mmc_card_type = MMC_SDCv1x;
			uart_str_send_P(PSTR("SDCv1.0\n\r"));
		}
		else {
			retry = 0x7FF;
			do {
				r1 = mmc_command(MMC_SEND_OP_COND, 0x00);
				sd_release();
			} while (r1 && retry--);

			if (0x00 == r1)
			{
				// Card is MMC
				mmc_status = MMC_OK;
			//	mmc_card_type = MMC_MMC;
				uart_str_send_P(PSTR("MMC\n\r"));
			}
		}


	}

	if (mmc_status != MMC_OK){
		uart_str_send_P(PSTR("mmc init failed 2\n\r"));
		return -2;  // Init Fail
	}

	//uart_str_send_P(PSTR("mmc init ok\n\r"));
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
//uint8_t MMCGet(void)
//{
//	uint8_t i, retval;
//
//	for (i=0; i < 8; i++) {
//		retval = spi_transmit(0xff);
//		if (retval != 0xff) return retval;
//	}
//
//	return retval;
//}

/***************************************************************************
 *	MMC_Read
 *	Read one sector from the memory card into mmc_sbuf[]
 *	Note this assumes block length of 512 Bytes
 *	@Param		<lba> the number of the sector to read
 *	@Return		Status from the read call.
 ***************************************************************************/
uint8_t mmc_read(uint32_t lba, uint8_t *buffer)
{
	uint8_t byte, r1, data;
	uint16_t sec,i;
	uint16_t word;
#undef USE_MMC_CACHE
#ifdef USE_MMC_CACHE
//	static uint32_t last_read_lba=0xffffffff; // remember last sector for caching purposes
//
//	if (last_read_lba == lba){ // exit ASSUMING BUFFER HAS NOT BEEN ALTERED
//		return 0x00; // OK
//	}
#endif

	spi_transmit(0xff);

	word = mmc_command(MMC_SEND_STATUS, 0x00); // check card status
	word = (word << 8) + spi_transmit(0xff);
	sd_release();

//	lba <<= 9; // * 512
//	pz = (lba>>8 & 0xff);
//	py = (lba>>16 & 0xff);
//	px = (lba>>24 & 0xff);

	spi_transmit(0xff);
	r1 = mmc_command(MMC_READ_SINGLE_BLOCK, lba);

	//TODO:  if i get a byte == 0x05 should I resend?

	if (r1 == 0x0) { // then everything okay
		i = 500; // wait up to 255 bytes for response
		data = spi_transmit(0xff);

		while ((data != MMC_STARTBLOCK_READ) && (i--)) {
			data = spi_transmit(0xff);
		}

		if (i != 0) { // read okay. start grabbing the data
		//	uart_str_send_P(PSTR("sd: read okay\n\r"));
			for(sec = 0; sec < 512; sec++)
				buffer[sec] = spi_transmit(0xff);

			spi_transmit(0xff); // flush MMC CRC
			spi_transmit(0xff); //
			spi_transmit(0xff); //Clear SPI
			spi_transmit(0xff); //
		}else{
			r1 = 0xff;// signal MMC Coms error
		}

	}

	sd_release();
	spi_transmit(0xff); // Clear SPI

#ifdef USE_MMC_CACHE
//	if (mmc_r1 == 0){
//		last_read_lba = lba;// save for next time.
//	}
#endif
	return r1;
}
