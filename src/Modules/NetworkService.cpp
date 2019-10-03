#include "Modules/NetworkService.h"

#include "Consts.h"
#include "Wireless.h"

using namespace PrintUtils;
using namespace StrUtils;

NetworkService::NetworkService() : AppModule(MOD_NETSVC) {
    dns = new DNSServer();
    mdns = new esp8266::MDNSImplementation::MDNSResponder();
    netbios = new ESP8266NetBIOS();
    has_dns = has_mdns = has_netbios = false;
    active = false;
}

bool NetworkService::begin() {
    String host_name = Wireless::hostName();
    if (!has_dns)
        has_dns = begin_dns(host_name);
    if (!has_netbios)
        has_netbios = begin_netbios(host_name);
    if (!has_mdns)
        has_mdns = begin_mdns(host_name);
    return active = has_dns || has_netbios || has_mdns;
}

void NetworkService::stop() {
    if (dns)
        dns->stop();
    if (netbios)
        netbios->end();
    if (mdns)
        mdns->close();
    active = has_dns = has_netbios = has_mdns = false;
}

bool NetworkService::begin_dns(String &host_name) {
    IPAddress ip = Wireless::hostIP();
    String dns_name = host_name + "." + HOST_DOMAIN;
    out->print(StrUtils::getIdentStrP(str_dns));
    out->print(StrUtils::getStr(dns_name));
    out->print(StrUtils::iptos(ip));
    out->print(':');
    out->print(DNS_PORT);
    bool result = dns->start(DNS_PORT, dns_name, ip);
    if (result)
        out->println();
    else
        out->println(StrUtils::getStrP(str_failed));
    return result;
}

bool NetworkService::begin_mdns(String &host_name) {
    out->print(StrUtils::getIdentStrP(str_mdns));
    bool result = mdns->begin(host_name);
    if (result) {
        mdns->addService(NULL, StrUtils::getStrP(str_telnet).c_str(),
                         StrUtils::getStrP(str_tcp).c_str(), TELNET_PORT);
        mdns->addService(NULL, StrUtils::getStrP(str_http).c_str(),
                         StrUtils::getStrP(str_tcp).c_str(), HTTP_PORT);
        mdns->enableArduino(OTA_PORT);
        out->println(host_name);
    } else {
        out->println(StrUtils::getStrP(str_failed));
    }
    return result;
}

bool NetworkService::begin_netbios(String &host_name) {
    out->print(StrUtils::getIdentStrP(str_netbios));
    bool result = netbios->begin(host_name.c_str());
    if (result) {
        out->println(host_name);
    } else {
        out->println(StrUtils::getStrP(str_failed));
    }
    return result;
}

size_t NetworkService::printDiag(Print *p) {
    size_t n = Wireless::printDiag(p);
    n = PrintUtils::print_nameP_value(p, str_active, getBoolStr(active));
    n = PrintUtils::print_nameP_value(p, str_dns, getBoolStr(has_dns));
    n = PrintUtils::print_nameP_value(p, str_mdns, getBoolStr(has_mdns));
    n = PrintUtils::print_nameP_value(p, str_netbios, getBoolStr(has_netbios));
    return n;
}

void NetworkService::loop() {
    if (!active)
        return;
    if (has_dns)
        dns->processNextRequest();
    if (has_mdns)
        mdns->update();
    // netbios has no "loop"
}
