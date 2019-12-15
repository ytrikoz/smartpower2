#include "Modules/SyslogModule.h"

#include "Wireless.h"

using namespace AppUtils;
using namespace StrUtils;
using namespace PrintUtils;

void SyslogModule::alert(const String& src, const String &str) { send(SYSLOG_ALERT, src, str); }

void SyslogModule::info(const String& src, const String &str) { send(SYSLOG_INFO, src, str); }

void SyslogModule::debug(const String& src, const String &str) { send(SYSLOG_DEBUG, src, str); }


SyslogModule::SyslogModule(): Module() {}

SyslogModule::SyslogModule(WiFiUDP* udp):SyslogModule() {
    udp_ = udp;
}

bool SyslogModule::onInit() {
  if (udp_ == nullptr) udp_ = new WiFiUDP();
  return udp_;
}

bool SyslogModule::onStart() {
    if (String(getSyslogServer()).isEmpty()) {        
        serverIp = IPADDR_NONE;
        error_ = Error(ERROR_PARAM, str_server);
        return false;
    }

    if (!WiFi.hostByName(server, serverIp)) {
        error_= Error(ERROR_NETWORK, FPSTR(str_dns));
        return false;
    }    
    return true;
}

void SyslogModule::onLoop() {}

void SyslogModule::onStop() { udp_->stop(); }

const char* SyslogModule::getSyslogServer() {
     return config_->getValueAsString(SYSLOG_SERVER);
}

const char* SyslogModule::getHostname() {
     return APP_NAME;
}

void SyslogModule::send(SysLogSeverity level, const String &routine, const String &message) {
    // String payload = getPayload(level, time, host.c_str(), message.c_str());
    // say_strP(str_log, payload.c_str());
    if (udp_->beginPacket(serverIp, SYSLOG_PORT)) {
        printPacket(level, routine, message);
        udp_->endPacket();
    }
}

void SyslogModule::printPacket(const SysLogSeverity level, const String &routine, const String &message) {
    udp_->print('<');
    udp_->print(SYSLOG_FACILITY * 8 + (int)level);
    udp_->print('>');
    udp_->print(host);
    udp_->print(' ');
    udp_->print('[');
    udp_->print(routine);
    udp_->print(']');
    udp_->print(' ');
    udp_->print(message);
}

size_t SyslogModule::onDiag(Print *p) {
    DynamicJsonDocument doc(64);
    doc[FPSTR(str_server)] = getSyslogServer();
    return serializeJsonPretty(doc, *p);
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