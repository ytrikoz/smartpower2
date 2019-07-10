#include "Display.h"

Display::Display(HardwareSerial *serial = NULL) {
    this->_serial = serial;
    this->addr = 0x00;
    this->connected = false;
    this->lastUpdateTime = 0;

    memset(&line[0], 0x00, (sizeof(DisplayLine) * LCD_ROWS));
}

bool Display::isConnected() { return this->connected; }

bool Display::init() {
    if (!this->connected) {
        this->_print("[lcd]");
        this->_print(" ");

        if (this->getAddr()) {
            this->lcd = new LiquidCrystal_I2C(this->addr, LCD_COLS, LCD_ROWS);
            this->lcd->init();
            this->lcd->clear();
            this->_print("+");
            this->connected = true;
        } else {
            this->_print("-");
            this->connected = false;
        }
        this->_println();
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

void Display::setLine(uint8_t row, const char *valueStr) {
    setParamLine(row, NULL, valueStr);
}

const char *Display::getParam(uint8_t row) { return line[row].param; }

void Display::setParamLine(uint8_t row, const char *paramStr,
                           const char *valueStr) {
    if (setstr(this->line[row].param, paramStr, LCD_COLS + 1) |
        setstr(this->line[row].value, valueStr, 64)) {
        this->line[row].index = 1;
        this->line[row].page = 1;
        this->line[row].needsToRefresh = true;
    }
}

void Display::onProgress(uint8_t per, const char *message) {
    if (!connected) return;

    if (message != NULL) {
        setLine(0, message);
    }

    char str[LCD_COLS + 1];
    charsOf(str, '#', (LCD_COLS * per / 100) + 1);
    setLine(1, str);

    update(true);
}

void Display::update(boolean forced) {
    if (!connected) return;

    if (!forced && ((millis() - this->lastUpdateTime) < REFRESH_INTERVAL) &&
        (this->lastUpdateTime != 0)) {
        return;
    }

    lastUpdateTime = millis();

    for (uint8_t row = 0; row < LCD_ROWS; row++) {
        if (!line[row].needsToRefresh) continue;

        uint8_t _param_len = strlen(this->line[row].param);
        uint8_t _value_len = strlen(line[row].value);
        uint8_t _page_size = LCD_COLS - _param_len;

        // Param
        this->lcd->setCursor(0, row);
        this->lcd->print(line[row].param);

        // Value
        // start over
        if (line[row].index > (_value_len + _page_size)) {
            line[row].index = 1;
            line[row].page = 1;
        }
        uint8_t _pos = line[row].index++;

        // Static
        if (_value_len <= _page_size) {
            // Value
            this->lcd->print(line[row].value);
            // Wipe
            char str[LCD_COLS + 1];
            charsOf(str, ' ', _page_size - _value_len + 1);
            this->lcd->print(str);

            this->line[row].needsToRefresh = false;
            continue;
        }

        // Moving
        // First page
        if (_pos < _page_size) {
            char str[LCD_COLS + 1];
            charsOf(str, ' ', _page_size - _pos + 1);
            this->lcd->print(str);

            strncpy(str, line[row].value, _pos);
            str[_pos] = '\x00';
            this->lcd->print(str);
            continue;
        }
        // Last page
        else if (_pos >= _page_size) {
            char str[LCD_COLS + 1];
            charsOf(str, ' ', _page_size + 1);

            for (uint8_t str_index = 0; str_index < _page_size; str_index++) {
                int8_t _index = _pos - _page_size + str_index;
                if (_index >= _value_len) {
                    str[str_index] = '\x00';
                    break;
                }
                char chr = line[row].value[_index];
                str[str_index] = chr;
                if (chr == '\x00') {
                    break;
                }
            }
            this->lcd->print(str);

            charsOf(str, ' ', _page_size - strlen(str) + 1);
            this->lcd->print(str);
        }
    };
}

void Display::show(const String &s) { this->lcd->print(s); }

void Display::_print(String arg) {
    if (this->_serial == NULL) return;

    this->_serial->print(arg);
}

void Display::_println(void) { this->_print("\r\n"); }

void Display::_println(String arg) {
    this->_print(arg);
    this->_println();
}