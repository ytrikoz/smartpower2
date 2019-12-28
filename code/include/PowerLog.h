#pragma once

#include "CommonTypes.h"
#include "Modules/Psu.h"
#include "Core/DedupLog.h"

enum PowerLogEnum {
    VOLTAGE = 0,
    CURRENT = 1,
};

class PowerLog : public PsuDataListener {
   public:
    void onPsuData(PsuData &item) override;

   public:
    void clear();
    void print(Print *p);
    void print(Print *p, PowerLogEnum item);

   public:
    PowerLog();

   public:
    DedupLog *getLog(PowerLogEnum item);
    size_t getSize(PowerLogEnum item);
    bool fill(PowerLogEnum log, float *values, size_t &size);

   private:
    DedupLog *log_[2] = {0};
    unsigned long startTime;
    unsigned long lastRecord_;
};