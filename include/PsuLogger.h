#pragma once

#include "Psu.h"
#include <string.h>
#include "Types.h"
#include "consts.h"

class PsuLogger {
   public:
    PsuLogger(Psu* provider, size_t capacity);
    ~PsuLogger();
    bool empty();
    size_t length();
    size_t size();
    void begin();
    void end();
    void clear();
    void loop();
    void printFirst(uint16_t n);
    void printLast(uint16_t n);
    void printSummary();
    size_t getItems(PsuInfo& info);
    void getVoltages(float* voltages);
    void printDiag(Print* p);
   private:
    void add(PsuInfo item, unsigned long time_ms);
    Psu* provider;
    PsuInfo* items;
    uint16_t capacity;
    uint16_t writePos;
    uint16_t readPos;
    unsigned long lastTime;
    unsigned long lastItem;
    bool full;
    bool active;
};
