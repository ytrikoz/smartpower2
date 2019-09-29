#include "LedBlinker.h"

namespace Led {
#define LED_PWM_DUTY_OFF 80
#define LED_PWM_UPDATE_INTERVAL_ms 25

/* LED  off            on on              off
 * per  0      0.5      1 0       0.5     1
 * phasefalse             true            false
 * duty DUTY_MAX          DUTY_MIN        DUTY_MAX
 */
uint16_t per_to_pwm_duty(float per, bool phase = false) {
    uint16_t duty = per * LED_PWM_DUTY_OFF;
    if (!phase)
        duty = LED_PWM_DUTY_OFF - duty;
    return duty;
}

LedBlinker::LedBlinker(uint8_t pin, bool lightOn, bool smooth) {
    this->pin = pin;
    this->smooth = smooth;
    pinMode(pin, OUTPUT);
    set(lightOn ? STAY_OFF : STAY_ON, true);
}

size_t LedBlinker::printDiag(Print *p) {
    size_t n = p->printf("pin %d mode %d state %d", pin, mode, state);
    return n += p->println();
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
        pattern[0] = {LED_OFF, 500};
        pattern[1] = {LED_ON, 250};
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
    LedState req = getPattern()->state;
    if (state != req) {
        updateState(req);
        return;
    }
    unsigned long passed = millis_since(pwmUpdated);
    if (getPattern()->time > 0) {
        if (smooth && (passed > LED_PWM_UPDATE_INTERVAL_ms)) {
            this->updateDuty(passed);
        }
    }
    passed = millis_since(stateUpdated);
    if (passed >= getPattern()->time)
        next();
}

void LedBlinker::updateDuty(unsigned long passed) {
    float f = (float)passed / getPattern()->time;
    uint16_t duty = per_to_pwm_duty(f, !getPattern()->state);
    analogWrite(pin, duty);
    pwmUpdated = millis();
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] analogWrite(%d)", pin, duty);
    DEBUG.println();
#endif
}

void LedBlinker::updateState(LedState req) {
    if (smooth) {
        uint8_t duty = per_to_pwm_duty(!req);
        analogWrite(pin, duty);
    } else {
        digitalWrite(pin, req);
    }
    this->state = req;
    stateUpdated = pwmUpdated = millis();
}

} // namespace Led