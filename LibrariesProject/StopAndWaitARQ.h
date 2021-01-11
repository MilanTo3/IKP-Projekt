#pragma once
#include <ws2tcpip.h>
#include "stdio.h"
#include "stdlib.h"

int snwarq_sendto(SOCKET uticnica, char* data, int duzinapodataka, int flag, const sockaddr* destination, int destinationlen);
int snwarq_recvfrom(SOCKET uticnica, char* data, int duzinapodataka, int flag, const sockaddr* clientaddress, int *clientlen);