#pragma once

#include "AppModule.h"
#include "LcdDisplay.h"
#include "Screen.h"

class Display : public AppModule {
  public:
    Display();
    bool begin();
    void loop();
    void setConfig(Config *config);

  public:
    void showProgress(uint8_t per, const char *str);
    void showPlot(PlotData *data, size_t cols);
    void showMessage(const char *header, const char *message);
    void clear(void);
    void refresh(void);
    void updateScreen(void);
    void enableBacklight(bool value = true);

    void load_message(Screen *obj, const char *header, const char *message);
    void load_wifi_sta(Screen *obj);
    void load_wifi_ap(Screen *obj);
    void load_wifi_ap_sta(Screen *obj);
    void load_ready(Screen *obj);
    void load_psu_info(Screen *obj);
    void load_psu_stat(Screen *obj);

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
    bool backlight;
    unsigned long lockUpdated, lockTimeout;
    unsigned long screenUpdated;
    unsigned long screenRedraw;
    unsigned long lastUpdated;
    unsigned long lastScroll;
    Screen screen;
    ScreenEnum activeScreen;
};
