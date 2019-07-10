#pragma once
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266NetBIOS.h>

#include "consts.h"
#include "str_utils.h"
#include "ip_utils.h"
#include "sysinfo.h"

class NetworkServices
{
public:
    void setOutput(Stream *stream);
    void begin();                
    void loop();    
private:  
    Stream *output;   
    bool ready;

    bool begin_dns(const char *domain, IPAddress ip, uint16_t port);
    bool begin_mdns(const char *hostname, uint16_t telnet_port, uint16_t http_port, uint16_t ota_port);
    bool begin_netbios(const char *hostname);

    DNSServer *dns;    
    esp8266::MDNSImplementation::MDNSResponder *mdns;
    ESP8266NetBIOS *netbios;
};