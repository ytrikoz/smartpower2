#include "Modules/Syslog.h"
#include "Core/Error.h"
#include "Wireless.h"

namespace Modules {

Syslog::Syslog() : Module(), proto_(VisualSyslogServer(&tranport_)) {}

bool Syslog::log(const LogSeverity level, const char* tag, const char* message) {
    clearError();
    if (!proto_.send(level, tag, message)) {
        setError(ERROR_EXECUTE);
    }
    return ok();
}

void Syslog::onDiag(const JsonObject& doc) {
    doc[FPSTR(str_name)] = name_;
    doc[FPSTR(str_ip)] = StrUtils::prettyIp(tranport_.ip(), tranport_.port());
    doc[FPSTR(str_count)] = proto_.cnt();
    doc[FPSTR(str_total)] = StrUtils::prettyBytes(proto_.total());
}

bool Syslog::onConfigChange(const ConfigItem param, const String& value) {
    if (param == SYSLOG_SERVER) {
        start(true);        
    }
    return true;
}

bool Syslog::onInit() {
    clearError();
    setServer(config_->value(SYSLOG_SERVER), SYSLOG_PORT);
    return ok();
}

bool Syslog::onStart() {
    return log(LOG_INFO, String(FPSTR(str_syslog)).c_str(), String(FPSTR(str_start)).c_str());
}

void Syslog::setSource(StringPullable* source) {
    source_ = source;
}

void Syslog::setServer(const char* name, uint16_t port) {
    size_t len = strlen(name);
    if (name_) delete name_;
    name_ = new char[len + 1];
    memset(name_, 0, len + 1);

    if (strlen(name) == 0) {
        setError(WRONG_PARAM, SYSLOG_SERVER);
        return;
    }

    if (StrUtils::setstr(name_, name, len + 1)) {
        IPAddress ip = IPAddress(IPADDR_ANY);
        if (ip.fromString(name_) || WiFi.hostByName(name_, ip)) {
            tranport_.setup(ip, port);
        } else {
            setError(WRONG_PARAM, SYSLOG_SERVER);
        }
    };
}

void Syslog::onLoop() {
    if (source_) {
        String buf;
        if (source_->pull(buf)) {
            log(LOG_DEBUG, "main", buf.c_str());
        }
    }
}

}  // namespace Modules
