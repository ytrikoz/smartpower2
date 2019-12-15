#pragma once

#include <Arduino.h>

#include "Module.h"
#include "LedBlinker.h"

#define RED_LED_PIN D1
#define BLUE_LED_PIN D4

#define PWM_DUTY_OFF 100
#define PWM_DUTY_ON 0
#define PWM_FREQ 500

#define INF_TIME 0
#define FULL_OFF 0
#define FULL_ON 1

enum LedEnum { RED_LED,
               BLUE_LED };

enum LedSignal { LIGHT_OFF,
                 LIGHT_ON,
                 BLINK,
                 BLINK_ALERT,
                 BLINK_ERROR };

enum LedParamEnum {
    DUTY_OFF,
    DUTY_ON,
};

struct SignalStep {
    float state;
    unsigned long time;
};

namespace Modules {

class LedBlinker;

class Led : public Module {
   public:
    Led();
    void set(LedEnum, LedSignal);
    void config(LedEnum led, LedParamEnum param, int value);
    LedBlinker *getLed(const LedEnum led) const;
   protected:
    void onLoop() override;

   private:
    void updatePwmFreq();

   private:
    LedBlinker *led_[2];
};

class LedBlinker {
   public:
    LedBlinker(uint8_t pin, uint8_t dutyOff, uint8_t dutyOn, bool smooth);
    void set(LedSignal mode, bool forced = false);
    void loop();
    size_t diag(Print *p);
    void setDutyOff(uint8_t value);
    void setDutyOn(uint8_t value);    
   private:
    void applyState(float state);

   private:
    uint16_t map2duty(const float k) const;
    uint8_t pin_;
    unsigned long updated_;

    uint8_t dutyOff_;
    uint8_t dutyOn_;

    bool smooth_;
    LedSignal mode_;
    float transition_;

    SignalStep pattern_[4];
    uint8_t step_;
    uint8_t size_;
};

}  // namespace Modules