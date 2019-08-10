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

enum GraphType { HORIZONTAL_BAR, VERTICAL_PLOT };

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
    float values[PLOT_COLS] = {0};
    float min_value = 32000;
    float max_value = -32000;
};

class Display {
   public:
    Display();
    bool init();
    bool ready();
    void setOutput(Print *p);
    void enableUpdates(bool enable);
    void drawTextCenter(uint8_t row, const char *str);
    void drawBar(uint8_t row, uint8_t per);
    void drawPlot(uint8_t start);
    void turnOn();
    void turnOff();
    void initGraph(GraphType type) {
        switch (type) {
            case HORIZONTAL_BAR:
                load_bar_bank();
                break;
            case VERTICAL_PLOT:
                load_plot_bank();
                break;
            default:
                break;
        }
    }

    void load_bar_bank();
    void load_plot_bank();

    void loadData(float *values, size_t size);
    void drawPlot(float min_value, float max_value, uint8_t offset_x);

    void setLine(uint8_t n, const char *str);
    void setLine(uint8_t n, const char *fixed_str, const char *var_str);

    void updateLCD(boolean forced = false);
    void lockUpdates(unsigned long period);
   private:  
    bool connect();
    uint8_t addr;
    bool connected;
    bool active;
    uint8_t get_row_for_update();
    uint8_t row_for_update;
    unsigned long lockTime;
    unsigned long lockUpdated;
    LiquidCrystal_I2C *lcd;
    Print *output;
    PlotData *plot = new PlotData();

    TextItem line[DISPLAY_VIRTUAL_ROWS];
    unsigned long updated;
};