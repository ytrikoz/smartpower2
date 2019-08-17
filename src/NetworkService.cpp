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

void NetworkService::setOutput(Print *p) { output = p; }

void NetworkService::begin() {
    has_dns = Wireless::getWirelessMode() == WLAN_AP ? begin_dns() : false;
    if (!has_netbios) has_netbios = begin_netbios();
    if (!has_mdns) has_mdns = begin_mdns();
    active = true;
}

void NetworkService::stop() {
    if (dns) dns->stop();
    has_dns = false;

    if (netbios) netbios->end();
    has_netbios = false;

    active = false;
}

bool NetworkService::begin_dns() {
    IPAddress ip = Wireless::hostIP();
    char dns_name[] = "*";
    output->print(FPSTR(str_dns));
    output->print(dns_name);
    output->print(' ');
    output->printf_P(strf_s_d, StrUtils::iptos(ip).c_str(), DNS_PORT);    
    if (dns->start(DNS_PORT, dns_name, ip)) {
        output->println();
        return true;
    }
    output->println(FPSTR(str_failed));
    return false;
}

bool NetworkService::begin_mdns() {
    output->print(FPSTR(str_mdns));
    String host_name = Wireless::hostName();
    output->print(host_name);
    if (mdns->begin(host_name)) {
        mdns->addService(NULL, "telnet", "tcp", TELNET_PORT);
        mdns->addService(NULL, "http", "tcp", HTTP_PORT);
        mdns->enableArduino(OTA_PORT);
        output->println();
        return true;
    }
    output->println(FPSTR(str_failed));
    return false;
}

bool NetworkService::begin_netbios() {
    output->print(FPSTR(str_netbios));
    String host_name = Wireless::hostName();
    output->print(host_name.c_str());
    if (netbios->begin(host_name.c_str())) {
        output->println();
        return true;
    }
    output->println(FPSTR(str_failed));
    return false;
}

void NetworkService::loop() {
    if (!active) return;
    if (has_dns) dns->processNextRequest();
    if (has_mdns) mdns->update();
    // netbios has no looping
}
