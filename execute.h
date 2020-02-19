#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdint.h>

// Executes the given instr and sets amount of clock cycles used by inst
// on a Gameboy
void execute(uint8_t instr, int* cycles);

#endif
