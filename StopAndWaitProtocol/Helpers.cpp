#include "pch.h"
#include "Helpers.h"
#include "ws2tcpip.h"
#include "stdio.h"
#include "stdlib.h"
#include <cstdint>

/*
 * Derive parameters from the standard-specific parameters in crc.h.
 */
#define WIDTH (8 * sizeof(crc))
#define TOPBIT (1 <<(WIDTH -1))


Frame makeframe(char* data, int duzinapodataka, unsigned int sequencenumber, bool lastframe) {

	Frame frame;
	memset(&frame, 0, sizeof(Frame));

	udpHeader header;
	header.crc = calculateCrc(data, duzinapodataka);
	header.length = duzinapodataka;
	header.sequencenum = sequencenumber;
	header.lastframe = lastframe;

	frame.header = header;
	memcpy(frame.data, data, duzinapodataka);

	return frame;
}

crc calculateCrc(char* data, int duzinaPodataka) {
    crc  remainder = 0;
	//int prob = WIDTH;

	for (int byte = 0; byte < duzinaPodataka; ++byte)
	{
		remainder ^= (data[byte] << (WIDTH - 8));
		//prob = (WIDTH - 8);

		for (uint8_t bit = 8; bit > 0; --bit)
		{
			if (remainder & TOPBIT)
			{
				remainder = (remainder << 1) ^ POLINOM;
			}
			else
			{
				remainder = (remainder << 1);
			}
		}
	}

	return (remainder);
}