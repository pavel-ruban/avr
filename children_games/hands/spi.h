#ifndef SPI_CONFIG_H
	#define SPI_CONFIG_H

	#include <avr/io.h>
	#include	"config.h"

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
	#define ENC28J60_SPI_PORT		ENC28J60_CS_PORT
	#define ENC28J60_SPI_DDR		ENC28J60_CS_DDR

	// Chip select lines.
	#define DEFAULT_SPI_CS		PB2
	#define RC522_SPI_CS		RC522_CS
	#define ENC28J60_SPI_CS		ENC28J60_CS
#endif

#ifndef SPI_H
	#define SPI_H

	#include <stdint.h>

	void spi_init();
	uint8_t spi_transmit(uint8_t data);

	#define rc522_select() SPI_PORT &= ~(1 << RC522_SPI_CS)
	#define rc522_release() SPI_PORT |= (1 << RC522_SPI_CS)

	#define enc28j60_select() ENC28J60_SPI_PORT &= ~(1 << ENC28J60_SPI_CS)
	#define enc28j60_release() ENC28J60_SPI_PORT |= (1 << ENC28J60_SPI_CS)

#endif
