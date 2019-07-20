#include "NTPClient.h"

NTPClient::NTPClient() {
    active = false;
    initialized = false;

    onDisconnected = WiFi.onStationModeDisconnected(
        {[this](const WiFiEventStationModeDisconnected &event) { end(); }});
    onGotIp = WiFi.onStationModeGotIP(
        {[this](const WiFiEventStationModeGotIP &event) { begin(); }});
}

void NTPClient::init() {
    #ifdef DEBUG_NTP    
    USE_DEBUG_SERIAL.printf_P(str_ntp);
    USE_DEBUG_SERIAL.printf_P(strf_s_d, server, port);
    USE_DEBUG_SERIAL.println(interval_ms / ONE_SECOND_ms);
    #endif
    udp = new WiFiUDP();    
    initialized = true;
}

void NTPClient::printDiag(Print *p) {
    p->printf_P(strf_s_d, "active", active);
    p->printf_P(strf_s_d, "initialized", initialized);
    p->printf_P(strf_s_d, "interval", (int)interval_ms / 1000);
    p->printf_P(strf_s_d, "updated", (int)updated_ms / 1000);
    p->println();
}

void NTPClient::setOutput(Print *p) { output = p; }

void NTPClient::setConfig(Config *config) {
    setSyncInterval(config->getIntValue(NTP_SYNC_INTERVAL));
    setTimeServer(config->getStrValue(NTP_POOL_SERVER));
}

void NTPClient::setSyncInterval(uint16_t time_s) {
    interval_ms = time_s * ONE_SECOND_ms;
}

void NTPClient::setTimeServer(const char *str) {
    strcpy(server, str);
    port = NTP_REMOTE_PORT;
}

void NTPClient::end() {
    if (active) {
        output->printf_P(str_ntp);
        output->printf_P(str_stopped);
        active = false;
    }
}

bool NTPClient::begin() {
    if (!initialized) {
        init();
    }
    output->printf_P(str_ntp);    
    output->printf_P(str_start);    
    active = udp->begin(NTP_LOCAL_PORT);
    if (active)
        output->printf_P(str_success);
    else
        output->printf_P(str_failed);           
    output->println();
    return active;
}

void NTPClient::loop() {
    if (!active) return;

    if ((updated_ms == 0) || (interval_ms > 0 && (millis() - updated_ms >= interval_ms))) sync();
}

void NTPClient::sync() {
    send_udp_packet();

    // data or timeout...
    uint8_t timeout = 0;
    int cb = 0;
    do {
        delay(10);
        cb = udp->parsePacket();
        // timeout after 1000 ms
        if (timeout > 100) {
#ifdef DEBUG_NTP
            USE_DEBUG_SERIAL.printf_P(strf_ntp, (char *)pgm_read_ptr(str_got));
            USE_DEBUG_SERIAL.print(time.epochTime_s);
            USE_DEBUG_SERIAL.println();
#endif
            return;
        }
        timeout++;
    } while (cb == 0);

    updated_ms = millis();

    udp->read(buffer, NTP_PACKET_SIZE);

    unsigned long highWord = word(this->buffer[40], buffer[41]);
    unsigned long lowWord = word(this->buffer[42], buffer[43]);

    epochTime = EpochTime((highWord << 16 | lowWord) - SEVENTY_YEARS_ms + (10 * (timeout + 1)));

#ifdef DEBUG_NTP
    USE_DEBUG_SERIAL.printf_P(str_ntp);
    USE_DEBUG_SERIAL.printf_P(str_got);
    USE_DEBUG_SERIAL.print(time.epochTime_s);
    USE_DEBUG_SERIAL.println();
#endif
    if (onTimeSynced) { onTimeSynced(epochTime); };
}

void NTPClient::send_udp_packet() {
    // send an NTP request to the time server at the given address
    IPAddress serverIP;
    // Get a random server from the pool
    WiFi.hostByName(server, serverIP);

    // set all bytes in the buffer to 0
    memset(this->buffer, 0, NTP_PACKET_SIZE);

    // Initialize values needed to form NTP request
    buffer[0] = 0b11100011;  // LI, Version, Mode
    buffer[1] = 0;           // Stratum, or type of clock
    buffer[2] = 6;           // Polling Interval
    buffer[3] = 0xEC;        // Peer Clock Precision
                             // 8 bytes of zero for Root Delay & Root Dispersion
    buffer[12] = 49;
    buffer[13] = 0x4E;
    buffer[14] = 49;
    buffer[15] = 52;

#ifdef DEBUG_NTP
    USE_DEBUG_SERIAL.printf("[ntp] -> %s:%d\r\n", serverIP.toString().c_str(),
                            port);
#endif
    // send a packet requesting a timestamp
    udp->beginPacket(serverIP, port);
    udp->write(buffer, NTP_PACKET_SIZE);
    udp->endPacket();
}

void NTPClient::setOnTimeSynced(NtpClientEventHandler h) { onTimeSynced = h; }
