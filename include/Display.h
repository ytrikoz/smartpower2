#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "BuildConfig.h"
#include "Strings.h"
#include "StrUtils.h"
#include "TimeUtils.h"

// slave address are 0x27 or 0x3f
#define LCD_SLAVE_ADDRESS 0x3f
#define LCD_SLAVE_ADDRESS_ALT 0x27
#define LCD_REFRESH_INTERVAL_ms 250
#define LCD_COLS 16
#define LCD_ROWS 2
#define LCD_ROW_1 0
#define LCD_ROW_2 1
#define PLOT_COLS 8
#define PLOT_ROWS 2

enum CharBank { BANK_NONE, BANK_BAR, BANK_PLOT };

struct TextItem {
    bool hasUpdates = false;
    char fixed_str[LCD_COLS + 1] = {0};
    char var_str[DISPLAY_VIRTUAL_COLS + 1] = {0};
    uint8_t var_pos = 0;
    // virtual screen
    uint8_t screen_X = 1;
    uint8_t screen_Y = 1;
};

struct PlotData {
    float cols[PLOT_COLS] = {0};
    float min_value = INT_MAX;
    float max_value = INT_MIN;
};

class Display {
   public:
    Display();
    bool init();
    bool ready();
    void loop();
    void setOutput(Print *p);
    void drawTextCenter(uint8_t row, const char *str);
    void drawBar(uint8_t row, uint8_t per);
    void drawPlot(uint8_t start);
    void disableBacklight();
    void enableBacklight();
    void turnOn();
    void turnOff();
    void loadBank(CharBank bank, bool force = false);
    void clear();
    void scrollDown();

    PlotData* getData();
    void drawPlot(uint8_t start_col, size_t cols);
    
    void setLines(size_t size);
    void setLine(uint8_t n, const char *str);
    void setLine(uint8_t n, const char *fixed_str, const char *var_str);

    void lock(unsigned long period);
    void lock();
    void unlock();
    bool locked();
   private:  
    void updateLCD(uint8_t row, TextItem *l, boolean forced = false);
    uint8_t get_row_for_update();
    bool updates_locked();
    bool connect();

    uint8_t addr;
    bool connected;
    bool active;
    CharBank bank;
    bool backlight;
    uint8_t row_for_update;
    unsigned long lockTimeLeft;
    unsigned long lockUpdated;

    LiquidCrystal_I2C *lcd;
    Print *output;
    
    PlotData data;     

    TextItem line[DISPLAY_VIRTUAL_ROWS];
    size_t lines = 0;
    unsigned long refreshed;
};