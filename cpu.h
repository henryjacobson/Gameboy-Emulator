#ifndef CPU_H
#define CPU_H

#include "memory.h"
#include <stdint.h>

typedef struct {
	uint8_t first, second;
} Pair;

typedef struct {
	Pair AF, BC, DE, HL;
	uint16_t SP, PC;
} CPUState;

CPUState* state;

void CPU();

void haltCPU();

// --- Register Gets ---

inline uint8_t A() {return state->AF.first;}
inline uint8_t F() {return state->AF.second;}
inline uint8_t B() {return state->BC.first;}
inline uint8_t C() {return state->BC.second;}
inline uint8_t D() {return state->DE.first;}
inline uint8_t E() {return state->DE.second;}
inline uint8_t H() {return state->HL.first;}
inline uint8_t L() {return state->HL.second;}
inline uint16_t SP() {return state->SP;}
inline uint16_t PC() {return state->PC++;}
inline uint16_t holdPC() {return state->PC;}

inline uint16_t AF() {return (((uint16_t) A()) << 8) | F();}
inline uint16_t BC() {return (((uint16_t) B()) << 8) | C();}
inline uint16_t DE() {return (((uint16_t) D()) << 8) | E();}
inline uint16_t HL() {return (((uint16_t) H()) << 8) | L();}

inline uint16_t imm8() {return readMem(PC());}
inline int16_t signedImm8() {return readMem(PC());}
inline uint16_t imm16() {uint16_t first = imm8(); char sec = imm8(); 
	return (first << 8) | sec;}
inline uint16_t imm16LSB() {uint16_t first = imm8(); char sec = imm8(); 
	return (sec << 8) | first;}

// --- Flag Gets ---

inline char Zflag() {return (F() >> 7) & 1;}
inline char Nflag() {return (F() >> 6) & 1;}
inline char Hflag() {return (F() >> 5) & 1;}
inline char Cflag() {return (F() >> 4) & 1;} //if((F() >> 4) % 2) return 1; else return 0;

// --- Register Sets ---

inline void setA(uint8_t in) {state->AF.first = in;}
inline void setF(uint8_t in) {state->AF.second = in;}
inline void setB(uint8_t in) {state->BC.first = in;}
inline void setC(uint8_t in) {state->BC.second = in;}
inline void setD(uint8_t in) {state->DE.first = in;}
inline void setE(uint8_t in) {state->DE.second = in;}
inline void setH(uint8_t in) {state->HL.first = in;}
inline void setL(uint8_t in) {state->HL.second = in;}
inline void setSP(uint16_t in) {state->SP = in;}
inline void setPC(uint16_t in) {state->PC = in;}

inline void setAF(uint16_t in) {setA(in >> 8); setF(in);}
inline void setBC(uint16_t in) {setB(in >> 8); setC(in);}
inline void setDE(uint16_t in) {setD(in >> 8); setE(in);}
inline void setHL(uint16_t in) {setH(in >> 8); setL(in);}

// --- Flag Sets ---

inline void setZflag() {setF(F() | (1 << 7));}
inline void resetZflag() {setF(F() & ~(1 << 7));}
inline void setNflag() {setF(F() | (1 << 6));}
inline void resetNflag() {setF(F() & ~(1 << 6));}
inline void setHflag() {setF(F() | (1 << 5));}
inline void resetHflag() {setF(F() & ~(1 << 5));}
inline void setCflag() {setF(F() | (1 << 4));}
inline void resetCflag() {setF(F() & ~(1 << 4));}

// Some operations are similar to others but the C flag is left
// unaffected, a push/pop is easier than redoing the operation
// with the one difference.
uint8_t tempC;
inline void pushC() {tempC = Cflag();}
inline void popC() {if(tempC) Cflag(); else resetCflag();}
							
#endif