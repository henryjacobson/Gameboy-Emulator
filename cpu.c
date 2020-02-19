#include "cpu.h"
#include "execute.h"
#include <malloc.h>

typedef struct {
	uint8_t first, second;
} Pair;

typedef struct {
	Pair AF, BC, DE, HL;
	uint16_t SP, PC;
} CPUState;

// The current state of the registers of the CPU
CPUState* state;
int halt;

void CPUStateInit()
{
	state = (CPUState*) malloc(sizeof(CPUState));

	// Set 8-bit regs to 0
	state->AF.first = state->AF.second = state->BC.first = state->BC.second = state->DE.first = state->DE.second = state->HL.first = state->HL.second = 0;

	state->SP = 0;
	state->PC = 0x100;

	halt = 0;
}

void haltCPU()
{
	halt = 1;
}

void CPU()
{
	uint8_t counter = 5;

	CPUStateInit();

	int cycles;

	for(;;)
	{
		unsigned char instr = readMem(PC());

		execute(instr, &cycles);

		if(halt)
			break;
	}
}

// --- Register Gets ---

uint8_t A() {return state->AF.first;}
uint8_t F() {return state->AF.second;}
uint8_t B() {return state->BC.first;}
uint8_t C() {return state->BC.second;}
uint8_t D() {return state->DE.first;}
uint8_t E() {return state->DE.second;}
uint8_t H() {return state->HL.first;}
uint8_t L() {return state->HL.second;}
uint16_t SP() {return state->SP;}
uint16_t PC() {return state->PC++;}
uint16_t holdPC() {return state->PC;}

uint16_t AF() {return (((uint16_t) A()) << 8) | F();}
uint16_t BC() {return (((uint16_t) B()) << 8) | C();}
uint16_t DE() {return (((uint16_t) D()) << 8) | E();}
uint16_t HL() {return (((uint16_t) H()) << 8) | L();}

uint16_t imm8() {return readMem(PC());}
int16_t signedImm8() {return readMem(PC());}
uint16_t imm16() {uint16_t first = imm8(); char sec = imm8();
	return (first << 8) | sec;}
uint16_t imm16LSB() {uint16_t first = imm8(); char sec = imm8();
	return (sec << 8) | first;}

// --- Flag Gets ---

char Zflag() {return (F() >> 7) & 1;}
char Nflag() {return (F() >> 6) & 1;}
char Hflag() {return (F() >> 5) & 1;}
char Cflag() {return (F() >> 4) & 1;} //if((F() >> 4) % 2) return 1; else return 0;

// --- Register Sets ---

void setA(uint8_t in) {state->AF.first = in;}
void setF(uint8_t in) {state->AF.second = in;}
void setB(uint8_t in) {state->BC.first = in;}
void setC(uint8_t in) {state->BC.second = in;}
void setD(uint8_t in) {state->DE.first = in;}
void setE(uint8_t in) {state->DE.second = in;}
void setH(uint8_t in) {state->HL.first = in;}
void setL(uint8_t in) {state->HL.second = in;}
void setSP(uint16_t in) {state->SP = in;}
void setPC(uint16_t in) {state->PC = in;}

void setAF(uint16_t in) {setA(in >> 8); setF(in);}
void setBC(uint16_t in) {setB(in >> 8); setC(in);}
void setDE(uint16_t in) {setD(in >> 8); setE(in);}
void setHL(uint16_t in) {setH(in >> 8); setL(in);}

// --- Flag Sets ---

void setZflag() {setF(F() | (1 << 7));}
void resetZflag() {setF(F() & ~(1 << 7));}
void setNflag() {setF(F() | (1 << 6));}
void resetNflag() {setF(F() & ~(1 << 6));}
void setHflag() {setF(F() | (1 << 5));}
void resetHflag() {setF(F() & ~(1 << 5));}
void setCflag() {setF(F() | (1 << 4));}
void resetCflag() {setF(F() & ~(1 << 4));}

// Some operations are similar to others but the C flag is left
// unaffected, a push/pop is easier than redoing the operation
// with the one difference.
uint8_t tempC;
void pushC() {tempC = Cflag();}
void popC() {if(tempC) Cflag(); else resetCflag();}
