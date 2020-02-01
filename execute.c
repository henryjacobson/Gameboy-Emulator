#include "cpu.h"
#include "execute.h"
#include <assert.h>
#include <stdio.h>

// Pushes the values onto the stack
void push(uint16_t value) 
{
	int sp = SP() - 2;
	setSP(sp);
	writeMem16(sp, value);
}

// Pops a value off the stack
uint16_t pop()
{
	int sp = SP();
	setSP(sp + 2);
	return readMem16(sp);
}

// Performs an 8-bit add operation and sets relevant flags.
uint8_t add8(uint8_t first, uint8_t second) 
{
	int sum = first + second;
	int halfSum = (first & 0xf) + (second & 0xf);
	
	if(sum)
		resetZflag();
	else
		setZflag();
	
	resetNflag();
	
	if(halfSum > 0xf)
		setHflag();
	else
		resetHflag();
	
	if(sum > 0xff)
		setCflag();
	else
		resetCflag();
	
	return sum;
}

// Performs an 16-bit add operation and sets relevant flags.
uint16_t add16(uint16_t first, uint16_t second) 
{
	int sum = first + second;
	int halfSum = (first & 0xfff) + (second & 0xfff);
	
	if(sum)
		resetZflag();
	else
		setZflag();
	
	resetNflag();
	
	if(halfSum > 0xfff)
		setHflag();
	else
		resetHflag();
	
	if(sum > 0xffff)
		setCflag();
	else
		resetCflag();
	
	return sum;
}


// Performs an 8-bit subtraction and sets the releveant flags.
uint8_t sub8(uint8_t first, uint8_t second) 
{
	int dif = first - second;
	int halfDif = (first & 0xf) - (second & 0xf);
	
	if(dif)
		resetZflag();
	else
		setZflag();
	
	setNflag();
	
	if(halfDif < 0)
		setHflag();
	else
		resetHflag();
	
	if(dif < 0)
		setCflag();
	else
		resetCflag();
	
	return dif;
}

// Performs 8-bit and and sets relevant flags.
uint8_t and8(uint8_t first, uint8_t second) 
{
	int ret = first & second;
	
	if(ret)
		resetZflag();
	else
		setZflag();
	
	resetNflag();
	setHflag();
	resetCflag();
	
	return ret;
}

// Performs 8-bit or and sets relevant flags.
uint8_t or8(uint8_t first, uint8_t second) 
{
	int ret = first | second;
	
	if(ret)
		resetZflag();
	else
		setZflag();
	
	resetNflag();
	resetHflag();
	resetCflag();
	
	return ret;
}

// Performs 8-bit or and sets relevant flags.
uint8_t xor8(uint8_t first, uint8_t second) 
{
	int ret = first ^ second;
	
	if(ret)
		resetZflag();
	else
		setZflag();
	
	resetNflag();
	resetHflag();
	resetCflag();
	
	return ret;
}

// Swaps the lower and upper nybble and set flags.
uint8_t swap8(uint8_t num)
{
	int ret = (num & 0xF0) >> 4;
	ret = ret & ((num & 0xF) << 4);
	
	if(ret)
		resetZflag();
	else
		setZflag();
	
	resetNflag();
	resetHflag();
	resetCflag();
	
	return ret;
}

// Corrects the given BCD number.
uint8_t daa8(uint8_t num)
{
	int correction = 0;
	if(Hflag() || (!Nflag() && (num & 0xF) > 0x9))
		correction |= 0x06;
	if(Cflag() || (!Nflag() && num > 0x99))
		correction |= 0x60;
	
	if(num)
		resetZflag();
	else 
		setZflag();
	
	resetHflag();
	return 0;
}

// Shifts the number left. Old bit 7 to carry flag.
uint8_t rlc(uint8_t num) 
{
	if(num >> 7) 
		setCflag(); 
	else 
		resetCflag(); 
	uint8_t ret = num << 1;
	if(ret) 
		resetZflag(); 
	else 
		setZflag(); 
	resetHflag(); 
	resetNflag();
	
	return ret;
}

// Shifts the number left through the carry flag.
uint8_t rl(uint8_t num) 
{
	int c = C();
	if(num >> 7) 
		setCflag(); 
	else 
		resetCflag(); 
	uint8_t ret = (num << 1) & c;
	if(ret) 
		resetZflag(); 
	else 
		setZflag(); 
	resetHflag(); 
	resetNflag();
	
	return ret;
}

// Shifts the number right. Old bit 0 to carry flag.
uint8_t rrc(uint8_t num) 
{
	if(num & 1) 
		setCflag(); 
	else 
		resetCflag(); 
	uint8_t ret = num >> 1;
	if(ret) 
		resetZflag(); 
	else 
		setZflag(); 
	resetHflag(); 
	resetNflag();
	
	return ret;
}

// Shifts the number right through the carry flag.
uint8_t rr(uint8_t num) 
{
	int c = Cflag() << 7;
	if(num & 1) 
		setCflag(); 
	else 
		resetCflag(); 
	uint8_t ret = (num >> 1) & c;
	if(ret) 
		resetZflag(); 
	else 
		setZflag(); 
	resetHflag(); 
	resetNflag();
	
	return ret;
}

// Commands organized as given in Game Boy CPU Manual Ch 3.3
void execute(uint8_t instr, int* cycles)
{
	switch(instr)
		{
			// --- 8-bit loads ---
			
			// 1. LD nn, n
			case 0x06: {*cycles = 8; setB(imm8());} break;
			case 0x0E: {*cycles = 8; setC(imm8());} break;
			case 0x16: {*cycles = 8; setD(imm8());} break;
			case 0x1E: {*cycles = 8; setE(imm8());} break;
			case 0x26: {*cycles = 8; setH(imm8());} break;
			case 0x2E: {*cycles = 8; setL(imm8());} break;
			
			// 2. LD r1, r2
			// - set A
			case 0x7F: {*cycles = 4; setA(A());} break;
			case 0x78: {*cycles = 4; setA(B());} break;
			case 0x79: {*cycles = 4; setA(C());} break;
			case 0x7A: {*cycles = 4; setA(D());} break;
			case 0x7B: {*cycles = 4; setA(E());} break;
			case 0x7C: {*cycles = 4; setA(H());} break;
			case 0x7D: {*cycles = 4; setA(L());} break;
			case 0x7E: {*cycles = 8; setA(readMem(HL()));} break;
			// - set B
			case 0x47: {*cycles = 4; setB(A());} break;
			case 0x40: {*cycles = 4; setB(B());} break;
			case 0x41: {*cycles = 4; setB(C());} break;
			case 0x42: {*cycles = 4; setB(D());} break;
			case 0x43: {*cycles = 4; setB(E());} break;
			case 0x44: {*cycles = 4; setB(H());} break;
			case 0x45: {*cycles = 4; setB(L());} break;
			case 0x46: {*cycles = 8; setB(readMem(HL()));} break;
			// - set C
			case 0x4F: {*cycles = 4; setC(A());} break;
			case 0x48: {*cycles = 4; setC(B());} break;
			case 0x49: {*cycles = 4; setC(C());} break;
			case 0x4A: {*cycles = 4; setC(D());} break;
			case 0x4B: {*cycles = 4; setC(E());} break;
			case 0x4C: {*cycles = 4; setC(H());} break;
			case 0x4D: {*cycles = 4; setC(L());} break;
			case 0x4E: {*cycles = 8; setA(readMem(HL()));} break;
			// - set D
			case 0x57: {*cycles = 4; setD(A());} break;
			case 0x50: {*cycles = 4; setD(B());} break;
			case 0x51: {*cycles = 4; setD(C());} break;
			case 0x52: {*cycles = 4; setD(D());} break;
			case 0x53: {*cycles = 4; setD(E());} break;
			case 0x54: {*cycles = 4; setD(H());} break;
			case 0x55: {*cycles = 4; setD(L());} break;
			case 0x56: {*cycles = 8; setD(readMem(HL()));} break;
			// - set E
			case 0x5F: {*cycles = 4; setE(A());} break;
			case 0x58: {*cycles = 4; setE(B());} break;
			case 0x59: {*cycles = 4; setE(C());} break;
			case 0x5A: {*cycles = 4; setE(D());} break;
			case 0x5B: {*cycles = 4; setE(E());} break;
			case 0x5C: {*cycles = 4; setE(H());} break;
			case 0x5D: {*cycles = 4; setE(L());} break;
			case 0x5E: {*cycles = 8; setE(readMem(HL()));} break;
			// - set H
			case 0x67: {*cycles = 4; setH(A());} break;
			case 0x60: {*cycles = 4; setH(B());} break;
			case 0x61: {*cycles = 4; setH(C());} break;
			case 0x62: {*cycles = 4; setH(D());} break;
			case 0x63: {*cycles = 4; setH(E());} break;
			case 0x64: {*cycles = 4; setH(H());} break;
			case 0x65: {*cycles = 4; setH(L());} break;
			case 0x66: {*cycles = 8; setH(readMem(HL()));} break;
			// - set L
			case 0x6F: {*cycles = 4; setL(A());} break;
			case 0x68: {*cycles = 4; setL(B());} break;
			case 0x69: {*cycles = 4; setL(C());} break;
			case 0x6A: {*cycles = 4; setL(D());} break;
			case 0x6B: {*cycles = 4; setL(E());} break;
			case 0x6C: {*cycles = 4; setL(H());} break;
			case 0x6D: {*cycles = 4; setL(L());} break;
			case 0x6E: {*cycles = 8; setL(readMem(HL()));} break;
			// - set (HL)
			case 0x77: {*cycles = 8; writeMem(HL(), A());} break;
			case 0x70: {*cycles = 8; writeMem(HL(), B());} break;
			case 0x71: {*cycles = 8; writeMem(HL(), C());} break;
			case 0x72: {*cycles = 8; writeMem(HL(), D());} break;
			case 0x73: {*cycles = 8; writeMem(HL(), E());} break;
			case 0x74: {*cycles = 8; writeMem(HL(), H());} break;
			case 0x75: {*cycles = 8; writeMem(HL(), L());} break;
			case 0x36: {*cycles = 12; writeMem(HL(), imm8());} break;
			
			// 3. LD A, n
			case 0x0A: {*cycles = 8; setA(readMem(BC()));} break;
			case 0x1A: {*cycles = 8; setA(readMem(DE()));} break;
			case 0xFA: {*cycles = 16; setA(readMem(imm8()));} break;
			case 0x3E: {printf("3E was called\n"); assert(0);}
			
			// 4. LD n, A
			case 0x02: {*cycles = 8; writeMem(BC(), A());} break;
			case 0x12: {*cycles = 8; writeMem(DE(), A());} break;
			case 0xEA: {*cycles = 16; writeMem(imm8(), A());} break;
			
			// 5. LD A, ($FF00 + C)
			case 0xF2: {*cycles = 8; setA(readMem(0xFF00 + C()));} break;
			
			// 6. LD ($FF00 + C), A
			case 0xE2: {*cycles = 8; writeMem(0xFF00 + C(), A());} break;
			
			// 7-9. LD A, (HL--)
			case 0x3A: {*cycles = 8; setA(readMem(HL())); setHL(HL() - 1);} break;
			
			// 10-12. LD (HL--), A
			case 0x32: {*cycles = 8; writeMem(HL(), A()); setHL(HL() - 1);} break;
			
			// 13-15. LD A, (HL++)
			case 0x2A: {*cycles = 8; setA(readMem(HL())); setHL(HL() + 1);} break;
			
			// 16-18. LD (HL++), A
			case 0x22: {*cycles = 8; writeMem(HL(), A()); setHL(HL() + 1);} break;
			
			// 19. LD A, ($FF00 + n)
			case 0xF0: {*cycles = 12; setA(readMem(0xFF00 + PC()));} break;
			
			// 20. LD ($FF00 + n), A
			case 0xE0: {*cycles = 12; writeMem(0xFF00 + PC(), A());} break;
			
			
			// --- 16-bit loads ---
			
			// 1. LD r, nn
			case 0x01: {*cycles = 12; setBC(imm16());} break;
			case 0x11: {*cycles = 12; setDE(imm16());} break;
			case 0x21: {*cycles = 12; setHL(imm16());} break;
			case 0x31: {*cycles = 12; setPC(imm16());} break;
			
			// 2. LD SP, HL
			case 0xF9: {*cycles = 8; setSP(HL());} break;
			
			// TODO: 3-4. LDHL SP, n
			//case 0xF8: {int sum = SP() + imm8(); *cycles = 12; setHL(sum); 
			//	resetZflag(); resetNflag(); setH(sum); setC(sum);} break;
			
			// 5. LD (nn), SP
			case 0x08: {*cycles = 20; writeMem16(imm16(), SP());} break;
			
			// 6. PUSH nn
			case 0xF5: {*cycles = 16; push(AF());} break;
			case 0xC5: {*cycles = 16; push(BC());} break;
			case 0xD5: {*cycles = 16; push(DE());} break;
			case 0xE5: {*cycles = 16; push(HL());} break;
			
			// 7. POP nn
			case 0xF1: {*cycles = 12; setAF(pop());} break;
			case 0xC1: {*cycles = 12; setBC(pop());} break;
			case 0xD1: {*cycles = 12; setDE(pop());} break;
			case 0xE1: {*cycles = 12; setHL(pop());} break;
			
			// --- 8-bit ALU ---
			
			// 1. ADD A, r
			case 0x87: {*cycles = 4; setA(add8(A(), A()));} break;
			case 0x80: {*cycles = 4; setA(add8(A(), B()));} break;
			case 0x81: {*cycles = 4; setA(add8(A(), C()));} break;
			case 0x82: {*cycles = 4; setA(add8(A(), D()));} break;
			case 0x83: {*cycles = 4; setA(add8(A(), E()));} break;
			case 0x84: {*cycles = 4; setA(add8(A(), H()));} break;
			case 0x85: {*cycles = 4; setA(add8(A(), L()));} break;
			// 1.2 ADD A, (HL)
			case 0x86: {*cycles = 8; setA(add8(A(), readMem(HL())));} break;
			// 1.3 ADD A, n
			case 0xC6: {*cycles = 8; setA(add8(A(), imm8()));} break;
				
			// 2. ADC A, r (add carry flag as well)
			case 0x8F: {*cycles = 4; setA(add8(A(), A() + C()));} break;
			case 0x88: {*cycles = 4; setA(add8(A(), B() + C()));} break;
			case 0x89: {*cycles = 4; setA(add8(A(), C() + C()));} break;
			case 0x8A: {*cycles = 4; setA(add8(A(), D() + C()));} break;
			case 0x8B: {*cycles = 4; setA(add8(A(), E() + C()));} break;
			case 0x8C: {*cycles = 4; setA(add8(A(), H() + C()));} break;
			case 0x8D: {*cycles = 4; setA(add8(A(), L() + C()));} break;
			// 2.2 ADC A, (HL)
			case 0x8E: {*cycles = 8; setA(add8(A(), readMem(HL()) + C()));} break;
			// 2.3 ADC A, n
			case 0xCE: {*cycles = 8; setA(add8(A(), imm8() + C()));} break;
			
			// 3. SUB A, r
			case 0x97: {*cycles = 4; setA(sub8(A(), A()));} break;
			case 0x90: {*cycles = 4; setA(sub8(A(), B()));} break;
			case 0x91: {*cycles = 4; setA(sub8(A(), C()));} break;
			case 0x92: {*cycles = 4; setA(sub8(A(), D()));} break;
			case 0x93: {*cycles = 4; setA(sub8(A(), E()));} break;
			case 0x94: {*cycles = 4; setA(sub8(A(), H()));} break;
			case 0x95: {*cycles = 4; setA(sub8(A(), L()));} break;
			// 3.2 SUB A, (HL)
			case 0x96: {*cycles = 8; setA(sub8(A(), readMem(HL())));} break;
			// 3.3 SUB A, n
			case 0xD6: {*cycles = 8; setA(sub8(A(), imm8()));} break;
			
			// 4. SBC A, r (sub carry flag as well)
			case 0x9F: {*cycles = 4; setA(sub8(A(), A() + C()));} break;
			case 0x98: {*cycles = 4; setA(sub8(A(), B() + C()));} break;
			case 0x99: {*cycles = 4; setA(sub8(A(), C() + C()));} break;
			case 0x9A: {*cycles = 4; setA(sub8(A(), D() + C()));} break;
			case 0x9B: {*cycles = 4; setA(sub8(A(), E() + C()));} break;
			case 0x9C: {*cycles = 4; setA(sub8(A(), H() + C()));} break;
			case 0x9D: {*cycles = 4; setA(sub8(A(), L() + C()));} break;
			// 4.2 SBC A, (HL)
			case 0x9E: {*cycles = 8; setA(sub8(A(), readMem(HL()) + C()));} break;
			// 4.3 SBC A, n
			case 0xDE: {*cycles = 8; setA(sub8(A(), imm8() + C()));} break;
			
			// 5 AND n
			case 0xA7: {*cycles = 4; setA(and8(A(), A()));} break;
			case 0xA0: {*cycles = 4; setA(and8(A(), B()));} break;
			case 0xA1: {*cycles = 4; setA(and8(A(), C()));} break;
			case 0xA2: {*cycles = 4; setA(and8(A(), D()));} break;
			case 0xA3: {*cycles = 4; setA(and8(A(), E()));} break;
			case 0xA4: {*cycles = 4; setA(and8(A(), H()));} break;
			case 0xA5: {*cycles = 4; setA(and8(A(), L()));} break;
			case 0xA6: {*cycles = 8; setA(and8(A(), readMem(HL())));} break;
			case 0xE6: {*cycles = 8; setA(and8(A(), imm8()));} break;
			
			// 6 OR n
			case 0xB7: {*cycles = 4; setA(or8(A(), A()));} break;
			case 0xB0: {*cycles = 4; setA(or8(A(), B()));} break;
			case 0xB1: {*cycles = 4; setA(or8(A(), C()));} break;
			case 0xB2: {*cycles = 4; setA(or8(A(), D()));} break;
			case 0xB3: {*cycles = 4; setA(or8(A(), E()));} break;
			case 0xB4: {*cycles = 4; setA(or8(A(), H()));} break;
			case 0xB5: {*cycles = 4; setA(or8(A(), L()));} break;
			case 0xB6: {*cycles = 8; setA(or8(A(), readMem(HL())));} break;
			case 0xF6: {*cycles = 8; setA(or8(A(), imm8()));} break;
			
			// 7 XOR n
			case 0xAF: {*cycles = 4; setA(or8(A(), A()));} break;
			case 0xA8: {*cycles = 4; setA(or8(A(), B()));} break;
			case 0xA9: {*cycles = 4; setA(or8(A(), C()));} break;
			case 0xAA: {*cycles = 4; setA(or8(A(), D()));} break;
			case 0xAB: {*cycles = 4; setA(or8(A(), E()));} break;
			case 0xAC: {*cycles = 4; setA(or8(A(), H()));} break;
			case 0xAD: {*cycles = 4; setA(or8(A(), L()));} break;
			case 0xAE: {*cycles = 8; setA(or8(A(), readMem(HL())));} break;
			case 0xEE: {*cycles = 8; setA(or8(A(), imm8()));} break;
			
			// 8. CP A r
			case 0xBF: {*cycles = 4; sub8(A(), A());} break;
			case 0xB8: {*cycles = 4; sub8(A(), B());} break;
			case 0xB9: {*cycles = 4; sub8(A(), C());} break;
			case 0xBA: {*cycles = 4; sub8(A(), D());} break;
			case 0xBB: {*cycles = 4; sub8(A(), E());} break;
			case 0xBC: {*cycles = 4; sub8(A(), H());} break;
			case 0xBD: {*cycles = 4; sub8(A(), L());} break;
			case 0xBE: {*cycles = 8; sub8(A(), readMem(HL()));} break;
			case 0xFE: {*cycles = 8; sub8(A(), imm8());} break;
			
			// 9. INC r
			case 0x3C: {*cycles = 4; pushC(); setA(add8(A(), 1)); popC();} break;
			case 0x04: {*cycles = 4; pushC(); setB(add8(B(), 1)); popC();} break;
			case 0x0C: {*cycles = 4; pushC(); setC(add8(C(), 1)); popC();} break;
			case 0x14: {*cycles = 4; pushC(); setD(add8(D(), 1)); popC();} break;
			case 0x1C: {*cycles = 4; pushC(); setE(add8(E(), 1)); popC();} break;
			case 0x24: {*cycles = 4; pushC(); setH(add8(H(), 1)); popC();} break;
			case 0x2C: {*cycles = 4; pushC(); setL(add8(L(), 1)); popC();} break;
			case 0x34: {*cycles = 12; pushC(); writeMem(HL(), add8(readMem(HL()), 1)); popC();} break;
			
			// 10. DEC r
			case 0x3D: {*cycles = 4; pushC(); setA(sub8(A(), 1)); popC();} break;
			case 0x05: {*cycles = 4; pushC(); setB(sub8(B(), 1)); popC();} break;
			case 0x0D: {*cycles = 4; pushC(); setC(sub8(C(), 1)); popC();} break;
			case 0x15: {*cycles = 4; pushC(); setD(sub8(D(), 1)); popC();} break;
			case 0x1D: {*cycles = 4; pushC(); setE(sub8(E(), 1)); popC();} break;
			case 0x25: {*cycles = 4; pushC(); setH(sub8(H(), 1)); popC();} break;
			case 0x2D: {*cycles = 4; pushC(); setL(sub8(L(), 1)); popC();} break;
			case 0x35: {*cycles = 12; pushC(); writeMem(HL(), sub8(readMem(HL()), 1)); popC();} break;
			
			// --- 16-bit ALU ---
			
			// 1. ADD HL, rr
			case 0x09: {*cycles = 8; setHL(add16(HL(), BC()));} break;
			case 0x19: {*cycles = 8; setHL(add16(HL(), DE()));} break;
			case 0x29: {*cycles = 8; setHL(add16(HL(), HL()));} break;
			case 0x39: {*cycles = 8; setHL(add16(HL(), SP()));} break;
			
			// 2. ADD SP, n
			case 0xE8: {*cycles = 16; setSP(add16(SP(), imm8()));} break;
			
			// 3. INC rr
			case 0x03: {*cycles = 8; setBC(BC() + 1);} break;
			case 0x13: {*cycles = 8; setDE(DE() + 1);} break;
			case 0x23: {*cycles = 8; setHL(HL() + 1);} break;
			case 0x33: {*cycles = 8; setSP(SP() + 1);} break;
			
			// 4. DEC rr
			case 0x0B: {*cycles = 8; setBC(BC() - 1);} break;
			case 0x1B: {*cycles = 8; setDE(DE() - 1);} break;
			case 0x2B: {*cycles = 8; setHL(HL() - 1);} break;
			case 0x3B: {*cycles = 8; setSP(SP() - 1);} break;
			
			// --- Miscellaneous ---
			
			// 1. SWAP r
			// Many commands have CB as there first opcode, and the
			// next opcode opcode determines which command it is. This 
			// includes SWAP r along with others.
			case 0xCB: {
				uint8_t next = readMem(PC());
				switch(next) {
					// 1. SWAP r
					case 0x37: {*cycles = 8; setA(swap8(A()));} break;
					case 0x30: {*cycles = 8; setB(swap8(B()));} break;
					case 0x31: {*cycles = 8; setC(swap8(C()));} break;
					case 0x32: {*cycles = 8; setD(swap8(D()));} break;
					case 0x33: {*cycles = 8; setE(swap8(E()));} break;
					case 0x34: {*cycles = 8; setH(swap8(H()));} break;
					case 0x35: {*cycles = 8; setL(swap8(L()));} break;
					case 0x36: {*cycles = 16; writeMem(HL(), swap8(readMem(HL())));} break;
					
					// 5. RLC n
					case 0x07: {*cycles = 8; setA(rlc(A()));} break;
					case 0x00: {*cycles = 8; setB(rlc(B()));} break;
					case 0x01: {*cycles = 8; setC(rlc(C()));} break;
					case 0x02: {*cycles = 8; setD(rlc(D()));} break;
					case 0x03: {*cycles = 8; setE(rlc(E()));} break;
					case 0x04: {*cycles = 8; setH(rlc(H()));} break;
					case 0x05: {*cycles = 8; setL(rlc(L()));} break;
					case 0x06: {*cycles = 16; writeMem(HL(), rlc(readMem(HL())));} break;
					
					// 6. RL n
					case 0x17: {*cycles = 8; setA(rl(A()));} break;
					case 0x10: {*cycles = 8; setB(rl(B()));} break;
					case 0x11: {*cycles = 8; setC(rl(C()));} break;
					case 0x12: {*cycles = 8; setD(rl(D()));} break;
					case 0x13: {*cycles = 8; setE(rl(E()));} break;
					case 0x14: {*cycles = 8; setH(rl(H()));} break;
					case 0x15: {*cycles = 8; setL(rl(L()));} break;
					case 0x16: {*cycles = 16; writeMem(HL(), rl(readMem(HL())));} break;
					
					// 7. RRC n
					case 0x0F: {*cycles = 8; setA(rrc(A()));} break;
					case 0x08: {*cycles = 8; setB(rrc(B()));} break;
					case 0x09: {*cycles = 8; setC(rrc(C()));} break;
					case 0x0A: {*cycles = 8; setD(rrc(D()));} break;
					case 0x0B: {*cycles = 8; setE(rrc(E()));} break;
					case 0x0C: {*cycles = 8; setH(rrc(H()));} break;
					case 0x0D: {*cycles = 8; setL(rrc(L()));} break;
					case 0x0E: {*cycles = 16; writeMem(HL(), rrc(readMem(HL())));} break;
					
					// 8. RR n
					case 0x1F: {*cycles = 8; setA(rr(A()));} break;
					case 0x18: {*cycles = 8; setB(rr(B()));} break;
					case 0x19: {*cycles = 8; setC(rr(C()));} break;
					case 0x1A: {*cycles = 8; setD(rr(D()));} break;
					case 0x1B: {*cycles = 8; setE(rr(E()));} break;
					case 0x1C: {*cycles = 8; setH(rr(H()));} break;
					case 0x1D: {*cycles = 8; setL(rr(L()));} break;
					case 0x1E: {*cycles = 16; writeMem(HL(), rr(readMem(HL())));} break;
					
					// TODO: {9-11
					
					default: printf("Unknown opcode: %X %X\n", instr, next); assert(0);
					} }
				break;
			
			// 2. DAA
			case 0x27: {*cycles = 4; setA(daa8(A()));} break;
			
			// 3. CPL
			case 0x2F: {*cycles = 4; setA(~A()); setNflag(); setHflag();} break;
			
			// 4. CCF
			case 0x3F: {*cycles = 4; if(Cflag()) resetCflag(); else setCflag(); resetNflag(); resetHflag();} break;
			
			// 5. SCF
			case 0x37: {*cycles = 4; resetNflag(); resetHflag(); setCflag();} break;
			
			// 6. NOP
			case 0x00: {*cycles = 4;} break;
			
			// TODO: {7. HALT
			case 0x76: {*cycles = 4; haltCPU();} break;
			
			// TODO: {8. STOP
			case 0x10: {*cycles = 4; haltCPU();} break;
			
			// TODO: {9. DI
			// TODO: {10. EI
			
			// --- Rotates & Shifts ---
			
			// 1. RLCA
			case 0x07: {*cycles = 4; setA(rlc(A()));} break;
				
			// 2. RLA
			case 0x17: {*cycles = 4; setA(rl(A()));} break;
			
			// 3. RRCA
			case 0x0F: {*cycles = 4; setA(rrc(A()));} break;
			
			// 4. RRA
			case 0x1F: {*cycles = 4; setA(rr(A()));} break;
			
			// Rotate commands 5-11 are with the other CBs above
			
			// TODO: {--- Bit Opcodes ---
			
			// --- Jumps ---
			
			// JP nn
			case 0xC3: {*cycles = 12; setPC(imm16LSB());} break;
			
			// JP cc nn
			case 0xC2: {*cycles = 12; if(!Zflag()) setPC(imm16LSB());} break;
			case 0xCA: {*cycles = 12; if(Zflag()) setPC(imm16LSB());} break;
			case 0xD2: {*cycles = 12; if(!Cflag()) setPC(imm16LSB());} break;
			case 0xDA: {*cycles = 12; if(Cflag()) setPC(imm16LSB());} break;
			
			// JP HL
			case 0xE9: {*cycles = 4; setPC(HL());} break;
			
			// JR n
			case 0x18: {*cycles = 8; setPC(holdPC() + signedImm8());} break;
			
			// JR cc, n
			case 0x20: {*cycles = 8; if(!Zflag()) setPC(holdPC() + signedImm8());} break;
			case 0x28: {*cycles = 8; if(Zflag()) setPC(holdPC() + signedImm8());} break;
			case 0x30: {*cycles = 8; if(!Cflag()) setPC(holdPC() + signedImm8());} break;
			case 0x38: {*cycles = 8; if(Cflag()) setPC(holdPC() + signedImm8());} break;
			
			// --- Calls ---
			
			// 1. CALL nn
			case 0xCD: {*cycles = 12; push(holdPC()); setPC(imm16LSB());} break;
			
			// 2. CALL cc, nn
			case 0xC4: {*cycles = 12; if(!Zflag()) {push(holdPC()); setPC(imm16LSB());}} break;
			case 0xCC: {*cycles = 12; if(Zflag()) {push(holdPC()); setPC(imm16LSB());}} break;
			case 0xD4: {*cycles = 12; if(!Cflag()) {push(holdPC()); setPC(imm16LSB());}} break;
			case 0xDC: {*cycles = 12; if(Cflag()) {push(holdPC()); setPC(imm16LSB());}} break;
			
			// --- Restarts ---
			case 0xC7: {*cycles = 32; push(holdPC()); setPC(0x00);} break;
			case 0xCF: {*cycles = 32; push(holdPC()); setPC(0x08);} break;
			case 0xD7: {*cycles = 32; push(holdPC()); setPC(0x10);} break;
			case 0xDF: {*cycles = 32; push(holdPC()); setPC(0x18);} break;
			case 0xE7: {*cycles = 32; push(holdPC()); setPC(0x20);} break;
			case 0xEF: {*cycles = 32; push(holdPC()); setPC(0x28);} break;
			case 0xF7: {*cycles = 32; push(holdPC()); setPC(0x30);} break;
			case 0xFF: {*cycles = 32; push(holdPC()); setPC(0x38);} break;
			
			// --- Returns ---
			
			// 1. RET
			case 0xC9: {*cycles = 8; setPC(pop());} break;
			
			// 2. RET cc
			case 0xC0: {*cycles = 8; if(!Zflag()) setPC(pop());} break;
			case 0xC8: {*cycles = 8; if(Zflag()) setPC(pop());} break;
			case 0xD0: {*cycles = 8; if(!Cflag()) setPC(pop());} break;
			case 0xD8: {*cycles = 8; if(Cflag()) setPC(pop());} break;
			
			// TODO: {3. RETI
			case 0xD9: {*cycles = 8; setPC(pop());} break;
			
			default: printf("Unknown opcode: %X\n", instr); assert(0);
		}
}































