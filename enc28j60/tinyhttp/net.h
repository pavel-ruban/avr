#ifndef __net_h__
#define __net_h__


// Ethernet
#define ETH_SIZE		14
#define ETH_DST			0
#define ETH_SRC			6
#define ETH_TYPE		12

#define ETHTYPE_IP		0x0800
#define ETHTYPE_ARP		0x0806


// ARP
#define	ARP_SIZE		28
#define	ARP_HTYPE		0
#define ARP_PTYPE		2
#define ARP_HLEN		4
#define ARP_PLEN		5
#define ARP_OPER		6
#define ARP_SHA			8
#define ARP_SPA			14
#define	ARP_THA			18
#define ARP_TPA			24

#define ARP_REQUEST		1
#define ARP_REPLY		2


// IP
#define IP_SIZE			20
#define IP_VER_HLEN		0
#define IP_TOS			1
#define IP_LENGTH		2
#define IP_ID			4
#define IP_FLAGS_FO		6
#define IP_TTL			8
#define IP_PROTO		9
#define IP_HCS			10
#define IP_SRC			12
#define IP_DST			16

#define IP_PROTO_ICMP	1
#define IP_PROTO_TCP	6
#define IP_PROTO_UDP	17


// ICMP
#define ICMP_SIZE		4
#define ICMP_TYPE		0
#define ICMP_CODE		1
#define ICMP_CHECKSUM	2

#define ICMP_ECHO_REQ	8
#define ICMP_ECHO_REPLY	0


// TCP
#define TCP_SIZE		20
#define TCP_SRCPORT		0
#define TCP_DSTPORT		2
#define TCP_SEQNUM		4
#define TCP_ACKNUM		8
#define TCP_DO			12
#define TCP_FLAGS		13
#define TCP_WINDOW		14
#define TCP_CHECKSUM	16
#define TCP_URGENT		18

#define FIN				0
#define SYN				1
#define	RST				2
#define	PSH				3
#define ACK				4
#define URG				5
#define ECE				6
#define CWR				7

#endif
