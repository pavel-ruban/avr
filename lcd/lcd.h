#define LCD_START_BYTE 0b11111000
#define LCD_RS 0b10
#define LCD_LO8(BYTE) (uint8_t) (0xF0 & (BYTE << 4))
#define LCD_HI8(BYTE) (uint8_t) (BYTE & 0xF0)

#define LCD_DISPLAY_ON			0b100
#define LCD_CURSOR_ON			0b010
#define LCD_BLINK_ON			0b001
#define LCD_CMD_CURSOR_DISPLAY_CONTROL	0b000001000
#define LCD_CMD_HOME			0b000000011
#define LCD_CMD_ENTRY_MODE		0b000000111
#define LCD_CMD_CLEAR			0b000000001
#define LCD_CMD_SET_CGRAM		0b001101011
#define LCD_CMD_FUNCTION_SET		0b000100000

void lcd_init();
void lcd_cmd(uint16_t cmd);
