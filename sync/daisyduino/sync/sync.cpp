#include "sync.h"

void Sync::initialize(Config& config, TickCallback callback) {
    _tick_generation_interval_mks = config.generation_interval_mks;
    _int_ppqn = config.internal_ppqn;
    _int_ticks_per_ext_tick = config.internal_ppqn / config.external_ppqn;
    _trtime = _tick_generation_interval_mks * config.internal_ppqn;
    _tick_callback = callback;
}


// Called by internal interrupt timer (audio callback in this implementation).
void Sync::generate() {
    if (!_is_playing) return;
    sync();
}


// External clock tick received
// This method starts playback on the first received tick
// after playback was scheduled. After that, calls sync 
// for every tick received.
void Sync::external_clock_tick() {
    if (!_is_playing && !_is_about_to_play) return;

    if (_is_about_to_play) {
        _is_about_to_play = false;
        _is_playing = true;
    }
    else {
        _resync = true;
        _hold = false;
        sync();
    }
}


// Derived from Maximum MIDI Music Applications in C++ by Paul Messick
// This method generates internal ticks and also synchronises to the external clock.
// So it's called both from internal interrupt timer (audio callback in this implementation) and upon external clock tick reception.
// nticks - integer count of internal ticks
// _fticks - fractional count of internal ticks
// _tempo_ticks - integer ticks count since last external tick
// _tempo_mks - tempo in microseconds / beat (quarter note)
// _resync - flag to resync to external clock. Is set to true once external clock tick is received.
// _hold - flag to stop advancing internal timeline if the number of internal ticks exceeded expected count of internal ticks per extrnal tick
// _trtime - internal resolution (ppqn) multiplied by interrupt interval.
void Sync::sync() {
    uint32_t nticks = 0;

    //If we generated more internal ticks per extrnal tick as expected,
    //we don't advance internal "timeline", but only accumulate _tempo_ticks
    //in order to calculate and correct the tempo.
    //This flag is set to false upon reception of the external tick.
    if (_hold) {
        nticks = (_fticks + _trtime) / _tempo_mks;
        _fticks += _trtime - (nticks * _tempo_mks);
        _tempo_ticks += nticks;
        return;
    }

    //Once a tick of the extrnal clock is received,
    //we do resync, i.e. align inernal timeline with the external one
    //and adjust tempo.
    if (_resync) {
        _fticks = 0;
        nticks = _int_ticks_per_ext_tick - (_ticks - _ticks_at_last_clock);
        _ticks_at_last_clock = _ticks + nticks;
        _tempo_mks -= ((int32_t)_int_ticks_per_ext_tick - (int32_t)_tempo_ticks) * (int32_t)_tempo_mks / (int32_t)_int_ppqn;
        _tempo_ticks = 0;
        _resync = false;
    }
    //Regular mode. We generate internal ticks.
    else {
        nticks = (_fticks + _trtime) / _tempo_mks;
        _fticks += _trtime - nticks * _tempo_mks;
        if (_is_synced_to_external_clock) {
            _tempo_ticks += nticks;
            //If there are more internal ticks per the external tick than 
            //expected, we set _hold to true effectively stopping advancing timeline
            //until next external tick
            if (_ticks - _ticks_at_last_clock + nticks >= _int_ticks_per_ext_tick) {
                nticks = _int_ticks_per_ext_tick - 1 - (_ticks - _ticks_at_last_clock);
                _hold = true;
            }
        }
    }

    //Accumulate ticks
    _ticks += nticks;

    //Advance timeline
    if (_tick_callback != nullptr) {
      for (uint32_t i = 0; i < nticks; i++) _tick_callback();
    }   
}


// In case of external clock sync this
// method only schedules playback. Actual playback 
// starts on the first tick of the external clock.
// see clock_in_tick() below.
void Sync::set_is_playing(bool is_playing) {
    if (is_playing == _is_playing) return;
     
    if (is_playing) {
        if (_is_synced_to_external_clock) _is_about_to_play = true;
        else _is_playing = true;
    }
    else {
        _is_playing = false;
        reset();
    }
}

// Setting tempo from internal control. 
// Has no effect in case of syncing to extrnal clock.
void Sync::set_tempo(float norm_value) {
    if (_is_synced_to_external_clock) return;
    _tempo = (kTempoMax - kTempoMin) * norm_value + kTempoMin;
    _tempo_mks = tempo_mks(_tempo);
}

void Sync::reset() {
    _fticks = 0;
    _ticks = 0;
    _ticks_at_last_clock = 0;
    _tempo_ticks = 0;
    _hold = false;
    _resync = false;
}
