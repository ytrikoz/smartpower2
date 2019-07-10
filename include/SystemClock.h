#pragma once
#include <Arduino.h>

#include "Config.h"
#include "FileStorage.h"

#include "consts.h"

class SystemClock
{
public:
	void setOutput(Print *output);
	void setConfig(Config *config);
	void setTimeZone(uint8_t timeZone);
	void setBackupInterval(uint8_t time_s);
	void setEpochTime(unsigned long epochTime);

	uint32_t getUptime();	
	unsigned long getUTC();	
	unsigned long getLocal();;

	String getLocalFormated();
	String getUptimeFormated();
	
	uint8_t getWeekDay();
    uint8_t getHours();
    uint8_t getMinutes();
    uint8_t getSeconds();
	
	bool isSynced();

	void loop();
	void begin();
private:			
	void backup();
	void restore();

	Print *output;
	
	FileStorage *backupAgent;

	bool active = false;
	bool synced = false;	
	sint16_t timeOffset_s = 0;
	
	unsigned long lastUpdated_ms = 0;
	unsigned long rolloverCounter = 0;
	unsigned long epochTime_s = 0;
	
	unsigned long lastBackup_ms = 0;		
	unsigned long backupInterval_ms = 0;
};

extern SystemClock rtc;

