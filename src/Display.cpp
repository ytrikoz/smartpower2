#include "Display.h"

#include "Plot.h"

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
    lockTimeout = 0;
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

            lcd = new LiquidCrystal_I2C(addr, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
            lcd->begin(LCD_COLS, LCD_ROWS);
            lcd->clear();

            memset(item, 0, sizeof(TextItem) * DISPLAY_VIRTUAL_ROWS);

            loadBank(BANK_NONE, true);

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

void Display::addTextItem(uint8_t row, const char *str) {
    addTextItem(row, nullptr, str);
}

void Display::addTextItem(uint8_t n, const char *fixed_str, const char *var_str) {
    TextItem *l = &item[n];
    bool updates = setstr(l->fixed_str, fixed_str, LCD_COLS + 1);
    updates |= setstr(l->var_str, var_str, DISPLAY_VIRTUAL_COLS + 1);
    if (updates) {
#ifdef DEBUG_DISPLAY
        DEBUG.printf("#%d addTextItem(%s_%s)", n, l->fixed_str, l->var_str);
        DEBUG.println();
#endif
        l->var_pos = 1;
        l->screen_X = 1;
        l->screen_Y = 1;
        l->hasUpdates = true;
    }
}

void Display::setScreen(Screen screen, size_t item_count) {
    if (this->screen != screen) {
        this->screen = screen;
        this->items_count = item_count;        
        lcd->clear();
        // for (uint8_t i = 0; i < item_count; ++i) item[i].hasUpdates = true;
        active = true;
    }
}

Screen Display::getScreen() { return this->screen; }

void Display::unlock() {
    active = true;
    lockTimeout = 0;
}

void Display::lock() {
    active = false;
    lockTimeout = 0;
}

void Display::lock(unsigned long timeout) {
#ifdef DEBUG_DISPLAY
    DEBUG.printf("lock %lu", timeout);
    DEBUG.println();
#endif
    lockTimeout = timeout;
    lockUpdated = millis();
}

bool Display::locked() {
    if (!active) return true;

    if (lockTimeout > 0) {
        unsigned long now = millis();
        unsigned long passed = millis_passed(lockUpdated, now);
        if (lockTimeout > passed) {
            lockTimeout -= passed;
        } else {
            lockTimeout = 0;
        }
        lockUpdated = now;
    }
    return (lockTimeout > 0);
}

TextItem *Display::getItemForRow(uint8_t row) { 
    size_t pos  = cur_item + row;     
    item[pos].hasUpdates = true;
    return &item[pos]; 
}

void Display::scrollDown() {
    if (++cur_item + LCD_ROWS > items_count) cur_item = 0;
#ifdef DEBUG_DISPLAY
    DEBUG.printf("scrollDown(%d) ", cur_item);
    DEBUG.println();
#endif
}

void Display::loop() {
    if (!connected) return;
    if (millis_since(refreshed) > LCD_REFRESH_INTERVAL_ms) {
        if (!locked()) {
            for (size_t n = 0; n < LCD_ROWS; ++n) {
                TextItem *l = getItemForRow(n);
#ifdef DEBUG_DISPLAY
                DEBUG.printf("#%d drawTextItem(%s) ", n, l->fixed_str);
                DEBUG.println();
#endif
                if (l->hasUpdates) {
                    drawTextItem(n, l);
                    refreshed = millis();
                }
            }
        }
    }
}

PlotData *Display::getData() { return &data; }

void Display::drawTextItem(uint8_t row, TextItem *l) {
    uint8_t _fix_len = strlen(l->fixed_str);
    uint8_t _var_len = strlen(l->var_str);

    uint8_t _free_space = LCD_COLS - _fix_len; 

    if (_var_len <= _free_space) {   
        char buf[LCD_COLS + 1];
        strcpy(buf, l->fixed_str);
        strcat(buf, l->var_str);
        if (_free_space - _var_len > 0) {
            char tmp[_free_space - _var_len + 1];
            strfill(tmp, '\x20', _free_space - _var_len + 1);
            tmp[_free_space - _var_len] = '\x00';
            strcat(buf, tmp);
        }
        drawText(0, row, buf);
        l->hasUpdates = false;
        return;    } 
    else 
    {   // Fixed
        drawText(0, row, l->fixed_str);
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
        strpadd(tmp, StrUtils::LEFT, _free_space + 1);
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
    DEBUG.printf("#%d drawBar(%s)", row, buf);
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
    DEBUG.printf("#%d drawTextCenter(%s)", row, str);
    DEBUG.println();
#endif
}

void Display::clear() {
    items_count = 0;
    cur_item = 0;
    screen = SCREEN_CLEAR;
    lcd->clear();
}

void Display::loadBank(CharBank bank, bool force) {
    if ((!force) && (this->bank == bank)) return;
    #ifdef DEBUG_DISPLAY
    DEBUG.printf("loadBank(%d, %d)", bank, force);
    DEBUG.println();
    #endif
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

void Display::drawPlot(uint8_t col_start) {
    if (!connected) return;

    loadBank(BANK_PLOT);
    lcd->clear();

    for (uint8_t x = 0; x < this->data.size; ++x) {
        uint8_t y = map_to_plot_min_max(&this->data, x);
#ifdef DEBUG_PLOT
        DEBUG.printf("#%d %2.4f %d", x, this->data.cols[x], y);
        DEBUG.println();
#endif
        uint8_t col = col_start + x;
        for (uint8_t row = LCD_ROWS; row > 0; row--) {
            lcd->setCursor(col, row - 1);
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

    uint8_t col = col_start + this->data.size + 1;
    drawFloat(col, LCD_ROW_1, this->data.max_value);
    drawFloat(col, LCD_ROW_2, this->data.min_value);
}

void Display::drawText(uint8_t col, uint8_t row, const char* str) {
    lcd->setCursor(col, row);
    lcd->print(str);
}

void Display::drawFloat(uint8_t col, uint8_t row, float value) {
    char tmp[16];
    lcd->setCursor(col, row);
    lcd->print(dtostrf(value, 5, 4, tmp));
}
