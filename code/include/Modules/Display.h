#pragma once

#include "Core/Module.h"

#include "Hardware/LcdDisplay.h"

namespace Modules {

class Display : public Module {
   public:
    bool isEnabled();
    void showProgress(uint8_t per, const char *str);
    void showPlot(PlotSummary *data, size_t cols);
    void showMessage(const char *header, const char *message);
    void clear(void);
    void refresh(void);
    void updateScreen(void);
    bool enableBacklight(const bool value = true, const time_t time = 0);

    void load_message(Screen *obj, String header, String message);
    void load_message(Screen *obj, const char *header, const char *message);
    void load_wifi_sta(Screen *obj);
    void load_wifi_ap(Screen *obj);
    void load_wifi_ap_sta(Screen *obj);
    void load_ready(Screen *obj);
    void load_psu_info(Screen *obj);
    void load_psu_stat(Screen *obj);

   public:
    Display() : Module(){};

   protected:
    bool onInit() override;
    bool onConfigChange(const ConfigItem param, const String& value) override;
    void onLoop() override;

   private:
    void setFirst();
    void refreshScreen();
    void setScreen(ScreenEnum value);
    void lock(unsigned long time);
    void unlock(void);
    bool locked(unsigned long now);
    bool locked(void);

   private:
    LcdDisplay *lcd;
    bool backlight_;
    time_t backlightTime_;
    unsigned long lockUpdated, lockTimeout;
    unsigned long screenUpdated;
    unsigned long screenRedraw;
    unsigned long lastUpdated;
    unsigned long lastScroll;
    Screen screen;
    ScreenEnum activeScreen;
};

}