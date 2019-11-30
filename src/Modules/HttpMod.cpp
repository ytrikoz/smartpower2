#include "Modules/HttpMod.h"

#include "App.h"
#include "AppUtils.h"
#include "Wireless.h"

bool HttpMod::onInit() {
    memset(clients_, 0, sizeof(WebClient) * MAX_WEB_CLIENTS);
    http_ = new WebService(HTTP_PORT, WEBSOCKET_PORT);
    http_->setRoot(FS_WEB_ROOT);
    http_->setOnClientConnection(
        [this](uint8_t num) { this->onHttpClientConnect(num); });
    http_->setOnClientDisconnected(
        [this](uint8_t num) { this->onHttpClientDisconnect(num); });
    http_->setOnClientData([this](uint8_t num, String data) {
        this->onHttpClientData(num, data);
    });
    
    return true;
}

bool HttpMod::onStart() {
    http_->start();
    return true;
}

void HttpMod::onStop() { http_->stop(); }

void HttpMod::onLoop() { http_->loop(); }

uint8_t HttpMod::getClients() {
    uint8_t result = 0;
    for (uint8_t i = 0; i < MAX_WEB_CLIENTS; i++)
        if (clients_[i].connected)
            result++;
    return result;
}

void HttpMod::onHttpClientConnect(uint8_t n) {
    clients_[n].connected = true;
    app.refresh_wifi_led();
}

void HttpMod::onHttpClientDisconnect(uint8_t n) {
    clients_[n].connected = false;
    app.refresh_wifi_led();
}

void HttpMod::onHttpClientData(uint8_t num, String data) {
    switch (data.charAt(0)) {
    case GET_PAGE_STATE: {
        uint8_t page = data.charAt(1) - CHR_ZERO;
        clients_[num].page = page;
        sendPageState(num, page);
        break;
    }
    case SET_POWER_ON_OFF: {
        PsuState state = PsuState(data.substring(1).toInt());
        Psu *psu = app.psu();
        if (!psu->checkState(state))
            psu->togglePower();
        String payload = String(SET_POWER_ON_OFF);
        payload += psu->getState();
        sendToClients(payload, PG_HOME, num);
    } break;

    case SET_VOLTAGE: {
        float value = data.substring(1).toFloat();
        app.psu()->setVoltage(value);
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
            app.params()->setValueString(items[index++],
                                         data.substring(last, pos).c_str());
            last = pos + 1;
        }
        app.config()->saveConfig();
        break;
    }
    case SET_LOG_WATTHOURS: {
        char ch = data.charAt(1);
        if (isdigit(ch)) {
            bool mode = (uint8_t)ch - CHR_ZERO;
            if (!app.psu()->enableWhStore(mode)) {
                // To all
                sendToClients(data, PG_HOME);
            } else {
                // Except sender
                sendToClients(data, PG_HOME, num);
            }
            app.params()->setValueBool(WH_STORE_ENABLED, mode);
            app.config()->saveConfig();
        }
        break;
    }
    }
}

void HttpMod::sendToClients(String payload, uint8_t page, uint8_t except_n) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients_[i].connected && (clients_[i].page == page) &&
            (except_n != i))
            http_->sendTxt(i, payload);
}

void HttpMod::sendToClients(String payload, uint8_t page) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients_[i].connected && clients_[i].page == page)
            http_->sendTxt(i, payload);
}

void HttpMod::sendPageState(uint8_t page) {
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients_[i].connected && clients_[i].page == page)
            sendPageState(i, page);
}

void HttpMod::sendPageState(uint8_t n, uint8_t page) {
    switch (page) {
    case PG_HOME: {
        String power(SET_POWER_ON_OFF);
        power += String(app.psu()->getState());
        http_->sendTxt(n, power);

        String wh_store(SET_LOG_WATTHOURS);
        wh_store += String(app.psu()->isWhStoreEnabled());
        http_->sendTxt(n, wh_store);
        break;
    }
    case PG_SETTINGS: {
        String bootpower;
        bootpower = String(SET_BOOT_POWER_MODE);
        bootpower += app.params()->getValueAsByte(POWER);
        http_->sendTxt(n, bootpower);

        String voltage = String(SET_VOLTAGE);
        voltage += String(app.psu()->getVoltage(), 2);
        http_->sendTxt(n, voltage);

        String network = String(SET_NETWORK);
        AppUtils::getNetworkConfig(app.params());
        http_->sendTxt(n, network);
        break;
    }
    case PG_STATUS: {
        String payload;
        // Version info
        payload = String(TAG_FIRMWARE_INFO);
        payload += SysInfo::getVersionJson();
        http_->sendTxt(n, payload);
        // System info
        payload = String(TAG_SYSTEM_INFO);
        payload += SysInfo::getSystemJson();
        http_->sendTxt(n, payload);
        // Network info
        payload = String(TAG_NETWORK_INFO);
        payload += SysInfo::getNetworkJson();
        http_->sendTxt(n, payload);
        break;
    }
    }
}