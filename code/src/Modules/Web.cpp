#include "Modules/Web.h"

#include "Global.h"
#include "Modules/Psu.h"

#define JSON_BUFFER_SIZE 1024

namespace Modules {

Web::Web() : Module() {
    memset(client_, 0, sizeof(WebClient) * WEB_SERVER_CLIENT_MAX);
    sprintf(last_modified_, "%s %s GMT", __DATE__, __TIME__);
    cnt_ = 0;
}

bool Web::onInit() {
    web_ = new WebServerAsync(HTTP_PORT);
    web_->init(this);
    return true;
}

bool Web::onStart() {
    web_->start();
    return true;
}

void Web::onStop() {
    web_->stop();
}

void Web::onLoop() {
    web_->loop();
}

size_t Web::getClients() {
    return cnt_;
}

bool Web::getFreeSlot(WebClient **c) {
    bool res = false;
    for (uint8_t i = 0; i < WEB_SERVER_CLIENT_MAX; ++i) {
        if (!client_[i].connected) {
            *c = &client_[i];
            res = true;
            break;
        }
    }
    return res;
}

bool Web::getClientByNum(uint32_t num, WebClient **c) {
    bool res = false;
    for (uint8_t i = 0; i < WEB_SERVER_CLIENT_MAX; ++i) {
        if (client_[i].num == num) {
            *c = &client_[i];
            res = true;
            break;
        }
    }
    return res;
}

void Web::onConnection(const uint32_t num, const bool connected) {
    WebClient *c;
    if (connected) {
        if (getFreeSlot(&c)) {
            c->num = num;
            c->connected = true;
            cnt_++;
        }
    } else {
        if (getClientByNum(num, &c)) {
            c->connected = false;
            cnt_--;
        }
    }
    app.onWebStatusChange(cnt_);
}

void Web::onData(const uint32_t num, const String &data) {
    DynamicJsonDocument doc(1024);
    DeserializationError jsonError = deserializeJson(doc, data);
    PrintUtils::print_ident(out_, FPSTR(str_web));
    PrintUtils::println(out_, data);
    if (jsonError) {
        setError(ERROR_JSON, jsonError.c_str());
        return;
    }
    for (JsonPair item : doc.as<JsonObject>()) {
        const char *cmd = item.key().c_str();
        // page
        if (strcmp(cmd, "page") == 0) {
            WebPageEnum page = WebPageEnum(item.value().as<uint>());
            WebClient *c;
            if (getClientByNum(num, &c)) {
                c->page = page;
                sendPage(page, num);
            }
        }
        // config
        else if (strcmp_P(cmd, str_config) == 0) {
            String param = item.value().as<String>().c_str();
            if (param.equalsIgnoreCase("save")) {
                config->save();
            } else if (param.equalsIgnoreCase("default")) {
                config->setDefaultConfig();
            }
        }
        // restart
        else if (strcmp_P(cmd, str_system) == 0) {
            String param = item.value().as<String>().c_str();
            if (param.equalsIgnoreCase("restart")) {
                app.systemRestart();
            }
        }
        // power
        else if (strcmp_P(cmd, str_power) == 0) {
            bool value = item.value().as<bool>();
            if (value)
                app.psu()->powerOn();
            else
                app.psu()->powerOff();
        }
        // wh
        else if (strcmp(cmd, "wh") == 0) {
            double value = item.value().as<double>();
            app.psu()->setWh(value);
        }
        // set
        else if (strcmp_P(cmd, str_set) == 0) {
            for (JsonObject arguments : item.value().as<JsonArray>()) {
                for (JsonPair item : arguments) {
                    config->get()->setByName(item.key().c_str(), item.value().as<String>().c_str());
                }
            }
        } else {
            setError(ERROR_UNSUPPORTED, "unhandled");
            return;
        }
        setError(Error::ok());
    }
};

bool Web::uriExist(const String &uri, String &lastModified) {
    lastModified = last_modified_;
    return true;
}

String Web::getPageData(const WebPageEnum page) {
    DynamicJsonDocument doc(JSON_BUFFER_SIZE);
    JsonObject obj = doc.createNestedObject();
    switch (page) {
        case PAGE_HOME: {
            fillMain(obj);
            break;
        }
        case PAGE_OPTIONS: {
            fillOptions(obj);
            break;
            default:
                break;
        }
    }
    String json;
    size_t size = serializeJson(doc, json);
    if (size > JSON_BUFFER_SIZE) {
        setError(Error::BufferLow(JSON_BUFFER_SIZE));
    }
    return json;
}

// switch (tag) {
//     case TAG_SET_ON_OFF: {
//         PsuState value = PsuState(data.substring(1).toInt());
//         Modules::Psu *psu = app.psu();
//         if (value == POWER_ON)
//             psu->powerOn();
//         else
//             psu->powerOff();
//         String data = "{\"switch\":" + getBoolStr(value) + "}";
//         sendData(data, PAGE_HOME, num);
//         break;
//     }
//     case TAG_SET_VOLTAGE: {
//         float value = data.substring(1).toFloat();
//         app.setOutputVoltage(value);
//         String data = "{\"voltage\":" + String(value, 2) + "}";
//         sendData(data, PAGE_OPTIONS, num);
//         break;
//     }
//     case SET_DEFAULT_VOLTAGE: {
//         config->save();
//         break;
//     }
//     case TAG_SET_STORE_WH_TOTAL: {
//         long value = data.substring(1).toInt();
//         config->get()->setValueBool(WH_STORE_ENABLED, value);
//         String data = "{\"total\":" + getBoolStr(value) + "}";
//         sendData(data, PAGE_HOME, num);
//         break;
//     }
//     case SET_BOOT_POWER_MODE: {
//         BootPowerState value = BootPowerState(data.substring(1).toInt());
//         if (config->get()->setValueByte(POWER, (uint8_t)(value))) {
//             config->save();
//             String data = "{\"boot\":" + String(value, DEC) + "}";
//             sendData(data, PAGE_INFO, num);
//         }
//         break;
//     }
//     case TAG_SET_NETWORK: {
//         static const ConfigItem items[] = {WIFI, SSID, PASSWORD, DHCP,
//                                            IPADDR, NETMASK, GATEWAY, DNS};
//         static const size_t paramCount = sizeof(items) / sizeof(ConfigItem);
//         size_t index = 0;
//         size_t last = 0;
//         size_t pos = 0;
//         while (index < paramCount && (pos = data.indexOf(",", last))) {
//             app.params()->setValue(items[index++],
//                                    data.substring(last, pos).c_str());
//             last = pos + 1;
//         }
//         config->save();
//         break;
//     }
// }

void Web::sendPage(const WebPageEnum page, uint32_t num) {
    String data = getPageData(page);
    if (failed()) {
        PrintUtils::print_ident(out_, FPSTR(str_web));
        PrintUtils::print(out_, getError());
        PrintUtils::println(out_);
    }
    if (data.length()) {
        web_->sendData(num, data);
    }
}

void Web::sendAll(const String &data, WebPageEnum page, uint32_t except_num) {
    for (uint8_t i = 0; i < WEB_SERVER_CLIENT_MAX; ++i) {
        WebClient *c = &client_[i];
        if (c->num != except_num && c->connected && c->page == page)
            web_->sendData(c->num, data);
    }
}

void Web::fillMain(JsonObject &obj) {
    Modules::Psu *ps = app.psu();
    obj[FPSTR(str_power)] = (uint8_t)ps->isPowerOn();
    obj[FPSTR(str_wh)] = ps->getInfo().Wh;
}

void Web::fillOptions(JsonObject &obj) {
    Config *cfg = config->get();
    for (uint8_t i = 0; i < PARAMS_COUNT; ++i) {
        ConfigItem param = ConfigItem(i);
        if (!config->isSecured(param))
            obj[cfg->name(param)] = cfg->value(param);
    }
}

void Web::fillVersion(JsonObject &obj) {
    obj[FPSTR(str_fw)] = SysInfo::getFW();
    obj[FPSTR(str_sdk)] = SysInfo::getSDK();
    obj[FPSTR(str_core)] = SysInfo::getCore();
}

void Web::fillNetwork(JsonObject &obj) {
    obj[FPSTR(str_phy)] = WirelessUtils::getWifiPhy();
    obj[FPSTR(str_ch)] = WirelessUtils::getWifiCh();

    NetInfo info;
    WiFiMode_t mode = WiFi.getMode();
    if (mode == WIFI_AP || mode == WIFI_AP_STA) {
        JsonObject ap = obj.createNestedObject(FPSTR(str_ap));
        ap[FPSTR(str_ssid)] = WirelessUtils::getApSsid();
        info = WirelessUtils::getApNetInfo();
        ap[FPSTR(str_ip)] = info.ip.toString();
        ap[FPSTR(str_subnet)] = info.subnet.toString();
        ap[FPSTR(str_gateway)] = info.gateway.toString();
        ap[FPSTR(str_mac)] = WirelessUtils::getApMac();
    }
    if (mode == WIFI_STA || mode == WIFI_AP_STA) {
        JsonObject sta = obj.createNestedObject(FPSTR(str_sta));
        info = WirelessUtils::getStaNetInfo();
        sta[FPSTR(str_ssid)] = WirelessUtils::getStaSsid();
        sta[FPSTR(str_mac)] = WiFi.macAddress();
        sta[FPSTR(str_host)] = WiFi.hostname();
        sta[FPSTR(str_ip)] = info.ip.toString();
        sta[FPSTR(str_subnet)] = info.subnet.toString();
        sta[FPSTR(str_gateway)] = info.gateway.toString();
        sta[FPSTR(str_dns)] = WiFi.dnsIP().toString();
    }
}

void Web::fillSystem(JsonObject &obj) {
    obj[FPSTR(str_cpu)] = SysInfo::getCpuFreq();
    obj[FPSTR(str_chip)] = SysInfo::getChipId();
    obj[FPSTR(str_file)] = FSUtils::getFSStats();
}

bool Web::getResponse(const String &uri, String &body) {
    DynamicJsonDocument doc(1024);
    JsonObject obj = doc.createNestedObject();
    if (uri.endsWith("version"))
        fillVersion(obj);
    else if (uri.endsWith("network"))
        fillNetwork(obj);
    else if (uri.endsWith("system"))
        fillSystem(obj);
    serializeJson(doc, body);
    return true;
}

}  // namespace Modules
