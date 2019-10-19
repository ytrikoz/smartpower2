#pragma once

#include "Arduino.h"
#include "Consts.h"
#include "StrUtils.h"

#define DISPLAY_VIRTUAL_COLS 63
#define DISPLAY_VIRTUAL_ROWS 6

using namespace StrUtils;

enum ScreenEnum {
    SCREEN_CLEAR,
    SCREEN_BOOT,
    SCREEN_PSU,
    SCREEN_TEXT,
    SCREEN_PLOT,
    SCREEN_MESSAGE
};

struct ScreenItem {
    bool redrawLabel, redrawText = false;
    char label[LCD_COLS + 1] = {0};
    char text[DISPLAY_VIRTUAL_COLS + 1] = {0};
    uint8_t text_pos = 0;
    bool needsRedraw() { return redrawLabel || redrawText; }
    void forceRedraw() {
        redrawLabel = true;
        redrawText = true;
    }
};

class Screen {
  private:
    ScreenItem items[DISPLAY_VIRTUAL_ROWS];
    size_t cur_line;
    size_t lines_count;

  public:
    Screen() { clear(); }
    void clear() {
        memset(items, 0, sizeof(ScreenItem) * DISPLAY_VIRTUAL_ROWS);
        cur_line = 0;
        lines_count = 0;
    }
    size_t count() { return lines_count; }

    void setCount(uint8_t value) {
        lines_count = value;
        if (cur_line > lines_count - 1)
            cur_line = 0;
    }
    void moveFirst() { cur_line = 0; }

    void next() {
        if (++cur_line + LCD_ROWS > lines_count)
            cur_line = 0;
#ifdef DEBUG_DISPLAY
        DEBUG.printf("[screen] next() = %d\r\n", cur_line);
#endif
        size_t y = cur_line;
        for (size_t n = 0; n < LCD_ROWS; ++n) {
            if (y + n > lines_count)
                y = 0;
            items[y + n].forceRedraw();
        }
    }

    void set(size_t n, const char *label, const char *text) {
        ScreenItem *item = &items[n];
        item->redrawLabel = setstr(item->label, label, LCD_COLS + 1);
        item->redrawText = setstr(item->text, text, DISPLAY_VIRTUAL_COLS + 1);
#ifdef DEBUG_DISPLAY
        DEBUG.printf("[screen] set(%d, %s, %s) = %d\r\n", n, item->label,
                     item->text, item->needsRedraw());
#endif
    }

    void set(size_t n, const char *text) { set(n, NULL, text); }

    void add(const char *label, const char *text) {
        set(lines_count++, label, text);
    }

    ScreenItem *get(uint8_t row) { return &items[cur_line + row]; }
};