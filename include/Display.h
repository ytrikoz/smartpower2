#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "BuildConfig.h"
#include "Strings.h"
#include "StrUtils.h"
#include "TimeUtils.h"
#include "Plot.h"

// slave address are 0x27 or 0x3f
#define LCD_SLAVE_ADDRESS 0x3f
#define LCD_SLAVE_ADDRESS_ALT 0x27
#define LCD_REFRESH_INTERVAL_ms 250
#define LCD_COLS 16
#define LCD_ROWS 2
#define LCD_ROW_1 0
#define LCD_ROW_2 1


enum Screen { SCREEN_CLEAR, SCREEN_BOOT, SCREEN_WIFI_OFF, SCREEN_WIFI_STATUS, SCREEN_WIFI_AP, SCREEN_WIFI_STA, SCREEN_AP_STA, SCREEN_PVI };
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

class Display {
   public:
    Display();
    bool init();
    bool ready();
    void loop();
    void setOutput(Print *p);
    void drawTextCenter(uint8_t row, const char *str);
    void drawBar(uint8_t row, uint8_t per);
    void drawPlot(uint8_t col_start);
    void drawFloat(uint8_t col, uint8_t row, float value);
    void drawText(uint8_t col, uint8_t row, const char* str);
    void printPlot(Print* p);
    void disableBacklight();
    void enableBacklight();
    void turnOn();
    void turnOff();
    void loadBank(CharBank bank, bool force = false);
    void clear();
    void scrollDown();
    Screen getScreen();
    void setScreen(Screen screen, size_t items_count);

    PlotData* getData();
    
    void addTextItem(uint8_t n, const char *str);
    void addTextItem(uint8_t n, const char *fixed_str, const char *var_str);

    void lock(unsigned long period);
    void lock();
    void unlock();
    bool locked();
   private:  
    void drawTextItem(uint8_t row, TextItem *l);
    uint8_t getRowForUpdate();
    TextItem* getItemForRow(uint8_t row);
    bool connect();

    uint8_t addr;
    bool connected;
    bool active;
    CharBank bank;
    bool backlight;
    uint8_t updatedRow;
    unsigned long lockTimeout;
    unsigned long lockUpdated;
    Screen screen;

    LiquidCrystal_I2C *lcd;
    Print *output;
    
    PlotData data;     

    TextItem item[DISPLAY_VIRTUAL_ROWS];
    
    size_t items_count = 0;    
    size_t cur_item = 0;
    unsigned long refreshed;
};