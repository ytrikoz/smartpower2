#pragma once

#include <Arduino.h>

class TimeProfiler {
   public:
    TimeProfiler(const char* label, uint16_t limit = 16);
    ~TimeProfiler();
    void finish();
    void printResults(Print*);
   private:
    Print* p;
    char* label;
    uint16_t limit;
    unsigned long start;
	unsigned long total;
};



inline void TimeProfiler::finish(){
    printResults(p);
}

inline void TimeProfiler::printResults(Print* p)
{

}

inline TimeProfiler::TimeProfiler(const char* label, uint16_t limit) 
{
	this->label = new char[16];
	strcpy(this->label, label);
    this->limit = limit;
    // default
    this->p = &USE_SERIAL;
    start = millis();
}

inline TimeProfiler::~TimeProfiler() {
	long time = millis_since(start);	
    if (time > limit) {
        p->print(label);
        p->print(' ');
        p->printf_P(strf_lu_ms, time);
        p->println();
    }
	delete[] label;
}