#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "BuildConfig.h"
#include "Plot.h"
#include "StrUtils.h"
#include "Strings.h"
#include "TimeUtils.h"

// address are 0x27 or 0x3f
#define LCD_SLAVE_ADDRESS 0x3f
#define LCD_SLAVE_ADDRESS_ALT 0x27
#define LCD_SCROLL_INTERVAL 5000
#define LCD_UPDATE_INTERVAL 250
#define LCD_COLS 16
#define LCD_ROWS 2
#define LCD_ROW_1 0
#define LCD_ROW_2 1

enum Screen {
    SCREEN_CLEAR,
    SCREEN_BOOT,
    SCREEN_WIFI_OFF,
    SCREEN_WIFI_STATUS,
    SCREEN_WIFI_AP,
    SCREEN_WIFI_STA,
    SCREEN_AP_STA,
    SCREEN_PVI
};
enum CharBank {
    BANK_NONE,
    BANK_PROGRESS,
    BANK_PROGRESS_START,
    BANK_PROGRESS_1_TO_4,
    BANK_PROGRESS_4_TO_7,
    BANK_PROGRESS_END,
    BANK_PLOT
};

struct ScreenItem {
    bool redrawLabel = false;
    char label[LCD_COLS + 1] = {0};
    bool redrawText = false;
    char text[DISPLAY_VIRTUAL_COLS + 1] = {0};
    uint8_t text_pos = 0;
    bool needsRedraw() { return redrawLabel || redrawText; }
    void forceRedraw() {
        redrawLabel = true;
        redrawText = true;
    }
};

class Display {
   public:
    Display();
    bool init();
    bool ready();
    void loop();
    void setOutput(Print *p);
    void backlightOn();
    void backlightOff();
    void drawTextLeft(uint8_t row, const char *str);
    void drawTextRight(uint8_t row, const char *str);
    void drawTextCenter(uint8_t row, const char *str);
    void drawProgressBar(uint8_t row, uint8_t per);
    void drawPlot(uint8_t col_start);
    void drawFloat(uint8_t col, uint8_t row, float value);
    void drawText(uint8_t col, uint8_t row, const char *str);
    void turnOn();
    void turnOff();
    void clear();
    Screen getScreen();
    void setScreen(Screen screen, size_t items_count);
    void addScreenItem(uint8_t n, const char *text);
    void addScreenItem(uint8_t n, const char *label, const char *text);
    void lock(unsigned long period);
    void unlock();
    bool locked();
    PlotData *getData();

   private:
    bool connect();
    bool locked(unsigned long now);
    void loadBank(CharBank bank, bool force = false);
    void drawScreenItem(uint8_t row, ScreenItem *l);
    uint8_t getRowForUpdate();
    ScreenItem *getItemForRow(uint8_t row);
    void scrollDown();

    uint8_t addr;
    bool connected;
    bool active;
    CharBank bank;
    bool backlight;
    unsigned long lockTimeout;
    unsigned long lockUpdated;
    unsigned long lastUpdated;
    unsigned long lastScroll;
    LiquidCrystal_I2C *lcd;
    Print *output;
    PlotData data;

    Screen screen;
    ScreenItem items[DISPLAY_VIRTUAL_ROWS];
    uint8_t item_pos = 0;
    uint8_t items_size = 0;
};