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
        if (getAddr()) {
            lcd = new LiquidCrystal_I2C(addr, LCD_COLS, LCD_ROWS);
            lcd->init();
            lcd->clear();
            connected = true;
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
    addr = 0;
    return false;
}

void Display::setItem(uint8_t row, const char *str) { setItem(row, NULL, str); }

void Display::setItem(uint8_t row, const char *param, const char *value) {
    DisplayItem *i = &item[row];
    bool changed = str_utils::setstr(i->param, param, LCD_COLS + 1) |
                   str_utils::setstr(i->value, value, DISPLAY_VIRTUAL_COLS + 1);
    #ifdef DEBUG_DISPLAY
        DEBUG.printf("[display] setItem row:%d, changed:%d, '%s', '%s'", row, changed, i->param, i->value);
        DEBUG.println();
    #endif            
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

    if (strlen(str) > 0) setItem(0, str);

    char tmp[LCD_COLS + 1];
    str_utils::strOfChar(tmp, '#', (LCD_COLS * per / 100) + 1);    
    setItem(1, tmp);

    redraw(true);
}

void Display::redraw(boolean forced) {
    if (!connected) return;

    if ((!forced) && (lastUpdated != 0))
        if (millis() - lastUpdated < LCD_REFRESH_INTERVAL_ms)
            return;

    for (uint8_t row = 0; row <= LCD_ROWS - 1; row++) {
        DisplayItem *i = &item[row];
        #ifdef DEBUG_DISPLAY
            DEBUG.printf("[display] redraw forced:%d, row:%d, updated:%d, param:'%s', value:'%s'", forced, row, i->updated, i->param, i->value);
            DEBUG.println();
        #endif   

        if (!i->updated) continue;

        uint8_t _param_len = strlen(i->param);
        uint8_t _value_len = strlen(i->value);
        uint8_t _free_space = LCD_COLS - _param_len;

        // Param
        lcd->setCursor(0, row);
        lcd->print(i->param);

        // Value
        // start over
        if ( i->index > _value_len + _free_space) {
            i->index = 1;
            i->screen_X = 1;
        }

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
            str_utils::strOfChar(buf, ' ', wipe_str_len);
            lcd->print(buf);
            i->updated = false;
        }

        if (!i->updated) continue;

        char buf[LCD_COLS + 1];
        // horizontal scrolling
        // [    <-abc]
        if (i->index < _free_space) {
            str_utils::strOfChar(buf, ' ', _free_space - i->index + 1);
            lcd->print(buf);
            strncpy(buf, i->value, i->index);
            buf[i->index] = '\x00';
            lcd->print(buf);
        }
        // [xyz<-    ]
        else if (i->index >= _free_space) {
            str_utils::strOfChar(buf, ' ', _free_space + 1);
            for (uint8_t str_index = 0; str_index < _free_space; str_index++) {
                int8_t _index = i->index - _free_space + str_index;
                if (_index >= _value_len) {
                    buf[str_index] = '\x00';
                    break;
                }
                char chr = i->value[_index];
                buf[str_index] = chr;
                if (chr == '\x00') break;
            }
            lcd->print(buf);
            str_utils::strOfChar(buf, ' ', _free_space - strlen(buf) + 1);
            lcd->print(buf);
        }
        i->index++;
    };
    lastUpdated = millis();
}

