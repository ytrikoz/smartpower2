#include "Modules/Button.h"

bool Button::onInit() {
    pinMode(POWER_BTN_PIN, INPUT);
    lastUpdated = 0;
    return true;
}

void Button::onLoop() {
    unsigned long now = millis();
    handleButton(now);
    if (clickFlag) {
        onClicked();
        pressTime = 0;
        clickFlag = false;
        return;
    }
    unsigned long passed = millis_passed(lastUpdated, now);
    if (passed >= ONE_SECOND_ms) {
        lastUpdated = now;
        if (state == BTN_PRESSED) {
            onHold(holdTime += passed);
            return;
        }
        if (state == BTN_RELEASED && holdTime > 0) {
            onHoldRelease(holdTime);
            holdTime = 0;
        }
    }
}

void Button::setOnClicked(ButtonCLickHandler h) { clickHandler = h; }

void Button::setOnHold(ButtonHoldHandler h) { holdHandler = h; }

void Button::setOnHoldRelease(ButtonHoldHandler h) { holdReleaseHandler = h; }

void Button::onClicked() {
    if (clickHandler)
        clickHandler();
}

void Button::onHold(unsigned long time) {
    if (holdHandler)
        holdHandler(time);
}

void Button::onHoldRelease(unsigned long time) {
    if (holdReleaseHandler)
        holdReleaseHandler(time);
}

void Button::handleButton(unsigned long now) {
    if (millis_passed(lastEvent, now) < 50)
        return;
    lastEvent = now;

    if (isPressed() && state != BTN_PRESSED) {
        state = BTN_PRESSED;
        pressTime = now;
        return;
    }

    if (isReleased() && state != BTN_RELEASED) {
        if (millis_passed(pressTime, now) < ONE_SECOND_ms)
            clickFlag = true;
        state = BTN_RELEASED;
    }
}

bool Button::isPressed() { return !digitalRead(POWER_BTN_PIN); }

bool Button::isReleased() { return digitalRead(POWER_BTN_PIN); }