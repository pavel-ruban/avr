#include "config.h"

#ifndef SCREEN_INIT

#define SCREEN_INIT

#ifndef IS_SCREEN_SRC
	extern uint8_t sr_screen[8];
	extern uint8_t screen_render_digits[3];
	extern uint8_t screen_current_digit;
	extern uint16_t screen_number;

	extern void screen_set_digit(uint8_t digit);
	extern void screen_upd_number(uint16_t number);
	extern void sr_screen_upd();
#endif

#define	SCREEN_DASH_ROW	10000

#define SCREEN_BLANK	11
#define SCREEN_DASH	10

#define SR_SCREEN_CLK_UP \
	SR_SCREEN_CLK_PORT |= 1 << SR_SCREEN_CLK

#define SR_SCREEN_CLK_DOWN \
	SR_SCREEN_CLK_PORT &= ~(1 << SR_SCREEN_CLK)

#define SR_SCREEN_STROBE_DOWN \
	SR_SCREEN_STROBE_PORT &= ~(1 << SR_SCREEN_STROBE)

#define SR_SCREEN_STROBE_UP \
	SR_SCREEN_STROBE_PORT |= 1 << SR_SCREEN_STROBE

#define SCREEN_SELECT(PORT, PIN) \
	PORT |= 1 << PIN

#define SCREEN_RELEASE(PORT, PIN) \
	PORT &= ~(1 << PIN)

#define SCREEN_LEFT_DIGIT_I 0
#define SCREEN_MIDDLE_DIGIT_I 1
#define SCREEN_RIGHT_DIGIT_I 2

#define SCREEN_CLR					\
	memset(sr_screen, 0, sizeof(sr_screen))

#define	SCREEN_A	7
#define	SCREEN_B	6
#define	SCREEN_C	5
#define	SCREEN_D	4
#define	SCREEN_E	3
#define	SCREEN_F	2
#define	SCREEN_G	1
#define	SCREEN_P	0

#endif
