#pragma once

#include "Core/Module.h"

enum ButtonState { BTN_PRESSED,
                   BTN_RELEASED };

typedef std::function<void(void)> ButtonClickEventHandler;
typedef std::function<void(time_t)> ButtonHoldEventHandler;
typedef std::function<void(time_t)> ButtonHoldReleaseEventHandler;

namespace Modules {

class Button : public Module {
   public:
    Button(uint8_t pin);

   public:
    void setOnClick(ButtonClickEventHandler handler);
    void setOnHold(ButtonHoldEventHandler handler);
    void setholdReleaseEvent(ButtonHoldReleaseEventHandler handler);

   public:
    void onDiag(const JsonObject& doc) override;

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

    ButtonState last_;
    unsigned long update_;
    unsigned long pressed_;
    unsigned long hold_;
    unsigned long prevHold_;
};

}  // namespace Modules