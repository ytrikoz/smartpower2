#include "WebServerAsync.h"

#include <ArduinoJson.h>

#include "AppUtils.h"
#include "Wireless.h"

using namespace AppUtils;
using namespace PrintUtils;
using namespace StoreUtils;
using namespace StrUtils;

WebServerAsync::WebServerAsync(uint16_t port) {
    sprintf(last_modified, "%s %s GMT", __DATE__, __TIME__);

    web_ = new AsyncWebServer(port);
    web_->rewrite("/settings.json", "/settings.json");
    web_->rewrite("/", "/index.html");
    
    web_->on("/settings.json", HTTP_GET, [this](AsyncWebServerRequest *request) {    
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
            AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", (uint8_t*) index_html_gz, index_html_gz_len);
            response->addHeader("Content-Encoding", "gzip");
            response->addHeader("Last-Modified", last_modified);
            request->send(response);
        };
    });

    // web_->serveStatic("/", SPIFFS, FS_WEB_ROOT).setDefaultFile("index.html");
    // web_->serveStatic("/js", SPIFFS, FS_WEB_ROOT "js/");

    web_->onFileUpload([](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (!index)
            Serial.printf("UploadStart: %s\n", filename.c_str());
        Serial.printf("%s", (const char *)data);
        if (final)
            Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
    });

    web_->onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        if (!index)
            Serial.printf("BodyStart: %u\n", total);
        Serial.printf("%s", (const char *)data);
        if (index + len == total)
            Serial.printf("BodyEnd: %u\n", total);
    });

    web_->onNotFound([this](AsyncWebServerRequest *request) {
        onNotFound(request);
    });

    web_->onFileUpload([](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (!index)
            Serial.printf("UploadStart: %s\n", filename.c_str());
        Serial.printf("%s", (const char *)data);
        if (final)
            Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
    });

    web_->onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        if (!index)
            Serial.printf("BodyStart: %u\n", total);
        Serial.printf("%s", (const char *)data);
        if (index + len == total)
            Serial.printf("BodyEnd: %u\n", total);
    });

    ws_ = new AsyncWebSocket("/ws");
    ws_->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        onWSEvent(server, client, type, arg, data, len);
    });

    web_->addHandler(ws_);
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

void WebServerAsync::sendData(const uint8_t num, const String &payload) {
    ws_->text(num, payload);
}

void WebServerAsync::setOnConnection(WebConnectionEventHandler h) { onConnectionEvent = h; }

void WebServerAsync::setOnDisconnection(WebConnectionEventHandler h) { onDisconnectionEvent = h; }

void WebServerAsync::setOnReceiveData(WebDataEventHendler h) { onReceiveDataEvent = h; }

void WebServerAsync::onNotFound(AsyncWebServerRequest *r) {
    Serial.printf("NOT_FOUND: ");
    if (r->method() == HTTP_GET)
        Serial.printf("GET");
    else if (r->method() == HTTP_POST)
        Serial.printf("POST");
    else if (r->method() == HTTP_DELETE)
        Serial.printf("DELETE");
    else if (r->method() == HTTP_PUT)
        Serial.printf("PUT");
    else if (r->method() == HTTP_PATCH)
        Serial.printf("PATCH");
    else if (r->method() == HTTP_HEAD)
        Serial.printf("HEAD");
    else if (r->method() == HTTP_OPTIONS)
        Serial.printf("OPTIONS");
    else
        Serial.printf("UNKNOWN");

    Serial.printf(" http://%s%s\n", r->host().c_str(), r->url().c_str());

    if (r->contentLength()) {
        Serial.printf("_CONTENT_TYPE: %s\n", r->contentType().c_str());
        Serial.printf("_CONTENT_LENGTH: %u\n", r->contentLength());
    }

    int headers = r->headers();
    for (int i = 0; i < headers; i++) {
        AsyncWebHeader *h = r->getHeader(i);
        Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = r->params();
    for (int i = 0; i < params; i++) {
        AsyncWebParameter *p = r->getParam(i);
        if (p->isFile()) {
            Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
        } else if (p->isPost()) {
            Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        } else {
            Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
    }
    r->send(404);
};

void WebServerAsync::onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            onConnectionEvent(client->id());
            return;
        case WS_EVT_DISCONNECT:
            onDisconnectionEvent(client->id());
            return;
        case WS_EVT_DATA: {
            AwsFrameInfo *info = (AwsFrameInfo *)arg;
            //the whole message is in a single frame and we got all of it's data
            if (info->final && info->index == 0 && info->len == len) {
                if (info->opcode == WS_TEXT)
                    onReceiveDataEvent(client->id(), (char *)&data[0]);
            }
            return;
        }
        default:
            return;
    }
}
