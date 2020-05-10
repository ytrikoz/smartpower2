#include "Modules/WebUI.h"

#include "Global.h"
#include "Modules/Psu.h"

#define JSON_BUFFER_SIZE 1024

namespace Modules {

WebUI::WebUI() : Module(), client_cnt_{0} {
    memset(&client_, 0, sizeof(WebClient) * WEB_SERVER_CLIENT_MAX);
    sprintf(last_modified_, "%s %s GMT", __DATE__, __TIME__);
}

bool WebUI::onInit() {
    web_ = new WebServerAsync(HTTP_PORT);
    web_->setHandler(this);
    return true;
}

bool WebUI::onStart() {
    web_->start();
    return true;
}

void WebUI::onStop() {
    web_->stop();
}

void WebUI::onLoop() {
    web_->loop();
}

size_t WebUI::getClients() {
    return client_cnt_;
}

bool WebUI::getSlot(WebClient **c) {
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

bool WebUI::getClient(uint32_t num, WebClient **c) {
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

void WebUI::setClientPage(const uint32_t num, const WebPageEnum page) {
    WebClient* c;
    if (getClient(num, &c)) {
        c->connected = true;
        c->page = page;
    }
}

void WebUI::onConnection(const uint32_t num, const bool connected) {
    WebClient *c;
    if (connected) {
        if (getSlot(&c)) {
            c->num = num;
            c->connected = true;
            client_cnt_++;
        }        
    } else {
        if (getClient(num, &c)) {
            c->connected = false;
            client_cnt_--;
        }
    }
    app.onWebStatusChange(client_cnt_);
}

void WebUI::onData(const uint32_t num, const String &data) {
    DynamicJsonDocument doc(1024);
    DeserializationError jsonError = deserializeJson(doc, data);
    PrintUtils::print_ident(out_, FPSTR(str_web));
    PrintUtils::println(out_, data);
    if (jsonError) {
        setSerializeError(jsonError.c_str());
        return;
    }
    for (JsonPair item : doc.as<JsonObject>()) {
        const char *cmd = item.key().c_str();
        // page
        if (strcmp_P(cmd, str_page) == 0) {
            WebPageEnum page = WebPageEnum(item.value().as<uint>());
            String data = getPageData(page);
            setClientPage(num, page);
            send(data, page, num);
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
        else if (strcmp_P(cmd, str_wh) == 0) {
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
            setError(UNKNOWN_OBJECT, cmd);
            return;
        }
        setError(Error::none());
    }
};

bool WebUI::exists(const String &uri, String &lastModified) {
    lastModified = last_modified_;
    return uri.endsWith("version") || uri.endsWith("system") || uri.endsWith("network");
}

String WebUI::getPageData(const WebPageEnum page) {
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
        setError(ERROR_SIZE, String(size).c_str());
    }
    return json;
}

void WebUI::updatePage(const WebPageEnum page) {
    String data = getPageData(page);
    broadcast(data, page);
}

void WebUI::send(const String &data, const WebPageEnum page, const uint32_t num) {
    WebClient *c;
    if (getClient(num, &c)) {  
        if (c->connected && c->page == page) web_->sendData(c->num, data);    
    }
}

void WebUI::broadcast(const String &data, const WebPageEnum page, const uint32_t except_num) {
    for (uint8_t i = 0; i < WEB_SERVER_CLIENT_MAX; ++i) {
        WebClient *c = &client_[i];
        if (c->num != except_num && c->connected && c->page == page) {
            web_->sendData(c->num, data);
        }
    }
}

void WebUI::fillOptions(JsonObject &obj) {
    Config *cfg = config->get();
    for (uint8_t i = 0; i < PARAMS_COUNT; ++i) {
        ConfigItem param = ConfigItem(i);
        if (!config->isSecured(param)) {
            obj[cfg->name(param)] = cfg->value(param);
        }
    }
}

void WebUI::fillMain(JsonObject &obj) {
    Modules::Psu *ps = app.psu();
    Config *cfg = config->get();
    obj[FPSTR(str_power)] = (int) ps->getState();
    obj[FPSTR(str_wh)] = ps->getData()->Wh;
    obj[FPSTR(str_voltage)] = cfg->value(OUTPUT_VOLTAGE);
}

void WebUI::fillVersion(JsonObject &obj) {
    obj[FPSTR(str_fw)] = SysInfo::getFW();
    obj[FPSTR(str_sdk)] = SysInfo::getSDK();
    obj[FPSTR(str_core)] = SysInfo::getCore().c_str();
}

void WebUI::fillNetwork(JsonObject &obj) {
    obj[FPSTR(str_phy)] = NetUtils::getWifiPhy().c_str();
    obj[FPSTR(str_ch)] = NetUtils::getWifiCh();
    NetInfo info;
    WiFiMode_t mode = WiFi.getMode();
    if (mode == WIFI_AP || mode == WIFI_AP_STA) {
        info = NetUtils::getNetInfo(0);
        obj[FPSTR(str_ssid)] = NetUtils::getApSsid().c_str();
        obj[FPSTR(str_ip)] = info.ip.toString().c_str();
        obj[FPSTR(str_subnet)] = info.subnet.toString().c_str();
        obj[FPSTR(str_gateway)] = info.gateway.toString().c_str();
        obj[FPSTR(str_mac)] = NetUtils::getApMac().c_str();
    }
    if (mode == WIFI_STA || mode == WIFI_AP_STA) {
        info = NetUtils::getNetInfo(1);
        obj[FPSTR(str_ssid)] = NetUtils::getStaSsid().c_str();
        obj[FPSTR(str_mac)] = WiFi.macAddress();
        obj[FPSTR(str_host)] = WiFi.hostname();
        obj[FPSTR(str_ip)] = info.ip.toString().c_str();
        obj[FPSTR(str_subnet)] = info.subnet.toString().c_str();
        obj[FPSTR(str_gateway)] = info.gateway.toString().c_str();
        obj[FPSTR(str_dns)] = WiFi.dnsIP().toString().c_str();
    }
}

void WebUI::fillSystem(JsonObject &obj) {
    obj[FPSTR(str_cpu)] = SysInfo::getCpuFreq().c_str();
    obj[FPSTR(str_heap)] == SysInfo::getHeapStats().c_str();
    obj[FPSTR(str_chip)] = SysInfo::getChipId().c_str();
    obj[FPSTR(str_file)] = FSUtils::getFSUsed().c_str();
}

bool WebUI::getResponse(const String &uri, String &body) {
    DynamicJsonDocument doc(1024);
    JsonObject root = doc.createNestedObject();
    if (uri.endsWith(FPSTR(str_version)))
        fillVersion(root);
    else if (uri.endsWith(FPSTR(str_network)))
        fillNetwork(root);
    else if (uri.endsWith(FPSTR(str_system)))
        fillSystem(root);
    serializeJson(doc, body);
    return true;
}

}