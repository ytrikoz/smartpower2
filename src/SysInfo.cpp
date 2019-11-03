#include "SysInfo.h"

#include <ESP8266WiFi.h>
#include <FS.h>

#include "StrUtils.h"
#include "Strings.h"
#include "Wireless.h"

using namespace StrUtils;

namespace SysInfo {

String getCpuFreq() {
    String str(system_get_cpu_freq());
    str += "MHz";
    return str;
}

String getFreeSketch() {
    String str = formatSize(ESP.getFreeSketchSpace());
    return str;
}

String getSketchSize() {
    String str = formatSize(ESP.getSketchSize());
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
    String str(flash_size_mbit);
    str += "Mbit";
    return str;
}

String getFlashMap() {
    flash_size_map flash_map = system_get_flash_size_map();
    String str;
    switch (flash_map) {
    case FLASH_SIZE_4M_MAP_256_256:
        str = "256KB+256KB";
        break;
    case FLASH_SIZE_2M:
        str = "256KB";
        break;
    case FLASH_SIZE_8M_MAP_512_512:
    case FLASH_SIZE_16M_MAP_512_512:
    case FLASH_SIZE_32M_MAP_512_512:
        str = "512KB+512KB";
        break;
    case FLASH_SIZE_16M_MAP_1024_1024:
    case FLASH_SIZE_32M_MAP_1024_1024:
    case FLASH_SIZE_64M_MAP_1024_1024:
    case FLASH_SIZE_128M_MAP_1024_1024:
        str = "1024KB+1024KB";
        break;
    case FLASH_SIZE_32M_MAP_2048_2048:
        str = "2048KB+2048KB";
        break;
    };
    return str;
}

String getFSUsedSpace() {
    FSInfo fsi;
    SPIFFS.info(fsi);
    String str = formatSize(fsi.usedBytes);
    return str;
}

String getFSTotalSpace() {
    FSInfo info;
    SPIFFS.info(info);
    String str = formatSize(info.totalBytes);
    return str;
}

String getFSFileList() {
    FSInfo info;
    SPIFFS.info(info);
    String str = "FileList";
    str += " {";
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
        str += formatSize(fileSize);
    }
    str += "}";
    return str;
}

String getClientsInfo() {
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

String getWifiChannel() {
    String str(WiFi.channel());
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

String getVersionJson() {
    String str;
    if (!str.reserve(128))
        return str;
    str += "[";
    str += asJsonObj(str_fw, APP_VERSION APP_BUILD_COMMIT);
    str += ",";
    str += asJsonObj(str_sdk, system_get_sdk_version());
    str += ",";
    str += asJsonObj(str_core, ESP.getCoreVersion());
    str += "]";
    return str;
}

String getNetworkJson() {
    String str;
    if (!str.reserve(256))
        return str;
    str += "[";
    str += asJsonObj(str_ssid, Wireless::hostSSID());
    str += ",";
    str += asJsonObj(str_phy, Wireless::getWiFiPhyMode());
    str += ",";
    str += asJsonObj(str_ch, Wireless::getWifiChannel());
    str += ",";
    str += asJsonObj(str_mac, Wireless::hostMac());
    str += ",";
    str += asJsonObj(str_host, Wireless::hostName());
    str += ",";
    str += asJsonObj(str_ip, Wireless::hostIP().toString());
    str += ",";
    str += asJsonObj(str_subnet, Wireless::hostSubnet().toString());
    str += ",";
    str += asJsonObj(str_gateway, Wireless::hostGateway().toString());
    str += ",";
    str += asJsonObj(str_dns, Wireless::hostDNS().toString());
    str += "]";
    return str;
}

String getSystemJson() {
    String str;
    if (!str.reserve(64))
        return str;
    str += "[";
    str += asJsonObj(str_cpu, getCpuFreq());
    str += ",";
    str += asJsonObj(str_used, getFSUsedSpace());
    str += ",";
    str += asJsonObj(str_total, getFSTotalSpace());
    str += "]";
    return str;
}

String getHeapStats() {
    uint32_t free = 0;
    uint16_t max = 0;
    uint8_t frag = 0;
    ESP.getHeapStats(&free, &max, &frag);
    char buf[32];
    sprintf(buf, "free: %s frag: %d%%", formatSize(free).c_str(), frag);
    return String(buf);
}

} // namespace SysInfo