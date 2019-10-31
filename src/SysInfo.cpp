#include "SysInfo.h"

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <FS.h>

#include "StrUtils.h"
#include "Strings.h"

using namespace StrUtils;

const char *flashChipMode[] = {"QIO", "QOUT", "DIO", "DOUT"};

String getWiFiPhyMode() {
    char buf[8];
    strcpy(buf, "801.11");
    switch (WiFi.getPhyMode()) {
    case WIFI_PHY_MODE_11B:
        buf[6] = 'b';
        break;
    case WIFI_PHY_MODE_11G:
        buf[6] = 'g';
        break;
    case WIFI_PHY_MODE_11N:
        buf[6] = 'n';
        break;
    }
    buf[7] = '\x00';
    return String(buf);
}

String getWiFiRSSI() {
    String str = String(WiFi.RSSI());
    str += "dB";
    return str;
}

String getCpuFreq() {
    String str = String(system_get_cpu_freq());
    str += "MHz";
    return str;
}

String getFreeSketch() {
    String str = StrUtils::formatSize(ESP.getFreeSketchSpace());
    return str;
}

String getSketchSize() {
    String str = StrUtils::formatSize(ESP.getSketchSize());
    return str;
}

String getFlashSize() {
    flash_size_map flash_map = system_get_flash_size_map();
    uint8_t flash_size_mbit = 0;
    switch (flash_map) {
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
    };
    return String(flash_size_mbit) + F("Mbit");
}

String getFlashMap() {
    flash_size_map flash_map = system_get_flash_size_map();
    String str = "";
    switch (flash_map) {
    case FLASH_SIZE_4M_MAP_256_256:
        str += "256KB+256KB";
        break;
    case FLASH_SIZE_2M:
        str += "256KB";
        break;
    case FLASH_SIZE_8M_MAP_512_512:
    case FLASH_SIZE_16M_MAP_512_512:
    case FLASH_SIZE_32M_MAP_512_512:
        str += "512KB+512KB";
        break;
    case FLASH_SIZE_16M_MAP_1024_1024:
    case FLASH_SIZE_32M_MAP_1024_1024:
    case FLASH_SIZE_64M_MAP_1024_1024:
    case FLASH_SIZE_128M_MAP_1024_1024:
        str += "1024KB+1024KB";
        break;
    case FLASH_SIZE_32M_MAP_2048_2048:
        str += "2048KB+2048KB";
        break;
    };
    return str;
}

String getFSUsedSpace() {
    FSInfo fsi;
    SPIFFS.info(fsi);
    return StrUtils::formatSize(fsi.usedBytes);
}

String getFSTotalSpace() {
    FSInfo info;
    SPIFFS.info(info);
    return StrUtils::formatSize(info.totalBytes);
}

String getFSFileList() {
    FSInfo info;
    SPIFFS.info(info);

    String str = "FileList";
    str += " ";
    str += "{";
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
        String fileName = dir.fileName();
        size_t fileSize = dir.fileSize();
        str += "FileName";
        str += ":";
        str += fileName.c_str();
        str += ",";
        str += " ";
        str += "FileSize";
        str += ":";
        str += StrUtils::formatSize(fileSize);
    }
    str += " ";
    str += "}";

    return str;
}

String getVersionInfoJson() {
    const int items_count = 3;
    const int size =
        JSON_ARRAY_SIZE(items_count) + items_count * JSON_OBJECT_SIZE(2);
    DynamicJsonDocument doc(size);

    JsonObject item = doc.createNestedObject();
    item["fw"] = APP_VERSION;
    item = doc.createNestedObject();
    item["sdk"] = system_get_sdk_version();
    item = doc.createNestedObject();
    item["core"] = ESP.getCoreVersion();

    String str;
    serializeJson(doc, str);
    return str;
}

String getConnectedStationInfo() {
    struct station_info *station = wifi_softap_get_station_info();
    struct station_info *next_station;
    char buf[32];
    String str;
    while (station) {
        str = "bssid: ";
        sprintf(buf, MACSTR, MAC2STR(station->bssid));
        str += buf;

        str += "\tip: ";
        sprintf(buf, IPSTR, IP2STR(&station->ip));
        str += buf;
        str += "\r\n";

        next_station = STAILQ_NEXT(station, next);
        free(station);
        station = next_station;
    }
    return str;
}

String getWifiChannel() {
    String str = String(WiFi.channel());
    return str;
}

String getVcc() {
    String str = String(ESP.getVcc() / 1000.0, 4);
    return str;
}

String getChipId() { return String(ESP.getChipId(), HEX); }

String getJsonObj(PGM_P strP, String str) {
    char key[64];
    strcpy_P(key, strP);
    char buf[64];
    sprintf(buf, "{\"%s\":\"%s\"}", key, str.c_str());
    return String(buf);
}

String getNetworkInfoJson() {
    String str;
    if (!str.reserve(512))
        return str;
    str = "[";
    str += getJsonObj(str_ssid, WiFi.SSID());
    str += ",";
    str += getJsonObj(str_phy, getWiFiPhyMode());
    str += ",";
    str += getJsonObj(str_ch, getWifiChannel());
    str += ",";
    str += getJsonObj(str_mac, WiFi.macAddress());
    str += ",";
    str += getJsonObj(str_host, WiFi.hostname());
    str += ",";
    str += getJsonObj(str_ip, WiFi.localIP().toString());
    str += ",";
    str += getJsonObj(str_subnet, WiFi.subnetMask().toString());
    str += ",";
    str += getJsonObj(str_gateway, WiFi.gatewayIP().toString());
    str += ",";
    str += getJsonObj(str_dns, WiFi.dnsIP().toString());
    str += "]";
    return str;
}

String getSystemInfoJson() {
    String str;
    if (!str.reserve(128))
        return str;
    str = "[";
    str += getJsonObj(str_freq, getCpuFreq());
    str += ",";
    str += getJsonObj(str_used, getFSUsedSpace());
    str += ",";
    str += getJsonObj(str_total, getFSTotalSpace());
    str += "]";
    return str;
}

String getHeapStat() {
    uint32_t free = 0;
    uint16_t max = 0;
    uint8_t frag = 0;
    ESP.getHeapStats(&free, &max, &frag);
    char buf[64];
    sprintf(buf, "free %s max %s frag %d%%", StrUtils::formatSize(free).c_str(),
            StrUtils::formatSize(max).c_str(), frag);
    return String(buf);
}
