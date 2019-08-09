#include "Led.h"

#define LED_SHIM_INTERVAL_ms 10
#define LED_OFF_DUTY_CYCLE 30
#define LED_ON_DUTY_CYCLE 255

Led::Led(uint8_t pin, LedState startState, bool enableShim) {
    this->pin = pin;
    this->shimEnabled = enableShim;
    pinMode(pin, OUTPUT);
    if (enableShim) {
        sigmaDeltaSetup(0, 1000);
        sigmaDeltaAttachPin(pin);
    }
    set((startState == LIGHT_ON ? STAY_ON : STAY_OFF));
    setState(startState);
}

void Led::set(LedMode mode) {
    switch (mode) {
        case STAY_OFF:
            turnOff();
            break;
        case STAY_ON:
            turnOn();
            break;
        case BLINK:
            blink();
            break;
        case BLINK_ONE:
            blinkSeqOne();
            break;
        case BLINK_TWO:
            blinkSeqTwo();
            break;
    }
    this->mode = mode;
    step = 0;
}

void Led::turnOn() {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] turnOn()", pin);
    DEBUG.println();
#endif
    size = 1;
    contract[0] = LedContract(LIGHT_ON, 0);
}

void Led::turnOff() {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] turnOff()", pin);
    DEBUG.println();
#endif
    size = 1;
    contract[0] = LedContract(LIGHT_OFF, 0);
}

void Led::blink() {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] blink()", pin);
    DEBUG.println();
#endif
    size = 2;
    contract[0] = LedContract(LIGHT_OFF, floor((float)500 / 2));
    contract[1] = LedContract(LIGHT_ON, floor((float)1000 / 2));
}

void Led::blinkSeqOne() {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] blinkSeqOne()", pin);
    DEBUG.println();
#endif
    size = 3;
    contract[0] = LedContract(LIGHT_ON, 1000);
    contract[1] = LedContract(LIGHT_OFF, 100);
    contract[2] = LedContract(LIGHT_ON, 100);
}

void Led::blinkSeqTwo() {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] blinkSeqTwo()", pin);
    DEBUG.println();
#endif
    size = 4;
    contract[0] = LedContract(LIGHT_ON, 1000);
    contract[1] = LedContract(LIGHT_OFF, 100);
    contract[2] = LedContract(LIGHT_ON, 100);
    contract[3] = LedContract(LIGHT_OFF, 100);
}


uint8_t f_to_duty(float f, bool invert) {
    uint8_t duty = LED_OFF_DUTY_CYCLE + floor(f * (LED_ON_DUTY_CYCLE - LED_OFF_DUTY_CYCLE));
    if (invert) duty = 255 - duty;
    return duty;
}

void Led::setState(LedState state) {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] setState(%d)", pin, state);
    DEBUG.println();
#endif
    this->state = state;
    if (shimEnabled) {
        sigmaDeltaWrite(0, (state == LIGHT_ON) ? LED_ON_DUTY_CYCLE : LED_OFF_DUTY_CYCLE);
    } else {
        digitalWrite(pin, state);
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
    long stateTime = getContract()->stateTime;    
    if (stateTime == 0) return ;
    if (millis_since(stateUpdated) >= stateTime) {
        nextStep();
    }
}

void Led::updateState() {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] updateState(%d)", pin, contractState);
    DEBUG.println();
#endif
    LedState contract = getContract()->state;
    if (state != contract) {
        setState(contract);
        return;        
    }     
    if (shimEnabled && millis_since(shimUpdated) > LED_SHIM_INTERVAL_ms) {
        float f = (float) millis_passed(stateUpdated, shimUpdated) / getContract()->stateTime;
        uint8_t duty = f_to_duty(f, getContract()->state == LIGHT_ON);
        sigmaDeltaWrite(0, duty);
        shimUpdated = millis();
    }
}