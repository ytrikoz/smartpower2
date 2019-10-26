#include "Modules/NtpClient.h"

#include "AppUtils.h"
#include "PrintUtils.h"
#include "StrUtils.h"
#include "TimeUtils.h"

using namespace AppUtils;
using namespace PrintUtils;
using namespace StrUtils;

NtpClient::NtpClient() : AppModule(MOD_NTP) {
    udp = new WiFiUDP();
    requestTime = responseTime = 0;
    active = false;
}

bool NtpClient::begin() {
    start();
    return active;
}

void NtpClient::start() {
    say_strP(str_server, timeServerPool);
    say_strP(str_port, NTP_REMOTE_PORT);
    active = udp->begin(NTP_LOCAL_PORT);
    if (!active)
        say_strP(str_failed);
}

void NtpClient::stop() {
    if (active) {
        udp->stop();
        say_strP(str_stopped);
    }
    active = false;
}

void NtpClient::loop() {
    if (!active)
        return;

    if (millis_since(requestTime) <= timeout && responseTime <= requestTime) {
        waitResponse();
        return;
    }

    if ((millis_since(responseTime) >= syncInterval) || !responseTime) {
        sendRequest();
    }
}

void NtpClient::setConfig(Config *config) {
    setInterval(config->getValueAsInt(NTP_SYNC_INTERVAL));
    setPoolServer(config->getValueAsString(NTP_POOL_SERVER));
    setTimeout(1000);
};

void NtpClient::setTimeout(uint16_t time_ms) { timeout = time_ms; }

void NtpClient::setInterval(uint16_t time_s) {
    syncInterval = time_s * ONE_SECOND_ms;
}

void NtpClient::setPoolServer(const char *str) {
    size_t len = strlen(str);
    timeServerPool = new char[len + 1];
    strcpy(timeServerPool, str);
}

void NtpClient::sendRequest() {
    IPAddress serverIp;
    if (!WiFi.hostByName(timeServerPool, serverIp)) {
        say_strP(str_dns, getStrP(str_error).c_str());
        active = false;
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

void NtpClient::waitResponse() {
    int cb = udp->parsePacket();
    if (cb) {
        responseTime = millis();

        char buf[NTP_PACKET_SIZE];
        udp->read(buf, NTP_PACKET_SIZE);
        uint16_t high = word(buf[40], buf[41]);
        uint16_t low = word(buf[42], buf[43]);

        epoch_s = (high << 16 | low) - SEVENTY_YEARS_ms;
        say_strP(str_response, epoch_s);
        if (responseHandler)
            responseHandler(epoch_s);
    }
}

void NtpClient::setOnResponse(TimeEventHandler h) {
    responseHandler = h;
    if (responseHandler && responseTime > 0)
        responseHandler(epoch_s + millis_since(responseTime) / ONE_SECOND_ms);
}

size_t NtpClient::printDiag(Print *p) {
    size_t n = println_nameP_value(p, str_active, boolStr(active).c_str());
    n += println_nameP_value(p, str_server, timeServerPool);
    n += println_nameP_value(p, str_port, NTP_LOCAL_PORT);
    n += println_nameP_value(p, str_epoch, epoch_s);
    n += println_nameP_value(p, str_interval, syncInterval / ONE_SECOND_ms);
    n += println_nameP_value(p, str_request, requestTime / ONE_SECOND_ms);
    n += println_nameP_value(p, str_response, responseTime / ONE_SECOND_ms);
    return n;
}
