#pragma once

#include "Arduino.h"

struct LogItem {
    size_t n;
    uint16_t value;
    LogItem() : n(0), value(0){};
    LogItem(size_t n, uint16_t value) : n(n), value(value){};
};

class PsuLog {
   public:
    virtual void push(unsigned long time, float value);
    virtual void values(float array[], size_t& array_size);   
   public:
    PsuLog(const char* label, size_t size);
    ~PsuLog();
    void clear();
    bool available();
    size_t free();
    size_t size();
    size_t first();
    size_t last();
    size_t count();

    LogItem* getFirst();
    LogItem* getLast();    
    LogItem* getPrev();
    LogItem* getItem(size_t pos);
    LogItem* getPrev(size_t pos);
    LogItem* getNext(size_t pos);
    void printTo(Print* p);
    void printFirst(Print* p, size_t n);
    void printLast(Print* p, size_t n);
    void printDiag(Print* p);
   protected:
    virtual uint16_t convert(float value);
    virtual float revert(uint16_t value);
    virtual void pushItem(const size_t n, const float val);
    LogItem* getItem();
    size_t getItemIndex(size_t pos);
    virtual size_t write(const size_t n, const float val);
    void calcMinMaxAvg(const float value, const size_t cnt);
    void print(Print* p, size_t n, float value);
    size_t next(size_t pos);
    size_t prev(size_t pos);
    private:
    float value_min, value_max, value_avg;
    unsigned long lastTime;
    size_t counter;
    size_t capacity;
    size_t writePos;
    LogItem* items;
    private:
    char* name;

};
