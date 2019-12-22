#include "WebServerAsync.h"

#include "WebJson.h"
#include "Wireless.h"
#include "Core/CharBuffer.h"

WebServerAsync::WebServerAsync(uint16_t port) {
    sprintf(last_modified, "%s %s GMT", __DATE__, __TIME__);

    web_ = new AsyncWebServer(port);
    //web_->rewrite("/", "/index.html");
    
    web_->serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");
    web_->serveStatic("/version.json", SPIFFS, FS_VERSION_JSON);
    web_->rewrite("/system.json", "/system.json");
    web_->rewrite("/main.json", "/main.json");
    
    web_->on("/system.json", HTTP_GET, [this](AsyncWebServerRequest *request) {
        DynamicJsonDocument doc(64);
        doc[FPSTR(str_ipaddr)] = Wireless::hostIP().toString();
        String json;
        serializeJson(doc, json);
        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", json);
        request->send(response);
    });

    web_->on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *request) {
        if (request->header("If-Modified-Since").equals(last_modified)) {
            request->send(304);
        } else {
            // AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", (uint8_t *)index_html_gz, index_html_gz_len);
            // response->addHeader("Content-Encoding", "gzip");
            // response->addHeader("Last-Modified", last_modified);
            // request->send(response);
        };
    });

    web_->onNotFound([this](AsyncWebServerRequest *request) {
        onNotFound(request);
    });

    ws_ = new AsyncWebSocket("/ws");
    ws_->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        onWSEvent(server, client, type, arg, data, len);
    });
    web_->addHandler(ws_);
}

void WebServerAsync::sendEvent(const String &event, const String &content) {
    //events_->send(event.c_str(), content.c_str(), millis());
}

bool WebServerAsync::start() {
    WebJson::updateVersionJson();
    web_->begin();
    return true;
}

void WebServerAsync::stop() {
    web_->end();
}

void WebServerAsync::loop() {
    ws_->cleanupClients();
}

void WebServerAsync::onNotFound(AsyncWebServerRequest *r) {
    r->send(404);
};

void WebServerAsync::onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
        case WS_EVT_DISCONNECT:
            if (connectionHandler) 
                connectionHandler(client->id(), type == WS_EVT_CONNECT);
            break;        
        case WS_EVT_DATA: {
            AwsFrameInfo *info = (AwsFrameInfo *)arg;
            if (info->final && info->index == 0 && info->len == len) {
                if (info->opcode == WS_TEXT) {
                    if (dataHandler) {
                        char* buf = new char[INPUT_MAX_LENGTH];
                        size_t len = info->len > INPUT_MAX_LENGTH? INPUT_MAX_LENGTH: info->len;
                        strncpy(buf, (char*)data, len);
                        buf[len] = '\x00';
                        dataHandler(client->id(), buf);
                        delete buf;
                    }
                    return;
                }
            }
            break;
        }
        default:
            return;
    }
}

void WebServerAsync::sendData(const uint32_t num, const String &payload) {
    ws_->text(num, payload);
}

void WebServerAsync::setOnConnection(WebClientConnectionEventHandler h) {
    connectionHandler = h; 
}

void WebServerAsync::setOnData(WebClientDataEventHandler h) { 
    dataHandler = h; 
}
