#pragma once

#include "Consts.h"

#include <Arduino.h>
#include <user_interface.h>

namespace SysInfo {

inline String getFW() {
    return APP_VERSION APP_BUILD_COMMIT;
}

inline String getSDK() {
    return system_get_sdk_version();
}

inline String getCore() {
    return ESP.getCoreVersion();     
}

String getUniqueName();

String getCpuFreq();

String getFreeSketch();

String getSketchSize();

String getFlashMap();

String getFlashSize();

String getVcc();

String getChipId();

String getNetworkJson();

String getSystemJson();

String getHeapStats();

String getFSStats();

uint8_t getAPClients();

String getAPClientsInfo();

} // namespace SysInfo