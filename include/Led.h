#pragma once

#include <Arduino.h>

#include "BuildConfig.h"
#include "Consts.h"
#include "TimeUtils.h"

namespace Led {

enum State { LIGHT_OFF = HIGH, LIGHT_ON = LOW };
enum Mode { OFF, ON, BLINK, BLINK_ONE, BLINK_TWO, BLINK_ERROR };

#define LED_PWM_UPDATE_INTERVAL_ms 50
#define LED_PWM_FREQ 500
#define LED_PWM_DUTY_ON 0
#define LED_PWM_DUTY_OFF 80
#define LED_PWM_RANGE 100

struct Contract {
   public:
    State state;
    unsigned long stateTime;
    Contract() : state(LIGHT_OFF), stateTime(0) {}
    Contract(State state, unsigned long time)
        : state(state), stateTime(time) {}
};

class Led {
   public:
    Led(uint8_t pin, bool lightOn = false, bool smooth = false);
    void set(Mode mode);
    void loop();
   private:
    void smoothTransition();
    void updateState(State state);
    Contract *getContract();
    void next();
    bool smooth;
    uint8_t pin;
    Mode mode;
    State state;
    unsigned long stateUpdated = 0, pwmUpdated = 0;
    size_t step = 0;
    size_t size = 1;
    Contract contract[4];
};

}