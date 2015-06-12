#define F_CPU 8000000UL

#include <avr/io.h>
#include <utils.h>
#include <spi.h>
#include <mfrc522.h>
#include <util/delay.h>


char *term_var_info;
char term_var_info_data[100];
uint8_t byte;
uint8_t str[MAX_LEN];


void main_init()
{
	term_var_info = term_var_info_data;

	_delay_ms(50);
	spi_init();
	mfrc522_init();
	_delay_ms(1500);
}

void main()
{
	//check version of the reader
	byte = mfrc522_read(VersionReg);
	/*
	if(byte == 0x92)
	{
		*(term_var_info + 0) = 'v';
		*(term_var_info + 1) = '2';
		*(term_var_info + 2) = '\n';
		*(term_var_info + 3) = '\r';
	}else if(byte == 0x91 || byte==0x90)
	{
		*(term_var_info + 0) = 'v';
		*(term_var_info + 1) = '1';
		*(term_var_info + 2) = '\n';
		*(term_var_info + 3) = '\r';
	}else
	{
		*(term_var_info + 0) = '-';
		*(term_var_info + 1) = '1';
		*(term_var_info + 2) = '\n';
		*(term_var_info + 3) = '\r';
	}
	*(term_var_info + 4) = '-';
	*(term_var_info + 5) = '-';
	*(term_var_info + 6) = '-';
	*(term_var_info + 7) = '\n';
	*(term_var_info + 8) = '\r';
*/

	byte = mfrc522_request(PICC_REQALL,str);

	if(byte == CARD_FOUND)
	{
		byte = mfrc522_get_card_serial(str);
		if(byte == CARD_FOUND)
		{
			byte = 0;
			while (byte < 5)
			{
				*(term_var_info +  byte) = str[byte];
				++byte;
			}

			*(term_var_info + byte) = 0x00;

			PORTC ^= 1 << 1;
			PORTD	^= 1 << 4;
		}
		else {
			*(term_var_info) = 0x00;
		}
	}
	else {
		*(term_var_info) = 0x00;
	}
}
