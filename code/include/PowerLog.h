#pragma once

#include "CommonTypes.h"
#include "Modules/Psu.h"
#include "Core/DedupLog.h"

class PowerLog : public PsuDataListener {
   public:
    void onPsuData(PsuData &item) override;

   public:
    void clear();
    void print(Print *p);
    void print(Print *p, PsuLogEnum item);

   public:
    PowerLog();

   public:
    DedupLog *getLog(PsuLogEnum item);
    size_t getSize(PsuLogEnum item);
    bool fill(PsuLogEnum log, float *values, size_t &size);

   private:
    DedupLog *log_[2] = {0};
    unsigned long startTime;
    unsigned long lastRecord;
};