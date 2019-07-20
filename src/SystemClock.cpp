#include "SystemClock.h"

#include "time_utils.h"

SystemClock rtc;

SystemClock::SystemClock() {
    timeOffset_s = 0;
    backupInterval_ms = 0;

    lastUpdated_ms = 0;
    lastBackup_ms = 0;
    rolloverCounter = 0;

    epochTime_s = 0;
}

void SystemClock::setConfig(Config *config) {
    setTimeZone(config->getSignedValue(TIME_ZONE));
    setBackupInterval(config->getIntValue(TIME_BACKUP_INTERVAL));
}

void SystemClock::setTimeZone(sint8_t timeZone_h) {
    timeOffset_s = timeZone_h * ONE_HOUR_s;
#ifdef DEBUG_SYSTEM_CLOCK
    DEBUG.print(FPSTR(str_clock));
    DEBUG.print(FPSTR(str_timezone));
    DEBUG.println(timeOffset_s);
#endif
}

void SystemClock::setBackupInterval(uint16_t time_s) {
    if ((time_s < TIME_BACKUP_INTERVAL_MIN_s) && (time_s != 0)) {
        time_s = TIME_BACKUP_INTERVAL_MIN_s;
    }
    backupInterval_ms = time_s * ONE_SECOND_ms;

#ifdef DEBUG_SYSTEM_CLOCK
    DEBUG.print(FPSTR(str_clock));
    DEBUG.print(FPSTR(str_backup));
    DEBUG.print(FPSTR(str_interval));
    DEBUG.println(time_s);
#endif
}

void SystemClock::begin() {
    if (!restore(new FileStorage(FILE_TIME_BACKUP))) {
        epochTime_s = getAppBuildTime();
    }
    lastUpdated_ms = millis();
    active = true;
}

void SystemClock::loop() {
    if (!active) return;

    unsigned long now_ms = millis();

    if (now_ms < lastUpdated_ms) rolloverCounter++;

    if (now_ms - lastUpdated_ms >= ONE_SECOND_ms) {
        lastUpdated_ms += ONE_SECOND_ms;
        epochTime_s += 1;

        if (synced && backupInterval_ms > 0) {
            if (((now_ms - lastBackup_ms >= backupInterval_ms) ||
                 (lastBackup_ms == 0))) {
                store(new FileStorage(FILE_TIME_BACKUP));
                lastBackup_ms = now_ms;
            }
        }
    }
}

bool SystemClock::store(FileStorage *agent) {
#ifdef DEBUG_SYSTEM_CLOCK
    DEBUG.print(FPSTR(str_clock));
    DEBUG.print(FPSTR(str_store));
#endif
    char buf[32];
    if (agent->put(itoa(getUTC(), buf, DEC))) {
#ifdef DEBUG_SYSTEM_CLOCK
        DEBUG.println(buf);
#endif
        return true;
    } else {
#ifdef DEBUG_SYSTEM_CLOCK
        DEBUG.println(FPSTR(str_failed));
#endif
        return false;
    }
}

bool SystemClock::restore(FileStorage *agent) {
    char buf[16];
    if (agent->get(buf)) {
        epochTime_s = atoi(buf);
#ifdef DEBUG_SYSTEM_CLOCK
        DEBUG.print(FPSTR(str_clock));
        DEBUG.print(FPSTR(str_restore));
        DEBUG.println(epochTime_s);
#endif
        return true;
    } else {
#ifdef DEBUG_SYSTEM_CLOCK
        DEBUG.print(FPSTR(str_clock));
        DEBUG.print(FPSTR(str_restore));
        DEBUG.println(FPSTR(str_failed));
#endif
        return false;
    }
}

void SystemClock::setTime(EpochTime &epoch) {
    output->printf_P(str_clock);
    output->printf_P(str_synced);
    epochTime_s = epoch.get();
    lastUpdated_ms = millis();
    synced = true;
    output->println(getLocalFormated().c_str());
}

String SystemClock::getLocalFormated() {
    String result = "";
    unsigned long now_ms = getLocal();
    uint8_t hours = (now_ms % 86400L) / 3600;
    uint8_t minutes = (now_ms % 3600) / 60;
    uint8_t seconds = now_ms % 60;
    char buf[16];
    sprintf_P(buf, strf_time, hours, minutes, seconds);
    return String(buf);
}

uint32_t SystemClock::getUptime() {
    return (0xFFFFFFFF / ONE_SECOND_ms) * rolloverCounter +
           (millis() / ONE_SECOND_ms);
}

String SystemClock::getUptimeFormated() {
    uint32_t now_s = getUptime();
    char buf[16];
    sprintf_P(buf, strf_time, now_s / 3600 % 24, now_s / 60 % 60, now_s % 60);
    return String(buf);
}

unsigned long SystemClock::getUTC() { return epochTime_s; }

unsigned long SystemClock::getLocal() { return timeOffset_s + getUTC(); }

uint8_t SystemClock::getWeekDay() { return (((getLocal() / 86400L) + 4) % 7); }

uint8_t SystemClock::getHours() { return ((getLocal() % 86400L) / 3600); }

uint8_t SystemClock::getMinutes() { return ((getLocal() % 3600) / 60); }

uint8_t SystemClock::getSeconds() { return (getLocal() % 60); }

void SystemClock::setOutput(Print *p) { output = p; }

bool SystemClock::isSynced() { return synced; }
