#pragma once

#include <Arduino.h>

#include "AppModule.h"

enum ButtonState { BTN_PRESSED,
                   BTN_RELEASED };

typedef std::function<void(void)> ButtonCLickHandler;
typedef std::function<void(unsigned long)> ButtonHoldHandler;
typedef std::function<void(unsigned long)> ButtonHoldReleaseHandler;

class Button : public AppModule {
   public:
    void setOnClicked(ButtonCLickHandler h);
    void setOnHold(ButtonHoldHandler h);
    void setOnHoldRelease(ButtonHoldHandler h);

   public:
    Button() : AppModule(MOD_BTN) {
        clickFlag = false;
        holdTime = lastEvent = pressTime = 0;
        state = BTN_RELEASED;
        lastUpdated = 0;
    };

   protected:
    bool onInit() override;
    void onLoop() override;

   private:
    bool isPressed();
    bool isReleased();
    void handleButton(unsigned long now);
    void onClicked();
    void onHold(unsigned long time);
    void onHoldRelease(unsigned long time);

   private:
    ButtonCLickHandler clickHandler;
    ButtonHoldHandler holdHandler;
    ButtonHoldReleaseHandler holdReleaseHandler;

    bool clickFlag;
    unsigned long holdTime;
    unsigned long lastEvent;
    unsigned long pressTime;
    ButtonState state = BTN_RELEASED;
    unsigned long lastUpdated;
};
