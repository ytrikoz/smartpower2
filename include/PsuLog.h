#pragma once

#include "Psu.h"

class PsuLog {
   public:
    PsuLog(const char* label, size_t size);
    ~PsuLog();
    void push(unsigned long time, float value);
    char* label();
    void clear();
    bool available();
    size_t free();
    size_t size();
    size_t first();
    size_t last();
    size_t count();
    unsigned long duration();
    LogItem* get(size_t pos);
    LogItem* getPrev(size_t pos);
    LogItem* getNext(size_t pos);
    LogItem* getFirst();
    LogItem* getLast();
    void values(float* values, size_t& size);
    void printTo(Print* p);
    void printFirst(Print* p, size_t n);
    void printLast(Print* p, size_t n);
    void printDiag(Print* p);

   protected:
    void calcStat(float value);
    float value_min, value_max, value_avg;
   private:
    void print(Print* p, size_t n, float value);
    size_t next(size_t pos);
    size_t prev(size_t pos);
    unsigned long lastTime;
    size_t counter;
    size_t capacity;
    size_t writePos;
    char* name;
    LogItem* items;
    bool rotated;
    bool active;
};