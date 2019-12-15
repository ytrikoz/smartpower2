#pragma once

#include "Module.h"

enum ButtonState { BTN_PRESSED,
                   BTN_RELEASED };

typedef std::function<void(void)> ButtonClickEventHandler;
typedef std::function<void(time_t)> ButtonHoldEventHandler;
typedef std::function<void(time_t)> ButtonHoldReleaseEventHandler;

namespace Modules {

class Button : public Module {
   public:
    void setOnClick(ButtonClickEventHandler handler);
    void setOnHold(ButtonHoldEventHandler handler);
    void setOnRelease(ButtonHoldReleaseEventHandler handler);

   public:
    Button(uint8_t pin);

   protected:
    void onLoop() override;

   private:
    ButtonState readState();
    void clickEvent();
    void holdEvent(time_t);
    void holdReleaseEvent(time_t);

   private:
    ButtonClickEventHandler clickEventHandler;
    ButtonHoldEventHandler holdEventHandler;
    ButtonHoldReleaseEventHandler holdReleaseEventHandler;

    time_t hold_;
    unsigned long update_;
    unsigned long pressed_;
    ButtonState last_;
};

}