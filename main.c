#include "cpu.h"

int main(int argc, char* argv[])
{
	memInit();
	CPU();
	memFree();
	return 0;
}