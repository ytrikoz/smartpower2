#pragma once

#include "ArduinoJson.h"
#include "Global.h"

namespace WebJson {

inline void fillMain(JsonObject& obj) {
    Modules::Psu* ps = app.psu();
    obj[FPSTR(str_power)] = (uint8_t)ps->isPowerOn();
    obj["wh"] = ps->getInfo().Wh;
}

inline void fillOptions(JsonObject& obj) {
    Config* cfg = config->get();
    for(uint8_t i = 0; i < CONFIG_ITEMS; ++i) {
        ConfigItem param = ConfigItem(i);
        obj[cfg->getParamName(param)] = cfg->getValue(param);
    }
}

inline void fillInfo(JsonObject& obj) {
    obj[FPSTR(str_file)] = SysInfo::getFSStats();
    obj[FPSTR(str_heap)] = SysInfo::getHeapStats();
}

inline void updateVersionJson() {
    DynamicJsonDocument doc(256);
    JsonObject obj = doc.createNestedObject();
    obj[FPSTR(str_fw)] = SysInfo::getFW();
    obj[FPSTR(str_sdk)] = SysInfo::getSDK();
    obj[FPSTR(str_core)] = SysInfo::getCore();
    obj[FPSTR(str_cpu)] = SysInfo::getCpuFreq();
    obj[FPSTR(str_chip)] = SysInfo::getChipId();
    File f = SPIFFS.open(FS_VERSION_JSON, "w");
    serializeJson(doc, f);
    f.close();
}

inline String getPageData(const WebPageEnum page) {
    DynamicJsonDocument doc(512);
    JsonObject obj = doc.createNestedObject();
    switch (page) {
        case PAGE_HOME: {
            fillMain(obj);
            break;
        }
        case PAGE_OPTIONS: {
            fillOptions(obj);
            break;
        }
        case PAGE_INFO: {
            fillInfo(obj);
            break;
        }
    }
    String json;
    serializeJson(doc, json);
    return json;
}  

}