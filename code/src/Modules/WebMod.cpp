#include "Modules/WebMod.h"

#include "App.h"
#include "AppUtils.h"
#include "Wireless.h"
#include "StoreUtils.h"
#include "WebServer/WebServerAsync.h"

bool WebMod::onInit() {
    memset(clients_, 0, sizeof(WebClient) * WEB_SERVER_CLIENT_MAX);
    web_ = new WebServerAsync(HTTP_PORT);
    web_->setOnConnection(
        [this](uint8_t num) { this->onHttpClientConnect(num); });
    web_->setOnDisconnection(
        [this](uint8_t num) { this->onHttpClientDisconnect(num); });
    web_->setOnReceiveData([this](uint8_t num, String data) {
        this->onHttpClientData(num, data);
    });

    return true;
}

bool WebMod::onStart() {
    web_->start();
    return true;
}

void WebMod::onStop() { web_->stop(); }

void WebMod::onLoop() { web_->loop(); }

uint8_t WebMod::getClients() {
    uint8_t result = 0;
    for (uint8_t i = 0; i < WEB_SERVER_CLIENT_MAX; i++)
        if (clients_[i].connected)
            result++;
    return result;
}

void WebMod::onHttpClientConnect(uint8_t n) {
    clients_[n].connected = true;
    app.refresh_wifi_led();
}

void WebMod::onHttpClientDisconnect(uint8_t n) {
    clients_[n].connected = false;
    app.refresh_wifi_led();
}

void WebMod::onHttpClientData(uint8_t num, String data) {
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
                String str = SET_BOOT_POWER_MODE + data.substring(1);
                sendToClients(str,
                              PG_SETTINGS, num);
            }
            break;
        }
        case SET_NETWORK: {
            static const ConfigItem items[] = {WIFI, SSID, PASSWORD, DHCP,
                                               IPADDR, NETMASK, GATEWAY, DNS};
            static const size_t paramCount = sizeof(items) / sizeof(ConfigItem);
            size_t index = 0;
            size_t last = 0;
            size_t pos = 0;
            while (index < paramCount && (pos = data.indexOf(",", last))) {
                app.params()->setValueAsString(items[index++],
                                               data.substring(last, pos).c_str());
                last = pos + 1;
            }
            app.config()->save();
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
                app.config()->save();
            }
            break;
        }
    }
}

void WebMod::sendToClients(String &payload, const uint8_t page, const uint8_t except_n) {
    for (uint8_t i = 0; i < WEB_SERVER_CLIENT_MAX; ++i)
        if (clients_[i].connected && (clients_[i].page == page) &&
            (except_n != i))
            web_->sendData(i, payload);
}

void WebMod::sendToClients(String &payload, const uint8_t page) {
    for (uint8_t i = 0; i < WEB_SERVER_CLIENT_MAX; ++i)
        if (clients_[i].connected && clients_[i].page == page)
            web_->sendData(i, payload);
}

void WebMod::sendPageState(const uint8_t page) {
    for (uint8_t i = 0; i < WEB_SERVER_CLIENT_MAX; ++i)
        if (clients_[i].connected && clients_[i].page == page)
            sendPageState(i, page);
}

void WebMod::sendPageState(uint8_t n, uint8_t page) {
    switch (page) {
        case PG_HOME: {
            String power(SET_POWER_ON_OFF);
            power += String(app.psu()->getState(), DEC);
            web_->sendData(n, power);

            String wh_store(SET_LOG_WATTHOURS);
            wh_store += String(app.psu()->isWhStoreEnabled(), DEC);
            web_->sendData(n, wh_store);
            break;
        }
        case PG_SETTINGS: {
            String bootpower = String(SET_BOOT_POWER_MODE);
            bootpower += String(app.params()->getValueAsByte(POWER), DEC);
            web_->sendData(n, bootpower);

            String voltage = String(SET_VOLTAGE);
            voltage += String(app.psu()->getVoltage(), 2);
            web_->sendData(n, voltage);

            String network = String(SET_NETWORK);
            network += AppUtils::getNetworkConfig(app.params());
            web_->sendData(n, network);
            break;
        }
        case PG_STATUS: {
            String payload;

            payload = String(TAG_FIRMWARE_INFO);
            payload += SysInfo::getVersionJson();
            web_->sendData(n, payload);

            payload = String(TAG_SYSTEM_INFO);
            payload += SysInfo::getSystemJson();
            web_->sendData(n, payload);

            payload = String(TAG_NETWORK_INFO);
            payload += SysInfo::getNetworkJson();
            web_->sendData(n, payload);
            break;
        }
    }
}