#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

void memInit();
void memFree();

uint8_t readMem(uint16_t address);
void writeMem(uint16_t address, uint8_t value);
uint16_t readMem16(uint16_t address);
void writeMem16(uint16_t address, uint16_t value);

#endif