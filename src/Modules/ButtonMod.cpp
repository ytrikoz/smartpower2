#include "Modules/ButtonMod.h"

bool ButtonMod::onInit() {
    pinMode(POWER_BTN_PIN, INPUT);

    holdTime_ = lastUpdate_ = pressedTime_ = 0;
    lastState_ = BTN_RELEASED;
    
    return true;
}

void ButtonMod::onLoop() {
    unsigned long now = millis();
    if (millis_passed(lastUpdate_, now) < 50) return;
    lastUpdate_ = now;    
    ButtonState state = readState();
    switch (state) {
        case BTN_PRESSED:
            if (isReleased(lastState_))
                pressedTime_ = now;

            if (isPressed(lastState_)) {
                time_t holdTime = millis_passed(pressedTime_, now) / ONE_SECOND_ms;
                if (holdTime > holdTime_) {
                    holdEvent(holdTime);
                    holdTime_ = holdTime;
                }
            }
            break;
        case BTN_RELEASED:
            if (isPressed(lastState_)) {
                if (holdTime_)
                    holdReleaseEvent(holdTime_);
                else
                    clickEvent();
                holdTime_ = 0;
                pressedTime_ = 0;
            }
            break;
    }
    lastState_ = state;
}

void ButtonMod::setOnClick(ButtonClickEventHandler handler) { clickEventHandler = handler; }

void ButtonMod::setOnHold(ButtonHoldEventHandler handler) { holdEventHandler = handler; }

void ButtonMod::setOnRelease(ButtonHoldReleaseEventHandler handler) { holdReleaseEventHandler = handler; }

void ButtonMod::clickEvent() {
    if (clickEventHandler) clickEventHandler();
}

void ButtonMod::holdEvent(time_t time) {
    if (holdEventHandler) holdEventHandler(time);
}

void ButtonMod::holdReleaseEvent(time_t time) {
    if (holdReleaseEventHandler) holdReleaseEventHandler(time);
}

ButtonState ButtonMod::readState() { return digitalRead(POWER_BTN_PIN) ? BTN_RELEASED : BTN_PRESSED; }

bool ButtonMod::isPressed(ButtonState state) { return state == BTN_PRESSED; }

bool ButtonMod::isReleased(ButtonState state) { return state == BTN_RELEASED; }