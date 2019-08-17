#include "OTAUpdate.h"

#include "StrUtils.h"
#define OTA_FLASH   0
#define OTA_FS      100

OTAUpdate::OTAUpdate() {
    ota = new ArduinoOTAClass();
    ota->onStart([this]() { this->onStart(); });
    ota->onProgress([this](unsigned int progress, unsigned int total) {
        this->onProgress(progress, total);
    });
    ota->onEnd([this]() { this->onEnd(); });
    ota->onError([this](ota_error_t error) { this->onError(error); });
    
    active = false;
}

void OTAUpdate::setOutput(Print *p) { output = p; }

bool OTAUpdate::begin(const char *host, uint16_t port) {
    // arduinoOTA->setPassword("admin");
    // arduinoOTA->setPasswordHash("21232f297a57a5a743894a0e4a801fc3"); //
    // MD5(admin)   
    USE_SERIAL.printf_P(str_update);
    if (strlen(host) == 0)
    {
        output->printf_P(str_failed);
        output->printf_P(str_host);
        output->printf_P(str_unset);
        output->println();
        return false;
    }
    ota->setHostname(host);
    ota->setPort(port);
    ota->setRebootOnSuccess(false);
    ota->begin(false);
    active = true;
    if (active)
    {
        output->printf_P(strf_s_d, host, port);
    } else {        
        output->printf_P(str_failed);       
    }
    output->println();
    return active;
}

void OTAUpdate::onStart(void) {
    output->printf_P(str_update);
    output->printf_P(str_start);
    PGM_P strP;
    switch (this->ota->getCommand()) {
        case OTA_FLASH:
            strP = str_firmware;
            break;
        case OTA_FS:
            strP = str_spiffs;
            break;
        default:
            strP = str_unknown;
            break;
    }
    output->printf_P(strP);
    output->println();
}

void OTAUpdate::onEnd() {
    output->printf_P(str_update);
    output->printf_P(str_complete);
    output->println();
}

void OTAUpdate::onProgress(unsigned int progress, unsigned int total) {
    char buf[8];
    output->printf_P(buf, strf_progress, progress / total / 100);
    output->print('\r');
}

void OTAUpdate::onError(ota_error_t error) {
    output->printf_P(str_update);
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
    output->printf_P(str_error);
    output->println();
}

void OTAUpdate::loop() {
    if (active) {
        ota->handle();
    }
}
