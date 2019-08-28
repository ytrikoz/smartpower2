#pragma once

#include "CommonTypes.h"
#include "Config.h"
#include "FileStore.h"

typedef std::function<void(const char*)> TimeEventHandler;

class SystemClock {
   private:
    bool storeEpoch(EpochTime);
    bool restoreEpoch(EpochTime&);
    bool isExpired(unsigned long);
   public:
    SystemClock();
    void setOptions(Config* config);
    void setEpoch(EpochTime& epochTime, bool trusted = false);
    void setTimeZone(sint8_t timeZone_h);
    void setBackupInterval(unsigned long time_s);
    void useFileStorage(bool enabled);
    void begin();
    void loop();
    unsigned long getUtc();
    unsigned long getLocal();
    unsigned long getSystemUptime();
    String getDateTimeStr();
    String getSystemUptimeStr();
    String getLocalTimeStr();
    void setOnTimeChange(TimeEventHandler h);
    void printDiag(Print* p);

   private:
    void onTimeChanged();
    void onTick();
    TimeEventHandler onTimeChangeEvent;
    bool useFS;
    bool active;
    bool trusted;

    EpochTime epoch;
    sint16_t timeOffset;
    unsigned long lastStored;
    unsigned long storeInterval;
    unsigned long lastUpdated;
    unsigned long leftStore;
    uint8_t rollover;
    
    Print* output = &USE_SERIAL;
    Print* err = &ERROR;
};

extern SystemClock rtc;