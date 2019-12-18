#pragma once

#include "Core/Module.h"

#include <time.h>

#include "Config.h"
#include "Core/Storage.h"
#include "Utils/TimeUtils.h"

namespace Modules {

typedef std::function<void(const time_t local, double diff)> TimeChangeEvent;

class Clock : public Module {
   public:
    Clock();
    ~Clock();

   public:
    void setOnChange(TimeChangeEvent);

    time_t getLocal();
    time_t getUtc();
    time_t getUptime();

   public:
    void onDiag(const JsonObject& doc) override;

   protected:
    bool onInit() override;
    bool onStart() override;
    void onLoop() override;

   private:
    time_t toLocal(time_t epoch);
    time_t getSystemTime();
    void setSystemTime(const time_t);
    bool restoreState();
    void onTimeChange(const time_t, const double drift);
    void setSntp();
    bool isTimeSet();
    const timezone getTimeZone();
    const char* getSntpServer();
    int getBiasInSeconds();
    int getBiasInMinutes();
    time_t getStoreInterval();

    void updateLastKnown(const time_t epoch, bool forced = false);

   private:
    TimeChangeEvent timeChangeHandler;
    unsigned long uptime_;
    time_t epoch_;
    time_t lastKnown_;
};

}  // namespace Modules