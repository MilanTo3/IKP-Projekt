#include "pch.h"
#include "memoryPool.h"

memoryPool* createMemoryPool() {

	memoryPool* mp = NULL;
	mp = (memoryPool*)malloc(sizeof(memoryPool));
	mp->firstallocation = NULL;
	mp->allocatedFrameSpaces = 0;

	return mp;
}

// FrameAllocation, memoryPool
void fit_as_last(memoryPool* pool, FrameAllocation* newallocation) {

	if (pool->firstallocation == NULL) {

		pool->firstallocation = newallocation;

	}
	else {

		FrameAllocation* current = pool->firstallocation;
		while (current->next != NULL) {

			current = current->next;

		}
		current->next = newallocation;
	}

}

void* mempool_calloc(memoryPool* pool, size_t num) { // Alociranje bloka velicina num frejmova.

	void* allocated = calloc(num, sizeof(Frame));

	if (allocated == NULL) {
		// memory allocation failed
		return allocated;
	}

	FrameAllocation* newallocation = (FrameAllocation*)malloc(sizeof(FrameAllocation));
	if (newallocation == NULL) {
		free(allocated);
		return NULL;
	}

	newallocation->next = NULL;
	newallocation->blockstart = (char*)allocated;
	newallocation->blockend = newallocation->blockstart + (num * sizeof(Frame));
	pool->allocatedFrameSpaces += num;

	fit_as_last(pool, newallocation);

	return newallocation;
}

void* checkallocation(memoryPool* pool, unsigned int sequencenum) {

	void* pointer = &sequencenum;
	if (pool->allocatedFrameSpaces < sequencenum + 1) {

		pointer = mempool_calloc(pool, ((sequencenum + 1) - pool->allocatedFrameSpaces) + 5);

	}

	return pointer;
}

Frame getFrameBySeqNum(memoryPool* pool, unsigned int sequencenum) {

	int numblocks;
	FrameAllocation* current = pool->firstallocation;
	Frame frame;
	memset(&frame, 0, sizeof(Frame));
	int i = 0;

	while (current != NULL) {

		numblocks = (current->blockend - current->blockstart) / sizeof(Frame);

		for (i = 0; i < numblocks; i++) {

			memcpy(&frame, current->blockstart + sizeof(Frame) * i, sizeof(Frame));
			if (frame.header.sequencenum == sequencenum) {
				return frame;
			}

		}

		current = current->next;

	}

	return frame;
}

void writeFrameToPool(memoryPool* pool, Frame frame) {

	int numblocks;
	int prethodnibloksize = 0;
	int writeblock;
	
	FrameAllocation* current = pool->firstallocation;
	numblocks = (current->blockend - current->blockstart) / sizeof(Frame);

	while (current != NULL){

		writeblock = frame.header.sequencenum - prethodnibloksize;

		if (numblocks > (int)frame.header.sequencenum) {

			//printf("Writeblock: %d.\n", writeblock);
			memcpy(current->blockstart + sizeof(Frame) * writeblock, &frame, sizeof(Frame));
			//printmemoryPool(current->blockstart + sizeof(Frame) * writeblock);
			break;
		}

		prethodnibloksize = numblocks;
		current = current->next;
		if (current != NULL) {
			numblocks += (current->blockend - current->blockstart) / sizeof(Frame);
		}

	}

}

void copyPoolToBuffer(memoryPool* pool, char* buffer, int duzinapodataka) {

	int numblocks;
	FrameAllocation* current = pool->firstallocation;
	int datapointer = 0;
	Frame frame;
	int i = 0;
	int j;

	while (current != NULL) {

		numblocks = (current->blockend - current->blockstart) / sizeof(Frame);

		for (i = 0; i < numblocks; i++) {

			memcpy(&frame, current->blockstart + sizeof(Frame) * i, sizeof(Frame));

			if (datapointer + frame.header.length < duzinapodataka) {
				memcpy(buffer + datapointer, &frame.data, frame.header.length);
				datapointer += frame.header.length;
			}
			else {
				memcpy(buffer + datapointer, &frame.data, duzinapodataka - datapointer);
				datapointer += duzinapodataka - datapointer;

			}

			if (frame.header.lastframe == true || datapointer >= duzinapodataka) {
				return;
			}

		}

		current = current->next;

	}

}

void printmemoryPool(char* framepointer) {

	Frame frame;
	memcpy(&frame, framepointer, sizeof(Frame));
	char* data = (char*)malloc(frame.header.length);
	memcpy(data, frame.data, frame.header.length);
	data[frame.header.length - 1] = '\0';

	printf("-------------\n");
	printf("%s.\n", data);

}

void mempool_clean(memoryPool* pool) {

	FrameAllocation* current = pool->firstallocation;
	FrameAllocation* tmp;
	while (current != NULL) {
		tmp = current->next;
		free(current->blockstart);
		free(current);
		current = tmp;
	}
	pool->firstallocation = NULL;
	pool->allocatedFrameSpaces = 0;

}