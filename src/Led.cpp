#include "Led.h"

#define LED_UPDATE_INTERVAL_ms 5

Led::Led(uint8_t pin, LedState startState, bool shim) {
    this->pin = pin;
    this->shim = shim;
    pinMode(pin, OUTPUT);
    if (shim) {
        sigmaDeltaSetup(0, 1000);
        sigmaDeltaAttachPin(pin);
    }
    LedMode startMode = (startState == LIGHT_ON ? STAY_ON : STAY_OFF);
    setMode(startMode, true);
    setLed(startState);
}

void Led::setMode(LedMode mode, bool force) {
    if (!force && this->mode == mode) return;
    this->mode = mode;
    step = 0;
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

void Led::loop() {
    this->updateState();
    long duration = getContract()->stateTime;
    if (duration > 0 && millis_since(stateUpdated) >= duration) {
        nextStep();
    }
}

uint8_t per_to_duty(float per, bool invert) {
    const uint8_t min_duty = 30;
    const uint8_t max_duty = 255;
    uint8_t res = floor(per * (max_duty - min_duty)) + min_duty;
    if (invert) {
        res = 255 - res;
    }
    return res;
}

void Led::updateState() {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] updateState(%d)", pin, contractState);
    DEBUG.println();
#endif
    LedState contractState = getContract()->state;
    if (state != contractState) {
        setLed(contractState);
    } else if (shim && millis_since(shimUpdated) > LED_UPDATE_INTERVAL_ms) {
        float per =
            (float)(shimUpdated - stateUpdated) / getContract()->stateTime;
        uint8_t duty = per_to_duty(per, getContract()->state == LIGHT_ON);
        sigmaDeltaWrite(0, duty);
        shimUpdated = millis();
    }
}

void Led::setLed(LedState state) {
#ifdef DEBUG_LEDS
    DEBUG.printf("[led#%d] setState(%d)", pin, state);
    DEBUG.println();
#endif
    this->state = state;
    if (shim) {
        sigmaDeltaWrite(0, state == LIGHT_ON ? 255 : 30);
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