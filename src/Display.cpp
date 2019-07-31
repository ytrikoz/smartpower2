#include "Display.h"

uint8_t char_blank[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                         0b00000, 0b00000, 0b00000, 0b00000};

uint8_t char_full[8] = {0b11111, 0b11111, 0b11111, 0b11111,
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
            lcd = new LiquidCrystal_I2C(addr, LCD_COLS, LCD_ROWS);
            lcd->init();
            lcd->clear();
            connected = true;
        }
        if (connected)
            output->println(FPSTR(str_ready));
        else
            output->println(FPSTR(str_unknown));
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

void Display::setItem(uint8_t row, const char *str) { setItem(row, NULL, str); }

void Display::setItem(uint8_t row, const char *param, const char *value) {
    DisplayItem *l = &item[row];
    bool changed = str_utils::setstr(l->param, param, LCD_COLS + 1) |
                   str_utils::setstr(l->value, value, DISPLAY_VIRTUAL_COLS + 1);
#ifdef DEBUG_DISPLAY
    DEBUG.printf("[display] setItem row:%d, changed:%d, '%s', '%s'", row,
                 changed, l->param, l->value);
    DEBUG.println();
#endif
    if (changed) {
        l->index = 1;
        l->screen_X = 1;
        l->screen_Y = 1;
        l->updated = true;
    }
}

void Display::drawBar(uint8_t row, uint8_t per) {
    if (!connected) return;

    char buf[LCD_COLS + 1];
    uint8_t bar_value = floor( (float) LCD_COLS * per / 100);
    str_utils::str_of_char(buf, '#', bar_value + 1);

    setItem(row, buf);
    redraw(true);
}

void Display::redraw(boolean forced) {
    if (!connected) return;

    if ((!forced) && (lastUpdated != 0))
        if (millis() - lastUpdated < LCD_REFRESH_INTERVAL_ms) return;

    for (uint8_t row = 0; row <= LCD_ROWS - 1; row++) {
        DisplayItem *l = &item[row];
#ifdef DEBUG_DISPLAY
        DEBUG.printf(
            "[display] redraw forced:%d, row:%d, updated:%d, param:'%s', "
            "value:'%s'",
            forced, row, l->updated, l->param, l->value);
        DEBUG.println();
#endif

        if (!l->updated) continue;

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
    lastUpdated = millis();
}

void Display::initHorizontalBar() {
    lcd->createChar(1, char_1_5);
    lcd->createChar(2, char_2_5);
    lcd->createChar(3, char_3_5);
    lcd->createChar(4, char_4_5);
}

void Display::initVerticalPlot() {
    lcd->createChar(1, char_1_8);
    lcd->createChar(2, char_2_8);
    lcd->createChar(3, char_3_8);
    lcd->createChar(4, char_4_8);
    lcd->createChar(5, char_5_8);
    lcd->createChar(6, char_6_8);
    lcd->createChar(7, char_7_8);
}

const uint8_t plot_width = 8;   // cols
const uint8_t plot_height = 2;  // rows
void Display::drawPlot(float *data, int data_size) {
    float plot[plot_width] = {0};
    float min_value = 0;
    float max_value = 0;
    int plot_element = floor((float)data_size / plot_width);
    int data_start = data_size % plot_width;
    DEBUG.printf("plot_element = %d, data_start = %d", plot_element,
                 data_start);
    DEBUG.println();
    for (uint8_t i = 0; i < plot_width; i++) {
        float plot_element_value = 0;
        for (uint8_t j = 0; j < plot_element; j++) {
            plot_element_value += data[data_start + (i * plot_element) + j];
        }
        plot[i] = floor(plot_element_value / plot_element);
        if (min_value > plot[i] || min_value == 0) {
            min_value = plot[i];
        }
        if (max_value < plot[i]) {
            max_value = plot[i];
        }
    }

    DEBUG.printf("min_value = %2.4f, max_value = %2.4f", min_value, max_value);
    DEBUG.println();

    drawPlot(plot, min_value, max_value, 0);
}

void Display::drawPlot(float *plot, float min_value, float max_value,
                       uint8_t start_pos) {
    for (uint8_t i = 0; i < plot_width; i++) {
        uint8_t plot_value = floor((float)(plot[i] - min_value) /
                                   (max_value - min_value) * plot_height * 8);
        DEBUG.printf("i = %d, data_value = %f, plot_value = %d", i, plot[i],
                     plot_value);
        DEBUG.println();
        for (uint8_t row = LCD_ROWS - 1; row >= 0; row--) {
            if (plot_value >= 8) {
                // full fill
                lcd->setCursor(i, row);
                lcd->write(0);
                plot_value = plot_value - 8;
                continue;
            }
            if (plot_value > 0) {
                lcd->setCursor(i, row);
                lcd->write(plot_value);
                plot_value = 0;
                continue;
            }

            if (plot_value == 0) {
                lcd->setCursor(i, row);
                lcd->write(16);
                continue;
            }
        }
    }
}