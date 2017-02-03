#ifndef COMMON_CONFIG

#define COMMON_CONFIG

// This file provides abstraction entities to split program interface from
// architecture especial in order to be configured simply & avoid each
// time adjusting when new MCU is used.

// Common
#define BTNS_COUNT	1
#define	NOISE_FILTER_THRESHOLD 2

// SD Status
#define	SD_STATUS_PORT		PORTC
#define	SD_STATUS_PIN		PINC
#define	SD_STATUS		2

// UART
#define	RX_PORT			PORTD
#define	RX_PIN			PIND
#define	RX			0

#define	TX_PORT			PORTD
#define	TX_PIN			PIND
#define	TX			0

#define LCD_RESET_PORT	PORTD
#define LCD_RESET_DDR	DDRD
#define LCD_RESET	2

// Audio
#define	AUDIO_PWM_A_PORT	PORTD
#define	AUDIO_PWM_A_DDR		DDRD
#define	AUDIO_PWM_A		6

#define	AUDIO_PWM_B_PORT	PORTD
#define	AUDIO_PWM_B_DDR		DDRD
#define	AUDIO_PWM_B		5

// Led indicators
#define	LED_PORT		PORTC
#define	LED_DDR			DDRC
#define	LED			0

#endif
