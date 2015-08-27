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

void io_ports_init();
void uart_init();

uint8_t buffer[50];

void main()
{
	io_ports_init();
	uart_init();

	while (1)
	{
		static uint16_t x = 0;
		double y, z;
		z = M_PI * x / 180;
		y = sin(z);
		sprintf_P(buffer, PSTR("angle %d radians %lf sin %lf\n\r"), x, z, y);
		uart_str_send(buffer);
		++x;
		if (x > 360) {
			x = 0;
		}
	}
}
