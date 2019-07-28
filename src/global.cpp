#include <global.h>

Led *wifi_led, *power_led;

SimpleTimer timer;
SimpleCLI *cli;

NetworkService *discovery;
PSU *psu;
TelnetServer *telnet;
ConfigHelper *config;

#ifndef DISABLE_NTP
NTPClient *ntp;
#endif
#ifndef DISABLE_HTTP
WebService *http;
#endif
#ifndef DISABLE_LCD
Display *display;
#endif
#ifndef DISABLE_OTA_UPDATE
OTAUpdate *ota;
#endif
#ifndef DISABLE_CONSOLE_SHELL
Termul *consoleTerm;
Shell *consoleShell;
#endif
#ifndef DISABLE_TELNET_SHELL
Shell *telnetShell;
Termul *telnetTerm;
#endif

void refresh_wifi_status_led() {
    if (!wireless::hasNetwork()) {
        wifi_led->setStyle(STAY_OFF);
        return;
    }
    uint8_t displayStatus = 0;
    if (get_http_clients_count() > 0) displayStatus++;
    if (get_telnet_clients_count() > 0) displayStatus++;
    switch (displayStatus) {
        case 0:
            wifi_led->setStyle(STAY_ON);
            break;
        case 1:
            wifi_led->setStyle(BLINK_ONE_ACCENT);
            break;
        case 2:
            wifi_led->setStyle(BLINK_TWO_ACCENT);
            break;
        default:           
            break;
    }
}


void start_services() {
    // only AP_STA
    if (wireless::getWirelessMode() == WLAN_AP_STA) {
        uint8_t broadcast_if = wifi_get_broadcast_if();
        USE_SERIAL.printf_P(str_wifi);
        char buf[32];
        sprintf_P(buf, strf_set_broadcast, broadcast_if, 3);
        USE_SERIAL.print(buf);
        if (!wifi_set_broadcast_if(3)) {
            USE_SERIAL.printf_P(str_failed);
        }
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
        refresh_wifi_status_led();
        return true;
    });
    telnet->setOnCLientDisconnect([]() {
        USE_SERIAL.printf_P(str_telnet);
        USE_SERIAL.printf_P(str_disconnected);        
        USE_SERIAL.println();
        refresh_wifi_status_led();
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
    rtc.begin();
}

void start_psu() {
    psu = new PSU();
    psu->setConfig(config);
    psu->setOnPowerOff([]() { power_led->setStyle(STAY_ON); });
    psu->setOnPowerOn([]() { power_led->setStyle(BLINK_REGULAR); });
    psu->begin();
}

void start_ntp() {
    ntp = new NTPClient();
    ntp->setConfig(config->getConfig());
    ntp->setOutput(&USE_SERIAL);
    ntp->setOnTimeSynced([](EpochTime &time) { rtc.setTime(time); });
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

void start_discovery() {
    discovery = new NetworkService();
    discovery->setOutput(&USE_SERIAL);
    discovery->begin();
}

void start_ota_update() {
    ota = new OTAUpdate();
    ota->setOutput(&USE_SERIAL);
    ota->begin(HOST_NAME, OTA_PORT);
}

uint8_t get_telnet_clients_count() {
#ifndef DISABLE_TELNET
    return telnet ? telnet->hasClientConnected() : 0;
#else
    return 0;
#endif
}

String getLoopStat() {
    char buf[64];
    sprintf_P(buf, strf_show_status, get_lps(), get_longest_loop());
    return String(buf);
}


