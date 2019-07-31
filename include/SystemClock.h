#pragma once
#include <Arduino.h>

#include "consts.h"
#include "types.h"
#include "Config.h"
#include "FileStorage.h"

class SystemClock {
   public:
    SystemClock();
    void begin();
    void loop();
    void setTime(EpochTime &epoch);
    void setConfig(Config *config);
    void setTimeZone(sint8_t timeZone);
    void setBackupInterval(uint16_t time_s);
    void setOutput(Print *output);        
    bool isSynced();
    uint32_t getSystemUptime();
    unsigned long getUtcEpoch();
    unsigned long getLocalEpoch();
    String getLocalTimeFormated();
    String getUptimeFormated();
    uint8_t getWeekDay();
    uint8_t getHours();
    uint8_t getMinutes();
    uint8_t getSeconds();
   private:
    bool store(FileStorage *agent);
    bool restore(FileStorage *agent);    

    bool active = false;
    bool synced = false;
    sint16_t timeOffset_s;
    unsigned long backupInterval_ms;
    unsigned long lastUpdated_ms;
    unsigned long lastBackup_ms;
    unsigned long rolloverCounter;
    unsigned long epochTime_s;
    Print *output;
};

extern SystemClock rtc;