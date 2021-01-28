#pragma once
#include <ws2tcpip.h>
#include "stdio.h"
#include "stdlib.h"
#define FrameDataSize 500

typedef unsigned long crc;

#define POLINOM 0x04C11DB7 

typedef struct udp_Header {

	unsigned int sequencenum;
	unsigned short length;
	unsigned long crc;
	bool lastframe;

} udpHeader;

typedef struct frame {

	udpHeader header;
	char data[FrameDataSize];

} Frame;

enum AckSignal{ Ack, AckError, TcpSyn, AckSyn, AckFin };

//void crcInit(void);
Frame makeframe(char* data, int duzinapodataka, unsigned int sequencenum, bool lastframe);
crc calculateCrc(char* data, int duzinaPodataka);
//unsigned int crc32(unsigned char* m, size_t len);
//unsigned short MODBUS_CRC16_v1(char* buf, unsigned int len);
