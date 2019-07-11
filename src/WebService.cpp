#include "WebService.h"

WebService::WebService(IPAddress ip, int http_port, int websocket_port,
                       const char *root) {
    #ifdef DEBUG_WEB_SERVICE
        USE_DEBUG_SERIAL.printf("ipaddr=\"%s\"\r\n", ip.toString().c_str());
    #endif
    File f = SPIFFS.open(FILE_WEB_SETTINGS, "w");
    f.printf("ipaddr=\"%s\"\r\n", ip.toString().c_str());
    f.flush();
    f.close();
    
    this->root = root;

    server = new ESP8266WebServer(ip, http_port);

    server->on("/upload", HTTP_POST, [this]() { server->send(200); },
               [this]() { onFileUploading(); });

    server->on("/filelist", HTTP_GET, [this]() { onGetFileList(); });

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

    server->on("/generate_204", HTTP_GET, [this]() { onNoContent(); });

    server->onNotFound([this]() {
        output->printf("[http] -> %s", this->server->uri().c_str());
        if (!this->getFileContent(server->uri())) {
            output->println("404");
            this->onNotFound();
        }
    });

    socket = new WebSocketsServer(websocket_port);
    socket->onEvent(
        [this](uint8_t num, WStype_t type, uint8_t *payload, size_t lenght) {
            socketEvent(num, type, payload, lenght);
        });

    ssdp = new SSDPClass();
    setup_ssdp(ssdp, http_port);
    if (ssdp->begin()) {
        server->on("/description.xml", HTTP_GET,
                   [this]() { ssdp->schema(server->client()); });
    }
}

void WebService::setOutput(Print *p) { this->output = p; }

void WebService::onNoContent() { server->send(204, "text/plan", "No Content"); }

void WebService::onNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server->uri();
    message += "\nMethod: ";
    message += (server->method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server->args();
    message += "\n";

    for (uint8_t i = 0; i < server->args(); i++)
        message += " " + server->argName(i) + ": " + server->arg(i) + "\n";

    server->send(404, "text/plain", message);
}

void WebService::begin() {
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
    output->printf("[ws] %d -> %s\r\n", num, payload);
    socket->sendTXT(num, payload, strlen(payload));
}

void WebService::socketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                             size_t lenght) {
    output->printf("[ws] %d ", num);

    String data = (char *)&payload[0];
    switch (type) {
        case WStype_CONNECTED:
            output->println(F("connected"));
            onConnection(num, true);
            break;
        case WStype_DISCONNECTED:
            output->println("disconnected");
            onConnection(num, false);
            break;
        case WStype_TEXT:
            output->printf(" <- %s\r\n", data.c_str());
            onData(num, data);
            return;
        case WStype_BIN:
            output->printf(" <- (binnary) %s\r\n", str_utils::formatSize(lenght).c_str());
            hexdump(payload, lenght);
            break;
        default:
            output->printf(" <- (unhandled) %d\r\n", type);
            break;
    }
}

bool WebService::getFileContent(const String uri) {
    String path = getFilePath(uri);
    path += uri.endsWith("/") ? "index.html" : "";

    return sendFile(path + ".gz") || sendFile(path);
}

String WebService::getFilePath(const String uri) { return String(root) + uri; }

bool WebService::sendFile(String path) {
    if (SPIFFS.exists(path)) {
        String type;
        if (server->hasArg("download"))
            type = F("application/octet-stream");
        else
            type = getContentType(path);

        File f = SPIFFS.open(path, "r");
        size_t sent = server->streamFile(f, type);
        f.close();

        output->print(path.c_str());
        output->print(" ");
        output->print(type.c_str());
        output->print(" ");
        output->print(str_utils::formatSize(sent).c_str());
        output->println();

        return true;
    }
    return false;
}

void WebService::onGetFileList() {
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

void WebService::onFileUploading() {
    File fsUploadFile;
    HTTPUpload &upload = server->upload();
    if (upload.status == UPLOAD_FILE_START) {
        int i, ac = server->args();
        for (i = 0; i < ac; i++) {
            Serial.printf("[http] %d %s %s\r\n", i, server->argName(i).c_str(),
                          server->arg(i).c_str());
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

        output->printf("[http] upload \"%s\"\r\n", filename.c_str());
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
            Serial.print("size: ");
            Serial.println(upload.totalSize);
            // Redirect the client to the success page
            server->sendHeader("location", "/success.html");
            server->send(303);
        } else {
            server->send(500, "text/plain", "500: couldn't create file");
        }
    }
}

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

void WebService::setOnConnection(WebSocketConnectionCallback callback) {
    onConnection = callback;
}

void WebService::setOnData(WebSocketDataCallback callback) {
    onData = callback;
}

void WebService::setup_ssdp(SSDPClass *ssdp, uint16_t port) {
    ssdp->setSchemaURL(F("description.xml"));
    ssdp->setHTTPPort(port);
    ssdp->setName(HOSTNAME);
    ssdp->setModelName(APPNAME);
    ssdp->setModelNumber(FW_VERSION);
    ssdp->setSerialNumber(getChipId());
    ssdp->setURL(F("index.html"));
    ssdp->setModelURL(F(
        "https://wiki.odroid.com/accessory/power_supply_battery/smartpower2"));
    ssdp->setManufacturer(F("HardKernel"));
    ssdp->setManufacturerURL(F("https://www.hardkernel.com"));
    ssdp->setDeviceType(F("pnp:rootdevice"));
}
