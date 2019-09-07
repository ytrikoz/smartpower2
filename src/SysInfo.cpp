#include "SysInfo.h"

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <FS.h>

#include "Strings.h"
#include "StrUtils.h"

const char *flashChipMode[] = {"QIO", "QOUT", "DIO", "DOUT"};
const char *wifiPhyMode[] = {"b", "g", "n"};

String getWiFiPhyMode() {
    String str = F("801.11");
    str += wifiPhyMode[WiFi.getPhyMode() - 1];
    return str;
}

String getWiFiRSSI() {
    String str = String(WiFi.RSSI());
    str += F("dB");
    return str;
}

String getCpuFreq() {
    String str = String(system_get_cpu_freq());
    str += F("MHz");
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
    size_t size = JSON_ARRAY_SIZE(2) + 2 * JSON_OBJECT_SIZE(1) + 64;
    DynamicJsonDocument doc(size);

    JsonObject item = doc.createNestedObject();
    item["fw"] = FW_VERSION;

    item = doc.createNestedObject();
    item["sdk"] = system_get_sdk_version();

    item = doc.createNestedObject();
    item["core"] = ESP.getCoreVersion();

    String str;

    serializeJson(doc, str);

    return str;
}

String getSystemInfoJson() {
    size_t size = JSON_ARRAY_SIZE(11) + 11 * JSON_OBJECT_SIZE(1) + 193;
    DynamicJsonDocument doc(size);
    JsonObject item = doc.createNestedObject();
    item["cpuFreq"] = getCpuFreq();

    item = doc.createNestedObject();
    item["flashSize"] = getFlashSize();

    item = doc.createNestedObject();
    item["flashMap"] = getFlashMap();

    item = doc.createNestedObject();
    item["sketchSize"] = getSketchSize();

    item = doc.createNestedObject();
    item["freeSketch"] = getFreeSketch();

    item = doc.createNestedObject();
    item["fsUsed"] = getFSUsedSpace();

    item = doc.createNestedObject();
    item["fsTotal"] = getFSTotalSpace();

    item = doc.createNestedObject();
    item["flashMode"] = String(flashChipMode[ESP.getFlashChipMode()]);

    item = doc.createNestedObject();
    item["ChipSpeed"] = StrUtils::formatInMHz(ESP.getFlashChipSpeed());

    item = doc.createNestedObject();
    item["ChipSize"] = String(StrUtils::formatSize(ESP.getFlashChipSize()));

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

const char *rssi2human(sint8_t rssi) {
    if (rssi < -50) {
        return "fantastic";
    } else if (rssi <= -60) {
        return "great";
    } else if (rssi <= -75) {
        return "average";
    } else if (rssi <= -85) {
        return "poor";
    } else if (rssi <= -95) {
        return "unusable";
    }
    return "unknown";
}

String getNetworkInfoJson() {
    size_t size = JSON_ARRAY_SIZE(11) + 11 * JSON_OBJECT_SIZE(1) + 193;

    DynamicJsonDocument doc(size);

    JsonObject item = doc.createNestedObject();
    item["ssid"] = WiFi.SSID();

    item = doc.createNestedObject();
    item["bssid"] = WiFi.BSSIDstr();

    item = doc.createNestedObject();
    item["phy"] = getWiFiPhyMode();

    item = doc.createNestedObject();
    item["channel"] = getWifiChannel();

    item = doc.createNestedObject();
    item["rssi"] = getWiFiRSSI();

    item = doc.createNestedObject();
    item["mac"] = WiFi.macAddress();

    item = doc.createNestedObject();
    item["hostname"] = WiFi.hostname();

    item = doc.createNestedObject();
    item["ip"] = WiFi.localIP().toString();

    item = doc.createNestedObject();
    item["subnet"] = WiFi.subnetMask().toString();

    item = doc.createNestedObject();
    item["gateway"] = WiFi.gatewayIP().toString();

    item = doc.createNestedObject();
    item["dns"] = WiFi.dnsIP().toString();

    String str;

    serializeJson(doc, str);

    return str;
}

void printResetInfo(Print* p) {
    p->print(getIdentStrP(str_reset));    
    p->print(getStrP(str_reason));
    p->println(ESP.getResetReason());
    
    p->print(getIdentStrP(str_reset));    
    p->print(getStrP(str_info));
    p->println(ESP.getResetInfo());    
}

String getHeapStat() {
    uint32_t free;
    uint16_t max;
    uint8_t frag;
    ESP.getHeapStats(&free, &max, &frag);
    char buf[64];
    sprintf(buf, "free %s max %s frag %d%%", StrUtils::formatSize(free).c_str(),
            StrUtils::formatSize(max).c_str(), frag);
    return String(buf);
}
