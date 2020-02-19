#include <stdio.h>

#include "execute.h"
#include "memory.h"
#include "cpu.h"

// Writes the given instrs into memory.
void fillMemory(int n, uint8_t* instrs) {
  for(int i = 0; i < n; i++) {
    writeMem(0x100 + i, instrs[i]);
  }
}

void test1() {
  uint8_t instrs[] = {0x06, 0xFF, 0x10};
  fillMemory(3, instrs);
  CPU();
  printf("B = %uh\n", B());
}

int main() {
  memInit();
  test1();
  return 0;
}
