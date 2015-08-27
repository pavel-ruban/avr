#include <spi.h>

#if SPI_CONFIG_AS_MASTER
void spi_init()
{

	SPI_DDR |= (1 << SPI_MOSI) | ( 1 << SPI_SCK) | (1 << SD_SPI_CS);


	SPI_DDR &= ~(1 << SPI_MISO);

	// By default tri state SPI devices.
	SPI_PORT |= (1 << DEFAULT_SPI_CS);
	SPI_PORT &= ~(1 << SPI_MISO);

	SD_SPI_PORT |= (1 << SD_SPI_CS);

	SPCR = (1 << SPE) | (1 << MSTR);// | (1 << SPR0) | (1 << SPR1);
	//SPSR |= (1<<SPI2X); // Maximum speed
}

uint8_t spi_transmit(uint8_t data)
{
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));

	return SPDR;
}

#else
void spi_init()
{
	SPI_DDR = (1<<SPI_MISO);
	SPCR = (1<<SPE);
}

uint8_t spi_transmit(uint8_t data)
{
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}
#endif
