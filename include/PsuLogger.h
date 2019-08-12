#pragma once

#include "Psu.h"

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
    
    PsuInfo first();
    PsuInfo last();
    
    size_t getItems(PsuInfo& info);
    void getVoltages(float* voltages);

    void printFirst(Print* p, uint16_t n);
    void print(Print* p);
    void printSummary(Print* p);
    void printDiag(Print* p);
    void printLast(Print* p, uint16_t n);

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
