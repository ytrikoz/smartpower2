#pragma once
#include "Arduino.h"

typedef enum { led_stay_off, led_stay_on, led_blink } LedMode;
typedef enum { led_off = HIGH, led_on = LOW } LedState;

class Led {
   public:
    Led(uint8_t pin);
    void loop();
    void turnOn();
    void turnOff();
    void blink(uint8_t sec = 0, uint8_t hz = 2);
   private:
    void setMode(LedMode mode);
    void setFreq(uint8_t hz);   
    void refresh();
    uint8_t pin;
    LedMode activeMode, previosMode;
    LedState state;
    bool changed;

    uint8_t blinkFreq, prevBlinkFreq;
    uint16_t intervalOn, intervalOff;
    unsigned long blinkTime;
    unsigned long blinkStart;
    unsigned long lastUpdate;
};