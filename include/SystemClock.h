#pragma once
#include <Arduino.h>

#include "consts.h"
#include "types.h"
#include "Config.h"
#include "FileStorage.h"

class SystemClock {
   public:
    void begin();
    void loop();
    void setTime(DateTime& time);
    void setConfig(Config *config);
    void setTimeZone(sint8_t timeZone);
    void setBackupInterval(uint16_t time_s);
    void setOutput(Print *output);        
    bool isSynced();
    uint32_t getUptime();
    unsigned long getUTC();
    unsigned long getLocal();
    String getLocalFormated();
    String getUptimeFormated();
    uint8_t getWeekDay();
    uint8_t getHours();
    uint8_t getMinutes();
    uint8_t getSeconds();
   private:
    void backup();
    void restore();

    Print *output;

    FileStorage *backupAgent;

    bool active = false;
    bool synced = false;
    sint16_t timeOffset_s = 0;
    unsigned long lastUpdated_ms = 0;
    unsigned long rolloverCounter = 0;
    unsigned long epochTime_s = 0;
    unsigned long lastBackup_ms = 0;
    unsigned long backupInterval_ms = 0;
};

extern SystemClock rtc;