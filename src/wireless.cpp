#include "wireless.h"

namespace wireless {

// https://github.com/esp8266/Arduino/issues/4114
class WiFiStationStaticIP : public ESP8266WiFiSTAClass {
   public:
    void useStaticStationIP(bool enabled) { _useStaticIp = enabled; }
};

void enableStaticStationIP(bool enabled) {
    WiFiStationStaticIP tmp;
    tmp.useStaticStationIP(enabled);
}

boolean disconnectWiFi() { return WiFi.disconnect(); }

bool isWiFiActive() {
    return WiFi.softAPgetStationNum() || WiFi.status() == WL_CONNECTED;
}

/*
void onConnected(const WiFiEventStationModeConnected& event){
  Connetect = false;

}
 */

}  // namespace wireless