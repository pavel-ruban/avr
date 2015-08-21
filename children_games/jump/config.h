// This file provides abstraction entities to split program interface from
// architecture especial in order to be configured simply & avoid each
// time adjusting when new MCU is used.

// Game modes
#define MOD_GAME_START		0
#define MOD_GAME		1
#define MOD_IDLE		2

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

#define	SR_SCREEN_S1_PORT		PORTD
#define	SR_SCREEN_S1_DDR		DDRD
#define	SR_SCREEN_S1			1

#define	SR_SCREEN_S2_PORT		PORTD
#define	SR_SCREEN_S2_DDR		DDRD
#define	SR_SCREEN_S2			0

#define	SR_SCREEN_S3_PORT		PORTD
#define	SR_SCREEN_S3_DDR		DDRD
#define	SR_SCREEN_S3			3
