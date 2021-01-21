#pragma once
#include <ws2tcpip.h>
#include "stdio.h"
#include "stdlib.h"
#define FrameDataSize 500

typedef struct udp_Header {

	unsigned int sequencenum;
	unsigned short length;
	unsigned short checksum;
	bool lastframe;

} udpHeader;

typedef struct frame {

	udpHeader header;
	char data[FrameDataSize];

} Frame;

enum Signals{ Ack, AckError };

int calculateCRC(char* data, int duzinapodataka, int polinom);
Frame makeframe(char* data, int duzinapodataka, unsigned int sequencenum, bool lastframe);

