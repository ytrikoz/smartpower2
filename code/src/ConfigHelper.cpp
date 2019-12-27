#include "ConfigHelper.h"

#include "Core/Storage.h"
#include "Utils/PrintUtils.h"

using namespace PrintUtils;

ConfigHelper::ConfigHelper(const char *name) : name_(nullptr) {
    setName(name);
}

ConfigHelper::~ConfigHelper() {
    delete name_;
}

const char *ConfigHelper::name() {
    return name_;
}

bool ConfigHelper::check() {
    return FSUtils::exist(name_);
}

void ConfigHelper::setName(const char *name) {
    if (name_ != nullptr) 
        delete name_;
    size_t size = strlen(name);
    name_ = new char[size + 1];
    strncpy(name_, name, size);
    name_[size] = '\x00';
}

void ConfigHelper::setOutput(Print *p) { 
    out_ = p; 
}

void ConfigHelper::load() {
    PrintUtils::print_ident(out_, FPSTR(str_config));
    PrintUtils::print(out_, name_);
    StringFile file(name_);
    if (file.read()) {
        auto data = file.get();
        if (data->available()) {
            while (data->available()) {
                String buf;
                data->pop(buf);
                String param = extractName(buf);
                String value = extractValue(buf);
                int res = obj_.setByName(param.c_str(), value.c_str());
                if (res == -1) {
                    PrintUtils::print(out_, FPSTR(str_error));
                    PrintUtils::print(out_, buf);
                } else
                    changed_ |= res;
            }
            PrintUtils::println(out_, FPSTR(str_done));
        } else {
            PrintUtils::print(out_, FPSTR(str_failed));
            PrintUtils::print(out_, file.getErrorInfo());
        }
    }
}

String ConfigHelper::extractName(const String &str) {
    String res;
    int split_index = str.indexOf("=");
    if (split_index != -1)
        res = str.substring(0, split_index);
    return res;
}

String ConfigHelper::extractValue(const String &str) {
    String res;
    int split_index = str.indexOf("=");
    if (split_index != -1)
        res = str.substring(split_index + 2, str.length() - 2);
    return res;
}

bool ConfigHelper::save(bool backup) {
    if (backup) FSUtils::move(FS_MAIN_CONFIG, FS_MAIN_CONFIG ".bak");
    auto file = StringFile(FS_MAIN_CONFIG);
    auto data = file.get();
    for (size_t i = 0; i < PARAMS_COUNT; ++i) {
        String buf(obj_.toString(ConfigItem(i)));
        data->push(buf);
    }
    return file.write();
}

size_t ConfigHelper::printTo(Print &p) const {
    size_t n = 0;
    for (size_t i = 0; i < PARAMS_COUNT; ++i) {
        ConfigItem param = ConfigItem(i);
        n += p.println(obj_.toString(param));
    }
    return n;
}

//TODO
bool ConfigHelper::isSecured(ConfigItem i) {
    return i == PASSWD || i == PASSWORD || i == AP_PASSWORD;
}

void ConfigHelper::setDefaultConfig() {
    for (size_t i = 0; i < PARAMS_COUNT; ++i)
        obj_.setDefault(ConfigItem(i));
}

bool ConfigHelper::getWhStoreEnabled() {
    return obj_.asBool(WH_STORE_ENABLED);
}

NetworkMode ConfigHelper::getWiFiMode() {
    return (NetworkMode) obj_.asByte(WIFI);    
}

Config *ConfigHelper::get() { return &obj_; }

bool ConfigHelper::setBootPowerState(BootPowerState value) {
    return obj_.setByte(POWER, (uint8_t)(value));
}

BootPowerState ConfigHelper::getBootPowerState() {
    return BootPowerState(obj_.asByte(POWER));
}

bool ConfigHelper::setNtpConfig(sint8_t timeZone_h, uint16_t sync_s) {
    return obj_.setSignedByte(TIME_ZONE, timeZone_h) |
           obj_.setInt(NTP_SYNC_INTERVAL, sync_s);
}

bool ConfigHelper::setNetworkSTAConfig(uint8_t wifi, const char *ssid,
                                       const char *passwd, bool dhcp,
                                       const char *ipaddr, const char *netmask,
                                       const char *gateway, const char *dns) {
    bool hasChanged = setWiFiMode((WiFiMode_t)wifi) | setSSID(ssid) |
                      setPassword(passwd) | setDHCP(dhcp) |
                      setIPAddress(ipaddr) | setNetmask(netmask) |
                      setGateway(gateway) | setDns(dns);
    changed_ |= hasChanged;
    return hasChanged;
}

bool ConfigHelper::setPowerConfig(BootPowerState state, float voltage) {
    return setBootPowerState(state) && setOutputVoltage(voltage);
}

bool ConfigHelper::setWiFiMode(uint8_t value) {
    return (value >= WIFI_OFF) && (value <= WIFI_AP_STA)
               ? obj_.setByte(WIFI, value)
               : false;
}

bool ConfigHelper::setWiFiMode(WiFiMode_t value) {
    return obj_.setByte(WIFI, (uint8_t)value);
}

bool ConfigHelper::setSSID(const char *value) {
    return obj_.set(SSID, value);
}

bool ConfigHelper::setPassword(const char *value) {
    return obj_.set(PASSWORD, value);
}

bool ConfigHelper::setIPAddress(IPAddress value) {
    return setIPAddress(value.toString().c_str());
}

bool ConfigHelper::setIPAddress(const char *value) {
    return obj_.set(IPADDR, value);
}

bool ConfigHelper::setGateway(const char *value) {
    return obj_.set(GATEWAY, value);
}

bool ConfigHelper::setNetmask(const char *value) {
    return obj_.set(NETMASK, value);
}

bool ConfigHelper::setDns(const char *value) {
    return obj_.set(DNS, value);
}

bool ConfigHelper::setDHCP(bool value) {
    return obj_.setBool(DHCP, value);
}

float ConfigHelper::getOutputVoltage() {
    return obj_.asFloat(OUTPUT_VOLTAGE);
}

const char *ConfigHelper::getPassword() {
    return obj_.value(PASSWORD);
}

const char *ConfigHelper::getPassword_AP() {
    return obj_.value(AP_PASSWORD);
}

// maximum value of RF Tx Power, unit: 0.25 dBm, range [0, 82]
uint8_t ConfigHelper::getTPW() { return obj_.asByte(TPW); }

// String ConfigHelper::getConfigJson() {
//     DynamicJsonDocument doc(1024);
//     String str;
//     for (uint8_t i = 0; i < CONFIG_ITEMS; ++i) {
//         ConfigItem param = ConfigItem(i);
//         JsonObject item = doc.createNestedObject();
//         item[config->getName(param)] = config->getValueAsString(param);
//     }
//     serializeJson(doc, str);
//     return str;
// }