#include "Modules/NtpClient.h"

#include "AppUtils.h"
#include "PrintUtils.h"
#include "StrUtils.h"
#include "TimeUtils.h"

using namespace AppUtils;
using namespace PrintUtils;
using namespace StrUtils;

NtpClient::NtpClient() : AppModule(MOD_NTP) { state = STOPPED; }

bool NtpClient::begin() {
    udp = new WiFiUDP();
    return start();
}

bool NtpClient::start() {
    if (state == STOPPED)
        if (udp->begin(NTP_LOCAL_PORT)) {
            state = STARTED;
        }
    return state != STOPPED;
}

void NtpClient::stop() {
    if (state == STARTED) {
        udp->stop();
        state = STOPPED;
    }
}

void NtpClient::loop() {
    if (state == STOPPED)
        return;

    if (millis_since(requestTime) <= NTP_TIMEOUT &&
        responseTime <= requestTime) {
        checkResponse();
        return;
    }

    if (!responseTime || (millis_since(responseTime) >= requestInterval))
        sendRequest();
}

void NtpClient::setConfig(Config *config) {
    setLocalPort(NTP_LOCAL_PORT);
    setServer(config->getValueAsString(NTP_POOL_SERVER));
    setInterval(config->getValueAsInt(NTP_SYNC_INTERVAL));
};

void NtpClient::setServer(const char *str) { setServer(str, NTP_REMOTE_PORT); }

void NtpClient::setServer(const char *str, uint16_t port) {
    server = new char[strlen(str) + 1];
    strcpy(server, str);
    localPort = port;
}

void NtpClient::setInterval(uint16_t value) {
    requestInterval = value * ONE_SECOND_ms;
}

void NtpClient::setRemotePort(int16_t port) { remotePort = port; }

void NtpClient::setLocalPort(int16_t port) { localPort = port; }

void NtpClient::sendRequest() {
    IPAddress serverIp;
    if (!WiFi.hostByName(server, serverIp)) {
        log_error(msg_dns_resolve_error, server);
        stop();
        return;
    }

    uint8_t buf[NTP_PACKET_SIZE];
    memset(buf, 0, NTP_PACKET_SIZE);
    buf[0] = 0b11100011; // LI, Version, Mode
    buf[1] = 0;          // Stratum, or type of clock
    buf[2] = 6;          // Polling Interval
    buf[3] = 0xEC;       // Peer Clock Precision 8 bytes of zero for Root
                         // Delay & Root Dispersion
    buf[12] = 49;
    buf[13] = 0x4E;
    buf[14] = 49;
    buf[15] = 52;

    char tmp[32];
    sprintf(tmp, "%s:%d", serverIp.toString().c_str(), NTP_REMOTE_PORT);
    say_strP(str_request, tmp);

    // send a packet requesting a timestamp
    udp->beginPacket(serverIp, NTP_REMOTE_PORT);
    udp->write(buf, NTP_PACKET_SIZE);
    udp->endPacket();

    requestTime = millis();
}

void NtpClient::checkResponse() {
    if (udp->parsePacket()) {
        char buf[NTP_PACKET_SIZE];
        udp->read(buf, NTP_PACKET_SIZE);
        uint16_t high = word(buf[40], buf[41]);
        uint16_t low = word(buf[42], buf[43]);
        unsigned long epoch = (high << 16 | low) - SEVENTY_YEARS_ms;
        gotResponse(epoch);
    }
}

void NtpClient::gotResponse(unsigned long epoch) {
    say_strP(str_got, epoch);
    responseTime = millis();
    time.set(epoch, responseTime);
    if (timeHandler)
        timeHandler(time.now());
    stop();
}

void NtpClient::setOnResponse(TimeHandler handler) {
    if (handler && time.now())
        handler(time.now());
    timeHandler = handler;
}

size_t NtpClient::printDiag(Print *p) {
    size_t n = println_nameP_value(p, str_active, boolStr(active));
    n += println_nameP_value(p, str_bind, NTP_LOCAL_PORT);
    n += println_nameP_value(p, str_server,
                             fmt_ip_port(server, NTP_REMOTE_PORT));
    n += println_nameP_value(p, str_interval, requestInterval / ONE_SECOND_ms);
    n += println_nameP_value(p, str_request, requestTime / ONE_SECOND_ms);
    n += println_nameP_value(p, str_response, responseTime / ONE_SECOND_ms);
    n += println_nameP_value(p, str_epoch, time.epoch);
    return n;
}
