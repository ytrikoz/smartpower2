#include "WebService.h"

#include "StoreUtils.h"
#include "Wireless.h"

using StoreUtils::storeString;
using StrUtils::isip;
using StrUtils::setstr;

WebService::WebService() {
    this->port_http = HTTP_PORT;
    this->port_websocket = WEBSOCKET_PORT;
    this->web_root = new char[strlen(HTTP_WEB_ROOT) + 1];
    strcpy(this->web_root, HTTP_WEB_ROOT);

    server = new ESP8266WebServer(port_http);
    server->on("/upload", HTTP_POST, [this]() { server->send(200); },
               [this]() { fileUpload(); });

    server->on("/filelist", HTTP_GET, [this]() { handleFileList(); });

    server->on("/cli", HTTP_GET, [this]() {
        if (server->args() > 0) {
            String command = "";
            String result = "";
            for (uint8_t i = 0; i < server->args(); i++)
                command +=
                    " " + server->argName(i) + " " + server->arg(i) + "\n";
            if (1 == 0) {
                server->send(200, "text/plain", result);
            } else {
                server->send(500, "text/plain", result);
            }
        } else {
            server->send(404, "text/plain", "Bad Args");
        }
    });

    // Android captive portal.
    server->on("/generate_204", [this]() { noContent(); });
    // server->on("/generate_204", [this]() { handleRoot(); });
    // Microsoft captive portal.
    server->on("/fwlink", [this]() { handleRoot(); });
    server->on("/", [this]() { handleRoot(); });
    server->onNotFound([this]() { handleUri(); });

    websocket = new WebSocketsServer(this->port_websocket);
    websocket->onEvent(
        [this](uint8_t num, WStype_t type, uint8_t *payload, size_t lenght) {
            webSocketEvent(num, type, payload, lenght);
        });

    if ((Wireless::getWirelessMode() == WLAN_STA ||
         Wireless::getWirelessMode() == WLAN_AP_STA)) {
        ssdp = new SSDPClass();
        ssdp->setSchemaURL(F("description.xml"));
        ssdp->setHTTPPort(port_http);
        ssdp->setName(Wireless::hostName());
        ssdp->setModelName(APPNAME);
        ssdp->setModelNumber(FW_VERSION);
        ssdp->setSerialNumber(getChipId());
        ssdp->setURL(F("index.html"));
        ssdp->setModelURL(
            F("https://wiki.odroid.com/accessory/power_supply_battery/"
              "smartpower2"));
        ssdp->setManufacturer(F("HardKernel"));
        ssdp->setManufacturerURL(F("https://www.hardkernel.com"));
        ssdp->setDeviceType(F("pnp:rootdevice"));
        server->on(F("/description.xml"), HTTP_GET,
                   [this]() { ssdp->schema(server->client()); });
    }
    active = false;
}

void WebService::begin() {
    String ip_str = Wireless::hostIP().toString();
    out->print(getIdentStrP(str_http));
    out->print(getStr(web_root));
    out->print(ip_str);
    out->print(':');
    out->print(port_http);
    out->print(',');
    out->println(port_websocket);

    String tmp = "ipaddr=\"";
    tmp += ip_str;
    tmp += "\"\r\n";
    StoreUtils::storeString(FILE_WEB_SETTINGS, tmp);

    server->begin();

    websocket->begin();

    active = true;
}

void WebService::end() {
    out->print(getIdentStrP(str_http));
    out->println(getStr(str_stopped));

    active = false;
}

void WebService::loop() {
    if (!active) return;
    server->handleClient();
    websocket->loop();
}

void WebService::handleRoot() {
    if (!captivePortal()) handleUri();
    ;
}

void WebService::handleUri() {
    String uri = server->uri();
    if (!sendFile(uri)) handleNotFound(uri);
}

void WebService::handleNotFound(String &uri) {
#ifdef DEBUG_WEB_SERVICE
    DEBUG.print(getIdentStrP(str_http));
    DEBUG.print(getStrP(str_file));
    DEBUG.print(getStrP(str_not));
    DEBUG.print(getStrP(str_found));
    DEBUG.println(uri);
#endif
    String str = F("File Not Found\n");
    str += F("\nURI: ");
    str += server->uri();
    str += F("\nMethod: ");
    str += (server->method() == HTTP_GET) ? "GET" : "POST";
    str += F("\nArguments: ");
    str += server->args();
    str += '\n';

    for (uint8_t i = 0; i < server->args(); i++)
        str += server->argName(i) + ": " + server->arg(i) + '\n';

    server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server->sendHeader("Pragma", "no-cache");
    server->sendHeader("Expires", "-1");

    server->send(404, "text/plain", str);
}

void WebService::sendTxt(uint8_t num, String &payload) {
#ifdef DEBUG_WEB_SERVICE
    out->print(getIdentStrP(str_http));
    out->print('#');
    out->print(getStr(num));
    out->print(getStrP(str_arrow_dest));
    out->println(payload);
#endif
    websocket->sendTXT(num, payload);
}

void WebService::setOutput(Print *p) { this->out = p; }

void WebService::noContent() { server->send(204, "text/plan", "No Content"); }

void WebService::webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                                size_t lenght) {
#ifdef DEBUG_WEB_SERVICE
    out->print(getIdentStrP(str_http));
    out->print('#');
    out->print(getStr(num));
    ;
#endif
    switch (type) {
        case WStype_CONNECTED:
#ifdef DEBUG_WEB_SERVICE
            out->println(getStrP(str_connected));
#endif
            onConnectEvent(num);
            return;
        case WStype_DISCONNECTED:
#ifdef DEBUG_WEB_SERVICE
            out->println(getStrP(str_disconnected));
#endif
            onDisconnectEvent(num);
            return;
        case WStype_TEXT: {
#ifdef DEBUG_WEB_SERVICE
            out->print(getStrP(str_arrow_src));
            out->println((char *)&payload[0]);
#endif
            onDataEvent(num, (char *)&payload[0]);
            return;
        }
        case WStype_BIN:
#ifdef DEBUG_WEB_SERVICE
            out->print(getStrP(str_arrow_src));
            out->println(StrUtils::formatSize(lenght).c_str());
#endif
            return;
        case WStype_PING:
#ifdef DEBUG_WEB_SERVICE
            out->println(getStrP(str_ping, false));
#endif
            return;
        case WStype_PONG:
#ifdef DEBUG_WEB_SERVICE
            out->println(getStrP(str_pong, false));
#endif
            return;
        default:
#ifdef DEBUG_WEB_SERVICE
            out->print(getStrP(str_unhandled));
            out->println(type);
#endif
            return;
    }
}

bool WebService::sendFile(String uri) {
    String path = getFilePath(uri);
    return sendFileContent(path + ".gz") || sendFileContent(path);
}

String WebService::getFilePath(String uri) {
    String path = String(web_root);
    path += uri;
    if (uri.endsWith("/")) path.concat("index.html");
    return path;
}

bool WebService::sendFileContent(String path) {
    if (SPIFFS.exists(path)) {
        String type;
        if (server->hasArg("download"))
            type = F("application/octet-stream");
        else
            type = getContentType(path);
        File f = SPIFFS.open(path, "r");
        size_t sent = server->streamFile(f, type);
        f.close();

#ifdef DEBUG_WEB_SERVICE
        out->print(getIdentStrP(str_http));
        out->print(path);
        out->print(' ');
        out->println(StrUtils::formatSize(sent).c_str());
#endif
        return true;
    }
    return false;
}

void WebService::handleFileList() {
    String path = server->hasArg("path") ? server->arg("path") : web_root;
    if (!path.startsWith("/")) path = "/" + path;
    out->print(FPSTR(str_http));
    out->printf(strf_filelist, path.c_str());
    out->println();
    Dir dir = SPIFFS.openDir(path);
    String output = "[";
    while (dir.next()) {
        File entry = dir.openFile("r");
        if (output != "[") output += ',';
        bool isDir = false;
        output += "{\"type\":\"";
        output += (isDir) ? "dir" : "file";
        output += "\",\"name\":\"";
        output += String(entry.name()).substring(1);
        output += "\"}";
        entry.close();
    }
    output += "]";
    server->send(200, "text/json", output);
}

void WebService::fileUpload() {
    HTTPUpload &upload = server->upload();
    if (upload.status == UPLOAD_FILE_START) {
        int ac = server->args();
        int i;
        for (i = 0; i < ac; i++) {
            out->print(getIdentStrP(str_http));
            out->printf("%d %s=%s", i, server->argName(i).c_str(),
                        server->arg(i).c_str());
            out->println();
        }
        String filename = upload.filename;
        if (server->hasArg("path")) {
            String path = server->arg("path");
            if (!path.endsWith("/")) {
                path += "/";
            }
            filename = path + filename;
        } else {
            if (!filename.startsWith("/")) {
                filename = "/" + filename;
            }
        }
        filename = getFilePath(filename);
        out->print(getIdentStrP(str_http));
        out->print(getStrP(str_upload));
        out->print(filename);

        fsUploadFile = SPIFFS.open(filename, "w");
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        // Write the received bytes to the file
        if (fsUploadFile) {
            fsUploadFile.write(upload.buf, upload.currentSize);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        // If the file was successfully created
        if (fsUploadFile) {
            // Close the file again
            fsUploadFile.close();
            out->print("size: ");
            out->println(upload.totalSize);
            // Redirect the client to the success page
            server->sendHeader("location", "/success.html");
            server->send(303);
        } else {
            server->send(500, "text/plain", "500: couldn't create file");
        }
    }
}

bool WebService::captivePortal() {
#ifdef DEBUG_WEB_SERVICE
    out->print(getStrP(str_http));
    out->print(getStrP(str_redirected));
    out->println(server->hostHeader());
#endif
    if (!isip(server->hostHeader()) &&
        server->hostHeader() != Wireless::hostName()) {
        server->sendHeader(
            "Location",
            String("http://") + server->client().localIP().toString(), true);
        server->send(302, "text/plain",
                     "");  // Empty content inhibits Content-length header so we
                           // have to close the websocket ourselves.
        server->client()
            .stop();  // Stop is needed because we sent no content length
        return true;
    }
    return false;
}

void WebService::setOnClientConnection(SocketConnectionEventHandler h) {
    onConnectEvent = h;
}

void WebService::setOnClientDisconnected(SocketConnectionEventHandler h) {
    onDisconnectEvent = h;
}

void WebService::setOnClientData(SocketDataEventHandler h) { onDataEvent = h; }

const char *WebService::getContentType(String filename) {
    if ((filename.endsWith(".htm")) || (filename.endsWith(".html"))) {
        return "text/html";
    } else if (filename.endsWith(".css")) {
        return "text/css";
    } else if (filename.endsWith(".js")) {
        return "application/javascript";
    } else if (filename.endsWith(".png")) {
        return "image/png";
    } else if (filename.endsWith(".gif")) {
        return "image/gif";
    } else if (filename.endsWith(".jpg")) {
        return "image/jpeg";
    } else if (filename.endsWith(".ico")) {
        return "image/x-icon";
    } else if (filename.endsWith(".xml")) {
        return "text/xml";
    } else if (filename.endsWith(".pdf")) {
        return "application/x-pdf";
    } else if (filename.endsWith(".zip")) {
        return "application/x-zip";
    } else if (filename.endsWith(".gz")) {
        return "application/x-gzip";
    }
    return "text/plain";
}