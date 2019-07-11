#include "main.h"

uint8_t volatile powerBtnLongPressCounter;
unsigned long volatile powerBtnLastEvent = 0;
bool volatile powerBtnPressed = false;
bool volatile powerBtnReleased = true;
bool volatile powerBtnPressedHandled = false;

typedef struct {
    bool connected = false;
    uint8_t page = 0;
} WebClient;
WebClient clients[MAX_WEB_CLIENTS];

PowerState state = POWER_OFF;
float outputVoltage;

bool display_alt_line = false;

WiFiEventHandler disconnectedEventHandler, gotIpEventHandler;

Led *wifi_led, *power_led;

PowerState get_power_state() { return state; }

void set_power_state(PowerState value) {
    state = value;

    USE_SERIAL.printf_P(str_power);
    USE_SERIAL.print(' ');
    USE_SERIAL.println(state == POWER_ON ? "on" : "off");

    digitalWrite(POWER_SWITCH_PIN, state);

    if (state == POWER_ON) {
        meter->begin();
        power_led->blink();
    } else if (state == POWER_OFF) {
        meter->end();
        power_led->turnOn();
    }

    if (get_http_clients_count() > 0) {
        sendClients(String(POWER_ONOFF) + state, PAGE_HOME);
    }
    config->setLastPowerState(state);
}

uint8_t get_http_clients_count() {
    uint8_t result = 0;
    #ifndef DISABLE_HTTP
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected) result++;
    #endif
    return result;
}

uint8_t get_telnet_clients_count() {
#ifndef DISABLE_TELNET
    return telnet ? telnet->hasClientConnected() : 0;
#else
    return 0;
#endif
}

int quadraticRegression(double volt) {
    double a = 0.0000006562;
    double b = 0.0022084236;
    float c = 4.08;
    double d = b * b - a * (c - volt);
    double root = (-b + sqrt(d)) / a;
    if (root < 0)
        root = 0;
    else if (root > 255)
        root = 255;
    return root;
}

unsigned long volatile loopMeasureTime = 0;
unsigned long volatile loopStartTime = 0;
unsigned long volatile loopCounter = 0;
unsigned long volatile loopLongest = 0;

ulong get_lps() { return loopCounter * ONE_SECOND_ms / (millis() - loopMeasureTime); }

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

    if (restartCount <= START_SHOWING_RESTART_COUNTER_ON) {
        USE_SERIAL.print(restartCount);
        USE_SERIAL.print(' ');
        USE_SERIAL.flush();
    }
    restartCount--;
}

void setup_restart_timer(int count) {
    if (restartTimer != 1) {
        timer.deleteTimer(restartTimer);
    }
    restartCount = count;

    USE_SERIAL.print(F("System restart"));
    if (restartCount > 0) {
        USE_SERIAL.print(" in ");
        USE_SERIAL.print(restartCount);
        USE_SERIAL.println('s');
    } else {
        USE_SERIAL.print(' ');
        USE_SERIAL.print("now!");
    }
    restartTimer = timer.setInterval(ONE_SECOND_ms, on_restart_sequence);
}

void set_output_voltage(float value) {
    outputVoltage = value;

    mcp4652_write(WRITE_WIPER0, quadraticRegression(value));
}

void onClient(uint8_t num, bool state) { clients[num].connected = state; }

void onClientData(uint8_t num, String data) {
    switch (data.charAt(0)) {
        case PAGE_STATE: {
            uint8_t page = data.charAt(1) - 48;
            clients[num].page = page;
            sendOnPageState(num, page);
            break;
        }
        case POWER_ONOFF: {
            PowerState new_state = PowerState(data.substring(1).toInt());
            set_power_state(new_state);

            String payload = String(POWER_ONOFF);
            payload += state;
            sendClients(payload, PAGE_HOME, num);
            break;
        }
        case SET_VOLTAGE: {
            float _voltage = data.substring(1).toFloat();
            set_output_voltage(_voltage);
            sendClients(String(SET_VOLTAGE) + data.substring(1), PAGE_HOME,
                        num);
            break;
        }
        case SET_DEFAULT_VOLTAGE: {
            if (config->setOutputVoltage(outputVoltage)) {
                config->save();
            }
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
            if (config->setNetworkSTAConfig(_config_mode.toInt(), _ssid.c_str(),
                                            _password.c_str(), _dhcp.toInt(),
                                            _ipaddr.c_str(), _netmask.c_str(),
                                            _gateway.c_str(), _dns.c_str())) {
                config->save();
                for (int i = 0; i < MAX_WEB_CLIENTS; i++) {
                    if (clients[i].connected && clients[i].page && (num != i))
                        sendOnPageState(i, 1);
                }
            }
            break;
        }

        case MEASURE_MODE: {
            bool value = (bool)atoi(data.substring(1).c_str());

            meter->enableWattHours(value);

            String payload = String(MEASURE_MODE);
            payload += value;
            sendClients(payload, PAGE_HOME, num);
            break;
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
            payload += String(state);
            http->sendTxt(num, payload.c_str());

            // WattHours
            payload = String(MEASURE_MODE);
            payload += String(meter->isWattHoursEnabled());
            http->sendTxt(num, payload.c_str());
            break;
        }
        case PAGE_SETTINGS: {
            String payload;
            // Power mode
            payload = String(SET_POWERMODE);
            payload += config->getData()->getStrValue(POWER);
            http->sendTxt(num, payload.c_str());

            // Output voltage
            payload = String(SET_VOLTAGE);
            payload += outputVoltage;
            http->sendTxt(num, payload.c_str());

            // Network config
            payload = String(SAVE_NETWORKS);
            payload += config->getData()->getStrValue(WIFI);
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

void update_wifi_led() {
    if (isWiFiActive()) {
        if (get_http_clients_count() > 0) {
            wifi_led->blink();
        } else {
            wifi_led->turnOn();
        }
    } else {
        wifi_led->turnOff();
    }
}

#ifndef DISABLE_LCD
void update_display() {
    if (!display->isConnected()) return;

    String str;
    if (state == POWER_OFF) {
        if (WiFi.getMode() == WIFI_STA) {
            display->setItem(0, "STA >", WiFi.SSID().c_str());
            if (display_alt_line) {
                display->setItem(1, "IP >",
                                      WiFi.localIP().toString().c_str());
            } else
                display->setItem(1, "RSSI>", String(WiFi.RSSI()).c_str());
        }

        else if (WiFi.getMode() == WIFI_AP) {
            display->setItem(0, "AP >", WiFi.softAPSSID().c_str());
            display->setItem(1, "1D>", WiFi.softAPPSK().c_str());
        }
    } else if (state == POWER_ON) {
        String str = String(meter->getVoltage(), 3);
        str += " V ";
        str += String(meter->getCurrent(), 3);
        str += " A ";
        display->setItem(0, str.c_str());

        double watt = meter->getPower();
        str = String(watt, (watt < 10) ? 3 : 2);
        str += " W ";
        double rwatth = meter->getWattHours();
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
#endif 

void handle_power_button_press() {
    if (powerBtnPressedHandled) {
        set_power_state((state == POWER_OFF) ? POWER_ON : POWER_OFF);
        powerBtnPressedHandled = false;
    }

    if (!digitalRead(POWER_BTN_PIN) && (powerBtnPressed)) {
        if (powerBtnLongPressCounter++ == 3) {
            power_led->setFreq(50);
            power_led->blink(2);
        }
        if (powerBtnLongPressCounter++ > 5) {
            setup_restart_timer(5);
        }
    } else {
        powerBtnLongPressCounter = 0;
    }
}

void send_multimeter_data_to_clients() {
    if (state == POWER_ON) {
        if (network) {
            String data = meter->toString();
#ifndef DISABLE_TELNET_SHELL
            if (get_telnet_clients_count()) {
                telnet->write(data.c_str());
                telnet->write("\r");
            }
#endif

#ifndef DISABLE_HTTP
            if (get_http_clients_count()) {
                String payload = String(DATA_PVI);
                payload += data;
                sendClients(payload, PAGE_HOME);
            }
#endif
        }
    }
}

void setup_hardware() {
    Wire.begin(I2C_SDA, I2C_SCL);
    // Button
    pinMode(POWER_BTN_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(POWER_BTN_PIN), powerButtonHandler,
                    CHANGE);
    // Leds
    power_led = new Led(POWER_LED_PIN);
    wifi_led = new Led(WIFI_LED_PIN);
    // Switch
    pinMode(POWER_SWITCH_PIN, OUTPUT);
    ina231_configure();
    SPIFFS.begin();
}

void onBootProgress(uint8_t per, const char *message)
{
    #ifndef DISABLE_LCD
    if (display) display->onProgress(per, message);
    #else
        USE_SERIAL.printf_P(strf_boot_progress, message, per);
    #endif
}

void setup() {
    USE_SERIAL.begin(115200);
    // gdbstub_init();
#ifdef SERIAL_DEBUG
    USE_SERIAL.setDebugOutput(true);
#endif
    USE_SERIAL.println();
    USE_SERIAL.print(F("[reset] reason: "));
    USE_SERIAL.println(ESP.getResetReason());
    USE_SERIAL.print(F("[reset] info: "));
    USE_SERIAL.println(ESP.getResetInfo());
    
    setup_hardware();

    memset(&clients[0], 0x00,
           (sizeof(WebClient) * WEBSOCKETS_SERVER_CLIENT_MAX));

    config = new ConfigHelper();
    meter = new Multimeter();
    
    #ifndef DISABLE_LCD
    display = new Display();
    display->setOutput(&USE_SERIAL);
    if (display->init()) display->turnOn();
    #endif

    str_utils::printWelcomeTo(&USE_SERIAL);
    USE_SERIAL.print("[wait]");
    USE_SERIAL.print(" ");
    delaySequence(3);
    USE_SERIAL.println();

    onBootProgress(10, "VER>" FW_VERSION);    
    meter->begin();

    onBootProgress(20, "CFG>");
    config->init(FILE_CONFIG);

    onBootProgress(40, "WIFI>");
    start_wifi();

    onBootProgress(80, "START>");
    switch (config->getBootPowerState()) {
        case BOOT_POWER_OFF: {
            state = POWER_OFF;
            break;
        }
        case BOOT_POWER_ON: {
            state = POWER_ON;
            break;
        }
        case BOOT_POWER_LAST_STATE: {
            state = config->getLastPowerState();
            break;
        }
    }

    set_output_voltage(config->getOutputVoltage());

    set_power_state(state);

    init_cli();

    init_serial_shell(&USE_SERIAL);

    onBootProgress(100, "");

    timer.setInterval(1000, [] {
        handle_power_button_press();
        update_wifi_led();        
        send_multimeter_data_to_clients();
        #ifndef DISABLE_LCD
        update_display();
        #endif
    });

    timer.setInterval(5000, [] { display_alt_line = !display_alt_line; });

    timer.setInterval(LOOP_STATS_INTERVAL, [] {
        loopMeasureTime = millis();
        loopCounter = 0;
        loopLongest = 0;
    });

    loopStartTime = millis();
}

void loop() {
    // LEDs
    wifi_led->loop();
    power_led->loop();

    // Multimeter
    if (meter) meter->loop();

#ifndef DISABLE_HTTP
    if (http) http->loop();
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
    if (telnet_shell) telnet_shell->loop();
#endif
#ifndef DISABLE_SERIAL_SHELL
    if (serial_shell) serial_shell->loop();
#endif
#ifndef DISABLE_NETWORK_DISCOVERY
    if (network_discovery) network_discovery->loop();
#endif
#ifndef DISABLE_LCD
    if (display) display->redraw();
#endif
    rtc.loop();
    
    timer.run();

    /* loop timings */
    unsigned long LoopEndTime = millis();
    unsigned long LoopTime = LoopEndTime - loopStartTime;
    if (loopLongest < LoopTime) loopLongest = LoopTime;
    loopCounter++;
    loopStartTime = LoopEndTime;
}

void delaySequence(uint8_t sec) {
    for (uint8_t t = sec; t > 0; t--) {
        USE_SERIAL.print(t);
        USE_SERIAL.print(" ");
        USE_SERIAL.flush();
        delay(1000);
    }
}

void start_services() {
    IPAddress host = hostIP();

#ifndef DISABLE_HTTP
    http = new WebService(host, HTTP_PORT, WEBSOCKET_PORT, WEB_ROOT);
    http->setOutput(&USE_SERIAL);
    http->setOnConnection(onClient);
    http->setOnData(onClientData);
    http->begin();
#endif

#ifndef DISABLE_OTA_UPDATE
    USE_SERIAL.print(FPSTR(str_ota_update));
    ota = new OTAUpdate();
    ota->setOutput(&USE_SERIAL);
    if (ota->begin(HOSTNAME, OTA_PORT)) {
        USE_SERIAL.printf_P(strf_ip_port, HOSTNAME, OTA_PORT);
    } else {
        USE_SERIAL.print(FPSTR(str_failed));
    }
    USE_SERIAL.println();
#endif

#ifndef DISABLE_TELNET
    setup_telnet();
#endif

#ifndef DISABLE_NETWORK_DISCOVERY
    setup_network_discovery();
#endif

    setup_clock();
}

void start_wifi() {
    const uint8_t wifi_mode = config->getWiFiMode();
    const char *ap_ssid = config->getSSID_AP();
    const char *ap_passwd = config->getPassword_AP();
    IPAddress ap_ipaddr = config->getIPAddr_AP();
    WiFi.hostname(HOSTNAME);
    system_phy_set_max_tpw(config->getTPW());
    if ((wifi_mode == WIFI_AP) || (wifi_mode == WIFI_AP_STA)) {
        USE_SERIAL.printf("[wifi] ap %s ip: %s ", ap_ssid,
                          ap_ipaddr.toString().c_str());
        if (startAP(ap_ssid, ap_passwd, ap_ipaddr)) {
            USE_SERIAL.println("success");
            network = true;
        } else {
            USE_SERIAL.println("failed");
            char failsafe_ssid[32];
            strcpy(failsafe_ssid, APPNAME);
            strcat(failsafe_ssid, getChipId().c_str());
            USE_SERIAL.printf("[wifi] ap %s ip: %s ", failsafe_ssid,
                              ap_ipaddr.toString().c_str());
            if (startAP(failsafe_ssid, ap_passwd, ap_ipaddr)) {
                USE_SERIAL.println("success");
                network = true;
            } else {
                USE_SERIAL.println("failed");
            }
        }
    }

    if (network) start_services();

    if (wifi_mode == WIFI_STA || wifi_mode == WIFI_AP_STA) {
        disconnectedEventHandler = WiFi.onStationModeDisconnected(
            [](const WiFiEventStationModeDisconnected &event) {
                if (network) {
                    USE_SERIAL.println("[wifi] sta disconnected");
                    network = false;
                }
            });

        gotIpEventHandler =
            WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event) {
                USE_SERIAL.println("[wifi] sta connected");
                network = true;
                uint8_t broadcast_if = wifi_get_broadcast_if();
                char buf[32];
                sprintf(buf, "[network] change broadcast if%d to if%d",
                        broadcast_if, 3);
                USE_SERIAL.print(buf);
                if (!wifi_set_broadcast_if(3)) {
                    USE_SERIAL.print(' ');
                    USE_SERIAL.println("failed");
                }
                start_services();
            });

        const char *ssid = config->getSSID();
        const char *passwd = config->getPassword();
        const bool dhcp = config->getDHCP();

        if (dhcp) {
            USE_SERIAL.print("[wifi] sta ");
            USE_SERIAL.print(ssid);
            USE_SERIAL.println(" dhcp on");
            startSTA(ssid, passwd);
        } else {
            IPAddress ipaddr, subnet, gateway, dns;

            ipaddr = config->getIPAddr();
            subnet = config->getNetmask();
            gateway = config->getGateway();
            dns = config->getDNS();
            USE_SERIAL.print("[wifi] sta ");
            USE_SERIAL.print(ssid);
            USE_SERIAL.print(" ip ");
            USE_SERIAL.println(ipaddr.toString().c_str());
            startSTA(ssid, passwd, ipaddr, subnet, gateway, dns);
        }
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
            powerBtnPressedHandled = true;
        }
    }
}
