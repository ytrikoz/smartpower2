#include "Led.h"

int per_to_pwm_duty(float per, bool backwards = false) {
    int duty = floor(per * PWM_RANGE);
    if (backwards) duty = PWM_RANGE - duty;
    return duty;
}

Led::Led(uint8_t pin, LedState startState, bool shim) {
    this->pin = pin;
    this->shim = shim;
    pinMode(pin, OUTPUT);
    if (shim) {     
        analogWriteFreq(PWM_FREQ);
        analogWriteRange(PWM_RANGE);    
    }    
    set((startState == LIGHT_ON ? STAY_ON : STAY_OFF));
}

void Led::set(LedMode mode) {
    #ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] set(%d)", pin, mode);
    DEBUG.println();
    #endif
    switch (mode) {
        case STAY_OFF:
            size = 1;
            contract[0] = LedContract(LIGHT_OFF, 0);
            break;
        case STAY_ON:
            size = 1;
            contract[0] = LedContract(LIGHT_ON, 0);
            break;
        case BLINK:
            size = 2;
            contract[0] = LedContract(LIGHT_ON, floor((float)1000 / 2));
            contract[1] = LedContract(LIGHT_OFF, floor((float)500 / 2));            
            break;
        case BLINK_ONE:
            size = 3;
            contract[0] = LedContract(LIGHT_ON, 1000);
            contract[1] = LedContract(LIGHT_OFF, 100);
            contract[2] = LedContract(LIGHT_ON, 100);
            break;
        case BLINK_TWO:
            size = 4;
            contract[0] = LedContract(LIGHT_ON, 1000);
            contract[1] = LedContract(LIGHT_OFF, 100);
            contract[2] = LedContract(LIGHT_ON, 100);
            contract[3] = LedContract(LIGHT_OFF, 100);
            break;
        case BLINK_ERROR:
            size = 4;
            contract[0] = LedContract(LIGHT_ON, 100);
            contract[1] = LedContract(LIGHT_OFF, 50);
            contract[2] = LedContract(LIGHT_ON, 100);
            contract[3] = LedContract(LIGHT_OFF, 50);
            break;
    }
    this->mode = mode;
    step = 0;
    setState(getContract()->state);
}

void Led::setState(LedState state) {
    this->state = state;
    if (shim) {
        int duty = (state == LIGHT_ON) ?  per_to_pwm_duty(0): per_to_pwm_duty(1);
        analogWrite(pin, duty);        
        #ifdef DEBUG_LEDS
        DEBUG.printf("[led#%d] analogWrite(%d)", pin, duty);
        DEBUG.println();
        #endif
    } else {
        digitalWrite(pin, state);
        #ifdef DEBUG_LEDS
        DEBUG.printf("[led#%d] digitalWrite(%d)", pin, state);
        DEBUG.println();
        #endif
    }
    stateUpdated = millis();
    shimUpdated = stateUpdated;
}

LedContract* Led::getContract() { return &contract[step]; }

void Led::nextStep() {
    if (step < size - 1)
        step++;
    else
        step = 0;
}

void Led::loop() {    
    this->updateState();
    if (millis_since(stateUpdated) >= getContract()->stateTime) nextStep();
}
 
void Led::updateState() {
    LedState contract = getContract()->state;
    if (state != contract) {
        setState(contract);
        return;        
    }
    if (getContract()->stateTime > 0) 
    {
        if (shim && millis_since(shimUpdated) > LED_SHIM_INTERVAL_ms) {
            float per = ((float) millis_passed(stateUpdated, shimUpdated)) / getContract()->stateTime;
            uint16_t duty = per_to_pwm_duty(per, getContract()->state == LIGHT_OFF);
            analogWrite(pin, duty);
            shimUpdated = millis();
            #ifdef DEBUG_LEDS
            DEBUG.printf("[led#%d] analogWrite(%d)", pin, duty);
            DEBUG.println();
            #endif
        }
    }
}