#include "memory.h"
#include <stdlib.h>

uint8_t *memory;

void memInit()
{
	memory = (uint8_t*) malloc(65536);
}

void memFree()
{
	free(memory);
}

uint8_t readMem(uint16_t address)
{
	return memory[address];
}

void writeMem(uint16_t address, uint8_t value)
{
	memory[address] = value;
}

uint16_t readMem16(uint16_t address)
{
	uint16_t ret = readMem(address) << 8;
	return ret | readMem((uint16_t) (address + 1));
}

void writeMem16(uint16_t address, uint16_t value)
{
	writeMem((uint16_t) (address + 1), value);
	writeMem(address, value >> 8);
}