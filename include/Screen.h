#pragma once

#include "Arduino.h"
#include "Consts.h"
#include "StrUtils.h"

#define DISPLAY_VIRTUAL_COLS 63
#define DISPLAY_VIRTUAL_ROWS 6

using namespace StrUtils;

enum ScreenEnum {
    SCREEN_CLEAR,
    SCREEN_PROGRESS,
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
    size_t items_pos;
    size_t items_size;

  public:
    Screen() { clear(); }
    void clear() {
        memset(items, 0, sizeof(ScreenItem) * DISPLAY_VIRTUAL_ROWS);
        items_pos = 0;
        items_size = 0;
    }
    size_t size() { return items_size; }

    void setSize(uint8_t size) { items_size = size; }

    void next() {
        if (++items_pos + LCD_ROWS > items_size) {
            items_pos = 0;
        }
#ifdef DEBUG_DISPLAY
        DEBUG.printf("[screen] next() = %d\r\n", item_pos);
#endif
        size_t pos = items_pos;
        for (size_t n = 0; n < LCD_ROWS; ++n) {
            if (pos + n > items_size)
                pos = 0;
            items[pos + n].forceRedraw();
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
        size_t n = items_size++;
        set(n, label, text);
    }

    ScreenItem *get(uint8_t row) { return &items[items_pos + row]; }
};