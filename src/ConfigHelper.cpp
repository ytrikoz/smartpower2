#include "ConfigHelper.h"

#include "FileStore.h"

void ConfigHelper::onConfigChanged(ConfigItem param) { stored = false; }

ConfigHelper::ConfigHelper() {
    this->filename = new char[sizeof(FILE_CONFIG)];
    strcpy(this->filename, FILE_CONFIG);

    this->config = new Config();
    loadConfig();
    this->config->setOnConfigChaged(
        [this](ConfigItem param) { onConfigChanged(param); });
}

void ConfigHelper::loadConfig() {
    FileStore *store = new FileStore(filename);
    StringQueue *data = new StringQueue();
    if (!store->read(data)) {
        if (store->getError(SE_NOT_EXIST)) {
            saveConfig();
        } else {
            err->print(StrUtils::getIdentStrP(str_config));
            err->print(StrUtils::getStrP(str_load));
            err->println(store->getErrorInfo());
        }
    } else {
        if (data->available())
            this->loadStrings(config, data);
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

bool ConfigHelper::loadStrings(Config *config, StringQueue *data) {
    String buf;
    unsigned long started = millis();
    while (data->available()) {
        data->get(buf);
        String paramStr = extractName(buf);
        String valueStr = extractValue(buf);
        if (paramStr.length() && valueStr.length()) {
            config->setValueStringByName(paramStr.c_str(), valueStr.c_str());
        } else {
            err->print(StrUtils::getIdentStrP(str_config));
            err->print(StrUtils::getStrP(str_load));
            err->print(StrUtils::getStrP(str_error));
            err->println(buf);
        }
        if (millis_since(started) > 10)
            break;
    }
    return data->available();
}

bool ConfigHelper::saveConfig() {
    StringQueue *data = new StringQueue(PARAM_COUNT);
    return saveConfig(this->config, data);
}

bool ConfigHelper::saveConfig(Config *config, StringQueue *data) {
    for (size_t index = 0; index < PARAM_COUNT; ++index) {
        String str = config->toString(ConfigItem(index));
        data->put(str);
    }
    FileStore *store = new FileStore(FILE_CONFIG);
    return store->write(data);
}

size_t ConfigHelper::printTo(Print &p) const {
    size_t n = 0;
    for (size_t i = 0; i < PARAM_COUNT; ++i)
        n += p.println(config->toString(ConfigItem(i)));
    return n;
}

void ConfigHelper::setDefault() {
    for (size_t i = 0; i < PARAM_COUNT; ++i)
        config->setDefaultValue(ConfigItem(i));
}

String ConfigHelper::getConfigJson() {
    DynamicJsonDocument doc(1024);
    String str;
    for (uint8_t i = 0; i < PARAM_COUNT; ++i) {
        ConfigItem param = ConfigItem(i);
        JsonObject item = doc.createNestedObject();
        item[config->getName(param)] = config->getValueAsString(param);
    }
    serializeJson(doc, str);
    return str;
}

bool ConfigHelper::getWhStoreEnabled() {
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
    stored |= hasChanged;
    return hasChanged;
}

bool ConfigHelper::setPowerConfig(BootPowerState state, float voltage) {
    return setBootPowerState(state) && setOutputVoltage(voltage);
}

bool ConfigHelper::setWiFiMode(uint8_t value) {
    return (value >= WIFI_OFF) && (value <= WIFI_AP_STA)
               ? config->setValueByte(WIFI, value)
               : false;
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

bool ConfigHelper::setDHCP(bool value) {
    return config->setValueBool(DHCP, value);
}

bool ConfigHelper::setOutputVoltage(float value) {
    return config->setValueFloat(OUTPUT_VOLTAGE, value);
}

float ConfigHelper::getOutputVoltage() {
    return config->getValueAsFloat(OUTPUT_VOLTAGE);
}

const char *ConfigHelper::getPassword() {
    return config->getValueAsString(PASSWORD);
}

const char *ConfigHelper::getPassword_AP() {
    return config->getValueAsString(AP_PASSWORD);
}
// maximum value of RF Tx Power, unit: 0.25 dBm, range [0, 82]
uint8_t ConfigHelper::getTPW() { return config->getValueAsByte(TPW); }
