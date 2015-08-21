#define F_CPU 8000000UL
#define	DOOR_PORT	PORTC
#define	PDOOR		PC5
#define	SIGNAL_PORT	PORTD
#define	PSIGNAL		PD6

#define EREVID                 (0x12 | 0x60)

#include <avr/io.h>
#include <avr/pgmspace.h>
//#include <utils.h>
#include <spi.h>
#include <mfrc522.h>
#include <util/delay.h>
#include "lan.h"

extern uint16_t CTCNT2;
extern uint16_t CTCNT3;
extern uint16_t CTCNT4;
extern uint16_t CTCNT5;
extern uint8_t *OP_OPEN;

extern uint8_t mac_addr[6];
extern uint32_t ip_addr;
extern uint32_t ip_mask;
extern uint32_t ip_gateway;
extern uint8_t net_buf[ENC28J60_MAXFRAME];
extern uint8_t NET_ACTIVITY;
extern uint8_t RFID_ACTIVITY;
extern uint8_t OPEN_SIGNAL;
extern uint8_t OPEN_DOOR;
extern uint8_t DENIED_SIGNAL;


char *term_var_info;
char term_var_info_array[20];

uint8_t byte;
uint8_t str[MAX_LEN];
uint16_t delta;

void open_door_script();
void close_door_script();
void deny_signal();
extern void clear_tcn(uint16_t *);

//const char retval[] PROGMEM = "ocean is deep";
//void fill_net_buff(uint8_t *, uint8_t);

void udp_packet(eth_frame_t *frame, uint16_t len)
{
	ip_packet_t *ip = (void*)(frame->data);
	udp_packet_t *udp = (void*)(ip->data);

	volatile uint8_t *data = udp->data;
	uint16_t count;

	NET_ACTIVITY = 1;

	uint8_t chars[15];
	uint8_t ch;

	uint8_t i = 0;

	if (
		*(data + 0) == 'c' &&
		*(data + 1) == 'o' &&
		*(data + 2) == 'm' &&
		*(data + 3) == 'm' &&
		*(data + 4) == 'a' &&
		*(data + 5) == 'n' &&
		*(data + 6) == 'd' &&
		*(data + 7) == ':' &&
		*(data + 8) == ' ' &&
		*(data + 9) == 'o' &&
		*(data + 10) == 'p' &&
		*(data + 11) == 'e' &&
		*(data + 12) == 'n'
	)
	{
		open_door_script();
		close_door_script();
	}
	if (
		*(data + 0) == 'd' &&
		*(data + 1) == 'e' &&
		*(data + 2) == 'n' &&
		*(data + 3) == 'i' &&
		*(data + 4) == 'e' &&
		*(data + 5) == 'd'
	)
	{
		deny_signal();
	}

	*data = 'o';
	*(data + 1) = 'k';

	count = 2;

	udp_reply(frame, count);

	clear_tcn(&CTCNT3);
}

void data_init()
{
	PORTB |= 1 << 7;
	term_var_info = term_var_info_array;
	*(term_var_info) = 'x';
	*(term_var_info + 1) = 'x';
	*(term_var_info + 2) = 'x';
	*(term_var_info + 3) = 'x';
	*(term_var_info + 4) = 'x';
	*(term_var_info + 5) = 'x';
	*(term_var_info + 6) = 'x';

	_delay_ms(1500);

	PORTB &= ~(1 << 7);
}

void main_init()
{
	PORTB |= 1 << 7;

	spi_init();

	enc28j60_release();

	mac_addr[0] = 0x00;
	mac_addr[1] = 0x4e;
	mac_addr[2] = 0x42;
	mac_addr[3] = 0x4c;
	mac_addr[4] = 0x55;
	mac_addr[5] = 0x45;

	ip_addr = IP_ADDR;
	ip_mask = inet_addr(255,255,255,0);
	ip_gateway = inet_addr(192,168,0,1);

	lan_init();
	//_delay_ms(250);
	_delay_ms(1500);

	*(term_var_info) = enc28j60_rcr(EREVID);
	PORTB &= ~(1 << 7);
}

void end_signal()
{
/*
	for (uint8_t i = 0; i < 1; ++i)
	{
		uint32_t c = 1500;

		while (c)
		{
			SIGNAL_PORT ^= 1 << PSIGNAL;
			--c;
		}

		SIGNAL_PORT &= ~(1 << PSIGNAL);
		c = 89500;

		while (c)
		{
			--c;
		}

	}
*/
	SIGNAL_PORT &= ~(1 << PSIGNAL);
}

/**
void door_delay()
{
	//OPEN_DOOR = 1;
	//clear_TCN2();
	//_delay_ms(220);
}
*/

void deny_signal()
{

	DENIED_SIGNAL = 1;
/*
	uint32_t c = 13850;
	uint8_t x = 0;

	while (c)
	{
		if (x == (delta + 5)) {
			SIGNAL_PORT ^= 1 << PSIGNAL;
			x = 0;
		}
		++x;
		--c;
	}

	SIGNAL_PORT &= ~(1 << PSIGNAL);
	*/
}

void ok_signal()
{
	OPEN_SIGNAL = 1;
/*
	for (uint8_t i = 0; i < 3; ++i)
	{
		uint32_t c = 850;
		for (uint8_t i = 0; i < 30; ++i)
		{
			uint8_t x = 0;
			while (c)
			{
				if (x == delta) {
					SIGNAL_PORT ^= 1 << PSIGNAL;
					x = 0;
				}
				++x;
				--c;
			}

			SIGNAL_PORT &= ~(1 << PSIGNAL);
			c = 50;

			while (c)
			{
				--c;
			}

		}
		c = 17550;

		while (c)
		{
			--c;
		}
	}
*/
}

void open_door()
{
	//DOOR_PORT &= ~(1 << PDOOR);
	OPEN_DOOR = 1;
	OPEN_SIGNAL = 1;
	clear_tcn(&CTCNT2);
}

void close_door()
{
//	DOOR_PORT |= 1 << PDOOR;
}

void open_door_script()
{
	//PORTC |= 1 << 1;
	open_door();
	//ok_signal();
	//door_delay();
}

void close_door_script()
{
/*
	PORTC &= ~(1 << 1);
	close_door();
	end_signal();
**/
}

void send_packet()
{
//	byte = mfrc522_request(PICC_REQALL,str);
//
//	if(byte == CARD_FOUND)
//	{
//		RFID_ACTIVITY = 1;
//		clear_tcn(&CTCNT4);
//		clear_tcn(&CTCNT5);
//
//		if (NET_ACTIVITY) return;
//
//		byte = mfrc522_get_card_serial(str);
//		if(byte == CARD_FOUND)
//		{


			eth_frame_t *frame = (void *) net_buf;
			ip_packet_t *ip = (void *)(frame->data);
			udp_packet_t *udp = (void *)(ip->data);
			frame->type = ETH_TYPE_IP;

			frame->from_addr[0] = 0x00;
			frame->from_addr[1] = 0xe0;
			frame->from_addr[2] = 0x41;
			frame->from_addr[3] = 0x22;
			frame->from_addr[4] = 0x0A;
			frame->from_addr[5] = 0xEE;

			udp->from_port = htons(APP_PORT);
			udp->to_port = htons(APP_PORT);

			*(udp->data + 0) = 'c';
			*(udp->data + 1) = 'a';
			*(udp->data + 2) = 'r';
			*(udp->data + 3) = 'd';
			*(udp->data + 4) = ':';
			*(udp->data + 5) = ' ';

			byte = 0;
//			while (byte < 4)
//			{
//				*(udp->data + 6 + byte) = str[byte];
//				*(term_var_info +  byte) = str[byte];
//				++byte;
//			}

			ip->ver_head_len = 0x45;

			//ip->from_addr = inet_addr(192,168,0,47);
			ip->from_addr = dest_ip_addr;
			ip->to_addr = ip_addr;
			ip->protocol = IP_PROTOCOL_UDP;
			ip->tos = 0;

			udp_reply(frame, 6);

//			*(term_var_info + byte) = 0x00;
//		}
//		else {
//			*(term_var_info) = 0x00;
//		}
//	}
//	else {
//		*(term_var_info) = 0x00;
//	}
}
