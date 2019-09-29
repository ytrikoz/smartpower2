#include "main.h"

#include "App.h"
#include "BuildConfig.h"
#include "CommonTypes.h"
#include "Consts.h"
#include "Global.h"
#include "StrUtils.h"
#include "SysInfo.h"
#include "Wireless.h"
#include "PrintUtils.h"

uint8_t get_http_clients_count() {
    uint8_t result = 0;
#ifndef DISABLE_HTTP
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected) result++;
#endif
    return result;
}

void onHttpClientConnect(uint8_t n) {
    clients[n].connected = true;
    app.refresh_wifi_led();
}

void onHttpClientDisconnect(uint8_t n) {
    clients[n].connected = false;
    app.refresh_wifi_led();
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
            if (new_state != app.getPsu()->getState()) {
                app.getPsu()->togglePower();
                String payload = String(SET_POWER_ON_OFF);
                payload += app.getPsu()->getState();
                sendToClients(payload, PG_HOME, n);
            }
            break;
        }
        case SET_VOLTAGE: {
            float _voltage = data.substring(1).toFloat();
            app.getPsu()->setOutputVoltage(_voltage);
            sendToClients(String(SET_VOLTAGE) + data.substring(1), PG_HOME, n);
            break;
        }
        case SET_DEFAULT_VOLTAGE: {
            app.setOutputVoltageAsDefault();
            break;
        }
        case SET_BOOT_POWER_MODE: {
            BootPowerState state = BootPowerState(data.substring(1).toInt());
            if (app.setBootPowerState(state)) {
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
                app.getConfig()->setValueString(
                    items[index++], data.substring(last, pos).c_str());
                last = pos + 1;
            }
            app.saveConfig();
            break;
        }
        case SET_LOG_WATTHOURS: {
            char ch = data.charAt(1);
            if (isdigit(ch)) {
                bool mode = (uint8_t)ch - CHR_ZERO;
                if (!app.getPsu()->enableWhStore(mode)) {
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
            stateStr += String(app.getPsu()->getState());
            http->sendTxt(n, stateStr);
            // WattHour
            String whStoreEnabledStr = String(SET_LOG_WATTHOURS);
            whStoreEnabledStr += String(app.getPsu()->isWhStoreEnabled());
            http->sendTxt(n, whStoreEnabledStr);
            break;
        }
        case PG_SETTINGS: {
            String payload;
            // Power mod
            payload = String(SET_BOOT_POWER_MODE);
            payload += app.getConfig()->getValueAsByte(POWER);
            http->sendTxt(n, payload);
            // Output voltage
            payload = String(SET_VOLTAGE);
            payload += String(app.getPsu()->getOutputVoltage(), 2);
            http->sendTxt(n, payload);
            // Network config
            {
            String payload = app.getNetworkConfig();
          
            http->sendTxt(n, payload);
            break;
            }
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

void setup() {
    // Setup serial
    USE_SERIAL.begin(115200);
    USE_SERIAL.println();

    // Try pushing frequency to 160MHz.
    system_update_cpu_freq(SYS_CPU_160MHZ);

#ifdef SERIAL_DEBUG
    USE_SERIAL.setDebugOutput(true);
#endif

    memset(clients, 0, sizeof(WebClient) * WEBSOCKETS_SERVER_CLIENT_MAX);

    app.init(&USE_SERIAL);

    app.start();
}

void loop() {
    app.loop();    

    timer.run();
}
