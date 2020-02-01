#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

// Initializes Gameboy memory.
void memInit();

// Frees Gameboy memory after execution.
void memFree();

// Reads one byte of memory at address.
uint8_t readMem(uint16_t address);

// Writes one byte of memory at address.
void writeMem(uint16_t address, uint8_t value);

// Read two bytes of memory as one 16-bit value at address.
uint16_t readMem16(uint16_t address);

// Writes two bytes of memory as one 16-bit value at address.
void writeMem16(uint16_t address, uint16_t value);

#endif
