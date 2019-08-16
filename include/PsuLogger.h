#pragma once

#include "Psu.h"

class PsuLogger {
   public:
    PsuLogger(Psu* psu, size_t capacity);
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
    
    void print(Print* p);
    void printFirst(Print* p, size_t n);
    void printLast(Print* p, size_t n);
    void printSummary(Print* p);
    void printDiag(Print* p);

   private:
    void add(PsuInfo item, unsigned long time_ms);
    void logVoltage(unsigned long time, float voltage);
    Psu* psu;
    LogItem* voltage_log;
    PsuInfo* items;
    size_t capacity;
    size_t writePos;
    size_t readPos;
    unsigned long lastTime;
    unsigned long lastItem;
    bool rotated;
    bool active;
};
