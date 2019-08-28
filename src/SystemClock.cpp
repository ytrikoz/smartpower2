#include "SystemClock.h"

#include "BuildConfig.h"
#include "StoreUtils.h"
#include "TimeUtils.h"

SystemClock rtc;

bool SystemClock::isExpired(unsigned long now) {
    return millis_passed(lastStored, now) >= storeInterval;
}

bool SystemClock::restoreEpoch(EpochTime& value) {
    String buf;
    bool res = StoreUtils::restoreString(FILE_VAR_UTC, buf);
    if (res) value = EpochTime(buf.toInt());
    return res;
}

bool SystemClock::storeEpoch(EpochTime value) {
    String buf = value.toString();
    return StoreUtils::storeString(FILE_VAR_UTC, buf);
}

SystemClock::SystemClock() {
    timeOffset = 0;
    storeInterval = 0;
    lastStored = 0;
    rollover = 0;

    lastUpdated = millis();
    epoch = EpochTime();
    trusted = false;
}

void SystemClock::printDiag(Print* p) {
    p->print(getStrP(str_time));
    p->print(getStrP(str_off, false) + getStrP(str_set));
    p->println(timeOffset);

    p->print(getStrP(str_backup) + getStrP(str_interval));
    p->println(storeInterval);

    p->print(getStrP(str_last) + getStrP(str_update));
    p->println(lastUpdated);

    p->print(getStrP(str_last) + getStrP(str_backup));
    p->println(lastStored);

    p->print(getStrP(str_rollover));
    p->println(rollover);

    p->print(getStrP(str_epoch));
    p->println(epoch);

    p->print(getStrP(str_trusted));
    p->println(getBoolStr(trusted, false));
}

void SystemClock::begin() {
    output->print(getIdentStrP(str_clock));
    output->println(getStrP(str_start));
    if (!trusted) {
        if (restoreEpoch(epoch)) setEpoch(epoch);
    }
}

void SystemClock::setOptions(Config* config) {
    setTimeZone(config->getValueAsSignedByte(TIME_ZONE));
    setBackupInterval(config->getValueAsInt(TIME_BACKUP_INTERVAL));
}

void SystemClock::setTimeZone(const sint8_t timeZone_h) {
    timeOffset = constrain(timeZone_h, -12, +12) * ONE_HOUR_s;
}

void SystemClock::setBackupInterval(unsigned long time) {
    storeInterval =
        time > 0 ? constrain(time, TIME_BACKUP_INTERVAL_MIN_s, ONE_DAY_s) *
                       ONE_SECOND_ms
                 : time;
}

void SystemClock::setOnTimeChange(TimeEventHandler h) { onTimeChangeEvent = h; }

void SystemClock::loop() {
    unsigned long now = millis();

    if (now < lastUpdated) rollover++;

    while (millis_passed(lastUpdated, now) >= ONE_SECOND_ms) {
        epoch.tick();
        lastUpdated += ONE_SECOND_ms;
    }
    if (!trusted) return;

    if (isExpired(now) || !lastStored) {
        if (!storeEpoch(epoch)) {
            err->print(getIdentStrP(str_clock));
            err->print(getStrP(str_store));
            err->println(getStrP(str_error));
        }        
        lastStored =  now; 
    }
}

void SystemClock::setEpoch(EpochTime& epoch, bool trusted) {
    if (this->trusted && !trusted) return;

    this->epoch = epoch;
    this->trusted = trusted;

    onTimeChanged();
}

void SystemClock::onTimeChanged() {
    char buf[64];
    tm tm;
    epochToDateTime(getLocal(), tm);
    tmtodtf(tm, buf);
    if (onTimeChangeEvent) onTimeChangeEvent(buf);
}

String SystemClock::getLocalTimeStr() {
    unsigned long now = getLocal();
    uint8_t hours = (now % 86400L) / 3600;
    uint8_t minutes = (now % 3600) / 60;
    uint8_t seconds = now % 60;
    char buf[16];
    sprintf_P(buf, strf_time, hours, minutes, seconds);
    return String(buf);
}

unsigned long SystemClock::getSystemUptime() {
    return (0xFFFFFFFF / ONE_SECOND_ms) * rollover + (millis() / ONE_SECOND_ms);
}

String SystemClock::getSystemUptimeStr() {
    unsigned long now = getSystemUptime();
    char buf[16];
    sprintf_P(buf, strf_time, (uint8_t)now / 3600 % 24, (uint8_t)now / 60 % 60,
              (uint8_t)now % 60);
    return String(buf);
}

unsigned long SystemClock::getUtc() { return epoch.toEpoch(); }

unsigned long SystemClock::getLocal() { return getUtc() + timeOffset; }

String SystemClock::getDateTimeStr() {
    tm tm;
    epochToDateTime(getLocal(), tm);
    char buf[64];
    tmtodtf(tm, buf);
    return String(buf);
}
