#pragma once

#include <stdlib.h>
#include <stdio.h>
#include "pch.h"
#include "Helpers.h"

typedef struct frameAllocation_s {

	char* blockstart;
	char* blockend;
	struct frameAllocation_s* next;

} FrameAllocation;

typedef struct mempool_struct {

	FrameAllocation* firstallocation; // Prva lokacija memory Pool-a.
	unsigned int allocatedFrameSpaces;
} memoryPool;

memoryPool* createMemoryPool();
void* mempool_calloc(memoryPool* pool, size_t size);
void* checkallocation(memoryPool* pool, unsigned int sequencenum);
void mempool_clean(memoryPool* pool);
Frame getFrameBySeqNum(memoryPool* pool, unsigned int seqnum);
void writeFrameToPool(memoryPool* pool, Frame frame);
int copyPoolToBuffer(memoryPool* pool, char* buffer, int duzinapodataka);
void printmemoryPool(char* framepointer);