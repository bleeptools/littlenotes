#include "daisy_seed.h"
#include "daisysp.h"
#include "hid/led.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
Led led;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		out[0][i] = in[0][i];
		out[1][i] = in[1][i];
	}
}

int main(void)
{
	hw.Init();

	led.Init(seed::D1, false, 31250);
	long counter = 0;
	long increment = 1;
	long max = static_cast<long>(1.0e+7);
	// hw.SetAudioBlockSize(4); // number of samples handled per callback
	// hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	// hw.StartAudio(AudioCallback);
	while(1) {
		counter += increment;
		if (counter == 0 || counter == max) increment *= -1;
		led.Set(0.1 + static_cast<float>(counter) / max);
		led.Update();
	}
}
