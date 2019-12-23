
#include "Config.h"

#include <Arduino.h>

#include "Utils/StrUtils.h"

using namespace StrUtils;

Config::Config() {
    for (size_t index = 0; index < CONFIG_ITEMS; ++index) {
        size_t size = getValueSize(ConfigItem(index));
        pool_[index].value = new char[size + 1];
        setstr(pool_[index].value, getValueDefault(index), size + 1);
    }
}

Config::~Config() {
    for (size_t index = 0; index < CONFIG_ITEMS; ++index)
        delete pool_[index].value;
}

bool Config::parseString(const String &str) {
    int index = str.indexOf("=");
    if (index == -1) return false;
    ConfigItem param;
    size_t size;
    if (getConfig(str.substring(0, index).c_str(), param, size))
        return setValue(param, str.substring(index + 2, str.length()).c_str());
    else
        return false;
}

void Config::resetDefault(ConfigItem param) {
    setValue(param, getValueDefault(param));
}

void Config::setOnChange(ConfigChangeEventHandler h) {
    onChangeEventHandler = h;
}

void Config::onChangedEvent(ConfigItem param) {
    if (onChangeEventHandler)
        onChangeEventHandler(param, getValue(param));
}

Param Config::getParam(size_t index) const { return pool_[index]; }

bool Config::exist(const char *name) const {
    for (uint8_t i = 0; i < CONFIG_ITEMS; ++i) {
        if (strcmp(name, getParamName(ConfigItem(i))) == 0)
            return true;
    }
    return false;
}

bool Config::getConfig(const char *name, ConfigItem &param) const {
    for (uint8_t i = 0; i < CONFIG_ITEMS; ++i) {
        param = ConfigItem(i);
        if (strcmp(name, getParamName(param)) == 0)
            return true;
    }
    return false;
}

bool Config::getConfig(const String &name, ConfigItem &param, size_t &size) const {
    return getConfig(name.c_str(), param, size);
}

bool Config::getConfig(const char *name, ConfigItem &param, size_t &size) const {
    bool result = getConfig(name, param);
    size = result ? getValueSize(param) : 0;
    return result;
}

String Config::toKeyValueJson(ConfigItem param) const {
    char buf[128];
    sprintf(buf, "\"%s\":%s", getParamName(param), getValue(param));
    return buf;
}

String Config::toString(ConfigItem param) const {
    char buf[128];
    sprintf(buf, "%s=\"%s\"", getParamName(param), getValue(param));
    return buf;
}

const char *Config::getParamName(ConfigItem param) const {
    return getParam(param).key_name;
}

size_t Config::getValueSize(uint8_t index) const {
    return getParam(ConfigItem(index)).value_size;
}

const char *Config::getValueDefault(uint8_t index) const {
    return getValueDefault(ConfigItem(index));
}

const char *Config::getValueDefault(ConfigItem param) const {
    return getParam(param).value_default;
}

bool Config::setValueBool(ConfigItem param, bool value) {
    char buf[8];
    return setValue(param, itoa(value, buf, DEC));
}

bool Config::setValueSignedByte(ConfigItem param, sint8_t value) {
    char buf[8];
    return setValue(param, itoa(value, buf, DEC));
}

bool Config::setValueByte(ConfigItem param, uint8_t value) {
    char buf[8];
    return setValue(param, itoa(value, buf, DEC));
}

bool Config::setValueInt(ConfigItem param, uint16_t value) {
    char buf[16];
    return setValue(param, itoa(value, buf, DEC));
}

bool Config::setValueFloat(ConfigItem param, float value) {
    char buf[16];
    return setValue(param, dtostrf(value, 2, 1, buf));
}

bool Config::setValueChar(ConfigItem param, const char ch) {
    char buf[2];
    buf[0] = ch;
    buf[1] = '\x00';
    return setValue(param, buf);
}

bool Config::setValueByName(const String &name, const String &value) {
    return setValueByName(name.c_str(), value.c_str());
}

int8_t Config::setValueByName(const char *name, const char *value) {
    ConfigItem param;
    int8_t res = -1;
    if (getConfig(name, param))
        res = setValue(param, value);
    return res;
}

bool Config::setValueAsString(const ConfigItem param, const String &value) {
    return setValue(param, value.c_str());
}

bool Config::setValue(const ConfigItem param, const char *value) {
    bool res = setstr(pool_[param].value, value, getValueSize(param) + 1);
    if (res)
        onChangedEvent(param);
    return res;
}

uint8_t Config::getValueAsByte(ConfigItem param) const {
    return atoi(getValue(param));
}

sint8_t Config::getValueAsSignedByte(ConfigItem param) const {
    return atoi(getValue(param));
}

uint16_t Config::getValueAsInt(ConfigItem param) const {
    return atoi(getValue(param));
}

float Config::getValueAsFloat(ConfigItem param) const {
    return atof(getValue(param));
}

bool Config::getValueAsBool(ConfigItem param) const {
    return (bool)atoi(getValue(param));
}

IPAddress Config::getValueAsIPAddress(ConfigItem param) const {
    return atoip(getValue(param));
}

const char *Config::getValue(ConfigItem param) const { return pool_[param].value; }