#include "Modules/Button.h"

namespace Modules {

Button::Button(uint8_t pin): Module(), hold_(0), update_(0),  pressed_(0), last_(BTN_PRESSED) {
    pinMode(pin, INPUT);
};

ButtonState Button::readState() { 
    ButtonState res  = digitalRead(POWER_BTN_PIN) ? BTN_RELEASED : BTN_PRESSED; 
    return res;
}

void Button::onLoop() {
    unsigned long now = millis();
    if (millis_passed(update_, now) < 50) return;
    update_ = now;    
    ButtonState state = readState();
    switch (state) {
        case BTN_PRESSED:
            if (last_ == BTN_RELEASED)
                pressed_ = now;
            if (last_ == BTN_PRESSED) {
                time_t holdTime = millis_passed(pressed_, now) / ONE_SECOND_ms;
                if (holdTime > hold_) {
                    holdEvent(holdTime);
                    hold_ = holdTime;
                }
            }
            break;
        case BTN_RELEASED:
            if (last_ == BTN_PRESSED) {
                if (hold_)
                    holdReleaseEvent(hold_);
                else
                    clickEvent();
                hold_ = 0;
                pressed_ = 0;
            }
            break;
    }
    last_ = state;
}

void Button::setOnClick(ButtonClickEventHandler handler) { clickEventHandler = handler; }

void Button::clickEvent() {
    if (clickEventHandler) {
        clickEventHandler();
    }
}

void Button::setOnHold(ButtonHoldEventHandler handler) { holdEventHandler = handler; }

void Button::holdEvent(time_t time) {
    if (holdEventHandler) {
        holdEventHandler(time);
    }
}

void Button::setOnRelease(ButtonHoldReleaseEventHandler handler) { holdReleaseEventHandler = handler; }

void Button::holdReleaseEvent(time_t time) {
    if (holdReleaseEventHandler) {
        holdReleaseEventHandler(time);
    }
}

}