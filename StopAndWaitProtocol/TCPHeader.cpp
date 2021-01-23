#include "pch.h"
#include "TCPheader.h"
#include "Helpers.h"
#include "ws2tcpip.h"

CRITICAL_SECTION sendlock;
CRITICAL_SECTION recvlock;

int udp_sendto(SOCKET uticnica, Frame frame, int duzinapodataka, LPSOCKADDR destination, int destinationlen) {
	// TODO here

	int result = 0;
	int sentbytes = 0;

	result = sendto(uticnica, (char *)&frame.header, sizeof(udpHeader), 0, destination, destinationlen);

	while (sentbytes < (frame.header.length)) {

		result = sendto(uticnica, frame.data + sentbytes, frame.header.length - sentbytes, 0, destination, destinationlen);
		sentbytes += result;

	}

	return sentbytes;
}

int udp_recvfrom(SOCKET uticnica, Frame *frame, int duzinapodataka, int flag, LPSOCKADDR clientaddress, int *clientlen) {

	int result = 0;
	int receivedbytes = 0;

	result = recvfrom(uticnica, (char *)&frame->header, sizeof(udpHeader), flag, clientaddress, clientlen);

	while (receivedbytes + sizeof(udpHeader) < duzinapodataka && receivedbytes < frame->header.length) {

		result = recvfrom(uticnica, frame->data + receivedbytes, frame->header.length - receivedbytes, 0, clientaddress, clientlen);
		receivedbytes += result;

	}

	return receivedbytes;
}