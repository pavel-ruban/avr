#include <avr/io.h>
#include "lan.h"

char *term_var_info;
char term_var_info_array[30];

extern uint8_t mac_addr[6];
extern uint32_t ip_addr;
extern uint32_t ip_mask;
extern uint32_t ip_gateway;
extern uint8_t net_buf[ENC28J60_MAXFRAME];

extern uint8_t send_net_buf[32 + 64];
extern uint8_t send_buf_prepared;

void udp_packet(eth_frame_t *frame, uint16_t len)
{
	ip_packet_t *ip = (void*)(frame->data);
	udp_packet_t *udp = (void*)(ip->data);

	uint8_t *data = udp->data;
	uint8_t i, count;

	count = 10;
	char reply_str[10];
	reply_str[0] = 'I';
	reply_str[1] = 'I';
	reply_str[2] = 'I';
	reply_str[3] = 'I';
	reply_str[4] = 'I';
	reply_str[5] = 'I';
	reply_str[6] = 'I';
	reply_str[7] = 'I';
	reply_str[8] = 'I';
	reply_str[9] = 'I';

	for(i = 0; i < count; ++i)
		*(data + i) = reply_str[i];

	if (send_buf_prepared == 0)
	{
		send_buf_prepared = 1;

		memcpy(send_net_buf, frame, 32 + 64);
	}

	udp_reply(frame, count);
}

void main_init()
{
	mac_addr[0] = 0x00;
	mac_addr[1] = 0x13;
	mac_addr[2] = 0x37;
	mac_addr[3] = 0x01;
	mac_addr[4] = 0x23;
	mac_addr[5] = 0x45;

	ip_addr = IP_ADDR;
	ip_mask = inet_addr(255,255,255,0);
	ip_gateway = inet_addr(192,168,0,47);
	term_var_info = term_var_info_array;
	send_buf_prepared = 0;
	lan_init();
}

void main_loop()
{
	lan_poll();
}

void send_loop()
{
	if (send_buf_prepared != 0)
	{
		PORTC ^= 1;

		eth_frame_t *frame = (eth_frame_t *)send_net_buf;
		ip_packet_t *ip = (ip_packet_t *) frame->data;
		udp_packet_t *udp = (udp_packet_t *) ip->data;
		uint8_t *data = (void *) udp->data;
/*
		memcpy(&frame->to_addr[0], &frame->from_addr[0], 6);
		memcpy(&frame->from_addr[0], &mac_addr[0], 6);
		ip->to_addr = inet_addr(192,168,0,47);
		ip->from_addr = inet_addr(192,168,0,11);
		udp->to_port = htons(291);
		udp->from_port = htons(291);
		*/
		*(udp->data + 0) = 'l';
		*(udp->data + 1) = 'i';
		*(udp->data + 2) = 'n';
		*(udp->data + 3) = 'u';
		*(udp->data + 4) = 'x';
		*(udp->data + 5) = 't';
		*(udp->data + 6) = 't';
		*(udp->data + 7) = 't';
		*(udp->data + 8) = 't';
		*(udp->data + 9) = 't';

		udp_reply(frame, 10);
	}
	*term_var_info = send_buf_prepared;
	*(term_var_info + 1) = 0;
}
