#pragma once
#include <WiFiUdp.h>

#include "Config.h"

#include "consts.h"
#include "str_utils.h"

// NTP time stamp is in the first 48 bytes of the message
#define NTP_PACKET_SIZE 48

typedef std::function<void(unsigned long epoch)> TimeSyncedEventHandler;

class NTPClient
{
public:
	NTPClient();

	void begin();
	void loop();
	void print();	

	void setOutput(Print* p);
	void setConfig(Config *config);
	void setOnTimeSynced(TimeSyncedEventHandler handler);

	void setSyncInterval(uint16_t seconds);
	void setTimeServer(const char* fqdn);
	void setTimeZone(uint8_t zone);

	unsigned long getLastUpdated();	
private:
	unsigned long getEpochTime();
	void syncTime();
	void sendNTPpacket();	
	void onTimeSynced();

	Print *output;		

	const char* server = DEF_NTP_POOL_SERVER; 	
	int port = NTP_LOCAL_PORT;	
	byte buffer[NTP_PACKET_SIZE];

    bool active = false;
	// In s
	unsigned long epochTime = 0;
	// In ms
	unsigned long syncInterval = ONE_HOUR_ms;
	// In ms
	unsigned long lastUpdated = 0;	
	// In s
	sint16_t timeOffset = 0;

	TimeSyncedEventHandler timeSyncedEvent;
	WiFiUDP *udp;
	WiFiEventHandler onDisconnected, onGotIp;
};