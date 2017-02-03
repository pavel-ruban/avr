#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "spi.h"
#include "lcd.h"

void lcd_init()
{
	_delay_ms(40);
	// @todo check if the condition about turning reset to low time limit (100ns) is true.
	LCD_RESET_PORT &= ~(1 << LCD_RESET);
	_delay_us(1);
	// Wait until LCD MCU is initialized.
	LCD_RESET_PORT |= 1 << LCD_RESET;
}

void lcd_cmd(uint16_t cmd)
{
	lcd_select();
	_delay_us(100);
	// Send start byte. RC (Register Select depends on cmd).
	spi_transmit(LCD_START_BYTE | (0x2 & (cmd >> 7)));
	_delay_us(100);
	spi_transmit(LCD_HI8(cmd));
	_delay_us(100);
	spi_transmit(LCD_LO8(cmd));
	_delay_us(100);
	lcd_release();
}
