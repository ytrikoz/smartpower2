#pragma once

#include "CommonTypes.h"
#include "Modules/Psu.h"
#include "DeduplicationLog.h"

class PsuLogHelper : public PsuListener {
   public:
    void onPsuData(PsuData &item) override;

   public:
    void clear();
    void print(Print *p);
    void print(Print *p, PsuLogEnum item);

   public:
    PsuLogHelper();

   public:
    DedupLog *getLog(PsuLogEnum item);
    size_t getSize(PsuLogEnum item);
    bool getValues(PsuLogEnum log, float *values, size_t &size);

   private:
    DedupLog *log_[2] = {0};
    unsigned long startTime;
    unsigned long lastRecord;
};