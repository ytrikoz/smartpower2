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

typedef struct {
    bool connected = false;
    uint8_t page = 0;
} WebClient;
WebClient clients[MAX_WEB_CLIENTS];

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
    if (restartCount == 0) system_restart();
    restartCount--;
}

void setup_restart_timer(uint8_t delay_s) {
    if (restartTimer != -1) timer.deleteTimer(restartTimer);
    restartCount = delay_s;
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

            String text = String(SET_POWER_ON_OFF);
            text += psu->getState();
            sendToClients(text, PG_HOME, n);
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

void sendToClients(String text, uint8_t page, uint8_t except_n) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected && (clients[i].page == page) &&
            (except_n != i))
            http->sendTxt(i, text.c_str());
}

void sendToClients(String text, uint8_t page) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected && clients[i].page == page)
            http->sendTxt(i, text.c_str());
}

void sendPageState(uint8_t n, uint8_t page) {
    switch (page) {
        case PG_HOME: {
            String text;
            // State
            text = String(SET_POWER_ON_OFF);
            text += String(psu->getState());
            http->sendTxt(n, text.c_str());
            // WattHour
            text = String(SET_MEASURE_MODE);
            text += String(psu->isWattHoursCalculationEnabled());
            http->sendTxt(n, text.c_str());
            break;
        }
        case PG_SETTINGS: {
            String text;
            // Power mod
            text = String(SET_BOOT_POWER_MODE);
            text += config->getConfig()->getStrValue(POWER);
            http->sendTxt(n, text.c_str());
            // Output voltage
            text = String(SET_VOLTAGE);
            text += psu->getOutputVoltage();
            http->sendTxt(n, text.c_str());
            // Network config
            text = String(SET_NETWORK);
            text += config->getConfig()->getStrValue(WIFI);
            text += ',';
            text += config->getSSID();
            text += ',';
            text += config->getPassword();
            text += ',';
            text += config->getDHCP();
            text += ',';
            text += config->getIPAddrStr();
            text += ',';
            text += config->getNetmaskStr();
            text += ',';
            text += config->getGatewayStr();
            text += ',';
            text += config->getDNSStr();
            http->sendTxt(n, text.c_str());
            break;
        }
        case PG_STATUS: {
            String text;
            // Version info
            text = String(TAG_FIRMWARE_INFO);
            text += getVersionInfoJson();
            http->sendTxt(n, text.c_str());
            // System info
            text = String(TAG_SYSTEM_INFO);
            text += getSystemInfoJson();
            http->sendTxt(n, text.c_str());
            // Network info
            text = String(TAG_NETWORK_INFO);
            text += getNetworkInfoJson();
            http->sendTxt(n, text.c_str());
            break;
        }
    }
}

void send_psu_data_to_clients() {
    if (psu->getState() == POWER_ON) {
        if (Wireless::hasNetwork()) {
#ifndef DISABLE_TELNET
            if (get_telnet_clients_count() && !telnetShell->isActive()) {
                String data = psu->toString();
                data += '\r';
                telnet->write(data.c_str());
            }
#endif
#ifndef DISABLE_HTTP
            if (get_http_clients_count()) {
                String data = String(TAG_PVI);
                data += psu->toString();
                sendToClients(data, PG_HOME);
            }
#endif
        }
    }
}

uint8_t boot_progress = 0;
void display_boot_progress(uint8_t per, const char *text) {
    while (per - boot_progress > 0) {
        boot_progress += 5;
#ifndef DISABLE_LCD
        if (display) {
            if (text != NULL) display->drawTextCenter(LCD_ROW_1, text);
            display->loadBank(BANK_PROGRESS);
            display->drawProgressBar(LCD_ROW_2, per);
        }
#endif
        delay(100);
    }
}

void delay_print(Print *p) {
    p->print(FPSTR(str_wait));
    for (uint8_t t = BOOT_WAIT_s; t > 0; t--) {
        p->print(t);
        p->print(" ");
        delay(1000);
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
    USE_SERIAL.begin(115200);
#ifdef SERIAL_DEBUG
    USE_SERIAL.setDebugOutput(true);
#endif
    // Leds
    power_led = new Led::Led(POWER_LED_PIN, Led::LIGHT_ON, true);
    wifi_led = new Led::Led(WIFI_LED_PIN, Led::LIGHT_OFF, true);

    USE_SERIAL.println();
    print_welcome(&USE_SERIAL);

    // Button
    pinMode(POWER_BTN_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(POWER_BTN_PIN),
                    power_button_state_change, CHANGE);

    printResetInfo(&USE_SERIAL);

    SPIFFS.begin();

    Wire.begin(I2C_SDA, I2C_SCL);

    memset(&clients[0], 0x00, sizeof(WebClient) * WEBSOCKETS_SERVER_CLIENT_MAX);

#ifndef DISABLE_LCD
    display = new Display();
    display->setOutput(&USE_SERIAL);
    if (display->init()) display->turnOn();
#endif
    delay_print(&USE_SERIAL);

    display_boot_progress(10, BUILD_DATE);

    config = new ConfigHelper();
    config->init(FILE_CONFIG);

    start_clock();

    start_psu();

    display_boot_progress(40, "<WIFI>");

    Wireless::setOnNetworkStateChange(
        [](bool hasNetwork) { refresh_wifi_led(); });
    Wireless::start_wifi();

    display_boot_progress(80, "<INIT>");

    Cli::init();
#ifndef DISABLE_CONSOLE_SHELL
    start_console_shell();
#endif

    timer.setInterval(1000, [] { send_psu_data_to_clients(); });

#ifndef DISABLE_LCD
    if ((display) && (display->ready())) {
        timer.setInterval(1000, [] { update_display_every_1_sec(); });
        timer.setInterval(5000, [] { update_display_every_5_sec(); });
    }
#endif

    display_boot_progress(100, "<COMPLETE>");
}

void loop() {
    // Button
    {
#ifdef DEBUG_LOOP
        Profiler::TimeProfiler tp = watchDog.run(Profiler::BUTTONS);
#endif
        power_button_handler();
    }
    delay(0);
    // Clock
    {
#ifdef DEBUG_LOOP
        Profiler::TimeProfiler tp = watchDog.run(Profiler::CLOCK);
#endif
        rtc.loop();
    }
    delay(0);
    // LEDs
    {
#ifdef DEBUG_LOOP
        Profiler::TimeProfiler tp = watchDog.run(Profiler::LEDS);
#endif
        wifi_led->loop();
        power_led->loop();
    }
    delay(0);
    // PSU
    {
#ifdef DEBUG_LOOP
        Profiler::TimeProfiler tp = watchDog.run(Profiler::PSU);
#endif
        psu->loop();
        psuLog->loop();
    }
    delay(0);
    // Tasks
    {
#ifdef DEBUG_LOOP
        Profiler::TimeProfiler tp = watchDog.run(Profiler::TASKS);
#endif
        timer.run();
    }
    delay(0);
#ifndef DISABLE_CONSOLE_SHELL
    {
#ifdef DEBUG_LOOP
        Profiler::TimeProfiler tp = watchDog.run(Profiler::SERIAL_SHELL);
#endif
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
        Profiler::TimeProfiler tp = watchDog.run(Profiler::LCD);
#endif
        if (display) display->loop();
    }
    delay(0);
#endif
    if (Wireless::hasNetwork()) {
#ifndef DISABLE_HTTP
        {
#ifdef DEBUG_LOOP
            Profiler::TimeProfiler tp = watchDog.run(Profiler::HTTP);
#endif
            if (http) http->loop();
        }
        delay(0);
#endif
#ifndef DISABLE_NETWORK_DISCOVERY
        {
#ifdef DEBUG_LOOP
            Profiler::TimeProfiler tp = watchDog.run(Profiler::NETSVC);
#endif
            if (discovery) discovery->loop();
        }
        delay(0);
#endif
#ifndef DISABLE_OTA_UPDATE
        {
#ifdef DEBUG_LOOP
            Profiler::TimeProfiler tp = watchDog.run(Profiler::OTA_UPDATE);
#endif
            if (ota) ota->loop();
        }
        delay(0);
#endif
#ifndef DISABLE_NTP
        {
#ifdef DEBUG_LOOP
            Profiler::TimeProfiler tp = watchDog.run(Profiler::NTP);
#endif
            if (ntp) ntp->loop();
        }
        delay(0);
#endif
#ifndef DISABLE_TELNET
        {
#ifdef DEBUG_LOOP
            Profiler::TimeProfiler tp = watchDog.run(Profiler::TELNET);
#endif
            if (telnet) telnet->loop();
        }
        delay(0);
#endif
#ifndef DISABLE_TELNET_SHELL
        {
#ifdef DEBUG_LOOP
            Profiler::TimeProfiler tp = watchDog.run(Profiler::TELNET_SHELL);
#endif
            if (telnetShell) telnetShell->loop();
        }
        delay(0);
#endif
    }
#ifdef DEBUG_LOOP
    watchDog.loop();
#endif
}

enum ButtonState { BTN_PRESSED, BTN_RELEASED };
uint8_t volatile powerBtnLongPressCounter;
unsigned long volatile power_btn_last_event = 0;
ButtonState volatile power_btn_state = BTN_RELEASED;
bool volatile powerButtonClicked = false;
static void ICACHE_RAM_ATTR power_button_state_change() {
    unsigned long now = millis();
    if (now - power_btn_last_event < 50) return;

    if (!digitalRead(POWER_BTN_PIN) && power_btn_state != BTN_PRESSED) {
        power_btn_last_event = now;
        power_btn_state = BTN_PRESSED;
    }
    if (digitalRead(POWER_BTN_PIN) && power_btn_state != BTN_RELEASED) {
        power_btn_last_event = now;
        power_btn_state = BTN_RELEASED;
        if (millis_passed(power_btn_last_event, now) < 1000)
            powerButtonClicked = true;
    }
}

unsigned long powerButtonUpdated = 0;
void power_button_handler() {
    unsigned long now = millis();

    if (millis_passed(powerButtonUpdated, now) >= (ONE_SECOND_ms / 2)) {
        if (powerButtonClicked) {
            psu->togglePower();
            refresh_wifi_led();
            powerBtnLongPressCounter = 0;
            powerButtonClicked = false;
        }
    }

    if (millis_passed(powerButtonUpdated, now) >= ONE_SECOND_ms) {
        if ((power_btn_state == BTN_PRESSED) && !digitalRead(POWER_BTN_PIN)) {
            if (++powerBtnLongPressCounter >= 5) {
                config->reset();
                setup_restart_timer();
            }
            if (powerBtnLongPressCounter == 3) {
                wifi_led->set(Led::BLINK_ERROR);
            }
        } else {
            powerBtnLongPressCounter = 0;
        }
        powerButtonUpdated = now;
    }
}