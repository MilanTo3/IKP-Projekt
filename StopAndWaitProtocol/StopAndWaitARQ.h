#pragma once
#include <ws2tcpip.h>
#include "stdio.h"
#include "stdlib.h"

void initializeMemPools();
void cleanMemoryPools();
void initializeArray(int* frameseqarray, int duzina);
bool isInArray(int* frameseqarray, int duzina, int value);
int snwarq_sendto(SOCKET uticnica, char* data, int duzinapodataka, int flag, LPSOCKADDR destination, int destinationlen);
int snwarq_recvfrom(SOCKET uticnica, char* data, int duzinapodataka, int flag, LPSOCKADDR clientaddress, int *clientlen);