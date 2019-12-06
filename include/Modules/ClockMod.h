#pragma once

#include "AppModule.h"
#include "Config.h"
#include "Storage.h"
#include "TimeUtils.h"
#include <time.h>

class ClockMod : public AppModule {
   public:
    ClockMod();
    ~ClockMod();

   public:
    void setOnChange(TimeChangeEvent);
    
    time_t getLocal();
    time_t getUtc();
    time_t getUptime();
   public:
    size_t onDiag(Print *p) override;

   protected:
    bool onInit() override;
    bool onStart() override;
    void onLoop() override; 

   private:
    time_t toLocal(time_t epoch);
    time_t getSystemTime();
    void setSystemTime(const time_t);
    bool restoreState();
    void onTimeChange(const time_t);
    void setSntp();
    bool isTimeSet();
    const timezone getTimeZone();
    const char *getSntpServer();
    int getBiasInSeconds();
    int getBiasInMinutes();
    time_t getStoreInterval();
    time_t getStored();
    
    bool writeStored(time_t);
    bool readStored(time_t&);
    void updateStored(const time_t epoch, bool forced = false);
   private:
    // ms    
    TimeChangeEvent timeChangeHandler;
    unsigned long uptime_;
    time_t epoch_;
    time_t stored_;
};
