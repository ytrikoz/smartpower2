#pragma once

#include <Arduino.h>

#include "AppModule.h"
#include "LedBlinker.h"

#define LED_PWM_DUTY_ON 0

namespace Led {

class Leds : public AppModule {
  public:
    void set(LedLamp, LedMode);

  public:
    Leds();
    void loop();
    size_t printDiag(Print *p);

  private:
    LedBlinker *wifi, *power;
};

} // namespace Led