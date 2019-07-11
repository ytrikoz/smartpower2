#include <global.h>

SimpleTimer timer;
SimpleCLI *cli;
Multimeter *meter;
TelnetServer *telnet;
NetworkServices *network_discovery;
Shell *serial_shell;
Shell *telnet_shell;
NTPClient *ntp;

#ifndef DISABLE_HTTP
WebService *http;
#endif

#ifndef DISABLE_LCD
Display *display;
#endif

#ifndef DISABLE_OTA_UPDATE
OTAUpdate *ota;
#endif

void init_serial_shell(Stream *input) {
    #ifndef DISABLE_SERIAL_SHELL
    serial_shell = new Shell(input);
    serial_shell->setParser(cli);
    serial_shell->setOnStart(start_cli);
    serial_shell->setOnQuit(quit_cli);
    serial_shell->enableEcho();
    serial_shell->start();
    #endif
}

bool init_telnet_shell(Stream *input) {
    #ifndef DISABLE_TELNET_SHELL
    str_utils::printWelcomeTo(input);
    telnet_shell = new Shell(input);
    telnet_shell->setParser(cli);
    telnet_shell->setOnStart(start_cli);
    telnet_shell->setOnQuit(quit_cli);
    telnet_shell->getTerminal()->enableCtrlCodes();
    telnet_shell->start();
    #endif
    return true;
}

void setup_telnet() {
#ifndef DISABLE_TELNET
    telnet = new TelnetServer();
    telnet->setOutput(&USE_SERIAL);
#ifndef DISABLE_TELNET_CLI    
    telnet->setOnConnection(init_telnet_shell);    
#endif
    telnet->begin(TELNET_PORT);
#endif
}


void setup_network_discovery() {    
#ifndef DISABLE_NETWORK_DISCOVERY
    network_discovery = new NetworkServices();
    network_discovery->setOutput(&Serial);
    network_discovery->begin();
#endif
}

void setup_clock() {
    rtc.setConfig(config->getData());
    rtc.setOutput(&USE_SERIAL);
    rtc.begin();

    ntp = new NTPClient();
#ifndef DISABLE_NTP
    ntp->setConfig(config->getData());
    ntp->setOutput(&USE_SERIAL);
    ntp->setOnTimeSynced([](unsigned long epoch) { rtc.setEpochTime(epoch); });
    ntp->begin();
#endif
}