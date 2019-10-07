#pragma once

#include "CommonTypes.h"
#include "PsuLog.h"

enum PsuLogEnum { VOLTAGE_LOG, CURRENT_LOG, POWER_LOG, WATTSHOURS_LOG };

class PsuLogger {
  public:
    PsuLogger();
    void clear();
    void printDiag(Print *p);

  public:
    void print(PsuLogEnum item, Print *p);
    PsuLog *getLog(PsuLogEnum item);
    size_t getSize(PsuLogEnum item);
    bool getValues(PsuLogEnum log, float *values, size_t &size);
    void log(PsuInfo &item);

  private:
    void log(PsuLogEnum log, unsigned long time, float voltage);

  private:
    PsuLog *psuLog[4];
    unsigned long startTime;
    unsigned long lastUpdated;
    bool v_enabled, i_enabled, p_enabled, wh_enabled;
};
