#pragma once

#include <string.h>

class MeasureHistory {
   public:
    MeasureHistory(size_t capacity = 8);
    ~MeasureHistory();
    bool empty();
    size_t length();
    size_t size();
    void clear();
    void insert(const char ch);
	asArray()
   private:
    char* buffer;
	size_t capacity;    
    size_t writePos;
    size_t readPos;
};