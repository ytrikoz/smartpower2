#include "main.h"

#include "cli.h"
#include "global.h"
#include "str_utils.h"

#include "Consts.h"
#include "Led.h"
#include "SysInfo.h"
#include "wireless.h"

#define I2C_SDA D2
#define I2C_SCL D5
#define POWER_LED_PIN D1
#define WIFI_LED_PIN D4
#define POWER_BTN_PIN D7

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

unsigned long volatile resetStatTime = 0;
unsigned long volatile loopStartTime = 0;
unsigned long volatile loopCounter = 0;
unsigned long volatile loopLongest = 0;
ulong get_lps() {
    return loopCounter * ONE_SECOND_ms / (millis() - resetStatTime);
}

ulong get_longest_loop() { return loopLongest; }

int restartTimer = -1;
int restartCount = 0;

void cancel_system_restart() {
    if (restartTimer != -1) {
        timer.disable(restartTimer);
    }
}
void on_restart_sequence() {
    if (restartCount <= 0) {
        system_restart();
    }
    restartCount--;
}

void setup_restart_timer(int count) {
    if (restartTimer != 1) {
        timer.deleteTimer(restartTimer);
    }
    restartCount = count;
    USE_SERIAL.print(F("System restart "));
    if (restartCount > 0) {
        USE_SERIAL.printf(strf_in_second, restartCount);
        USE_SERIAL.println();
    } else {
        USE_SERIAL.println(F("now!"));
    }
    restartTimer = timer.setInterval(ONE_SECOND_ms, on_restart_sequence);
}

void onHttpClientConnect(uint8_t num) {
    clients[num].connected = true;
    refresh_wifi_status_led();
}
void onHttpClientDisconnect(uint8_t num) {
    clients[num].connected = false;
    refresh_wifi_status_led();
}

void onHttpClientData(uint8_t num, String data) {
    switch (data.charAt(0)) {
        case PAGE_STATE: {
            uint8_t page = data.charAt(1) - CHR_ZERO;
            clients[num].page = page;
            sendOnPageState(num, page);
            break;
        }
        case POWER_ONOFF: {
            PowerState new_state = PowerState(data.substring(1).toInt());

            psu->setState(new_state);

            String payload = String(POWER_ONOFF);
            payload += psu->getState();
            sendClients(payload, PAGE_HOME, num);
            break;
        }
        case SET_VOLTAGE: {
            float _voltage = data.substring(1).toFloat();
            psu->setOutputVoltage(_voltage);
            sendClients(String(SET_VOLTAGE) + data.substring(1), PAGE_HOME,
                        num);
            break;
        }
        case SET_DEFAULT_VOLTAGE: {
            float _voltage = psu->getOutputVoltage();
            config->setOutputVoltage(_voltage);
            break;
        }
        case SET_POWERMODE: {
            if (config->setBootPowerState(
                    BootPowerState(data.substring(1).toInt()))) {
                config->save();
                sendClients(SET_POWERMODE + data.substring(1).c_str(),
                            PAGE_SETTINGS, num);
            }
            break;
        }
        case SAVE_NETWORKS: {
            static const Parameter items[8] = {WIFI,   SSID,    PASSWD,  DHCP,
                                               IPADDR, NETMASK, GATEWAY, DNS};
            uint8_t last = 0;
            uint8_t pos = 0;
            uint8_t index = 0;
            while ((index < 8) && (pos = data.indexOf(",", last))) {
                config->getConfig()->setValue(
                    items[index++], data.substring(last, pos).c_str());
                last = pos + 1;
            }
            config->save();
            for (int i = 0; i < MAX_WEB_CLIENTS; i++)
                if (clients[i].connected && clients[i].page && (num != i))
                    sendOnPageState(i, 1);
            break;
            /*
            String _config_mode = data.substring(1, data.indexOf(','));
            data.remove(1, data.indexOf(','));
            String _ssid = data.substring(1, data.indexOf(','));
            data.remove(1, data.indexOf(','));
            String _password = data.substring(1, data.indexOf(','));
            data.remove(1, data.indexOf(','));
            String _dhcp = data.substring(1, data.indexOf(','));
            data.remove(1, data.indexOf(','));
            String _ipaddr = data.substring(1, data.indexOf(','));
            data.remove(1, data.indexOf(','));
            String _netmask = data.substring(1, data.indexOf(','));
            data.remove(1, data.indexOf(','));
            String _gateway = data.substring(1, data.indexOf(','));
            data.remove(1, data.indexOf(','));
            String _dns = data.substring(1, data.indexOf(','));

            if (config->setNetworkSTAConfig(_config_mode.toInt(),
            _ssid.c_str(), _password.c_str(), _dhcp.toInt(),
                                            _ipaddr.c_str(),
            _netmask.c_str(), _gateway.c_str(), _dns.c_str()))
            */
        }
        case SET_MEASURE_MODE: {
            char ch = data.charAt(1);
            if (isdigit(ch)) {
                bool mode = (uint8_t)ch - CHR_ZERO;
                psu->enableWattHoursCalculation(mode);
                sendClients(data, PAGE_HOME, num);
                break;
            }
        }
    }
}

void sendClients(String payload, uint8_t page, uint8_t except_num) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected && (clients[i].page == page) &&
            (except_num != i))
            http->sendTxt(i, payload.c_str());
}

void sendClients(String payload, uint8_t page) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected && clients[i].page == page)
            http->sendTxt(i, payload.c_str());
}

void sendOnPageState(uint8_t num, uint8_t page) {
    switch (page) {
        case PAGE_HOME: {
            String payload;
            // State
            payload = String(POWER_ONOFF);
            payload += String(psu->getState());
            http->sendTxt(num, payload.c_str());

            // WattHours
            payload = String(SET_MEASURE_MODE);
            payload += String(psu->isWattHoursCalculationEnabled());
            http->sendTxt(num, payload.c_str());
            break;
        }
        case PAGE_SETTINGS: {
            String payload;
            // Power mode
            payload = String(SET_POWERMODE);
            payload += config->getConfig()->getStrValue(POWER);
            http->sendTxt(num, payload.c_str());

            // Output voltage
            payload = String(SET_VOLTAGE);
            payload += psu->getOutputVoltage();
            http->sendTxt(num, payload.c_str());

            // Network config
            payload = String(SAVE_NETWORKS);
            payload += config->getConfig()->getStrValue(WIFI);
            payload += ',';
            payload += config->getSSID();
            payload += ',';
            payload += config->getPassword();
            payload += ',';
            payload += config->getDHCP();
            payload += ',';
            payload += config->getIPAddrStr();
            payload += ',';
            payload += config->getNetmaskStr();
            payload += ',';
            payload += config->getGatewayStr();
            payload += ',';
            payload += config->getDNSStr();
            http->sendTxt(num, payload.c_str());
            break;
        }
        case PAGE_STATUS: {
            String payload;
            // Version info
            payload = String(TAG_FIRMWARE_INFO);
            payload += getVersionInfoJson();
            http->sendTxt(num, payload.c_str());
            // System info
            payload = String(TAG_SYSTEM_INFO);
            payload += getSystemInfoJson();
            http->sendTxt(num, payload.c_str());
            // Network info
            payload = String(TAG_NETWORK_INFO);
            payload += getNetworkInfoJson();
            http->sendTxt(num, payload.c_str());
            break;
        }
    }
}

void update_display() {
    if (!display->isConnected()) return;
    String str;
    if (psu->getState() == POWER_OFF) {
        if (WiFi.getMode() == WIFI_STA) {
            display->setItem(0, "STA >", WiFi.SSID().c_str());
            if (display_alt_line) {
                display->setItem(1, "IP >", WiFi.localIP().toString().c_str());
            } else
                display->setItem(1, "RSSI>", String(WiFi.RSSI()).c_str());
        } else if (WiFi.getMode() == WIFI_AP) {
            display->setItem(0, "AP >", WiFi.softAPSSID().c_str());
            display->setItem(1, "1D>", WiFi.softAPPSK().c_str());
        }
    } else if (psu->getState() == POWER_ON) {
        String str = String(psu->getVoltage(), 3);
        str += " V ";
        str += String(psu->getCurrent(), 3);
        str += " A ";
        display->setItem(0, str.c_str());

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
        display->setItem(1, str.c_str());
    }
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
                String payload = String(DATA_PVI);
                payload += data;
                sendClients(payload, PAGE_HOME);
            }
#endif
        }
    }
}

void setup_hardware() {
    SPIFFS.begin();

    Wire.begin(I2C_SDA, I2C_SCL);

    // Button
    pinMode(POWER_BTN_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(POWER_BTN_PIN), powerButtonHandler,
                    CHANGE);

    // Leds
    power_led = new Led(POWER_LED_PIN);
    power_led->setStyle(STAY_ON);

    wifi_led = new Led(WIFI_LED_PIN);
    wifi_led->setStyle(STAY_OFF);
}

void onBootProgress(uint8_t per, const char *message) {
#ifndef DISABLE_LCD
    if (display) display->onProgress(per, message);
#else
    USE_SERIAL.printf_P(strf_boot_progress, message, per);
#endif
}

void print_reset_reason() {
    USE_SERIAL.print(F("[reset] reason: "));
    USE_SERIAL.println(ESP.getResetReason());
    USE_SERIAL.print(F("[reset] info: "));
    USE_SERIAL.println(ESP.getResetInfo());
}

void setup() {
    USE_SERIAL.begin(115200);
    // gdbstub_init();
#ifdef SERIAL_DEBUG
    USE_SERIAL.setDebugOutput(true);
#endif
    USE_SERIAL.println();

    print_reset_reason();

    setup_hardware();

    memset(&clients[0], 0x00,
           (sizeof(WebClient) * WEBSOCKETS_SERVER_CLIENT_MAX));
#ifndef DISABLE_LCD
    display = new Display();
    display->setOutput(&USE_SERIAL);
    if (display->init()) display->turnOn();
#endif

    onBootProgress(20, "CFG>");
    config = new ConfigHelper();
    config->init(FILE_CONFIG);

    start_clock();

    start_psu();

    USE_SERIAL.print("[wait]");
    USE_SERIAL.print(" ");
    delaySequence(3);
    USE_SERIAL.println();

    onBootProgress(30, "VER>" FW_VERSION);

    str_utils::printWelcomeTo(&USE_SERIAL);

    onBootProgress(40, "WIFI>");

    wireless::setOnNetworkStateChange(
        [](bool hasNetwork) { refresh_wifi_status_led(); });
    wireless::start_wifi();

    onBootProgress(80, "START>");

    CLI::init();
    onBootProgress(100, "");

    timer.setInterval(100, [] { send_psu_data_to_clients(); });

    timer.setInterval(1000, [] {
        handle_power_button_press();
#ifndef DISABLE_LCD
        update_display();
#endif
    });

    timer.setInterval(5000, [] { display_alt_line = !display_alt_line; });

#ifndef DISABLE_CONSOLE_SHELL
    start_console_shell();
#endif
}

void delaySequence(uint8_t sec) {
    for (uint8_t t = sec; t > 0; t--) {
        USE_SERIAL.print(t);
        USE_SERIAL.print(" ");
        USE_SERIAL.flush();
        delay(1000);
    }
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
    // LEDs
    wifi_led->loop();
    power_led->loop();
    // PSU
    psu->loop();
    // Clock
    rtc.loop();
    // Tasks
    timer.run();

#ifndef DISABLE_CONSOLE_SHELL
    if (consoleShell) consoleShell->loop();
#endif
#ifndef DISABLE_LCD
    if (display) display->redraw();
#endif

    if (wireless::hasNetwork()) {
#ifndef DISABLE_HTTP
        if (http) http->loop();
#endif
#ifndef DISABLE_NETWORK_DISCOVERY
        if (discovery) discovery->loop();
#endif
#ifndef DISABLE_OTA_UPDATE
        if (ota) ota->loop();
#endif
#ifndef DISABLE_NTP
        if (ntp) ntp->loop();
#endif
#ifndef DISABLE_TELNET
        if (telnet) telnet->loop();
#endif
#ifndef DISABLE_TELNET_SHELL
        if (telnetShell) telnetShell->loop();
#endif
    }

    calcLoopTimings();
}

void calcLoopTimings() {
    unsigned long LoopEndTime = millis();
    if (LoopEndTime - resetStatTime > LOOP_STATS_INTERVAL) {
        resetStatTime = LoopEndTime;
        loopCounter = 0;
        loopLongest = 0;
    };

    unsigned long LoopTime = LoopEndTime - loopStartTime;

    if (loopLongest < LoopTime) loopLongest = LoopTime;
    loopCounter++;
    loopStartTime = millis();
}