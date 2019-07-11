#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>

#include <LiquidCrystal_I2C.h>

#include "consts.h"
#include "str_utils.h"

#define LCD_ROWS 2
#define LCD_COLS 16
#define REFRESH_INTERVAL 250

#define MAX_LABEL_LENGTH 4
#define FPS_ITER 1
#define DELAY_TIME 3500

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
   private:
    Print *output;    
    uint8_t addr;
    LiquidCrystal_I2C *lcd;
    DisplayItem item[LCD_ROWS];

    unsigned long lastUpdated;
    bool connected;
    
    bool getAddr();

    void showByteXfer(unsigned long etime);
    void showFPS(unsigned long etime, const char *type);
    unsigned long timeFPS(uint8_t iter, uint8_t cols, uint8_t rows);

    void show(const String &s);
    void _print(String arg);
    void _println(void);
    void _println(String arg);

   public:
    Display();
    void setOutput(Print *p);
    bool init();
    bool isConnected();
    void turnOn();
    void turnOff();

    void onProgress(uint8_t per, const char *message = NULL);

    void setItem(uint8_t row, const char *str);
    void setItem(uint8_t row, const char *param, const char *value);
    const char *getParamStr(uint8_t row);
    
    void redraw(boolean forced = false);
};
