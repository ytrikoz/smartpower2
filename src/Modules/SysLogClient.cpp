#include "Modules/SyslogClient.h"

#include "Wireless.h"

SyslogClient::SyslogClient() : AppModule(MOD_SYSLOG) { udp = new WiFiUDP(); }

SyslogClient::SyslogClient(WiFiUDP *udp) : AppModule(MOD_SYSLOG) {
    this->udp = udp;
}

void SyslogClient::alert(String str) { send(SYSLOG_ALERT, str); }

void SyslogClient::info(String str) { send(SYSLOG_INFO, str); }

void SyslogClient::debug(String str) { send(SYSLOG_DEBUG, str); }

void SyslogClient::setConfig(Config *cfg) {
    setServer("tower");
    setPort(514);
}

void SyslogClient::setServer(const char *str) {
    size_t len = strlen(str);
    server = new char[len + 1];
    strcpy(server, str);
}

void SyslogClient::setPort(const uint16_t udp_port) { port = udp_port; }

void SyslogClient::send(SysLogSeverity level, String message) {
    unsigned long time = millis();
    const char *host = Wireless::getConfigHostname().c_str();
    String payload = getPayload(level, time, host, message.c_str());
}

//<%prio%>[%time% ] %host %message%
String SyslogClient::getPayload(const SysLogSeverity level, unsigned long time,
                                const char *host, const char *message) {
    int prio = SYSLOG_FACILITY * 8 + (int)level;
    char buf[128];
    sprintf_P(buf, "<%d>[%lu ]%s %s", prio, time, host, message);
    return String(buf);
}
