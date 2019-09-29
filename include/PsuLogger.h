#pragma once

#include "CommonTypes.h"
#include "PsuLog.h"

enum PsuLogEnum { VOLTAGE_LOG, CURRENT_LOG, POWER_LOG, WATTSHOURS_LOG };

class PsuLogger {
  public:
    PsuLogger();
    bool begin();
    void end();
    void loop();
    void print(Print *p, PsuLogEnum item);
    void printDiag(Print *p);
    PsuLog *getLog(PsuLogEnum item);
    bool getLogValues(PsuLogEnum log, float *values, size_t &size);
    void log(PsuInfo &item);

  private:
    void clear();
    void log(PsuLogEnum log, unsigned long time, float voltage);

  private:
    PsuLog *psuLog[4];
    bool active;
    unsigned long startTime;
    unsigned long lastUpdated;
    bool v_enabled, i_enabled, p_enabled, wh_enabled;
};
