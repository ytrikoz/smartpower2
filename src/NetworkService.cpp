#include "NetworkService.h"

#include "Consts.h"

NetworkService::NetworkService() {
    dns = new DNSServer();
    mdns = new esp8266::MDNSImplementation::MDNSResponder();
    netbios = new ESP8266NetBIOS();
    has_dns = false;
    has_mdns = false;
    has_netbios = false;
    active = false;
}

void NetworkService::setOutput(Print *p) { this->p = p; }

void NetworkService::begin() {
    String host_name = Wireless::hostName();
    if (!has_dns) has_dns = begin_dns(host_name);
    if (!has_netbios) has_netbios = begin_netbios(host_name);
    if (!has_mdns) has_mdns = begin_mdns(host_name);    
    active = has_dns || has_netbios || has_mdns;
}

void NetworkService::stop() {
    if (dns) dns->stop();
    if (netbios) netbios->end();
    has_dns = false;
    has_netbios = false;
    active = false;
}

bool NetworkService::begin_dns(String& host_name) {
    IPAddress ip = Wireless::hostIP();
    String dns_name = host_name + "." + HOST_DOMAIN;        
    p->print(StrUtils::getIdentStrP(str_dns));
    p->print(StrUtils::getStr(dns_name));
    p->print(StrUtils::iptos(ip));
    p->print(':');
    p->print(DNS_PORT);    
    bool result = dns->start(DNS_PORT, dns_name, ip);
    if (result)
        p->println();
    else
        p->println(StrUtils::getStrP(str_failed));
    return result;
}

bool NetworkService::begin_mdns(String& host_name) {
    p->print(StrUtils::getIdentStrP(str_mdns));
    bool result = mdns->begin(host_name);
    if (result) {
        mdns->addService(NULL, StrUtils::getStrP(str_telnet).c_str(), StrUtils::getStrP(str_tcp).c_str(), TELNET_PORT);
        mdns->addService(NULL, StrUtils::getStrP(str_http).c_str(),  StrUtils::getStrP(str_tcp).c_str(), HTTP_PORT);
        mdns->enableArduino(OTA_PORT);
        p->println(host_name);
    } else {
        p->println(StrUtils::getStrP(str_failed));
    }
    return result;
}

bool NetworkService::begin_netbios(String& host_name) {
    p->print(StrUtils::getIdentStrP(str_netbios));
    bool result = netbios->begin(host_name.c_str());
    if (result) {
        p->println(host_name);
    } else {
        p->println(StrUtils::getStrP(str_failed));
    }
    return result;
}

void NetworkService::loop() {
    if (!active) return;
    if (has_dns) dns->processNextRequest();
    if (has_mdns) mdns->update();
    // netbios has no looping
}
