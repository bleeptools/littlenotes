#include "daisy_seed.h"

using namespace daisy;

DaisySeed hw;

struct PersistentParams {
	int even_pattern_a;
	int cword_pattern_a;
	int even_pattern_b;
	int cword_pattern_b; 
};

PersistentParams params_ram;
PersistentParams DSY_QSPI_BSS params_qspi;

//#define READONLY

int main(void)
{
	hw.Configure();
	hw.Init();
	hw.StartLog();

#ifndef READONLY
	params_ram.cword_pattern_a = 1;
	params_ram.cword_pattern_b = 3;
	params_ram.even_pattern_a = 5;
	params_ram.even_pattern_b = 7;

	size_t size = sizeof(params_ram);
	/** Grab physical address from pointer */
	
	size_t address = (size_t)(&params_qspi);
	/** Erase qspi and then write that wave */
	hw.qspi.Erase(address, address + size);
	hw.qspi.Write(address, size, (uint8_t*)(&params_ram));
#endif

	while(1) {
#ifdef READONLY
	auto even_pattern_a = params_qspi.even_pattern_a;
	auto even_pattern_b = params_qspi.even_pattern_b;
	auto cword_pattern_a = params_qspi.cword_pattern_a;
	auto cword_pattern_b = params_qspi.cword_pattern_b;

	hw.PrintLine("%d %d %d %d", even_pattern_a, even_pattern_b, cword_pattern_a, cword_pattern_b);

	System::Delay(1000);
#endif
	}
}
