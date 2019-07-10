#include "OTAUpdate.h"

OTAUpdate::OTAUpdate() {
    arduinoOTA = new ArduinoOTAClass();
    arduinoOTA->onStart([this]() { this->onStart(); });
    arduinoOTA->onProgress([this](unsigned int progress, unsigned int total) {
        this->onProgress(progress, total);
    });
    arduinoOTA->onEnd([this]() { this->onEnd(); });
    arduinoOTA->onError([this](ota_error_t error) { this->onError(error); });
}

void OTAUpdate::setOutput(Print *p) { _output = p; }

bool OTAUpdate::begin(const char *hostname, uint16_t port) {
    // arduinoOTA->setPassword("admin");
    // arduinoOTA->setPasswordHash("21232f297a57a5a743894a0e4a801fc3"); //
    // MD5(admin)

    arduinoOTA->setPort(port);
    arduinoOTA->setHostname(hostname);
    arduinoOTA->setRebootOnSuccess(false);
    arduinoOTA->begin(false);

    return true;
}

void OTAUpdate::onStart(void) {
    _output->print("[update]");
    _output->print(" start ");
    switch (this->arduinoOTA->getCommand()) {
        case U_FLASH:
            _output->println("firmware");
            break;
        case U_SPIFFS:
            _output->println("spiffs");
            break;
        default:
            _output->println("unknown");
            break;
    }
}

void OTAUpdate::onEnd() {
    _output->print("[update]");
    _output->println(" complete");
}

void OTAUpdate::onProgress(unsigned int progress, unsigned int total) {
    char buf[16];
    sprintf(buf, " %u%%\r", (progress / (total / 100)));
    _output->print(buf);
}

void OTAUpdate::onError(ota_error_t error) {
    _output->print("[update] ");
    _output->print("error on ");
    if (error == OTA_AUTH_ERROR) {
        _output->println("auth");
    } else if (error == OTA_BEGIN_ERROR) {
        _output->println("begin");
    } else if (error == OTA_CONNECT_ERROR) {
        _output->println("connect");
    } else if (error == OTA_RECEIVE_ERROR) {
        _output->println("receive");
    } else if (error == OTA_END_ERROR) {
        _output->println("end");
    };
}

void OTAUpdate::loop() {
    if (arduinoOTA) {
        arduinoOTA->handle();
    }
}
