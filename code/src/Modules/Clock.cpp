#include "Modules/Clock.h"

#include "BuildConfig.h"
#include "TimeUtils.h"
#include "Wireless.h"

namespace Modules {

#define MIN_DATETIME 1575158400

Clock::Clock() : Module(){};

Clock::~Clock() {}

size_t Clock::onDiag(Print* p) {
    DynamicJsonDocument doc(128);
    doc[FPSTR(str_uptime)] = getUptime();
    doc[FPSTR(str_local)] = getLocal();
    doc[FPSTR(str_offset)] = getBiasInMinutes();
    doc[FPSTR(str_interval)] = getStoreInterval();
    doc[FPSTR(str_stored)] = lastKnown_;
    return serializeJson(doc, *p);
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
        PrintUtils::print_ident(out_, FPSTR(str_clock));
        if (restoreState()) {
            PrintUtils::println(out_, FPSTR(str_restored), TimeUtils::format_time(getLocal()));
            setSystemTime(epoch_);
        } else {
            PrintUtils::println(out_, FPSTR(str_not), FPSTR(str_stored));
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

time_t Clock::getUptime() {
    return uptime_ / ONE_SECOND_ms;
}

time_t Clock::getUtc() { return epoch_; };

time_t Clock::getLocal() { return epoch_ + getBiasInSeconds(); };

const char* Clock::getSntpServer() {
    return config_->getValueAsString(NTP_POOL_SERVER);
}

int Clock::getBiasInMinutes() {
    return getBiasInSeconds() / ONE_MINUTE_s;
}

int Clock::getBiasInSeconds() {
    return TimeUtils::timeZoneInSeconds(config_->getValueAsByte(TIME_ZONE));
}

time_t Clock::getStoreInterval() {
    time_t interval = config_->getValueAsInt(TIME_BACKUP_INTERVAL);
    if (interval) interval = constrain(interval, TIME_BACKUP_INTERVAL_MIN_s, ONE_DAY_s);
    return interval;
}

void Clock::onTimeChange(const time_t now, const double drift) {
    if (timeChangeHandler)
        timeChangeHandler(toLocal(now), drift);
}

time_t Clock::toLocal(time_t epoch) {
    return epoch + getBiasInSeconds();
}

bool Clock::restoreState() {
    if (FSUtils::readTime(FS_UTC_VAR, lastKnown_))
        epoch_ = lastKnown_;
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

void Clock::updateLastKnown(const time_t epoch, bool forced) {
    const time_t interval = getStoreInterval();
    if (forced || (interval && (!lastKnown_ || difftime(epoch, lastKnown_) > interval))) {
        if (FSUtils::writeTime(FS_UTC_VAR, epoch))
            lastKnown_ = epoch;
    }
}

}  // namespace Module