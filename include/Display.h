#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>

#include <LiquidCrystal_I2C.h>

#include "consts.h"
#include "str_utils.h"

#define LCD_ROWS 2
#define LCD_COLS 16
#define LCD_REFRESH_INTERVAL_ms 250
typedef struct {
    bool updated = false;
    // fixed str part
    char param[LCD_COLS + 1] = {0};
    // scrolling str part
    char value[DISPLAY_VIRTUAL_COLS + 1] = {0};
    // active virtual screen
    uint8_t screen_X = 1;
    uint8_t screen_Y = 1;
    // scrolling inline position
    uint8_t index;
} DisplayItem;

class Display {
   public:
    Display();
    bool init();
    void setOutput(Print *p);
    bool isConnected();
    void turnOn();
    void turnOff();

    void onProgress(uint8_t per, const char *message = NULL);

    void setItem(uint8_t row, const char *str);
    void setItem(uint8_t row, const char *param, const char *value);
    const char *getParamStr(uint8_t row);

    void redraw(boolean forced = false);

   private:
    Print *output;
    uint8_t addr;
    LiquidCrystal_I2C *lcd;
    DisplayItem item[LCD_ROWS];
    unsigned long lastUpdated;
    bool connected;
    bool getAddr();
};
