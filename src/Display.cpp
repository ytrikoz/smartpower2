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

Display::Display() {
    addr = 0x00;
    connected = false;
    lastUpdated = 0;
    memset(&item[0], 0, sizeof(DisplayItem) * LCD_ROWS);
}

void Display::setOutput(Print *p) { output = p; }

bool Display::isConnected() { return connected; }

bool Display::init() {
    if (!connected) {
        output->print(FPSTR(str_lcd));
        if (getAddr()) {
            output->println(FPSTR(str_ready));

            lcd = new LiquidCrystal_I2C(addr, LCD_COLS, LCD_ROWS);
            lcd->init();
            lcd->clear();

            connected = true;
        } else {
            output->println(FPSTR(str_unknown));
        }
    }
    return connected;
}

void Display::turnOn() {
    lcd->display();
    lcd->backlight();
}

void Display::turnOff() {
    lcd->noDisplay();
    lcd->noBacklight();
}

bool Display::getAddr() {
    Wire.beginTransmission(LCD_SLAVE_ADDRESS);
    if (!Wire.endTransmission()) {
        addr = LCD_SLAVE_ADDRESS;
        return true;
    }
    Wire.beginTransmission(LCD_SLAVE_ADDRESS_ALT);
    if (!Wire.endTransmission()) {
        addr = LCD_SLAVE_ADDRESS_ALT;
        return true;
    }
    addr = 0;
    return false;
}

void Display::setItem(uint8_t row, const char *str) {
    setItem(row, nullptr, str);
}

void Display::setItem(uint8_t row, const char *param, const char *value) {
    DisplayItem *l = &item[row];
    bool changed = str_utils::setstr(l->param, param, LCD_COLS + 1);
    changed |= str_utils::setstr(l->value, value, DISPLAY_VIRTUAL_COLS + 1);
    if (changed) {
#ifdef DEBUG_DISPLAY
        DEBUG.printf("[display] setItem row: %d /%s %s/", row, l->param, l->value);
        DEBUG.println();
#endif
        l->index = 1;
        l->screen_X = 1;
        l->screen_Y = 1;
        l->updated = true;
    }
}

void Display::redraw(boolean forced) {
    if (!connected) return;
    if (!forced)
        if (millis_since(lastUpdated) < LCD_REFRESH_INTERVAL_ms) return;
    for (uint8_t row = 0; row <= LCD_ROWS - 1; row++) {
        DisplayItem *l = &item[row];
        if (!l->updated) continue;
#ifdef DEBUG_DISPLAY
        DEBUG.printf("[display] redraw row: %d forced: %d", row, forced);
        DEBUG.println();
#endif
        uint8_t _param_len = strlen(l->param);
        uint8_t _value_len = strlen(l->value);
        uint8_t _free_space = LCD_COLS - _param_len;

        lastUpdated = millis();

        // Param
        lcd->setCursor(0, row);
        lcd->print(l->param);

        // Value
        // start over
        if (l->index > _value_len + _free_space) {
            l->index = 1;
            l->screen_X = 1;
        }

        // Fixed
        if (_value_len == _free_space) {
            // Value
            lcd->print(l->value);
            l->updated = false;
            continue;
        } else if (_value_len < _free_space) {
            // Value
            lcd->print(l->value);
            // Wipe
            uint8_t wipe_str_len = _free_space - _value_len;
            char buf[wipe_str_len + 1];
            str_utils::str_of_char(buf, ' ', wipe_str_len + 1);
            lcd->print(buf);
            l->updated = false;
            continue;
        }

        char buf[LCD_COLS + 1];
        // horizontal scrolling
        // [    <-abc]
        if (l->index < _free_space) {
            str_utils::str_of_char(buf, ' ', _free_space - l->index + 1);
            lcd->print(buf);
            strncpy(buf, l->value, l->index);
            buf[l->index] = '\x00';
            lcd->print(buf);
        }
        // [xyz<-    ]
        else if (l->index >= _free_space) {
            str_utils::str_of_char(buf, ' ', _free_space + 1);
            for (uint8_t str_index = 0; str_index < _free_space; str_index++) {
                int8_t _index = l->index - _free_space + str_index;
                if (_index >= _value_len) {
                    buf[str_index] = '\x00';
                    break;
                }
                char chr = l->value[_index];
                buf[str_index] = chr;
                if (chr == '\x00') break;
            }
            lcd->print(buf);
            str_utils::str_of_char(buf, ' ', _free_space - strlen(buf) + 1);
            lcd->print(buf);
        }
        l->index++;
    };
}

void Display::drawBar(uint8_t row, uint8_t per) {
    if (!connected) return;

    char buf[LCD_COLS + 1];
    uint8_t bar_value = floor((float)LCD_COLS * per / 100);
    str_utils::str_of_char(buf, '#', bar_value + 1);

    setItem(row, buf);
    redraw(true);
}

void Display::initHorizontalBar() {
    lcd->createChar(1, char_1_5);
    lcd->createChar(2, char_2_5);
    lcd->createChar(3, char_3_5);
    lcd->createChar(4, char_4_5);
}

void Display::prepareToPlot() {
    lcd->createChar(0, char_solid);
    lcd->createChar(1, char_1_8);
    lcd->createChar(2, char_2_8);
    lcd->createChar(3, char_3_8);
    lcd->createChar(4, char_4_8);
    lcd->createChar(5, char_5_8);
    lcd->createChar(6, char_6_8);
    lcd->createChar(7, char_7_8);
}

void Display::drawPlot(float *data, size_t data_size) {
    float min_value = 32000;
    float max_value = -32000;
    int page_size = floor((float)data_size / PLOT_COLS);
    int itemsLeft = data_size % PLOT_COLS;
    int offset = 0;
    DEBUG.printf("size %d => group by %d, left %d", data_size, page_size,
                 itemsLeft);
    DEBUG.println();
    for (uint8_t page = 0; page < PLOT_COLS; page++) {
        float page_sum = 0;
        for (uint8_t n = 0; n < page_size; n++) {
            page_sum += data[offset + (page * page_size) + n];
        }
        // avg value for group, min, max for set
        float page_value = (page_sum / page_size);

        DEBUG.printf("sum %.4f for %d items, avg %.4f", page_sum, page_size,
                     page_value);
        DEBUG.println();

        if (min_value > page_value) min_value = page_value;
        if (max_value < page_value) max_value = page_value;

        plot[page] = page_value;
    }

    DEBUG.printf("min = %2.4f, max = %2.4f", min_value, max_value);
    DEBUG.println();

    prepareToPlot();
    lcd->clear();
    drawPlot(min_value, max_value, 0);
}

float getPlotMapped(float value, float min_value, float max_value) {
    return floor((value - min_value) / (max_value - min_value) * PLOT_ROWS * 8);
}

void Display::drawPlot(float min_value, float max_value, uint8_t start_pos) {
    for (uint8_t i = 0; i < PLOT_COLS; i++) {
        uint8_t mapped = getPlotMapped(plot[i], min_value, max_value);
        DEBUG.printf("#%d %2.4f => %d", i, plot[i], mapped);
        DEBUG.println();
        uint8_t col = start_pos + i;
        for (uint8_t row = LCD_ROWS; row > 0; row--) {
            lcd->setCursor(col, row - 1);
            if (mapped >= 8) {
                DEBUG.printf("r%dc%d = 8", row, col);
                DEBUG.println();
                lcd->write(0);
                mapped = mapped - 8;
                continue;
            }
            if (mapped > 0 && mapped < 8) {
                DEBUG.printf("r%dc%d = %d", row, col, mapped);
                DEBUG.println();
                lcd->write((uint8_t)mapped);
                continue;
            }
            if (mapped == 0) {
                DEBUG.printf("r%dc%d = 0", row, col);
                DEBUG.println();
                lcd->write(16);
                continue;
            }
        }
    }
}