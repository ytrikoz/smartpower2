
#include "Config.h"

#include <Arduino.h>

#include "StrUtils.h"

using namespace StrUtils;

Config::Config() {
    for (size_t i = 0; i < CONFIG_ITEMS; ++i) {
        size_t len = getSize(ConfigItem(i)) + 1;
        values[i] = new char[len];
        setstr(values[i], getDefaultValue(i), len);
    }
}

Config::~Config() {
    for (int index = 0; index < CONFIG_ITEMS; ++index)
        delete[] values[index];
}

bool Config::load(const char *str, size_t len) {
    char buf[128];
    size_t index;
    for (index = 0; index < len; ++index)
        if (str[index] == '=')
            break;
    if (index == len)
        return false;
    strncpy(buf, str, index);
    ConfigItem param;
    size_t size;
    if (getConfig(buf, param, size)) {
        strncpy(buf, &str[index + 2], strlen(str) - (index + 2));
        return setValueAsString(param, buf);
    }
    return false;
}

void Config::resetDefault(ConfigItem param) {
    setValueAsString(param, getDefaultValue(param));
}

void Config::setOnConfigChaged(ConfigChangeEventHandler h) {
    onChangeEventHandler = h;
}

void Config::onChangedEvent(ConfigItem param) {
    if (onChangeEventHandler)
        onChangeEventHandler(param);
}

ConfigDefinition Config::getDefinition(size_t index) { return define[index]; }

bool Config::getConfig(const char *name, ConfigItem &param) {
    for (uint8_t i = 0; i < CONFIG_ITEMS; ++i) {
        param = ConfigItem(i);
        if (strcmp(name, getName(param)) == 0)
            return true;
    }
    return false;
}

bool Config::getConfig(const char *name, ConfigItem &param, size_t &size) {
    bool result = getConfig(name, param);
    size = result ? getSize(param) : 0;
    return result;
}

const String Config::asString(const ConfigItem param) {
    char buf[128];
    sprintf(buf, "%s=\"%s\"", getName(param), getValueAsString(param));
    return String(buf);
}

const char *Config::getName(ConfigItem param) {
    return getDefinition(param).key_name;
}

const size_t Config::getSize(ConfigItem param) {
    return getDefinition(param).value_size;
}

const char *Config::getDefaultValue(uint8_t index) {
    return getDefaultValue(ConfigItem(index));
}

const char *Config::getDefaultValue(ConfigItem param) {
    return getDefinition(param).default_value;
}

bool Config::setValueBool(ConfigItem param, bool value) {
    char buf[8];
    return setValueAsString(param, itoa(value, buf, DEC));
}

bool Config::setValueSignedByte(ConfigItem param, sint8_t value) {
    char buf[8];
    return setValueAsString(param, itoa(value, buf, DEC));
}

bool Config::setValueByte(ConfigItem param, uint8_t value) {
    char buf[8];
    return setValueAsString(param, itoa(value, buf, DEC));
}

bool Config::setValueInt(ConfigItem param, uint16_t value) {
    char buf[16];
    return setValueAsString(param, itoa(value, buf, DEC));
}

bool Config::setValueFloat(ConfigItem param, float value) {
    char buf[16];
    return setValueAsString(param, dtostrf(value, 2, 1, buf));
}

bool Config::setValueChar(ConfigItem param, const char ch) {
    char buf[2];
    buf[0] = ch;
    buf[1] = '\x00';
    return setValueAsString(param, buf);
}

bool Config::setValueAsStringByName(const char *name, const char *value) {
    ConfigItem param;
    return getConfig(name, param) && setValueAsString(param, value);
}

bool Config::setValueAsString(const ConfigItem param, String &value) {
    return setValueAsString(param, value.c_str());
}

bool Config::setValueAsString(const ConfigItem param, const char *value) {
    bool result = setstr(values[param], value, getSize(param) + 1);
    if (result)
        onChangedEvent(param);
    return result;
}

uint8_t Config::getValueAsByte(ConfigItem param) {
    return atoi(getValueAsString(param));
}

sint8_t Config::getValueAsSignedByte(ConfigItem param) {
    return atoi(getValueAsString(param));
}

uint16_t Config::getValueAsInt(ConfigItem param) {
    return atoi(getValueAsString(param));
}

float Config::getValueAsFloat(ConfigItem param) {
    return atof(getValueAsString(param));
}

bool Config::getValueAsBool(ConfigItem param) {
    return (bool)atoi(getValueAsString(param));
}

IPAddress Config::getValueAsIPAddress(ConfigItem param) {
    return atoip(getValueAsString(param));
}

const char *Config::getValueAsString(ConfigItem param) { return values[param]; }