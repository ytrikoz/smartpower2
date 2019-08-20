#pragma once
#include <Arduino.h>

#include "CommonTypes.h"
#include "Config.h"
#include "Consts.h"
#include "FileStorage.h"

typedef std::function<void(const char *)> TimeChangedEvent;

class SystemClock {
   public:
    SystemClock();
    void begin();
    void loop();
    void setTime(unsigned long epoch_s);
    void setConfig(Config *config);
    void setTimeZone(sint8_t timeZone);
    void setBackupInterval(uint16_t time_s);
    void setOutput(Print *output);
    void setOnTimeChanged(TimeChangedEvent);
    bool isSynced();
    uint32_t getUptimeEpoch();
    unsigned long getUtcEpoch();
    unsigned long getLocalEpoch();
    String getLocalFormated();
    String getUptimeFormated();
    uint8_t getWeekDay();
    uint8_t getHours();
    uint8_t getMinutes();
    uint8_t getSeconds();

    tm getDateTime();

   private:
    bool store();
    bool restore();
    TimeChangedEvent onTimeChanged;

    bool active = false;
    bool synced = false;
    unsigned long utcEpochTime_s;
    sint16_t timeOffset_s;
    unsigned long backupInterval;
    unsigned long lastUpdated;
    unsigned long lastBackup_ms;    
    uint8_t rollover;
    FileStorage *storage;
    Print *output;
};

extern SystemClock rtc;