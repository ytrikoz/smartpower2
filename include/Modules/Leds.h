#pragma once

#include <Arduino.h>

#include "AppModule.h"
#include "LedBlinker.h"

#define LED_PWM_DUTY_ON 0

class LedMod : public AppModule {
   public:
    LedMod() : AppModule(MOD_LED){};
    void set(LedLamp, LedMode);

   protected:
    bool onInit() override;
    void onLoop() override;
    size_t onDiag(Print *p) override;

   private:
    LedBlinker *wifi, *power;
};