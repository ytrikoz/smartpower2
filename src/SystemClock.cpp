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

void SystemClock::begin() {
    if (!restore(new FileStorage(FILE_TIME_BACKUP))) {
        epochTime_s = getAppBuildTime();
        #ifdef DEBUG_SYSTEM_CLOCK
        DEBUG.print(FPSTR(str_clock));
        DEBUG.print(FPSTR(str_set));
        DEBUG.println(epochTime_s);
        #endif
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
            if (((now_ms - lastBackup_ms >= backupInterval_ms) || (lastBackup_ms == 0))) {
                store(new FileStorage(FILE_TIME_BACKUP));
                lastBackup_ms = now_ms;
            }
        }
    }
}

bool SystemClock::store(FileStorage *agent) {
    char buf[32];
    return agent->put(itoa(getLocalEpoch(), buf, DEC));
}

bool SystemClock::restore(FileStorage *agent) {
#ifdef DEBUG_SYSTEM_CLOCK
        DEBUG.print(FPSTR(str_clock));
        DEBUG.println(FPSTR(str_restore));   
#endif
    char buf[16];
    if (agent->get(buf)) {
        epochTime_s = atoi(buf);
#ifdef DEBUG_SYSTEM_CLOCK
        DEBUG.print(FPSTR(str_clock));
        DEBUG.println(epochTime_s);
#endif
        return true;
    } else {
#ifdef DEBUG_SYSTEM_CLOCK
        DEBUG.print(FPSTR(str_clock));
        DEBUG.println(FPSTR(str_failed));
#endif
        return false;
    }
}

void SystemClock::setTime(EpochTime &epoch) {
    output->print(FPSTR(str_clock));
    output->print(FPSTR(str_system_time));
    
    epochTime_s = epoch.get();
    lastUpdated_ms = millis();
    synced = true;    
    onSystemTimeChanged(getLocalTimeFormated().c_str());
}

String SystemClock::getLocalTimeFormated() {
    String result = "";
    unsigned long now_s = getLocalEpoch();
    uint8_t hours = (now_s % 86400L) / 3600;
    uint8_t minutes = (now_s % 3600) / 60;
    uint8_t seconds = now_s % 60;
    char buf[16];
    sprintf_P(buf, strf_time, hours, minutes, seconds);
    return String(buf);
}

uint32_t SystemClock::getSystemUptime() {
    return (0xFFFFFFFF / ONE_SECOND_ms) * rolloverCounter +
           (millis() / ONE_SECOND_ms);
}

String SystemClock::getUptimeFormated() {
    uint32_t now_s = getSystemUptime();
    char buf[16];
    sprintf_P(buf, strf_time, now_s / 3600 % 24, now_s / 60 % 60, now_s % 60);
    return String(buf);
}

unsigned long SystemClock::getUtcEpoch() { return epochTime_s; }

unsigned long SystemClock::getLocalEpoch() { return timeOffset_s + getUtcEpoch(); }

uint8_t SystemClock::getWeekDay() { return (((getLocalEpoch() / 86400L) + 4) % 7); }

uint8_t SystemClock::getHours() { return ((getLocalEpoch() % 86400L) / 3600); }

uint8_t SystemClock::getMinutes() { return ((getLocalEpoch() % 3600) / 60); }

uint8_t SystemClock::getSeconds() { return (getLocalEpoch() % 60); }

void SystemClock::setOutput(Print *p) { output = p; }

bool SystemClock::isSynced() { return synced; }
