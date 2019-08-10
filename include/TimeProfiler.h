#pragma once

#include <Arduino.h>

class TimeProfiler {
   public:
    TimeProfiler(const char* label, uint16_t limit = 16);
    ~TimeProfiler();
    void finish();
   private:
    void print(unsigned long time);
    Print* p;
    char* label;
    uint16_t limit;
    unsigned long start;
    unsigned long total;
};

inline void TimeProfiler::finish() {  
    long time =  micros() - start;
    if (((limit > 0) && (time > limit)) || (limit == 0)) print(time);
}

inline TimeProfiler::TimeProfiler(const char* label, uint16_t limit) {
    this->label = new char[16];
    strcpy(this->label, label);
    this->limit = limit;
    // default
    this->p = &USE_SERIAL;
    start = micros();
}

inline TimeProfiler::~TimeProfiler() {
    finish();
    delete[] label;
}

inline void TimeProfiler::print(unsigned long time) {
    p->print(label);
    p->print(' ');
    p->print(time);
    p->println();
}