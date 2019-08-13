#include "Display.h"

uint8_t char_empty[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                         0b00000, 0b00000, 0b00000, 0b00000};

uint8_t char_solid[8] = {0b11111, 0b11111, 0b11111, 0b11111,
                         0b11111, 0b11111, 0b11111, 0b11111};

uint8_t char_1_5[8] = {0b10000, 0b10000, 0b10000, 0b10000,
                       0b10000, 0b10000, 0b10000, 0b10000};

uint8_t char_2_5[8] = {0b11000, 0b11000, 0b11000, 0b11000,
                       0b11000, 0b11000, 0b11000, 0b11000};

uint8_t char_3_5[8] = {0b11100, 0b11100, 0b11100, 0b11100,
                       0b11100, 0b11100, 0b11100, 0b11100};

uint8_t char_4_5[8] = {0b11110, 0b11110, 0b11110, 0b11110,
                       0b11110, 0b11110, 0b11110, 0b11110};

uint8_t char_1_8[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                       0b00000, 0b00000, 0b00000, 0b11111};

uint8_t char_2_8[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                       0b00000, 0b00000, 0b11111, 0b11111};

uint8_t char_3_8[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                       0b00000, 0b11111, 0b11111, 0b11111};

uint8_t char_4_8[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                       0b11111, 0b11111, 0b11111, 0b11111};

uint8_t char_5_8[8] = {0b00000, 0b00000, 0b00000, 0b11111,
                       0b11111, 0b11111, 0b11111, 0b11111};

uint8_t char_6_8[8] = {0b00000, 0b00000, 0b11111, 0b11111,
                       0b11111, 0b11111, 0b11111, 0b11111};

uint8_t char_7_8[8] = {0b00000, 0b11111, 0b11111, 0b11111,
                       0b11111, 0b11111, 0b11111, 0b11111};

uint8_t char_bell[8] = {0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4};
uint8_t char_note[8] = {0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0};
uint8_t char_clock[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
uint8_t char_heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
uint8_t char_duck[8] = {0x0, 0xc, 0x1d, 0xf, 0xf, 0x6, 0x0};
uint8_t char_check[8] = {0x0, 0x1, 0x3, 0x16, 0x1c, 0x8, 0x0};
uint8_t char_cross[8] = {0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};
uint8_t char_retarrow[8] = {0x1, 0x1, 0x5, 0x9, 0x1f, 0x8, 0x4};

using StrUtils::setstr;
using StrUtils::strfill;
using StrUtils::strpadd;

Display::Display() {
    addr = 0x00;
    connected = false;
    refreshed = 0;
    lockTimeLeft = 0;
}

void Display::setOutput(Print *p) { output = p; }

bool Display::ready() { return connected; }

bool Display::init() {
    if (!connected) {
        output->print(FPSTR(str_lcd));
        if (connect()) {
            output->print(FPSTR(str_found));
            output->print("0x");
            output->println(addr, HEX);

            lcd = new LiquidCrystal_I2C(addr, LCD_COLS, LCD_ROWS);
            lcd->init();
            lcd->clear();

            memset(line, 0, sizeof(TextItem) * DISPLAY_VIRTUAL_ROWS);

            loadBank(BANK_NONE);
            backlight = true;

            connected = true;
        } else {
            output->println(FPSTR(str_not_found));
        }
    }
    return connected;
}

void Display::disableBacklight() {
    lcd->noBacklight();
    backlight = false;
}

void Display::enableBacklight() {
    lcd->backlight();
    backlight = true;
}
void Display::turnOn() {
    if (lcd) {
        lcd->display();
        if (backlight) lcd->backlight();
    }
}

void Display::turnOff() {
    if (lcd) {
        lcd->noDisplay();
        lcd->noBacklight();
    }
}

bool Display::connect() {
    addr = 0;
    Wire.beginTransmission(LCD_SLAVE_ADDRESS);
    if (!Wire.endTransmission()) {
        addr = LCD_SLAVE_ADDRESS;
    } else {
        Wire.beginTransmission(LCD_SLAVE_ADDRESS_ALT);
        if (!Wire.endTransmission()) addr = LCD_SLAVE_ADDRESS_ALT;
    }
    return (addr);
}

void Display::setLine(uint8_t row, const char *str) {
    setLine(row, nullptr, str);
}

void Display::setLine(uint8_t n, const char *fixed_str, const char *var_str) {
    TextItem *l = &line[n];
    bool updates = setstr(l->fixed_str, fixed_str, LCD_COLS + 1);
    updates |= setstr(l->var_str, var_str, DISPLAY_VIRTUAL_COLS + 1);
    if (updates) {
#ifdef DEBUG_DISPLAY
        DEBUG.printf("[display] setLine n: %d /%s_%s/", n, l->fixed_str,
                     l->var_str);
        DEBUG.println();
#endif
        l->var_pos = 1;
        l->screen_X = 1;
        l->screen_Y = 1;
        l->hasUpdates = true;
    }
}

uint8_t Display::get_row_for_update() {
    if (++row_for_update > LCD_ROWS) row_for_update = 0;
    return row_for_update;
}

void Display::unlock() {
    active = true;
    lockTimeLeft = 0;
}

void Display::lock() {
    active = false;
    lockTimeLeft = 0;
}

void Display::lock(unsigned long period) {
#ifdef DEBUG_DISPLAY
    DEBUG.printf("[display] lockUpdates %lu", period);
    DEBUG.println();
#endif
    lockTimeLeft = period;
    lockUpdated = millis();
}

bool Display::locked() {
    if (!active) return true;

    if (lockTimeLeft > 0) {
        unsigned long now = millis();
        unsigned long passed = millis_passed(lockUpdated, now);
        if (lockTimeLeft > passed) {
            lockTimeLeft -= passed;
        } else {
            lockTimeLeft = 0;
        }
        lockUpdated = now;
    }
    return (lockTimeLeft > 0);
}

void Display::loop() {
    if (!connected) return;
    if (millis_since(refreshed) > LCD_REFRESH_INTERVAL_ms) {
        if (!locked()) {
            for (size_t n = 0; n < LCD_ROWS; ++n) {
                uint8_t row = get_row_for_update();
                TextItem *l = &line[row];
                if (l->hasUpdates) updateLCD(row, l);
            }
        }
    }
}

PlotData *Display::getData() { return &data; }

void Display::updateLCD(uint8_t row, TextItem *l, boolean forced) {
    lcd->setCursor(0, row);

    refreshed = millis();

    uint8_t _fix_len = strlen(l->fixed_str);
    uint8_t _var_len = strlen(l->var_str);

    uint8_t _free_space = LCD_COLS - _fix_len;

    // Fixed
    lcd->print(l->fixed_str);

    if (_var_len == _free_space) {
        lcd->print(l->var_str);
        l->hasUpdates = false;
        return;
    }

    if (_var_len < _free_space) {
        char tmp[_free_space + 1];
        memset(tmp, '\x20', _free_space);
        tmp[_free_space] = '\x00';
        for (uint8_t n = 0; n < _var_len; ++n) tmp[n] = l->var_str[n];
        lcd->print(tmp);
        l->hasUpdates = false;
        return;
    }

    // repeat
    if (l->var_pos > _var_len + _free_space) {
        l->var_pos = 1;
        l->screen_X = 1;
    }
    // [    <-abc]
    if (l->var_pos < _free_space) {
        char tmp[32];
        strfill(tmp, '\x20', 32);
        uint8_t x = _free_space - l->var_pos + 1;
        strncpy(tmp + x, l->var_str, l->var_pos);
        tmp[_free_space] = '\x00';
        lcd->print(tmp);
    } else if (_free_space <= l->var_pos) {
        // [xyz<-    ]
        char tmp[32];
        strfill(tmp, '\x20', _free_space + 1);
        for (uint8_t str_index = 0; str_index < _free_space; str_index++) {
            int8_t _index = l->var_pos - _free_space + str_index;
            if (_index >= _var_len) {
                tmp[str_index] = '\x00';
                break;
            }
            char ch = l->var_str[_index];
            tmp[str_index] = ch;
            if (ch == '\x00') break;
        }
        lcd->print(tmp);
        strfill(tmp, '\x20', _free_space - strlen(tmp) + 1);
        lcd->print(tmp);
    }
    l->var_pos++;
}

void Display::drawBar(uint8_t row, uint8_t per) {
    if (!connected) return;
    uint8_t cols = floor((float)LCD_COLS * per / 100);
    char buf[LCD_COLS + 1];

    strfill(buf, '*', cols + 1);
    strpadd(buf, StrUtils::LEFT, LCD_COLS);
    lcd->setCursor(0, row);
    lcd->print(buf);
#ifdef DEBUG_DISPLAY
    DEBUG.printf("[display] drawBar(%s)", buf);
    DEBUG.println();
#endif
}

void Display::drawTextCenter(uint8_t row, const char *str) {
    if (!connected) return;

    char buf[LCD_COLS + 1];

    size_t str_len = strlen(str);
    if (str_len > LCD_COLS) str_len = LCD_COLS;
    strncpy(buf, str, str_len);
    buf[str_len] = '\x00';
    StrUtils::strpadd(buf, StrUtils::CENTER, LCD_COLS + 1);
    lcd->setCursor(0, row);
    lcd->print(buf);
#ifdef DEBUG_DISPLAY
    DEBUG.printf("[display] drawTextCenter(%s)", tmp);
    DEBUG.println();
#endif
}

void Display::scrollDown() {
    if (lines > LCD_ROWS ) {

    };
}

void Display::setLines(size_t size) { lines = size; }

void Display::clear() {
    lcd->clear();
    lines = 0;
}

void Display::loadBank(CharBank bank, bool force) {
    if ((!force) && (this->bank == bank)) return;
    switch (bank) {
        case BANK_PLOT:
            lcd->createChar(0, char_solid);
            lcd->createChar(1, char_1_8);
            lcd->createChar(2, char_2_8);
            lcd->createChar(3, char_3_8);
            lcd->createChar(4, char_4_8);
            lcd->createChar(5, char_5_8);
            lcd->createChar(6, char_6_8);
            lcd->createChar(7, char_7_8);
            break;
        case BANK_BAR:
            lcd->createChar(1, char_1_5);
            lcd->createChar(2, char_2_5);
            lcd->createChar(3, char_3_5);
            lcd->createChar(4, char_4_5);
            break;
        case BANK_NONE:
            for (uint8_t i = 0; i < 8; ++i) lcd->createChar(i, char_empty);
        default:
            break;
    }
    this->bank = bank;
}

float map_to_plot_min_max(PlotData *pd, uint8_t x) {
    float val = pd->cols[x];
    float min = pd->min_value;
    float max = pd->max_value - min;
    return 1 + floor((val - min) / max * (PLOT_ROWS * 8 - 2));
}

void Display::drawPlot(uint8_t start_col, size_t cols) {
    if (!connected) return;

    loadBank(BANK_PLOT);
    lcd->clear();

    for (uint8_t x = 0; x < cols; ++x) {
        uint8_t y = map_to_plot_min_max(&this->data, x);
        uint8_t col = start_col + x;
        for (uint8_t row = LCD_ROWS; row > 0; row--) {
            lcd->setCursor(col, row - 1);
            #ifdef DEBUG_PLOT
                        DEBUG.printf("#%d %2.4f => %d", row, this->data.cols[x], y);
                        DEBUG.println();
            #endif
            if (y >= 8) {
                lcd->write(0);  // Full
                y -= 8;
            } else if (y > 0) {
                lcd->write((uint8_t)y);  // Partial
                y = 0;
            } else if (y == 0) {
                lcd->write('\x20');  // Empty
            }
        }
    }

    char tmp[16];
    const char *str = dtostrf(this->data.max_value, 5, 4, tmp);
    lcd->setCursor(start_col + cols + 1, LCD_ROW_1);
    lcd->print(str);
#ifdef DEBUG_PLOT
    DEBUG.printf("max_value %s", str);
    DEBUG.println();
#endif
    str = dtostrf(this->data.min_value, 5, 4, tmp);
    lcd->setCursor(start_col + cols + 1, LCD_ROW_2);
    lcd->print(str);
#ifdef DEBUG_PLOT
    DEBUG.printf("min_value %s", str);
    DEBUG.println();
#endif
}
