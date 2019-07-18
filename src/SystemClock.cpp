#include "SystemClock.h"

SystemClock rtc;

void SystemClock::setConfig(Config *config) {
    setTimeZone(config->getSignedValue(TIME_ZONE));
    setBackupInterval(config->getIntValue(TIME_BACKUP_INTERVAL));
}

void SystemClock::setTimeZone(sint8_t timeZone_h) {
    #ifdef DEBUG_SYSTEM_CLOCK 
    USE_DEBUG_SERIAL.printf_P(str_clock);
    USE_DEBUG_SERIAL.printf_P(strf_timezone, timeZone_h);
    USE_DEBUG_SERIAL.println();
    #endif
    timeOffset_s = timeZone_h * ONE_HOUR_s;
}

void SystemClock::setBackupInterval(uint16_t time_s) {
    if ((time_s < TIME_BACKUP_INTERVAL_MIN_s) && (time_s != 0)) {
        time_s = TIME_BACKUP_INTERVAL_MIN_s;
    }
    backupInterval_ms = time_s * ONE_SECOND_ms;
}

bool SystemClock::isSynced() { return synced; }

void SystemClock::begin() {
    backupAgent = new FileStorage(FILE_TIME_BACKUP);
    restore();
    active = true;
}

void SystemClock::setOutput(Print *p) { output = p; }

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
                backup();
                lastBackup_ms = now_ms;
            }
        }
    }
}

void SystemClock::backup() {
    if (backupAgent) {
        char buf[32];
        backupAgent->set(itoa(getUTC(), buf, DEC));
        #ifdef DEBUG_SYSTEM_CLOCK     
            USE_DEBUG_SERIAL.printf_P(str_clock);
            USE_DEBUG_SERIAL.printf_P(str_store);
            USE_DEBUG_SERIAL.println(buf);
        #endif
        
    }
}

void SystemClock::restore() {
    if (backupAgent) {
        char buf[16];
        #ifdef DEBUG_SYSTEM_CLOCK  
            USE_DEBUG_SERIAL.printf_P(str_clock);
            USE_DEBUG_SERIAL.printf_P(str_restore);
        #endif
        if (backupAgent->restore(buf)) {
            epochTime_s = atoi(buf);
            lastUpdated_ms = millis();
            #ifdef DEBUG_SYSTEM_CLOCK  
                USE_DEBUG_SERIAL.println(getLocalFormated().c_str());
            #endif
        } else {
            output->printf_P(str_clock);
            output->printf_P(str_restore);
            output->printf_P(str_failed);
            output->println();
        }
    }
}

void SystemClock::setTime(DateTime &time) {
    output->printf_P(str_clock);
    output->printf_P(str_synced);
    epochTime_s = time.epochTime_s;
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
