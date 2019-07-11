#include "sysinfo.h"

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
    String str = str_utils::formatSize(ESP.getFreeSketchSpace());
    return str;
}

String getSketchSize() {
    String str = str_utils::formatSize(ESP.getSketchSize());
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
    return String(str_utils::formatSize(fsi.usedBytes));
}

String getFSTotalSpace() {
    FSInfo info;
    SPIFFS.info(info);

    String str = str_utils::formatSize(info.totalBytes);
    return str;
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
        str += str_utils::formatSize(fileSize);
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
    item["ChipSpeed"] = str_utils::getStrInMHz(ESP.getFlashChipSpeed());

    item = doc.createNestedObject();
    item["ChipSize"] = String(str_utils::formatSize(ESP.getFlashChipSize()));

    String str;

    serializeJson(doc, str);

    return str;
}

String getHostName() {
    String str = String(wifi_station_get_hostname());
    return str;
}

String getConnectedStationInfo() {
    struct station_info *station = wifi_softap_get_station_info();
    struct station_info *next_station;
    char buf[32];
    String str;
    while (station) {
        sprintf(buf, MACSTR, MAC2STR(station->bssid));
        str = "bssid: ";
        str += buf;
        sprintf(buf, IPSTR, IP2STR(&station->ip));
        str += "\tip: ";
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

String getHeapFree() {
    String str = str_utils::formatSize(ESP.getFreeHeap());
    return str;
}

String getChipId() { return String(ESP.getChipId(), HEX); }

const char *rssi2human(sint8_t rssi) {
    if (rssi < -50) {
        return "Fantastic";
    } else if (rssi <= -60) {
        return "Great";
    } else if (rssi <= -75) {
        return "Average";
    } else if (rssi <= -85) {
        return "Poor";
    } else if (rssi <= -95) {
        return "Unusable";
    }
    return "Unknown";
}

uint8_t rssi2per(sint8_t rssi) {
    int r;

    if (rssi > 200)
        r = 100;
    else if (rssi < 100)
        r = 0;
    else
        r = 100 - 2 * (200 - rssi);

    if (r > 100) r = 100;
    if (r < 0) r = 0;

    return r;
}

String getHostIPInfo() {
    station_status_t status = wifi_station_get_connect_status();

    String str = F("sta: ");
    switch (status) {
        case STATION_IDLE:
            str += F("idle");
        case STATION_CONNECTING:
            str += F("connecting");
        case STATION_WRONG_PASSWORD:
            str += F("wrong password");
        case STATION_NO_AP_FOUND:
            str += F("no ap found");
        case STATION_CONNECT_FAIL:
            str += F("connect fail");
        case STATION_GOT_IP: {
            str += F("got ip ");
            str += WiFi.localIP().toString();
        }
    }
    str += F(", reconnect policy: ");
    str += (wifi_station_get_reconnect_policy()) ? F("yes") : F("no");
    return str;
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
    item["hostname"] = getHostName();

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

String getHeapStatistic() {
    uint32_t free;
    uint16_t max;
    uint8_t frag;
    ESP.getHeapStats(&free, &max, &frag);
    char buf[64];
    sprintf(buf, "free %s max %s frag %d%%", str_utils::formatSize(free).c_str(),
            str_utils::formatSize(max).c_str(), frag);
    return String(buf);
}

String getWifiStaStatus() {
    station_status_t status = wifi_station_get_connect_status();
    String str = "";
    switch (status) {
        case STATION_GOT_IP:
            str = "connected";
        case STATION_NO_AP_FOUND:
            str = "no ap found";
        case STATION_CONNECT_FAIL:
            str = "connection failed";
        case STATION_WRONG_PASSWORD:
            str = "wrong password";
        case STATION_IDLE:
            str = "idle";
        default:
            str = "disconnected";
    }
    return str;
}

bool isWiFiActive() {
    return WiFi.softAPgetStationNum() || WiFi.status() == WL_CONNECTED;
}


