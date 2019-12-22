#include "Modules/Button.h"

namespace Modules {

Button::Button(uint8_t pin) : Module(), last_(BTN_RELEASED), update_(0), pressed_(0), hold_(0), prevHold_(0) {
    pinMode(pin, INPUT);
};

ButtonState Button::readState() {
    ButtonState res = digitalRead(POWER_BTN_PIN) ? BTN_RELEASED : BTN_PRESSED;
    return res;
}

void Button::onLoop() {
    unsigned long now = millis();
    if (millis_passed(update_, now) < 100)
        return;
    update_ = now;
    ButtonState state = readState();
    switch (state) {
        case BTN_PRESSED:
            if (last_ == BTN_RELEASED) {
                pressed_ = now;
                prevHold_ = 0;
            }
            if (last_ == BTN_PRESSED) {
                hold_ = millis_passed(pressed_, now);
                if (hold_ > prevHold_ + 500) {
                    holdEvent(hold_ / ONE_SECOND_ms);
                    prevHold_ = hold_;
                }
            }
            break;
        case BTN_RELEASED:
            if (last_ == BTN_PRESSED) {
                if (hold_ < 500)
                    clickEvent();
                else
                    holdReleaseEvent(hold_ / ONE_SECOND_ms);
            }
            break;
    }
    last_ = state;
}

void Button::onDiag(const JsonObject& doc) {
    doc[FPSTR(str_time)] = hold_;
}

void Button::clickEvent() {
    if (clickEventHandler) 
        clickEventHandler();
}

void Button::holdEvent(time_t time) {
    if (holdEventHandler)
        holdEventHandler(time);
}

void Button::holdReleaseEvent(time_t time) {
    if (holdReleaseEventHandler)
        holdReleaseEventHandler(time);
}

void Button::setOnClick(ButtonClickEventHandler h) {
    clickEventHandler = h;
}

void Button::setOnHold(ButtonHoldEventHandler h) {
    holdEventHandler = h;
}

void Button::setholdReleaseEvent(ButtonHoldReleaseEventHandler h) {
    holdReleaseEventHandler = h;
}

}  // namespace Modules