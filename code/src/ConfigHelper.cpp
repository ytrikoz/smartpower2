#include "ConfigHelper.h"

#include "Storage.h"
#include "PrintUtils.h"

using namespace PrintUtils;

void ConfigHelper::onConfigChanged(ConfigItem param) { stored_ = false; }

ConfigHelper::ConfigHelper() {
    strncpy(name_, FS_MAIN_CONFIG, FILENAME_SIZE);
    if (!SPIFFS.exists(name_)) {
        print_ident(&Serial, FPSTR(str_config));
        println(&Serial, name_, FPSTR(str_not_exist), FPSTR(str_arrow_dest), FPSTR(str_save), FPSTR(str_default));
        save();
    }
    load();
    obj_.setOnChange([this](ConfigItem param) {
        onConfigChanged(param);
    });
}

void ConfigHelper::load() {
    print_ident(&Serial, FPSTR(str_config));
    print(&Serial, FPSTR(str_load), name_);
    auto file = StringFile(name_);
    auto data = file.get();
    if (file.read()) {
        if (data->available()) {
            while (data->available()) {
                String buf;
                data->pop(buf);
                String param_str = extractName(buf);
                String value_str = extractValue(buf);
                if (param_str.length()) {
                    obj_.setValueAsStringByName(param_str.c_str(), value_str.c_str());
                } else {
                    println(&Serial, FPSTR(str_load), FPSTR(str_error), ":", StrUtils::getQuotedStr(buf));
                }
            }
            print_ln(&Serial);
        } else {
            println(&Serial, FPSTR(str_failed), ":", FPSTR(str_empty));
        }
    } else {
        println(&Serial, FPSTR(str_failed), ":", file.getErrorInfo());
    }
}

String ConfigHelper::extractName(String &str) {
    int split_index = str.indexOf("=");
    if (split_index == -1)
        return String("");
    else
        return str.substring(0, split_index);
}

String ConfigHelper::extractValue(String &str) {
    int split_index = str.indexOf("=");
    if (split_index == -1)
        return String("");
    else
        return str.substring(split_index + 2, str.length() - 2);
}

bool ConfigHelper::save() {
    auto file = StringFile(FS_MAIN_CONFIG);
    auto data = file.get();
    for (size_t i = 0; i < CONFIG_ITEMS; ++i) {
        String buf(obj_.toString(ConfigItem(i)));
        data->push(buf);
    }
    return file.write();
}

size_t ConfigHelper::printTo(Print &p) const {
    size_t n = 0;
    for (size_t i = 0; i < CONFIG_ITEMS; ++i)
        n += p.println(obj_.toString(ConfigItem(i)));
    return n;
}

void ConfigHelper::setDefault() {
    for (size_t i = 0; i < CONFIG_ITEMS; ++i)
        obj_.resetDefault(ConfigItem(i));
}

bool ConfigHelper::getWhStoreEnabled() {
    return obj_.getValueAsBool(WH_STORE_ENABLED);
}

Config *ConfigHelper::get() { return &obj_; }

bool ConfigHelper::setBootPowerState(BootPowerState value) {
    return obj_.setValueByte(POWER, (uint8_t)(value));
}

BootPowerState ConfigHelper::getBootPowerState() {
    return BootPowerState(obj_.getValueAsByte(POWER));
}

bool ConfigHelper::setNtpConfig(sint8_t timeZone_h, uint16_t sync_s) {
    return obj_.setValueSignedByte(TIME_ZONE, timeZone_h) |
           obj_.setValueInt(NTP_SYNC_INTERVAL, sync_s);
}

bool ConfigHelper::setNetworkSTAConfig(uint8_t wifi, const char *ssid,
                                       const char *passwd, bool dhcp,
                                       const char *ipaddr, const char *netmask,
                                       const char *gateway, const char *dns) {
    bool hasChanged = setWiFiMode((WiFiMode_t)wifi) | setSSID(ssid) |
                      setPassword(passwd) | setDHCP(dhcp) |
                      setIPAddress(ipaddr) | setNetmask(netmask) |
                      setGateway(gateway) | setDns(dns);
    stored_ |= hasChanged;
    return hasChanged;
}

bool ConfigHelper::setPowerConfig(BootPowerState state, float voltage) {
    return setBootPowerState(state) && setOutputVoltage(voltage);
}

bool ConfigHelper::setWiFiMode(uint8_t value) {
    return (value >= WIFI_OFF) && (value <= WIFI_AP_STA)
               ? obj_.setValueByte(WIFI, value)
               : false;
}

bool ConfigHelper::setWiFiMode(WiFiMode_t value) {
    return obj_.setValueByte(WIFI, (uint8_t)value);
}

bool ConfigHelper::setSSID(const char *value) {
    return obj_.setValueAsString(SSID, value);
}

bool ConfigHelper::setPassword(const char *value) {
    return obj_.setValueAsString(PASSWORD, value);
}

bool ConfigHelper::setIPAddress(IPAddress value) {
    return setIPAddress(value.toString().c_str());
}

bool ConfigHelper::setIPAddress(const char *value) {
    return obj_.setValueAsString(IPADDR, value);
}

bool ConfigHelper::setGateway(const char *value) {
    return obj_.setValueAsString(GATEWAY, value);
}

bool ConfigHelper::setNetmask(const char *value) {
    return obj_.setValueAsString(NETMASK, value);
}

bool ConfigHelper::setDns(const char *value) {
    return obj_.setValueAsString(DNS, value);
}

bool ConfigHelper::setDHCP(bool value) {
    return obj_.setValueBool(DHCP, value);
}

bool ConfigHelper::setOutputVoltage(float value) {
    return obj_.setValueFloat(OUTPUT_VOLTAGE, value);
}

float ConfigHelper::getOutputVoltage() {
    return obj_.getValueAsFloat(OUTPUT_VOLTAGE);
}

const char *ConfigHelper::getPassword() {
    return obj_.getValueAsString(PASSWORD);
}

const char *ConfigHelper::getPassword_AP() {
    return obj_.getValueAsString(AP_PASSWORD);
}

// maximum value of RF Tx Power, unit: 0.25 dBm, range [0, 82]
uint8_t ConfigHelper::getTPW() { return obj_.getValueAsByte(TPW); }

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