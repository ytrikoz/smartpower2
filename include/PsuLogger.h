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
    
    size_t first();
    size_t last();
    size_t next(size_t pos);
    size_t prev(size_t pos);

    size_t getItems(PsuInfo& info);
    void getVoltages(float* voltages);
    
    void print(Print* p);
    void printFirst(Print* p, size_t n);
    void printLast(Print* p, size_t n);
    void printSummary(Print* p);
    void printDiag(Print* p);

   private:
    void printItem(Print* p, size_t index);
    void add(PsuInfo item);
    void logVoltage(unsigned long time, float voltage);
    Psu* psu;
    LogItem* voltage_log;
    PsuInfo* items;
    unsigned long lastUpdated;
    size_t capacity;
    size_t writePos;
    bool rotated;
    bool active;
};
