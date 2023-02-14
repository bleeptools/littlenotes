#include "daisy_seed.h"

using namespace daisy;

DaisySeed hw;

const size_t param_count = 2;
float param_ram[param_count];
float DSY_QSPI_BSS qspi_buffer[param_count];

#define READONLY

int main(void)
{
	hw.Configure();
	hw.Init();
	
#ifdef READONLY
	hw.StartLog();
#else
	param_ram[0] = 0.65;
	param_ram[1] = 0.75;

	size_t size = sizeof(param_ram[0]) * param_count;
	/** Grab physical address from pointer */
	size_t address = (size_t)qspi_buffer;
	/** Erase qspi and then write that wave */
	hw.qspi.Erase(address, address + size);
	hw.qspi.Write(address, size, (uint8_t*)param_ram);
#endif

	while(1) {
#ifdef READONLY
	auto param_a = static_cast<int>(qspi_buffer[0] * 1000);
	auto param_b = static_cast<int>(qspi_buffer[1] * 1000);

	hw.PrintLine("%d %d", param_a, param_b);

	System::Delay(1000);
#endif
	}
}
