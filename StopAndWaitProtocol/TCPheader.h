#pragma once
#include <ws2tcpip.h>
#include "stdio.h"
#include "stdlib.h"
#include "Helpers.h"

int udp_sendto(SOCKET uticnica, Frame frame, int duzinapodataka, LPSOCKADDR destination, int destinationlen);
int udp_recvfrom(SOCKET uticnica, Frame *frame, int duzinapodataka, int flag, LPSOCKADDR clientaddress, int *clientlen);