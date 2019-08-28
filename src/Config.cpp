#include "Config.h"

#include <Arduino.h>

#include "StrUtils.h"

using namespace StrUtils;

Config::Config() {
    for (size_t index = 0; index < PARAM_COUNT; ++index) {
        size_t value_size = getSize(Parameter(index));
        values[index] = new char[value_size + 1];
        setstr(values[index], getMetadata(index).default_value, value_size + 1);
    }
}

Config::~Config() {
    for (int index = 0; index < PARAM_COUNT; ++index) delete[] values[index];
}

void Config::setDefaultValue(Parameter param) {
    setValueString(param, getDefaultValue(param));
}

void Config::setOnParameterChanged(ParameterChangedEventHandler handler) {
    onParameterChanged = handler;
}

void Config::onChangedEvent(Parameter param) {
    if (onParameterChanged) onParameterChanged(param);
}

Metadata Config::getMetadata(size_t index) { return metadata[index]; }

const char *Config::getValueAsString(Parameter param) { return values[param]; }

bool Config::getParameter(const char *name, Parameter &param) {
    for (uint8_t i = 0; i < PARAM_COUNT; ++i) {
        param = Parameter(i);
        if (strcmp(name, getName(param)) == 0) return true;
    }
    return false;
}

bool Config::getParameter(const char *name, Parameter &param, size_t &size) {
    bool result = getParameter(name, param);
    size = result ? getSize(param) : 0;
    return result;
}

String Config::toString(Parameter param) {
    char buf[128];
    sprintf(buf, "%s=\"%s\"", getName(param), getValueAsString(param));
    return String(buf);
}

const char *Config::getName(Parameter param) { return getMetadata(param).name; }

const size_t Config::getSize(Parameter param) {
    return getMetadata(param).size;
}

const char *Config::getDefaultValue(Parameter param) {
    return getMetadata(param).default_value;
}

bool Config::setValueBool(Parameter param, bool value) {
    char buf[8];
    return setValueString(param, itoa(value, buf, DEC));
}


bool Config::setValueSignedByte(Parameter param, sint8_t value) {
    char buf[8];
    return setValueString(param, itoa(value, buf, DEC));
}

bool Config::setValueByte(Parameter param, uint8_t value) {
    char buf[8];
    return setValueString(param, itoa(value, buf, DEC));
}

bool Config::setValueInt(Parameter param, uint16_t value) {
    char buf[16];
    return setValueString(param, itoa(value, buf, DEC));
}

bool Config::setValueFloat(Parameter param, float value) {
    char buf[8];
    return setValueString(param, dtostrf(value, 2, 1, buf));
}

bool Config::setValueChar(Parameter param, char ch) {
    char buf[2];
    buf[0] = ch;
    buf[1] = '\x00';
    return setValueString(param, buf);
}

bool Config::setValueStringByName(const char *name, const char *value) {
    Parameter param;
    bool result = getParameter(name, param);
    if (result) {
        result = setValueString(param, value);
    }
    return result;
}

bool Config::setValueString(const Parameter param, const char *str) {
    bool result = setstr(values[param], str, getSize(param) + 1);
    if (result) onChangedEvent(param);
    return result;
}

bool Config::load(const char *str, size_t len) {
    size_t split;
    for (split = 0; split < len; ++split)
        if (str[split] == '=') break;
    if (split == len) return false;
    char buf[64];
    strncpy(buf, str, split);
    Parameter param;
    size_t size;
    if (getParameter(buf, param, size)) {
        strncpy(buf, &str[split + 2], strlen(str) - (split + 2));
        return setValueString(param, buf);
    }
    return false;
}

uint8_t Config::getValueAsByte(Parameter param) {
    return atoi(getValueAsString(param));
}

sint8_t Config::getValueAsSignedByte(Parameter param) {
    return atoi(getValueAsString(param));
}

uint16_t Config::getValueAsInt(Parameter param) {
    return atoi(getValueAsString(param));
}

float Config::getValueAsFloat(Parameter param) {
    return atof(getValueAsString(param));
}

bool Config::getValueAsBool(Parameter param) {
    return (bool)atoi(getValueAsString(param));
}

IPAddress Config::getValueAsIPAddress(Parameter param) {
    return atoip(getValueAsString(param));
}
