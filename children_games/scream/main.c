#include <avr/io.h>
#include <avr/interrupt.h>
#include "spi.h"
#include "sd.h"
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

#undef ADC_LOG

#define START		27

#define	SCREEN_DASH_ROW	10000

#define SCREEN_S8	11 // s8
#define SCREEN_S7	10 // s7
#define SCREEN_S6	9 // s6
#define SCREEN_S5	8 // s5
#define SCREEN_S4	7 // s4
#define SCREEN_S3	6 // s3
#define SCREEN_S2	5 // s2
#define SCREEN_S1	4 // s1
#define SCREEN_S9	0
#define SCREEN_S10	1
#define SCREEN_S11	2
#define SCREEN_S12	3

#define SCREEN_LEFT_TOP_MIDDLE_DIGIT 4
#define SCREEN_RIGHT_TOP_MIDDLE_DIGIT 10

#define SCREEN_BLANK	11
#define SCREEN_DASH	10

#define SCREEN_LEFT_BOTTOM	1
#define SCREEN_LEFT_TOP		2
#define SCREEN_RIGHT_BOTTOM	3
#define SCREEN_RIGHT_TOP	4

#define SR_SCREEN_CLK_UP \
	SR_SCREEN_CLK_PORT |= 1 << SR_SCREEN_CLK

#define SR_SCREEN_CLK_DOWN \
	SR_SCREEN_CLK_PORT &= ~(1 << SR_SCREEN_CLK)

#define SR_SCREEN_STROBE_DOWN \
	SR_SCREEN_STROBE_PORT &= ~(1 << SR_SCREEN_STROBE)

#define SR_SCREEN_STROBE_UP \
	SR_SCREEN_STROBE_PORT |= 1 << SR_SCREEN_STROBE

#define SCREEN_CLR_SELECT					\
	for (int i = 0; i < sizeof(sr_screen); ++i)		\
	{							\
		sr_screen[i] = 0;				\
	}

#define	SCREEN_A	19
#define	SCREEN_B	18
#define	SCREEN_C	17
#define	SCREEN_D	16
#define	SCREEN_E	15
#define	SCREEN_F	14
#define	SCREEN_G	13
#define	SCREEN_P	12

#define BTNS_COUNT	1
#define	NOISE_FILTER_THRESHOLD 2

#undef USE_METERS

#define CLR_CTCNT6()		\
	CTCNT6 = 0;		\
	*(&CTCNT6 + 1) = 0;	\
	*(&CTCNT6 + 2) = 0

#define CLR_CTCNT3()		\
	CTCNT3 = 0;		\
	*(&CTCNT6 + 1) = 0;	\
	*(&CTCNT6 + 2) = 0

uint8_t buffer[48];

void main_init()
{
	spi_init();
	while(mmc_init());

	memset(mmc_sbuf, 0, sizeof(mmc_sbuf));
	// ADEN: Set to turn on ADC , by default it is turned off
	// ADPS2: ADPS2 and ADPS0 set to make division factor 32
	ADCSRA = (1 << ADEN);// | (1 << ADPS2) | (1 << ADPS0) | (1 <<  ADPS1);
	ADMUX = /* (1 << REFS1) | */(1 << REFS0) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0);
	// ADC7 selected & intnernal Vcc ref with external capacitor.
}

void script()
{
	//mmc_init();
	static uint32_t sector = 0;
	uint8_t status = mmc_read(sector);
	sprintf_P(buffer, PSTR("status of read: %u, sector %lu\n\r"), status, sector);
	uart_str_send(buffer);
//
	uint16_t st = mmc_check();
	sprintf_P(buffer, PSTR("card status: %" PRIu16 "\n\r"), st);
	uart_str_send(buffer);

	uart_str_send_cnt(mmc_sbuf, 512);
}


//uint16_t adc_val;
//uint16_t cached_adc;

//void adc_update()
//{
//	//uart_str_send_P(PSTR("Hi there! What's up?\n\r"));
//	ADCSRA |= (1 << ADSC); // Start conversion
//	while (ADCSRA & (1 << ADSC)); // wait for conversion to complete
//
////	buf[0] = ADCL;
////	buf[1] = ADCH;
//	adc_val = ADCW;
//	uint16_t cc;
//	cc = (uint16_t) (((double) adc_val / 1024.) * 255.);
//	OCR0A = (uint8_t) cc;
//	sprintf_P(buffer, PSTR("ocr0a %u, %u\n\r"), (uint8_t) cc, adc_val);
//	uart_str_send(buffer);
//	//sprintf_P(buffer, PSTR("%u\n\r"), adc_val);
////	uart_str_send_cnt(&(buf[1]), 1);
////	uart_str_send_cnt(&(buf[0]), 1);
//
////	static uint16_t cnt = 0;
////
////	if (adc_val > cached_adc) cached_adc = adc_val;
////
////	if (cnt > 500)
////	{
////		cnt = 0;
////		cached_adc = adc_val;
////	}
////
////	++cnt;
//	//double volts = (double) cached_adc * (1.1 / 1024);
//	//sprintf_P(buf, PSTR("%" PRIu16 "\n\r"), cached_adc);
//	//uart_str_send(buf);
//}


//uint8_t const sine[256] PROGMEM = {
//                     127,
//                     130,
//                     133,
//                     136,
//                     139,
//                     143,
//                     146,
//                     149,
//                     152,
//                     155,
//                     158,
//                     161,
//                     164,
//                     167,
//                     170,
//                     173,
//                     176,
//                     178,
//                     181,
//                     184,
//                     187,
//                     189,
//                     192,
//                     195,
//                     197,
//                     200,
//                     203,
//                     205,
//                     207,
//                     210,
//                     212,
//                     214,
//                     217,
//                     219,
//                     221,
//                     223,
//                     225,
//                     227,
//                     229,
//                     231,
//                     232,
//                     234,
//                     236,
//                     237,
//                     239,
//                     240,
//                     242,
//                     243,
//                     244,
//                     245,
//                     246,
//                     248,
//                     248,
//                     249,
//                     250,
//                     251,
//                     251,
//                     252,
//                     253,
//                     253,
//                     253,
//                     254,
//                     254,
//                     254,
//                     254,
//                     254,
//                     254,
//                     254,
//                     253,
//                     253,
//                     253,
//                     252,
//                     252,
//                     251,
//                     250,
//                     250,
//                     249,
//                     248,
//                     247,
//                     246,
//                     245,
//                     243,
//                     242,
//                     241,
//                     239,
//                     238,
//                     236,
//                     235,
//                     233,
//                     231,
//                     229,
//                     227,
//                     225,
//                     224,
//                     221,
//                     219,
//                     217,
//                     215,
//                     213,
//                     210,
//                     208,
//                     206,
//                     203,
//                     201,
//                     198,
//                     195,
//                     193,
//                     190,
//                     187,
//                     185,
//                     182,
//                     179,
//                     176,
//                     173,
//                     170,
//                     167,
//                     164,
//                     161,
//                     158,
//                     155,
//                     152,
//                     149,
//                     146,
//                     143,
//                     140,
//                     137,
//                     134,
//                     131,
//                     128,
//                     125,
//                     121,
//                     118,
//                     115,
//                     112,
//                     109,
//                     106,
//                     103,
//                     100,
//                     97,
//                     94,
//                     91,
//                     88,
//                     85,
//                     82,
//                     79,
//                     76,
//                     73,
//                     71,
//                     68,
//                     65,
//                     62,
//                     60,
//                     57,
//                     55,
//                     52,
//                     50,
//                     47,
//                     45,
//                     42,
//                     40,
//                     38,
//                     36,
//                     34,
//                     31,
//                     29,
//                     27,
//                     26,
//                     24,
//                     22,
//                     20,
//                     19,
//                     17,
//                     15,
//                     14,
//                     13,
//                     11,
//                     10,
//                     9,
//                     8,
//                     7,
//                     6,
//                     5,
//                     4,
//                     3,
//                     3,
//                     2,
//                     2,
//                     1,
//                     1,
//                     0,
//                     0,
//                     0,
//                     0,
//                     0,
//                     0,
//                     0,
//                     1,
//                     1,
//                     1,
//                     2,
//                     2,
//                     3,
//                     4,
//                     4,
//                     5,
//                     6,
//                     7,
//                     8,
//                     9,
//                     10,
//                     12,
//                     13,
//                     14,
//                     16,
//                     17,
//                     19,
//                     21,
//                     22,
//                     24,
//                     26,
//                     28,
//                     30,
//                     32,
//                     34,
//                     36,
//                     39,
//                     41,
//                     43,
//                     45,
//                     48,
//                     50,
//                     53,
//                     55,
//                     58,
//                     61,
//                     63,
//                     66,
//                     69,
//                     72,
//                     74,
//                     77,
//                     80,
//                     83,
//                     86,
//                     89,
//                     92,
//                     95,
//                     98,
//                     101,
//                     104,
//                     107,
//                     110,
//                     113,
//                     116,
//                     119,
//                     122,
//};


extern void audio_stream(uint8_t *);

void audio_routine()
{
	return;
//	uint8_t delay,n;
//
//	for(delay=1;delay<=50;delay++)
//	{
//		for(n=0;n<(51-delay);n++)
//		{
//			for(uint8_t i=0;i<=254;i++)
//			{
//				OCR0B = OCR0A = pgm_read_byte_near(sine+i);
//				_delay_loop_2(delay);
//
//			}
//		}
//	}
//	for(delay=50;delay>=2;delay--)
//	{
//		for(n=0;n<(51-delay);n++)
//		{
//			for(uint8_t i=0;i<=254;i++)
//			{
//				OCR0B = OCR0A = pgm_read_byte_near(sine+i);
//				_delay_loop_2(delay);
//			}
//		}
//	}


	static uint16_t pointer = 0;
	audio_stream((uint8_t *) pointer);
	++pointer;
//	static uint16_t x = 0;
//	double y, z;
//	z = M_PI * x / 180;
//	y = sin(z);
//	double c = ((y + 1) * 127.5);
//	OCR0A = (uint8_t) c ;
//	//sprintf_P(buffer, PSTR("angle %d radians %lf sin %lf\n\r"), x, z, y);
//	//uart_str_send(buffer);
//	x += 7;
//	if (x > 360) {
//		x = 0;
//	}
}
