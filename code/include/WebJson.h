#pragma once

#include "ArduinoJson.h"
#include "Global.h"

namespace WebJson {

inline void fillMain(JsonObject& obj) {
    Modules::Psu* ps = app.psu();
    obj[FPSTR(str_power)] = (uint8_t)ps->isPowerOn();
    obj["v"] = ps->getInfo().V;
    obj["i"] = ps->getInfo().I;
    obj["p"] = ps->getInfo().P;
    obj["wh"] = ps->getInfo().Wh;
}

inline void fillOptions(JsonObject& obj) {
    Config* cfg = config->get();
    // General
    obj[FPSTR(str_bootpwr)] = cfg->getValueAsByte(POWER);
    obj[FPSTR(str_voltage)] = cfg->getValueAsFloat(OUTPUT_VOLTAGE);
    obj[FPSTR(str_storewh)] = cfg->getValueAsByte(WH_STORE_ENABLED);
    // Live

    // Network    
    obj[FPSTR(str_wifi)] = cfg->getValueAsByte(WIFI);
    obj[FPSTR(str_ssid)] = cfg->getValue(SSID);
    obj[FPSTR(str_passwd)] = cfg->getValue(PASSWORD);
    obj[FPSTR(str_dhcp)] = cfg->getValueAsByte(DHCP);
    obj[FPSTR(str_ipaddr)] = cfg->getValue(IPADDR);
    obj[FPSTR(str_netmask)] = cfg->getValue(NETMASK);
    obj[FPSTR(str_gateway)] = cfg->getValue(GATEWAY);
    obj[FPSTR(str_dns)] = cfg->getValue(DNS);
    obj[FPSTR(str_ap_ssid)] = cfg->getValue(AP_SSID);
    obj[FPSTR(str_ap_passwd)] = cfg->getValue(AP_PASSWORD);
    obj[FPSTR(str_ap_ipaddr)] = cfg->getValue(AP_IPADDR);
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