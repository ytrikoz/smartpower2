#pragma once

#include "AppModule.h"

enum ButtonState { BTN_PRESSED,
                   BTN_RELEASED };

typedef std::function<void(void)> ButtonClickEventHandler;
typedef std::function<void(time_t)> ButtonHoldEventHandler;
typedef std::function<void(time_t)> ButtonHoldReleaseEventHandler;

class ButtonMod : public AppModule {
   public:
    void setOnClick(ButtonClickEventHandler handler);
    void setOnHold(ButtonHoldEventHandler handler);
    void setOnRelease(ButtonHoldReleaseEventHandler handler);

   public:
    ButtonMod() : AppModule(MOD_BTN) {};

   protected:
    bool onInit() override;
    void onLoop() override;

   private:
    ButtonState readState();
    bool isPressed(ButtonState state);
    bool isReleased(ButtonState state);

    void clickEvent();
    void holdEvent(time_t);
    void holdReleaseEvent(time_t);

   private:
    ButtonClickEventHandler clickEventHandler;
    ButtonHoldEventHandler holdEventHandler;
    ButtonHoldReleaseEventHandler holdReleaseEventHandler;

    time_t holdTime_;
    unsigned long lastUpdate_;
    unsigned long pressedTime_;
    ButtonState lastState_;
};
