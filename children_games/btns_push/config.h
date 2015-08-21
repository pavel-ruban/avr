// This file provides abstraction entities to split program interface from
// architecture especial in order to be configured simply & avoid each
// time adjusting when new MCU is used.

// Game modes
#define MOD_GAME_START		0
#define MOD_GAME_SELECT		3
#define MOD_GAME_OVER		4
#define MOD_GAME		1
#define MOD_IDLE		2

#define SUB_MOD_COMMON		0
#define SUB_MOD_LEFT_GAME_OVER	1
#define SUB_MOD_RIGHT_GAME_OVER	2

// Games
#define	SINGLE_GAME_COUNT	0
#define	SINGLE_GAME_TIME	1
#define	MULTIPLAYER_GAME_COUNT	2
#define	MULTIPLAYER_GAME_TIME	3

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
#define	SR_OUT_DATA_PORT	PORTC
#define	SR_OUT_DATA_DDR		DDRC
#define	SR_OUT_DATA		4

#define	SR_OUT_STROBE_PORT	PORTC
#define	SR_OUT_STROBE_DDR	DDRC
#define	SR_OUT_STROBE		3

#define	SR_OUT_CLK_PORT		PORTC
#define	SR_OUT_CLK_DDR		DDRC
#define	SR_OUT_CLK		5

#define	SR_IN_DATA_PORT		PORTC
#define	SR_IN_DATA_PIN		PINC
#define	SR_IN_DATA_DDR		DDRC
#define	SR_IN_DATA		0

#define	SR_IN_PL_PORT		PORTC
#define	SR_IN_PL_DDR 		DDRC
#define	SR_IN_PL      		1

#define	SR_IN_CLK_PORT		PORTC
#define	SR_IN_CLK_DDR		DDRC
#define	SR_IN_CLK		2

#define	SR_IN_FIRST_PORT	PORTD
#define	SR_IN_FIRST_PIN		PIND
#define	SR_IN_FIRST_DDR		DDRD
#define	SR_IN_FIRST		0

#define	SR_IN_SECOND_PORT	PORTD
#define	SR_IN_SECOND_PIN	PIND
#define	SR_IN_SECOND_DDR	DDRD
#define	SR_IN_SECOND		1

#define	SR_IN_THIRD_PORT	PORTD
#define	SR_IN_THIRD_PIN		PIND
#define	SR_IN_THIRD_DDR		DDRD
#define	SR_IN_THIRD		3

// Led indicators
#define	LED_PORT	PORTD
#define	LED_DDR		DDRD
#define	LED		4

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
