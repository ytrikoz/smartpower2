#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "BuildConfig.h"
#include "Core/Module.h"
#include "Utils/StrUtils.h"
#include "Utils/TimeUtils.h"
#include "Plot.h"
#include "Screen.h"
#include "Strings.h"

enum CharBank {
    BANK_NONE,
    BANK_PROGRESS,
    BANK_PROGRESS_START,
    BANK_PROGRESS_1_TO_4,
    BANK_PROGRESS_4_TO_7,
    BANK_PROGRESS_END,
    BANK_PLOT
};

class LcdDisplay {
   public:
    LcdDisplay();
    bool begin();
    void loop();

   public:
    bool isEnabled();
    void loadBank(CharBank bank, bool force = false);
    bool connect();
    void turnOn();
    void turnOff();
    void clear();

    void drawScreen(Screen *screen, bool force = false);
    void drawTextLeft(uint8_t row, const char *str);
    void drawTextRight(uint8_t row, const char *str);
    void drawTextCenter(uint8_t row, const char *str);
    void drawProgressBar(uint8_t row, uint8_t per);
    void drawPlot(PlotData *data, size_t col_start);
    void drawFloat(uint8_t col, uint8_t row, float value);
    void drawText(uint8_t col, uint8_t row, const char *str);

   private:
    void drawScreenItem(uint8_t row, ScreenItem *l);

   private:
    uint8_t addr;
    LiquidCrystal_I2C *lcd;
    bool active;
    CharBank bank;
};