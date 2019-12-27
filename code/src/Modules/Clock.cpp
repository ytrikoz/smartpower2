#include "Modules/Clock.h"

#include "BuildConfig.h"
#include "Utils/TimeUtils.h"
#include "Wireless.h"

namespace Modules {

#define MIN_DATETIME 1575158400

Clock::Clock() : Module(){};

Clock::~Clock() {}

void Clock::onDiag(const JsonObject& doc) {
    doc[FPSTR(str_uptime)] = uptime();
    doc[FPSTR(str_local)] = local();
    doc[FPSTR(str_stored)] = lastKnown_;
}

bool Clock::isTimeSet() {
    return getSystemTime() > MIN_DATETIME;
}

bool Clock::onInit() {
    epoch_ = 0;
    lastKnown_ = 0;
    uptime_ = 0;
    return true;
};

bool Clock::onStart() {
    if (!isTimeSet()) {
        if (restoreState()) {        
            setSystemTime(epoch_);
        }
    }
    setSntp();
    return true;
}

void Clock::onLoop() {
    // uptime
    unsigned long passed = millis_since(uptime_);
    if (passed < ONE_SECOND_ms)
        return;
    uptime_ += passed;
    // world time
    time_t now = getSystemTime();
    time_t estimated = epoch_ + (passed / ONE_SECOND_ms);
    double drift = difftime(now, estimated);
    if (drift > 1) {
        onTimeChange(now, drift);
    }
    updateLastKnown(now, drift > ONE_MINUTE_s);
    epoch_ = now;
}

void Clock::setSntp() {
    const char* smtpServer = getSntpServer();
    int timeOffset = getBiasInMinutes();
    int daylightOffset = 0;
    if (smtpServer)
        configTime(timeOffset, daylightOffset, smtpServer, "pool.ntp.org", "time.nist.gov");
    else
        configTime(timeOffset, daylightOffset, "pool.ntp.org", "time.nist.gov");
}

void Clock::setOnChange(TimeChangeEvent h) {
    timeChangeHandler = h;
}

 tm* Clock::now() {
    time_t local_ = local();
    time_ = localtime(&local_);
    return time_;
}

String Clock::timeStr() {
    char buf[32];
    strftime(buf, 32, "%T", now());
    return buf;
}

time_t Clock::uptime() const {
    return uptime_ / ONE_SECOND_ms;
}

time_t Clock::utc() const{ 
    return epoch_; 
};

time_t Clock::local() const { 
    return epoch_ + getBiasInSeconds(); };

const char* Clock::getSntpServer() {
    return config_->value(NTP_POOL_SERVER);
}

int Clock::getBiasInMinutes() const {
    return getBiasInSeconds() / ONE_MINUTE_s;
}

int Clock::getBiasInSeconds() const {
    return TimeUtils::timeZoneInSeconds(config_->asByte(TIME_ZONE));
}

time_t Clock::getStoreInterval() {
    time_t interval = config_->asInt(TIME_BACKUP_INTERVAL);
    if (interval) interval = constrain(interval, TIME_BACKUP_INTERVAL_MIN_s, ONE_DAY_s);
    return interval;
}

void Clock::onTimeChange(const time_t now, const double drift) {
    if (timeChangeHandler)
        timeChangeHandler(toLocal(now), drift);
}

time_t Clock::toLocal(time_t local) {
    return local + getBiasInSeconds();
}

bool Clock::restoreState() {
    if (FSUtils::readTime(FS_UTC_VAR, lastKnown_)) {
        epoch_ = lastKnown_;
        PrintUtils::print_ident(out_, FPSTR(str_clock));
        PrintUtils::print(out_, TimeUtils::format_time(local()));
        PrintUtils::println(out_);
    }
    else
        lastKnown_ = 0;
    return lastKnown_;
}

const timezone Clock::getTimeZone() {
    return timezone{getBiasInMinutes(), 0};
}

void Clock::setSystemTime(const time_t epoch) {
    timeval tv{epoch, 0};
    timezone tz = getTimeZone();
    settimeofday(&tv, &tz);
}

time_t Clock::getSystemTime() {
    timeval tv{0, 0};
    timezone tz = getTimeZone();
    time_t epoch = 0;
    if (gettimeofday(&tv, &tz) != -1)
        epoch = tv.tv_sec;
    return epoch;
}

void Clock::updateLastKnown(const time_t time, bool forced) {
    const time_t interval = getStoreInterval();
    if (forced || (interval && (!lastKnown_ || difftime(time, lastKnown_) > interval))) {
        if (FSUtils::writeTime(FS_UTC_VAR, time))
            lastKnown_ = time;
    }
}

}  // namespace Module