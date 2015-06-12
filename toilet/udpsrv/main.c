#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define APP_PORT 36208

#define CARD_A 7
#define CARD_B 8
#define CARD_C 9
#define CARD_D 10

// Protocol actions.
#define OP_OPEN 1
#define INFO_DENIED 2

#define SOMI_PIPE_ERROR 1

char buff[1000];

int sockfd, bytes;
unsigned int len;

struct sockaddr_in servaddr,cliaddr;

socklen_t len;

struct timeval timeout;

fd_set fds;

void log_packet(int bytes)
{
	buff[bytes] = 0;

	printf("-------------------------------------------------------\n");
	printf("Received the following:\n");

	if (!strncmp("card: ", buff, 6)) {
		printf("%.6s", buff);
		printf("%02X:%02X:%02X:%02X\n", buff[CARD_A] & 0xff, buff[CARD_B] & 0xff, buff[CARD_C] & 0xff, buff[CARD_D] & 0xff);
	}
	else {
		printf("%s\n",buff);
	}

	printf("-------------------------------------------------------\n");
}

int access_handler(uint8_t A, uint8_t B, uint8_t C, uint8_t D)
{
	char cmd[50];

	sprintf(cmd, "somi.sh %02X:%02X:%02X:%02X", buff[CARD_A] & 0xff, buff[CARD_B] & 0xff, buff[CARD_C] & 0xff, buff[CARD_D] & 0xff);

	FILE* pipe = popen(cmd, "r");

	if (!pipe) return SOMI_PIPE_ERROR;

	char buffer[262144];

	int dist = 0;
	int size;

	while(!feof(pipe)) {
		size = (int) fread(buffer, 1, 262144, pipe);

		if (dist + size < sizeof(buff)) {
			memcpy(&buff[dist], buffer, size);
		}
		else {
			size = sizeof(buffer) - dist;
			memcpy(&buff[dist], buffer, size);
			break;
		}

		dist += size;
	}

	pclose(pipe);

	printf("\nexternal retval %s\n\n", buff);

	return 0;
}

void reliable_packet_delivery(unsigned int type)
{
	while (1)
	{
		timeout.tv_sec = 0;
		timeout.tv_usec = 300000;

		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);

		switch (type)
		{
			case OP_OPEN:
				strcpy((void *) &buff, "command: open");
				bytes = 13;

				printf("opening the door\n");
				break;

			case INFO_DENIED:
				strcpy((void *) &buff, "denied");
				bytes = 6;

				printf("deny access informing\n");
				break;

			default:
				return;
		}

		// Return value:
		// -1: error occurred
		// 0: timed out
		// > 0: data ready to be read
		printf("packet: %s\n", buff);
		sendto(sockfd, buff, bytes, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

		int retval = select(sockfd + 1, &fds, NULL, NULL, &timeout);
		switch (retval)
		{
			case -1:
				printf("Error\n");
				break;

			case 0:
				printf("Timeout MCU doesn't respond, resending packet\n");
				break;

			default:
				bytes = recvfrom(sockfd, buff, 1000, 0, (struct sockaddr *) &cliaddr, &len);

				log_packet(bytes);

				if (!strncmp("ok", buff, 2)) {
					printf("MCU have been received packet\n");

					return;
				}
				break;
		}
	}
}

int main(int argc, char**argv)
{
	sockfd = socket(AF_INET,SOCK_DGRAM,0);

	len = sizeof(cliaddr);

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(APP_PORT);

	bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	while (1)
	{
		bytes = recvfrom(sockfd, buff, 1000, 0, (struct sockaddr *) &cliaddr, &len);

		log_packet(bytes);

		if (!strncmp("card: ", buff, 6)) {
			if (access_handler(buff[CARD_A], buff[CARD_B], buff[CARD_C], buff[CARD_D]))
			{
				printf("popen failed to open SOMI manager\n");
			}

			if (!strncmp("granted", buff, 7)) {
				reliable_packet_delivery(OP_OPEN);
			}
			else if (!strncmp("denied", buff, 6)) {
				reliable_packet_delivery(INFO_DENIED);
			}
			else {
				printf("Error the SOMI third party manager doesn't response or returns garbage!\n");
			}
		}
		else if (!strncmp("command: admin open", buff, 19)) {
			cliaddr.sin_family = AF_INET;
			cliaddr.sin_addr.s_addr = inet_addr("192.168.1.75");
			cliaddr.sin_port = htons(36208);

			reliable_packet_delivery(OP_OPEN);
		}
	}
}
