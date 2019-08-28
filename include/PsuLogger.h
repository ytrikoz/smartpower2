#pragma once

#include "Psu.h"
#include "PsuLog.h"

class PsuLogger {
   public:
    PsuLogger(Psu* psu);
    void begin();
    void end();
    void loop();
    PsuLog* getLog(PsuLogItem param);    
    void fill(PsuLogItem log, float* array, size_t& cnt);
    void print(Print* p, PsuLogItem log);
    void printDiag(Print* p);
   private:
    void clear();
    void log(PsuInfo& item);
    void log(PsuLogItem log, unsigned long time, float voltage);
    Psu* psu;
    bool active;
    unsigned long startTime;
    unsigned long lastUpdated;
    PsuLog* psuLog[4];
    bool v_enabled, i_enabled,  p_enabled, wh_enabled;
};
