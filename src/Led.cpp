#include "Led.h"

Led::Led(uint8_t pin) {
    this->pin = pin;

    pinMode(pin, OUTPUT);

    // defaults
    blinkTime = 0;
    freq = 2;

    state = state_off;
    curr = mode_off;
    prev = curr;

    updated = 0;
    changed = true;
}

void Led::turnOn() {
    if (curr != mode_on) {
#ifdef DEBUG_LEDS
        USE_DEBUG_SERIAL.printf("<led #%d on>", pin);
#endif
        curr = mode_on;
        changed = true;
    }
}

void Led::turnOff() {
    if (curr != mode_off) {
#ifdef DEBUG_LEDS
        USE_DEBUG_SERIAL.printf("<led#%d off>", pin);
#endif
        curr = mode_off;
        changed = true;
    }
}

void Led::setFreq(uint8_t hz) {
    if (hz > 0) {
        freq = hz;
        intervalOn = (500.0 / freq);
        intervalOff = (1000.0 / freq);
    }
#ifdef DEBUG_LEDS
    USE_DEBUG_SERIAL.printf("<led#%d freq %d on %d off %d>", pin, freq,
                            intervalOn, intervalOff);
#endif
}

void Led::blink(uint8_t sec) {
#ifdef DEBUG_LEDS
    USE_DEBUG_SERIAL.printf("<led#%d blink %d>", pin, sec);
#endif
    unsigned long now = millis();
    if (sec > 0) {
        blinkTime = now + (sec * 1000);
    } else {
        blinkTime = 0;
    }
    prev = curr;
    curr = mode_blink;
}

void Led::loop() {
    unsigned long now = millis();

    if (curr == mode_blink) {
        if ((blinkTime > 0) && (now > blinkTime)) {
            curr = prev;
#ifdef DEBUG_LEDS
            USE_DEBUG_SERIAL.printf("<led#%d blink finished %d>", pin, curr);
#endif
            changed = true;
        } else {
            unsigned long passed = now - updated;
            if (passed > (1000 / freq)) {
                changed = true;
                updated = now;
#ifdef DEBUG_LEDS
                USE_DEBUG_SERIAL.printf("<led#%d blink>", pin);
#endif
            }
        };
    };

    refresh();
}

void Led::refresh() {
    if (changed) {
#ifdef DEBUG_LEDS
        USE_DEBUG_SERIAL.printf("<led#%d curr %d>", pin, curr);
#endif

        switch (curr) {
            case mode_off:
                state = state_off;
                break;
            case mode_on:
                state = state_on;
                break;
            case mode_blink:
                state = (state == state_off) ? state_on : state_off;
                break;
        }
        digitalWrite(pin, state);
#ifdef DEBUG_LEDS
        USE_DEBUG_SERIAL.printf("<led#%d->%d>", pin, state);
#endif
        changed = false;
    }
}