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

inline TimeProfiler::TimeProfiler(const char* label, uint16_t limit) 
{
	this->label = new char[16];
	strcpy(this->label, label);
    this->limit = limit;
    this->p = &USE_SERIAL;
    start = millis();
}

inline void TimeProfiler::finish(){
    printResults(p);
}

inline void TimeProfiler::printResults(Print* p)
{

}

inline TimeProfiler::~TimeProfiler() {
	long passed = millis_since(start);	
    if (passed >= limit) {
        Serial.print(label);
        Serial.printf_P(strf_for_lu_ms, passed);
        Serial.println();
    }
	delete[] label;
}