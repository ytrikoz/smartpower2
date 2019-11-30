#include "Modules/SystemClock.h"

#include "BuildConfig.h"
#include "PrintUtils.h"
#include "StoreUtils.h"
#include "TimeUtils.h"

using namespace PrintUtils;

bool SystemClock::onInit() {
    setTimeZone(config()->getValueAsByte(TIME_ZONE));
    setBackupInterval(config()->getValueAsInt(TIME_BACKUP_INTERVAL));
    EpochTime now;
    if (!restoreState(now))
        now = EpochTime(TimeUtils::getAppBuildUtc());
    setEpoch(now);
    return true;
}

size_t SystemClock::printDiag(Print *p) {
    size_t n =
        println_nameP_value(p, str_timezone, timeOffset / ONE_HOUR_s);
    n += println_nameP_value(p, str_backup, storeInterval / ONE_SECOND_ms);
    n += println_nameP_value(p, str_updated, lastUpdated / ONE_SECOND_ms);
    n += println_nameP_value(p, str_stored, lastStored / ONE_SECOND_ms);
    n += println_nameP_value(p, str_rollover, rollover);
    n += println_nameP_value(p, str_epoch, epoch);
    return n += println_nameP_value(p, str_trusted, trusted);
}

void SystemClock::setTimeZone(uint8_t timeZone) {
    timeOffset = TimeUtils::timeZoneToOffset(timeZone);
}

void SystemClock::setBackupInterval(unsigned long time) {
    storeInterval =
        time > 0 ? constrain(time, TIME_BACKUP_INTERVAL_MIN_s, ONE_DAY_s) *
                       ONE_SECOND_ms
                 : 0;
}

void SystemClock::onLoop() {
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
    bool res = StoreUtils::restoreString(FS_UTC_VAR, buf);
    if (res)
        value = EpochTime(buf.toInt());
    return res;
}

bool SystemClock::storeState(EpochTime &value) {
    String buf = value.toString();
    return StoreUtils::storeString(FS_UTC_VAR, buf);
}
