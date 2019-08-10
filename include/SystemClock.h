#pragma once
#include <Arduino.h>

#include "CommonTypes.h"
#include "Config.h"
#include "Consts.h"
#include "FileStorage.h"

typedef std::function<void(const char *)> SystemTimeChangedEvent;

class SystemClock {
   public:
    SystemClock();
    void setOnSystemTimeChanged(SystemTimeChangedEvent);
    void begin();
    void loop();
    void setTime(unsigned long epoch_s);
    void setConfig(Config *config);
    void setTimeZone(sint8_t timeZone);
    void setBackupInterval(uint16_t time_s);
    void setOutput(Print *output);
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
    SystemTimeChangedEvent onSystemTimeChanged;

    bool active = false;
    bool synced = false;
    unsigned long utcEpochTime_s;
    sint16_t timeOffset_s;
    unsigned long backupInterval_ms;
    unsigned long lastUpdated_ms;
    unsigned long lastBackup_ms;
    unsigned long rolloverCounter;
    FileStorage *storage;
    Print *output;
};

extern SystemClock rtc;