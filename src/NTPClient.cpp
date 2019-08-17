#include "NtpClient.h"

#include "TimeUtils.h"

NtpClient::NtpClient() {
    udp = new WiFiUDP();

    onDisconnected = WiFi.onStationModeDisconnected(
        {[this](const WiFiEventStationModeDisconnected &event) { end(); }});
    onGotIp = WiFi.onStationModeGotIP(
        {[this](const WiFiEventStationModeGotIP &event) { begin(); }});

    active = false;
}

void NtpClient::printDiag(Print *p) {
    p->printf_P(strf_active, active);    
    p->printf_P(strf_every_sec, interval_ms / ONE_SECOND_ms);
    p->printf_P(strf_synced, (int) (millis() - updated_ms) / ONE_SECOND_ms);
    p->println();
}

void NtpClient::setOutput(Print *p) { output = p; }

void NtpClient::setConfig(Config *config) {
    setSyncInterval(config->getIntValue(NTP_SYNC_INTERVAL));
    setTimeServer(config->getStrValue(NTP_POOL_SERVER));
}

void NtpClient::setSyncInterval(uint16_t time_s) {
    interval_ms = time_s * ONE_SECOND_ms;
}

void NtpClient::setTimeServer(const char *str) {
    StrUtils::setstr(this->server, str, strlen(str) + 1);
}

bool NtpClient::begin() {
    output->print(FPSTR(str_ntp));    
    output->printf_P(strf_s_d, server, NTP_REMOTE_PORT);
    active = udp->begin(NTP_REMOTE_PORT);
    if (!active) output->print(FPSTR(str_failed));
    output->println();
    return active;
}

void NtpClient::end() {
    if (active) {
        output->print(FPSTR(str_ntp));
        output->println(FPSTR(str_stopped));        
        udp->stop();
        active = false;
    }
}


void NtpClient::loop() {
    if (!active) return;

    if (millis_since(updated_ms) >= interval_ms) {
        sync();
        updated_ms = millis();
    }
}

void NtpClient::sync() {
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
    DEBUG.printf("[ntp] -> %s:%d\r\n", serverIP.toString().c_str(),
                            port);
#endif
    // send a packet requesting a timestamp
    udp->beginPacket(serverIP, port);
    udp->write(buffer, NTP_PACKET_SIZE);
    udp->endPacket();

    uint8_t timeout = 0;
    int cb = 0;
    do {
        delay(10);
        cb = udp->parsePacket();
        if (timeout > 10) {
#ifdef DEBUG_NTP
            DEBUG.printf_P(strf_ntp, (char *)pgm_read_ptr(str_got));
            DEBUG.print(time.epochTime_s);
            DEBUG.println();
#endif
            return;
        }
        timeout++;
    } while (cb == 0);

    udp->read(buffer, NTP_PACKET_SIZE);

    unsigned long highWord = word(this->buffer[40], buffer[41]);
    unsigned long lowWord = word(this->buffer[42], buffer[43]);

    epochTime = EpochTime((highWord << 16 | lowWord) - SEVENTY_YEARS_ms +
                          (10 * (timeout + 1)));
#ifdef DEBUG_NTP
    DEBUG.print(FPSTR(str_ntp));
    DEBUG.print(FPSTR(str_got));
    DEBUG.println(time.epochTime_s);
#endif
    if (onTimeSynced) onTimeSynced(epochTime);
}

void NtpClient::setOnTimeSynced(NtpClientEventHandler h) { onTimeSynced = h; }
