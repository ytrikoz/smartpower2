#include "SystemClock.h"

SystemClock rtc;

void SystemClock::setConfig(Config *config)
{
	setTimeZone(config->getIntValue(TIME_ZONE));
    setBackupInterval(config->getIntValue(TIME_BACKUP_INTERVAL));
}

void SystemClock::setTimeZone(uint8_t timeZone_h)
{
    timeOffset_s = timeZone_h * ONE_HOUR_s;
}

void SystemClock::setBackupInterval(uint8_t time_s)
{
    if ((time_s < TIME_BACKUP_INTERVAL_MIN_s) && (time_s != 0))
    {
        time_s = TIME_BACKUP_INTERVAL_MIN_s;
    }
    backupInterval_ms = time_s * ONE_SECOND_ms;
}

bool SystemClock::isSynced()
{
    return synced;
}

void SystemClock::begin()
{
    backupAgent = new FileStorage(FILE_TIME_BACKUP);
    restore();
    active = true;
}

void SystemClock::setOutput(Print *p)
{
    output = p;
}

void SystemClock::loop()
{
    if (!active) return;

    unsigned long now = millis();

    if (now < lastUpdated_ms) rolloverCounter++;

    if (now - lastUpdated_ms >= ONE_SECOND_ms)
    {
        lastUpdated_ms += ONE_SECOND_ms;
        epochTime_s += 1;
    
        if (synced && backupInterval_ms > 0)
        {        
            if (((now - lastBackup_ms >= backupInterval_ms) || (lastBackup_ms == 0)))
            {
                backup();
                lastBackup_ms = now;
            }    
        }
    }
}
    
void SystemClock::backup()
{
    if (backupAgent)
    {
        char buf[16];
        backupAgent->store(itoa(getUTC(), buf, DEC));
    }
}

void SystemClock::restore()
{
    
    if (backupAgent)
    {
        char buf[16];
        if (backupAgent->restore(buf))
        {
            setEpochTime(atoi(buf));
            return;
        }
    }

    
}

String SystemClock::getLocalFormated()
{
    unsigned long now = getLocal();
    unsigned long hours = (now % 86400L) / 3600;
    unsigned long minutes = (now % 3600) / 60;
    unsigned long seconds = now % 60;
    String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);
    String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);
    String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);    
    return hoursStr + ":" + minuteStr + ":" + secondStr;
}

uint32_t SystemClock::getUptime()
{
    return (0xFFFFFFFF / ONE_SECOND_ms) * rolloverCounter + (millis() / ONE_SECOND_ms);
}

String SystemClock::getUptimeFormated()
{
    uint32_t now_s = getUptime();
    char buf[10];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", now_s / 3600 % 24, now_s / 60 % 60, now_s % 60);
    return String(buf);
}

unsigned long SystemClock::getUTC()
{
    return epochTime_s;
}

unsigned long SystemClock::getLocal()
{
    return timeOffset_s + getUTC();
}

void SystemClock::setEpochTime(unsigned long epoch_s)
{
    epochTime_s = epoch_s;	
	lastUpdated_ms = millis();	
    synced = true;
    
    output->printf_P(PSTR("[clock] set %s\r\n"), getLocalFormated().c_str());
}

uint8_t SystemClock::getWeekDay()
{
    return (((getLocal() / 86400L) + 4) % 7);
}

uint8_t SystemClock::getHours()
{
    return ((getLocal() % 86400L) / 3600);
}

uint8_t SystemClock::getMinutes()
{
    return ((getLocal() % 3600) / 60);
}

uint8_t SystemClock::getSeconds()
{
	return (getLocal() % 60);
}
