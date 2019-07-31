#pragma once
#include <Arduino.h>

#include "Config.h"
#include "FileStorage.h"
#include "consts.h"
#include "types.h"

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
    bool store(FileStorage *agent);
    bool restore(FileStorage *agent);
    SystemTimeChangedEvent onSystemTimeChanged;
    bool active = false;
    bool synced = false;
    sint16_t timeOffset_s;
    unsigned long backupInterval_ms;
    unsigned long lastUpdated_ms;
    unsigned long lastBackup_ms;
    unsigned long rolloverCounter;
    unsigned long epochTime_s;
    Print *output;

    struct tm dateTime;
};

extern SystemClock rtc;