#include <avr/io.h>
#include <avr/interrupt.h>
#include "spi.h"
#include "sd.h"
#include <util/delay.h>
#include "config.h"
#include <avr/cpufunc.h>
#include <math.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "tools.h"
#include <stdlib.h>
#include <stdio.h>

#undef ADC_LOG

#define START		27
#define GAME_1		2
#define GAME_2		1
#define GAME_3		0
#define GAME_4		11

#define GAME_COUNT_TIME 600
#define GAME_TIME_GOAL	30
#define	SCREEN_DASH_ROW	10000

#define	L_R1		34
#define	L_R2		33
#define	L_R3		32
#define	L_R4		31
#define	L_R5		30

#define	L_G1		29
#define	L_G2		28
#define	L_G3		26
#define	L_G4		25
#define	L_G5		24

#define	L_Y1		23
#define	L_Y2		22
#define	L_Y3		21
#define	L_Y4		20
#define	L_Y5		19

#define	R_R1		18
#define	R_R2		17
#define	R_R3		16
#define	R_R4		15
#define	R_R5		14

#define	R_G1		13
#define	R_G2		12
#define	R_G3		10
#define	R_G4		9
#define	R_G5		8

#define	R_Y1		7
#define	R_Y2		6
#define	R_Y3		5
#define	R_Y4		4
#define	R_Y5		3

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

#define SR_OUT_CLK_DOWN \
	SR_OUT_CLK_PORT &= ~(1 << SR_OUT_CLK)

#define SR_OUT_CLK_UP \
	SR_OUT_CLK_PORT |= 1 << SR_OUT_CLK

#define SR_OUT_STROBE_DOWN \
	SR_OUT_STROBE_PORT &= ~(1 << SR_OUT_STROBE)

#define SR_OUT_STROBE_UP \
	SR_OUT_STROBE_PORT |= 1 << SR_OUT_STROBE

#define	SR_OUT_SET(BTN) \
	sr_out[BTN] = 1

#define	SR_OUT_CLR(BTN) \
	sr_out[BTN] = 0

#define SR_IN_CLK_DOWN \
	SR_IN_CLK_PORT &= ~(1 << SR_IN_CLK)

#define SR_IN_CLK_UP \
	SR_IN_CLK_PORT |= 1 << SR_IN_CLK

#define SR_IN_PL_DOWN \
	SR_IN_PL_PORT &= ~(1 << SR_IN_PL)

#define SR_IN_PL_UP \
	SR_IN_PL_PORT |= 1 << SR_IN_PL

#define	SR_IN_SET(BTN) \
	sr_in[BTN] = 1

#define	SR_IN_CLR(BTN) \
	sr_in[BTN] = 0

#define SR_SCREEN_CLK_DOWN \
	SR_SCREEN_CLK_PORT &= ~(1 << SR_SCREEN_CLK)

#define SR_SCREEN_CLK_UP \
	SR_SCREEN_CLK_PORT |= 1 << SR_SCREEN_CLK

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


#define	NOISE_FILTER_THRESHOLD 2

#define SR_OUT_BTNS_COUNT 35
#define BTNS_COUNT 35
#define SR_SCREEN_BTNS_COUNT 20
#define  BTN_OFFSET	10
#define  COMMON_OFFSET	110

#undef USE_METERS

#define CLR_CTCNT6()		\
	CTCNT6 = 0;		\
	*(&CTCNT6 + 1) = 0;	\
	*(&CTCNT6 + 2) = 0

#define CLR_CTCNT3()		\
	CTCNT3 = 0;		\
	*(&CTCNT6 + 1) = 0;	\
	*(&CTCNT6 + 2) = 0

uint8_t sr_screen[SR_SCREEN_BTNS_COUNT];
uint8_t screen_digit_index[SR_SCREEN_BTNS_COUNT - 8];
uint8_t current_game_index[4];
uint8_t sr_out[SR_OUT_BTNS_COUNT];
uint8_t sr_in[BTNS_COUNT];

uint8_t screen_render_digits[SR_SCREEN_BTNS_COUNT - 8];
uint8_t screen_render_cnt;
uint8_t screen_current_digit;
extern uint8_t context;
extern uint8_t sub_context;
extern uint8_t current_game;
extern uint8_t zebra;
extern uint8_t CTCNT6, CTCNT3;

uint8_t btns_noise_filter[BTNS_COUNT];
uint8_t current_left_button, current_right_button;
uint8_t left_is_float, right_is_float;

void sr_in_upd();
void sr_out_upd();
void screen_set_digit(uint8_t digit);
void screen_upd_number(uint16_t number, uint8_t screen);
void sr_screen_upd();
void screen_set_game_params();

volatile unsigned long startBlock;
volatile unsigned long totalBlocks;
volatile unsigned long firstDataSector, rootCluster, totalClusters;
volatile unsigned int bytesPerSector, sectorPerCluster, reservedSectorCount;
unsigned char buffer[48];
uint8_t track[16];

uint16_t screen_lb_number;
uint16_t screen_lt_number;
uint16_t screen_rb_number;
uint16_t screen_rt_number;
uint16_t left_count_game_record, left_time_game_record, right_count_game_record, right_time_game_record;

void main_init()
{

//	uart_str_send_P(PSTR("starting init routines...\n\r"));
	//spi_init();
	//memset(track, 0, sizeof(track));
//
//	uart_str_send_P(PSTR("starting SD routines...\n\r"));
//	SD_init();
	//return;

	context = MOD_IDLE;
	sub_context = SUB_MOD_COMMON;
	current_game = SINGLE_GAME_COUNT;
	current_left_button = START;
	current_right_button = GAME_4;

	left_count_game_record = left_time_game_record = right_count_game_record = right_time_game_record = SCREEN_DASH_ROW;

	screen_current_digit = 0;
	left_is_float = right_is_float = 1;

	int i = 0;
	while (i < sizeof(sr_in)) {
		sr_in[i] = 0;
		++i;
	}

	i = 0;
	while (i < sizeof(sr_out)) {
		sr_out[i] = 0;
		++i;
	}

	i = 0;
	while (i < sizeof(btns_noise_filter)) {
		btns_noise_filter[i] = 0;
		++i;
	}

	i = 0;
	while (i < sizeof(sr_screen)) {
		sr_screen[i] = 0;
		++i;
	}

	i = 0;
	while (i < sizeof(screen_render_digits)) {
		screen_render_digits[i] = 1;
		++i;
	}

	screen_digit_index[0] =	SCREEN_S1;
	screen_digit_index[1] =	SCREEN_S2;
	screen_digit_index[2] =	SCREEN_S3;
	screen_digit_index[3] =	SCREEN_S4;
	screen_digit_index[4] =	SCREEN_S5;
	screen_digit_index[5] =	SCREEN_S6;
	screen_digit_index[6] =	SCREEN_S7;
	screen_digit_index[7] =	SCREEN_S8;
	screen_digit_index[8] =	SCREEN_S9;
	screen_digit_index[9] = SCREEN_S10;
	screen_digit_index[10] = SCREEN_S11;
	screen_digit_index[11] = SCREEN_S12;

	current_game_index[0] = GAME_1;
	current_game_index[1] = GAME_2;
	current_game_index[2] = GAME_3;
	current_game_index[3] = GAME_4;

	screen_lb_number = SCREEN_DASH_ROW;
	screen_lt_number = SCREEN_DASH_ROW;
	screen_rb_number = SCREEN_DASH_ROW;
	screen_rt_number = SCREEN_DASH_ROW;
	screen_upd_number(screen_lb_number, SCREEN_LEFT_BOTTOM);
	screen_upd_number(screen_lt_number, SCREEN_LEFT_TOP);
	screen_upd_number(screen_rb_number, SCREEN_RIGHT_BOTTOM);
	screen_upd_number(screen_rt_number, SCREEN_RIGHT_TOP);
	SCREEN_CLR_SELECT;
	sr_screen_upd();

	sr_out_upd();

	// ADEN: Set to turn on ADC , by default it is turned off
	// ADPS2: ADPS2 and ADPS0 set to make division factor 32
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS0) | (1 <<  ADPS1);
	ADMUX = /*(1 << REFS1) | */(1 << REFS0) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0);
	// ADC7 selected & intnernal Vcc ref with external capacitor.
}

void sr_out_clr()
{
	for (int i = 0; i < sizeof(sr_out); ++i)
	{
		sr_out[i] = 0;
	}

}

void sr_out_off()
{
	SR_OUT_CLK_DOWN;
	SR_OUT_STROBE_DOWN;

	uint8_t i = 0;
	while (i < SR_OUT_BTNS_COUNT)
	{

		SR_OUT_DATA_PORT &= ~(1 << SR_OUT_DATA);

		SR_OUT_CLK_UP;
		SR_OUT_CLK_DOWN;

		++i;
	}

	SR_OUT_STROBE_UP;
	SR_OUT_STROBE_DOWN;
}

void sr_out_on()
{
	SR_OUT_CLK_DOWN;
	SR_OUT_STROBE_DOWN;

	uint8_t i = 0;
	while (i < SR_OUT_BTNS_COUNT)
	{

		SR_OUT_DATA_PORT |= (1 << SR_OUT_DATA);

		SR_OUT_CLK_UP;
		SR_OUT_CLK_DOWN;

		++i;
	}

	SR_OUT_STROBE_UP;
	SR_OUT_STROBE_DOWN;
}

uint8_t generate_random(uint8_t ignore_val)
{
	static uint8_t fake = 0;
	uint8_t random = 0xff;
	uint8_t limit = 3, top_limit = 55;
	uint8_t buf[30];

	while (top_limit && (random > 15 || random == ignore_val))
	{
		// Get not predictible value from floating ADC pin.
generator:	if (limit)
		{
			random = 0;

			for (int i = 0; i < 4; ++i)
			{
				ADCSRA |= (1 << ADSC); // Start conversion
				while (ADCSRA & (1 << ADSC)); // wait for conversion to complete

				buf[1] = ADCL;
				buf[0] = ADCH;
				buf[2] = 0;

				random |= (uint8_t) ((1 & (buf[1] >> i)) << i);


				#ifdef ADC_LOG
					sprintf_P(buf, PSTR("adc %u\n\r\0"), buf[1], random);
					uart_str_send(buf);
				#endif
			}

			#ifdef ADC_LOG
			sprintf_P(buf, PSTR("--- gen rand %u\n\r\0"), random);
			#endif
		}
		// Use incremented fake rand if ADC didn't gen new value.
		else {
			random = fake;
			++fake;
			if (fake > 15) fake = 0;

			#ifdef ADC_LOG
			sprintf_P(buf, PSTR("lim %u fake %u\n\r\0"), limit, fake);
			uart_str_send(buf);
			#endif
		}

		if (limit) --limit;
		if (top_limit) --top_limit;
	}

	++track[random];

	if (track[random] > 6) {
		uint8_t full = 1;
		for (int i = 0; i < sizeof(track); ++i)
		{
			if (track[i] < 7) {
				full = 0;
				break;
			}
		}

		if (full) {
			memset(track, 0, sizeof(track));
		}
		else {
			goto generator;
		}
	}

	#ifdef ADC_LOG
		sprintf_P(buf, PSTR("-----------  coeff %u  --------- ignore %u\n\r\0"), random, ignore_val);
		uart_str_send(buf);
	#endif

	return random;
}

void screen_upd_left_button()
{
	uint8_t ignore_val = current_left_button - 19;
	uint8_t random = 0xff;
	uint8_t limit = 55;

	while (random == current_left_button || random < L_Y5 || random > L_R1 || random == START)
	{
		if (limit) {
			random = (generate_random(ignore_val)) + 19;
			--limit;
		}
		else {
			++random;
			if (random > L_R1) random = 0;
		}
	}

	sr_out[random] = 1;
	sr_out[current_left_button] = 0;
	current_left_button = random;
	sr_out_upd();
}

void screen_upd_right_button()
{
	uint8_t ignore_val = current_right_button - 3;
	uint8_t random = 0xff;
	uint8_t limit = 55;

	while (random == current_right_button || random < R_Y5 || random > R_R1 || random == GAME_4)
	{
		if (limit) {
			random = (generate_random(ignore_val)) + 3;
			--limit;
		}
		else {
			++random;
			if (random > R_R1) random = 0;
		}
	}

	sr_out[random] = 1;
	sr_out[current_right_button] = 0;
	current_right_button = random;
	sr_out_upd();
}

void events_poll()
{
	sr_in_upd();
	uint8_t i = 0;

	while (i < BTNS_COUNT) {
		int pressed = sr_in[i];

		if (pressed) {
			++btns_noise_filter[i];

			if (btns_noise_filter[i] > NOISE_FILTER_THRESHOLD)
			{
				CLR_CTCNT6();

				if (i == START) {
					if (btns_noise_filter[i] > 20)
					{
						context = MOD_IDLE;
					}
					else {
						context = MOD_GAME_START;
					}
				}
				else if (context == MOD_IDLE || context == MOD_GAME_SELECT) {
					uint8_t changed = 0;
					switch (i)
					{
						case GAME_1:
							current_game = SINGLE_GAME_COUNT;
							changed = 1;
							break;

						case GAME_2:
							current_game = SINGLE_GAME_TIME;
							changed = 1;
							break;

						case GAME_3:
							current_game = MULTIPLAYER_GAME_COUNT;
							changed = 1;
							break;

						case GAME_4:
							current_game = MULTIPLAYER_GAME_TIME;
							changed = 1;
							break;
					}

					if (changed)
					{
						sr_out_clr();
						SR_OUT_SET(i);
						sr_out_upd();

						if (context == MOD_IDLE)
						{
							context = MOD_GAME_SELECT;
							CLR_CTCNT6();
						}

						screen_set_game_params();
					}

				}
				else if (context == MOD_GAME) {
					uint8_t left_hit = 0, right_hit = 0;
					if (i == current_left_button) {
						left_hit = 1;
						sr_out[i] = 0;

					}
					if (i == current_right_button) {
						right_hit = 1;
						sr_out[i] = 0;
					}

					switch (current_game)
					{
						case SINGLE_GAME_COUNT:
							if (left_hit)
							{
								++screen_lb_number;
								screen_upd_number(screen_lb_number, SCREEN_LEFT_BOTTOM);
								screen_upd_left_button();

								if (left_count_game_record == SCREEN_DASH_ROW || screen_lb_number > left_count_game_record)
								{
									left_count_game_record = screen_lb_number;
								}
							}
							break;

						case SINGLE_GAME_TIME:
							if (left_hit)
							{
								if (screen_lb_number > 0) {
									--screen_lb_number;
									screen_upd_number(screen_lb_number, SCREEN_LEFT_BOTTOM);
									if (screen_lb_number) screen_upd_left_button();
									else sr_out_upd();

									if (!screen_lb_number)
									{
										context = MOD_GAME_OVER;

										if (screen_lt_number == SCREEN_DASH_ROW || screen_lt_number < left_time_game_record)
										{
											left_time_game_record = screen_lt_number;
										}
									}

								}
							}
							break;

						case MULTIPLAYER_GAME_COUNT:
							if (left_hit)
							{
								++screen_lb_number;
								screen_upd_number(screen_lb_number, SCREEN_LEFT_BOTTOM);
								screen_upd_left_button();

								if (left_count_game_record == SCREEN_DASH_ROW || screen_lb_number > left_count_game_record)
								{
									left_count_game_record = screen_lb_number;
								}
							}
							if (right_hit)
							{
								++screen_rb_number;
								screen_upd_number(screen_rb_number, SCREEN_RIGHT_BOTTOM);
								screen_upd_right_button();

								if (right_count_game_record == SCREEN_DASH_ROW || screen_rb_number > right_count_game_record)
								{
									right_count_game_record = screen_rb_number;
								}
							}
							break;

						case MULTIPLAYER_GAME_TIME:
							if (left_hit)
							{
								if (screen_lb_number > 0) {
									--screen_lb_number;
									screen_upd_number(screen_lb_number, SCREEN_LEFT_BOTTOM);
									if (screen_lb_number) screen_upd_left_button();
									else sr_out_upd();

									if (!screen_lb_number)
									{
										if (screen_lt_number == SCREEN_DASH_ROW || screen_lt_number < left_time_game_record)
										{
											left_time_game_record = screen_lt_number;
										}

										sub_context = SUB_MOD_LEFT_GAME_OVER;
									}

									if (!screen_lb_number && !screen_rb_number)
									{
										context = MOD_GAME_OVER;
										CLR_CTCNT3();

									}

								}
							}
							if (right_hit)
							{
								if (screen_rb_number > 0) {
									--screen_rb_number;
									screen_upd_number(screen_rb_number, SCREEN_RIGHT_BOTTOM);
									if (screen_rb_number) screen_upd_right_button();
									else sr_out_upd();

									if (!screen_rb_number)
									{
										if (screen_rt_number == SCREEN_DASH_ROW || screen_rt_number < right_time_game_record)
										{
											right_time_game_record = screen_rt_number;
										}

										sub_context = SUB_MOD_RIGHT_GAME_OVER;
									}

									if (!screen_lb_number && !screen_rb_number)
									{
										context = MOD_GAME_OVER;
										CLR_CTCNT3();

									}

								}
							}
							break;
					}
				}
			}
		}
		else {
			btns_noise_filter[i] = 0;
		}

		++i;
	}
}

extern uint8_t ccc;
void script()
{
//	screen_upd_left_button();
//	screen_upd_right_button();
//	_delay_ms(10);
//	return;
	if (context == MOD_GAME)
	{
		switch (current_game_index[current_game])
		{
			case GAME_1:
				screen_lt_number -= 1;
				screen_upd_number(screen_lt_number, SCREEN_LEFT_TOP);
				if (!screen_lt_number)
				{
					context = MOD_GAME_OVER;
				}
				break;

			case GAME_2:
				if (screen_lb_number) {
					screen_lt_number += 1;
					screen_upd_number(screen_lt_number, SCREEN_LEFT_TOP);
				}
				break;

			case GAME_3:
				screen_lt_number -= 1;
				screen_rt_number -= 1;
				screen_upd_number(screen_lt_number, SCREEN_LEFT_TOP);
				screen_upd_number(screen_rt_number, SCREEN_RIGHT_TOP);
				if (!screen_lt_number || !screen_rt_number)
				{
					context = MOD_GAME_OVER;

					sr_out[current_left_button] = sr_out[current_right_button] = 0;
					sr_out_upd();
				}
				break;

			case GAME_4:
				if (screen_lb_number) {
					screen_lt_number += 1;
					screen_upd_number(screen_lt_number, SCREEN_LEFT_TOP);
				}
				if (screen_rb_number) {
					screen_rt_number += 1;
					screen_upd_number(screen_rt_number, SCREEN_RIGHT_TOP);
				}
				break;
		}
	}

	return;
	static uint16_t x = 0;
	if (x > 7) {
		++screen_lb_number;
		++screen_lt_number;
		++screen_rb_number;
		++screen_rt_number;
		if (screen_rt_number > 999) screen_rt_number = 0;
		if (screen_rb_number > 999) screen_rb_number = 0;
		if (screen_lb_number > 999) screen_lb_number = 0;
		if (screen_lt_number > 999) screen_lt_number = 0;

		screen_upd_number(screen_lb_number, SCREEN_LEFT_BOTTOM);
		screen_upd_number(screen_lt_number, SCREEN_LEFT_TOP);
		screen_upd_number(screen_rb_number, SCREEN_RIGHT_BOTTOM);
		screen_upd_number(screen_rt_number, SCREEN_RIGHT_TOP);
		x = 0;
	}
	++x;
	return;
	unsigned long address = 0;
	unsigned char retval = '\0';
	retval = SD_readSingleBlock(address);

	if (retval == '\0')
		while (address < 512) {
			//uart_str_send_P(PSTR("read "));
			//uart_str_send(&address);
			//uart_str_send_P(PSTR(": "));
			uart_str_send((void *) &(buffer[address++]));
			uart_str_send_P(PSTR(" | "));
		}
}

void screen_upd_number(uint16_t number, uint8_t screen)
{
	uint8_t current_screen_left_digit, current_screen_middle_digit, current_screen_right_digit;

	if (screen == SCREEN_LEFT_BOTTOM) {
		current_screen_left_digit = 0;
		current_screen_middle_digit = 1;
		current_screen_right_digit = 2;

		if (number > 999 && number != SCREEN_DASH_ROW) number = 999;
	}
	else if (screen == SCREEN_LEFT_TOP) {
		current_screen_left_digit = 3;
		current_screen_middle_digit = 4;
		current_screen_right_digit = 5;

		if (number == 1000 && left_is_float) left_is_float = 0;
		else if (number == 999 && !left_is_float) left_is_float = 1;
		else if (number == 0xffff && left_is_float) left_is_float = 0;

		if (number > 10000) number = 9999;
		if (number > 999 & left_is_float) left_is_float = 0;
		else if (number < 1000 & !left_is_float) left_is_float = 1;
	}
	else if (screen == SCREEN_RIGHT_BOTTOM) {
		current_screen_left_digit = 6;
		current_screen_middle_digit = 7;
		current_screen_right_digit = 8;

		if (number > 999 && number != SCREEN_DASH_ROW) number = 999;
	}
	else if (screen == SCREEN_RIGHT_TOP) {
		current_screen_left_digit = 9;
		current_screen_middle_digit = 10;
		current_screen_right_digit = 11;

		if (number == 1000 && right_is_float) right_is_float = 0;
		else if (number == 999 && !right_is_float) right_is_float = 1;
		else if (number == 0xffff && right_is_float) right_is_float = 0;

		if (number > 10000) number = 9999;
		if (number > 999 & right_is_float) right_is_float = 0;
		else if (number < 1000 & !right_is_float) right_is_float = 1;
	}

	if (number == SCREEN_DASH_ROW)
	{
		screen_render_digits[current_screen_left_digit] = screen_render_digits[current_screen_middle_digit] = screen_render_digits[current_screen_right_digit] = SCREEN_DASH;
		return;
	}

	uint8_t counter_rank;
	uint8_t left_rank = 0;

	{
		uint16_t overhead = number, adjusterd_number;

		int i = 0;
		if (overhead > 1000) overhead /= 10;
		adjusterd_number = overhead;
		while (overhead > 199) {
			overhead -= 100;
			++i;
		}

		counter_rank = (uint8_t) overhead;

		left_rank = (uint8_t) (adjusterd_number / 100) % 10;
	}

	for (int i = 0; i < 2; ++i)
	{
		uint8_t digit = counter_rank % 10;

		if (i == 0) { screen_render_digits[current_screen_right_digit] = digit; }
		else if (i == 1) { screen_render_digits[current_screen_middle_digit] = digit; }

		counter_rank /= 10;
	}

	screen_render_digits[current_screen_left_digit] = left_rank;
}

void sr_screen_upd()
{
	SR_SCREEN_CLK_DOWN;
	SR_SCREEN_STROBE_DOWN;

	uint8_t i = 0;
	while (i < sizeof(sr_screen))
	{

		if (sr_screen[i]) {
			if (i > 11)
				SR_SCREEN_DATA_PORT &= ~(1 << SR_SCREEN_DATA);
			else
				SR_SCREEN_DATA_PORT |= 1 << SR_SCREEN_DATA;
		}
		else {
			if (i > 11)
				SR_SCREEN_DATA_PORT |= 1 << SR_SCREEN_DATA;
			else
				SR_SCREEN_DATA_PORT &= ~(1 << SR_SCREEN_DATA);
		}

		SR_SCREEN_CLK_UP;
		SR_SCREEN_CLK_DOWN;

		++i;
	}

	SR_SCREEN_STROBE_UP;
	SR_SCREEN_STROBE_DOWN;
}

void screen_unset_digit()
{
	sr_screen[SCREEN_A] = 0;
	sr_screen[SCREEN_B] = 0;
	sr_screen[SCREEN_C] = 0;
	sr_screen[SCREEN_D] = 0;
	sr_screen[SCREEN_E] = 0;
	sr_screen[SCREEN_F] = 0;
	sr_screen[SCREEN_G] = 0;
	sr_screen[SCREEN_P] = 0;
}

void screen_set_digit(uint8_t digit)
{
	if (digit == 0) {
			sr_screen[SCREEN_A] = 1;
			sr_screen[SCREEN_B] = 1;
			sr_screen[SCREEN_C] = 1;
			sr_screen[SCREEN_D] = 1;
			sr_screen[SCREEN_E] = 1;
			sr_screen[SCREEN_F] = 1;
			sr_screen[SCREEN_G] = 0;
			sr_screen[SCREEN_P] = 0;
	}
	else if (digit == 1) {
			sr_screen[SCREEN_A] = 0;
			sr_screen[SCREEN_B] = 1;
			sr_screen[SCREEN_C] = 1;
			sr_screen[SCREEN_D] = 0;
			sr_screen[SCREEN_E] = 0;
			sr_screen[SCREEN_F] = 0;
			sr_screen[SCREEN_G] = 0;
			sr_screen[SCREEN_P] = 0;
	}

	else if (digit == 2) {
			sr_screen[SCREEN_A] = 1;
			sr_screen[SCREEN_B] = 1;
			sr_screen[SCREEN_C] = 0;
			sr_screen[SCREEN_D] = 1;
			sr_screen[SCREEN_E] = 1;
			sr_screen[SCREEN_F] = 0;
			sr_screen[SCREEN_G] = 1;
			sr_screen[SCREEN_P] = 0;
	}

	else if (digit == 3) {
			sr_screen[SCREEN_A] = 1;
			sr_screen[SCREEN_B] = 1;
			sr_screen[SCREEN_C] = 1;
			sr_screen[SCREEN_D] = 1;
			sr_screen[SCREEN_E] = 0;
			sr_screen[SCREEN_F] = 0;
			sr_screen[SCREEN_G] = 1;
			sr_screen[SCREEN_P] = 0;
	}

	else if (digit == 4) {
			sr_screen[SCREEN_A] = 0;
			sr_screen[SCREEN_B] = 1;
			sr_screen[SCREEN_C] = 1;
			sr_screen[SCREEN_D] = 0;
			sr_screen[SCREEN_E] = 0;
			sr_screen[SCREEN_F] = 1;
			sr_screen[SCREEN_G] = 1;
			sr_screen[SCREEN_P] = 0;
	}

	else if (digit == 5) {
			sr_screen[SCREEN_A] = 1;
			sr_screen[SCREEN_B] = 0;
			sr_screen[SCREEN_C] = 1;
			sr_screen[SCREEN_D] = 1;
			sr_screen[SCREEN_E] = 0;
			sr_screen[SCREEN_F] = 1;
			sr_screen[SCREEN_G] = 1;
			sr_screen[SCREEN_P] = 0;
	}

	else if (digit == 6) {
			sr_screen[SCREEN_A] = 1;
			sr_screen[SCREEN_B] = 0;
			sr_screen[SCREEN_C] = 1;
			sr_screen[SCREEN_D] = 1;
			sr_screen[SCREEN_E] = 1;
			sr_screen[SCREEN_F] = 1;
			sr_screen[SCREEN_G] = 1;
			sr_screen[SCREEN_P] = 0;
	}

	else if (digit == 7) {
			sr_screen[SCREEN_A] = 1;
			sr_screen[SCREEN_B] = 1;
			sr_screen[SCREEN_C] = 1;
			sr_screen[SCREEN_D] = 0;
			sr_screen[SCREEN_E] = 0;
			sr_screen[SCREEN_F] = 0;
			sr_screen[SCREEN_G] = 0;
			sr_screen[SCREEN_P] = 0;
	}
	else if (digit == 8) {
			sr_screen[SCREEN_A] = 1;
			sr_screen[SCREEN_B] = 1;
			sr_screen[SCREEN_C] = 1;
			sr_screen[SCREEN_D] = 1;
			sr_screen[SCREEN_E] = 1;
			sr_screen[SCREEN_F] = 1;
			sr_screen[SCREEN_G] = 1;
			sr_screen[SCREEN_P] = 0;
	}
	else if (digit == 9) {
			sr_screen[SCREEN_A] = 1;
			sr_screen[SCREEN_B] = 1;
			sr_screen[SCREEN_C] = 1;
			sr_screen[SCREEN_D] = 1;
			sr_screen[SCREEN_E] = 0;
			sr_screen[SCREEN_F] = 1;
			sr_screen[SCREEN_G] = 1;
			sr_screen[SCREEN_P] = 0;

	}
	else if (digit == 10) {
			sr_screen[SCREEN_A] = 0;
			sr_screen[SCREEN_B] = 0;
			sr_screen[SCREEN_C] = 0;
			sr_screen[SCREEN_D] = 0;
			sr_screen[SCREEN_E] = 0;
			sr_screen[SCREEN_F] = 0;
			sr_screen[SCREEN_G] = 1;
			sr_screen[SCREEN_P] = 0;

	}
	else if (digit == 11) {
			sr_screen[SCREEN_A] = 0;
			sr_screen[SCREEN_B] = 0;
			sr_screen[SCREEN_C] = 0;
			sr_screen[SCREEN_D] = 0;
			sr_screen[SCREEN_E] = 0;
			sr_screen[SCREEN_F] = 0;
			sr_screen[SCREEN_G] = 0;
			sr_screen[SCREEN_P] = 0;

	}
	else {
			sr_screen[SCREEN_A] = 0;
			sr_screen[SCREEN_B] = 1;
			sr_screen[SCREEN_C] = 1;
			sr_screen[SCREEN_D] = 1;
			sr_screen[SCREEN_E] = 1;
			sr_screen[SCREEN_F] = 0;
			sr_screen[SCREEN_G] = 1;
			sr_screen[SCREEN_P] = 0;

	}
}

void sr_out_upd()
{
	SR_OUT_CLK_DOWN;
	SR_OUT_STROBE_DOWN;

	uint8_t i = 0;
	while (i < sizeof(sr_out))
	{

		if (sr_out[i]) {
			SR_OUT_DATA_PORT |= 1 << SR_OUT_DATA;
		}
		else {
			SR_OUT_DATA_PORT &= ~(1 << SR_OUT_DATA);
		}

		SR_OUT_CLK_UP;
		SR_OUT_CLK_DOWN;
		++i;
	}

	SR_OUT_STROBE_UP;
	SR_OUT_STROBE_DOWN;
}

void sr_in_upd()
{
	SR_IN_PL_DOWN;
	SR_IN_CLK_UP;
	SR_IN_PL_UP;

	uint8_t i = 0, ii = 34;
	while (i < BTNS_COUNT - 3)
	{
		++i;

		sr_in[ii] = 1 ^ (((1 << SR_IN_DATA) & SR_IN_DATA_PIN) >> SR_IN_DATA);
		--ii;

		SR_IN_CLK_DOWN;
		SR_IN_CLK_UP;

	}

	SR_IN_PL_DOWN;

	sr_in[1] = 1 ^ (((1 << SR_IN_THIRD) & SR_IN_THIRD_PIN) >> SR_IN_THIRD);
	sr_in[2] = 1 ^ (((1 << SR_IN_SECOND) & SR_IN_SECOND_PIN) >> SR_IN_SECOND);
	sr_in[0] = 1 ^ (((1 << SR_IN_FIRST) & SR_IN_FIRST_PIN) >> SR_IN_FIRST);
}

void screen_select_digit(uint8_t digit)
{
	sr_screen[screen_digit_index[digit]] = 1;
}

/**
 * Caution this is a interrupt routine, please don't overhead the function.
 */
void screen_render()
{
	SCREEN_CLR_SELECT;
	sr_screen_upd();

	if (screen_current_digit > 11) screen_current_digit = 0;
	screen_select_digit(screen_current_digit);
	screen_set_digit(screen_render_digits[screen_current_digit]);

	if (screen_render_digits[screen_current_digit] < 10)
	{
		if (left_is_float && screen_current_digit == SCREEN_LEFT_TOP_MIDDLE_DIGIT) {
			sr_screen[SCREEN_P] = 1;
		}
		else if (right_is_float && screen_current_digit == SCREEN_RIGHT_TOP_MIDDLE_DIGIT) {
			sr_screen[SCREEN_P] = 1;
		}
	}

	sr_screen_upd();
	++screen_current_digit;
}

void screen_set_game_params()
{
	switch (current_game_index[current_game])
	{
		case GAME_1:
			screen_lb_number = 0;
			screen_lt_number = GAME_COUNT_TIME;
			screen_rb_number = SCREEN_DASH_ROW;
			screen_rt_number = SCREEN_DASH_ROW;
			break;

		case GAME_2:
			screen_lb_number = GAME_TIME_GOAL;
			screen_lt_number = 0;
			left_is_float = 1;
			screen_rb_number = SCREEN_DASH_ROW;
			screen_rt_number = SCREEN_DASH_ROW;
			break;

		case GAME_3:
			screen_lb_number = 0;
			screen_lt_number = GAME_COUNT_TIME;
			screen_rb_number = 0;
			screen_rt_number = GAME_COUNT_TIME;

			break;

		case GAME_4:
			screen_lb_number = GAME_TIME_GOAL;
			screen_lt_number = 0;
			screen_rb_number = GAME_TIME_GOAL;
			screen_rt_number = 0;
			right_is_float = left_is_float = 1;
			break;
	}

	screen_upd_number(screen_lb_number, SCREEN_LEFT_BOTTOM);
	screen_upd_number(screen_lt_number, SCREEN_LEFT_TOP);
	screen_upd_number(screen_rb_number, SCREEN_RIGHT_BOTTOM);
	screen_upd_number(screen_rt_number, SCREEN_RIGHT_TOP);
}

void game_start_complete_script()
{
	sr_out_clr();
	screen_set_game_params();

	screen_upd_left_button();
	if (current_game == MULTIPLAYER_GAME_TIME || current_game == MULTIPLAYER_GAME_COUNT)
		screen_upd_right_button();

	SR_OUT_SET(START);
	SR_OUT_SET(current_game_index[current_game]);
	sr_out_upd();
	CLR_CTCNT6();
}

void game_idle_action_script()
{
	static uint8_t game_button = 0;

	if (game_button > 3) game_button = 0;

	uint8_t start_bit = sr_out[START] ^ 1;

	sr_out_clr();
	screen_upd_left_button();
	screen_upd_right_button();

	switch (game_button)
	{
		case 0:
			sr_out[GAME_1] = 1;
			break;
		case 1:
			sr_out[GAME_2] = 1;
			break;
		case 2:
			sr_out[GAME_3] = 1;
			break;
		case 3:
			sr_out[GAME_4] = 1;
			break;
	}

	sr_out[START] = start_bit;

	sr_out_upd();

	++game_button;

	screen_lb_number = left_count_game_record;
	screen_lt_number = left_time_game_record;
	screen_rb_number = right_count_game_record;
	screen_rt_number = right_time_game_record;

	screen_upd_number(screen_lb_number, SCREEN_LEFT_BOTTOM);
	screen_upd_number(screen_lt_number, SCREEN_LEFT_TOP);
	screen_upd_number(screen_rb_number, SCREEN_RIGHT_BOTTOM);
	screen_upd_number(screen_rt_number, SCREEN_RIGHT_TOP);
}

void game_select_blink()
{
	sr_out[START] ^= 1;
	sr_out_upd();
}

void game_select_complete_script()
{
}

void game_start_blink()
{
	static uint8_t odd = 0;
	if (odd)
	{
		for (int i = 0; i < sizeof(screen_render_digits); ++i)
		{
			screen_render_digits[i] = SCREEN_BLANK;
		}

		sr_out_off();
	}
	else {
		for (int i = 0; i < sizeof(screen_render_digits); ++i)
		{
			screen_render_digits[i] = SCREEN_DASH;
		}

		sr_out_on();
	}

	odd ^= 1;
}

void game_over_complete_script()
{
	screen_upd_number(screen_lb_number, SCREEN_LEFT_BOTTOM);
	screen_upd_number(screen_lt_number, SCREEN_LEFT_TOP);
	screen_upd_number(screen_rb_number, SCREEN_RIGHT_BOTTOM);
	screen_upd_number(screen_rt_number, SCREEN_RIGHT_TOP);

	for (int i = 0; i < sizeof(sr_out); ++i)
	{
		if (((i >= R_Y5 && i <= R_R1) || (i >= L_Y5 && i <= L_R1))  && i != GAME_4 && i != GAME_3 && i != GAME_2 && i != GAME_1 && i != START)
			sr_out[i] = 0;
	}

	sr_out_upd();

	context = MOD_GAME_SELECT;
	sub_context = SUB_MOD_COMMON;

	CLR_CTCNT6();
	CLR_CTCNT3();
}

void game_over_blink()
{
	static uint8_t odd = 0;
	if (odd)
	{
		for (int i = 0; i < sizeof(screen_render_digits); ++i)
		{
			screen_render_digits[i] = SCREEN_BLANK;
		}

		sr_out_off();
	}
	else {

		screen_upd_number(screen_lb_number, SCREEN_LEFT_BOTTOM);
		screen_upd_number(screen_lt_number, SCREEN_LEFT_TOP);
		screen_upd_number(screen_rb_number, SCREEN_RIGHT_BOTTOM);
		screen_upd_number(screen_rt_number, SCREEN_RIGHT_TOP);

		sr_out_on();
	}
	odd ^= 1;
}

void left_game_over_complete_script()
{
	screen_upd_number(screen_lb_number, SCREEN_LEFT_BOTTOM);
	screen_upd_number(screen_lt_number, SCREEN_LEFT_TOP);

	for (int i = 0; i < sizeof(sr_out); ++i)
	{
		if (i >= L_Y5 && i <= L_R1  && i != GAME_4 && i != GAME_3 && i != GAME_2 && i != GAME_1 && i != START)
			sr_out[i] = 0;
	}

	sr_out_upd();

	sub_context = SUB_MOD_COMMON;
}

void left_game_over_blink()
{
	static uint8_t odd = 0;
	if (odd)
	{
		for (int i = 0; i < sizeof(screen_render_digits); ++i)
		{
				if (i < 6)
					screen_render_digits[i] = SCREEN_BLANK;
		}

		for (int i = 0; i < sizeof(sr_out); ++i)
		{
			if (i >= L_Y5 && i <= L_R1  && i != GAME_4 && i != GAME_3 && i != GAME_2 && i != GAME_1 && i != START)
				sr_out[i] = 0;
		}

		sr_out_upd();
	}
	else {

		screen_upd_number(screen_lb_number, SCREEN_LEFT_BOTTOM);
		screen_upd_number(screen_lt_number, SCREEN_LEFT_TOP);

		for (int i = 0; i < sizeof(sr_out); ++i)
		{
			if (i >= L_Y5 && i <= L_R1  && i != GAME_4 && i != GAME_3 && i != GAME_2 && i != GAME_1 && i != START)
				sr_out[i] = 1;
		}

		sr_out_upd();
	}
	odd ^= 1;
}

void right_game_over_complete_script()
{
	screen_upd_number(screen_rb_number, SCREEN_RIGHT_BOTTOM);
	screen_upd_number(screen_rt_number, SCREEN_RIGHT_TOP);

	for (int i = 0; i < sizeof(sr_out); ++i)
	{
		if (i >= R_Y5 && i <= R_R1 && i != GAME_4 && i != GAME_3 && i != GAME_2 && i != GAME_1 && i != START)
			sr_out[i] = 0;
	}

	sr_out_upd();

	sub_context = SUB_MOD_COMMON;
}

void right_game_over_blink()
{
	static uint8_t odd = 0;
	if (odd)
	{
		for (int i = 0; i < sizeof(screen_render_digits); ++i)
		{
				if (i > 5)
					screen_render_digits[i] = SCREEN_BLANK;
		}

		for (int i = 0; i < sizeof(sr_out); ++i)
		{
			if (i >= R_Y5 && i <= R_R1 && i != GAME_4 && i != GAME_3 && i != GAME_2 && i != GAME_1 && i != START)
				sr_out[i] = 0;
		}

		sr_out_upd();
	}
	else {

		screen_upd_number(screen_rb_number, SCREEN_RIGHT_BOTTOM);
		screen_upd_number(screen_rt_number, SCREEN_RIGHT_TOP);

		for (int i = 0; i < sizeof(sr_out); ++i)
		{
			if (i >= R_Y5 && i <= R_R1 && i != GAME_4 && i != GAME_3 && i != GAME_2 && i != GAME_1 && i != START)
				sr_out[i] = 1;
		}

		sr_out_upd();
	}
	odd ^= 1;
}

extern void audio_stream();

void audio_routine()
{
		audio_stream();

/*
	#define INC_AUDIO
	static uint32_t cnt = 0, x;
	x = 5;
	#ifdef INC_AUDIO
	++cnt;
	if (cnt > x)
	#else
		cnt ^= 1;
		if (cnt)
	#endif
	{
		OCR0A = OCR0B = 55;
		#ifdef INC_AUDIO
		if (cnt > x * 2) cnt = 0;
		#endif
	}
	else {

		OCR0A = OCR0B = 0;
	}
	*/
}
