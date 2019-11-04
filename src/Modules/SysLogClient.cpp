#include "Modules/SyslogClient.h"

#include "Wireless.h"

#include "AppModule.h"

using namespace AppUtils;
using namespace StrUtils;
using namespace PrintUtils;

SyslogClient::SyslogClient() : AppModule(MOD_SYSLOG) {
    log_buffer = new MemoryBuffer(512);
    udp = new WiFiUDP();
}

SyslogClient::SyslogClient(WiFiUDP *udp) : AppModule(MOD_SYSLOG) {
    this->udp = udp;
}

void SyslogClient::alert(String &str) { send(SYSLOG_ALERT, str); }

void SyslogClient::info(String &str) { send(SYSLOG_INFO, str); }

void SyslogClient::debug(String &str) { send(SYSLOG_DEBUG, str); }

bool SyslogClient::begin() {
    String msg = getStrP(str_start);
    start();
    if (active)
        send(SYSLOG_INFO, msg);
    return active;
}

void SyslogClient::start() {
    active = false;
    if (strcmp(server, "") == 0) {
        say_strP(str_disabled);
        serverIp = IPADDR_NONE;
        return;
    }
    if (!WiFi.hostByName(server, serverIp)) {
        say_strP(str_dns, getStrP(str_error).c_str());
        return;
    }
    char buf[32];
    sprintf(buf, "%s:%d", serverIp.toString().c_str(), port);
    say_strP(server, buf);
    active = true;
}

void SyslogClient::loop() {}

void SyslogClient::stop() {
    if (active) {
        udp->stop();
        say_strP(str_stopped);
    }
    active = false;
}

void SyslogClient::setConfig(Config *cfg) {
    setServer(config->getValueAsString(SYSLOG_SERVER));
    setPort(SYSLOG_PORT);
    setHost(Wireless::hostName().c_str());
}

void SyslogClient::setHost(const char *value) { setstr(host, value, 16); }

void SyslogClient::setServer(const char *value) { setstr(server, value, 16); }

void SyslogClient::setPort(const uint16_t value) { port = value; }

void SyslogClient::send(SysLogSeverity level, String &message) {
    if (!active)
        return;
    // String payload = getPayload(level, time, host.c_str(), message.c_str());
    // say_strP(str_log, payload.c_str());
    if (udp->beginPacket(serverIp, port)) {
        printPacket(udp, level, MOD_SYSLOG, message);
        udp->endPacket();
    }
}

void SyslogClient::printPacket(Print *p, const SysLogSeverity level,
                               AppModuleEnum mod, String &message) {
    p->print('<');
    p->print(SYSLOG_FACILITY * 8 + (int)level);
    p->print('>');
    p->print(host);
    p->print(' ');
    p->print('[');
    p->print(AppUtils::getModuleName(mod));
    p->print(']');
    p->print(' ');
    p->print(message);
}

size_t SyslogClient::printDiag(Print *p) {
    size_t n = println_nameP_value(p, str_active, boolStr(active).c_str());
    n += println_nameP_value(p, str_server, server);
    n += println_nameP_value(p, str_port, port);
    n += println_nameP_value(p, str_ip, serverIp);
    return n;
}

const char *getLevelStr(SysLogSeverity level) {
    switch (level) {
    case SYSLOG_ALERT:
        return getStrP(str_alert).c_str();
        break;
    case SYSLOG_INFO:
        return getStrP(str_info).c_str();
        break;
    case SYSLOG_DEBUG:
        return getStrP(str_debug).c_str();
        break;
    default:
        return getStrP(str_unknown).c_str();
        break;
    }
}