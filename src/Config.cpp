#include "Config.h"

#include "str_utils.h"
using namespace str_utils;

Config::Config() {
    for (uint8_t i = 0; i < PARAM_COUNT; i++) {
        values[i] = new char[metadata[i].size];
        setstr(values[i], '\x00', metadata[i].size);
    }
}

Config::~Config() {
    for (int i = 0; i < PARAM_COUNT; i++) {
        delete[] values[i];
    }
}

void Config::setDefault() {
    for (int i = 0; i < PARAM_COUNT; i++) {
        setstr(values[i], metadata[i].def, metadata[i].size);
    }
}

void Config::setOnEvents(ConfigEventHandler eventHandler) {
    onEvents = eventHandler;
}

void Config::onConfigChangeEvent(Parameter param) {
    if (onEvents) onEvents(param);
}

const char *Config::getStrValue(Parameter param) { return values[param]; }

bool Config::setValue(Parameter param, bool value) {
    char buf[2];
    return setValue(param, itoa(value, buf, DEC));
}

bool Config::setValue(Parameter param, sint8_t value) {
    char buf[8];
    return setValue(param, itoa(value, buf, DEC));
}

bool Config::setValue(Parameter param, uint8_t value) {
    char buf[8];
    return setValue(param, itoa(value, buf, DEC));
}

bool Config::setValue(Parameter param, uint16_t value) {
    char buf[16];
    return setValue(param, itoa(value, buf, DEC));
}

bool Config::setValue(Parameter param, float value) {
    char buf[8];
    return setValue(param, dtostrf(value, 2, 1, buf));
}

bool Config::setValue(Parameter param, const char value) {
    char buf[2];
    buf[0] = value;
    buf[1] = '\x00';
    return setValue(param, buf);
}

bool Config::setValue(const char *name, const char *value) {
    bool result = false;
    Parameter param;
    if (getParameter(name, param)) result = setValue(param, value);
    return result;
}

bool Config::setValue(Parameter param, const char *value) {
    bool changed =
        setstr(values[param], value, metadata[param].size);
#ifdef DEBUG_CONFIG
    DEBUG->printf("[config] set #%d: %s\r\n", param, value);
#endif
    if (changed) onConfigChangeEvent(param);
    return changed;
}

void Config::setValue(String str) {
    uint8_t split = str.indexOf('=');

    char buf[split];
    str.toCharArray(buf, split + 1);
    Parameter param;
    if (getParameter(buf, param)) {
        setValue(param, str.substring(split + 2, str.length() - 2).c_str());
    } else {
#ifdef DEBUG_CONFIG
        DEBUG->printf("[config] unknown param %s", buf);
#endif
    }
}

uint8_t Config::getByteValue(Parameter param) {
    return atoi(getStrValue(param));
}

sint8_t Config::getSignedValue(Parameter param) {
    return atoi(getStrValue(param));
}

uint16_t Config::getIntValue(Parameter param) {
    return atoi(getStrValue(param));
}

bool Config::getBoolValue(Parameter param) {
    return (bool)atoi(getStrValue(param));
}

IPAddress Config::getIPAddrValue(Parameter param) {
    return atoip(getStrValue(param));
}

bool Config::getParameter(const char *name, Parameter &param) {
    for (uint8_t i = 0; i < PARAM_COUNT; i++) {
        if (strcmp(name, metadata[i].name) == 0) {
            param = Parameter(i);
            return true;
        }
    }
    return false;
}

bool Config::getParameter(const char *name, Parameter &param, size_t &size) {
    if (getParameter(name, param)) {
        size = metadata[param].size;
        return true;
    }
    return false;
}

void Config::getConfigLine(Parameter param, char *str) {
    sprintf(str, "%s=\"%s\"", getName(param), getStrValue(param));
}

const char *Config::getName(Parameter param) { return metadata[param].name; }

const size_t Config::getValueSize(Parameter param) {
    return metadata[param].size;
}
