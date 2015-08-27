/**
	@file	mmc.h
	@brief	MultiMedia Card low level Functions
	@author	Nick Lott brokentoaster@users.sf.net
	@date	September 2004

	$Id: mmc.h,v 1.7 2007/06/04 15:12:17 brokentoaster Exp $

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

// MMC commands
#define MMC_GO_IDLE_STATE		0
#define MMC_SEND_OP_COND		1
#define MMC_SEND_CSD			9
#define MMC_SEND_IF_COND		8
#define MMC_SEND_CID			10
#define MMC_SEND_STATUS			13
#define MMC_SET_BLOCKLEN		16
#define MMC_READ_SINGLE_BLOCK		17
#define MMC_WRITE_BLOCK			24
#define MMC_PROGRAM_CSD			27
#define MMC_SET_WRITE_PROT		28
#define MMC_CLR_WRITE_PROT		29
#define MMC_SEND_WRITE_PROT		30
#define MMC_TAG_SECTOR_START		32
#define MMC_TAG_SECTOR_END		33
#define MMC_UNTAG_SECTOR		34
#define MMC_TAG_ERASE_GROUP_START 	35
#define MMC_TAG_ERARE_GROUP_END		36
#define MMC_UNTAG_ERASE_GROUP		37
#define MMC_ERASE			38
#define MMC_READ_OCR			58
#define MMC_CMD_SD_SEND_OP_COND		41
#define MMC_CRC_ON_OFF			59

// R1 Response bit-defines
#define MMC_R1_BUSY				0x80
#define MMC_R1_PARAMETER			0x40
#define MMC_R1_ADDRESS				0x20
#define MMC_R1_ERASE_SEQ			0x10
#define MMC_R1_COM_CRC				0x08
#define MMC_R1_ILLEGAL_COM			0x04
#define MMC_R1_ERASE_RESET			0x02
#define MMC_R1_IDLE_STATE			0x01

// Data Start tokens
#define MMC_STARTBLOCK_READ			0xFE
#define MMC_STARTBLOCK_WRITE			0xFE
#define MMC_STARTBLOCK_MWRITE			0xFC

// Data Stop tokens
#define MMC_STOPTRAN_WRITE			0xFD

// Data Error Token values
#define MMC_DE_MASK				0x1F
#define MMC_DE_ERROR				0x01
#define MMC_DE_CC_ERROR				0x02
#define MMC_DE_ECC_FAIL				0x04
#define MMC_DE_OUT_OF_RANGE			0x04
#define MMC_DE_CARD_LOCKED			0x04

// Data Response Token values
#define MMC_DR_MASK				0x1F
#define MMC_DR_ACCEPT				0x05
#define MMC_DR_REJECT_CRC			0x0B
#define MMC_DR_REJECT_WRITE_ERROR		0x0D

#define MMC_CARD 1
#define MMC_SDCv1x 2
#define MMC_SDCv2x 3
#define MMC_MMC 4
#define MMC_SDHC 5
#define MMC_OK 0
#define MMC_KO 1

#ifndef MMC_C

extern uint8_t mmc_sbuf[512];///< 512 byte sector buffer in internal RAM
extern uint8_t mmc_scratch[32]; ///< 32 byte Scratch buffer for CSD/CID/ ops

uint8_t mmc_init(void);				// init mmc
uint16_t mmc_check(void);				// check status
uint8_t mmc_read(uint32_t lba);
uint8_t MMCGet(void);

#endif
