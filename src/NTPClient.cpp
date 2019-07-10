#include "NTPClient.h"

NTPClient::NTPClient()
{
    udp = new WiFiUDP();
    active = false;    
}

void NTPClient::setOutput(Print* p)
{
    output = p;
}

void NTPClient::setConfig(Config *config)
{
    setSyncInterval(config->getIntValue(NTP_SYNC_INTERVAL));
    setTimeServer(config->getStrValue(NTP_POOL_SERVER));
}   

void NTPClient::setSyncInterval(uint16_t time_s)
{
    if ((time_s < NTP_SYNC_INTERVAL_MIN_s) && (time_s != 0))
    {
        time_s = NTP_SYNC_INTERVAL_MIN_s;
    }
    syncInterval = time_s * ONE_SECOND_ms;
}

void NTPClient::setTimeServer(const char *fqdn)
{
    server = fqdn;
}

void NTPClient::setOnTimeSynced(TimeSyncedEventHandler handler)
{
    timeSyncedEvent = handler;
}

void NTPClient::begin()
{
    onDisconnected = WiFi.onStationModeDisconnected({
        [this](const WiFiEventStationModeDisconnected &event) 
        {
            output->println("[ntp] stopped");
            active = false;
        }});
    
    onGotIp = WiFi.onStationModeGotIP({
        [this](const WiFiEventStationModeGotIP &event) 
        {
            if (!active) active = udp->begin(port); 
            if (active) output->printf("[ntp] %s:%d every %lu sec\r\n", server, port, syncInterval / ONE_SECOND_ms);
        }});
}

unsigned long NTPClient::getLastUpdated()
{
    return lastUpdated;
}

unsigned long NTPClient::getEpochTime()
{
    return epochTime + (millis() - lastUpdated) / 1000;
}

void NTPClient::loop()
{
    if (!active || syncInterval == 0) return;
    
    if (((millis() - lastUpdated >= syncInterval) || (lastUpdated == 0)))
    {
        syncTime();
    }    
}

void NTPClient::syncTime()
{
    sendNTPpacket();

    // data or timeout...
    uint8_t timeout = 0;
    int cb = 0;
    do
    {
        delay(10);
        cb = udp->parsePacket();
        // timeout after 1000 ms
        if (timeout > 100)
            return;
        timeout++;
        #ifdef DEBUG_NTP
        USE_DEBUG_SERIAL->println("[ntp] ?")
        #endif
    } while (cb == 0);
    
    lastUpdated = millis() - (10 * (timeout + 1));

    udp->read(buffer, NTP_PACKET_SIZE);

    unsigned long highWord = word(this->buffer[40], buffer[41]);
    unsigned long lowWord = word(this->buffer[42], buffer[43]);

    epochTime = (highWord << 16 | lowWord) - SEVENTY_YEARS_ms;

    onTimeSynced();
}

void NTPClient::onTimeSynced()
{
    #ifdef DEBUG_NTP
    USE_DEBUG_SERIAL->printf("[ntp] time synced %lu\r\n", epochTime);
    #endif

    if (timeSyncedEvent) timeSyncedEvent(epochTime);
}

// send an NTP request to the time server at the given address
void NTPClient::sendNTPpacket()
{
    IPAddress serverIP;
    // Get a random server from the pool
    WiFi.hostByName(server, serverIP);

    // set all bytes in the buffer to 0
    memset(this->buffer, 0, NTP_PACKET_SIZE);

    // Initialize values needed to form NTP request
    buffer[0] = 0b11100011; // LI, Version, Mode
    buffer[1] = 0;          // Stratum, or type of clock
    buffer[2] = 6;          // Polling Interval
    buffer[3] = 0xEC;       // Peer Clock Precision
                            // 8 bytes of zero for Root Delay & Root Dispersion
    buffer[12] = 49;
    buffer[13] = 0x4E;
    buffer[14] = 49;
    buffer[15] = 52;

    // send a packet requesting a timestamp
    udp->beginPacket(serverIP, NTP_REMOTE_PORT);
    udp->write(buffer, NTP_PACKET_SIZE);
    udp->endPacket();
}
