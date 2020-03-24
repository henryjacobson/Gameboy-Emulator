#include <stdio.h>

#include "execute.h"
#include "memory.h"
#include "cpu.h"
#include "assert.h"

// Writes the given instrs into memory.
void fillMemory(int n, uint8_t* instrs) {
  for(int i = 0; i < n; i++) {
    writeMem(0x100 + i, instrs[i]);
  }
}

// LD B, 0xFF
// LD A, B
void testLD() {
  uint8_t instrs[] = {0x06, 0xFF, 0x78, 0x10};
  fillMemory(4, instrs);
  CPU();
  assert(A() == 0xFF);
  printf("PASSED testLD\n");
}

// LD BC, 0xABCD
// PUSH BC
// POP DE
void testPUSH() {
  uint8_t instrs[] = {0x01, 0xCD, 0xAB, 0xC5, 0xD1, 0x10};
  fillMemory(6, instrs);
  CPU();
  assert(DE() == 0xABCD);
  printf("PASSED testPUSH\n");
}

int main() {
  memInit();
  testLD();
  testPUSH();
  return 0;
}
