#include "Modules/Syslog.h"

#include "Wireless.h"

namespace Modules {

bool Syslog::onConfigChange(const ConfigItem param, const String& value) {
    if (param == SYSLOG_SERVER) {
        return setServer(value.c_str());
    }
    return true;
}

bool Syslog::setServer(const char* value) {
    if (!strlen(value)) return false;

    if (StrUtils::isip(value)) {
        ip_ = StrUtils::atoip(value);
        return true;
    }

    IPAddress buf;
    if (WiFi.hostByName(value, buf)) {
        ip_ = buf;
        return true;
    };

    setError(WRONG_PARAM, SYSLOG_SERVER);
    return false;
}

bool Syslog::onInit() {
    udp_ = new WiFiUDP();
    return udp_;
}

bool Syslog::onStart() {
    bool res = setServer(config_->value(SYSLOG_SERVER));
    return res;
}

void Syslog::onLoop() {}

void Syslog::onStop() { udp_->stop(); }

const char* Syslog::getHostname() {
    return APP_NAME;
}

void Syslog::log(LogSeverity level, const String& routine, const String& message) {
    // String payload = getPayload(level, time, host.c_str(), message.c_str());
    // say_strP(str_log, payload.c_str());
    if (udp_->beginPacket(ip_, port_)) {
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
        udp_->endPacket();
    }
}

void Syslog::onDiag(const JsonObject& doc) {
    doc[FPSTR(str_server)] = StrUtils::prettyIp(ip_, port_);
}

}  // namespace Modules
