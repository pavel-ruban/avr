#ifndef __config_h__
#define __config_h__

#define ENC_DDR		DDRB
#define ENC_PORT	PORTB
#define ENC_SEL		PB4
#define ENC_DI		PB6
#define ENC_DO		PB5
#define ENC_SCK		PB7


#define MAC0		0x12
#define MAC1		0x34
#define MAC2		0x56
#define MAC3		0x78
#define MAC4		0x9a
#define MAC5		0xbc

#define IP0			192
#define IP1			168
#define IP2			0
#define IP3			100

#define TCP_PORT	80

#define RX_END		0x17ff
#define TX_START	0x1800
#define TX_BASE		(TX_START+1)

#define PBUF_SZ		64
#define IP_TTLVALUE	128

#define HTTP_SERV	"ATtiny2313"

#define DATA_PORT	PORTB
#define DATA_DDR	DDRB
#define DATA_BITS	4

#endif
