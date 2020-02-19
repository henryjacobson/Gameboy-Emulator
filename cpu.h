#ifndef CPU_H
#define CPU_H

#include "memory.h"
#include <stdint.h>

// Actrually runs the CPU until powered off.
void CPU();

// Stops the execution of the CPU.
void haltCPU();

// --- Register Gets ---

uint8_t A();
uint8_t F();
uint8_t B();
uint8_t C();
uint8_t D();
uint8_t E();
uint8_t H();
uint8_t L();
uint16_t SP();
uint16_t PC(); // Increments PC after returning.
uint16_t holdPC();

uint16_t AF();
uint16_t BC();
uint16_t DE();
uint16_t HL();

uint16_t imm8();
int16_t signedImm8();
uint16_t imm16();
uint16_t imm16LSB();

// --- Flag Gets ---

char Zflag();
char Nflag();
char Hflag();
char Cflag();

// --- Register Sets ---

void setA(uint8_t in);
void setF(uint8_t in);
void setB(uint8_t in);
void setC(uint8_t in);
void setD(uint8_t in);
void setE(uint8_t in);
void setH(uint8_t in);
void setL(uint8_t in);
void setSP(uint16_t in);
void setPC(uint16_t in);

void setAF(uint16_t in);
void setBC(uint16_t in);
void setDE(uint16_t in);
void setHL(uint16_t in);

// --- Flag Sets ---

void setZflag();
void resetZflag();
void setNflag();
void resetNflag();
void setHflag();
void resetHflag();
void setCflag();
void resetCflag();

// Some operations are similar to others but the C flag is left
// unaffected, a push/pop is simpler  than redoing the operation
// with the one difference.
void pushC();
void popC();

#endif
