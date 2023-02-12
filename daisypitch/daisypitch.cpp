#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
PitchShifter ps;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	
	for (size_t i = 0; i < size; i++)
	{
		float in0 = in[0][i];
		float in1 = in[0][i];
		out[0][i] = ps.Process(in0);
		out[1][i] = ps.Process(in1);
	}
}

int main(void)
{
	hw.Init();

	ps.Init(48000);
	ps.SetTransposition(-12.0);
	ps.SetDelSize(4096);

	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);
	while(1) {}
}
