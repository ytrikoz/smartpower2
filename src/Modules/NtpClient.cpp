#include "Modules/NtpClient.h"

#include "AppUtils.h"
#include "TimeUtils.h"

NtpClient::NtpClient() : AppModule(MOD_NTP) {
    udp = new WiFiUDP();
    active = false;
    lastUpdated = 0;
}

bool NtpClient::begin() {
    start();
    return active;
}

void NtpClient::start() {
    sayP_value(str_server, timeServerPool);
    sayP_value(str_port, String(NTP_REMOTE_PORT).c_str());
    active = udp->begin(NTP_LOCAL_PORT);
    if (!active) {
        String str = StrUtils::getStrP(str_failed);
        say(str);
    }
}

void NtpClient::stop() {
    if (active) {
        String str = StrUtils::getStrP(str_stopped);
        say(str);
        udp->stop();
        active = false;
    }
}

void NtpClient::loop() {
    if (!active)
        return;
    if ((millis_since(lastUpdated) >= syncInterval) || lastUpdated == 0) {
        sync();
        lastUpdated = millis();
    }
}

void NtpClient::setConfig(Config *config) {
    setInterval(config->getValueAsInt(NTP_SYNC_INTERVAL));
    setPoolServer(config->getValueAsString(NTP_POOL_SERVER));
}

void NtpClient::setInterval(uint16_t time_s) {
    syncInterval = time_s * ONE_SECOND_ms;
}

void NtpClient::setPoolServer(const char *str) {
    size_t len = strlen(str);
    this->timeServerPool = new char[len + 1];
    strcpy(this->timeServerPool, str);
}

void NtpClient::sync() {
    IPAddress serverIp;
    WiFi.hostByName(timeServerPool, serverIp);

    uint8_t buf[NTP_PACKET_SIZE];
    memset(buf, 0, NTP_PACKET_SIZE);
    buf[0] = 0b11100011; // LI, Version, Mode
    buf[1] = 0;          // Stratum, or type of clock
    buf[2] = 6;          // Polling Interval
    buf[3] = 0xEC; // Peer Clock Precision 8 bytes of zero for Root Delay &
                   // Root Dispersion
    buf[12] = 49;
    buf[13] = 0x4E;
    buf[14] = 49;
    buf[15] = 52;
#ifdef DEBUG_NTP
    out->printf("[ntp] %s:%d ", serverIp.toString().c_str(), NTP_REMOTE_PORT);
#endif
    // send a packet requesting a timestamp
    udp->beginPacket(serverIp, NTP_REMOTE_PORT);
    udp->write(buf, NTP_PACKET_SIZE);
    udp->endPacket();

    uint8_t timeout = 0;
    int cb = 0;
    do {
        delay(10);
        cb = udp->parsePacket();
        if (timeout > 10) {
            String str = StrUtils::getStrP(str_timeout);
            say(str);
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
    if (onResponse)
        onResponse(epochTime);
}

void NtpClient::setOnResponse(EpochTimeEventHandler h) {
    onResponse = h;
    if (onResponse && epochTime.toEpoch() > 0)
        onResponse(epochTime);
}

size_t NtpClient::printDiag(Print *p) {
    char buf[16];
    size_t n = 0;

    n += p->print(paramStrP(buf, str_active));
    n += p->println(boolStr(buf, active));

    n += p->print(paramStrP(buf, str_server));
    n += p->println(timeServerPool);

    n += p->print(paramStrP(buf, str_port));
    n += p->println(NTP_LOCAL_PORT);

    n += p->print(paramStrP(buf, str_epoch));
    n += p->println(epochTime.toEpoch());

    n += p->print(paramStrP(buf, str_interval));
    n += p->println(syncInterval / ONE_SECOND_ms);

    n += p->print(paramStrP(buf, str_update));
    n += p->println(lastUpdated / ONE_SECOND_ms);

    return n;
}
