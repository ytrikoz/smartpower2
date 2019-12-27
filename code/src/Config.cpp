
#include "Config.h"

#include "Utils/StrUtils.h"

using namespace StrUtils;

Config::Config() {
    for (size_t i = 0; i < PARAMS_COUNT; ++i) {
        size_t size = getValueSize(i);
        pool_[i].value = new char[size + 1];
        setstr(pool_[i].value, getDefault(i), size + 1);
    }
}

Config::~Config() {
    for (size_t i = 0; i < PARAMS_COUNT; ++i) {
        delete pool_[i].value;
    }
}

bool Config::fromString(const String &str) {
    int index = str.indexOf("=");
    if (index == -1) return false;
    String name = str.substring(0, index);
    String value = str.substring(index + 2);
    ConfigItem param;
    size_t size;
    bool res = getParamByName(name.c_str(), param, size);
    if (res) {
        res = set(param, value.c_str());
    }
    return res;
}

void Config::setDefault(ConfigItem param) {
    set(param, getDefault(param));
}

void Config::setOnChange(ConfigChangeEventHandler h) {
    onChangeEventHandler = h;
}

void Config::onChangedEvent(ConfigItem param) {
    if (onChangeEventHandler) {
        onChangeEventHandler(param, value(param));
    }
}

bool Config::getParamByName(const char *name, ConfigItem &param, size_t &size) const {
    bool res = getParamByName(name, param);
    if (res) size = getValueSize(param);
    return res;
}

bool Config::getParamByName(const char *name, ConfigItem &param) const {
    bool res = false;
    for (uint8_t i = 0; i < PARAMS_COUNT; ++i) {
        if (strcmp_P(name, get(i)->key_name) == 0) {
            param = ConfigItem(i);
            res = true;
            break;
        }
    }
    return res;
}

String Config::toJsonPair(ConfigItem param) {
    char buf[128];
    sprintf(buf, "\"%s\":%s", name(param).c_str(), value(param));
    return buf;
}

String Config::toString(ConfigItem param) const {
    char buf[128];
    sprintf(buf, "%s=\"%s\"", name(param).c_str(), value(param));
    return buf;
}

bool Config::setBool(ConfigItem param, bool value) {
    char buf[8];
    return set(param, itoa(value, buf, DEC));
}

bool Config::setSignedByte(ConfigItem param, sint8_t value) {
    char buf[8];
    return set(param, itoa(value, buf, DEC));
}

bool Config::setByte(ConfigItem param, uint8_t value) {
    char buf[4];
    return set(param, itoa(value, buf, DEC));
}

bool Config::setInt(ConfigItem param, uint16_t value) {
    char buf[16];
    return set(param, itoa(value, buf, DEC));
}

bool Config::setFloat(ConfigItem param, float value) {
    char buf[16];
    return set(param, dtostrf(value, 5, 6, buf));
}

bool Config::setChar(ConfigItem param, const char ch) {
    char buf[2];
    buf[0] = ch;
    buf[1] = '\x00';
    return set(param, buf);
}

int Config::setByName(const char *name, const char *value) {
    ConfigItem param;
    int res = -1;
    if (getParamByName(name, param)) {
        res = set(param, value);
    }
    return res;
}

bool Config::set(const ConfigItem param, const char *value) {
    bool res = setstr(get(param)->value, value, getValueSize(param) + 1);
    if (res) {
        onChangedEvent(param);
    }
    return res;
}

bool Config::asBool(const ConfigItem param) const {
    return (bool)atoi(value(param));
}

uint8_t Config::asByte(const ConfigItem param) const {
    return atoi(value(param));
}

sint8_t Config::asSignedByte(const ConfigItem param) const {
    return atoi(value(param));
}

uint16_t Config::asInt(const ConfigItem param) const {
    return atoi(value(param));
}

float Config::asFloat(const ConfigItem param) const {
    return atof(value(param));
}

IPAddress Config::asIPAddress(const ConfigItem param) const {
    return atoip(value(param));
}

const char *Config::getDefault(const size_t index) const {
    return get(index)->value_default;
}

const String Config::name(const size_t index) const {
    PGM_P strP = get(index)->key_name;
    size_t len = strlen_P(strP);
    char buf[len + 1];
    strncpy_P(buf, strP, len);
    buf[len] = '\x00';
    return String(buf);
}

const char *Config::value(const size_t index) const {
    return get(index)->value;
}

size_t Config::getValueSize(const size_t index) const {
    return get(index)->value_size;
}

bool Config::exist(const char *name) const {
    bool res = false;
    for (size_t i = 0; i < PARAMS_COUNT; ++i) {
        if (strcmp_P(name, get(i)->key_name) == 0) {
            res = true;
            break;
        }
    }
    return res;
}

const Param *Config::get(const size_t index) const {
    return &pool_[index];
}
/*
const char *Config::getName(ConfigItem param) const {
    return getParam(param).key_name;
}
*/