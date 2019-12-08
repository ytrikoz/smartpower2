#include "Modules/Leds.h"

#define LED_PWM_FREQ 500
#define LED_PWM_RANGE 100

bool LedMod::onInit() {
    analogWriteFreq(LED_PWM_FREQ);
    analogWriteRange(LED_PWM_RANGE);
    power = new LedBlinker(POWER_LED_PIN, true, true);
    wifi = new LedBlinker(WIFI_LED_PIN, false, true);
    return true;
}

size_t LedMod::onDiag(Print *p) {
    size_t n = p->print(StrUtils::getStrP(str_power));
    n += p->print(':');
    n += p->print(' ');
    n += power->onDiag(p);
    n += p->print(StrUtils::getStrP(str_wifi));
    n += p->print(':');
    n += p->print(' ');
    n += wifi->onDiag(p);

    return n;
}

void LedMod::onLoop() {
    wifi->loop();
    power->loop();
}

void LedMod::set(LedLamp led, LedMode mode) {
    switch (led) {
    case POWER_LED:
        power->set(mode);
        break;
    case WIFI_LED:
        wifi->set(mode);
        break;
    default:
        break;
    }
}
