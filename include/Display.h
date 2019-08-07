#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "consts.h"
#include "debug.h"
#include "str_utils.h"
#include "time_utils.h"

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

struct PlotData {
    float blocks[PLOT_COLS] = {0};
    float minValue = 32000;
    float maxValue = -32000;
};

class Display {
   public:
    Display();
    bool init();
    void setOutput(Print *p);
    void drawBar(uint8_t row, uint8_t per);
    void drawPlot(uint8_t start_pos);
    bool isConnected();
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

    void loadData(float *data, size_t size);
    void drawPlot(float min_value, float max_value, uint8_t start_pos);

    void setItem(uint8_t row, const char *str);
    void setItem(uint8_t row, const char *param, const char *value);
    void redraw(boolean forced = false);

   private:
    bool getAddr();

    Print *output;
    uint8_t addr;
    LiquidCrystal_I2C *lcd;

    PlotData *data = new PlotData();
    DisplayItem item[LCD_ROWS];
    unsigned long lastUpdated;
    bool connected;
};

static uint8_t char_empty[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                                0b00000, 0b00000, 0b00000, 0b00000};

static uint8_t char_solid[8] = {0b11111, 0b11111, 0b11111, 0b11111,
                                0b11111, 0b11111, 0b11111, 0b11111};

static uint8_t char_1_5[8] = {0b10000, 0b10000, 0b10000, 0b10000,
                              0b10000, 0b10000, 0b10000, 0b10000};

static uint8_t char_2_5[8] = {0b11000, 0b11000, 0b11000, 0b11000,
                              0b11000, 0b11000, 0b11000, 0b11000};

static uint8_t char_3_5[8] = {0b11100, 0b11100, 0b11100, 0b11100,
                              0b11100, 0b11100, 0b11100, 0b11100};

static uint8_t char_4_5[8] = {0b11110, 0b11110, 0b11110, 0b11110,
                              0b11110, 0b11110, 0b11110, 0b11110};

static uint8_t char_1_8[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                              0b00000, 0b00000, 0b00000, 0b11111};

static uint8_t char_2_8[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                              0b00000, 0b00000, 0b11111, 0b11111};

static uint8_t char_3_8[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                              0b00000, 0b11111, 0b11111, 0b11111};

static uint8_t char_4_8[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                              0b11111, 0b11111, 0b11111, 0b11111};

static uint8_t char_5_8[8] = {0b00000, 0b00000, 0b00000, 0b11111,
                              0b11111, 0b11111, 0b11111, 0b11111};

static uint8_t char_6_8[8] = {0b00000, 0b00000, 0b11111, 0b11111,
                              0b11111, 0b11111, 0b11111, 0b11111};

static uint8_t char_7_8[8] = {0b00000, 0b11111, 0b11111, 0b11111,
                              0b11111, 0b11111, 0b11111, 0b11111};

static uint8_t char_bell[8] = {0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4};

static uint8_t char_note[8] = {0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0};

static uint8_t char_clock[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};

static uint8_t char_heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};

static uint8_t char_duck[8] = {0x0, 0xc, 0x1d, 0xf, 0xf, 0x6, 0x0};

static uint8_t char_check[8] = {0x0, 0x1, 0x3, 0x16, 0x1c, 0x8, 0x0};

static uint8_t _cross[8] = {0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};

static uint8_t _retarrow[8] = {0x1, 0x1, 0x5, 0x9, 0x1f, 0x8, 0x4};