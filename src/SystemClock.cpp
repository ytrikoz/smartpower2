#include "SystemClock.h"

#include "BuildConfig.h"
#include "PrintUtils.h"
#include "StoreUtils.h"
#include "TimeUtils.h"

SystemClock::SystemClock() : AppModule(MOD_CLOCK) {
    trusted = false;
    timeOffset = storeInterval = lastStored = rollover = 0;
    epoch = EpochTime(getAppBuildUtc());
    lastUpdated = millis();
}

bool SystemClock::begin() {
    sayln_P(str_start);
    if (!trusted)
        if (restoreState(epoch)) setEpoch(epoch);
    return true;
}

void SystemClock::printDiag(Print* p) {
    PrintUtils::print(out, StrUtils::getStrP(str_timezone), ':',
                      timeOffset / ONE_HOUR_s);
    PrintUtils::print(out, StrUtils::getStrP(str_backup),
                      StrUtils::getStrP(str_every), ':',
                      storeInterval / ONE_SECOND_ms);
    PrintUtils::print(out, StrUtils::getStrP(str_updated), ':',
                      lastUpdated / ONE_SECOND_ms);
    PrintUtils::print(out, StrUtils::getStrP(str_stored), ':',
                      lastStored / ONE_SECOND_ms);
    PrintUtils::print(out, StrUtils::getStrP(str_rollover), ':', rollover);
    PrintUtils::print(out, StrUtils::getStrP(str_epoch), ':', epoch);
    PrintUtils::println(out, StrUtils::getStrP(str_trusted), ':',
                        StrUtils::getBoolStr(trusted));
}

void SystemClock::setConfig(Config* config) {
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
                 : 0;
}

void SystemClock::loop() {
    unsigned long now = millis();
    if (now < lastUpdated) rollover++;
    while (millis_passed(lastUpdated, now) >= ONE_SECOND_ms) {
        epoch.tick();
        lastUpdated += ONE_SECOND_ms;
    }
    if (trusted) {
        if (isStoreNeedsUpdate(now)) {
            if (!storeState(epoch)) {
                err->print(StrUtils::getIdentStrP(str_clock));
                err->print(StrUtils::getStrP(str_store));
                err->println(StrUtils::getStrP(str_error));
            }
            lastStored = now;
        }
    }
}

void SystemClock::setEpoch(EpochTime& epoch, bool trusted) {
    if (this->trusted && !trusted) return;

    this->epoch = epoch;
    this->trusted = trusted;

    onTimeChange();
}

void SystemClock::onTimeChange() {
    if (timeChangeHandler) timeChangeHandler(epoch);
}

void SystemClock::setOnTimeChange(TimeEventHandler h) { 
    timeChangeHandler = h; 
    if (trusted) onTimeChange();
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

bool SystemClock::restoreState(EpochTime& value) {
    String buf;
    bool res = StoreUtils::restoreString(FILE_VAR_UTC, buf);
    if (res) value = EpochTime(buf.toInt());
    return res;
}

bool SystemClock::storeState(EpochTime& value) {
    String buf = value.toString();
    return StoreUtils::storeString(FILE_VAR_UTC, buf);
}
