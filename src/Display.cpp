#include "Display.h"

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
        output->printf_P(str_lcd);
        if (this->getAddr()) {
            this->lcd = new LiquidCrystal_I2C(this->addr, LCD_COLS, LCD_ROWS);
            this->lcd->init();
            this->lcd->clear();
            this->connected = true;
        } else {
            this->connected = false;
        }
        if (connected)
            output->printf_P(str_ready);
        else
            output->printf_P(str_unknown);
        output->println();
    }
    return connected;
}

void Display::turnOn() {
    this->lcd->display();
    this->lcd->backlight();
}

void Display::turnOff() {
    this->lcd->noDisplay();
    this->lcd->noBacklight();
}

bool Display::getAddr() {
    Wire.beginTransmission(LCD_SLAVE_ADDRESS);
    if (!Wire.endTransmission()) {
        this->addr = LCD_SLAVE_ADDRESS;
        return true;
    }
    this->addr = 0;
    return false;
}

void Display::setItem(uint8_t row, const char *str) { setItem(row, NULL, str); }

void Display::setItem(uint8_t row, const char *param, const char *value) {
    DisplayItem *i = &item[row];
#ifdef DEBUG_DISPLAY
    output->printf("[display] %d, %s, %s ", row, param, value);
#endif
    bool changed = str_utils::setstr(i->param, param, LCD_COLS + 1) |
                   str_utils::setstr(i->value, value, DISPLAY_VIRTUAL_COLS + 1);
    if (changed) {
        i->index = 1;
        i->screen_X = 1;
        i->screen_Y = 1;
        i->updated = true;
    }
}

const char *Display::getParamStr(uint8_t row) { return item[row].param; }

void Display::onProgress(uint8_t per, const char *str) {
    if (!connected) return;
    if (strlen(str) > 0) {
        setItem(0, str);
    }
    char tmp[LCD_COLS + 1];
    str_utils::setStrOfChar(tmp, '#', (LCD_COLS * per / 100) + 1);
    setItem(1, tmp);
    redraw(true);
}

void Display::redraw(boolean forced) {
    if (!connected) return;
    unsigned long now = millis();
    if (!forced && ((now - lastUpdated) < REFRESH_INTERVAL) &&
        (lastUpdated != 0))
        return;

    lastUpdated = now;

    for (uint8_t row = 0; row < LCD_ROWS - 1; row++) {
        DisplayItem *i = &item[row];
        if (!i->updated) continue;
        uint8_t _param_len = strlen(i->param);
        uint8_t _value_len = strlen(i->value);
        uint8_t _free_space = LCD_COLS - _param_len;

        // Param
        this->lcd->setCursor(0, row);
        this->lcd->print(i->param);

        // Value
        // start over
        if (i->index > (_value_len + _free_space)) {
            i->index = 1;
            i->screen_X = 1;
        }
        uint8_t pos = i->index++;

        // Fixed
        if (_value_len == _free_space) {
            // Value
            lcd->print(i->value);
            i->updated = false;
        } else if (_value_len < _free_space) {
            // Value
            lcd->print(i->value);
            // Wipe
            uint8_t wipe_str_len = _free_space - _value_len;
            char buf[wipe_str_len + 1];
            str_utils::setStrOfChar(buf, ' ', wipe_str_len);
            lcd->print(buf);
            i->updated = false;
        }

        if (!i->updated) continue;
        char buf[LCD_COLS + 1];
        // horizontal scrolling
        // [    <-abc]
        if (pos < _free_space) {
            str_utils::setStrOfChar(buf, ' ', _free_space - pos + 1);
            lcd->print(buf);

            strncpy(buf, i->value, pos);
            buf[pos] = '\x00';
            lcd->print(buf);
        }
        // [xyz<-    ]
        else if (pos >= _free_space) {
            str_utils::setStrOfChar(buf, ' ', _free_space + 1);
            for (uint8_t str_index = 0; str_index < _free_space; str_index++) {
                int8_t _index = pos - _free_space + str_index;
                if (_index >= _value_len) {
                    buf[str_index] = '\x00';
                    break;
                }
                char chr = i->value[_index];
                buf[str_index] = chr;
                if (chr == '\x00') {
                    break;
                }
            }
            lcd->print(buf);
            str_utils::setStrOfChar(buf, ' ', _free_space - strlen(buf) + 1);
            lcd->print(buf);
        }
    };
}

void Display::show(const String &s) { this->lcd->print(s); }

void Display::_print(String arg) {
    if (output == nullptr) return;

    output->print(arg);
}

void Display::_println(void) { _print("\r\n"); }

void Display::_println(String arg) {
    _print(arg);
    _println();
}
