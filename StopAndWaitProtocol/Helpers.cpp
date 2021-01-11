#include "pch.h"
#include "Helpers.h"
#include "ws2tcpip.h"
#include "stdio.h"
#include "stdlib.h"

Frame makeframe(char* data, int duzinapodataka, unsigned int sequencenumber) {

	Frame frame;
	memset(&frame, 0, sizeof(Frame));

	udpHeader header;
	header.checksum = 0; // Postaviti ovde na nesto drugo.
	header.length = duzinapodataka;
	header.sequencenum = sequencenumber;

	frame.header = header;
	memcpy(frame.data, data, duzinapodataka);

	return frame;
}