#ifndef COMMON_CONFIG

#define COMMON_CONFIG

// This file provides abstraction entities to split program interface from
// architecture especial in order to be configured simply & avoid each
// time adjusting when new MCU is used.

// Common
#define BTNS_COUNT	1
#define	NOISE_FILTER_THRESHOLD 2

// Game modes
#define MOD_IDLE		0
#define MOD_GAME_START		1
#define MOD_GAME		2
#define MOD_GAME_OVER		3

// UART
#define	RX_PORT			PORTD
#define	RX_PIN			PIND
#define	RX			0

#define	TX_PORT			PORTD
#define	TX_PIN			PIND
#define	TX			0

// Audio
#define	AUDIO_PWM_A_PORT	PORTD
#define	AUDIO_PWM_A_DDR		DDRD
#define	AUDIO_PWM_A		6

#define	AUDIO_PWM_B_PORT	PORTD
#define	AUDIO_PWM_B_DDR		DDRD
#define	AUDIO_PWM_B		5

// Hand buttons
#define	START_IN_PORT		PORTC
#define	START_IN_PIN		PINC
#define	START_IN_DDR		DDRC
#define	START_IN		5

#define	START_OUT_PORT		PORTC
#define	START_OUT_DDR		DDRC
#define	START_OUT		4

#define START_OUT_ON				\
	START_OUT_PORT |= 1 << START_OUT

#define START_OUT_OFF				\
	START_OUT_PORT &= ~(1 << START_OUT)

#define START_OUT_INV				\
	START_IN_PORT ^= 1 << START_OUT

// Led indicators
#define	LED_PORT		PORTD
#define	LED_DDR			DDRD
#define	LED			4

// Screen
#define	SR_SCREEN_DATA_PORT		PORTB
#define	SR_SCREEN_DATA_DDR		DDRB
#define	SR_SCREEN_DATA			0

#define	SR_SCREEN_STROBE_PORT		PORTB
#define	SR_SCREEN_STROBE_DDR		DDRB
#define	SR_SCREEN_STROBE		1

#define	SR_SCREEN_CLK_PORT		PORTD
#define	SR_SCREEN_CLK_DDR		DDRD
#define	SR_SCREEN_CLK			7

// Screen selects
#define	SCREEN_LEFT_DIGIT_PORT		PORTC
#define	SCREEN_LEFT_DIGIT_DDR		DDRC
#define	SCREEN_LEFT_DIGIT		3

#define	SCREEN_MIDDLE_DIGIT_PORT	PORTC
#define	SCREEN_MIDDLE_DIGIT_DDR		DDRC
#define	SCREEN_MIDDLE_DIGIT		2

#define	SCREEN_RIGHT_DIGIT_PORT		PORTD
#define	SCREEN_RIGHT_DIGIT_DDR		DDRD
#define	SCREEN_RIGHT_DIGIT		3

#define CLR_CTCNT6()		\
	CTCNT6 = 0;		\
	*(&CTCNT6 + 1) = 0;	\
	*(&CTCNT6 + 2) = 0

#define CLR_CTCNT3()		\
	CTCNT3 = 0;		\
	*(&CTCNT6 + 1) = 0;	\
	*(&CTCNT6 + 2) = 0
#endif

