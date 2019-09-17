#include "Global.h"

Led::Led *wifi_led, *power_led;

AppModule *appModule[13] = {0};
WebClient clients[MAX_WEB_CLIENTS];
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

void start_module(AppModule* module) {
    module->setOutput(&USE_SERIAL);
    module->setConfig(config->get());
    module->begin();
}

void refresh_wifi_led() {
    uint8_t level = 0;
    if (Wireless::hasNetwork()) {
        level++;
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
        default:
            break;
    }
}

void setBroadcastTo(uint8_t broadcast_if) {
    String old_if = String(wifi_get_broadcast_if(), DEC);
    USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
    USE_SERIAL.print(StrUtils::getStrP(str_set));
    USE_SERIAL.print(StrUtils::getStrP(str_broadcast));
    USE_SERIAL.print(StrUtils::getStr(old_if));
    USE_SERIAL.print(StrUtils::getStrP(str_arrow_dest));
    USE_SERIAL.print(broadcast_if);
    if (!wifi_set_broadcast_if(broadcast_if))
        USE_SERIAL.print(StrUtils::getStrP(str_failed));
    USE_SERIAL.println();
}

void start_services() {
    WirelessMode mode = Wireless::getWirelessMode();
    // only AP_STA
    if (mode == WLAN_AP_STA) setBroadcastTo(3);
#ifndef DISABLE_NTP
    if (mode == WLAN_STA || mode == WLAN_AP_STA) start_ntp();
#endif
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
        telnet->setOnClientConnect([](Stream *stream) {
            USE_SERIAL.print(StrUtils::getIdentStrP(str_telnet));
            USE_SERIAL.println(StrUtils::getStrP(str_connected));
#ifndef DISABLE_TELNET_CLI
            start_telnet_shell(stream);
#endif
            refresh_wifi_led();
            return true;
        });
        telnet->setOnCLientDisconnect([]() {
            USE_SERIAL.print(StrUtils::getIdentStrP(str_telnet));
            USE_SERIAL.println(StrUtils::getStrP(str_disconnected));
            refresh_wifi_led();
        });
    }
    telnet->begin();
}

void start_console_shell() {
    consoleTerm = new Termul(&USE_SERIAL);
    consoleTerm->enableEcho();

    consoleShell = new Shell(cli, consoleTerm);
}

void start_telnet_shell(Stream *s) {
    telnetTerm = new Termul(s);
    telnetTerm->enableControlCodes();

    telnetShell = new Shell(cli, telnetTerm);
    telnetShell->enableWelcome();
}

void start_clock() {
    rtc.setOptions(config->get());
    rtc.setOnTimeChange(onTimeChangeEvent);
    rtc.begin();
}

void init_psu() {
    psu = new Psu();
    psu->setConfig(config);
    psuLog = new PsuLogger(psu);

    psu->setOnTogglePower([]() { sendPageState(PG_HOME); });

    psu->setOnPowerOn([]() {
        power_led->set(Led::BLINK);
        if (display) display->unlock();
        psuLog->start();
    });

    psu->setOnPowerOff([]() {
        power_led->set(Led::ON);
        psuLog->stop();
        // if (size > 0) {
        //     float val[size];
        //     psuLog->fill(PP_VOLTAGE, val, size);
        //     if (display) {
        //         size_t cols = fill_data(display->getData(), val, size);
        //         display->drawPlot(8 - cols);
        //         display->lock(15000);
        //     }
        //     delete[] val;
        // }
    });

    psu->setOnError([]() { power_led->set(Led::BLINK_ERROR); });

    psu->begin();
}

void start_ntp() {
    if (!ntp) {
        ntp = new NtpClient();
        ntp->setOnResponse([](EpochTime &epoch) { rtc.setEpoch(epoch, true); });       
    }
    appModule[MOD_NTP] = ntp;
    start_module(appModule[MOD_NTP]);
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

void onTimeChangeEvent(const char *str) {
    USE_SERIAL.print(StrUtils::getIdentStrP(str_clock));
    USE_SERIAL.print(StrUtils::getStrP(str_time));
    USE_SERIAL.print(StrUtils::getStrP(str_change));
    USE_SERIAL.println(str);
}

void load_screen_psu_pvi() {
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
    display->addScreenItem(0, "WIFI> ",
                           Wireless::getConnectionStatus().c_str());
    display->addScreenItem(1, "STA> ", Wireless::hostSTA_SSID().c_str());

    display->addScreenItem(2, "IP> ", Wireless::hostIP().toString().c_str());
    display->addScreenItem(3, "RSSI> ", Wireless::RSSIInfo().c_str());
    display->setScreen(SCREEN_WIFI_STA, 4);
};

void load_screen_ap_wifi() {
    display->addScreenItem(0, "AP> ", Wireless::hostAP_SSID().c_str());
    display->addScreenItem(1, "PWD> ", Wireless::hostAP_Password().c_str());
    display->setScreen(SCREEN_WIFI_AP, 2);
};

void load_screen_ap_sta_wifi() {
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
    display->addScreenItem(0, "READY> ");
    display->setScreen(SCREEN_WIFI_OFF, 1);
}

void update_display() {
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
