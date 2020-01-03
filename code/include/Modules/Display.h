#pragma once

#include "Core/Module.h"

#include "Hardware/LcdDisplay.h"

namespace Modules {

class Display : public Module {
   public:
    void showProgress(uint8_t per, const char *str);
    void showPlot(PlotSummary *data, size_t cols);
    void showMessage(const char *header, const char *message);
    void clear(void);
    void updateScreen(void);
    bool enableBacklight(const bool value = true);
    
    bool connected();

    void show_psu_data(const PsuData& data);
    void show_psu_stat();
    void show_message(const char *header, const char *message);
    void show_network(const NetworkMode mode);

   public:
    Display() : Module(){};

   protected:
    bool onInit() override;
    bool onConfigChange(const ConfigItem param, const String &value) override;
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
}  // namespace Modules