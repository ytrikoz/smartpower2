#include "Display.h"

#include "Plot.h"

uint8_t char_empty[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                         0b00000, 0b00000, 0b00000, 0b00000};

uint8_t char_solid[8] = {0b11111, 0b11111, 0b11111, 0b11111,
                         0b11111, 0b11111, 0b11111, 0b11111};

uint8_t progress_start_0_4[8] = {0b01111, 0b11000, 0b10000, 0b10000,
                                 0b10000, 0b10000, 0b11000, 0b01111};

uint8_t progress_start_1_4[8] = {0b01111, 0b11000, 0b10000, 0b10000,
                                 0b10000, 0b10011, 0b11000, 0b01111};

uint8_t progress_start_2_4[8] = {0b01111, 0b11000, 0b10000, 0b10000,
                                 0b10111, 0b10011, 0b11000, 0b01111};

uint8_t progress_start_3_4[8] = {0b01111, 0b11000, 0b10000, 0b10111,
                                 0b10111, 0b10011, 0b11000, 0b01111};

uint8_t progress_start_4_4[8] = {0b01111, 0b11000, 0b10011, 0b10111,
                                 0b10111, 0b10011, 0b11000, 0b01111};

uint8_t progress_0_8[8] = {0b11111, 0b00000, 0b00000, 0b00000,
                           0b00000, 0b00000, 0b00000, 0b11111};

uint8_t progress_1_8[8] = {0b11111, 0b00000, 0b00000, 0b00000,
                           0b00000, 0b11000, 0b00000, 0b11111};

uint8_t progress_2_8[8] = {0b11111, 0b00000, 0b00000, 0b00000,
                           0b11000, 0b11000, 0b00000, 0b11111};

uint8_t progress_3_8[8] = {0b11111, 0b00000, 0b00000, 0b11000,
                           0b11000, 0b11000, 0b00000, 0b11111};

uint8_t progress_4_8[8] = {0b11111, 0b00000, 0b11000, 0b11000,
                           0b11000, 0b11000, 0b00000, 0b11111};

uint8_t progress_5_8[8] = {0b11111, 0b00000, 0b11000, 0b11000,
                           0b11000, 0b11011, 0b00000, 0b11111};

uint8_t progress_6_8[8] = {0b11111, 0b00000, 0b11000, 0b11000,
                           0b11011, 0b11011, 0b00000, 0b11111};

uint8_t progress_7_8[8] = {0b11111, 0b00000, 0b11000, 0b11011,
                           0b11011, 0b11011, 0b00000, 0b11111};

uint8_t progress_8_8[8] = {0b11111, 0b00000, 0b11011, 0b11011,
                           0b11011, 0b11011, 0b00000, 0b11111};

uint8_t progress_end_0_4[8] = {0b11110, 0b00011, 0b00001, 0b00001,
                               0b00001, 0b00001, 0b00011, 0b11110};

uint8_t progress_end_1_4[8] = {0b11110, 0b00011, 0b00001, 0b00001,
                               0b00001, 0b11001, 0b00011, 0b11110};

uint8_t progress_end_2_4[8] = {0b11110, 0b00011, 0b00001, 0b00001,
                               0b11101, 0b11001, 0b00011, 0b11110};

uint8_t progress_end_3_4[8] = {0b11110, 0b00011, 0b00001, 0b11101,
                               0b11101, 0b11001, 0b00011, 0b11110};

uint8_t progress_end_4_4[8] = {0b11110, 0b00011, 0b11001, 0b11101,
                               0b11101, 0b11001, 0b00011, 0b11110};

uint8_t plot_1_8[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                       0b00000, 0b00000, 0b00000, 0b11111};

uint8_t plot_2_8[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                       0b00000, 0b00000, 0b11111, 0b11111};

uint8_t plot_3_8[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                       0b00000, 0b11111, 0b11111, 0b11111};

uint8_t plot_4_8[8] = {0b00000, 0b00000, 0b00000, 0b00000,
                       0b11111, 0b11111, 0b11111, 0b11111};

uint8_t plot_5_8[8] = {0b00000, 0b00000, 0b00000, 0b11111,
                       0b11111, 0b11111, 0b11111, 0b11111};

uint8_t plot_6_8[8] = {0b00000, 0b00000, 0b11111, 0b11111,
                       0b11111, 0b11111, 0b11111, 0b11111};

uint8_t plot_7_8[8] = {0b00000, 0b11111, 0b11111, 0b11111,
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
    lastUpdated = 0;
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

            memset(items, 0, sizeof(ScreenItem) * DISPLAY_VIRTUAL_ROWS);

            loadBank(BANK_NONE, true);

            backlight = true;
            connected = true;
        } else {
            output->println(FPSTR(str_not_found));
        }
    }
    return connected;
}

void Display::backlightOff() {
    lcd->noBacklight();
    backlight = false;
}

void Display::backlightOn() {
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
    return addr;
}

void Display::addScreenItem(uint8_t n, const char *text) {
    addScreenItem(n, NULL, text);
}

void Display::addScreenItem(uint8_t n, const char *label, const char *text) {
    ScreenItem *item = &items[n];
    if (setstr(item->label, label, LCD_COLS + 1)) item->redrawLabel = true;
    if (setstr(item->text, text, DISPLAY_VIRTUAL_COLS + 1))
        item->redrawText = true;
#ifdef DEBUG_DISPLAY
    if (item->needsRedraw()) {
        DEBUG.printf("#%d addScreenItem(%s_%s)", n, item->fixed_str,
                     item->text);
        DEBUG.println();
    }
#endif
}

void Display::setScreen(Screen screen, size_t item_size) {
    if (this->screen != screen) {
        this->screen = screen;
        this->items_size = item_size;
        this->item_pos = 0;
        lcd->clear();
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

bool Display::locked() { return locked(millis()); }

bool Display::locked(unsigned long now) {
    if (!active) return true;
    if (lockTimeout > 0) {
        unsigned long passed = millis_passed(lockUpdated, now);
        if (lockTimeout > passed) {
            lockTimeout -= passed;
        } else {
            lockTimeout = 0;
        }
        lockUpdated = now;
    }
    return lockTimeout;
}

ScreenItem *Display::getItemForRow(uint8_t row) {
    size_t pos = item_pos + row;
    return &items[pos];
}

void Display::scrollDown() {
    if (items_size <= LCD_ROWS) return;
    if (++item_pos + LCD_ROWS > items_size) item_pos = 0;

    uint8_t pos = item_pos;
    for (uint8_t n = 0; n < LCD_ROWS; ++n) {
        if (pos + n > items_size) item_pos = 0;
        items[pos + n].forceRedraw();
    }
#ifdef DEBUG_DISPLAY
    DEBUG.printf("scrollDown(%d) ", item_pos);
    DEBUG.println();
#endif
}

void Display::loop() {
    if (!connected) return;
    unsigned long now = millis();
    if (millis_passed(lastUpdated, now) >= LCD_UPDATE_INTERVAL) {
        if (!locked(now)) {
            for (uint8_t row = 0; row < LCD_ROWS; ++row) {
                ScreenItem *item = getItemForRow(row);
                if (item->needsRedraw()) drawScreenItem(row, item);
            }
        }
        lastUpdated = now;
    }
}

PlotData *Display::getData() { return &data; }

void Display::drawScreenItem(uint8_t row, ScreenItem *item) {
#ifdef DEBUG_DISPLAY
    DEBUG.printf("#%d drawScreenItem(%s) ", n, item->fixed_str);
    DEBUG.println();
#endif
    uint8_t label_size = strlen(item->label);
    uint8_t text_size = strlen(item->text);
    uint8_t free_size = LCD_COLS - label_size;

    if (text_size <= free_size) {
        char buf[LCD_COLS + 1];
        strcpy(buf, item->label);
        strcat(buf, item->text);
        if (free_size - text_size > 0) {
            char tmp[free_size - text_size + 1];
            strfill(tmp, '\x20', free_size - text_size + 1);
            tmp[free_size - text_size] = '\x00';
            strcat(buf, tmp);
        }
        drawText(0, row, buf);
        item->redrawLabel = false;
        item->redrawText = false;
        return;
    }

    if (item->redrawLabel) {
        drawText(0, row, item->label);
        item->redrawLabel = false;
    }

    if (item->text_pos > text_size + free_size) item->text_pos = 1;

    char tmp[free_size + 1];
    strfill(tmp, '\x20', free_size + 1);
    // [    <-abc]
    if (item->text_pos < free_size) {
        uint8_t abc_len = free_size - item->text_pos;
        strncpy(tmp + abc_len + 1, item->text, item->text_pos);
    } else if (free_size <= item->text_pos) {
        // [xyz<-    ]
        for (uint8_t col_index = 0; col_index < free_size; col_index++) {
            int8_t index = item->text_pos - free_size + col_index;
            if (index < text_size)
                tmp[col_index] = item->text[index];
            else
                break;
        }
    }
    tmp[free_size] = '\x00';
    drawText(label_size, row, tmp);
    item->text_pos++;
    item->redrawText = true;
}

void Display::drawTextCenter(uint8_t row, const char *str) {
    if (!connected) return;
    char buf[LCD_COLS + 1];
    size_t str_len = strlen(str);
    if (str_len > LCD_COLS) str_len = LCD_COLS;
    strncpy(buf, str, str_len);
    buf[str_len] = '\x00';

    StrUtils::strpadd(buf, StrUtils::CENTER, LCD_COLS + 1);

    drawText(0, row, buf);
}

void Display::clear() {
    screen = SCREEN_CLEAR;
    item_pos = 0;
    items_size = 0;
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
            lcd->createChar(1, plot_1_8);
            lcd->createChar(2, plot_2_8);
            lcd->createChar(3, plot_3_8);
            lcd->createChar(4, plot_4_8);
            lcd->createChar(5, plot_5_8);
            lcd->createChar(6, plot_6_8);
            lcd->createChar(7, plot_7_8);
            break;
        case BANK_PROGRESS:
            lcd->createChar(0, progress_start_4_4);
            lcd->createChar(1, progress_0_8);
            lcd->createChar(2, progress_8_8);
            lcd->createChar(3, progress_end_0_4);
            break;
        case BANK_PROGRESS_START:
            lcd->createChar(4, progress_start_0_4);
            lcd->createChar(5, progress_start_1_4);
            lcd->createChar(6, progress_start_2_4);
            lcd->createChar(7, progress_start_3_4);
        case BANK_PROGRESS_1_TO_4:
            lcd->createChar(4, progress_1_8);
            lcd->createChar(5, progress_2_8);
            lcd->createChar(6, progress_3_8);
            lcd->createChar(7, progress_4_8);
            break;
        case BANK_PROGRESS_4_TO_7:
            lcd->createChar(4, progress_4_8);
            lcd->createChar(5, progress_5_8);
            lcd->createChar(6, progress_6_8);
            lcd->createChar(7, progress_7_8);
            break;
        case BANK_PROGRESS_END:
            lcd->createChar(4, progress_end_1_4);
            lcd->createChar(5, progress_end_2_4);
            lcd->createChar(6, progress_end_3_4);
            lcd->createChar(7, progress_end_4_4);
            break;
        case BANK_NONE:
            for (uint8_t i = 0; i < 8; ++i) lcd->createChar(i, char_empty);
        default:
            break;
    }
    this->bank = bank;
}

void Display::drawProgressBar(uint8_t row, uint8_t per) {
    lcd->setCursor(0, row);

    uint8_t nb_columns = map(per, 0, 100, 0, (LCD_COLS - 4) * 2 * 4 - 2 * 4);

    for (uint8_t i = 0; i < LCD_COLS - 4; ++i) {
        if (i == 0) {
            if (nb_columns > 4) {
                lcd->write((uint8_t)0);
                nb_columns -= 4;
            } else if (nb_columns == 4) {
                lcd->write((uint8_t)0);
                nb_columns = 0;
            } else {
                loadBank(BANK_PROGRESS_START);
                lcd->setCursor(i, row);
                lcd->write((uint8_t)nb_columns + 4);
                nb_columns = 0;
            }
        } else if (i == LCD_COLS - 5) {
            if (nb_columns > 0) {
                loadBank(BANK_PROGRESS_END);
                lcd->setCursor(i, row);
                lcd->write((uint8_t)nb_columns + 3);
            } else {
                lcd->write((uint8_t)3);
            }
        } else {
            if (nb_columns == 0) {
                lcd->write((uint8_t)1);
            } else if (nb_columns >= 8) {
                lcd->write((uint8_t)2);
                nb_columns -= 8;
            } else if (nb_columns >= 4 && nb_columns < 8) {
                loadBank(BANK_PROGRESS_4_TO_7);
                lcd->setCursor(i, row);
                lcd->write((uint8_t)nb_columns);
                nb_columns = 0;
            } else if (nb_columns < 4) {
                loadBank(BANK_PROGRESS_1_TO_4);
                lcd->setCursor(i, row);
                lcd->write((uint8_t)nb_columns + 3);
                nb_columns = 0;
            }
        }
    }
    char tmp[5];
    sprintf(tmp, "%3d%%", per);
    lcd->print(tmp);
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

    uint8_t col = col_start + this->data.size + 2;
    drawFloat(col, LCD_ROW_1, this->data.max_value);
    drawFloat(col, LCD_ROW_2, this->data.min_value);
}

void Display::drawFloat(uint8_t col, uint8_t row, float value) {
    char buf[16];
    sprintf(buf, "%2.4f", value);
    drawText(col, row, buf);
}

void Display::drawText(uint8_t col, uint8_t row, const char *str) {
#ifdef DEBUG_DISPLAY
    DEBUG.printf("#%d drawText(%s)", row, str);
    DEBUG.println();
#endif
    lcd->setCursor(col, row);
    lcd->print(str);
}