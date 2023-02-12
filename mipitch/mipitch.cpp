#include "daisy_seed.h"
#include "fx/mi/pitch_shifter.h"
#include "fx/mi/units.h"

using namespace daisy;

DaisySeed hw;
clouds::PitchShifter ps;

static const int kBlockSize = 4;
static clouds::FloatFrame _frames[kBlockSize];
static uint16_t DSY_SDRAM_BSS _buf[4096];

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
	for (size_t i = 0; i < size; i++) {
		_frames[i].l = in[0][i];
		_frames[i].r = in[1][i];
	}
	
	ps.Process(_frames, size);

	for (size_t i = 0; i < size; i++) {
		 out[0][i] = _frames[i].l;
		 out[1][i] = _frames[i].r;
	}
}

int main(void) {
	hw.Init();

	ps.Init(_buf);
	ps.set_ratio(stmlib::SemitonesToRatio(0));
	ps.set_size(1.0);

	hw.SetAudioBlockSize(kBlockSize);
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);
	while(1) {}
}
