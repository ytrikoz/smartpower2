#include "Led.h"

namespace Led {

/* LED  off            on on              off
 * per  0      0.5      1 0       0.5     1
 * dim  false             true            false
 * duty DUTY_MAX          DUTY_MIN        DUTY_MAX
 */
uint16_t per_to_pwm_duty(float per, bool dim = false) {
    uint16_t duty = per * LED_PWM_DUTY_OFF;    
    if (!dim) {
        duty = LED_PWM_DUTY_OFF - duty;
    }
    return duty;
}

Led::Led(uint8_t pin, bool lightOn, bool smoothStateChange) {
    analogWriteFreq(LED_PWM_FREQ);
    analogWriteRange(LED_PWM_RANGE);
    pinMode(pin, OUTPUT);

    this->pin = pin;
    this->smooth = smoothStateChange;
    
    set(lightOn ? ON : OFF);
}

void Led::set(Mode mode) {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] set(%d)", pin, mode);
    DEBUG.println();
#endif
    switch (mode) {
        case OFF:
            size = 1;
            contract[0] = Contract(LIGHT_OFF, 0);
            break;
        case ON:
            size = 1;
            contract[0] = Contract(LIGHT_ON, 0);
            break;
        case BLINK:
            size = 2;
            contract[0] = Contract(LIGHT_ON, floor((float)1000 / 2));
            contract[1] = Contract(LIGHT_OFF, floor((float)500 / 2));
            break;
        case BLINK_ONE:
            size = 3;
            contract[0] = Contract(LIGHT_ON, 1000);
            contract[1] = Contract(LIGHT_OFF, 100);
            contract[2] = Contract(LIGHT_ON, 100);
            break;
        case BLINK_TWO:
            size = 4;
            contract[0] = Contract(LIGHT_ON, 1000);
            contract[1] = Contract(LIGHT_OFF, 100);
            contract[2] = Contract(LIGHT_ON, 100);
            contract[3] = Contract(LIGHT_OFF, 100);
            break;
        case BLINK_ERROR:
            size = 4;
            contract[0] = Contract(LIGHT_ON, 100);
            contract[1] = Contract(LIGHT_OFF, 50);
            contract[2] = Contract(LIGHT_ON, 100);
            contract[3] = Contract(LIGHT_OFF, 50);
            break;
    }
    this->mode = mode;
    this->step = 0;

    updateState(getContract()->state);
}

Contract* Led::getContract() { return &contract[step]; }

void Led::next() {
    if (step < size - 1)
        step++;
    else
        step = 0;
}

void Led::loop() {
    State contract = getContract()->state;
    if (state != contract) {
        updateState(contract);
        return;
    }
    if (getContract()->stateTime > 0) {
        if (smooth && millis_since(pwmUpdated) > LED_PWM_UPDATE_INTERVAL_ms) {
            this->smoothTransition();
        }
    }
    if (millis_since(stateUpdated) >= getContract()->stateTime) next();
}

void Led::smoothTransition() {
    float per = ((float)millis_passed(stateUpdated, pwmUpdated)) /
                getContract()->stateTime;
    uint16_t duty = per_to_pwm_duty(per, getContract()->state == LIGHT_ON);
    analogWrite(pin, duty);
    pwmUpdated = millis();
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] analogWrite(%d)", pin, duty);
    DEBUG.println();
#endif
}

void Led::updateState(State new_state) {
    if (this->smooth) {
        uint8_t duty = per_to_pwm_duty(!new_state);
        analogWrite(pin, duty);
#ifdef DEBUG_LEDS
        DEBUG.printf("[led#%d] analogWrite(%d)", pin, duty);
        DEBUG.println();
#endif
    } else {
        digitalWrite(pin, new_state);
#ifdef DEBUG_LEDS
        DEBUG.printf("[led#%d] digitalWrite(%d)", pin, state);
        DEBUG.println();
#endif
    }

    this->state = new_state;
    this->stateUpdated = millis();
    this->pwmUpdated = stateUpdated;
}

}  // namespace Led