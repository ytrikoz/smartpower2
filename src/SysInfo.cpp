#include "SysInfo.h"

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <FS.h>

#include "StrUtils.h"
#include "FSUtils.h"
#include "Strings.h"
#include "Wireless.h"

using namespace StrUtils;

namespace SysInfo {

const String getUniqueName() {
    String str(APP_SHORT "_");
    str += getChipId();
    return str;
}

const String getCpuFreq() {
    String str(system_get_cpu_freq());
    str += "MHz";
    return str;
}

const String getFreeSketch() {
    String str = prettyBytes(ESP.getFreeSketchSpace());
    return str;
}

const String getSketchSize() {
    String str = prettyBytes(ESP.getSketchSize());
    return str;
}

const String getFlashSize() {
    uint8_t flash_size_mbit;
    switch (system_get_flash_size_map()) {
        case FLASH_SIZE_2M:
            flash_size_mbit = 2;
            break;
        case FLASH_SIZE_4M_MAP_256_256:
            flash_size_mbit = 4;
            break;
        case FLASH_SIZE_8M_MAP_512_512:
            flash_size_mbit = 8;
            break;
        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            flash_size_mbit = 16;
            break;
        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
        case FLASH_SIZE_32M_MAP_2048_2048:
            flash_size_mbit = 32;
            break;
        case FLASH_SIZE_64M_MAP_1024_1024:
            flash_size_mbit = 64;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            flash_size_mbit = 128;
            break;
        default:
            flash_size_mbit = 0;
    };
    String str(flash_size_mbit);
    str += "Mb";
    return str;
}

const String getFlashMap() {
    String str;
    switch (system_get_flash_size_map()) {
        case FLASH_SIZE_4M_MAP_256_256:
            str = F("256KB+256KB");
            break;
        case FLASH_SIZE_2M:
            str = F("256KB");
            break;
        case FLASH_SIZE_8M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_512_512:
            str = F("512KB+512KB");
            break;
        case FLASH_SIZE_16M_MAP_1024_1024:
        case FLASH_SIZE_32M_MAP_1024_1024:
        case FLASH_SIZE_64M_MAP_1024_1024:
        case FLASH_SIZE_128M_MAP_1024_1024:
            str = F("1024KB+1024KB");
            break;
        case FLASH_SIZE_32M_MAP_2048_2048:
            str = F("2048KB+2048KB");
            break;
        default:
            str = FPSTR(str_unknown);
    };
    return str;
}

String getAPClientsInfo() {
    struct station_info *station = wifi_softap_get_station_info();
    struct station_info *next_station;
    char buf[32];
    String str;
    while (station) {
        str = "ip: ";
        sprintf(buf, IPSTR, IP2STR(&station->ip));
        str += buf;
        str += " bssid: ";
        sprintf(buf, MACSTR, MAC2STR(station->bssid));
        str += buf;
        str += "\r\n";
        next_station = STAILQ_NEXT(station, next);
        free(station);
        station = next_station;
    }
    return str;
}

String getVcc() {
    String str(ESP.getVcc() / 1000.0, 4);
    return str;
}

String getChipId() {
    String str(ESP.getChipId(), HEX);
    return str;
}

const String getVersionJson() {
    DynamicJsonDocument doc(256);

    JsonObject doc_0 = doc.createNestedObject();
    doc_0[FPSTR(str_fw)] = APP_VERSION APP_BUILD_COMMIT;

    JsonObject doc_1 = doc.createNestedObject();
    doc_1[FPSTR(str_sdk)] = system_get_sdk_version();

    JsonObject doc_2 = doc.createNestedObject();
    doc_2[FPSTR(str_core)] = ESP.getCoreVersion();
   
    String json;
    serializeJson(doc, json);
    return json;
}

String getSystemJson() {
    DynamicJsonDocument doc(256);

    JsonObject doc_0 = doc.createNestedObject();
    doc_0[FPSTR(str_cpu)] = getCpuFreq();

    JsonObject doc_1 = doc.createNestedObject();
    doc_1[FPSTR(str_chip)] = getChipId();

    JsonObject doc_2 = doc.createNestedObject();
    doc_2[FPSTR(str_file)] = getFSStats();

    JsonObject doc_3 = doc.createNestedObject();
    doc_3[FPSTR(str_heap)] = getHeapStats();

    String json;
    serializeJson(doc, json);
    return json;
}

String getNetworkJson() {
    DynamicJsonDocument doc(512);

    JsonObject doc_0 = doc.createNestedObject();
    doc_0[FPSTR(str_ssid)] = Wireless::hostSSID();

    JsonObject doc_1 = doc.createNestedObject();
    doc_1[FPSTR(str_phy)] = Wireless::getWiFiPhyMode();

    JsonObject doc_2 = doc.createNestedObject();
    doc_2[FPSTR(str_ch)] = Wireless::getWifiChannel();

    JsonObject doc_3 = doc.createNestedObject();
    doc_3[FPSTR(str_mac)] = Wireless::hostMac();

    JsonObject doc_4 = doc.createNestedObject();
    doc_4[FPSTR(str_host)] = Wireless::hostName();

    JsonObject doc_5 = doc.createNestedObject();
    doc_5[FPSTR(str_ip)] = Wireless::hostIP().toString();

    JsonObject doc_6 = doc.createNestedObject();
    doc_6[FPSTR(str_subnet)] =  Wireless::hostSubnet().toString();
    
    JsonObject doc_7 = doc.createNestedObject();
    doc_7[FPSTR(str_gateway)] =  Wireless::hostGateway().toString();
    
    JsonObject doc_8 = doc.createNestedObject();
    doc_8[FPSTR(str_dns)] = Wireless::hostDNS().toString();
    
    String json;
    serializeJson(doc, json);
    return json;
}

const String getFSStats() {
    return getFSUsed() + " / " + getFSTotal();
}

const String getHeapStats() {
    uint32_t free;
    uint16_t max;
    uint8_t frag;
    ESP.getHeapStats(&free, &max, &frag);
    String buf;
    buf = "free: ";
    buf += prettyBytes(free);
    buf += " frag: ";
    buf += frag;
    buf += '%';
    return buf;
}

}  // namespace SysInfo