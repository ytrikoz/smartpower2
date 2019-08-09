#include "Display.h"

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

uint8_t _cross[8] = {0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};

uint8_t _retarrow[8] = {0x1, 0x1, 0x5, 0x9, 0x1f, 0x8, 0x4};

using str_utils::setstr;
using str_utils::strfill;

Display::Display() {
    addr = 0x00;
    connected = false;
    updated = 0;
}

void Display::setOutput(Print *p) { output = p; }

bool Display::ready() { return connected; }

bool Display::init() {
    if (!connected) {
        output->print(FPSTR(str_lcd));
        if (connect()) {
            output->print(FPSTR(str_found));
            output->print(addr, HEX);

            lcd = new LiquidCrystal_I2C(addr, LCD_COLS, LCD_ROWS);
            lcd->init();
            lcd->clear();

            memset(line, 0, sizeof(TextItem) * DISPLAY_VIRTUAL_ROWS);

            connected = true;
        } else {
            output->println(FPSTR(str_not_found));
        }
    }
    return connected;
}

void Display::turnOn() {
    if (lcd) {
        lcd->display();
        lcd->backlight();
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

void Display::updateLCD(boolean forced) {
    if (!connected) return;
    if (!forced && (millis_since(updated) < LCD_REFRESH_INTERVAL_ms)) return;
    uint8_t row = get_row_for_update();
    TextItem *l = &line[row];
    if (!l->hasUpdates) return;
    lcd->setCursor(0, row);
#ifdef DEBUG_DISPLAY
    DEBUG.printf("[updateLCD] redraw row: %d forced: %d", row, forced);
    DEBUG.println();
#endif
    updated = millis();

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
        strfill(tmp, ' ', 32);
        uint8_t x = _free_space - l->var_pos + 1;
        strncpy(tmp + x, l->var_str, l->var_pos);
        tmp[_free_space] = '\x00';
        lcd->print(tmp);
    } else if (_free_space <= l->var_pos) {
        // [xyz<-    ]
        char tmp[32];
        strfill(tmp, ' ', _free_space + 1);
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
        strfill(tmp, ' ', _free_space - strlen(tmp) + 1);
        lcd->print(tmp);
    }
    l->var_pos++;
}

void Display::drawBar(uint8_t row, uint8_t per) {
    if (!connected) return;

    uint8_t cols = floor((float)LCD_COLS * per / 100);
    char tmp[cols + 1];
    strfill(tmp, '#', cols + 1);
    lcd->setCursor(0, row);
    lcd->print(tmp);
}

void Display::drawText(uint8_t row, const char * str) {
    if (!connected) return;
    lcd->setCursor(0, row);
    lcd->print(str);
}

void Display::load_bar_bank() {
    lcd->createChar(1, char_1_5);
    lcd->createChar(2, char_2_5);
    lcd->createChar(3, char_3_5);
    lcd->createChar(4, char_4_5);
}

void Display::load_plot_bank() {
    lcd->createChar(0, char_solid);
    lcd->createChar(1, char_1_8);
    lcd->createChar(2, char_2_8);
    lcd->createChar(3, char_3_8);
    lcd->createChar(4, char_4_8);
    lcd->createChar(5, char_5_8);
    lcd->createChar(6, char_6_8);
    lcd->createChar(7, char_7_8);
}

void Display::loadData(float *values, size_t size) {
    int block_size = floor((float)size / PLOT_COLS);
    int offset = 0;
#ifdef DEBUG_PLOT
    DEBUG.printf("per %d => group by %d, left %d", per, block_size);
    DEBUG.println();
#endif
    for (uint8_t block = 0; block < PLOT_COLS; block++) {
        float block_sum = 0;
        for (uint8_t n = 0; n < block_size; n++) {
            block_sum += plot->values[offset + (block * block_size) + n];
        }
        float value = block_sum / block_size;
#ifdef DEBUG_PLOT
        DEBUG.printf("sum %.4f for %d items, avg %.4f", block_sum, block_size,
                     value);
        DEBUG.println();
#endif
        if (plot->min_value > value) plot->min_value = value;
        if (plot->max_value < value) plot->max_value = value;
        plot->values[block] = value;
    }
#ifdef DEBUG_PLOT
    DEBUG.printf("min = %2.4f, max = %2.4f", data->minvalue, data->maxvalue);
    DEBUG.println();
#endif
}

float get_plot_y(PlotData *p, uint8_t x) {
    float value = p->values[x];
    float min = p->min_value;
    float max = p->max_value;
    return floor((value - min) / (max - min) * PLOT_ROWS * 8);
}

void Display::drawPlot(uint8_t offset_x) {
    if (!connected) return;
    load_plot_bank();
    lcd->clear();
    for (uint8_t x = 0; x < PLOT_COLS; ++x) {
        uint8_t y = get_plot_y(plot, x);
        uint8_t col = offset_x + x;
        for (uint8_t row = LCD_ROWS; row > 0; row--) {
            lcd->setCursor(col, row - 1);
            // Full
            if (y >= 8) {
                lcd->write(0);
                y = y - 8;
                // Partial
            } else if (y > 0) {
                lcd->write((uint8_t)y);
                y = 0;
                // Empty
            } else if (y == 0) {
                lcd->write(16);
            }
#ifdef DEBUG_PLOTS
            DEBUG.printf("#%d %2.4f => %d", i, plot[i], value);
            DEBUG.println();
#endif
        }
    }
}
