#include "Modules/Web.h"

#include "Global.h"
#include "Modules/Psu.h"

namespace Modules {

Web::Web() : Module() {
    memset(session_, 0, sizeof(WebClient) * WEB_SERVER_CLIENT_MAX);
}

bool Web::onInit() {
    web_ = new WebServerAsync(HTTP_PORT);
    web_->setOnConnection(
        [this](uint8_t num) { this->onConnection(num); });
    web_->setOnDisconnection(
        [this](uint8_t num) { this->onDisconnection(num); });
    web_->setOnReceiveData([this](uint8_t num, String data) {
        this->onData(num, data);
    });

    return true;
}

bool Web::onStart() {
    web_->start();
    return true;
}

void Web::onStop() { web_->stop(); }

void Web::onLoop() { web_->loop(); }

uint8_t Web::getClients() {
    uint8_t result = 0;
    for (uint8_t i = 0; i < WEB_SERVER_CLIENT_MAX; i++)
        if (session_[i].connected)
            result++;
    return result;
}

void Web::onConnection(uint8_t n) {
    session_[n].connected = true;
    app.onWebStatusChange(true);
}

void Web::onDisconnection(uint8_t n) {
    session_[n].connected = false;
    app.onWebStatusChange(getClients());
}

void Web::onData(uint8_t num, const String &data) {
    switch (data.charAt(0)) {
        case GET_PAGE_STATE: {
            uint8_t page = data.charAt(1) - CHR_ZERO;
            session_[num].page = page;
            sendPageState(num, page);
            break;
        }
        case SET_POWER_ON_OFF: {
            PsuState state = PsuState(data.substring(1).toInt());
            Modules::Psu *psu = app.psu();
            if (state == POWER_ON)
                psu->powerOn();
            else
                psu->powerOff();
            String payload = String(SET_POWER_ON_OFF);
            payload += state;
            sendToClients(payload, PG_HOME, num);
            break;
        }
        case SET_VOLTAGE: {
            float value = data.substring(1).toFloat();
            app.setOutputVoltage(value);
            sendToClients(data, PG_SETTINGS, num);
            break;
        }
        case SET_DEFAULT_VOLTAGE: {
            app.setOutputVoltageAsDefault();
            break;
        }
        case SET_BOOT_POWER_MODE: {
            BootPowerState state = BootPowerState(data.substring(1).toInt());
            if (app.setBootPowerState(state)) {
                String str = String(SET_BOOT_POWER_MODE) + String(state);
                sendToClients(str, PG_SETTINGS, num);
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
                app.params()->setValue(items[index++],
                                       data.substring(last, pos).c_str());
                last = pos + 1;
            }
            config->save();
            break;
        }
        case SET_LOG_WATTHOURS: {
            bool mode = data.substring(1).toInt();
            config->get()->setValueBool(WH_STORE_ENABLED, mode);
            config->save();
            sendToClients(data, PG_HOME, num);
            break;
        }
    }
}  // namespace Modules

void Web::sendToClients(const String &payload, uint8_t page, uint8_t except) {
    for (uint8_t i = 0; i < WEB_SERVER_CLIENT_MAX; ++i)
        if (session_[i].connected && (session_[i].page == page) && (i != except))
            web_->sendData(i, payload);
}

void Web::sendToClients(const String &payload, uint8_t page) {
    for (uint8_t i = 0; i < WEB_SERVER_CLIENT_MAX; ++i)
        if (session_[i].connected && session_[i].page == page)
            web_->sendData(i, payload);
}

void Web::sendPageState(uint8_t page) {
    for (uint8_t i = 0; i < WEB_SERVER_CLIENT_MAX; ++i)
        if (session_[i].connected && session_[i].page == page)
            sendPageState(i, page);
}

void Web::sendPageState(uint8_t client, uint8_t page) {
    switch (page) {
        case PG_HOME: {
            DynamicJsonDocument doc(512);
            // Switch
            doc[FPSTR(str_switch)] = app.psu()->isPowerOn();
            // Data
            doc["V"] = app.psu()->getInfo().V;
            doc["I"] = app.psu()->getInfo().I;
            doc["P"] = app.psu()->getInfo().P;
            // Total    
            doc[FPSTR(str_total)] = app.psu()->isWhStoreEnabled();            
            doc["Wh"] = app.psu()->getInfo().Wh;        
            String json;
            serializeJson(doc, json);
            json = TAG_MAIN_PAGE + json;
            web_->sendData(client, json);
            break;
        }
        case PG_SETTINGS: {
            DynamicJsonDocument doc(512);
            doc[FPSTR(str_boot)] = config_->getValueAsByte(POWER);
            doc[FPSTR(str_voltage)] = config_->getValueAsFloat(OUTPUT_VOLTAGE);
            doc[FPSTR(str_wifi)] = config_->getValueAsByte(WIFI);
            doc[FPSTR(str_ssid)] = config_->getValue(SSID);
            doc[FPSTR(str_password)] = config_->getValue(PASSWORD);
            doc[FPSTR(str_dhcp)] = config_->getValueAsBool(DHCP);
            doc[FPSTR(str_ipaddr)] = config_->getValue(IPADDR);
            doc[FPSTR(str_netmask)] = config_->getValue(NETMASK);
            doc[FPSTR(str_dns)] = config_->getValue(DNS);
            String json;
            serializeJson(doc, json);
            json = TAG_OPTIONS_PAGE + json;
            web_->sendData(client, json);
            break;
        }
        case PG_STATUS: {
            String payload;

            payload = String(TAG_FIRMWARE_INFO);
            payload += SysInfo::getVersionJson();
            web_->sendData(client, payload);

            payload = String(TAG_SYSTEM_INFO);
            payload += SysInfo::getSystemJson();
            web_->sendData(client, payload);

            payload = String(TAG_NETWORK_INFO);
            payload += SysInfo::getNetworkJson();
            web_->sendData(client, payload);
            break;
        }
    }
}

}  // namespace Modules
