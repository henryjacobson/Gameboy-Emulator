#include "cpu.h"
#include "execute.h"
#include <malloc.h>

//uint8_t[] instrs = 
//{
//	0x3E, 0x01
//}

/*
typedef struct {
	std::pair <unsigned char, unsigned char> 
		AF, BC, DE, HL;
	uint16_t SP, PC;
} CPUState;
*/

void CPUStateInit()
{
	state = (CPUState*) malloc(sizeof(CPUState));
	
	// Set 8-bit regs to 0
	state->AF.first = state->AF.second = state->BC.first = state->BC.second = state->DE.first = state->DE.second = state->HL.first = state->HL.second = 0;
	
	state->SP = 0;
	state->PC = 0x100;
}

void haltCPU()
{
	
}

void CPU()
{
	char counter = 5;
	
	CPUStateInit();
	
	int cycles;
	
	for(;;)
	{
		unsigned char instr = readMem(PC());
		
		execute(instr, &cycles);
		
		break;
	}
}