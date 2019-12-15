#include "Modules/Led.h"

namespace Modules {

Led::Led() : Module() {
    analogWriteRange(PWM_DUTY_OFF - PWM_DUTY_ON);
    analogWriteFreq(PWM_FREQ);
    led_[RED_LED] = new LedBlinker(RED_LED_PIN, PWM_DUTY_OFF, PWM_DUTY_ON, true);
    led_[RED_LED]->set(LIGHT_ON, true);
    led_[BLUE_LED] = new LedBlinker(BLUE_LED_PIN, PWM_DUTY_OFF, PWM_DUTY_ON, true);
    led_[BLUE_LED]->set(LIGHT_OFF, true);
}

void Led::config(LedEnum led, LedParamEnum param, int value) {
    LedBlinker* obj = getLed(led);
    if (obj) {
        switch (param) {
            case DUTY_OFF:
                obj->setDutyOff(value);
            case DUTY_ON:
                obj->setDutyOn(value);
            default:
                return;
        }
    }
}

void Led::onLoop() {
    getLed(RED_LED)->loop();
    getLed(BLUE_LED)->loop();
}

void Led::set(LedEnum led, LedSignal mode) {
    LedBlinker* obj = getLed(led);
    if (obj) obj->set(mode, true);
}

LedBlinker* Led::getLed(const LedEnum led) const {
    return led_[led];
}

LedBlinker::LedBlinker(uint8_t pin, uint8_t dutyOff, uint8_t dutyOn, bool smooth) : pin_(pin), updated_(0), dutyOff_(dutyOff), dutyOn_(dutyOn), smooth_(smooth) {
    pinMode(pin, OUTPUT);
}

void LedBlinker::setDutyOff(uint8_t value) {
    dutyOff_ = value;
    applyState(0);
}

void LedBlinker::setDutyOn(uint8_t value) {
    dutyOn_ = value;
    applyState(0);
}

uint16_t LedBlinker::map2duty(const float k) const {
    return dutyOff_ - (float)(dutyOff_ - dutyOn_) * k;
}

void LedBlinker::set(LedSignal mode, bool forced) {
    if (!forced && mode_ == mode)
        return;
    switch (mode) {
        case LIGHT_OFF:
            pattern_[0] = {FULL_OFF, INF_TIME};
            size_ = 1;
            break;
        case LIGHT_ON:
            pattern_[0] = {FULL_ON, INF_TIME};
            size_ = 1;
            break;
        case BLINK:
            pattern_[0] = {FULL_ON, 250};
            pattern_[1] = {FULL_OFF, 500};
            size_ = 2;
            break;
        case BLINK_ALERT:
            pattern_[0] = {FULL_ON, 1000};
            pattern_[1] = {FULL_OFF, 100};
            pattern_[2] = {FULL_ON, 100};
            pattern_[3] = {FULL_OFF, 100};
            size_ = 4;
            break;
        case BLINK_ERROR:
            pattern_[0] = {FULL_ON, 100};
            pattern_[1] = {FULL_OFF, 50};
            pattern_[2] = {FULL_ON, 100};
            pattern_[3] = {FULL_OFF, 50};
            size_ = 4;
            break;
    }
    mode_ = mode;
    step_ = 0;
    updated_ = millis();
    transition_ = 0;
    if (forced) applyState(transition_);
}

void LedBlinker::loop() {
    unsigned long duration = pattern_[step_].time;
    if (duration == INF_TIME) return;

    unsigned long now = millis();
    unsigned long passed = millis_passed(updated_, now);
    if (passed >= duration) {
        updated_ = now;
        if (++step_ >= size_ - 1) step_ = 0;
        return;
    }
    transition_ = (float)passed / duration;
    applyState(transition_);
}

void LedBlinker::applyState(float k) {
    float f;
    if (pattern_[step_].state == FULL_ON)
        f = 1 - k;
    else
        f = k;

    analogWrite(pin_, map2duty(f));
}

size_t LedBlinker::diag(Print *p) {
    DynamicJsonDocument doc(256);
    doc[FPSTR(str_mode)] = (uint8_t)mode_;
    doc[FPSTR(str_state)] = transition_;
    doc[FPSTR(str_complete)] = String(step_ + 1) + '/' + String(size_);
    return serializeJson(doc, *p);
}

}  // namespace Modules