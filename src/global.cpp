#include <global.h>

#include "Termul.h"
#include "str_utils.h"

NetworkService *discovery;
SimpleTimer timer;
Multimeter *meter;
TelnetServer *telnet;
Shell *t_sh;
Termul *t_term;
Shell *s_sh;
Termul *s_term;
NTPClient *ntp;
ConfigHelper *config;
SimpleCLI *cli;
#ifndef DISABLE_HTTP
WebService *http;
#endif
#ifndef DISABLE_LCD
Display *display;
#endif
#ifndef DISABLE_OTA_UPDATE
OTAUpdate *ota;
#endif
#ifndef DISABLE_SERIAL_SHELL

#endif

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
        USE_SERIAL.printf_P(str_telnet);
        USE_SERIAL.printf_P(str_connected);
        start_telnet_shell(s);
        USE_SERIAL.println();
        return true;
    });

    telnet->setOnCLientDisconnect([]() {
        USE_SERIAL.printf_P(str_telnet);
        USE_SERIAL.printf_P(str_disconnected);
        USE_SERIAL.println();
    });
#endif
    telnet->begin();
}

bool start_telnet_shell(Stream *s) {
    if (!t_term) {
        t_term = new Termul();
        t_term->setControlCodes(true);
    }
    t_term->setStream(s);

    if (!t_sh) {
        t_sh = new Shell();
        t_sh->setParser(cli);
        t_sh->setTermul(t_term);
        t_sh->setOnStart(start_cli);
        t_sh->setOnQuit(quit_cli);
        t_sh->enableWelcome();
    }
    t_sh->start();

    return true;
}

void start_serial_shell(Stream *s) {
    if (!s_term) {
        s_term = new Termul();
        s_term->setControlCodes(false);
    }
    s_term->setStream(s);

    if (!s_sh) {
        s_sh = new Shell();
        s_sh->setParser(cli);
        s_sh->setTermul(s_term);
        s_sh->setOnStart(start_cli);
        s_sh->setOnQuit(quit_cli);
        s_sh->enableEcho(true);
    }
    s_sh->start();
}

void start_clock() {
    rtc.setConfig(config->getData());
    rtc.setOutput(&USE_SERIAL);
    rtc.begin();
}

void start_ntp() {
    ntp = new NTPClient();
    ntp->setConfig(config->getData());
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
    discovery->setOutput(&Serial);
    discovery->begin();
}

void start_ota_update() {
    ota = new OTAUpdate();
    ota->setOutput(&USE_SERIAL);
    ota->begin(HOST_NAME, OTA_PORT);
}

void printWelcomeTo(Print *p) {
    char title[SCREEN_WIDTH + 1];
    strcpy(title, APPNAME " v" FW_VERSION);
    uint8_t width = SCREEN_WIDTH / 2;
    str_utils::addPaddingTo(title, str_utils::CENTER, width, ' ');
    char decor[width + 1];
    str_utils::strOfChar(decor, '#', width);
    p->println(decor);
    p->println(title);
    p->println(decor);
}

uint8_t get_telnet_clients_count() {
#ifndef DISABLE_TELNET
    return telnet ? telnet->hasClientConnected() : 0;
#else
    return 0;
#endif
}

String getLoopStat() {
    char buf[32];
    sprintf_P(buf, strf_show_status, get_lps(), get_longest_loop());
    return String(buf);
}