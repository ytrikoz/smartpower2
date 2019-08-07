#include "main.h"

#include "cli.h"
#include "global.h"
#include "str_utils.h"

#include "Consts.h"
#include "Led.h"
#include "SysInfo.h"
#include "wireless.h"

#include "TimeProfiler.h"

#define I2C_SDA D2
#define I2C_SCL D5
#define POWER_LED_PIN D1
#define WIFI_LED_PIN D4
#define POWER_BTN_PIN D7

LoopWatchDog loopWD;

uint8_t volatile powerBtnLongPressCounter;
unsigned long volatile powerBtnLastEvent = 0;
bool volatile powerBtnPressed = false;
bool volatile powerBtnReleased = true;
bool volatile powerBtnPressEvent = false;

typedef struct {
    bool connected = false;
    uint8_t page = 0;
} WebClient;
WebClient clients[MAX_WEB_CLIENTS];

bool display_alt_line = false;

uint8_t get_http_clients_count() {
    uint8_t result = 0;
#ifndef DISABLE_HTTP
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected) result++;
#endif
    return result;
}

float data[8] = {0, 2.101, 3.130, 4.450, 3.501, 4.030, 4.001};

int restartTimer = -1;
int restartCount = 0;

void cancel_system_restart() {
    if (restartTimer != -1) timer.disable(restartTimer);
}
void on_restart_sequence() {
    if (restartCount == 0) system_restart();
    restartCount--;
}

void setup_restart_timer(uint8_t count) {
    USE_SERIAL.print(FPSTR(str_system_restart));
    if (restartTimer != -1) timer.deleteTimer(restartTimer);
    restartCount = count;
    if (restartCount > 0) {
        USE_SERIAL.printf(strf_in_second, restartCount);
        USE_SERIAL.println();
    } else {
        USE_SERIAL.println(F("now!"));
    }
    restartTimer = timer.setInterval(ONE_SECOND_ms, on_restart_sequence);
}

void onHttpClientConnect(uint8_t n) {
    clients[n].connected = true;
    refresh_wifi_led();
}
void onHttpClientDisconnect(uint8_t n) {
    clients[n].connected = false;
    refresh_wifi_led();
}

void onHttpClientData(uint8_t n, String data) {
    switch (data.charAt(0)) {
        case GET_PAGE_STATE: {
            uint8_t page = data.charAt(1) - CHR_ZERO;
            clients[n].page = page;
            sendPageState(n, page);
            break;
        }
        case SET_POWER_ON_OFF: {
            PowerState new_state = PowerState(data.substring(1).toInt());
            psu->setState(new_state);
            String str = String(SET_POWER_ON_OFF);
            str += psu->getState();
            sendToClients(str, PG_HOME, n);
            break;
        }
        case SET_VOLTAGE: {
            float _voltage = data.substring(1).toFloat();
            psu->setOutputVoltage(_voltage);
            sendToClients(String(SET_VOLTAGE) + data.substring(1), PG_HOME, n);
            break;
        }
        case SET_DEFAULT_VOLTAGE: {
            float _voltage = psu->getOutputVoltage();
            config->setOutputVoltage(_voltage);
            break;
        }
        case SET_BOOT_POWER_MODE: {
            if (config->setBootPowerState(
                    BootPowerState(data.substring(1).toInt()))) {
                config->save();
                sendToClients(SET_BOOT_POWER_MODE + data.substring(1).c_str(),
                              PG_SETTINGS, n);
            }
            break;
        }
        case SET_NETWORK: {
            static const uint8_t paramCount = 8;
            static const Parameter items[paramCount] = {
                WIFI, SSID, PASSWD, DHCP, IPADDR, NETMASK, GATEWAY, DNS};
            uint8_t last = 0, pos = 0, index = 0;
            while (index < paramCount && (pos = data.indexOf(",", last))) {
                config->getConfig()->setValue(
                    items[index++], data.substring(last, pos).c_str());
                last = pos + 1;
            }
            config->save();
            break;
        }
        case SET_MEASURE_MODE: {
            char ch = data.charAt(1);
            if (isdigit(ch)) {
                bool mode = (uint8_t)ch - CHR_ZERO;
                psu->enableWattHoursCalculation(mode);
                sendToClients(data, PG_HOME, n);
                break;
            }
        }
    }
}

void sendToClients(String str, uint8_t page, uint8_t except_n) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected && (clients[i].page == page) &&
            (except_n != i))
            http->sendTxt(i, str.c_str());
}

void sendToClients(String str, uint8_t page) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected && clients[i].page == page)
            http->sendTxt(i, str.c_str());
}

void sendPageState(uint8_t n, uint8_t page) {
    switch (page) {
        case PG_HOME: {
            String str;
            // State
            str = String(SET_POWER_ON_OFF);
            str += String(psu->getState());
            http->sendTxt(n, str.c_str());
            // WattHours
            str = String(SET_MEASURE_MODE);
            str += String(psu->isWattHoursCalculationEnabled());
            http->sendTxt(n, str.c_str());
            break;
        }
        case PG_SETTINGS: {
            String str;
            // Power mode
            str = String(SET_BOOT_POWER_MODE);
            str += config->getConfig()->getStrValue(POWER);
            http->sendTxt(n, str.c_str());

            // Output voltage
            str = String(SET_VOLTAGE);
            str += psu->getOutputVoltage();
            http->sendTxt(n, str.c_str());

            // Network config
            str = String(SET_NETWORK);
            str += config->getConfig()->getStrValue(WIFI);
            str += ',';
            str += config->getSSID();
            str += ',';
            str += config->getPassword();
            str += ',';
            str += config->getDHCP();
            str += ',';
            str += config->getIPAddrStr();
            str += ',';
            str += config->getNetmaskStr();
            str += ',';
            str += config->getGatewayStr();
            str += ',';
            str += config->getDNSStr();
            http->sendTxt(n, str.c_str());
            break;
        }
        case PG_STATUS: {
            String str;
            // Version info
            str = String(TAG_FIRMWARE_INFO);
            str += getVersionInfoJson();
            http->sendTxt(n, str.c_str());
            // System info
            str = String(TAG_SYSTEM_INFO);
            str += getSystemInfoJson();
            http->sendTxt(n, str.c_str());
            // Network info
            str = String(TAG_NETWORK_INFO);
            str += getNetworkInfoJson();
            http->sendTxt(n, str.c_str());
            break;
        }
    }
}

void load_screen_network(){ 
    display->setLine(0, "STA> ", wireless::hostSSID().c_str());
    display->setLine(1, "IP> ", wireless::hostIP().toString().c_str());            
    display->setLine(1, "RSSI> ", wireless::RSSIInfo().c_str());
};

void update_display() {
    if (!display->init()) return;
    if (psu->getState() == POWER_OFF) {
        if (wireless::getWirelessMode() == WLAN_STA) {
        load_screen_network();
   
        } else if (wireless::getWirelessMode() == WLAN_AP) {
            display->setLine(0, "AP> ", wireless::hostSSID().c_str());
            display->setLine(1, "PWD> ", wireless::hostAPPassword().c_str());
        }
    } else if (psu->getState() == POWER_ON) {
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
}

void send_psu_data_to_clients() {
    if (psu->getState() == POWER_ON) {
        if (wireless::hasNetwork()) {
#ifndef DISABLE_TELNET
            if (get_telnet_clients_count() && !telnetShell->isActive()) {
                String data = psu->toString();
                telnet->write(data.c_str());
                telnet->write("\r");
            }
#endif
#ifndef DISABLE_HTTP
            if (get_http_clients_count()) {
                String data = psu->toString();
                String str = String(TAG_PVI);
                str += data;
                sendToClients(str, PG_HOME);
            }
#endif
        }
    }
}

void display_boot_progress(float f, const char *str) {
#ifndef DISABLE_LCD
    if (display) display->drawBar(LCD_ROW_2, f);
#else
    USE_SERIAL.printf_P(strf_boot_progress, message, per);
    USE_SERIAL.println();
#endif
}

void print_reset_reason(Print *p) {
    p->print(F("reset reason: "));
    p->println(ESP.getResetReason());
    p->print(F("reset info: "));
    p->println(ESP.getResetInfo());
}

void print_boot_delay(Print *p) {
    p->println();
    p->print(FPSTR(str_wait));
    p->print(" ");
    for (uint8_t t = BOOT_WAIT_s; t > 0; t--) {
        p->print(t);
        p->print(" ");
        delay(1000);
    }
    p->println();
    p->flush();
}
void print_welcome(Print *p) {
    char title[SCREEN_WIDTH + 1];
    strcpy(title, APPNAME " v" FW_VERSION);
    uint8_t width = SCREEN_WIDTH / 2;
    addPaddingTo(title, str_utils::CENTER, width, ' ');
    char decor[width + 1];
    str_utils::str_of_char(decor, '*', width);
    p->println(decor);
    p->println(title);
    p->println(decor);
}

void setup() {
    // gdbstub_init();
    USE_SERIAL.begin(115200);
#ifdef SERIAL_DEBUG
    USE_SERIAL.setDebugOutput(true);
#endif
    print_boot_delay(&USE_SERIAL);
    print_reset_reason(&USE_SERIAL);

    SPIFFS.begin();

    Wire.begin(I2C_SDA, I2C_SCL);

    // Button
    pinMode(POWER_BTN_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(POWER_BTN_PIN), powerButtonHandler,
                    CHANGE);

    // Leds
    power_led = new Led(POWER_LED_PIN, LIGHT_ON, true);
    power_led->setMode(STAY_ON);

    wifi_led = new Led(WIFI_LED_PIN);
    wifi_led->setMode(STAY_OFF);

    memset(&clients[0], 0x00, sizeof(WebClient) * WEBSOCKETS_SERVER_CLIENT_MAX);

#ifndef DISABLE_LCD
    display = new Display();
    display->setOutput(&USE_SERIAL);
    if (display->init()) display->turnOn();
#endif

    display_boot_progress(.2, "VER> " FW_VERSION);
    delay(100);

    config = new ConfigHelper();
    config->init(FILE_CONFIG);

    start_clock();
    start_psu();

    display_boot_progress(.3);
    delay(100);

    print_welcome(&USE_SERIAL);
    delay(100);

    display_boot_progress(.4, "WIFI>");
    delay(100);

    wireless::setOnNetworkStateChange(
        [](bool hasNetwork) { refresh_wifi_led(); });
    wireless::start_wifi();

    display_boot_progress(.8, "INIT>");
    delay(100);

    CLI::init();
#ifndef DISABLE_CONSOLE_SHELL
    start_console_shell();
#endif

    display_boot_progress(1, "COMPLETE");
    delay(100);

    timer.setInterval(1000, [] {
        send_psu_data_to_clients(), handle_power_button_press();
#ifndef DISABLE_LCD
        update_display();
#endif
    });

    timer.setInterval(5000, [] { display_alt_line = !display_alt_line; });
}

static void ICACHE_RAM_ATTR powerButtonHandler() {
    bool lock = (millis() - powerBtnLastEvent) < 30;
    if (!lock) {
        if (!digitalRead(POWER_BTN_PIN) && !powerBtnPressed) {
            lock = true;
            powerBtnLastEvent = millis();
            powerBtnPressed = true;
            powerBtnReleased = false;
        }
    }
    if (!lock) {
        if (digitalRead(POWER_BTN_PIN) && (!powerBtnReleased)) {
            lock = true;
            powerBtnLastEvent = millis();
            powerBtnPressed = false;
            powerBtnReleased = true;
            powerBtnPressEvent = true;
        }
    }
}

void loop() {
    // Clock
    {
#ifdef DEBUG_LOOP
        TimeProfiler _tp_rtc = TimeProfiler("rtc");
#endif
        rtc.loop();
    }
    delay(0);
    // LEDs
    {
#ifdef DEBUG_LOOP
        TimeProfiler _tp_leds = TimeProfiler("leds");
#endif
        wifi_led->loop();
        power_led->loop();
    }
    delay(0);
    // PSU
    {
#ifdef DEBUG_LOOP
        TimeProfiler _tp_psu = TimeProfiler("psu");
#endif
        psu->loop();
        psuLog->loop();
    }
    delay(0);
    // Tasks
    {
        TimeProfiler _tp_timer = TimeProfiler("timer");
        timer.run();
    }
    delay(0);
#ifndef DISABLE_CONSOLE_SHELL
    {
        TimeProfiler _tp_console = TimeProfiler("console");
        if (consoleShell) consoleShell->loop();
    }
    delay(0);
#endif
/*
 *   LCD Display
 */
#ifndef DISABLE_LCD
    {
#ifdef DEBUG_LOOP
        TimeProfiler _tp_display = TimeProfiler("lcd", 24);
#endif
        if (display) display->redraw();
    }
    delay(0);
#endif

    if (wireless::hasNetwork()) {
#ifndef DISABLE_HTTP
        {
#ifdef DEBUG_LOOP
            TimeProfiler profiler = TimeProfiler("http");
#endif
            if (http) http->loop();
        }
        delay(0);
#endif
#ifndef DISABLE_NETWORK_DISCOVERY
        {
#ifdef DEBUG_LOOP
            TimeProfiler _tp_discover = TimeProfiler("net");
#endif
            if (discovery) discovery->loop();
        }
        delay(0);
#endif

#ifndef DISABLE_OTA_UPDATE
        if (ota) ota->loop();
        delay(0);
#endif
#ifndef DISABLE_NTP
        {
#ifdef DEBUG_LOOP
            TimeProfiler _tp_ntp = TimeProfiler("ntp");
#endif
            if (ntp) ntp->loop();
        }
        delay(0);
#endif
#ifndef DISABLE_TELNET
        {
#ifdef DEBUG_LOOP
            TimeProfiler _tp_telnet = TimeProfiler("telnet");
#endif
            if (telnet) telnet->loop();
        }
        delay(0);
#endif
#ifndef DISABLE_TELNET_SHELL
        {
#ifdef DEBUG_LOOP
            TimeProfiler _tp_shell = TimeProfiler("shell");
#endif
            if (telnetShell) telnetShell->loop();
        }
        delay(0);
#endif
    }
    loopWD.run();
}

void handle_power_button_press() {
    if (powerBtnPressEvent) {
        psu->togglePower();
        powerBtnPressEvent = false;
    }
    if (!digitalRead(POWER_BTN_PIN) && (powerBtnPressed)) {
        if (powerBtnLongPressCounter++ > 5) {
            setup_restart_timer(5);
        }
    } else {
        powerBtnLongPressCounter = 0;
    }
}
