#include "NtpClient.h"

#include "TimeUtils.h"

using StrUtils::setstr;

NtpClient::NtpClient() {
    udp = new WiFiUDP();    
    onDisconnected = WiFi.onStationModeDisconnected(
        {[this](const WiFiEventStationModeDisconnected &event) { end(); }});
    onGotIp = WiFi.onStationModeGotIP(
        {[this](const WiFiEventStationModeGotIP &event) { begin(); }});
    active = false;
    lastUpdated = 0;    
}

void NtpClient::setOutput(Print *p) { output = p; }

void NtpClient::setConfig(Config *config) {
    setInterval(config->getValueAsInt(NTP_SYNC_INTERVAL));
    setServer(config->getValueAsString(NTP_POOL_SERVER));
}

void NtpClient::setInterval(uint16_t time_s) {
    syncInterval = time_s * ONE_SECOND_ms;
}

void NtpClient::setServer(const char *str) {
    size_t len = strlen(str);
    this->server = new char[len + 1];
    setstr(this->server, str, len + 1);
}

bool NtpClient::begin() {
    output->print(getIdentStrP(str_ntp));
    output->printf_P(strf_s_d, server, NTP_REMOTE_PORT);
    active = udp->begin(NTP_LOCAL_PORT);
    if (!active) output->print(getStrP(str_failed));
    output->println();
    return active;
}

void NtpClient::end() {
    if (active) {
        output->print(getIdentStrP(str_ntp));
        output->println(getStrP(str_stopped));
        udp->stop();
        active = false;
    }
}

void NtpClient::loop() {
    if (!active) return;
    if ((millis_since(lastUpdated) >= syncInterval) || lastUpdated == 0) {
        sync();
        lastUpdated = millis();
    }
}

void NtpClient::sync() {
    IPAddress serverIP;
    WiFi.hostByName(server, serverIP);

    uint8_t buf[NTP_PACKET_SIZE];
    memset(buf, 0, NTP_PACKET_SIZE);
    buf[0] = 0b11100011;  // LI, Version, Mode
    buf[1] = 0;           // Stratum, or type of clock
    buf[2] = 6;           // Polling Interval
    buf[3] = 0xEC;  // Peer Clock Precision 8 bytes of zero for Root Delay &
                    // Root Dispersion
    buf[12] = 49;
    buf[13] = 0x4E;
    buf[14] = 49;
    buf[15] = 52;
#ifdef DEBUG_NTP
    output->printf("[ntp] %s:%d ", serverIP.toString().c_str(),
                   NTP_REMOTE_PORT);
#endif
    // send a packet requesting a timestamp
    udp->beginPacket(serverIP, NTP_REMOTE_PORT);
    udp->write(buf, NTP_PACKET_SIZE);
    udp->endPacket();

    uint8_t timeout = 0;
    int cb = 0;
    do {
        delay(10);
        cb = udp->parsePacket();
        if (timeout > 10) {
#ifdef DEBUG_NTP
            output->print(getStrP(str_timeout));
#endif
            return;
        }
        timeout++;
    } while (cb == 0);

    udp->read(buf, NTP_PACKET_SIZE);

    uint16_t high = word(buf[40], buf[41]);
    uint16_t low = word(buf[42], buf[43]);

    epochTime =
        EpochTime((high << 16 | low) - SEVENTY_YEARS_ms + (10 * (timeout + 1)));
#ifdef DEBUG_NTP
    output->print(getStrP(str_gpt));
    output->println(epoch.asEpoch());
#endif
    if (onResponse) onResponse(epochTime);
}

void NtpClient::setOnResponse(NtpClientEventHandler handler) {
    onResponse = handler;
    if (epochTime.toEpoch() > 0) onResponse(epochTime);
}

void NtpClient::printDiag(Print *p) {
    p->print(getStrP(str_active));
    p->println(getBoolStr(active));
    p->print(getStrP(str_interval));
    p->println(syncInterval / ONE_SECOND_ms);
    p->print(getStrP(str_epoch));
    p->println(epochTime.toEpoch());
}
