#pragma once

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
    bool begin_dns(const char *domain, IPAddress ip, uint16_t port);
    bool begin_mdns(const char *hostname, uint16_t telnet_port,
                    uint16_t http_port, uint16_t ota_port);
    bool begin_netbios(const char *hostname);
    Print *output;
    bool active;
    DNSServer *dns;
    esp8266::MDNSImplementation::MDNSResponder *mdns;
    ESP8266NetBIOS *netbios;
};