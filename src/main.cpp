#include "main.h"

#include "BuildConfig.h"
#include "Cli.h"
#include "CommonTypes.h"
#include "Consts.h"
#include "Global.h"
#include "Led.h"
#include "LoopWatchDog.h"
#include "StrUtils.h"
#include "SysInfo.h"
#include "Wireless.h"

Profiler::LoopWatchDog watchDog;

uint8_t get_http_clients_count() {
    uint8_t result = 0;
#ifndef DISABLE_HTTP
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected) result++;
#endif
    return result;
}

int restartTimer = -1;
int restartCount = 0;

void cancel_system_restart() {
    if (restartTimer != -1) timer.disable(restartTimer);
}

void on_restart_sequence() {
    if (--restartCount == 0) system_restart();
}

void setup_restart_timer(uint8_t delay_s) {
    if (restartTimer != -1) timer.deleteTimer(restartTimer);
    restartCount = delay_s;
    if (restartCount > 0) {
        USE_SERIAL.printf(strf_in_second, restartCount);
        USE_SERIAL.println();
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
            if (new_state != psu->getState()) {
                psu->togglePower();
                String payload = String(SET_POWER_ON_OFF);
                payload += psu->getState();
                sendToClients(payload, PG_HOME, n);
            }
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
                config->saveConfig();
                sendToClients(SET_BOOT_POWER_MODE + data.substring(1).c_str(),
                              PG_SETTINGS, n);
            }
            break;
        }
        case SET_NETWORK: {
            static const ConfigItem items[] = {WIFI,   SSID,    PASSWD,  DHCP,
                                               IPADDR, NETMASK, GATEWAY, DNS};
            static const size_t paramCount = sizeof(items) / sizeof(ConfigItem);
            size_t index = 0;
            size_t last = 0;
            size_t pos = 0;
            while (index < paramCount && (pos = data.indexOf(",", last))) {
                config->get()->setValueString(
                    items[index++], data.substring(last, pos).c_str());
                last = pos + 1;
            }
            config->saveConfig();
            break;
        }
        case SET_LOG_WATTHOURS: {
            char ch = data.charAt(1);
            if (isdigit(ch)) {
                bool mode = (uint8_t)ch - CHR_ZERO;
                if (!psu->enableWhStore(mode)) {
                    // To all
                    sendToClients(data, PG_HOME);
                } else {
                    // Except sender
                    sendToClients(data, PG_HOME, n);
                }
            }
            break;
        }
    }
}

void sendToClients(String payload, uint8_t page, uint8_t except_n) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected && (clients[i].page == page) &&
            (except_n != i))
            http->sendTxt(i, payload);
}

void sendToClients(String payload, uint8_t page) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected && clients[i].page == page)
            http->sendTxt(i, payload);
}

void sendPageState(uint8_t page) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected && clients[i].page == page)
            sendPageState(i, page);
}

void sendPageState(uint8_t n, uint8_t page) {
    switch (page) {
        case PG_HOME: {
            // State
            String stateStr = String(SET_POWER_ON_OFF);
            stateStr += String(psu->getState());
            http->sendTxt(n, stateStr);
            // WattHour
            String whStoreEnabledStr = String(SET_LOG_WATTHOURS);
            whStoreEnabledStr += String(psu->isWhStoreEnabled());
            http->sendTxt(n, whStoreEnabledStr);
            break;
        }
        case PG_SETTINGS: {
            String payload;
            // Power mod
            payload = String(SET_BOOT_POWER_MODE);
            payload += config->get()->getValueAsByte(POWER);
            http->sendTxt(n, payload);
            // Output voltage
            payload = String(SET_VOLTAGE);
            payload += String(psu->getOutputVoltage(), 2);
            http->sendTxt(n, payload);
            // Network config
            payload = String(SET_NETWORK);
            payload += config->getWiFiMode();
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
            payload += config->getDnsStr();
            http->sendTxt(n, payload);
            break;
        }
        case PG_STATUS: {
            String payload;
            // Version info
            payload = String(TAG_FIRMWARE_INFO);
            payload += getVersionInfoJson();
            http->sendTxt(n, payload);
            // System info
            payload = String(TAG_SYSTEM_INFO);
            payload += getSystemInfoJson();
            http->sendTxt(n, payload);
            // Network info
            payload = String(TAG_NETWORK_INFO);
            payload += getNetworkInfoJson();
            http->sendTxt(n, payload);
            break;
        }
    }
}

void send_psu_data_to_clients() {
    if (psu->getState() == POWER_ON) {
        PsuInfo pi = psu->getInfo();
        if (Wireless::hasNetwork()) {
#ifndef DISABLE_TELNET
            if (get_telnet_clients_count() && !telnetShell->isActive()) {
                String data = pi.toString();
                data += '\r';
                telnet->write(data.c_str());
            }
#endif
#ifndef DISABLE_HTTP
            if (get_http_clients_count()) {
                String data = String(TAG_PVI);
                data += pi.toString();
                sendToClients(data, PG_HOME);
            }
#endif
        }
    }
}

uint8_t boot_progress = 0;
void display_boot_progress(uint8_t per, const char *payload) {
    while (per - boot_progress > 0) {
        boot_progress += 5;
#ifndef DISABLE_LCD
        if (display) display->drawProgressBar(LCD_ROW_2, per);
#endif
        delay(100);
    }
#ifndef DISABLE_LCD
    if (display && payload != NULL) display->drawTextCenter(LCD_ROW_1, payload);
#endif
}

void delay_print(Print *p) {
    p->print(StrUtils::getIdentStrP(str_wait));
    for (uint8_t t = BOOT_WAIT_s; t > 0; t--) {
        p->print(t);
        p->print(' ');
        delay(ONE_SECOND_ms);
    }
    p->println();
    p->flush();
}

void print_welcome(Print *p) {
    size_t width = SCREEN_WIDTH / 2;
    char tmp[width + 1];

    strcpy(tmp, " Welcome ");
    StrUtils::strpadd(tmp, StrUtils::CENTER, width, '#');
    p->println(tmp);

    strcpy(tmp, APPNAME " v" FW_VERSION);
    StrUtils::strpadd(tmp, StrUtils::CENTER, width);
    p->println(tmp);

    strcpy(tmp, " " BUILD_DATE " ");
    StrUtils::strpadd(tmp, StrUtils::CENTER, width, '#');
    p->println(tmp);
}

void setup() {
    // Try pushing frequency to 160MHz.
    system_update_cpu_freq(SYS_CPU_160MHZ);

    USE_SERIAL.begin(115200);
#ifdef SERIAL_DEBUG
    USE_SERIAL.setDebugOutput(true);
#endif
    Wire.begin(I2C_SDA, I2C_SCL);
    SPIFFS.begin();
    config = new ConfigHelper();

    // Leds
    power_led = new Led::Led(POWER_LED_PIN, Led::LIGHT_ON, true);
    wifi_led = new Led::Led(WIFI_LED_PIN, Led::LIGHT_OFF, true);

    USE_SERIAL.println();
    print_welcome(&USE_SERIAL);

    // Button
    pinMode(POWER_BTN_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(POWER_BTN_PIN),
                    power_button_state_change, CHANGE);


    memset(clients, 0, sizeof(WebClient) * WEBSOCKETS_SERVER_CLIENT_MAX);

    printResetInfo(&USE_SERIAL);    
#ifndef DISABLE_LCD
    start_lcd();
#endif
    delay_print(&USE_SERIAL);

    display->setScreen(SCREEN_BOOT, 0);

    display_boot_progress(0, BUILD_DATE);

    start_clock();

    init_psu();

    display_boot_progress(40, "<WIFI>");

    Wireless::setOnNetworkStateChange(
        [](bool hasNetwork) { refresh_wifi_led(); });

    Wireless::start_wifi();

    display_boot_progress(80, "<INIT>");

    Cli::init();
#ifndef DISABLE_CONSOLE_SHELL
    start_console_shell();
#endif

    display_boot_progress(100, "<COMPLETE>");

    timer.setInterval(1000, [] { send_psu_data_to_clients(); });

#ifndef DISABLE_LCD
    if ((display) && (display->ready())) {
        timer.setInterval(1000, [] { update_display(); });
    }
#endif
}

void loop() {
    // Button
    {
        Profiler::TimeProfiler tp = watchDog.run(Profiler::BUTTONS);
        power_button_handler();
    }
    delay(0);
    // Clock
    {
        Profiler::TimeProfiler tp = watchDog.run(Profiler::CLOCK);
        rtc.loop();
    }
    delay(0);
    // LEDs
    {
        Profiler::TimeProfiler tp = watchDog.run(Profiler::LEDS);
        wifi_led->loop();
        power_led->loop();
    }
    delay(0);
    // PSU
    {
        Profiler::TimeProfiler tp = watchDog.run(Profiler::PSU);
        psu->loop();
        psuLog->loop();
    }
    delay(0);
    // Tasks
    {
        Profiler::TimeProfiler tp = watchDog.run(Profiler::TASKS);
        timer.run();
    }
    delay(0);
#ifndef DISABLE_CONSOLE_SHELL
    {
        Profiler::TimeProfiler tp = watchDog.run(Profiler::SERIAL_SHELL);
        if (consoleShell) consoleShell->loop();
    }
    delay(0);
#endif
/*
 *   LCD Display
 */
#ifndef DISABLE_LCD
    {
        Profiler::TimeProfiler tp = watchDog.run(Profiler::LCD);
        if (display) display->loop();
    }
    delay(0);
#endif
    if (Wireless::hasNetwork()) {
#ifndef DISABLE_HTTP
        {
            Profiler::TimeProfiler tp = watchDog.run(Profiler::HTTP);
            if (http) http->loop();
        }
        delay(0);
#endif
#ifndef DISABLE_NETWORK_DISCOVERY
        {
            Profiler::TimeProfiler tp = watchDog.run(Profiler::NETSVC);
            if (discovery) discovery->loop();
        }
        delay(0);
#endif
#ifndef DISABLE_OTA_UPDATE
        {
            Profiler::TimeProfiler tp = watchDog.run(Profiler::OTA_UPDATE);
            if (ota) ota->loop();
        }
        delay(0);
#endif
#ifndef DISABLE_NTP
        {
            Profiler::TimeProfiler tp = watchDog.run(Profiler::NTP);
            if (ntp) ntp->loop();
        }
        delay(0);
#endif
#ifndef DISABLE_TELNET
        {
            Profiler::TimeProfiler tp = watchDog.run(Profiler::TELNET);
            if (telnet) telnet->loop();
        }
        delay(0);
#endif
#ifndef DISABLE_TELNET_SHELL
        {
            Profiler::TimeProfiler tp = watchDog.run(Profiler::TELNET_SHELL);
            if (telnetShell) telnetShell->loop();
        }
        delay(0);
#endif
    }
    watchDog.loop();
}

enum ButtonState { BTN_PRESSED, BTN_RELEASED };
uint8_t volatile powerBtnLongPressCounter;
unsigned long volatile power_btn_last_event = 0;
ButtonState volatile power_btn_state = BTN_RELEASED;
bool volatile powerButtonClicked = false;
static void ICACHE_RAM_ATTR power_button_state_change() {
    unsigned long now = millis();
    if (millis_passed(power_btn_last_event, now) < 50) return;
    if (!digitalRead(POWER_BTN_PIN) && power_btn_state != BTN_PRESSED) {
        power_btn_last_event = now;
        power_btn_state = BTN_PRESSED;
    }
    if (digitalRead(POWER_BTN_PIN) && power_btn_state != BTN_RELEASED) {
        if (millis_passed(power_btn_last_event, now) < 1000)
            powerButtonClicked = true;
        power_btn_last_event = now;
        power_btn_state = BTN_RELEASED;
    }
}

unsigned long powerButtonUpdated = 0;
void power_button_handler() {
    unsigned long now = millis();
    if (powerButtonClicked) {
        psu->togglePower();
        powerBtnLongPressCounter = 0;
        powerButtonClicked = false;
    } else if (millis_passed(powerButtonUpdated, now) >= ONE_SECOND_ms) {
        if (power_btn_state == BTN_PRESSED) {
            if (++powerBtnLongPressCounter >= 10) {
                config->setDefault();
                config->saveConfig();
                setup_restart_timer();
            } else if (powerBtnLongPressCounter == 5) {
                power_led->set(Led::BLINK_ERROR);
            }
        } else {
            if (powerBtnLongPressCounter > 0) {
                power_led->set(psu->getState() == POWER_ON ? Led::BLINK
                                                           : Led::ON);
                powerBtnLongPressCounter = 0;
            }
        }
        powerButtonUpdated = now;
    }
}