#include "DaisyDuino.h"
#include "sync.h"

Sync sync;

const uint32_t kMicroSecInSec = 1e6;
int clock_pin = 0;

static void AudioCallback(float **in, float **out, size_t size) {

  sync.generate();

  for (size_t i = 0; i < size; i++) {
    out[0][i] = in[0][i];
    out[1][i] = in[1][i];
  }
}

static void SyncTickCallback() {
  //Advance timeline
}

void setup() {
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  Sync::Config cfg;
  cfg.generation_interval_mks = kMicroSecInSec / DAISY.AudioCallbackRate();
  cfg.internal_ppqn = 96; 
  cfg.external_ppqn = 4;
  sync.initialize(cfg, SyncTickCallback);
  sync.set_is_synced_to_external_clock(true);
  sync.set_is_playing(true);
  
  pinMode(clock_pin, INPUT_PULLDOWN);

  DAISY.begin(AudioCallback);
}

void loop() {
  //Consider adding debounce
  static auto clock_pin_state = false;
  auto new_state = digitalRead(clock_pin);
  if (new_state && !clock_pin_state) {
    sync.external_clock_tick();
  }
  clock_pin_state = new_state;
}
