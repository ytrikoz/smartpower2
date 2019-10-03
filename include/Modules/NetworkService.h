#pragma once

#include <DNSServer.h>
#include <ESP8266NetBIOS.h>
#include <ESP8266mDNS.h>

#include "AppModule.h"
#include "StrUtils.h"
#include "SysInfo.h"
#include "Wireless.h"

class NetworkService : public AppModule {
  public:
    NetworkService();
    bool begin();
    void stop();
    void loop();
    size_t printDiag(Print *p);

  private:
    bool begin_dns(String &hostname);
    bool begin_mdns(String &hostname);
    bool begin_netbios(String &hostname);

    bool active;

    bool has_dns;
    bool has_mdns;
    bool has_netbios;

    DNSServer *dns;
    esp8266::MDNSImplementation::MDNSResponder *mdns;
    ESP8266NetBIOS *netbios;
};