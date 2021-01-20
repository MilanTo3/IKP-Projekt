#include "pch.h"
#include "ws2tcpip.h"
#include "StopAndWaitARQ.h"
#include "TCPheader.h"
#include "Helpers.h"

int snwarq_sendto(SOCKET uticnica, char *data, int duzinapodataka, int flag, LPSOCKADDR destination, int destinationlen) {

	unsigned int datapointer = 0;
	int tosend = duzinapodataka;
	int res = 0;
	int returnsignal = -1;
	unsigned int sequence = 0;
	Frame frame;
	struct timeval timevalue;

	if (uticnica == INVALID_SOCKET) {
		return 100;
	}
	else if (duzinapodataka < 0) {
		return 101;
	}

	while (datapointer < duzinapodataka){

		if (tosend > FrameDataSize) {
			frame = makeframe(data + (sequence * FrameDataSize), FrameDataSize, sequence, false);
			tosend = tosend - FrameDataSize;
			datapointer += FrameDataSize;
		}
		else {
			frame = makeframe(data + (sequence * FrameDataSize), tosend, sequence, true);
			datapointer += tosend;
		}

		res = udp_sendto(uticnica, frame, sizeof(Frame), destination, destinationlen);

		sequence++;

		//timevalue.tv_sec = 10;
		//timevalue.tv_usec = 0;
		//
		//if (setsockopt(uticnica, SOL_SOCKET, SO_RCVTIMEO, (char *)&timevalue, sizeof(timevalue)) < 0) {
		//	perror("Greška prilikom socket timeout operacije.");
		//	return 102;
		//}

	}

	return res;
}

int snwarq_recvfrom(SOCKET uticnica, char *data, int duzinapodataka, int flag, LPSOCKADDR clientaddress, int *clientlen) {

	if (uticnica == INVALID_SOCKET) {
		return 100;
	}
	else if (duzinapodataka < 0) {
		return 101;
	}

	Frame frame;
	bool lastframe = false;
	unsigned int datapointer = 0;
	int res = 0;

	while (!lastframe) {

		memset(&frame, 0, sizeof(Frame));
		res = udp_recvfrom(uticnica, &frame, sizeof(Frame), flag, clientaddress, clientlen);
		memcpy(data + datapointer, frame.data, frame.header.length);

		datapointer += frame.header.length;
		lastframe = frame.header.lastframe; // ako je frame.header.lastframe == true kraj prijema.

		printf("Primljen frejm sa sequence brojem: %d.\n", frame.header.sequencenum);

	}

	return res;
}
