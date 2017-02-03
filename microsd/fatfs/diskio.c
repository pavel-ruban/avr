/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include <inttypes.h>
#include <string.h>
#include "../sd.h"
#include <avr/pgmspace.h>
#include "../tools.h"
#include <stdio.h>

/* Definitions of physical drive number for each drive */
#define ATA		2	/* Example: Map ATA harddisk to physical drive 0 */
#define MMC		0	/* Example: Map MMC/SD card to physical drive 1 */
#define USB		1	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	uint8_t result;

	switch (pdrv) {
	case ATA :
	//	result = ATA_disk_status();

		// translate the reslut code here

		return STA_NOINIT;

	case MMC :
		result = (uint8_t) (mmc_check() >> 8);
		uart_str_send_P(PSTR("s\n\r"));
		if (result) stat = STA_NOINIT;
		else stat = 0;


		// translate the reslut code here

		return stat;

	case USB :
		//result = USB_disk_status();

		// translate the reslut code here

		return STA_NOINIT;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;


	//uart_str_send_P(PSTR("pre disk init\n\r"));
	switch (pdrv) {
	case ATA :
	//	result = ATA_disk_initialize();

		// translate the reslut code here

		return STA_NOINIT;

	case MMC :
	{
	//	result = MMC_disk_initialize();

		// translate the reslut code here
		uint8_t r1 =  mmc_init();
	//	uart_str_send_P(PSTR("disk init\n\r"));

		uart_str_send_P(PSTR("i\n\r"));
		if (!r1) stat = 0x0;
		else stat = STA_NOINIT;

		return stat;
	}
	case USB :
	//	result = USB_disk_initialize();

		// translate the reslut code here

		return STA_NOINIT;
	}
	return STA_NOINIT;
}


//void int2str(uint8_t integer, char *chars[4])
//{
//	uint8_t length = integer > 99 ? 3 : (integer > 9 ? 2 : 1);
//	{
//		uint8_t i = 0;
//		uint8_t rank = 1;
//
//		for (i; i < length; ++i)
//		{
//			rank = integer % 10;
//			chars[length == 3 ? 2 - i : (length == 2 ? 1 - i : 0)] = rank;
//			integer /= 10;
//		}
//	}
//
//	chars[length] = 0;
//
//	return chars;
//}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

	//uart_str_send_P(PSTR("pre disk read\n\r"));
	// translate the arguments here
	//uint8_t retry = 3;
	uint8_t r1 = 1;

	//uart_str_send_P(PSTR("r\n\r"));
	char line[20];
	//while (r1 && retry)
	//{
	//	mmc_init();
	 	r1 = mmc_read(sector, buff);
		sprintf_P(line, PSTR("r1 er: %d c:%d\n\r"), r1, count);
		uart_str_send(line);
	//	--retry;
	//}

	//uart_str_send_P(PSTR("post disk read\n\r"));
	//uart_str_send_cnt(buff, count);

	if (!r1) {
		res = RES_OK;
	}
	else {
	//	uart_str_send_P(PSTR("error on read side\n\r"));
		res = RES_NOTRDY;
	}

	return res;
//	switch (pdrv) {
//	case ATA :
//		// translate the arguments here
//
//	//	result = ATA_disk_read(buff, sector, count);
//
//		// translate the reslut code here
//
//		//return RES_PARERR;
//
//	case USB :
//		// translate the arguments here
//
//	//	result = USB_disk_read(buff, sector, count);
//
//		// translate the reslut code here
//
//		//return RES_PARERR;
//	case MMC:
//	{
//	}
//	}
//
//	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case ATA :
		// translate the arguments here

		result = ATA_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case MMC :
		// translate the arguments here

		result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case USB :
		// translate the arguments here

		result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case ATA :

		// Process of the command for the ATA drive

		return res;

	case MMC :

		// Process of the command for the MMC/SD card

		return res;

	case USB :

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}
#endif
