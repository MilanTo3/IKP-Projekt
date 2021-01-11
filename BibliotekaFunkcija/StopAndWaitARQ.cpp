#include "pch.h"
#include "ws2tcpip.h"
#include "StopAndWaitARQ.h"
#include "TCPheader.h"

int snwarq_sendto(SOCKET uticnica, char *data, int duzinapodataka, int flag, const sockaddr* destination, int destinationlen) {

	int res = udp_sendto(uticnica, data, duzinapodataka, destination, destinationlen);

	return res;
}

int snwarq_recvfrom(SOCKET uticnica, char *data, int duzinapodataka, int flag, const sockaddr* clientaddress, int *clientlen) {

	int res = udp_recvfrom(uticnica, data, duzinapodataka, flag, clientaddress, clientlen);

	return res;
}

