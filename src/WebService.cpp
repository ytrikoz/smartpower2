#include "WebService.h"

#include "Wireless.h"

using StrUtils::isip;
using StrUtils::setstr;

File fsUploadFile;

static const char stre_http_file_not_found[] PROGMEM =
    "File Not Found\n\nURI: %s\nMethod: %s\nArguments: %d\n";

WebService::WebService() {
    this->port_http = HTTP_PORT;
    this->port_websocket = WEBSOCKET_PORT;
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
    // server->on("/generate_204", HTTP_GET, [this]() { noContent(); });
    server->on("/generate_204", [this]() { handleRoot(); });
    // Microsoft captive portal.
    server->on("/fwlink", [this]() { handleRoot(); });
    // Root
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
    output->print(FPSTR(str_http));
    output->printf_P(strf_http_params, web_root, port_http, port_websocket);                     
    output->println();

    File f = SPIFFS.open(FILE_WEB_SETTINGS, "w");
    f.printf("ipaddr=\"%s\"\r\n", Wireless::hostIP().toString().c_str());
    f.flush();
    f.close();
 
    server->begin();
    websocket->begin();

    active = true;
}

void WebService::end() {
    output->print(FPSTR(str_http));
    output->print(FPSTR(str_stopped));
    output->println();
    active = false;
}

void WebService::loop() {
    if (!active) return;
    server->handleClient();
    //delay(2);
    websocket->loop();
    //delay(2);
}

void WebService::handleRoot() {
    if (captivePortal()) {  // If caprive portal redirect instead of displaying
                            // the page.
        return;
    }
    handleUri();
}

void WebService::handleUri() {
    String uri = server->uri();
    if (!sendFile(uri)) {
        handleNotFound(uri);
    }
}

void WebService::handleNotFound(String &uri) {
#ifdef DEBUG_HTTP
    DEBUG.print(FPSTR(str_http));
    DEBUG.printf_P(strf_file_not_found, uri.c_str());
    DEBUG.println();
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

void WebService::sendTxt(uint8_t num, const char *payload) {
#ifdef DEBUG_WEBSOCKET
    output->printf_P(str_http);
    output->printf_P(strf_client, num);
    output->printf_P(strf_arrow_dest, payload);
    output->println();
#endif
    websocket->sendTXT(num, payload, strlen(payload));
}

void WebService::setOutput(Print *p) { this->output = p; }

void WebService::noContent() { server->send(204, "text/plan", "No Content"); }

void WebService::webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                                size_t lenght) {
#ifdef DEBUG_WEBSOCKET
    DEBUG.printf_P(str_http);
    DEBUG.printf_P(strf_client, num);
#endif
    switch (type) {
        case WStype_CONNECTED:
#ifdef DEBUG_WEBSOCKET
            DEBUG.printf_P(str_connected);
            DEBUG.println();
#endif
            onConnectEvent(num);
            return;
        case WStype_DISCONNECTED:
#ifdef DEBUG_WEBSOCKET
            DEBUG.printf_P(str_disconnected);
            DEBUG.println();
#endif
            onDisconnectEvent(num);
            return;
        case WStype_TEXT: {
#ifdef DEBUG_WEBSOCKET
            DEBUG.printf_P(strf_arrow_src, (char *)&payload[0]);
            DEBUG.println();
#endif
            onDataEvent(num, (char *)&payload[0]);
            return;
        }
        case WStype_BIN:
#ifdef DEBUG_WEBSOCKET
            DEBUG.printf_P(strf_binnary, StrUtils::formatSize(lenght).c_str());
            hexdump(payload, lenght);
            DEBUG.println();
#endif
            return;
        default:
#ifdef DEBUG_WEBSOCKET
            DEBUG.printf_P(strf_unhandled, type);
            DEBUG.println();
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
#ifndef DEBUG_HTTP
        server->streamFile(f, type);
        f.close();
#else
        size_t sent = server->streamFile(f, type);
        f.close();
        DEBUG.print(path.c_str());
        DEBUG.print(" ");
        DEBUG.print(type.c_str());
        DEBUG.print(" ");
        DEBUG.print(StrUtils::formatSize(sent).c_str());
        DEBUG.println();
#endif
        return true;
    }
    return false;
}

void WebService::handleFileList() {
    String path = server->hasArg("path") ? server->arg("path") : web_root;
    if (!path.startsWith("/")) path = "/" + path;
    output->print(FPSTR(str_http));
    output->printf(strf_filelist, path.c_str());
    output->println();
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
            output->print(FPSTR(str_http));
            output->printf("%d %s=%s", i, server->argName(i).c_str(),
                           server->arg(i).c_str());
            output->println();
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

        output->printf("[http] upload %s\r\n", filename.c_str());
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
            output->print("size: ");
            output->println(upload.totalSize);
            // Redirect the client to the success page
            server->sendHeader("location", "/success.html");
            server->send(303);
        } else {
            server->send(500, "text/plain", "500: couldn't create file");
        }
    }
}

/** Redirect to captive portal if we got a request for another domain. Return
 * true in that case so the page handler do not try to handle the request again.
 */
bool WebService::captivePortal() {
    if (!isip(server->hostHeader()) &&
        server->hostHeader() != (Wireless::hostName() + ".local")) {
#ifdef DEBUG_HTTP
        output->print(FPSTR(str_http));
        output->print(FPSTR(str_redirected));
        output->println(server->hostHeader().c_str());
        output->println();
#endif
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