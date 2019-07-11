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

void OTAUpdate::setOutput(Print *p) { output = p; }

bool OTAUpdate::begin(const char *hostname, uint16_t port) {
    // arduinoOTA->setPassword("admin");
    // arduinoOTA->setPasswordHash("21232f297a57a5a743894a0e4a801fc3"); //
    // MD5(admin)
    arduinoOTA->setHostname(hostname);
    arduinoOTA->setPort(port);
    arduinoOTA->setRebootOnSuccess(false);
    arduinoOTA->begin(false);
    return true;
}

void OTAUpdate::onStart(void) {
    output->print(FPSTR(str_ota_update));
    output->print(FPSTR(str_start));
    PGM_P strP;
    switch (this->arduinoOTA->getCommand()) {
        case U_FLASH:
            strP = str_firmware;
            break;
        case U_SPIFFS:
            strP = str_spiffs;
            break;
        default:
            strP = str_unknown;
            break;
    }
    output->println(FPSTR(strP));
}

void OTAUpdate::onEnd() {
    output->print(FPSTR(str_ota_update));
    output->println(FPSTR(str_complete));
}

void OTAUpdate::onProgress(unsigned int progress, unsigned int total) {
    char buf[8];
    output->printf_P(buf, FPSTR(strf_progress), progress / total / 100);
    output->print('\r');
}

void OTAUpdate::onError(ota_error_t error) {
    output->print(FPSTR(str_ota_update));
    output->print(FPSTR(str_error));
    if (error == OTA_AUTH_ERROR) {
        output->println("auth");
    } else if (error == OTA_BEGIN_ERROR) {
        output->println("begin");
    } else if (error == OTA_CONNECT_ERROR) {
        output->println("connect");
    } else if (error == OTA_RECEIVE_ERROR) {
        output->println("receive");
    } else if (error == OTA_END_ERROR) {
        output->println("end");
    };
}

void OTAUpdate::loop() {
    if (arduinoOTA) {
        arduinoOTA->handle();
    }
}
