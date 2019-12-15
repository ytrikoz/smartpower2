#pragma once

#include <DNSServer.h>
#include <ESP8266NetBIOS.h>
#include <ESP8266mDNS.h>

#include "Module.h"
#include "StrUtils.h"
#include "SysInfo.h"
#include "Wireless.h"

class NetworkService : public Module {
   public:
    NetworkService() : Module(){};
    
    size_t onDiag(Print *p) override;

    bool isCompatible(NetworkMode value) override {
        return (value != NetworkMode::NETWORK_OFF);
    }

    bool isNetworkDepended() override { return true; }


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