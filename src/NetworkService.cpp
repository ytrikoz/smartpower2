#include "NetworkService.h"

#include "Consts.h"

NetworkService::NetworkService() {  
    has_dns = false;
    has_mdns = false;
    has_netbios = false;
    
    dns = new DNSServer();
    mdns = new esp8266::MDNSImplementation::MDNSResponder();
    netbios = new ESP8266NetBIOS();       

    active = false;
}

void NetworkService::setOutput(Print *p) { output = p; }

void NetworkService::begin() {
    IPAddress ip = Wireless::hostIP();
    const uint16_t dns_port = DNS_PORT;
    const uint16_t telnet_port = TELNET_PORT;
    const uint16_t http_port = HTTP_PORT;
    const uint16_t ota_port = OTA_PORT;
    char dns_name[PARAM_STR_SIZE + 1];
    char host_name[PARAM_STR_SIZE + 1];
   
    strcpy(host_name, HOST_NAME);
    strcpy(dns_name, "*");
    // strcpy(dns_name, HOST_NAME "." HOST_DOMAIN);
   
    if (Wireless::getWirelessMode() == WLAN_AP) {
        output->print(FPSTR(str_dns));
        if (begin_dns(dns_name, ip, dns_port)) {
            output->print(dns_name);
            output->print(' ');
            output->printf_P(strf_arrow_dest, StrUtils::getSocketStr(ip, dns_port).c_str());
            has_dns = true;
        } else {
            output->print(FPSTR(str_failed));
        }     
        output->println();
      
    }
    output->printf_P(str_netbios);
    if (begin_netbios(host_name)) {        
        output->printf_P(str_ready);
    } else {
        output->printf_P(str_failed);
    }
    output->println();

    has_mdns = begin_mdns(host_name, telnet_port, http_port, ota_port);

    active = true;
}

void NetworkService::stop() {
    if (has_dns) dns->stop();
    if (has_mdns) mdns->end();
    if (has_netbios) netbios->end();
    has_dns = false;
    has_mdns = false;
    has_netbios = false;
    active = false;
}

bool NetworkService::begin_dns(const char *domain, IPAddress ip,
                               uint16_t port) {
    return dns->start(port, domain, ip);
}

bool NetworkService::begin_mdns(const char *host_name, uint16_t telnet_port,
                                uint16_t http_port, uint16_t ota_port) {
    output->print(FPSTR(str_mdns));    
    if (mdns->begin(host_name)) {        
        mdns->addService(host_name, "telnet", "tcp", telnet_port);
        mdns->addService(host_name, "http", "tcp", http_port);
        mdns->enableArduino(ota_port);
        
        output->println(FPSTR(str_ready));
        return true;
    }
    output->println(FPSTR(str_failed));
    return false;
}

bool NetworkService::begin_netbios(const char *host_name) {
    return netbios->begin(host_name);
}

void NetworkService::loop() {
    if (!active) return;
    if (has_dns) dns->processNextRequest();
    if (has_mdns) mdns->update();
}
