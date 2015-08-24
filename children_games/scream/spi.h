#ifndef SPI_CONFIG_H
	#define SPI_CONFIG_H

	#include <avr/io.h>
	#include "config.h"

	/*
	 * Set to 1, spi api will work in master mode
	 * else in slave mode
	 */
	#define SPI_CONFIG_AS_MASTER 	1


	/*
	 * Config SPI pin diagram
	 */
	#define SPI_DDR			DDRB
	#define SPI_PORT		PORTB
	#define SPI_PIN			PINB
	#define SPI_MOSI		PB3
	#define SPI_MISO		PB4
	#define SPI_SCK			PB5

	// Beyond the default SPI portc C.
	#define SD_SPI_PORT		PORTB
	#define SD_SPI_DDR		DDRB

	// Chip select lines.
	#define DEFAULT_SPI_CS		PB2
	#define SD_SPI_CS		PB2
#endif

#ifndef SPI_H
	#define SPI_H

	#include <stdint.h>

	void spi_init();
	uint8_t spi_transmit(uint8_t data);
	uint8_t spi_receive();

	#define default_cs_select() SPI_PORT &= ~(1 << DEFAULT_SPI_CS)
	#define default_cs_release() SPI_PORT |= (1 << DEFAULT_SPI_CS)

	#define sd_select() SD_SPI_PORT &= ~(1 << SD_SPI_CS)
	#define sd_release() SD_SPI_PORT |= (1 << SD_SPI_CS)

#endif
