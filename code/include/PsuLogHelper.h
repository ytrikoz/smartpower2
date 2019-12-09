#pragma once

#include "CommonTypes.h"
#include "Modules/PsuModule.h"
#include "DeduplicationLog.h"


class PsuLogHelper : PsuListener {
  public:
    void log(PsuData &item) override;
    void clear();
    void print(Print *p);
    void print(Print *p, PsuLogEnum item);
  public:
    PsuLogHelper();
  public:
    DeduplicationLog *getLog(PsuLogEnum item);
    size_t getSize(PsuLogEnum item);
    bool getValues(PsuLogEnum log, float *values, size_t &size);

  private:
    DeduplicationLog* log_[2] = {0};
    unsigned long startTime;
    unsigned long lastRecord;
};
