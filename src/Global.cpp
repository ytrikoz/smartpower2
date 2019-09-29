#include "Global.h"

SimpleCLI *cli;

PsuLogger *psuLogger;
WebService *http;
TelnetServer *telnet;

WebClient clients[MAX_WEB_CLIENTS];
SimpleTimer timer;

uint8_t get_telnet_clients_count() {
#ifndef DISABLE_TELNET
    return telnet ? telnet->hasClientConnected() : 0;
#else
    return 0;
#endif
}

void load_screen_psu_pvi() {
    Psu* psu = app.getPsu();

    Display* display  = app.getDisplay();
    String str = String(psu->getV(), 3);
    str += " V ";
    str += String(psu->getI(), 3);
    str += " A ";
    display->addScreenItem(0, str.c_str());

    double watt = psu->getP();
    str = String(watt, (watt < 10) ? 3 : 2);
    str += " W ";
    double rwatth = psu->getWh();
    if (rwatth < 1000) {
        str += String(rwatth, rwatth < 10 ? 3 : rwatth < 100 ? 2 : 1);
        str += " Wh";
    } else {
        str += String(rwatth / 1000, 3);
        str += "KWh";
    }
    display->addScreenItem(1, str.c_str());
    display->setScreen(SCREEN_PVI, 2);
}

void load_screen_sta_wifi() {
    Display* display  = app.getDisplay();
    display->addScreenItem(0, "WIFI> ",
                           Wireless::getConnectionStatus().c_str());
    display->addScreenItem(1, "STA> ", Wireless::hostSTA_SSID().c_str());

    display->addScreenItem(2, "IP> ", Wireless::hostIP().toString().c_str());
    display->addScreenItem(3, "RSSI> ", Wireless::RSSIInfo().c_str());
    display->setScreen(SCREEN_WIFI_STA, 4);
};

void load_screen_ap_wifi() {
    Display* display  = app.getDisplay();
    display->addScreenItem(0, "AP> ", Wireless::hostAP_SSID().c_str());
    display->addScreenItem(1, "PWD> ", Wireless::hostAP_Password().c_str());
    display->setScreen(SCREEN_WIFI_AP, 2);  
};

void load_screen_ap_sta_wifi() {
    Display* display  = app.getDisplay();
    display->addScreenItem(0, "AP> ", Wireless::hostAP_SSID().c_str());
    display->addScreenItem(1, "STA> ", Wireless::hostSTA_SSID().c_str());
    display->addScreenItem(2, "WIFI> ",
                           Wireless::getConnectionStatus().c_str());
    display->addScreenItem(3, "IP AP> ",
                           Wireless::hostAP_IP().toString().c_str());
    display->addScreenItem(4, "IP STA> ",
                           Wireless::hostSTA_IP().toString().c_str());
    display->addScreenItem(5, "RSSI> ", Wireless::RSSIInfo().c_str());
    display->setScreen(SCREEN_AP_STA, 6);
};

void load_screen_ready() {
    Display* display  = app.getDisplay();
    display->addScreenItem(0, "READY> ");
    display->setScreen(SCREEN_WIFI_OFF, 1);
}

void update_display() {
    Psu* psu = app.getPsu();

    Wireless::Mode mode = Wireless::getMode();
    if (psu->getState() == POWER_OFF) {
        if (mode == Wireless::WLAN_STA) {
            load_screen_sta_wifi();
        } else if (mode == Wireless::WLAN_AP) {
            load_screen_ap_wifi();
        } else if (mode == Wireless::WLAN_AP_STA) {
            load_screen_ap_sta_wifi();
        } else if (mode == Wireless::WLAN_OFF) {
            load_screen_ready();
        }
    } else if (psu->getState() == POWER_ON) {
        load_screen_psu_pvi();
    }
}