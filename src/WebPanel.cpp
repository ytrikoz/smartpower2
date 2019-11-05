#include "WebPanel.h"

#include "App.h"
#include "AppUtils.h"
#include "Wireless.h"

namespace WebPanel {
WebClient clients[MAX_WEB_CLIENTS];

void init() { memset(clients, 0, sizeof(WebClient) * MAX_WEB_CLIENTS); }

uint8_t get_http_clients_count() {
    uint8_t result = 0;
    for (uint8_t i = 0; i < MAX_WEB_CLIENTS; i++)
        if (clients[i].connected)
            result++;
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

void onHttpClientData(uint8_t num, String data) {
    switch (data.charAt(0)) {
    case GET_PAGE_STATE: {
        uint8_t page = data.charAt(1) - CHR_ZERO;
        clients[num].page = page;
        sendPageState(num, page);
        break;
    }
    case SET_POWER_ON_OFF: {
        PsuState state = PsuState(data.substring(1).toInt());
        Psu *psu = app.getPsu();
        if (!psu->checkState(state))
            psu->togglePower();
        String payload = String(SET_POWER_ON_OFF);
        payload += psu->getState();
        sendToClients(payload, PG_HOME, num);
    } break;

    case SET_VOLTAGE: {
        float value = data.substring(1).toFloat();
        app.getPsu()->setVoltage(value);
        sendToClients(String(SET_VOLTAGE) + data.substring(1), PG_HOME, num);
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
                          PG_SETTINGS, num);
        }
        break;
    }
    case SET_NETWORK: {
        static const ConfigItem items[] = {WIFI,   SSID,    PASSWORD, DHCP,
                                           IPADDR, NETMASK, GATEWAY,  DNS};
        static const size_t paramCount = sizeof(items) / sizeof(ConfigItem);
        size_t index = 0;
        size_t last = 0;
        size_t pos = 0;
        while (index < paramCount && (pos = data.indexOf(",", last))) {
            app.getConfig()->setValueString(items[index++],
                                            data.substring(last, pos).c_str());
            last = pos + 1;
        }
        app.getEnv()->saveConfig();
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
                sendToClients(data, PG_HOME, num);
            }
            app.getConfig()->setValueBool(WH_STORE_ENABLED, mode);
            app.getEnv()->saveConfig();
        }
        break;
    }
    }
}

void sendToClients(String payload, uint8_t page, uint8_t except_n) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected && (clients[i].page == page) &&
            (except_n != i))
            app.getHttp()->sendTxt(i, payload);
}

void sendToClients(String payload, uint8_t page) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected && clients[i].page == page)
            app.getHttp()->sendTxt(i, payload);
}

void sendPageState(uint8_t page) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected && clients[i].page == page)
            sendPageState(i, page);
}

void sendPageState(uint8_t n, uint8_t page) {
    switch (page) {
    case PG_HOME: {
        String power(SET_POWER_ON_OFF);
        power += String(app.getPsu()->getState());
        app.getHttp()->sendTxt(n, power);

        String wh_store(SET_LOG_WATTHOURS);
        wh_store += String(app.getPsu()->isWhStoreEnabled());
        app.getHttp()->sendTxt(n, wh_store);
        break;
    }
    case PG_SETTINGS: {
        String bootpower;
        bootpower = String(SET_BOOT_POWER_MODE);
        bootpower += app.getConfig()->getValueAsByte(POWER);
        app.getHttp()->sendTxt(n, bootpower);

        String voltage = String(SET_VOLTAGE);
        voltage += String(app.getPsu()->getVoltage(), 2);
        app.getHttp()->sendTxt(n, voltage);

        String network = String(SET_NETWORK);
        AppUtils::getNetworkConfig(app.getConfig());
        app.getHttp()->sendTxt(n, network);
        break;
    }
    case PG_STATUS: {
        String payload;
        // Version info
        payload = String(TAG_FIRMWARE_INFO);
        payload += SysInfo::getVersionJson();
        app.getHttp()->sendTxt(n, payload);
        // System info
        payload = String(TAG_SYSTEM_INFO);
        payload += SysInfo::getSystemJson();
        app.getHttp()->sendTxt(n, payload);
        // Network info
        payload = String(TAG_NETWORK_INFO);
        payload += SysInfo::getNetworkJson();
        app.getHttp()->sendTxt(n, payload);
        break;
    }
    }
} // namespace WebPanel
} // namespace WebPanel