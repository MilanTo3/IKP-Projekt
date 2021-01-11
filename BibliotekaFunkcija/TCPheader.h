#pragma once
#include <ws2tcpip.h>
#include "stdio.h"
#include "stdlib.h"

typedef struct data_struct {

	int size;
	char* data;

}Datastruct;

int udp_sendto(SOCKET uticnica, char* data, int duzinapodataka, const sockaddr* destination, int destinationlen);
int udp_recvfrom(SOCKET uticnica, char* data, int duzinapodataka, int flag, const sockaddr* clientaddress, int *clientlen);