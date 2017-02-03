#define IS_SCREEN_SRC

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

uint8_t sr_screen[8];
uint8_t screen_render_digits[3];
uint8_t screen_current_digit;
uint16_t screen_number;

extern uint8_t is_float;

void screen_set_digit(uint8_t digit);
void screen_upd_number(uint16_t number);
void sr_screen_upd();

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
	else if (digit == SCREEN_DASH) {
			sr_screen[SCREEN_A] = 0;
			sr_screen[SCREEN_B] = 0;
			sr_screen[SCREEN_C] = 0;
			sr_screen[SCREEN_D] = 0;
			sr_screen[SCREEN_E] = 0;
			sr_screen[SCREEN_F] = 0;
			sr_screen[SCREEN_G] = 1;
			sr_screen[SCREEN_P] = 0;

	}
	else if (digit == SCREEN_BLANK) {
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

void screen_upd_number(uint16_t number)
{
	uint8_t current_screen_left_digit = 0;
	uint8_t current_screen_middle_digit = 1;
	uint8_t current_screen_right_digit = 2;

	if (number == 1000 && is_float) is_float = 0;
	else if (number == 999 && !is_float) is_float = 1;
	else if (number == 0xffff && is_float) is_float = 0;

	if (number > 10000) number = 9999;
	if (number > 999 & is_float) is_float = 0;
	else if (number < 1000 & !is_float) is_float = 1;

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

void screen_select_digit(uint8_t digit)
{
	switch (digit)
	{
		case SCREEN_LEFT_DIGIT_I:
			SCREEN_RELEASE(SCREEN_MIDDLE_DIGIT_PORT, SCREEN_MIDDLE_DIGIT);
			SCREEN_RELEASE(SCREEN_RIGHT_DIGIT_PORT, SCREEN_RIGHT_DIGIT);
			SCREEN_SELECT(SCREEN_LEFT_DIGIT_PORT, SCREEN_LEFT_DIGIT);
			break;

		case SCREEN_MIDDLE_DIGIT_I:
			SCREEN_RELEASE(SCREEN_LEFT_DIGIT_PORT, SCREEN_LEFT_DIGIT);
			SCREEN_RELEASE(SCREEN_RIGHT_DIGIT_PORT, SCREEN_RIGHT_DIGIT);
			SCREEN_SELECT(SCREEN_MIDDLE_DIGIT_PORT, SCREEN_MIDDLE_DIGIT);
			break;

		case SCREEN_RIGHT_DIGIT_I:
			SCREEN_RELEASE(SCREEN_LEFT_DIGIT_PORT, SCREEN_LEFT_DIGIT);
			SCREEN_RELEASE(SCREEN_MIDDLE_DIGIT_PORT, SCREEN_MIDDLE_DIGIT);
			SCREEN_SELECT(SCREEN_RIGHT_DIGIT_PORT, SCREEN_RIGHT_DIGIT);
			break;
	}
}

void screen_render()
{
	SCREEN_CLR;
	sr_screen_upd();

	if (screen_current_digit > 2) screen_current_digit = 0;
	screen_select_digit(screen_current_digit);
	screen_set_digit(screen_render_digits[screen_current_digit]);

	if (screen_render_digits[screen_current_digit] < 10)
	{
		if (is_float && screen_current_digit == SCREEN_MIDDLE_DIGIT_I) {
			sr_screen[SCREEN_P] = 1;
		}
	}

	sr_screen_upd();
	++screen_current_digit;
}
