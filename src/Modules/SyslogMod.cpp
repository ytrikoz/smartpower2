#include "Modules/SyslogMod.h"

#include "Wireless.h"

using namespace AppUtils;
using namespace StrUtils;
using namespace PrintUtils;

void SyslogMod::alert(String &str) { send(SYSLOG_ALERT, str); }

void SyslogMod::info(String &str) { send(SYSLOG_INFO, str); }

void SyslogMod::debug(String &str) { send(SYSLOG_DEBUG, str); }

bool SyslogMod::onInit() {
    setServer(config_->getValueAsString(SYSLOG_SERVER));
    setPort(SYSLOG_PORT);
    setHost(Wireless::hostName().c_str());

    udp = new WiFiUDP();

    return true;
}

bool SyslogMod::onStart() {
    if (String(server).length() == 0) {
        say_strP(str_disabled);
        serverIp = IPADDR_NONE;
        return false;
    }
    if (!WiFi.hostByName(server, serverIp)) {
        say_strP(str_dns, getStrP(str_error).c_str());
        return false;
    }
    String msg = FPSTR(str_start);

    send(SYSLOG_INFO, msg);
    char buf[32];
    sprintf(buf, "%s:%d", serverIp.toString().c_str(), port);
    say_strP(server, buf);
    return active = true;
}

void SyslogMod::onLoop() {}

void SyslogMod::onStop() { udp->stop(); }

void SyslogMod::setHost(const char *value) { setstr(host, value, 16); }

void SyslogMod::setServer(const char *value) { setstr(server, value, 16); }

void SyslogMod::setPort(const uint16_t value) { port = value; }

void SyslogMod::send(SysLogSeverity level, String &message) {
    if (!active)
        return;
    // String payload = getPayload(level, time, host.c_str(), message.c_str());
    // say_strP(str_log, payload.c_str());
    if (udp->beginPacket(serverIp, port)) {
        printPacket(udp, level, MOD_SYSLOG, message);
        udp->endPacket();
    }
}

void SyslogMod::printPacket(Print *p, const SysLogSeverity level,
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

size_t SyslogMod::onDiag(Print *p) {
    size_t n = println_nameP_value(p, str_active, boolStr(active).c_str());
    n += println_nameP_value(p, str_server, server);
    n += println_nameP_value(p, str_port, port);
    n += println_nameP_value(p, str_ip, serverIp);
    return n;
}

String getLevelStr(SysLogSeverity level) {
    PGM_P strP = str_unknown;
    switch (level) {
    case SYSLOG_ALERT:
        strP = str_alert;
        break;
    case SYSLOG_INFO:
        strP = str_info;
        break;
    case SYSLOG_DEBUG:
        strP = str_debug;
        break;
    }
    return String(FPSTR(strP));
}