#include "SystemClock.h"
#include "debug.h"
#include "time_utils.h"

SystemClock rtc;

SystemClock::SystemClock() {
    storage = new FileStorage(FILE_TIME_BACKUP);
    timeOffset_s = 0;
    backupInterval_ms = 0;
    lastUpdated_ms = 0;
    lastBackup_ms = 0;
    rolloverCounter = 0;
    utcEpochTime_s = 0;
}

tm SystemClock::getDateTime() {
    struct tm dateTime;
    epoch_to_tm(getLocalEpoch(), dateTime);
    return dateTime;
}

void SystemClock::setConfig(Config *config) {
    setTimeZone(config->getSignedValue(TIME_ZONE));
    setBackupInterval(config->getIntValue(TIME_BACKUP_INTERVAL));
}

void SystemClock::setTimeZone(sint8_t timeZone_h) {
#ifdef DEBUG_SYSTEM_CLOCK
    DEBUG.print(FPSTR(str_clock));
    DEBUG.print(FPSTR(str_timezone));
#endif
    timeOffset_s = timeZone_h * ONE_HOUR_s;
#ifdef DEBUG_SYSTEM_CLOCK
    DEBUG.println(timeOffset_s);
#endif
}

void SystemClock::setBackupInterval(uint16_t time_s) {
#ifdef DEBUG_SYSTEM_CLOCK
    DEBUG.print(FPSTR(str_clock));
    DEBUG.print(FPSTR(str_backup));
#endif

    if ((time_s < TIME_BACKUP_INTERVAL_MIN_s) && (time_s != 0)) {
        time_s = TIME_BACKUP_INTERVAL_MIN_s;
    }
    backupInterval_ms = time_s * ONE_SECOND_ms;
#ifdef DEBUG_SYSTEM_CLOCK
    DEBUG.print(FPSTR(str_interval));
    DEBUG.println(time_s);
#endif
}
void SystemClock::setOnSystemTimeChanged(SystemTimeChangedEvent e) {
    onSystemTimeChanged = e;
}

void SystemClock::begin() {
    if (!restore()) {
        unsigned long utc_epoch_s = get_appbuild_epoch();
#ifdef DEBUG_SYSTEM_CLOCK
        DEBUG.print(FPSTR(str_clock));
        DEBUG.print(FPSTR(str_last_known));
        DEBUG.println(utc_epoch_s);
#endif
        setTime(utc_epoch_s);
    }
    active = true;
}

void SystemClock::loop() {
    if (!active) return;
    unsigned long now_s = millis();
    if (now_s < lastUpdated_ms) rolloverCounter++;
    if (now_s - lastUpdated_ms >= ONE_SECOND_ms) {
        lastUpdated_ms += ONE_SECOND_ms;
        utcEpochTime_s += 1;
        if (synced && backupInterval_ms > 0) {
            if (((now_s - lastBackup_ms >= backupInterval_ms) ||
                 (lastBackup_ms == 0))) {
                store();
            }
        }
    }
}

bool SystemClock::store() {
    lastBackup_ms = millis();
    char buf[16];
    bool res = storage->put(itoa(getUtcEpoch(), buf, DEC));
    return res;
}

bool SystemClock::restore() {
    char buf[16];
    if (!storage->get(buf)) {
#ifdef DEBUG_SYSTEM_CLOCK
        DEBUG.print(FPSTR(str_clock));
        DEBUG.print(FPSTR(str_restore));
        DEBUG.println(FPSTR(str_failed));
#endif
        return false;
    }
    unsigned long utc_epoch_s = atoi(buf);
    setTime(utc_epoch_s);
    return true;
}

void SystemClock::setTime(unsigned long utc_epoch_s) {
    utcEpochTime_s = utc_epoch_s;
    synced = true;
    if (onSystemTimeChanged) {
        onSystemTimeChanged(getLocalFormated().c_str());
    };   
}

String SystemClock::getLocalFormated() {
    String result = "";
    unsigned long now_s = getLocalEpoch();
    uint8_t hours = (now_s % 86400L) / 3600;
    uint8_t minutes = (now_s % 3600) / 60;
    uint8_t seconds = now_s % 60;
    char buf[16];
    sprintf_P(buf, strf_time, hours, minutes, seconds);
    return String(buf);
}

uint32_t SystemClock::getUptimeEpoch() {
    return (0xFFFFFFFF / ONE_SECOND_ms) * rolloverCounter +
           (millis() / ONE_SECOND_ms);
}

String SystemClock::getUptimeFormated() {
    uint32_t now_s = getUptimeEpoch();
    char buf[16];
    sprintf_P(buf, strf_time, now_s / 3600 % 24, now_s / 60 % 60, now_s % 60);
    return String(buf);
}

unsigned long SystemClock::getUtcEpoch() { return utcEpochTime_s; }

unsigned long SystemClock::getLocalEpoch() {
    return timeOffset_s + getUtcEpoch();
}

uint8_t SystemClock::getWeekDay() {
    return (((getLocalEpoch() / 86400L) + 4) % 7);
}

uint8_t SystemClock::getHours() { return ((getLocalEpoch() % 86400L) / 3600); }

uint8_t SystemClock::getMinutes() { return ((getLocalEpoch() % 3600) / 60); }

uint8_t SystemClock::getSeconds() { return (getLocalEpoch() % 60); }

void SystemClock::setOutput(Print *p) { output = p; }

bool SystemClock::isSynced() { return synced; }
