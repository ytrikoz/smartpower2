#include "Led.h"

Led::Led(uint8_t pin) {
    this->pin = pin;
    pinMode(pin, OUTPUT);    
    style = STAY_OFF;
    state = LIGHT_OFF;    
    turnOff();
    lastUpdate = 0;
    digitalWrite(pin, state);
}

void Led::turnOn() {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] turnOn()", pin);
    DEBUG.println();
#endif
    pattern_size = 1;
    pattern = new Pattern[pattern_size];
    pattern[0] = Pattern(LIGHT_ON, 1000.0);
}

void Led::turnOff() {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] turnOff()", pin);
    DEBUG.println();
#endif
    pattern_size = 1;
    pattern = new Pattern[pattern_size];
    pattern[0] = Pattern(LIGHT_OFF, 1000.0);
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

void Led::setStyle(LedStyle style) {
    if (this->style == style) return;
    switch (style) {
        case STAY_OFF:
            turnOff();
            break;
        case STAY_ON:
            turnOn();
            break;
        case BLINK_REGULAR : 
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

Pattern* Led::getPattern() { return &pattern[pattern_n]; }

void Led::loop() {
    LedState needs = getPattern()->state;
    if (this->state != needs) setState(needs);    
    unsigned long duration_ms = getPattern()->duration_ms;
    if (( duration_ms != 0) && (millis() - lastUpdate >= duration_ms)) {
        setNextState();
        lastUpdate = millis();  
    }      
}

void Led::setState(LedState value) {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] setState(%d)", pin, state);
    DEBUG.println();
#endif
    this->state = value;
    digitalWrite(pin, value);
}

void Led::setNextState() {
    if (pattern_n < pattern_size - 1)
        pattern_n++;
    else
        pattern_n = 0;

    setState(getPattern()->state);
}