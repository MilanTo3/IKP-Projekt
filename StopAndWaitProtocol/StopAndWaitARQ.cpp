#include "pch.h"
#include "ws2tcpip.h"
#include "StopAndWaitARQ.h"
#include "TCPheader.h"
#include "Helpers.h"
#include "time.h"
#include "memoryPool.h"
#include "math.h"

memoryPool* senderPool;
memoryPool* receiverPool;
CRITICAL_SECTION senderlock;
CRITICAL_SECTION receiverlock;
bool init = false;

void initializeMemPools() {

	senderPool = createMemoryPool();
	receiverPool = createMemoryPool();
	if (senderPool == NULL || receiverPool == NULL) {
		return;
	}
	InitializeCriticalSection(&senderlock);
	InitializeCriticalSection(&receiverlock);
	init = true;

}

int snwarq_sendto(SOCKET uticnica, char* data, int duzinapodataka, int flag, LPSOCKADDR destination, int destinationlen) {

	if (init == false) {
		return 100;
	}
	else if (uticnica == INVALID_SOCKET) {
		return 101;
	}
	else if (duzinapodataka < 0) {
		return 102;
	}
	else if (checkallocation(senderPool, ceil((double)duzinapodataka / FrameDataSize)) == NULL) {
		return 103;
	}

	unsigned int datapointer = 0;
	int tosend = duzinapodataka;
	int res = 0;
	short returnsignal = -1;
	unsigned int sequence = 0;
	Frame frame;
	struct timeval timevalue;
	short synchrosig;
	short synchroack;
	double delta = 100; // pocetna delta je 1 msec.
	DWORD timeout;
	int oldRTT;
	bool retrnsmit;

	DWORD firsttimeout = 100;

	EnterCriticalSection(&senderlock);
	if (setsockopt(uticnica, SOL_SOCKET, SO_RCVTIMEO, (const char*)&firsttimeout, sizeof(firsttimeout)) < 0) {
		printf("Greška prilikom socket timeout operacije.");
		return 102;
	}

	while (datapointer < duzinapodataka) {

		/*Do something*/
		if (tosend > FrameDataSize) {
			frame = makeframe(data + (sequence * FrameDataSize), FrameDataSize, sequence, false);
			writeFrameToPool(senderPool, frame);
			tosend = tosend - FrameDataSize;
			datapointer += FrameDataSize;
		}
		else {
			frame = makeframe(data + (sequence * FrameDataSize), tosend, sequence, true);
			writeFrameToPool(senderPool, frame);
			datapointer += tosend;
		}




		do {

			//synchroack = -1;
			//synchrosig = TcpSyn;

			clock_t start = clock();

			//while (synchroack != AckSyn) {
			//	sendto(uticnica, (char*)&synchrosig, sizeof(short), 0, destination, destinationlen);
			//	printf("Poslat TcpSyn.\n");
			//	recvfrom(uticnica, (char*)&synchroack, sizeof(short), 0, destination, &destinationlen);
			//	printf("Primljen Acksyn.\n");
			//}

			res = sendto(uticnica, (char*)&frame, sizeof(Frame), 0, destination, destinationlen);
			printf("Poslat frejm: %d.\n", frame.header.sequencenum);

			if (sequence == 0) {
				timeout = (int)(1.8 * delta); // (sec * 1000) to get milliseconds.
			}
			else {
				timeout = (int)(0.8 * oldRTT) + (0.2 * delta); //(a*Stari_RTT) + ((1-a) * Novi_RTT_Uzorak)

			}
			timeout = 1.2 * timeout;

			if (setsockopt(uticnica, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
				printf("Greška prilikom socket timeout operacije.");
				return 102;
			}

			returnsignal = -1;
			recvfrom(uticnica, (char*)&returnsignal, sizeof(short), 0, destination, &destinationlen);

			if (WSAGetLastError() == WSAETIMEDOUT) {
				retrnsmit = true;
				printf("Vrsi se retransmisija za TIMEOUT\n");
			}
			/*else if (returnsignal != Ack) {
				retrnsmit = true;
				printf("Vrsi se retransmisija zbog neodgovarajuceg SIGNALA\n");
			}*/
			else {
				retrnsmit = false;
				printf("Primljen ack.\n");
			}

			clock_t end = clock();

			oldRTT = delta;
			delta = ((double)(end - start)) / CLOCKS_PER_SEC; // in seconds.
			delta = delta * 1000; // in millis

			printf("Odredjen timeout je %d. Delta je: %lf.\n", timeout, delta);
			printf("Stara delta je %d. Nova delta je: %lf.\n\n", oldRTT, delta);

		} while (retrnsmit);

		sequence++;

	}

	LeaveCriticalSection(&senderlock);

	return datapointer;
}

int snwarq_recvfrom(SOCKET uticnica, char* data, int duzinapodataka, int flag, LPSOCKADDR clientaddress, int* clientlen) {

	if (init == false) {
		return 100;
	}
	else if (uticnica == INVALID_SOCKET) {
		return 101;
	}
	else if (duzinapodataka < 0) {
		return 102;
	}
	else if (checkallocation(receiverPool, ceil((double)duzinapodataka / FrameDataSize)) == NULL) {
		return 103;
	}

	int numframeseq = ceil((double)duzinapodataka / FrameDataSize);
	int* frameseqarray = (int*)malloc(numframeseq * sizeof(int));
	if (frameseqarray == NULL) {
		return 104;
	}

	Frame frame;
	memset(&frame, 0, sizeof(Frame));
	frame.header.lastframe = false;
	bool lastframe = false;
	unsigned int datapointer = 0;
	int res = 0;
	short signal;
	short synchroack;
	int seqindex = 0;
	u_long mode;

	EnterCriticalSection(&receiverlock);

	initializeArray(frameseqarray, numframeseq);
	while (datapointer < duzinapodataka && frame.header.lastframe != true) {
		//-----------------

		//signal = -1;
		//synchroack = AckSyn;
		//
		//while (signal != TcpSyn) {
		//	recvfrom(uticnica, (char*)&signal, sizeof(short), 0, clientaddress, clientlen);
		//	printf("Primljen TcpSyn.\n");
		//	sendto(uticnica, (char*)&synchroack, sizeof(short), 0, clientaddress, *clientlen);
		//	printf("Poslat AckSyn.\n");
		//}

		memset(&frame, 0, sizeof(Frame));
		res = recvfrom(uticnica, (char*)&frame, sizeof(Frame), flag, clientaddress, clientlen);

		if (isInArray(frameseqarray, numframeseq, frame.header.sequencenum) == false) {
			//memcpy(data + datapointer, frame.data, frame.header.length);
			if (frame.header.crc != calculateCrc(frame.data, frame.header.length)) {
				printf("Podaci unutar frame-a sa SeqNumber: %d su izmenjeni!\n", frame.header.sequencenum);
				continue;
			}
			writeFrameToPool(receiverPool, frame);

			datapointer += frame.header.length;
			lastframe = frame.header.lastframe; // ako je frame.header.lastframe == true kraj prijema.

			printf("Datapointer: %d.\n", datapointer);

			frameseqarray[seqindex] = frame.header.sequencenum;
			seqindex++;
			printf("Frame sa SeqNumberom %d je primljen\n", frame.header.sequencenum);
		}

		signal = Ack;
		sendto(uticnica, (char*)&signal, sizeof(short), 0, clientaddress, *clientlen);

		//printf("Primljen frejm sa sequence brojem: %d.\n", frame.header.sequencenum);

	}

	copyPoolToBuffer(receiverPool, data, duzinapodataka);

	LeaveCriticalSection(&receiverlock);
	free(frameseqarray);

	return datapointer;
}

void initializeArray(int* frameseqarray, int duzina) {

	int i = 0;
	for (i = 0; i < duzina; i++) {
		frameseqarray[i] = -1;
	}

}

bool isInArray(int* frameseqarray, int duzina, int value) {

	int i;
	for (i = 0; i < duzina; i++) {
		if (frameseqarray[i] == value) {
			printf("Primljen duplikat, SeqNumber: %d.\n", value);
			return true;
		}
	}

	return false;
}

void cleanMemoryPools() {

	mempool_clean(senderPool);
	mempool_clean(receiverPool);

	free(senderPool);
	free(receiverPool);
	init = false;

}
