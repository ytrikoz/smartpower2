#pragma once

#include <DNSServer.h>
#include <ESP8266NetBIOS.h>
#include <ESP8266mDNS.h>

#include "Module.h"
#include "StrUtils.h"
#include "SysInfo.h"
#include "Wireless.h"

class NetworkService : public NetworkModule {
   public:
    NetworkService() : NetworkModule(NetworkMode::NETWORK_AP){};  
    void onDiag(const JsonObject&) override;
   protected:
    bool onInit() override;
    void onStop() override;
    bool onStart() override;
    void onLoop() override;

   private:
    bool start_mdns();

    bool has_dns;
    bool has_mdns;
    bool has_netbios;

    String host_;
    String dns_name_;
    uint16_t dns_port_;
    IPAddress ip_;
    DNSServer *dns;
    esp8266::MDNSImplementation::MDNSResponder *mdns;
    ESP8266NetBIOS *netbios;
};