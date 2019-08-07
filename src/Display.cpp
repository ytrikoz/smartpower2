#include "Display.h"

Display::Display() {
    addr = 0x00;
    connected = false;
    lastUpdated = 0;
}

void Display::setOutput(Print *p) { output = p; }

bool Display::isConnected() { return connected; }

bool Display::init() {
    if (!connected) {
        memset(&item[0], 0, sizeof(DisplayItem) * LCD_ROWS);
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
        DEBUG.printf("[display] setItem row: %d /%s %s/", row, l->param,
                     l->value);
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

        // scrolling
        char buf[LCD_COLS + 1];
        // [    <-abc]
        if (l->index < _free_space) {
            str_utils::str_of_char(buf, ' ', _free_space - l->index + 1);
            lcd->print(buf);
            strncpy(buf, l->value, l->index);
            buf[l->index] = '\x00';
            lcd->print(buf);
        } else if (l->index >= _free_space) {
            // [xyz<-    ]
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
        lastUpdated = millis();
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
    DEBUG.printf("size %d => group by %d, left %d", size, block_size);
    DEBUG.println();
#endif
    for (uint8_t block = 0; block < PLOT_COLS; block++) {
        float block_sum = 0;
        for (uint8_t n = 0; n < block_size; n++) {
            block_sum += values[offset + (block * block_size) + n];
        }
        float block_value = (block_sum / block_size);
#ifdef DEBUG_PLOT
        DEBUG.printf("sum %.4f for %d items, avg %.4f", block_sum, block_size,
                     block_value);
        DEBUG.println();
#endif
        if (data->minValue > block_value) data->minValue = block_value;
        if (data->maxValue < block_value) data->maxValue = block_value;
        data->blocks[block] = block_value;
    }
#ifdef DEBUG_PLOT
    DEBUG.printf("min = %2.4f, max = %2.4f", data->minValue, data->maxValue);
    DEBUG.println();
#endif
}


float mapToPlot(float value, float min_value, float max_value) {
    return floor((value - min_value) / (max_value - min_value) * PLOT_ROWS * 8);
}

void Display::drawPlot(uint8_t start_pos) {
    load_plot_bank();
    lcd->clear();
    for (uint8_t i = 0; i < PLOT_COLS; i++) {
        uint8_t plotValue =
            mapToPlot(data->blocks[i], data->minValue, data->maxValue);
        uint8_t col = start_pos + i;
        for (uint8_t row = LCD_ROWS; row > 0; row--) {
            lcd->setCursor(col, row - 1);
            if (plotValue >= 8) {
                lcd->write(0);
                plotValue = plotValue - 8;
            } else if (plotValue > 0) {
                lcd->write((uint8_t)plotValue);
                plotValue = 0;
            } else if (plotValue == 0) {
                lcd->write(16);
            }
#ifdef DEBUG_PLOTS
            DEBUG.printf("#%d %2.4f => %d", i, plot[i], plotValue);
            DEBUG.println();
#endif
        }
    }
}
