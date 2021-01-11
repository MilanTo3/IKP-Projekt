#include "pch.h"
#include "TCPheader.h"
#include "ws2tcpip.h"

int udp_sendto(SOCKET uticnica, char* data, int duzinapodataka, const sockaddr* destination, int destinationlen) {

	if (uticnica == INVALID_SOCKET) {
		return 100;
	}
	else if (duzinapodataka < 0) {
		return 101;
	}

	int result = 0;
	int sentbytes = 0;
	Datastruct datastruct;
	datastruct.size = duzinapodataka;
	datastruct.data = data;

	memset(datastruct.data, 0, duzinapodataka);
	memcpy(datastruct.data, data, duzinapodataka);

	while (sentbytes < duzinapodataka) {

		result = sendto(uticnica, ((char*)&datastruct) + sentbytes, duzinapodataka - sentbytes, 0, (LPSOCKADDR)&destination, destinationlen);
		sentbytes += result;
		printf("Sent %d bytes.\n", sentbytes);

	}

	return sentbytes;
}

int udp_recvfrom(SOCKET uticnica, char* data, int duzinapodataka, int flag, const sockaddr* clientaddress, int *clientlen) {

	if (uticnica == INVALID_SOCKET) {
		return 100;
	}
	else if (duzinapodataka < 0) {
		return 101;
	}

	int result = 0;
	int receivedbytes = 0;
	Datastruct datastruct;

	datastruct.data = data;

	if (flag == 0) {
		while (receivedbytes < duzinapodataka) {

			result = recvfrom(uticnica, ((char*)&datastruct) + receivedbytes, duzinapodataka - receivedbytes, flag, (LPSOCKADDR)&clientaddress, clientlen);
			receivedbytes += result;

		}
	}
	else {

		result = recv(uticnica, (char*)&datastruct, duzinapodataka, 0);
		receivedbytes += result;
		printf("Datastruct size in bytes: %d.\n", datastruct.size);

		while (receivedbytes < datastruct.size) {

			result = recvfrom(uticnica, ((char*)&datastruct) + receivedbytes, datastruct.size - receivedbytes, 0, (LPSOCKADDR)&clientaddress, clientlen);
			receivedbytes += result;

		}

	}

	return receivedbytes;
}