#pragma once

#include <string.h>
#include "Types.h"

class MeasureLogger {
   public:
    MeasureLogger(size_t capacity = 8);
    ~MeasureLogger();
    bool empty();
    size_t length();
    size_t size();
    void clear();
    void add(PSUInfo m);

   private:
    size_t capacity;
    PSUInfo *array;
    size_t writePos;
    size_t readPos;
};

inline MeasureLogger::MeasureLogger(size_t capacity) {
    this->capacity = capacity;
    array = new PSUInfo[capacity]();
    clear();
}

inline MeasureLogger::~MeasureLogger() { delete[] array; }

inline void MeasureLogger::clear() {
    writePos = 0;
    readPos = 0;
}

inline void MeasureLogger::add(PSUInfo m) {
    if (writePos == capacity) writePos = 0;
    array[writePos] = PSUInfo(m);
    writePos++;
}

inline bool MeasureLogger::empty() { return (writePos == 0); }


inline size_t MeasureLogger::size() { return writePos; };