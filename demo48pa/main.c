#include <stdint.h>
#include "avr.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define SR_CLK_DOWN \
	SR_CLK_PORT &= ~(1 << SR_CLK)

#define SR_CLK_UP \
	SR_CLK_PORT |= 1 << SR_CLK

#define SR_STROBE_DOWN \
	SR_STROBE_PORT &= ~(1 << SR_STROBE)

#define SR_STROBE_UP \
	SR_STROBE_PORT |= 1 << SR_STROBE

void shift_reg_upd()
{
	int x = 2 + 2;
}

//void delay_05s(void) {
// volatile unsigned int i,j;
// for (i=); i
//
//}

uint8_t s = 0;
uint8_t data;
uint8_t xxx = 0;
uint8_t yyy = 0;

void sr_init()
{
	uint8_t pos = 8;
	uint8_t data0 = 0b10000000;
	uint8_t data1 = 0b01000000;
	uint8_t data2 = 0b00100000;
	uint8_t data3 = 0b00010000;
	uint8_t data4 = 0b00001000;
	uint8_t data5 = 0b00000100;
	uint8_t data6 = 0b00000010;
	uint8_t data7 = 0b00000001;

	SR_CLK_DOWN;
	SR_STROBE_UP;

	if (s == 0) data = data0;
	else if (s == 1) data = data1;
	else if (s == 2) data = data2;
	else if (s == 3) data = data3;
	else if (s == 4) data = data4;
	else if (s == 5) data = data5;
	else if (s == 6) data = data6;
	else if (s == 7) data = data7;

	while (pos)
	{

		--pos;
		SR_DATA_PORT ^= (-((data & (1 << pos)) ? 1 : 0) ^ SR_DATA_PORT) & (1 << SR_DATA);

		SR_CLK_UP;
		SR_CLK_DOWN;
	}

	++s;
	if (s > 7) s = 0;

	SR_STROBE_DOWN;
	_delay_ms(350);
	SR_STROBE_UP;
}

int q[8];
void sr_pl_test()
{
	SR_CLK_PORT |= 1 << SR_CLK;
	SR_PL_PORT |= 1 << SR_PL;

	int *cq = &q[7] + 1;

	do
	{
		--cq;
		SR_CLK_PORT &= ~(1 << SR_CLK);
		SR_CLK_PORT |= 1 << SR_CLK;
		*cq = ((1 << SR_DATA) & SR_DATA_PIN) >> SR_DATA;

	} while (cq > &q[0]);

	SR_PL_PORT &= ~(1 << SR_PL);

	if (q[0]) {
		PORTB |= 1 << 1;
	}
	else {
		PORTB &= ~(1 << 1);
	}
}

int cnt;
void led_test()
{

	if (cnt > 11) {
		cnt = 0;
		LED_SELECT_PORT |= 1 << LED_SELECT;
	}
	else {
		LED_SELECT_PORT &= ~(1 << LED_SELECT);
	}

	++cnt;
}
