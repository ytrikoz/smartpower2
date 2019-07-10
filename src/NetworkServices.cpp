#include "NetworkService.h"

void NetworkServices::setOutput(Stream *stream) { output = stream; }

void NetworkServices::begin() {
    const char hostname[] = HOSTNAME;
    const uint16_t dns_port = DNS_PORT;
    const uint16_t telnet_port = TELNET_PORT;
    const uint16_t http_port = HTTP_PORT;
    const uint16_t ota_port = OTA_PORT;
    IPAddress ip = hostIP();

    if (WiFi.getMode() == WIFI_AP) {
        output->print(F("[dns] "));
        if (begin_dns("*", ip, dns_port)) {
            output->print('*');
            output->print(FPSTR(str_arrow_dest));
            output->print(ip);
            output->print(':');
            output->println(dns_port);
        } else {
            output->println(FPSTR(str_failed));
        }
    }

    output->print(F("[mdns] "));
    if (begin_mdns(hostname, telnet_port, http_port, ota_port)) {
        output->println(FPSTR(str_ready));
    } else {
        output->println(FPSTR(str_failed));
    }

    output->print(F("[netbios] "));
    if (begin_netbios(hostname)) {
        output->println(FPSTR(str_ready));
    } else {
        output->println(FPSTR(str_failed));
    }

    ready = true;
}

bool NetworkServices::begin_dns(const char *domain, IPAddress ip,
                                uint16_t port) {
    dns = new DNSServer();
    return dns->start(port, domain, ip);
}

bool NetworkServices::begin_mdns(const char *hostname, uint16_t telnet_port,
                                 uint16_t http_port, uint16_t ota_port) {
    mdns = new esp8266::MDNSImplementation::MDNSResponder();
    if (mdns->begin(hostname)) {
        mdns->addService(hostname, "telnet", "tcp", telnet_port);
        mdns->addService(hostname, "http", "tcp", http_port);
        mdns->enableArduino(ota_port);
        return true;
    }
    return false;
}

bool NetworkServices::begin_netbios(const char *hostname) {
    netbios = new ESP8266NetBIOS();
    return netbios->begin(hostname);
}

void NetworkServices::loop() {
    if (!ready) return;

    if (dns) dns->processNextRequest();

    if (mdns) mdns->update();

    // netbios - no in loop executing
}