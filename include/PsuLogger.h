#pragma once

#include "Psu.h"
#include "PsuLog.h"

class PsuLogger : public AppModule {
   public:
    PsuLogger(Psu* psu);
    bool begin();
    void end();
    void loop();        
    void print(Print* p, PsuLogItem log);
    void printDiag(Print* p);
    PsuLog* getLog(PsuLogItem log);    
    bool getLogValues(PsuLogItem log, float* values, size_t& size);
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
