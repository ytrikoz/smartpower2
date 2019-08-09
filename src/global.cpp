#include <global.h>

Led *wifi_led, *power_led;

SimpleTimer timer;
SimpleCLI *cli;

NetworkService *discovery;
ConfigHelper *config;
Display *display;
Psu *psu;
PsuLogger *psuLog;
WebService *http;
TelnetServer *telnet;
NTPClient *ntp;
OTAUpdate *ota;
Termul *consoleTerm;
Termul *telnetTerm;
Shell *telnetShell;
Shell *consoleShell;

void refresh_wifi_led() {
    uint8_t level;
    if (!wireless::hasNetwork()) {
        level = 0;
    } else {
        level = 1;
        if (get_http_clients_count() || get_telnet_clients_count()) level++;
    }
    switch (level) {
        case 0:
            wifi_led->set(STAY_OFF);
            return;
        case 1:
            wifi_led->set(STAY_ON);
            break;
        case 2:
            wifi_led->set(BLINK_ONE);
            break;
        case 3:
            wifi_led->set(BLINK_TWO);
            break;
        default:
            break;
    }
}

void start_services() {
    // only AP_STA
    if (wireless::getWirelessMode() == WLAN_AP_STA) {
        uint8_t broadcast_if = wifi_get_broadcast_if();
        USE_SERIAL.print(FPSTR(str_wifi));
        char buf[32];
        sprintf_P(buf, strf_set_broadcast, broadcast_if, 3);
        USE_SERIAL.print(buf);
        if (!wifi_set_broadcast_if(3)) USE_SERIAL.print(FPSTR(str_failed));
        USE_SERIAL.println();
    }
    // only STA
    if (wireless::getWirelessMode() == WLAN_STA) {
#ifndef DISABLE_NTP
        start_ntp();
#endif
    }
    // any
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

    psu->setOnPowerOff([]() {
        power_led->set(STAY_ON);
        psuLog->end();

        USE_SERIAL.print(FPSTR(str_psu));
        USE_SERIAL.print(FPSTR(str_duration));
        USE_SERIAL.println(psu->getDuration());

        size_t logSize = psuLog->size();
        USE_SERIAL.print(FPSTR(str_psu));
        USE_SERIAL.print(FPSTR(str_log));
        USE_SERIAL.print(FPSTR(str_size));
        USE_SERIAL.println(logSize);

        float *items = new float[logSize];
        psuLog->getVoltages(items);
        display->loadData(items, logSize);
        display->drawPlot(0);
    });

    psu->setOnPowerOn([]() {
        power_led->set(BLINK);
        psuLog->begin();
    });

    psu->begin();
}

void start_ntp() {
    ntp = new NTPClient();
    ntp->setConfig(config->getConfig());
    ntp->setOutput(&USE_SERIAL);
    ntp->setOnTimeSynced([](EpochTime &time) { rtc.setTime(time.get()); });
    ntp->begin();
}

void start_http() {
    http = new WebService(HTTP_PORT, WEBSOCKET_PORT, WEB_ROOT);
    http->setOutput(&USE_SERIAL);
    http->setOnClientConnection(onHttpClientConnect);
    http->setOnClientDisconnected(onHttpClientDisconnect);
    http->setOnClientData(onHttpClientData);
    http->begin();
}

void start_ota_update() {
    ota = new OTAUpdate();
    ota->setOutput(&USE_SERIAL);
    ota->begin(HOST_NAME, OTA_PORT);
}

void start_discovery() {
    discovery = new NetworkService();
    discovery->setOutput(&USE_SERIAL);
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
    USE_SERIAL.print(FPSTR(str_clock));
    USE_SERIAL.print(FPSTR(str_set));
    USE_SERIAL.println(str);
}

void load_screen_psu_pvi() {
    String str = String(psu->getVoltage(), 3);
    str += " V ";
    str += String(psu->getCurrent(), 3);
    str += " A ";
    display->setLine(0, str.c_str());

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
    display->setLine(1, str.c_str());
}

void load_screen_sta_wifi() {
    display->setLine(0, "STA> ", wireless::hostSSID().c_str());
    display->setLine(1, "IP> ", wireless::hostIP().toString().c_str());
    display->setLine(2, "RSSI> ", wireless::RSSIInfo().c_str());
};

void load_screen_ap_wifi() {
    display->setLine(0, "AP> ", wireless::hostSSID().c_str());
    display->setLine(1, "PWD> ", wireless::hostAPPassword().c_str());
};

void update_display() {
    if ((!display) || !display->ready()) return;
    if (psu->getState() == POWER_OFF) {
        if (wireless::getWirelessMode() == WLAN_STA) {
            load_screen_sta_wifi();
        } else if (wireless::getWirelessMode() == WLAN_AP) {
            load_screen_ap_wifi();
        }
    } else if (psu->getState() == POWER_ON) {
        load_screen_psu_pvi();
    }
}