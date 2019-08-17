#pragma once

#define DEBUG_ESP_MDNS_RESPONDER

#include <DNSServer.h>
#include <ESP8266NetBIOS.h>
#include <ESP8266mDNS.h>

#include "StrUtils.h"
#include "SysInfo.h"
#include "Wireless.h"

class NetworkService {
   public:
    NetworkService();
    void begin();
    void stop();
    void loop();
    void setOutput(Print *p);

   private:
    bool begin_dns();
    bool begin_mdns();
    bool begin_netbios();
    Print *output;

    bool active;
    
    bool has_dns;
    bool has_mdns;
    bool has_netbios;
    
    DNSServer *dns;
    esp8266::MDNSImplementation::MDNSResponder *mdns;
    ESP8266NetBIOS *netbios;
};