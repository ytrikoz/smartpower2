#include "Global.h"

Led::Led *wifi_led, *power_led;

SimpleTimer timer;
SimpleCLI *cli;

NetworkService *discovery;
ConfigHelper *config;
Display *display;
Psu *psu;
PsuLogger *psuLog;
WebService *http;
TelnetServer *telnet;
NtpClient *ntp;
OTAUpdate *ota;
Termul *consoleTerm;
Termul *telnetTerm;
Shell *telnetShell;
Shell *consoleShell;

void refresh_wifi_led() {
    uint8_t level;
    if (!Wireless::hasNetwork()) {
        level = 0;
    } else {
        level = 1;
        if (get_http_clients_count() || get_telnet_clients_count()) level++;
    }
    switch (level) {
        case 0:
            wifi_led->set(Led::OFF);
            return;
        case 1:
            wifi_led->set(Led::ON);
            break;
        case 2:
            wifi_led->set(Led::BLINK_ONE);
            break;
        case 3:
            wifi_led->set(Led::BLINK_TWO);
            break;
        default:
            break;
    }
}

void start_services() {
    WirelessMode mode = Wireless::getWirelessMode();
    // only AP_STA
    if (mode == WLAN_AP_STA) {
        uint8_t broadcast_if = wifi_get_broadcast_if();
        USE_SERIAL.print(FPSTR(str_wifi));
        char buf[32];
        sprintf_P(buf, strf_set_broadcast, broadcast_if, 3);
        USE_SERIAL.print(buf);
        if (!wifi_set_broadcast_if(3)) USE_SERIAL.print(FPSTR(str_failed));
        USE_SERIAL.println();
    }
    if (mode == WLAN_STA || mode == WLAN_AP_STA)
    {
    #ifndef DISABLE_NTP
        start_ntp();
    #endif
    }
#ifndef DISABLE_TELNET
    start_telnet();
#endif
#ifndef DISABLE_OTA_UPDATE
    start_ota_update();
#endif
#ifndef DISABLE_HTTP
    start_http();
#endif
#ifndef DISABLE_NETWORK_DISCOVERY
    start_discovery();
#endif
}

void start_telnet() {
    if (!telnet) {
    telnet = new TelnetServer(TELNET_PORT);
    telnet->setOutput(&USE_SERIAL);
#ifndef DISABLE_TELNET_CLI
    telnet->setOnClientConnect([](Stream *s) {
        USE_SERIAL.print(FPSTR(str_telnet));
        USE_SERIAL.println(FPSTR(str_connected));

        start_telnet_shell(s);
        refresh_wifi_led();
        return true;
    });

    telnet->setOnCLientDisconnect([]() {
        USE_SERIAL.print(FPSTR(str_telnet));
        USE_SERIAL.println(FPSTR(str_disconnected));

        refresh_wifi_led();
    });
#endif
    }
    telnet->begin();
}

void start_console_shell() {
    consoleTerm = new Termul(&USE_SERIAL);
    consoleTerm->enableEcho(true);

    consoleShell = new Shell();
    consoleShell->setParser(cli);
    consoleShell->setTermul(consoleTerm);
}

bool start_telnet_shell(Stream *s) {
    telnetTerm = new Termul(s);
    telnetTerm->enableControlCodes(true);

    telnetShell = new Shell();
    telnetShell->setParser(cli);
    telnetShell->enableWelcome();
    telnetShell->setTermul(telnetTerm);

    return true;
}

void start_clock() {
    rtc.setConfig(config->getConfig());
    rtc.setOutput(&USE_SERIAL);
    rtc.setOnSystemTimeChanged(onSystemTimeChanged);
    rtc.begin();
}

void start_psu() {
    psu = new Psu();
    psu->setConfig(config);
    psuLog = new PsuLogger(psu, PSU_LOG_SIZE);

    psu->setOnOn([]() {
        power_led->set(Led::BLINK);
        if (display) display->unlock();
        psuLog->begin();
    });

    psu->setOnOff([]() {
        power_led->set(Led::ON);
        psuLog->end();
        if (int size = psuLog->size()) {
            float *vals = new float[size];
            psuLog->getVoltages(vals);
            if (display) {
                size_t cols = fill_data(display->getData(), vals, size);
                display->drawPlot(8 - cols);
                display->lock(15000);
            }
            delete[] vals;
        }
    });

    psu->setOnError([]() { power_led->set(Led::BLINK_ERROR); });

    psu->begin();
}

void start_ntp() {
    if (!ntp) {
        ntp = new NtpClient();
        ntp->setConfig(config->getConfig());
        ntp->setOutput(&USE_SERIAL);
        ntp->setOnTimeSynced([](EpochTime &time) { rtc.setTime(time.get()); });
    }
}

void start_http() {
    if (!http) {
        http = new WebService();
        http->setOutput(&USE_SERIAL);
        http->setOnClientConnection(onHttpClientConnect);
        http->setOnClientDisconnected(onHttpClientDisconnect);
        http->setOnClientData(onHttpClientData);
    }
    http->begin();
}

void start_ota_update() {
    String host_name = Wireless::hostName();
    if (!ota) {
        ota = new OTAUpdate();
        ota->setOutput(&USE_SERIAL);
    }    
    ota->begin(host_name.c_str(), OTA_PORT);
}

void start_discovery() {
    if (!discovery) {
        discovery = new NetworkService();
        discovery->setOutput(&USE_SERIAL);
    }    
    discovery->begin();
}

uint8_t get_telnet_clients_count() {
#ifndef DISABLE_TELNET
    return telnet ? telnet->hasClientConnected() : 0;
#else
    return 0;
#endif
}

void onSystemTimeChanged(const char *str) {
    USE_SERIAL.print(getSquareBracketsStrP(str_clock));
    USE_SERIAL.print(getStrP(str_system));
    USE_SERIAL.print(getStrP(str_time));
    USE_SERIAL.print(getStrP(str_set));
    USE_SERIAL.println(str);
}

void load_screen_psu_pvi() {
    String str = String(psu->getVoltage(), 3);
    str += " V ";
    str += String(psu->getCurrent(), 3);
    str += " A ";
    display->addTextItem(0, str.c_str());

    double watt = psu->getPower();
    str = String(watt, (watt < 10) ? 3 : 2);
    str += " W ";
    double rwatth = psu->getWattHours();
    if (rwatth < 1000) {
        str += String(rwatth, rwatth < 10 ? 3 : rwatth < 100 ? 2 : 1);
        str += " Wh";
    } else {
        str += String(rwatth / 1000, 3);
        str += "KWh";
    }
    display->addTextItem(1, str.c_str());

    display->setScreen(SCREEN_PVI, 2);    
}

void load_screen_sta_wifi() {
    display->addTextItem(0, "WIFI> ",  Wireless::getConnectionStatus().c_str());
    display->addTextItem(1, "STA> ", Wireless::hostSTA_SSID().c_str());
    display->addTextItem(2, "IP> ", Wireless::hostIP().toString().c_str());
    display->addTextItem(3, "RSSI> ", Wireless::RSSIInfo().c_str());
    display->setScreen(SCREEN_WIFI_STA, 4);
};

void load_screen_ap_wifi() {    
    display->addTextItem(0, "AP> ", Wireless::hostAP_SSID().c_str());
    display->addTextItem(1, "PWD> ", Wireless::hostAP_Password().c_str());
    display->setScreen(SCREEN_WIFI_AP, 2);
};

void load_screen_ap_sta_wifi() {
    display->addTextItem(0, "AP> ", Wireless::hostAP_SSID().c_str());
    display->addTextItem(1, "STA> ", Wireless::hostSTA_SSID().c_str());
    display->addTextItem(2, "WIFI> ",  Wireless::getConnectionStatus().c_str());
    display->addTextItem(3, "IP AP> ", Wireless::hostAP_IP().toString().c_str());
    display->addTextItem(4, "IP STA> ", Wireless::hostSTA_IP().toString().c_str());
    display->setScreen(SCREEN_AP_STA, 5);
};

void load_screen_wifi_status() {

}

void load_screen_ready() {
    display->addTextItem(0, "READY> ");
    display->setScreen(SCREEN_WIFI_OFF, 1);
}

void update_display_every_1_sec() {
    if (display->locked()) return;
    WirelessMode mode = Wireless::getWirelessMode();
    if (psu->getState() == POWER_OFF) {
        if (mode == WLAN_STA) {
            load_screen_sta_wifi();
        } else if (mode == WLAN_AP) {
            load_screen_ap_wifi();
        } else if (mode == WLAN_AP_STA) {
            load_screen_ap_sta_wifi();
        } else if (mode == WLAN_OFF) {
            load_screen_ready();
        }
    } else if (psu->getState() == POWER_ON) {
        load_screen_psu_pvi();
    }
}

void update_display_every_5_sec() { display->scrollDown(); }
