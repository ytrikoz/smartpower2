#pragma once

#include "DeduplicationLog.h"

class DiffLog : public DeduplicationLog {
   public:
    DiffLog(const char* label, size_t size, float base);
    // PsuLog
   public:
    void decode(size_t pos, float &value) {        
   
    }
    void encode(size_t pos, float value) {
    
    }
    private:
        float base;    
};
