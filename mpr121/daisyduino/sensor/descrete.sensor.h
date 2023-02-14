#pragma once

#include "DaisyDuino.h"
#include <Wire.h>
#include "Adafruit_MPR121.h"
#include "descrete.sensor.pad.h"

#ifndef _pin
#define _pin(shift) (1 << shift)
#endif

#ifndef _together
#define _together(a, b) (a | b)
#endif

class  DescreteSensor {
public:
    //PADS TARGETS #####################################################
    //
    enum class Target {
        PlayStop,
        OneShot,
        Note
    };
    //
    //TARGET COUNT ######################################################
    //
    //Should be the same as the number of entries in Target enum
    static const int targets_count = 3;
    //
    //###################################################################

    DescreteSensor() = default;
    ~ DescreteSensor() = default;

    void initialize() {
        _mpr.begin(0x5A);

        //TARGET TO PIN MAPPING #########################################
        //
        //The order should correspond order of Target enum entries
        uint16_t mask[targets_count] = {
          _pin(0), //PlayStop to Pin 0
          _pin(1), //OneShot to Pin 1
          _together(_pin(2), _pin(3)) //Note to pins 2 and 3 simultaneously touched
        };
        //
        //#################################################################

        for (auto i = 0; i < targets_count; i++) _pads[i].initialize(mask[i]);
    }

    void process() {
        auto state = _mpr.touched();
        for (auto& p: _pads) p.process(state);
    }
    
    void set_on_touch(std::function<void()> on_touch, Target target) {
        pad(target).on_touch = on_touch;
    }

    void set_on_release(std::function<void()> on_release, Target target) {
        pad(target).on_release = on_release;
    }

    void set_mode(DescreteSensorPad::Mode mode, Target target) {
        pad(target).mode = mode;
    }

    bool is_on(Target target) {
        return pad(target).is_on();
    }

private:
    Adafruit_MPR121 _mpr;
    DescreteSensorPad _pads[targets_count];
    
    DescreteSensorPad& pad(Target target) {
        auto index = static_cast<int>(target);
        return _pads[index];
    }
};
