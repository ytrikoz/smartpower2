
#include "Config.h"

#include <Arduino.h>

#include "StrUtils.h"

Config::Config() {
    for (size_t index = 0; index < PARAM_COUNT; ++index) {
        ConfigDefine df = getDefine(index);
        size_t size = getSize(ConfigItem(index));
        values[index] = new char[df.value_size + 1];
        StrUtils::setstr(values[index], getDefine(index).defaults, size + 1);
    }
}

Config::~Config() {
    for (int index = 0; index < PARAM_COUNT; ++index)
        delete[] values[index];
}

void Config::setDefaultValue(ConfigItem param) {
    String value = String(getDefaults(param));
    setValueString(param, value.c_str());
}

void Config::setOnConfigChaged(ConfigChangeEventHandler h) {
    onChangeEventHandler = h;
}

void Config::onChangedEvent(ConfigItem param) {
    if (onChangeEventHandler)
        onChangeEventHandler(param);
}

ConfigDefine Config::getDefine(size_t index) { return define[index]; }

const char *Config::getValueAsString(ConfigItem param) { return values[param]; }

bool Config::getConfig(const char *name, ConfigItem &param) {
    for (uint8_t i = 0; i < PARAM_COUNT; ++i) {
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

String Config::toString(ConfigItem param) {
    char buf[128];
    sprintf(buf, "%s=\"%s\"", getName(param), getValueAsString(param));
    return String(buf);
}

const char *Config::getName(ConfigItem param) {
    return getDefine(param).key_name;
}

const size_t Config::getSize(ConfigItem param) {
    return getDefine(param).value_size;
}

const char *Config::getDefaults(ConfigItem param) {
    return getDefine(param).defaults;
}

bool Config::setValueBool(ConfigItem param, bool value) {
    char buf[8];
    return setValueString(param, itoa(value, buf, DEC));
}

bool Config::setValueSignedByte(ConfigItem param, sint8_t value) {
    char buf[8];
    return setValueString(param, itoa(value, buf, DEC));
}

bool Config::setValueByte(ConfigItem param, uint8_t value) {
    char buf[8];
    return setValueString(param, itoa(value, buf, DEC));
}

bool Config::setValueInt(ConfigItem param, uint16_t value) {
    char buf[16];
    return setValueString(param, itoa(value, buf, DEC));
}

bool Config::setValueFloat(ConfigItem param, float value) {
    char buf[8];
    return setValueString(param, dtostrf(value, 2, 1, buf));
}

bool Config::setValueChar(ConfigItem param, char ch) {
    char buf[2];
    buf[0] = ch;
    buf[1] = '\x00';
    return setValueString(param, buf);
}

bool Config::setValueStringByName(const char *name, const char *value) {
    ConfigItem param;
    return getConfig(name, param) && setValueString(param, value);
}

bool Config::setValueString(const ConfigItem param, String &str) {
    return setValueString(param, str.c_str());
}

bool Config::setValueString(const ConfigItem param, const char *str) {
    bool result = StrUtils::setstr(values[param], str, getSize(param) + 1);
    if (result)
        onChangedEvent(param);
    return result;
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
        return setValueString(param, buf);
    }
    return false;
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
    return StrUtils::atoip(getValueAsString(param));
}
