#pragma once

#include <cstdint>


typedef void (*TickCallback)();

class Sync {
public:
    struct Config {
      uint32_t generation_interval_mks;
      uint32_t internal_ppqn;
      uint32_t external_ppqn;
    };

    Sync() = default;
    ~Sync() = default;   

    void initialize(Config& config, TickCallback callback);
    void generate();
    void external_clock_tick();
    
    float tempo() { 
      return 60000000.f / _tempo_mks; 
    }
    void set_tempo(float normValue);

    void set_is_playing(bool is_playing);
    void set_is_synced_to_external_clock(bool value) {
      _is_synced_to_external_clock = value;
    }

private:
    void sync();
    void reset();
    uint32_t tempo_mks(const float tempo) {
        return static_cast<uint32_t>(60.f * 1e6 / tempo);
    }

    TickCallback _tick_callback = nullptr;

    const float kTempoMin = 30;
    const float kTempoMax = 240;

    bool _is_playing                  = false;
    bool _is_about_to_play            = false;
    bool _is_synced_to_external_clock = false;

    float _tempo                  = 120;
    uint32_t _tempo_mks           = 500000;

    uint32_t _tick_generation_interval_mks;
    uint32_t _int_ppqn;
    uint32_t _int_ticks_per_ext_tick;
    uint32_t _trtime;
    uint32_t _tempo_delta_mks     = 0;
    uint32_t _ticks               = 0;
    uint32_t _fticks              = 0;
    uint32_t _ticks_at_last_clock = 0;
    uint32_t _tempo_ticks         = 0;

    bool _hold                    = false;
    bool _resync                  = false;    
};
