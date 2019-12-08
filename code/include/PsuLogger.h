#pragma once

#include "CommonTypes.h"
#include "Modules/PsuModule.h"
#include "PsuLog.h"

class MemoryPsuLogger : PsuLogger {
  public:
    MemoryPsuLogger();
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
    unsigned long lastRecord;
};
