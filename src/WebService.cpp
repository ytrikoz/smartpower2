#include "WebService.h"

#include "wireless.h"

WebService::WebService(uint16_t http_port, uint16_t socket_port,
                       const char *root) {
    this->http_port = http_port;
    this->socket_port = socket_port;
    strcpy(this->root, root);
    active = false;
}

void WebService::begin() {
    USE_SERIAL.printf_P(str_http);
    IPAddress ip = wireless::hostIP();
    USE_SERIAL.printf_P(strf_http_params, root, ip.toString().c_str(),
                        http_port, socket_port);
    server = new ESP8266WebServer(ip, http_port);
    server->on("/upload", HTTP_POST, [this]() { server->send(200); },
               [this]() { fileUpload(); });

    server->on("/filelist", HTTP_GET, [this]() { getFileList(); });

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

    server->on("/generate_204", HTTP_GET, [this]() { noContent(); });

    server->onNotFound([this]() {
        String uri = server->uri();
        #ifdef DEBUG_HTTP 
        USE_DEBUG_SERIAL->printf_P(str_http);
        #endif
        if (!sendFile(uri)) {
            char buf[128];
            sprintf_P(buf, strf_http_file_not_found, server->uri().c_str(),
                      (server->method() == HTTP_GET) ? "GET" : "POST",
                      server->args());
            server->send(404, "text/plan", buf);
            #ifdef DEBUG_HTTP 
            USE_DEBUG_SERIAL.printf_P(strf_file_not_found, uri.c_str();
            USE_DEBUG_SERIAL.println();
            #endif
        }        
    });

    socket = new WebSocketsServer(this->socket_port);
    socket->onEvent(
        [this](uint8_t num, WStype_t type, uint8_t *payload, size_t lenght) {
            socketEvent(num, type, payload, lenght);
        });

    File f = SPIFFS.open(FILE_WEB_SETTINGS, "w");
    f.printf("ipaddr=\"%s\"\r\n", ip.toString().c_str());
    f.flush();
    f.close();

    ssdp = new SSDPClass();
    ssdp->setSchemaURL(F("description.xml"));
    ssdp->setHTTPPort(http_port);
    ssdp->setName(HOST_NAME);
    ssdp->setModelName(APPNAME);
    ssdp->setModelNumber(FW_VERSION);
    ssdp->setSerialNumber(getChipId());
    ssdp->setURL(F("index.html"));
    ssdp->setModelURL(F(
        "https://wiki.odroid.com/accessory/power_supply_battery/smartpower2"));
    ssdp->setManufacturer(F("HardKernel"));
    ssdp->setManufacturerURL(F("https://www.hardkernel.com"));
    ssdp->setDeviceType(F("pnp:rootdevice"));

    server->on("/description.xml", HTTP_GET,
               [this]() { ssdp->schema(server->client()); });

    USE_SERIAL.println();

    server->begin();
    socket->begin();

    active = true;
}

void WebService::loop() {
    if (!active) return;
    server->handleClient();
    socket->loop();
}

void WebService::sendTxt(uint8_t num, const char *payload) {
    #ifdef DEBUG_WEBSOCKET
    output->printf_P(str_http);
    output->printf_P(strf_client, num);
    output->printf_P(strf_arrow_dest, payload);
    output->println();
    #endif 
    socket->sendTXT(num, payload, strlen(payload));
}

void WebService::setOutput(Print *p) { this->output = p; }

void WebService::noContent() { server->send(204, "text/plan", "No Content"); }

void WebService::socketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                             size_t lenght) {
    #ifdef DEBUG_WEBSOCKET
    USE_DEBUG_SERIAL.printf_P(str_http);
    USE_DEBUG_SERIAL.printf_P(strf_client, num);
    #endif
    switch (type) {
        case WStype_CONNECTED:
            #ifdef DEBUG_WEBSOCKET
            USE_DEBUG_SERIAL.printf_P(str_connected);
            USE_DEBUG_SERIAL.println();
            #endif
            onConnectEvent(num);
            return;
        case WStype_DISCONNECTED:
            #ifdef DEBUG_WEBSOCKET
            USE_DEBUG_SERIAL.printf_P(str_disconnected);
            USE_DEBUG_SERIAL.println();
            #endif
            onDisconnectEvent(num);
            return;
        case WStype_TEXT: {
            #ifdef DEBUG_WEBSOCKET
            USE_DEBUG_SERIAL.printf_P(strf_arrow_src, (char *)&payload[0]);
            USE_DEBUG_SERIAL.println();
            #endif
            onDataEvent(num, (char *)&payload[0]);
            return;
        }
        case WStype_BIN:
            #ifdef DEBUG_WEBSOCKET
            USE_DEBUG_SERIAL.printf_P(strf_binnary,
                             str_utils::formatSize(lenght).c_str());
            hexdump(payload, lenght);
            USE_DEBUG_SERIAL.println();
            #endif
            return;
        default:
            #ifdef DEBUG_WEBSOCKET
            USE_DEBUG_SERIAL.printf_P(strf_unhandled, type);
            USE_DEBUG_SERIAL.println();
            #endif
            return;
    }
}

bool WebService::sendFile(String uri) {
    String path = getFilePath(uri);

    return sendFileContent(path + ".gz") || sendFileContent(path);
}

String WebService::getFilePath(String uri) {
    String path = String(root);
    path += uri;
    if (uri.endsWith("/")) {
        path.concat(F("index.html"));
    }
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
#ifdef DEBUG_HTTP
        USE_DEBUG_SERIAL.print(path.c_str());
        USE_DEBUG_SERIAL.print(" ");
        USE_DEBUG_SERIAL.print(type.c_str());
        USE_DEBUG_SERIAL.print(" ");
        USE_DEBUG_SERIAL.print(str_utils::formatSize(sent).c_str());
        USE_DEBUG_SERIAL.println();
#endif
        return true;
    }
    return false;
}

void WebService::getFileList() {
    String path = server->hasArg("path") ? server->arg("path") : root;
    output->printf("[http] filelist %s\r\n", path.c_str());
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
    File fsUploadFile;
    HTTPUpload &upload = server->upload();
    if (upload.status == UPLOAD_FILE_START) {
        int i, ac = server->args();
        for (i = 0; i < ac; i++) {
            output->printf_P(str_http);
            output->printf("%d %s %s\r\n", i, server->argName(i).c_str(),
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