#include "Modules/NetworkService.h"

#include "Consts.h"
#include "Wireless.h"

using namespace PrintUtils;
using namespace StrUtils;

bool NetworkService::onInit() {
    dns = new DNSServer();
    mdns = new esp8266::MDNSImplementation::MDNSResponder();
    netbios = new ESP8266NetBIOS();
    has_dns = has_mdns = has_netbios = false;
    return true;
}

bool NetworkService::onStart() {
    host_ = Wireless::hostName();
    dns_name_ = host_ + "." + HOST_DOMAIN;            
    ip_ = Wireless::hostIP();
    dns_port_ = DNS_PORT;

    if (!has_dns) 
        has_dns = dns->start(dns_port_, dns_name_, ip_);   
    if (!has_netbios)
        has_netbios = netbios->begin(host_.c_str());
    if (!has_mdns)
        has_mdns = start_mdns();
    return has_dns || has_netbios || has_mdns;
}

void NetworkService::onStop() {
    if (dns)
        dns->stop();
    if (netbios)
        netbios->end();
    if (mdns)
        mdns->close();
}

bool NetworkService::start_mdns() {
    bool result = mdns->begin(host_);
    if (result) {
        mdns->addService(NULL, "telnet", "tcp", TELNET_PORT);
        mdns->addService(NULL, "http", "tcp", HTTP_PORT);
        mdns->enableArduino(OTA_PORT);
    }
    return result;
}

size_t NetworkService::onDiag(Print *p) {
    size_t n = 0;
    if (has_dns)
        n += println_nameP_value(p, str_dns, dns_name_.c_str());        
         
    if (has_netbios)
        n += println_nameP_value(p, str_netbios, host_.c_str());
    
    n += println_nameP_value(p, str_mdns, getBoolStr(has_mdns).c_str());           
    return n;
}

void NetworkService::onLoop() {
    if (has_dns)
        dns->processNextRequest();
    if (has_mdns)
        mdns->update();
    // netbios has no "loop"
}
