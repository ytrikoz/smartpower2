#pragma once

#include <Arduino.h>

#include "AppModule.h"

enum ButtonState { BTN_PRESSED, BTN_RELEASED };

typedef std::function<void(void)> ButtonCLickHandler;
typedef std::function<void(unsigned long)> ButtonHoldHandler;
typedef std::function<void(unsigned long)> ButtonHoldReleaseHandler;

class Button : public AppModule {
  public:
    void loop();

  public:
    Button();
    void setOnClicked(ButtonCLickHandler h);
    void setOnHold(ButtonHoldHandler h);
    void setOnHoldRelease(ButtonHoldHandler h);

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

    bool clickFlag = false;
    unsigned long holdTime = 0;
    unsigned long lastEvent = 0;
    unsigned long pressTime = 0;
    ButtonState state = BTN_RELEASED;
    unsigned long lastUpdated = 0;
};
