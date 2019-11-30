#pragma once

#include "Arduino.h"

enum class PsuLogEnum : uint8_t {
    VOLTAGE = 0x0,
    CURRENT = 0x1,
    POWER = 0x2,
    WATTSHOURS = 0x3
};

struct LogItem {
    size_t n;
    uint16_t v;
    LogItem() {
        n = 0;
        v = 0;
    }
    LogItem(size_t n, uint16_t v) {
        this->n = n;
        this->v = v;
    }
    float revert(uint16_t value) { return (float)value / 1000; }

    uint16_t convert(float value) { return floor(value * 1000); }
};

class PsuLog {
   public:
    void log(unsigned long time, float value);
    void values(float array[], size_t &size);

   public:
    PsuLog(const char *label, size_t size);

    void printTo(Print *p);
    void printFirst(Print *p, size_t n);
    void printLast(Print *p, size_t n);
    void printDiag(Print *p);
    void clear();
    size_t count();

   private:
    LogItem *getEntry();
    LogItem *getPrevEntry();
    LogItem *getFirstEntry();
    LogItem *getLastEntry();

   private:
    LogItem *getEntry(size_t pos);
    LogItem *getPrevEntry(size_t pos);
    LogItem *getNextEntry(size_t pos);

   private:
    size_t getEntryIndex(size_t pos);
    size_t getPrevEntryIndex(size_t pos);
    size_t getNextEntryIndex(size_t pos);
    size_t getFirstEntryIndex();
    size_t getLastEntryIndex();

   private:
    void updateStats(const float value, const size_t cnt);
    void print(Print *p, size_t n, float value);

   private:
    char name[8];
    float min, max, avg;
    unsigned long lastTime;
    size_t counter, capacity, writePos;
    LogItem *items;
};
