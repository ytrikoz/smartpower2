#pragma once

#include <Arduino.h>

#include "BuildConfig.h"
#include "Consts.h"
#include "StrUtils.h"
#include "TimeUtils.h"

namespace Led {

enum LedLamp { POWER_LED, WIFI_LED };
enum LedState { LED_OFF = HIGH, LED_ON = LOW };
enum LedMode { STAY_OFF, STAY_ON, BLINK, BLINK_ALERT, BLINK_ERROR };

struct Pattern {
  public:
    LedState state;
    unsigned long time;
    Pattern() : state(LED_OFF), time(0) {}
    Pattern(LedState state, unsigned long time) : state(state), time(time) {}
};

class LedBlinker {
  public:
    void set(LedMode mode, bool forced = false);

  public:
    LedBlinker(uint8_t pin, bool lightOn = false, bool smooth = false);
    void loop();
    size_t printDiag(Print *p);

  private:
    void updateState(LedState state);
    void updateDuty(unsigned long passed);
    Pattern *getPattern();
    void next();

  private:
    uint8_t pin;
    bool smooth;
    LedMode mode;
    LedState state;
    unsigned long stateUpdated = 0, pwmUpdated = 0;
    Pattern pattern[4];
    size_t step = 0, size = 1;
};

} // namespace Led