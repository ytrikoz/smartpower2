#include "WebServer.h"

#include "AppUtils.h"
#include "PrintUtils.h"
#include "StoreUtils.h"
#include "Wireless.h"

using namespace AppUtils;
using namespace StoreUtils;
using namespace StrUtils;
using namespace PrintUtils;

WebServer::WebServer(uint16_t http, uint16_t websocket) {
    server = new ESP8266WebServer();
    socket = new WebSocketsServer(WEBSOCKET_PORT);
    
    server->on("/", [this]() { handleRoot(); });
    server->on("/upload", HTTP_POST, [this]() { server->send(200); },
               [this]() { handleUpload(); });

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
    server->on("/generate_204", [this]() { handleRoot(); });//handleNoContent(); });
    // Microsoft captive portal.
    server->on("/fwlink", [this]() { handleRoot(); });//handleNoContent(); });

    server->onNotFound([this]() { handleUri(); });

    socket->onEvent(
        [this](uint8_t num, WStype_t type, uint8_t *payload, size_t lenght) {
            handleWebSocketEvent(num, type, payload, lenght);
        });

    Wireless::NetworkMode mode = Wireless::getMode();

    if ((mode == Wireless::NETWORK_STA || mode == Wireless::NETWORK_AP_STA)) {
        ssdp = new SSDPClass();
        ssdp->setDeviceType(F("upnp:rootdevice"));
        ssdp->setSchemaURL(F("description.xml"));
        ssdp->setHTTPPort(HTTP_PORT);
        ssdp->setName(Wireless::hostName());
        ssdp->setModelName(APP_NAME);
        ssdp->setModelNumber(APP_VERSION);
        ssdp->setSerialNumber(SysInfo::getChipId());
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
}

bool WebServer::start() {
    char buf[64];
    sprintf(buf, "ipaddr=\"%s\"\r\n", Wireless::hostIP().toString().c_str());
    StoreUtils::storeString(FS_WEB_CONFIG, buf);

    server->begin(HTTP_PORT);
    socket->begin();
    return true;
}

void WebServer::stop() {
    server->stop();
    socket->close();
}

void WebServer::loop() {
    server->handleClient();
    socket->loop();
}

void WebServer::handleRoot() {
    if (!captivePortal())
        handleUri();
}

void WebServer::handleUri() {
    String uri = server->uri();
    if (!sendFile(uri))
        handleNotFound(uri);
}

void WebServer::handleNotFound(String &uri) {
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

    server->send(404, "text/plain", str);
}

void WebServer::sendTxt(const uint8_t num, String &payload) {
    socket->sendTXT(num, payload);
}

void WebServer::handleNoContent() {
    server->send(204, "text/plan", "No Content");
}

void WebServer::handleWebSocketEvent(uint8_t num, WStype_t type,
                                      uint8_t *payload, size_t lenght) {
    switch (type) {
    case WStype_CONNECTED:
        onConnectEvent(num);
        return;
    case WStype_DISCONNECTED:
        onDisconnectEvent(num);
        return;
    case WStype_TEXT: {
        onDataEvent(num, (char *)&payload[0]);
        return;
    }
    case WStype_BIN:
        return;
    case WStype_PING:
        return;
    case WStype_PONG:
        return;
    default:
        return;
    }
}

bool WebServer::sendFile(String uri) {
    String path = getFilePath(uri);
    return sendFileContent(path + ".gz") || sendFileContent(path);
}

String WebServer::getFilePath(String uri) {
    String path(FS_WEB_ROOT);
    path += uri;
    if (uri.endsWith("/")) path.concat("index.html");
    println(&DEBUG, uri);
    return path;
}

bool WebServer::sendFileContent(String path) {
    size_t sent = 0;
    println(&DEBUG, path);
    if (SPIFFS.exists(path)) {
        String type = server->hasArg("download")? "application/octet-stream": getContentType(path);
        File f = SPIFFS.open(path, "r");
        sent = server->streamFile(f, type);
        f.close();
    }
    return sent;
}

void WebServer::handleFileList() {
    String path = server->hasArg("path") ? server->arg("path") : FS_WEB_ROOT;
    if (!path.startsWith("/"))
        path = "/" + path;
    Dir dir = SPIFFS.openDir(path);
    String output = "[";
    while (dir.next()) {
        File entry = dir.openFile("r");
        if (output != "[")
            output += ',';
        bool isDir = dir.isDirectory();
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

void WebServer::handleUpload() {
    HTTPUpload &upload = server->upload();
    if (upload.status == UPLOAD_FILE_START) {
        int ac = server->args();
        int i;
        for (i = 0; i < ac; i++) {
            char buf[64];
            sprintf(buf, "%d %s=%s", i, server->argName(i).c_str(),
                    server->arg(i).c_str());
            // pinr(buf);
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
        // saylnf("%s %s", StrUtils::getStrP(str_upload).c_str(),
        // filename.c_str()); out->print(getIdentStrP(str_http));
        // out->print(StrUtils::getStrP(str_upload));
        // out->print(filename);

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
            // Redirect the client to the success page
            server->sendHeader("location", "/success.html");
            server->send(303);
        } else {
            server->send(500, "text/plain", "500: couldn't create file");
        }
    }
}

bool WebServer::captivePortal() {
    if (!isip(server->hostHeader()) &&
        server->hostHeader() != Wireless::hostName()) {
        server->sendHeader(
            "Location",
            String("http://") + server->client().localIP().toString(), true);
        server->send(302, "text/plain",
                     ""); // Empty content inhibits Content-length header so we
                          // have to close the websocket ourselves.
        server->client()
            .stop(); // Stop is needed because we sent no content length
        return true;
    }
    return false;
}

void WebServer::setOnClientConnection(SocketConnectionEventHandler h) {
    onConnectEvent = h;
}

void WebServer::setOnClientDisconnected(SocketConnectionEventHandler h) {
    onDisconnectEvent = h;
}

void WebServer::setOnClientData(SocketDataEventHandler h) { onDataEvent = h; }

String WebServer::getContentType(String filename) {
    String result;
    if ((filename.endsWith(".htm")) || (filename.endsWith(".html"))) {
        result = "text/html";
    } else if (filename.endsWith(".css")) {
        result = "text/css";
    } else if (filename.endsWith(".js")) {
        result = "application/javascript";
    } else if (filename.endsWith(".png")) {
        result = "image/png";
    } else if (filename.endsWith(".gif")) {
        result = "image/gif";
    } else if (filename.endsWith(".jpg")) {
        result = "image/jpeg";
    } else if (filename.endsWith(".ico")) {
        result = "image/x-icon";
    } else if (filename.endsWith(".xml")) {
        result = "text/xml";
    } else if (filename.endsWith(".pdf")) {
        result = "application/x-pdf";
    } else if (filename.endsWith(".zip")) {
        result = "application/x-zip";
    } else if (filename.endsWith(".gz")) {
        result = "application/x-gzip";
    } else 
        result =  "text/plain";
    return result;
}