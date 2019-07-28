#pragma once

#include <Arduino.h>

#include "consts.h"

typedef enum { LIGHT_OFF = HIGH, LIGHT_ON = LOW } LedState;
typedef enum {
    STAY_OFF,
    STAY_ON,
    BLINK_REGULAR,
    BLINK_ONE_ACCENT,
    BLINK_TWO_ACCENT
} LedStyle;

typedef struct Pattern {
   public:
    LedState state;
    unsigned long duration_ms;
    Pattern() : state(LIGHT_OFF), duration_ms(0) {}
    Pattern(LedState state, unsigned long duration_ms)
        : state(state), duration_ms(duration_ms) {}
} Pattern;

class Led {
   public:
    Led(uint8_t pin);
    void loop();
    void setStyle(LedStyle style);

   private:
    void turnOn();
    void turnOff();
    void regularBlink();
    void accentOneBlink();
    void accentTwoBlink();
    Pattern *getPattern();
    void setNextState();
    void setState(LedState value);
    void refresh();

    uint8_t pin;
    LedStyle style;
    LedState state;
    unsigned long lastUpdate;

    size_t pattern_n;
    size_t pattern_size;
    Pattern *pattern;
};