#include "WebServerAsync.h"
#include "Wireless.h"
#include "Core/CharBuffer.h"

WebServerAsync::WebServerAsync(uint16_t port) {    
    web_ = new AsyncWebServer(port);
    web_->serveStatic("/", LittleFS, FS_WEB_ROOT).setDefaultFile("index.html");

    web_->onNotFound([this](AsyncWebServerRequest *request) {
        String lastModified;        
        String url = request->url();
        if (!handler_->uriExist(url, lastModified)) {
            request->send(404);  
            return;
        }        
        if (request->header("If-Modified-Since").equals(lastModified)) {
            request->send(304);
            return;
        }
        String responseBody;
        if (handler_->getResponse(url, responseBody)) {
            if (responseBody) {
                AsyncWebServerResponse *response = request->beginResponse(200, "application/json", responseBody);
                response->addHeader("Last-Modified", lastModified);
                request->send(response);
                return;
            }
        }
        request->send(504);         
    });

    ws_ = new AsyncWebSocket("/ws");
    ws_->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        onWSEvent(server, client, type, arg, data, len);
    });
    web_->addHandler(ws_);

    //web_->rewrite("/", "/index.html");
    // web_->on("/system.json", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //     DynamicJsonDocument doc(64);
    //     doc[FPSTR(str_ipaddr)] = Wireless::hostIP().toString();
    //     String json;
    //     serializeJson(doc, json);
    //     AsyncWebServerResponse *response = request->beginResponse(200, "application/json", json);
    //     request->send(response);
    // });
    // web_->on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //     if (request->header("If-Modified-Since").equals(last_modified)) {
    //         request->send(304);
    //     } else {
    //         AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", (uint8_t *)index_html_gz, index_html_gz_len);
    //         response->addHeader("Content-Encoding", "gzip");
    //         response->addHeader("Last-Modified", last_modified);
    //         request->send(response);
    //     };
    // });
}

void WebServerAsync::sendEvent(const String &event, const String &content) {
    //events_->send(event.c_str(), content.c_str(), millis());
}

bool WebServerAsync::start() {
    web_->begin();
    return true;
}

void WebServerAsync::stop() {
    web_->end();
}

void WebServerAsync::loop() {
    ws_->cleanupClients();
}

void WebServerAsync::onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
        case WS_EVT_DISCONNECT:
            handler_->onConnection(client->id(), type == WS_EVT_CONNECT);
            break;
        case WS_EVT_DATA: {
            AwsFrameInfo *info = (AwsFrameInfo *)arg;
            if (info->final && info->index == 0 && info->len == len) {
                if (info->opcode == WS_TEXT) {
                    char *buf = new char[info->len + 1];
                    strncpy(buf, (char *)data, info->len);
                    buf[info->len] = '\x00';
                    handler_->onData(client->id(), buf);
                    delete buf;
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

