#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>
#include <LiquidCrystal_I2C.h>

#include "consts.h"
#include "debug.h"
#include "str_utils.h"

// slave address are 0x27 or 0x3f
#define LCD_SLAVE_ADDRESS 0x3f
#define LCD_SLAVE_ADDRESS_ALT 0x27
#define LCD_REFRESH_INTERVAL_ms 250
#define LCD_COLS 16
#define LCD_ROWS 2
#define LCD_ROW_1 0
#define LCD_ROW_2 1

enum GraphType { HORIZONTAL_BAR, VERTICAL_PLOT };

struct DisplayItem {
    bool updated = false;
    // fixed part
    char param[LCD_COLS + 1] = {0};
    // scrolling part
    char value[DISPLAY_VIRTUAL_COLS + 1] = {0};
    // active virtual screen
    uint8_t screen_X = 1;
    uint8_t screen_Y = 1;
    // scrolling inline position
    uint8_t index;
};

class Display {
   public:
    Display();
    bool init();
    void setOutput(Print *p);
    void drawBar(uint8_t row, uint8_t per);
    bool isConnected();
    void turnOn();
    void turnOff();
    void initGraph(GraphType type) {
        switch (type) {
            case HORIZONTAL_BAR:
                initHorizontalBar();
                break;
            case VERTICAL_PLOT:
                initVerticalPlot();
                break;
            default:
                break;
        }
    }
    void initHorizontalBar();
    void initVerticalPlot();
    void drawPlot(float *data, int data_size);
    void drawPlot(float *plot, float min_value, float max_value,
                  uint8_t start_pos);
    void setItem(uint8_t row, const char *str);
    void setItem(uint8_t row, const char *param, const char *value);
    void redraw(boolean forced = false);
   private:
    bool getAddr();


    Print *output;
    uint8_t addr;
    LiquidCrystal_I2C *lcd;
    DisplayItem item[LCD_ROWS];
    unsigned long lastUpdated;
    bool connected;
};
