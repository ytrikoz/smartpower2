#include "ConfigHelper.h"

#include "Core/Storage.h"
#include "Utils/PrintUtils.h"

using namespace PrintUtils;

ConfigHelper::ConfigHelper(const char* name):name_(nullptr){
    setName(name);
 }

void ConfigHelper::setName(const char* name) {
    if (name_ != nullptr) delete name_;    
    size_t size = strlen(name);    
    name_ = new char[size + 1];
    strncpy(name_, name, size);
    name_[size] = '\x00';
}


const char* ConfigHelper::name() {
    return name_;
}

bool ConfigHelper::check() {
    return SPIFFS.exists(name_);
}

ConfigHelper::~ConfigHelper() {
    delete name_;
}

void ConfigHelper::setOutput(Print *p) { out_ = p; }

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
                String paramStr = extractName(buf);
                String valueStr = extractValue(buf);
                int8_t res = obj_.setValueByName(paramStr, valueStr);
                if (res == -1) {
                    PrintUtils::print(out_, FPSTR(str_error));
                    PrintUtils::print(out_, buf);
                }
                else                         
                    changed_ |= res;               
            }         
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

void ConfigHelper::setDefaultParams() {
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
    changed_ |= hasChanged;
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
    return obj_.setValue(SSID, value);
}

bool ConfigHelper::setPassword(const char *value) {
    return obj_.setValue(PASSWORD, value);
}

bool ConfigHelper::setIPAddress(IPAddress value) {
    return setIPAddress(value.toString().c_str());
}

bool ConfigHelper::setIPAddress(const char *value) {
    return obj_.setValue(IPADDR, value);
}

bool ConfigHelper::setGateway(const char *value) {
    return obj_.setValue(GATEWAY, value);
}

bool ConfigHelper::setNetmask(const char *value) {
    return obj_.setValue(NETMASK, value);
}

bool ConfigHelper::setDns(const char *value) {
    return obj_.setValue(DNS, value);
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
    return obj_.getValue(PASSWORD);
}

const char *ConfigHelper::getPassword_AP() {
    return obj_.getValue(AP_PASSWORD);
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