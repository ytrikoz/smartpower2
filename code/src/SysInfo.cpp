#include "SysInfo.h"

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <FS.h>

#include "Utils/StrUtils.h"
#include "Utils/FSUtils.h"
#include "Strings.h"
#include "Wireless.h"

using namespace StrUtils;

namespace SysInfo {

String getUniqueName() {
    String str(APP_SHORT "_");
    str += getChipId();
    return str;
}

String getCpuFreq() {
    String str(system_get_cpu_freq());
    str += "MHz";
    return str;
}

String getFreeSketch() {
    String str = prettyBytes(ESP.getFreeSketchSpace());
    return str;
}

String getSketchSize() {
    String str = prettyBytes(ESP.getSketchSize());
    return str;
}

String getFlashSize() {
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

String getFlashMap() {
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

String getNetworkJson() {
    DynamicJsonDocument doc(512);
    JsonObject jo;
    jo = doc.createNestedObject();
    jo[FPSTR(str_ssid)] = Wireless::hostSSID();
    jo = doc.createNestedObject();
    jo[FPSTR(str_phy)] = Wireless::wifiPhyMode();
    jo = doc.createNestedObject();
    jo[FPSTR(str_ch)] = Wireless::wifiChannel();
    jo = doc.createNestedObject();
    jo[FPSTR(str_mac)] = Wireless::hostMac();
    jo = doc.createNestedObject();
    jo[FPSTR(str_host)] = Wireless::hostName();
    jo = doc.createNestedObject();
    jo[FPSTR(str_ip)] = Wireless::hostIP().toString();
    jo = doc.createNestedObject();
    jo[FPSTR(str_subnet)] = Wireless::hostSubnet().toString();
    jo = doc.createNestedObject();
    jo[FPSTR(str_gateway)] = Wireless::hostGateway().toString();
    jo = doc.createNestedObject();
    jo[FPSTR(str_dns)] = Wireless::hostDNS().toString();
    String json;
    serializeJson(doc, json);
    return json;
}

String getFSStats() {
    return FSUtils::getFSUsed() + " / " + FSUtils::getFSTotal();
}

String getHeapStats() {
    uint32_t free;
    uint16_t max;
    uint8_t frag;
    ESP.getHeapStats(&free, &max, &frag);
    String buf;
    buf += prettyBytes(free);
    buf += " ";
    buf += frag;
    buf += '%';
    return buf;
}

}  // namespace SysInfo