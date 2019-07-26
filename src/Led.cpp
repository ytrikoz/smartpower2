#include "Led.h"

Led::Led(uint8_t pin) {
    this->pin = pin;
    pinMode(pin, OUTPUT);

    blinkTime = 0;
    blinkFreq = 0;
    state = led_off;
    activeMode = led_stay_off;
    lastUpdate = 0;
    changed = true;
}

void Led::turnOn() {
    #ifdef DEBUG_LEDS
    DEBUG.printf("<led#%d turnOn()>", pin);
    #endif
    setMode(led_stay_on);
}

void Led::turnOff() {
    #ifdef DEBUG_LEDS
    DEBUG.printf("<led#%d turnOff()>", pin);
    #endif
    setMode(led_stay_off);
}

void Led::blink(uint8_t sec, uint8_t hz) {
    #ifdef DEBUG_LEDS
        USE_DEBUG_SERIAL.printf("<led#%d blink %d &d>", pin, sec, hz);
    #endif
    if (sec > 0) {
        blinkTime = sec * 1000;
        blinkStart = millis();
    } else {
        blinkTime = 0;
        blinkStart = 0;
    }
    setFreq(hz);
    setMode(led_blink);
}


void Led::loop() {                      
    if (activeMode == led_blink) {
        unsigned long timePassed = millis() - lastUpdate;           
        if ((state == led_on) && (timePassed >= intervalOn))
            state = led_off;
        else if ((state == led_off) && (timePassed >= intervalOff))
            state = led_on;
        else 
            return;
    
        changed = true;
        lastUpdate = millis();
        refresh();
    }
}

void Led::refresh() {
    if (changed) {
        #ifdef DEBUG_LEDS
            DEBUG.printf("<led#%d mode %d>", pin, activeMode);
        #endif
        if (activeMode == led_stay_off) {
            state = led_off;
        } else if (activeMode == led_stay_on) {
            state = led_on;            
        }
        digitalWrite(pin, state);
#ifdef DEBUG_LEDS
        DEBUG.printf("<led#%d->%d>", pin, state);
#endif
        changed = false;
    }
}

void Led::setMode(LedMode mode) {
    if (activeMode != mode) {
        previosMode = activeMode;
        activeMode = mode;
        changed = true;
    }    
}

void Led::setFreq(uint8_t hz) {
    #ifdef DEBUG_LEDS
        USE_DEBUG_SERIAL.printf("<led#%d freq %d on %d off %d>", pin, freq, intervalOn, intervalOff);
    #endif
    if (hz == 0) {
        turnOff();
        return;
    }
    if (hz != blinkFreq)
    {
        prevBlinkFreq = blinkFreq;
     
        blinkFreq = hz;
        intervalOn = (500.0 / blinkFreq);
        intervalOff = (1000.0 / blinkFreq);
    }
}
