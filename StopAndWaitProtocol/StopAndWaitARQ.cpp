#include "pch.h"
#include "ws2tcpip.h"
#include "StopAndWaitARQ.h"
#include "TCPheader.h"
#include "Helpers.h"
#include "time.h"

int snwarq_sendto(SOCKET uticnica, char *data, int duzinapodataka, int flag, LPSOCKADDR destination, int destinationlen) {

	unsigned int datapointer = 0;
	int tosend = duzinapodataka;
	int res = 0;
	short returnsignal = -1;
	unsigned int sequence = 0;
	short recvcode = 0;
	Frame frame;
	struct timeval timevalue;
	short synchrosig;
	short synchroack;
	double delta = 500; // pocetna delta je 500 msec.
	DWORD timeout;
	int oldRTT;
	bool retrnsmit;

	if (uticnica == INVALID_SOCKET) {
		return 100;
	}
	else if (duzinapodataka < 0) {
		return 101;
	}

	while (datapointer < duzinapodataka) {

		synchroack = -1;
		synchrosig = TcpSyn;

		/*Do something*/
		if (tosend > FrameDataSize) {
			frame = makeframe(data + (sequence * FrameDataSize), FrameDataSize, sequence, false);
			tosend = tosend - FrameDataSize;
			datapointer += FrameDataSize;
		}
		else {
			frame = makeframe(data + (sequence * FrameDataSize), tosend, sequence, true);
			datapointer += tosend;
		}

		do {

			clock_t start = clock();

			res = udp_sendto(uticnica, frame, sizeof(Frame), destination, destinationlen);

			if (sequence == 0) {
				timeout = (int)(1.8 * delta); // (sec * 1000) to get milliseconds.
			}
			else {
				timeout = (int)(0.8 * oldRTT) + (0.2 * delta); //(a*Stari_RTT) + ((1-a) * Novi_RTT_Uzorak)

			}
			timeout = 1.5 * timeout;

			if (setsockopt(uticnica, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
				printf("Greška prilikom socket timeout operacije.");
				return 102;
			}

			returnsignal = -1;
			recvcode = recvfrom(uticnica, (char*)&returnsignal, sizeof(short), 0, destination, &destinationlen);

			if (WSAGetLastError() == WSAETIMEDOUT) {
				printf("Recv timeout.\n");
				retrnsmit = true;
			}
			else {
				retrnsmit = false;
			}

			clock_t end = clock();

			printf("Odredjen timeout je %d. Delta je: %lf.\n", timeout, delta);

			oldRTT = delta;
			delta = ((double)(end - start)) / CLOCKS_PER_SEC; // in seconds.
			delta = delta * 1000; // in millis

		} while (retrnsmit);

		sequence++;

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
	short signal = 0;

	while (!lastframe) {
		//-----------------

		memset(&frame, 0, sizeof(Frame));
		res = udp_recvfrom(uticnica, &frame, sizeof(Frame), flag, clientaddress, clientlen);
		memcpy(data + datapointer, frame.data, frame.header.length);

		datapointer += frame.header.length;
		lastframe = frame.header.lastframe; // ako je frame.header.lastframe == true kraj prijema.

		signal = Ack;
		sendto(uticnica, (char*)&signal, sizeof(short), 0, clientaddress, *clientlen);

		printf("Primljen frejm sa sequence brojem: %d.\n", frame.header.sequencenum);


	}

	return res;
}
