#pragma once
#include <ws2tcpip.h>
#include "stdio.h"
#include "stdlib.h"

typedef struct upd_Header {

	unsigned int sequencenum;
	unsigned short length;
	unsigned short checksum;

};

int calculateCRC(char* data, int duzinapodataka, int polinom);
char* makeframe(char* data, int duzinapodataka);

