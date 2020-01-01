#include "Modules/Syslog.h"
#include "Core/Error.h"
#include "Wireless.h"

namespace Modules {

Syslog::Syslog() : Module(), proto_(VisualSyslogServer(&tranport_)) {}

bool Syslog::log(const LogSeverity level, const char* tag, const char* message) {
    setError(proto_.send(level, tag, message));
    if (failed()) {
        PrintUtils::print(out_, getError());
        PrintUtils::println(out_);
    }
    return success();
}

void Syslog::onDiag(const JsonObject& doc) {
    doc[FPSTR(str_name)] = name_;
    doc[FPSTR(str_ip)] = StrUtils::prettyIp(tranport_.ip(), tranport_.port());
    doc[FPSTR(str_count)] = proto_.cnt();
    doc[FPSTR(str_total)] = StrUtils::prettyBytes(proto_.total());
}

bool Syslog::onConfigChange(const ConfigItem param, const String& value) {
    if (param == SYSLOG_SERVER) {
        PrintUtils::print_ident(out_, FPSTR(str_syslog));
        PrintUtils::println(out_, value);
        return init(value.c_str(), SYSLOG_PORT);
    }
    return true;
}

bool Syslog::onInit() {
    bool res = init(config_->value(SYSLOG_SERVER), SYSLOG_PORT);
    if (!res) {
        setError(WRONG_PARAM, SYSLOG_SERVER);
    }
    return res;
}

bool Syslog::onStart() {
    return log(LOG_INFO, String(FPSTR(str_syslog)).c_str(), String(FPSTR(str_start)).c_str());
}

void Syslog::setSource(SourceLog* source) {
    source_ = source;
}

bool Syslog::init(const char* name, uint16_t port) {
    bool res = false;

    size_t len = strlen(name);
    if (name_) delete name_;
    name_ = new char[len + 1];

    if (StrUtils::setstr(name_, name, len + 1)) {
        IPAddress ip = IPAddress(IPADDR_ANY);
        if (ip.fromString(name_) || WiFi.hostByName(name_, ip)) {
            tranport_.setup(ip, port);
            res = true;
        }
    };
    return res;
}

void Syslog::onLoop() {}

}  // namespace Modules
