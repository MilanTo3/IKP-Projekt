#include "pch.h"
#include "ws2tcpip.h"
#include "StopAndWaitARQ.h"
#include "TCPheader.h"
#include "Helpers.h"

int snwarq_sendto(SOCKET uticnica, char *data, int duzinapodataka, int flag, LPSOCKADDR destination, int destinationlen) {

	Frame frame = makeframe(data, duzinapodataka, 0);
	int res = udp_sendto(uticnica, frame, sizeof(Frame), destination, destinationlen);

	return res;
}

int snwarq_recvfrom(SOCKET uticnica, char *data, int duzinapodataka, int flag, LPSOCKADDR clientaddress, int *clientlen) {

	Frame frame;
	memset(&frame, 0, sizeof(Frame));
	int res = udp_recvfrom(uticnica, &frame, duzinapodataka, flag, clientaddress, clientlen);
	memcpy(data, frame.data, frame.header.length);

	return res;
}

