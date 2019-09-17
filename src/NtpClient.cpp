#include "NtpClient.h"

#include "TimeUtils.h"

NtpClient::NtpClient() : AppModule(MOD_NTP) {
    udp = new WiFiUDP();
    onDisconnected = WiFi.onStationModeDisconnected(
        {[this](const WiFiEventStationModeDisconnected &event) {
            if (active) {
                say_P(str_stopped);
                udp->stop();
                active = false;
            }
        }});
    onGotIp = WiFi.onStationModeGotIP(
        {[this](const WiFiEventStationModeGotIP &event) {            
            sayf("%s:%d", timeServerPool, NTP_REMOTE_PORT);
            active = udp->begin(NTP_LOCAL_PORT);
        }});

    active = false;
    lastUpdated = 0;
}

void NtpClient::loop() {
    if (!active) return;
    if ((millis_since(lastUpdated) >= syncInterval) || lastUpdated == 0) {
        sync();
        lastUpdated = millis();
    }
}

void NtpClient::setConfig(Config *config) {
    setInterval(config->getValueAsInt(NTP_SYNC_INTERVAL));
    setServer(config->getValueAsString(NTP_POOL_SERVER));
}

void NtpClient::setInterval(uint16_t time_s) {
    syncInterval = time_s * ONE_SECOND_ms;
}

void NtpClient::setServer(const char *str) {
    size_t len = strlen(str);
    this->timeServerPool = new char[len + 1];
    strcpy(this->timeServerPool, str);    
}

void NtpClient::sync() {
    IPAddress timeSrvIpAddr;
    WiFi.hostByName(timeServerPool, timeSrvIpAddr);

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
    out->printf("[ntp] %s:%d ", timeSrvIpAddr.toString().c_str(),
                NTP_REMOTE_PORT);
#endif
    // send a packet requesting a timestamp
    udp->beginPacket(timeSrvIpAddr, NTP_REMOTE_PORT);
    udp->write(buf, NTP_PACKET_SIZE);
    udp->endPacket();

    uint8_t timeout = 0;
    int cb = 0;
    do {
        delay(10);
        cb = udp->parsePacket();
        if (timeout > 10) {
#ifdef DEBUG_NTP
            out->print(StrUtils::getStrP(str_timeout));
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
    out->print(StrUtils::getStrP(str_gpt));
    out->println(epoch.asEpoch());
#endif
    if (onResponse) onResponse(epochTime);
}

void NtpClient::setOnResponse(EpochTimeEventHandler h) {
    onResponse = h;
    if (onResponse && epochTime.toEpoch() > 0) onResponse(epochTime);
}

void NtpClient::printDiag() {
    sayf("%s: %s", StrUtils::getStrP(str_active, false).c_str(), StrUtils::getBoolStr(active).c_str());
    sayf("%s: %s:%d", StrUtils::getStrP(str_server, false).c_str(), timeServerPool, NTP_LOCAL_PORT);
    sayf("%s: %lu", StrUtils::getStrP(str_epoch, false).c_str(), epochTime.toEpoch());
    sayf("%s: %d", StrUtils::getStrP(str_interval, false).c_str(), syncInterval / ONE_SECOND_ms);
    sayf("%s: %lu", StrUtils::getStrP(str_update, false).c_str(), lastUpdated);
}
