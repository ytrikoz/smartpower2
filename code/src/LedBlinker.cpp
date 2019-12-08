#include "LedBlinker.h"

namespace {
#define LED_PWM_DUTY_OFF 80

/* LED  off            on on              off
 * per  0      0.5      1 0       0.5     1
 * phasefalse             true            false
 * duty DUTY_MAX          DUTY_MIN        DUTY_MAX
 */
uint16_t per_to_pwm_duty(float per, bool phase = false) {
    uint16_t range = LED_PWM_DUTY_OFF;
    if (!phase)
        per = 1 - per;
    uint16_t duty = floor(per * range);
    return duty;
}
} // namespace

LedBlinker::LedBlinker(uint8_t pin, bool on, bool smooth) {
    this->pin = pin;
    this->smooth = smooth;
    pinMode(pin, OUTPUT);
    set(on ? STAY_ON : STAY_OFF, true);
}

size_t LedBlinker::onDiag(Print *p) {
    size_t n = p->print(state);
    n += p->print(' ');
    n += p->print(mode);
    n += p->print('/');
    n += p->print(step + 1);
    n += p->print('-');
    return n += p->println(size);
}

void LedBlinker::set(LedMode mode, bool forced) {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] set(%d)", pin, mode);
    DEBUG.println();
#endif
    if (this->mode == mode && !forced)
        return;

    this->mode = mode;
    this->step = 0;
    switch (mode) {
    case STAY_OFF:
        size = 1;
        pattern[0] = {LED_OFF, 0};
        break;
    case STAY_ON:
        size = 1;
        pattern[0] = {LED_ON, 0};
        break;
    case BLINK:
        size = 2;
        pattern[0] = {LED_OFF, 250};
        pattern[1] = {LED_ON, 500};
        break;
    case BLINK_ALERT:
        size = 4;
        pattern[0] = {LED_ON, 1000};
        pattern[1] = {LED_OFF, 100};
        pattern[2] = {LED_ON, 100};
        pattern[3] = {LED_OFF, 100};
        break;
    case BLINK_ERROR:
        size = 4;
        pattern[0] = {LED_ON, 100};
        pattern[1] = {LED_OFF, 50};
        pattern[2] = {LED_ON, 100};
        pattern[3] = {LED_OFF, 50};
        break;
    }
    updateState(getPattern()->state);
}

Pattern *LedBlinker::getPattern() { return &pattern[step]; }

void LedBlinker::next() {
    if (step < size - 1)
        step++;
    else
        step = 0;
}

void LedBlinker::loop() {
    unsigned long now = millis();
    LedState req = getPattern()->state;
    if (state != req) {
        updateState(req);
        stateUpdated = now;
    }
    if (getPattern()->time == 0)
        return;
    unsigned long passed = millis_passed(stateUpdated, now);
    if (passed < getPattern()->time) {
        this->updateDuty(passed);
    } else if (passed >= getPattern()->time) {
        next();
    }
}

void LedBlinker::updateDuty(unsigned long passed) {
    float f = (float)passed / getPattern()->time;
    uint16_t duty = per_to_pwm_duty(f, !getPattern()->state);
    analogWrite(pin, duty);
}

void LedBlinker::updateState(LedState req) {
    digitalWrite(pin, req);
    state = req;
}
