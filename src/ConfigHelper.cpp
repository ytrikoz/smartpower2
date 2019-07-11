#include "ConfigHelper.h"

ConfigHelper *config;

ConfigHelper::ConfigHelper() {
    filename = new char[FILENAME_MAX_LENGTH];
    config = new Config();
    config->setOnEvents([this](Parameter param) { onConfigEvent(param); });
}

ConfigHelper::~ConfigHelper() { delete[] filename; }

void ConfigHelper::onConfigEvent(Parameter param) {
#ifdef DEBUG_CONFIG
    USE_DEBUG_SERIAL.printf("[config] param #%d changed\r\n", param);
#endif
    synced = false;
}

Config *ConfigHelper::getData() { return config; }

void ConfigHelper::init(const char *name) {
    str_utils::setstr(this->filename, name, FILENAME_MAX_LENGTH);
    if (!loadFile(filename)) {
        reset();
    }
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
            getData()->setValue(str);
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
#ifdef DEBUG_CONFIG
    debug->printf("[config] synced %s\r\n", synced ? "yes" : "no");
#endif
    if (!synced) {
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
        getData()->getNameValuePair(Parameter(i), str);
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
    if (loadFile(filename)) {
        synced = true;
    }
}

void ConfigHelper::reset() {
    getData()->setDefault();
    save();
}

bool ConfigHelper::setBootPowerState(BootPowerState value) {
    return getData()->setValue(POWER, (uint8_t)(value));
}

BootPowerState ConfigHelper::getBootPowerState() {
    return BootPowerState(getIntValue(POWER));
}

PowerState ConfigHelper::getLastPowerState() {
    if (!SPIFFS.exists(FILE_LAST_POWER_STATE)) {
        File f = SPIFFS.open(FILE_LAST_POWER_STATE, "w");
        f.println(POWER_OFF);
        f.flush();
        f.close();
        return POWER_OFF;
    }

    File f = SPIFFS.open(FILE_LAST_POWER_STATE, "r");
    PowerState value = PowerState(f.readString().toInt());
    f.close();
    return value;
}

void ConfigHelper::setLastPowerState(PowerState state) {
    File f = SPIFFS.open(FILE_LAST_POWER_STATE, "w");
    f.println(state);
    f.flush();
    f.close();
}

String ConfigHelper::getConfigJson() {
    size_t size = 1024;
    DynamicJsonDocument doc(size);
    for (uint8_t i = 0; i < PARAM_COUNT; i++) {
        Parameter param = Parameter(i);
        JsonObject item = doc.createNestedObject();
        item[getData()->getName(param)] = getData()->getStrValue(param);
    }
    String str;
    serializeJson(doc, str);
    return str;
}

bool ConfigHelper::setNtpConfig(sint8_t timeZone_h, uint16_t sync_s) {
    return getData()->setValue(TIME_ZONE, timeZone_h) |
           getData()->setValue(NTP_SYNC_INTERVAL, sync_s);
}

bool ConfigHelper::setNetworkSTAConfig(uint8_t wifi, const char *ssid,
                                       const char *passwd, bool dhcp,
                                       const char *ipaddr, const char *netmask,
                                       const char *gateway, const char *dns) {
    bool changes = setWiFiMode(wifi) | setSSID(ssid) | setPassword(passwd) |
                   setDHCP(dhcp) | setIPAddress(ipaddr) | setNetmask(netmask) |
                   setGateway(gateway) | setDNS(dns);
    return changes;
}

bool ConfigHelper::setPowerConfig(BootPowerState state, float voltage) {
    return setBootPowerState(state) | setOutputVoltage(voltage);
}

const char *ConfigHelper::getStrValue(Parameter param) {
    return getData()->getStrValue(param);
}

float ConfigHelper::getFloatValue(Parameter param) {
    return atof(getStrValue(param));
}

int ConfigHelper::getIntValue(Parameter param) {
    return atoi(getStrValue(param));
}

bool ConfigHelper::getBoolValue(Parameter param) {
    return (bool)atoi(getStrValue(param));
}

IPAddress ConfigHelper::getIPAddr(Parameter param) {
    return atoip(getStrValue(param));
}

bool ConfigHelper::setWiFiMode(uint8_t value) {
    if (value >= 1 && value <= 3) {
        return getData()->setValue(WIFI, value);
    }
    return false;
}

bool ConfigHelper::setSSID(const char *value) {
    return getData()->setValue(SSID, value);
}

bool ConfigHelper::setPassword(const char *value) {
    return getData()->setValue(PASSWORD, value);
}

bool ConfigHelper::setIPAddress(IPAddress value) {
    return setIPAddress(value.toString().c_str());
}

bool ConfigHelper::setIPAddress(const char *value) {
    return getData()->setValue(IPADDR, value);
}

bool ConfigHelper::setGateway(const char *value) {
    return getData()->setValue(GATEWAY, value);
}

bool ConfigHelper::setNetmask(const char *value) {
    return getData()->setValue(NETMASK, value);
}

bool ConfigHelper::setDNS(const char *value) {
    return getData()->setValue(DNS, value);
}

bool ConfigHelper::setDHCP(bool value) {
    return getData()->setValue(DHCP, value);
}

bool ConfigHelper::setOutputVoltage(float value) {
    if ((value > 4 && value < 6) | (value > 11 && value < 13)) {
        return getData()->setValue(OUTPUT_VOLTAGE, value);
    }
    return false;
}

uint8_t ConfigHelper::getWiFiMode() {
    // 1: Station 2: SoftAP 3: Station + SoftAP
    return getIntValue(WIFI);
}

const char *ConfigHelper::getSSID() { return getStrValue(SSID); }

const char *ConfigHelper::getSSID_AP() { return getStrValue(AP_SSID); }

const char *ConfigHelper::getPassword() { return getStrValue(PASSWORD); }

const char *ConfigHelper::getPassword_AP() { return getStrValue(AP_PASSWORD); }

IPAddress ConfigHelper::getIPAddr_AP() { return getIPAddr(AP_IPADDR); }

bool ConfigHelper::getDHCP() { return getBoolValue(DHCP); }

float ConfigHelper::getOutputVoltage() { return getFloatValue(OUTPUT_VOLTAGE); }
IPAddress ConfigHelper::getDNS() { return getIPAddr(DNS); }
IPAddress ConfigHelper::getNetmask() { return getIPAddr(NETMASK); }
IPAddress ConfigHelper::getGateway() { return getIPAddr(NETMASK); }
IPAddress ConfigHelper::getIPAddr() { return getIPAddr(IPADDR); }
const char *ConfigHelper::getIPAddrStr() { return getStrValue(IPADDR); }
const char *ConfigHelper::getNetmaskStr() { return getStrValue(NETMASK); }
const char *ConfigHelper::getGatewayStr() { return getStrValue(GATEWAY); }
const char *ConfigHelper::getDNSStr() { return getStrValue(DNS); }

uint8_t ConfigHelper::getTPW() {
    // maximum value of RF Tx Power, unit: 0.25 dBm, range [0, 82]
    return getIntValue(TPW);
}
