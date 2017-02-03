#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "config.h"
#include <avr/cpufunc.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "tools.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include "screen.h"
#include <avr/sleep.h>
//#include "fatfs/ff.h"

#include "spi.h"
#include "lcd.h"
//#include "sd.h"

//uint16_t adc_val;
//uint8_t play;

//extern uint8_t mmc_sbuf[512];
//static uint16_t pos;
//static uint32_t sector;

void audio_routine();
//void adc_update();
//char buff[50];
//uint32_t sector;
//uint16_t pos;
//extern void pwm_init();
//extern uint8_t SD_INIT;

//uint8_t mmc_buf[512];

//void main_sd_init()
//{
//	if (!(SD_STATUS_PIN & (1 << SD_STATUS)))
//	{
//		uart_str_send_P(PSTR("sd: starting initialize...\n\r"));
//		mmc_init();
//		//mmc_init();
//	}
//}

//void mmc_print_r1(uint8_t);
//uint16_t mmc_check();
void c_load()
{
//	if (pos > 511)
//	{
//		//uart_str_send_P(PSTR("reading...\n\r"));
//		uint8_t r1 = mmc_read(sector, mmc_buf);
//		if (!r1)
//		{
//		//	uart_str_send_P(PSTR("read ok\n\r"));
//		//	uart_str_send_cnt(mmc_buf, 512);
//			++sector;
//			pos = 0;
//		}
//	}
}

void c_handle()
{
	uart_str_send_P(PSTR("handler context...\n\r"));
	//lcd_cmd(LCD_CMD_HOME);
	lcd_init();
	lcd_cmd(LCD_CMD_SET_CGRAM);
	lcd_cmd(LCD_CMD_CURSOR_DISPLAY_CONTROL | LCD_DISPLAY_ON | LCD_CURSOR_ON | LCD_BLINK_ON);
//	if (SD_INIT)
//	{
//		uart_str_send_P(PSTR("sd: reinit\n\r"));
//		uint8_t r1 = mmc_init();
//		if (!r1) SD_INIT = 0;
//	}

	//uart_str_send_P(PSTR("handler:iterating...\n\r"));
//	uint8_t pin = (PINC & (1 << PC2)) >> PC2;
//	char line[20];
//	sprintf_P(line, PSTR("pinc2 sate %d\n\r"), pin);
//	uart_str_send(line);
	//uart_str_send_cnt(mmc_sbuf, 512);
	//play = 1;
	//audio_routine();
	//mmc_read(sector++);
	//uart_str_send_cnt(mmc_sbuf, 512);
	//audio_routine();
	//	uart_str_send_P(PSTR("uart: check environment state, it's ok, sleep...\n\r"));
	//uart_str_send_cnt(mmc_sbuf, 512);

//	{
//		uint8_t buffer[512];
//		mmc_read(sector, buffer);
//		uart_str_send_cnt(buffer, 512);
//	}
//	uint16_t status = mmc_check();
//	mmc_print_r1((uint8_t) (status >> 8));
}

void main_init()
{
	//sector = 0;
	//pos = 512;
	//SD_INIT = 1;

	lcd_init();
	uart_str_send_P(PSTR("reset routine...\n\r"));
	spi_init();
	uart_str_send_P(PSTR("initializing lcd...\n\r"));

	lcd_cmd(LCD_CMD_FUNCTION_SET);
	lcd_cmd(LCD_CMD_HOME);
	lcd_cmd(LCD_CMD_CURSOR_DISPLAY_CONTROL | LCD_DISPLAY_ON | LCD_CURSOR_ON | LCD_BLINK_ON);
	lcd_cmd(LCD_CMD_CLEAR);
	lcd_cmd(LCD_CMD_ENTRY_MODE);
	lcd_cmd(LCD_CMD_SET_CGRAM);

	//main_sd_init();
	//uart_str_send_P(PSTR("sd: initialize done\n\r"));
	//pwm_init();

//FATFS FatFs;
//FIL fil;       /* File object */
//uint8_t br;
//FRESULT fr;    /* FatFs return code */
//	char line[12]; /* Line buffer */
//	/* Register work area to the default drive */
//
//	//uart_str_send_P(PSTR("o\n\r"));
//	f_mount(&FatFs, "", 0);
//
//	/* Open a text file */
//	fr = f_open(&fil, "message.txt", FA_READ);
//	sprintf_P(line, PSTR("fr %d\n\r"), fr);
//	uart_str_send(line);
//
//	/* Read all lines and display it */
//	//uart_str_send_P(PSTR("printing file...\n\r"));
//	if (!fr)
//	{
//		while (!fr)
//		{
//			fr = f_read(&fil, line, sizeof line, (UINT *) &br);
//			if (!br) break;
//
//			uart_str_send_cnt(line, br);
//		}
//
//		/* Close the file */
//		f_close(&fil);
//	}
}


//void adc_update()
//{
//	//uart_str_send_P(PSTR("Hi there! What's up?\n\r"));
//	//ADCSRA |= (1 << ADSC); // Start conversion
//	while (ADCSRA & (1 << ADSC)); // wait for conversion to complete
//
//	ADMUX =  (1 << REFS0) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0);
//	sei();
//	sleep_mode();
//
//	adc_val = ADCW;
////	sprintf_P(buff, PSTR("%" PRIu16 "\n\r"), adc_val);
////	uart_str_send(buff);
//	//adc_val = (uint16_t) fmax(0, (double) adc_val - 460);
//	//adc_val *= 10;
//	//adc_val = (uint16_t) (fmax(0, ((double) adc_val) - 512));
//}

//uint8_t const sine[256] PROGMEM = {
//	127, 130, 133, 136, 139, 143, 146, 149, 152, 155, 158, 161, 164, 167, 170, 173, 176, 178, 181, 184, 187, 189, 192, 195, 197, 200, 203, 205, 207, 210, 212, 214, 217, 219, 221, 223, 225, 227, 229, 231, 232, 234, 236, 237, 239, 240, 242, 243, 244, 245, 246, 248, 248, 249, 250, 251, 251, 252, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254, 253, 253, 253, 252, 252, 251, 250, 250, 249, 248, 247, 246, 245, 243, 242, 241, 239, 238, 236, 235, 233, 231, 229, 227, 225, 224, 221, 219, 217, 215, 213, 210, 208, 206, 203, 201, 198, 195, 193, 190, 187, 185, 182, 179, 176, 173, 170, 167, 164, 161, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131, 128, 125, 121, 118, 115, 112, 109, 106, 103, 100, 97, 94, 91, 88, 85, 82, 79, 76, 73, 71, 68, 65, 62, 60, 57, 55, 52, 50, 47, 45, 42, 40, 38, 36, 34, 31, 29, 27, 26, 24, 22, 20, 19, 17, 15, 14, 13, 11, 10, 9, 8, 7, 6, 5, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 16, 17, 19, 21, 22, 24, 26, 28, 30, 32, 34, 36, 39, 41, 43, 45, 48, 50, 53, 55, 58, 61, 63, 66, 69, 72, 74, 77, 80, 83, 86, 89, 92, 95, 98, 101, 104, 107, 110, 113, 116, 119, 122
//};

void audio_routine()
{
//	// Fetch new data from SD.
//	if (pos < 512) {
//	//	uart_str_send_P(PSTR("audio routine...\n\r"));
//		uint8_t voltage;
//
//		voltage = mmc_buf[pos++];
//		//voltage |= mmc_sbuf[pos++] << 8;
//
//		//++pos; ++pos;
//
//		OCR0A = voltage * 1;
//	}
}

//void audio_routine()
//{
//	cli();
//	if (1 || play)
//	{
//		pwm_init();
//		uint8_t delay,n;
//
//		for(delay = 1; delay <= 50; delay++)
//		{
//			for(n = 0; n < (51 - delay); n++)
//			{
//				for(uint8_t i = 0; i <= 254; i++)
//				{
//					OCR0A = pgm_read_byte_near(sine + i);
//					_delay_loop_2(delay);
//				}
//			}
//		}
//		for(delay=50;delay>=2;delay--)
//		{
//			for(n = 0; n < (51 - delay); n++)
//			{
//				for(uint8_t i=0;i<=254;i++)
//				{
//					OCR0A = pgm_read_byte_near(sine + i);
//					_delay_loop_2(delay);
//				}
//			}
//		}
//	}
//	sei();
//	TCCR0A = 0x00;
//	AUDIO_PWM_A_PORT |= 1 << AUDIO_PWM_A;
//
//	play = 0;
//}
