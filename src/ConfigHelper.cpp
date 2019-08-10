#include "ConfigHelper.h"

#include <ESP8266WiFiType.h>
#include <FS.h>
#include "StrUtils.h"

using namespace StrUtils;

ConfigHelper::ConfigHelper() {
    filename = new char[FILENAME_MAX_LENGTH];
    config = new Config();
    config->setOnEvents([this](Parameter param) { onConfigEvent(param); });
}

ConfigHelper::~ConfigHelper() { delete[] filename; }

void ConfigHelper::onConfigEvent(Parameter param) {
#ifdef DEBUG_CONFIG
    debug->printf("[config] param #%d changed\r\n", param);
#endif
    synced = false;
}

Config *ConfigHelper::getConfig() { return config; }

void ConfigHelper::init(const char *name) {
    setstr(this->filename, name, FILENAME_MAX_LENGTH);
    if (!loadFile(filename)) reset();
}

bool ConfigHelper::loadFile(const char *name) {
#ifdef DEBUG_CONFIG
    debug->printf("[config] loading %s\r\n", name);
#endif
    if (SPIFFS.exists(name)) {
        File f = SPIFFS.open(name, "r");
        if (!f) {
#ifdef DEBUG_CONFIG
            debug->println("[config] file open failed");
#endif
            return false;
        }
        while (f.available()) {
            String str = f.readStringUntil('\n');
#ifdef DEBUG_CONFIG
            debug->printf("[config] <- %s\r\n", str.c_str());
#endif
            config->setValue(str);
        }
        f.close();
        return true;
    } else {
#ifdef DEBUG_CONFIG
        debug->println("[config] file not found");
#endif
        return false;
    }
}

void ConfigHelper::save() {
    if (!synced) {
#ifdef DEBUG_CONFIG
        debug->printf("[config] sync");
#endif
        synced = saveFile(filename);
    }
}

bool ConfigHelper::saveFile(const char *file) {
#ifdef DEBUG_CONFIG
    debug->printf("[config] saving %s\r\n", file);
#endif
    File f = SPIFFS.open(file, "w");
    if (!f) {
#ifdef DEBUG_CONFIG
        debug->println("[config] file open failed");
#endif
        return false;
    }
    char str[64];
    for (uint8_t i = 0; i < PARAM_COUNT; i++) {
        config->getConfigLine(Parameter(i), str);
#ifdef DEBUG_CONFIG
        debug->printf("[config] -> %s\r\n", str);
#endif
        f.println(str);
    }
    f.flush();
    f.close();
    return true;
}
void ConfigHelper::reload() {
#ifdef DEBUG_CONFIG
    debug->printf("[config] reload %s\r\n", filename);
#endif
    if (loadFile(filename)) synced = true;
}

void ConfigHelper::reset() {
    config->setDefault();
    save();
}

bool ConfigHelper::setBootPowerState(BootPowerState value) {
    return config->setValue(POWER, (uint8_t)(value));
}

BootPowerState ConfigHelper::getBootPowerState() {
    return BootPowerState(config->getByteValue(POWER));
}

String ConfigHelper::getConfigJson() {
    size_t size = 1024;
    DynamicJsonDocument doc(size);
    for (uint8_t i = 0; i < PARAM_COUNT; i++) {
        Parameter param = Parameter(i);
        JsonObject item = doc.createNestedObject();
        item[config->getName(param)] = config->getStrValue(param);
    }
    String str;
    serializeJson(doc, str);
    return str;
}

bool ConfigHelper::setNtpConfig(sint8_t timeZone_h, uint16_t sync_s) {
    return config->setValue(TIME_ZONE, timeZone_h) |
           config->setValue(NTP_SYNC_INTERVAL, sync_s);
}

bool ConfigHelper::setNetworkSTAConfig(uint8_t wifi, const char *ssid,
                                       const char *passwd, bool dhcp,
                                       const char *ipaddr, const char *netmask,
                                       const char *gateway, const char *dns) {
    bool changes = setWiFiMode((WiFiMode_t)wifi) | setSSID(ssid) |
                   setPassword(passwd) | setDHCP(dhcp) | setIPAddress(ipaddr) |
                   setNetmask(netmask) | setGateway(gateway) | setDNS(dns);
    return changes;
}

bool ConfigHelper::setPowerConfig(BootPowerState state, float voltage) {
    return setBootPowerState(state) | setOutputVoltage(voltage);
}

bool ConfigHelper::setWiFiMode(uint8_t value) {
    if (value >= WIFI_OFF && value <= WIFI_AP_STA) {
        return config->setValue(WIFI, value);
    }
    return false;
}

bool ConfigHelper::setWiFiMode(WiFiMode_t value) {
    return config->setValue(WIFI, (uint8_t)value);
}

bool ConfigHelper::setSSID(const char *value) {
    return config->setValue(SSID, value);
}

bool ConfigHelper::setPassword(const char *value) {
    return config->setValue(PASSWORD, value);
}

bool ConfigHelper::setIPAddress(IPAddress value) {
    return setIPAddress(value.toString().c_str());
}

bool ConfigHelper::setIPAddress(const char *value) {
    return config->setValue(IPADDR, value);
}

bool ConfigHelper::setGateway(const char *value) {
    return config->setValue(GATEWAY, value);
}

bool ConfigHelper::setNetmask(const char *value) {
    return config->setValue(NETMASK, value);
}

bool ConfigHelper::setDNS(const char *value) {
    return config->setValue(DNS, value);
}

bool ConfigHelper::setDHCP(bool value) {
    return config->setValue(DHCP, value);
}

bool ConfigHelper::setOutputVoltage(float value) {
    if ((value > 4 && value < 6) | (value > 11 && value < 13)) {
        return config->setValue(OUTPUT_VOLTAGE, value);
    }
    return false;
}

WiFiMode_t ConfigHelper::getWiFiMode() {
    return (WiFiMode_t)config->getByteValue(WIFI);
}
const char *ConfigHelper::getSSID() { return config->getStrValue(SSID); }
const char *ConfigHelper::getSSID_AP() { return config->getStrValue(AP_SSID); }
const char *ConfigHelper::getPassword() {
    return config->getStrValue(PASSWORD);
}
const char *ConfigHelper::getPassword_AP() {
    return config->getStrValue(AP_PASSWORD);
}
IPAddress ConfigHelper::getIPAddr_AP() {
    return config->getIPAddrValue(AP_IPADDR);
}
bool ConfigHelper::getDHCP() { return config->getBoolValue(DHCP); }
float ConfigHelper::getOutputVoltage() {
    float res = config->getFloatValue(OUTPUT_VOLTAGE);
    if (!(res > 4 && res < 6) && !(res > 11 && res < 13)) {
        USE_SERIAL.print(FPSTR(str_config));
        USE_SERIAL.print(FPSTR(str_invalid));
        USE_SERIAL.printf_P(strf_output_voltage, res);
        res = 5.0;
    }
    return res;
}
IPAddress ConfigHelper::getDNS() { return config->getIPAddrValue(DNS); }
IPAddress ConfigHelper::getNetmask() { return config->getIPAddrValue(NETMASK); }
IPAddress ConfigHelper::getGateway() { return config->getIPAddrValue(NETMASK); }
IPAddress ConfigHelper::getIPAddr() { return config->getIPAddrValue(IPADDR); }
const char *ConfigHelper::getIPAddrStr() { return config->getStrValue(IPADDR); }
const char *ConfigHelper::getNetmaskStr() {
    return config->getStrValue(NETMASK);
}
const char *ConfigHelper::getGatewayStr() {
    return config->getStrValue(GATEWAY);
}
const char *ConfigHelper::getDNSStr() { return config->getStrValue(DNS); }
// maximum value of RF Tx Power, unit: 0.25 dBm, range [0, 82]
uint8_t ConfigHelper::getTPW() { return config->getByteValue(TPW); }
