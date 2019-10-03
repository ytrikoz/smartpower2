#include "Modules/Leds.h"

#define LED_PWM_FREQ 500
#define LED_PWM_RANGE 100

namespace Led {

Leds::Leds() : AppModule(MOD_LED) {
    analogWriteFreq(LED_PWM_FREQ);
    analogWriteRange(LED_PWM_RANGE);
    power = new Led::LedBlinker(POWER_LED_PIN, true, true);
    wifi = new Led::LedBlinker(WIFI_LED_PIN, false, true);
};

size_t Leds::printDiag(Print *p) {
    size_t n = p->print(StrUtils::getStrP(str_power));
    n += p->print(':');
    n += p->print(' ');
    n += power->printDiag(p);
    n += p->print(StrUtils::getStrP(str_wifi));
    n += p->print(':');
    n += p->print(' ');
    n += wifi->printDiag(p);

    return n;
}

void Leds::loop() {
    wifi->loop();
    power->loop();
}

void Leds::set(Led::LedLamp led, Led::LedMode mode) {
    switch (led) {
    case Led::POWER_LED:
        power->set(mode);
        break;
    case Led::WIFI_LED:
        wifi->set(mode);
        break;
    default:
        break;
    }
}

}