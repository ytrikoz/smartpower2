#include <global.h>

SimpleTimer timer;
SimpleCLI *cli;
Multimeter *meter;
TelnetServer *telnet;
NetworkServices *network_discovery;
Shell *serial_shell;
Shell *telnet_shell;
NTPClient *ntp;

void init_serial_shell(Stream *input)
{
    serial_shell = new Shell(input);
    serial_shell->setParser(cli);
    serial_shell->setOnStart(start_cli);
	serial_shell->setOnQuit(quit_cli);
    serial_shell->enableEcho();
    serial_shell->start();
}

bool init_telnet_shell(Stream *input)
{    
    printWelcome(input);    
    telnet_shell = new Shell(input);
    telnet_shell->setParser(cli);
    telnet_shell->setOnStart(start_cli);
    telnet_shell->setOnQuit(quit_cli);
    telnet_shell->getTerminal()->enableCtrlCodes();    
    telnet_shell->start();
    return true;
}

void setup_telnet()
{
    telnet = new TelnetServer();
#ifndef DISABLE_TELNET_CLI
    telnet->setOnConnection(init_telnet_shell);
    telnet->begin(TELNET_PORT);    
#endif
}

void setup_network_discovery()
{
    network_discovery = new NetworkServices();
    network_discovery->setOutput(&Serial);
    network_discovery->begin();
}

void setup_clock()
{
    rtc.setConfig(config->getData());
    rtc.setOutput(&USE_SERIAL);
    rtc.begin();

    ntp = new NTPClient();
    #ifndef DISABLE_NTP
    ntp->setConfig(config->getData());
    ntp->setOutput(&USE_SERIAL);
    ntp->setOnTimeSynced(
        [](unsigned long epoch)
        {   
            rtc.setEpochTime(epoch);
        }
    );
    ntp->begin();
    #endif
}