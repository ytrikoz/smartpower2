#pragma once

#include "AppModule.h"
#include "Config.h"
#include "FileStore.h"
#include "TimeUtils.h"

class SystemClock : public AppModule {
  public:
    SystemClock();
    void setConfig(Config *config);
    bool begin();
    void loop();
    size_t printDiag(Print *p);

  public:
    void setOnChange(TimeEventHandler);
    void setEpoch(const EpochTime &epochTime, bool trusted = false);
    unsigned long getUptime();
    unsigned long getUtc();
    unsigned long getLocal();

  private:
    void setTimeZone(sint8_t timeZone_h);
    void setBackupInterval(unsigned long time_s);

    bool isStoreNeedsUpdate(unsigned long);
    bool restoreState(EpochTime &value);
    bool storeState(EpochTime &value);

    void onTimeChange();

  private:
    TimeEventHandler timeHandler;

    bool trusted;
    EpochTime epoch;
    uint8_t rollover;

    sint16_t timeOffset;
    unsigned long storeInterval;

    unsigned long lastStored;
    unsigned long leftStore;
    unsigned long lastUpdated;
};
