#include "Led.h"
#include "sigma_delta.h"

Led::Led(uint8_t pin) {
    this->pin = pin;
    pinMode(pin, OUTPUT);
    style = STAY_OFF;
    state = LIGHT_OFF;
    turnOff();
    lastStateUpdated = 0;
    digitalWrite(pin, state);
    shimmer = false;
}

void Led::turnOn() {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] turnOn()", pin);
    DEBUG.println();
#endif
    pattern_size = 1;
    pattern = new Pattern[pattern_size];
    pattern[0] = Pattern(LIGHT_ON, 0.0);
}

void Led::turnOff() {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] turnOff()", pin);
    DEBUG.println();
#endif
    pattern_size = 1;
    pattern = new Pattern[pattern_size];
    pattern[0] = Pattern(LIGHT_OFF, 0.0);
}

void Led::regularBlink() {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] regularBlink()", pin);
    DEBUG.println();
#endif
    delete[] pattern;

    const uint8_t freq = 2;

    pattern_size = 2;
    pattern = new Pattern[pattern_size];
    pattern[0] = Pattern(LIGHT_OFF, 500.0 / freq);
    pattern[1] = Pattern(LIGHT_ON, 1000.0 / freq);
}

void Led::accentOneBlink() {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] accentOneBlink()", pin);
    DEBUG.println();
#endif
    delete[] pattern;

    pattern_size = 3;
    pattern = new Pattern[pattern_size];
    pattern[0] = Pattern(LIGHT_ON, 1000);
    pattern[1] = Pattern(LIGHT_OFF, 100);
    pattern[2] = Pattern(LIGHT_ON, 100);
}

void Led::accentTwoBlink() {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] accentTwoBlink()", pin);
    DEBUG.println();
#endif
    delete[] pattern;

    pattern_size = 4;
    pattern = new Pattern[pattern_size];
    pattern[0] = Pattern(LIGHT_ON, 1000);
    pattern[1] = Pattern(LIGHT_OFF, 100);
    pattern[2] = Pattern(LIGHT_ON, 100);
    pattern[3] = Pattern(LIGHT_OFF, 100);
}

void Led::setShimmering(bool enabled) {
    if (enabled) {
        sigmaDeltaSetup(0, 1000);
        sigmaDeltaAttachPin(pin);
    } else {
        sigmaDeltaDetachPin(pin);
    }
    shimmer = enabled;
}

void Led::setStyle(LedStyle style) {
    if (this->style == style) return;
    switch (style) {
        case STAY_OFF:
            turnOff();
            break;
        case STAY_ON:
            turnOn();
            break;
        case BLINK_REGULAR:
            regularBlink();
            break;
        case BLINK_ONE_ACCENT:
            accentOneBlink();
            break;
        case BLINK_TWO_ACCENT:
            accentTwoBlink();
            break;
    }
    this->style = style;
    pattern_n = 0;
}

void Led::loop() {
    LedState needs = getPattern()->state;
    if (this->state != needs) setState(needs);
    unsigned long duration_ms = getPattern()->duration_ms;
    unsigned long passed_ms = millis() - lastStateUpdated;
    if (duration_ms != 0) {
        if (passed_ms >= duration_ms) {
            setNextState();
        } else if ((shimmer) && (passed_ms - lastSigmaDeltaUpdated >= 5)) {
            lastSigmaDeltaUpdated += 5;
            uint8_t duty = 0;
            if (getNextState() == LIGHT_OFF) {
                duty = 255 - floor(((float) passed_ms / duration_ms) * 255);                
            } else if (this->state == LIGHT_ON) {
                duty = floor(((float) passed_ms / duration_ms) * 255);
            }
            sigmaDeltaWrite(0, duty);
        }
    }
}

void Led::setState(LedState value) {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] setState(%d)", pin, state);
    DEBUG.println();
#endif
    if (!shimmer) digitalWrite(pin, value);
    this->state = value;
    lastSigmaDeltaUpdated = 0;
    lastStateUpdated = millis();
}

Pattern* Led::getPattern() {
    return &pattern[pattern_n];
}

LedState Led::getNextState() {
    uint8_t n = pattern_n;
    if (n >= pattern_size - 1) n = 0;    
    return pattern[n].state;
}

void Led::setNextState() {
    if (pattern_n < pattern_size - 1)
        pattern_n++;
    else
        pattern_n = 0;

    setState(getPattern()->state);
}