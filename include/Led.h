#pragma once

#include <Arduino.h>

#include "consts.h"
#include "debug.h"
#include "sigma_delta.h"
#include "time_utils.h"

enum LedState { LIGHT_OFF = HIGH, LIGHT_ON = LOW };

enum LedMode { STAY_OFF, STAY_ON, BLINK, BLINK_ONE, BLINK_TWO };

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
    void loop();
    void setMode(LedMode mode, bool force = false);

   private:
    void updateState();
    void setLed(LedState state);
    LedContract *getContract();

    void turnOn();
    void turnOff();
    void blink();
    void blinkSeqOne();
    void blinkSeqTwo();

    void nextStep();
    void refresh();

    bool shim;
    uint8_t pin;
    LedMode mode;
    LedState state;
    unsigned long stateUpdated = 0, shimUpdated = 0;
    size_t step = 0;
    size_t size = 1;
    LedContract contract[4];
};