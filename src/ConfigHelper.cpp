#include "ConfigHelper.h"

#include "FileStore.h"

using namespace StrUtils;

void ConfigHelper::handleParameterChanged(Parameter param) { synced = false; }

ConfigHelper::ConfigHelper() {
    size_t filename_len = strlen(FILE_CONFIG);
    this->filename = new char[filename_len + 1];
    strcpy(this->filename, FILE_CONFIG);

    this->config = new Config();
    loadConfig();
    this->config->setOnParameterChanged(
        [this](Parameter param) { handleParameterChanged(param); });
}

void ConfigHelper::loadConfig() {
    FileStore *store = new FileStore(filename);
    StringQueue *data = new StringQueue();
    if (!store->read(data)) {
        if (store->getError(SE_NOT_EXIST)) {
            saveConfig();
        } else {
            err->print(getIdentStrP(str_config));
            err->print(getStrP(str_load));
            err->println(store->getErrorInfo());
        }
    } else {
        if (data->available()) this->loadConfig(config, data);
    }
}

String  ConfigHelper::extractName(String &str) {
    int split_index = str.indexOf("=");
    if (split_index==-1)  
        return String("");
    else 
        return str.substring(0, split_index);
}

String  ConfigHelper::extractValue(String &str) {
    int split_index = str.indexOf("=");
    if (split_index==-1)  
        return String("");
    else 
        return str.substring(split_index + 2, str.length() - 2);
}

void ConfigHelper::loadConfig(Config *config, StringQueue *data) {
    String buf;
    while (data->available()) {
        data->get(buf);
        String paramStr = extractName(buf);
        String valueStr = extractValue(buf);
        if (paramStr.length() && valueStr.length()) {
            config->setValueStringByName(paramStr.c_str(), valueStr.c_str());
        } else  {
            err->print(getIdentStrP(str_config));
            err->print(getStrP(str_load));
            err->print(getStrP(str_error));
            err->println(buf);
        }
    }
}

bool ConfigHelper::saveConfig() {
    StringQueue *data = new StringQueue(PARAM_COUNT);
    return saveConfig(this->config, data);
}

bool ConfigHelper::saveConfig(Config *config, StringQueue *data) {
    for (uint8_t i = 0; i < PARAM_COUNT; ++i) {
        Parameter param = Parameter(i);
        String str = config->toString(param);
        data->put(str);
    }
    FileStore *store = new FileStore(FILE_CONFIG);
    return store->write(data);
}

size_t ConfigHelper::printTo(Print &p) const {
    size_t n = 0;
    for (size_t i = 0; i < PARAM_COUNT; ++i)
        n += p.println(config->toString(Parameter(i)));
    return n;
}

void ConfigHelper::setDefault() {
    for (size_t i = 0; i < PARAM_COUNT; ++i)
        config->setDefaultValue(Parameter(i));
}

String ConfigHelper::getConfigJson() {
    DynamicJsonDocument doc(1024);
    String str;
    for (uint8_t i = 0; i < PARAM_COUNT; ++i) {
        Parameter param = Parameter(i);
        JsonObject item = doc.createNestedObject();
        item[config->getName(param)] = config->getValueAsString(param);
    }
    serializeJson(doc, str);
    return str;
}

bool ConfigHelper::getWatthHoursLogEnabled() {
    return config->getValueAsBool(WH_STORE_ENABLED);
}

Config *ConfigHelper::get() { return config; }

bool ConfigHelper::setBootPowerState(BootPowerState value) {
    return config->setValueByte(POWER, (uint8_t)(value));
}

BootPowerState ConfigHelper::getBootPowerState() {
    return BootPowerState(config->getValueAsByte(POWER));
}

bool ConfigHelper::setNtpConfig(sint8_t timeZone_h, uint16_t sync_s) {
    return config->setValueSignedByte(TIME_ZONE, timeZone_h) |
           config->setValueInt(NTP_SYNC_INTERVAL, sync_s);
}

bool ConfigHelper::setNetworkSTAConfig(uint8_t wifi, const char *ssid,
                                       const char *passwd, bool dhcp,
                                       const char *ipaddr, const char *netmask,
                                       const char *gateway, const char *dns) {
    bool hasChanged = setWiFiMode((WiFiMode_t)wifi) | setSSID(ssid) |
                      setPassword(passwd) | setDHCP(dhcp) |
                      setIPAddress(ipaddr) | setNetmask(netmask) |
                      setGateway(gateway) | setDns(dns);
    synced |= hasChanged;
    return hasChanged;
}

bool ConfigHelper::setPowerConfig(BootPowerState state, float voltage) {
    return setBootPowerState(state) | setOutputVoltage(voltage);
}

bool ConfigHelper::setWiFiMode(uint8_t value) {
    if (value >= WIFI_OFF && value <= WIFI_AP_STA) {
        return config->setValueByte(WIFI, value);
    }
    return false;
}

bool ConfigHelper::setWiFiMode(WiFiMode_t value) {
    return config->setValueByte(WIFI, (uint8_t)value);
}

bool ConfigHelper::setSSID(const char *value) {
    return config->setValueString(SSID, value);
}

bool ConfigHelper::setPassword(const char *value) {
    return config->setValueString(PASSWORD, value);
}

bool ConfigHelper::setIPAddress(IPAddress value) {
    return setIPAddress(value.toString().c_str());
}

bool ConfigHelper::setIPAddress(const char *value) {
    return config->setValueString(IPADDR, value);
}

bool ConfigHelper::setGateway(const char *value) {
    return config->setValueString(GATEWAY, value);
}

bool ConfigHelper::setNetmask(const char *value) {
    return config->setValueString(NETMASK, value);
}

bool ConfigHelper::setDns(const char *value) {
    return config->setValueString(DNS, value);
}

bool ConfigHelper::setDHCP(bool value) { return config->setValueBool(DHCP, value); }

bool ConfigHelper::setOutputVoltage(float value) {
    bool result = (value > 4 && value < 6) || (value > 11 && value < 13);
    if (result) result = config->setValueFloat(OUTPUT_VOLTAGE, value);
    return result;
}

WiFiMode_t ConfigHelper::getWiFiMode() {
    return (WiFiMode_t)config->getValueAsByte(WIFI);
}

const char *ConfigHelper::getSSID() { return config->getValueAsString(SSID); }

const char *ConfigHelper::getSSID_AP() {
    return config->getValueAsString(AP_SSID);
}

const char *ConfigHelper::getPassword() {
    return config->getValueAsString(PASSWORD);
}

const char *ConfigHelper::getPassword_AP() {
    return config->getValueAsString(AP_PASSWORD);
}

IPAddress ConfigHelper::getIPAddr_AP() {
    return config->getValueAsIPAddress(AP_IPADDR);
}

bool ConfigHelper::getDHCP() { return config->getValueAsBool(DHCP); }

float ConfigHelper::getOutputVoltage() {
    float res = config->getValueAsFloat(OUTPUT_VOLTAGE);
    if (!(res > 4 && res < 6) && !(res > 11 && res < 13)) {
        USE_SERIAL.print(getIdentStrP(str_config));
        USE_SERIAL.print(getStrP(str_invalid));
        USE_SERIAL.print(getStrP(str_output));
        USE_SERIAL.println(getStrP(str_voltage));
        res = 5.0;
    }
    return res;
}
IPAddress ConfigHelper::getDns() { return config->getValueAsIPAddress(DNS); }

IPAddress ConfigHelper::getNetmask() {
    return config->getValueAsIPAddress(NETMASK);
}

IPAddress ConfigHelper::getGateway() {
    return config->getValueAsIPAddress(NETMASK);
}

IPAddress ConfigHelper::getIPAddr() {
    return config->getValueAsIPAddress(IPADDR);
}

const char *ConfigHelper::getIPAddrStr() {
    return config->getValueAsString(IPADDR);
}

const char *ConfigHelper::getNetmaskStr() {
    return config->getValueAsString(NETMASK);
}

const char *ConfigHelper::getGatewayStr() {
    return config->getValueAsString(GATEWAY);
}

const char *ConfigHelper::getDnsStr() { return config->getValueAsString(DNS); }

// maximum value of RF Tx Power, unit: 0.25 dBm, range [0, 82]
uint8_t ConfigHelper::getTPW() { return config->getValueAsByte(TPW); }
