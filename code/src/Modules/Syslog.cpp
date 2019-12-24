#include "Modules/Syslog.h"

#include "Wireless.h"

namespace Modules {

void Syslog::alert(const String& src, const String& str) { send(SYSLOG_ALERT, src, str); }

void Syslog::info(const String& src, const String& str) { send(SYSLOG_INFO, src, str); }

void Syslog::debug(const String& src, const String& str) { send(SYSLOG_DEBUG, src, str); }

bool Syslog::onInit() {
    udp_ = new WiFiUDP();
    return udp_;
}

bool Syslog::onStart() {
    if (!strlen(config_->value(SYSLOG_SERVER))) {
        setError(Error(ERROR_PARAM, FPSTR(str_server)));
        return false;
    }

    if (!WiFi.hostByName(config_->value(SYSLOG_SERVER), ip_)) {
        setError(Error(ERROR_NETWORK, FPSTR(str_dns)));
        return false;
    }

    return true;
}

void Syslog::onLoop() {}

void Syslog::onStop() { udp_->stop(); }

const char* Syslog::getHostname() {
    return APP_NAME;
}

void Syslog::send(SysLogSeverity level, const String& routine, const String& message) {
    // String payload = getPayload(level, time, host.c_str(), message.c_str());
    // say_strP(str_log, payload.c_str());
    if (udp_->beginPacket(ip_, port_)) {
        printPacket(level, routine, message);
        udp_->endPacket();
    }
}

void Syslog::printPacket(const SysLogSeverity level, const String& routine, const String& message) {
    udp_->print('<');
    udp_->print(SYSLOG_FACILITY * 8 + (int)level);
    udp_->print('>');
    udp_->print(APP_NAME);
    udp_->print(' ');
    udp_->print('[');
    udp_->print(routine);
    udp_->print(']');
    udp_->print(' ');
    udp_->print(message);
}

void Syslog::onDiag(const JsonObject& doc) {
    doc[FPSTR(str_server)] = StrUtils::prettyIpAddress(ip_, port_);
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

}  // namespace Modules
