#pragma once

#include <Arduino.h>

#include "BuildConfig.h"
#include "Consts.h"
#include "TimeUtils.h"

enum LedState { LIGHT_OFF = HIGH, LIGHT_ON = LOW };
enum LedMode { STAY_OFF, STAY_ON, BLINK, BLINK_ONE, BLINK_TWO, BLINK_ERROR };

#define LED_SHIM_INTERVAL_ms 50
#define PWM_FREQ 500
#define PWM_RANGE 100

struct LedContract {
   public:
    LedState state;
    unsigned long stateTime;
    LedContract() : state(LIGHT_OFF), stateTime(0) {}
    LedContract(LedState state, unsigned long time)
        : state(state), stateTime(time) {}
};

class Led {
   public:
    Led(uint8_t pin, LedState state = LIGHT_OFF, bool shim = false);
    void set(LedMode mode);
    void loop();

   private:
    void updateState();
    void setState(LedState state);
    LedContract *getContract();

    void nextStep();

    bool shim;
    uint8_t pin;
    LedMode mode;
    LedState state;
    unsigned long stateUpdated = 0, shimUpdated = 0;
    size_t step = 0;
    size_t size = 1;
    LedContract contract[4];
};
