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
    udp = new WiFiUDP();
    initialized = true;
}

void NTPClient::printDiag(Print *p) {
    p->printf_P(strf_init, initialized);
    p->printf_P(strf_active, active);    
    p->printf_P(strf_every_sec, interval_ms / ONE_SECOND_ms);
    p->printf_P(strf_synced, (int) (millis() - updated_ms) / ONE_SECOND_ms);
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
        udp->stop();
        output->print(FPSTR(str_ntp));
        output->println(FPSTR(str_stopped));
        active = false;
    }
}

bool NTPClient::begin() {
    if (!initialized) {
        init();
    }
    output->print(FPSTR(str_ntp));
    active = udp->begin(port);
    if (active) {
        output->printf_P(strf_ip_port, server, port);
        output->printf_P(strf_every_ms, interval_ms / ONE_SECOND_ms);
        output->println();
    } else {
        output->println(FPSTR(str_failed));
    }
    return active;
}

void NTPClient::loop() {
    if (!active) return;
    if ((updated_ms == 0) ||
        (interval_ms > 0 && (millis() - updated_ms >= interval_ms))) {
        sync();
    }
}

void NTPClient::sync() {
    send_udp_packet();
    uint8_t timeout = 0;
    int cb = 0;
    do {
        delay(10);
        cb = udp->parsePacket();
        if (timeout > 100) {
#ifdef DEBUG_NTP
            DEBUG.printf_P(strf_ntp, (char *)pgm_read_ptr(str_got));
            DEBUG.print(time.epochTime_s);
            DEBUG.println();
#endif
            return;
        }
        timeout++;
    } while (cb == 0);

    updated_ms = millis();

    udp->read(buffer, NTP_PACKET_SIZE);

    unsigned long highWord = word(this->buffer[40], buffer[41]);
    unsigned long lowWord = word(this->buffer[42], buffer[43]);

    epochTime = EpochTime((highWord << 16 | lowWord) - SEVENTY_YEARS_ms +
                          (10 * (timeout + 1)));

#ifdef DEBUG_NTP
    USE_DEBUG_SERIAL.printf_P(str_ntp);
    USE_DEBUG_SERIAL.printf_P(str_got);
    USE_DEBUG_SERIAL.print(time.epochTime_s);
    USE_DEBUG_SERIAL.println();
#endif
    if (onTimeSynced) {
        onTimeSynced(epochTime);
    };
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
