#include <avr/io.h>
#include <avr/pgmspace.h>
//#include <utils.h>
#include <spi.h>
#include <util/delay.h>
#include "config.h"
#include <avr/cpufunc.h>

#define OUT_START	0
#define	OUT_H1		1
#define	OUT_H2		2
#define	OUT_H3		3
#define	OUT_H4		4
#define	OUT_H5		5
#define	OUT_H6		6
#define	OUT_H7		7
#define	OUT_H8		8
#define	OUT_H9		9
#define	OUT_H10		10

#define START		0
#define	H1		1
#define	H2		2
#define	H3		3
#define	H4		4
#define	H5		5
#define	H6		6
#define	H7		7
#define	H8		8
#define	H9		9
#define	H10		10

#define IN_START	10
#define	IN_H1		9
#define	IN_H2		8

#define	IN_H3		7
#define	IN_H4		6
#define	IN_H5		5
#define	IN_H6		4
#define	IN_H7		3
#define	IN_H8		2
#define	IN_H9		1
#define	IN_H10		0

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

#define SR_SCREEN_SELECT_LEFT \
	SR_SCREEN_S2_PORT &= ~(1 << SR_SCREEN_S2); \
	SR_SCREEN_S3_PORT &= ~(1 << SR_SCREEN_S3); \
	SR_SCREEN_S1_PORT |= 1 << SR_SCREEN_S1

#define SR_SCREEN_SELECT_MIDDLE \
	SR_SCREEN_S1_PORT &= ~(1 << SR_SCREEN_S1); \
	SR_SCREEN_S3_PORT &= ~(1 << SR_SCREEN_S3); \
	SR_SCREEN_S2_PORT |= 1 << SR_SCREEN_S2

#define SR_SCREEN_SELECT_RIGHT \
	SR_SCREEN_S1_PORT &= ~(1 << SR_SCREEN_S1); \
	SR_SCREEN_S2_PORT &= ~(1 << SR_SCREEN_S2); \
	SR_SCREEN_S3_PORT |= 1 << SR_SCREEN_S3

#define	SCREEN_A	7
#define	SCREEN_B	6
#define	SCREEN_C	5
#define	SCREEN_D	4
#define	SCREEN_E	3
#define	SCREEN_F	2
#define	SCREEN_G	0
#define	SCREEN_P	1

#define	NOISE_FILTER_THRESHOLD 2

#define SR_OUT_BTNS_COUNT 11
#define BTNS_COUNT 11
#define SR_IN_BTNS_COUNT 16
#define SR_SCREEN_BTNS_COUNT 8
#define  BTN_OFFSET	10
#define  COMMON_OFFSET	110

#undef USE_METERS

uint8_t sr_screen[SR_SCREEN_BTNS_COUNT];
uint8_t sr_out[SR_OUT_BTNS_COUNT];
uint8_t sr_in[SR_IN_BTNS_COUNT - 5];

uint8_t screen_render_current_s;
uint8_t record_btn;
uint8_t screen_render_s1_digit;
uint8_t screen_render_s2_digit;
uint8_t screen_render_s3_digit;
extern uint8_t context;
extern uint8_t zebra;
extern uint8_t CTCNT6;

uint8_t btns_noise_filter[BTNS_COUNT];

void sr_in_upd();
void sr_out_upd();
void screen_set_digit(uint8_t digit);

void initial_delay()
{
	_delay_ms(100);
}

void __data_start() {}
void __data_load_start() {}
void __data_end() {}
void __bss_start() {}
void __bss_end() {}

uint16_t screen_number;
uint16_t record;

void main_init()
{
	context = MOD_IDLE;
	record = 0;
	record_btn = 0;
	screen_render_current_s = 1;
	#ifdef USE_METERS
		screen_render_s1_digit = 11;
	#else
		screen_render_s1_digit = 0;
	#endif
	screen_render_s2_digit = 0;
	screen_render_s3_digit = 0;

	int i = 0;
	while (i < sizeof(sr_in)) {
		sr_in[i] = 0;
		++i;
	}

	i = 0;
	while (i < sizeof(sr_out)) {
		sr_out[i] = 1;
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

	screen_number = 0;
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

void screen_upd_number(uint16_t number);

void events_poll()
{
	sr_in_upd();
	int i = 0;

	while (i < BTNS_COUNT) {
		int pressed = sr_in[i];

		if (pressed) {
			++btns_noise_filter[i];

			if (btns_noise_filter[i] > NOISE_FILTER_THRESHOLD)
			{
				CTCNT6 = 0;
				*(&CTCNT6 + 1) = 0;
				*(&CTCNT6 + 2) = 0;

				if (i == START) {
					if (btns_noise_filter[i] > 20)
					{
						context = MOD_IDLE;
					}
					else context = MOD_GAME_START;
				}
				else if (context == MOD_GAME) {
					uint16_t length = COMMON_OFFSET + i * BTN_OFFSET;
					if (length > screen_number) {
						screen_number = length;
						screen_upd_number(screen_number);

						sr_out_clr();

						SR_OUT_SET(START);
						SR_OUT_SET(i);
						sr_out_upd();

						if (length > record) {
							record = length;
							record_btn = i;
						}
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

void script()
{
//	static uint8_t x = 0;
//	x ^= 1;
//	if (x) {
//		sr_out_on();
//	}
//	else {
//		sr_out_off();
//	}

	//screen_upd_number(screen_number);
	//screen_number++;
}

void screen_upd_number(uint16_t number)
{
	uint8_t counter_rank;
	uint8_t left_rank = 0;

	{
		uint16_t overhead = number;

		int i = 0;
		while (overhead > 199) {
			overhead -= 100;
			++i;
		}

		counter_rank = (uint8_t) overhead;

		if (number > 199) left_rank = 1 + i;
		else {
			uint8_t temp = (uint8_t) number / 100;
			left_rank = temp % 10;
		}
	}

	for (int i = 0; i < 2; ++i)
	{
		uint8_t digit = counter_rank % 10;

		#ifdef USE_METERS
			if (i == 1) { screen_render_s3_digit = digit; }
		#else
			if (i == 0) { screen_render_s3_digit = digit; }
			else if (i == 1) { screen_render_s2_digit = digit; }
		#endif

		counter_rank /= 10;
	}

	#ifdef USE_METERS
		screen_render_s2_digit = left_rank;
		screen_render_s1_digit = 11;
	#else
		screen_render_s1_digit = left_rank;
	#endif
}

void sr_screen_upd()
{
	SR_SCREEN_CLK_DOWN;
	SR_SCREEN_STROBE_DOWN;

	uint8_t i = 0;
	while (i < sizeof(sr_screen))
	{

		if (sr_screen[i]) {
			SR_SCREEN_DATA_PORT &= ~(1 << SR_SCREEN_DATA);
		}
		else {
			SR_SCREEN_DATA_PORT |= 1 << SR_SCREEN_DATA;
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

	sr_screen_upd();
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

	uint8_t i = 0, ii = 10;
	while (i < SR_IN_BTNS_COUNT)
	{
		++i;

		// Skip not connected pins.
		if (i > 8 && i < 14) {
			SR_IN_CLK_DOWN;
			SR_IN_CLK_UP;

			continue;
		}
		else {
			sr_in[ii] = 1 ^ (((1 << SR_IN_DATA) & SR_IN_DATA_PIN) >> SR_IN_DATA);
			--ii;
		}

		SR_IN_CLK_DOWN;
		SR_IN_CLK_UP;

	}

	SR_IN_PL_DOWN;
}



/**
 * Caution this is a interrupt routine, please don't overhead the function.
 */
void screen_render()
{
	screen_unset_digit();
	sr_screen_upd();
	if (screen_render_current_s == 1) {
		SR_SCREEN_SELECT_MIDDLE;
		screen_render_current_s = 2;
		screen_set_digit(screen_render_s2_digit);
		#ifdef USE_METERS
			if (screen_render_s2_digit < 10) {
				sr_screen[SCREEN_P] = 1;
				sr_screen_upd();
			}
		#endif

	}
	else if (screen_render_current_s == 2) {
		SR_SCREEN_SELECT_RIGHT;
		screen_render_current_s = 3;
		screen_set_digit(screen_render_s3_digit);
	}
	else {
		SR_SCREEN_SELECT_LEFT;
		screen_render_current_s = 1;
		screen_set_digit(screen_render_s1_digit);
	}
}

void game_start_complete_script()
{
	#ifdef USE_METERS
		screen_render_s1_digit = 11;
	#else
		screen_render_s1_digit = 0;
	#endif
	screen_render_s3_digit = screen_render_s2_digit = 0;
	screen_number = 0;
	sr_out_clr();
	SR_OUT_SET(START);
	sr_out_upd();
}

void game_idle_action_script()
{
	screen_upd_number(record);
	uint8_t start_bit = sr_out[START] ^ 1;
	sr_out_clr();
	sr_out[START] = start_bit;
	if (record_btn != START) {
		SR_OUT_SET(record_btn);
	}

	sr_out_upd();
}

void game_start_blink_off()
{
	screen_render_s1_digit = 11;
	screen_render_s2_digit = screen_render_s3_digit = 11;
	sr_out_off();
}

void game_start_blink_on()
{
	screen_render_s1_digit = 10;
	screen_render_s2_digit = screen_render_s3_digit = 10;
	sr_out_on();
}
