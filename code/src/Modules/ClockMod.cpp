#include "Modules/ClockMod.h"

#include "BuildConfig.h"
#include "PrintUtils.h"
#include "StoreUtils.h"
#include "TimeUtils.h"
#include "Wireless.h"

using namespace PrintUtils;

#define MIN_DATETIME 1575158400
#define FORCED true

ClockMod::ClockMod() : AppModule(MOD_CLOCK){};

ClockMod::~ClockMod() {}

size_t ClockMod::onDiag(Print* p) {
    DynamicJsonDocument doc(256);

    doc[FPSTR(str_offset)] = getBiasInMinutes();
    doc[FPSTR(str_interval)] = getStoreInterval();
    doc[FPSTR(str_stored)] = getStored();
    doc[FPSTR(str_uptime)] = getUptime();
    doc[FPSTR(str_local)] = getLocal();

    return serializeJsonPretty(doc, *p);
}

bool ClockMod::isTimeSet() {
    return getSystemTime() > MIN_DATETIME;
}

bool ClockMod::onInit() {
    epoch_ = 0;
    stored_ = 0;
    uptime_ = millis();
    return true;
};

bool ClockMod::onStart() {
    if (!isTimeSet()) {
        print_ident(out, FPSTR(str_clock));
        if (restoreState()) {
            println(out, FPSTR(str_restored), TimeUtils::format_time(getLocal()));
            setSystemTime(epoch_);
        } else {
            println(out, FPSTR(str_not), FPSTR(str_stored));
        }
    }
    setSntp();
    return true;
}

void ClockMod::setOnChange(TimeChangeEvent h) {
    timeChangeHandler = h;
}

void ClockMod::setSntp() {
    const char* smtpServer = getSntpServer();
    int timeOffset = getBiasInMinutes();
    int daylightOffset = 0;
    if (smtpServer)
        configTime(timeOffset, daylightOffset, smtpServer);
    else
        configTime(timeOffset, daylightOffset, "pool.ntp.org", "time.nist.gov");
}

time_t ClockMod::getUptime() {
    return uptime_ / ONE_SECOND_ms;
}

time_t ClockMod::getUtc() { return epoch_; };

time_t ClockMod::getLocal() { return epoch_ + getBiasInSeconds(); };

const char* ClockMod::getSntpServer() {
    return config_->getValueAsString(NTP_POOL_SERVER);
}

int ClockMod::getBiasInMinutes() {
    return getBiasInSeconds() / ONE_MINUTE_s;
}

int ClockMod::getBiasInSeconds() {
    return TimeUtils::timeZoneInSeconds(config_->getValueAsByte(TIME_ZONE));
}

time_t ClockMod::getStoreInterval() {
    time_t interval = config_->getValueAsInt(TIME_BACKUP_INTERVAL);
    if (interval) interval = constrain(interval, TIME_BACKUP_INTERVAL_MIN_s, ONE_DAY_s);
    return interval;
}

void ClockMod::onLoop() {
    // uptime
    unsigned long passed = millis_since(uptime_);
    if (passed < ONE_SECOND_ms)
        return;
    uptime_ += passed;

    // world time
    time_t now = getSystemTime();
    time_t estimated = epoch_ + (passed / 1000);
    bool drift = difftime(now, estimated);
    if (drift) onTimeChange(now);        
    updateStored(now, drift);
    epoch_ = now;
}

void ClockMod::onTimeChange(const time_t now) {
    if (timeChangeHandler)
        timeChangeHandler(toLocal(now), difftime(now, epoch_));
}

time_t ClockMod::toLocal(time_t epoch) {
    return epoch + getBiasInSeconds();
}

bool ClockMod::restoreState() {
    if (readStored(stored_))
        epoch_ = stored_;
    else
        stored_ = 0;
    return stored_;
}

const timezone ClockMod::getTimeZone() {
    return timezone{getBiasInMinutes(), 0};
}

void ClockMod::setSystemTime(const time_t epoch) {
    timeval tv{epoch, 0};
    timezone tz = getTimeZone();
    settimeofday(&tv, &tz);
}

time_t ClockMod::getSystemTime() {
    timeval tv{0, 0};
    timezone tz = getTimeZone();
    time_t epoch = 0;
    if (gettimeofday(&tv, &tz) != -1)
        epoch = tv.tv_sec;
    return epoch;
}

time_t ClockMod::getStored() {
    return stored_;
}

void ClockMod::updateStored(const time_t epoch, bool forced) {
    const time_t interval = getStoreInterval();
    const time_t stored = getStored();
    if (forced || (interval && (!stored || difftime(epoch, stored) > interval))) {
        String buf = String((long)epoch);
        if (StoreUtils::storeString(FS_UTC_VAR, buf))
            stored_ = epoch;
    }
}

bool ClockMod::writeStored(time_t epoch) {
    return StoreUtils::storeInt(FS_UTC_VAR, epoch);
}

bool ClockMod::readStored(time_t& epoch) {
    return StoreUtils::restoreInt(FS_UTC_VAR, epoch);
}
