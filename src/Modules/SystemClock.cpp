#include "Modules/SystemClock.h"

#include "BuildConfig.h"
#include "PrintUtils.h"
#include "StoreUtils.h"
#include "TimeUtils.h"

using namespace PrintUtils;

SystemClock::SystemClock() : AppModule(MOD_CLOCK) {
    trusted = false;
    timeOffset = storeInterval = lastStored = rollover = 0;
    epoch = EpochTime(TimeUtils::getAppBuildUtc());
    lastUpdated = millis();
}

bool SystemClock::begin() {
    if (!trusted)
        if (restoreState(epoch))
            setEpoch(epoch);
    return true;
}

size_t SystemClock::printDiag(Print *p) {
    size_t n = PrintUtils::print_nameP_value(p, str_timezone,
                                             (float)timeOffset / ONE_HOUR_s);
    n += PrintUtils::print_nameP_value(p, str_backup,
                                       storeInterval / ONE_SECOND_ms);
    n += PrintUtils::print_nameP_value(p, str_updated,
                                       lastUpdated / ONE_SECOND_ms);
    n += PrintUtils::print_nameP_value(p, str_stored,
                                       lastStored / ONE_SECOND_ms);
    n += PrintUtils::print_nameP_value(p, str_rollover, rollover);
    n += PrintUtils::print_nameP_value(p, str_epoch, epoch);
    n += PrintUtils::print_nameP_value(p, str_trusted, trusted);
    return n;
}

void SystemClock::setConfig(Config *config) {
    setTimeZone(config->getValueAsByte(TIME_ZONE));
    setBackupInterval(config->getValueAsInt(TIME_BACKUP_INTERVAL));
}

void SystemClock::setTimeZone(uint8_t timeZone) {
    timeOffset = TimeUtils::getTimeOffset(timeZone);
}

void SystemClock::setBackupInterval(unsigned long time) {
    storeInterval =
        time > 0 ? constrain(time, TIME_BACKUP_INTERVAL_MIN_s, ONE_DAY_s) *
                       ONE_SECOND_ms
                 : 0;
}

void SystemClock::loop() {
    unsigned long now = millis();
    if (now < lastUpdated)
        rollover++;
    while (millis_passed(lastUpdated, now) >= ONE_SECOND_ms) {
        epoch.tick();
        lastUpdated += ONE_SECOND_ms;
    }
    if (trusted) {
        if (isStoreNeedsUpdate(now)) {
            storeState(epoch);
            lastStored = now;
        }
    }
}

void SystemClock::setEpoch(const EpochTime &epoch, bool trusted) {
    if (this->trusted && !trusted)
        return;

    this->epoch = epoch;
    this->trusted = trusted;

    onTimeChange();
}

void SystemClock::onTimeChange() {
    String time = epoch.toString();
    if (trusted)
        say_strP(str_time, time.c_str());
}

unsigned long SystemClock::getUptime() {
    return (0xFFFFFFFF / ONE_SECOND_ms) * rollover + (millis() / ONE_SECOND_ms);
}

unsigned long SystemClock::getUtc() { return epoch.toEpoch(); }

unsigned long SystemClock::getLocal() { return getUtc() + timeOffset; }

bool SystemClock::isStoreNeedsUpdate(unsigned long now) {
    return storeInterval > 0 &&
           (!lastStored || millis_passed(lastStored, now) >= storeInterval);
}

bool SystemClock::restoreState(EpochTime &value) {
    String buf;
    bool res = StoreUtils::restoreString(FILE_VAR_UTC, buf);
    if (res)
        value = EpochTime(buf.toInt());
    return res;
}

bool SystemClock::storeState(EpochTime &value) {
    String buf = value.toString();
    return StoreUtils::storeString(FILE_VAR_UTC, buf);
}
