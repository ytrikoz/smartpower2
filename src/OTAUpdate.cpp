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
    USE_SERIAL.print(getIdentStrP(str_update));
    if (strlen(host) == 0)
    {
        output->print(getStrP(str_failed));
        output->print(getStrP(str_host));
        output->print(getStrP(str_unset));
        output->println();
        active = false;
    }
    ota->setHostname(host);
    ota->setPort(port);
    ota->setRebootOnSuccess(false);
    ota->begin(false);
    
    active = true;

    if (active) 
        output->printf_P(strf_s_d, host, port);
    else
        output->print(getStrP(str_failed));
    output->println();
    return active;
}

void OTAUpdate::onStart(void) {
    total_progress = 0;
    output->print(getIdentStrP(str_update));
    output->print(getStrP(str_start));
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
    output->print(getStrP(strP));
    output->println();
}

void OTAUpdate::onEnd() {
    output->print(getIdentStrP(str_update));
    output->print(getStrP(str_update));
    output->println(getStrP(str_complete));
}


void OTAUpdate::onProgress(unsigned int progress, unsigned int total) {
    uint8_t cur_progress = progress / total * 100;
    if (cur_progress - total_progress >= 10)
    {
        total_progress = cur_progress;
        output->printf_P(strf_progress, total_progress);
    }    
}

void OTAUpdate::onError(ota_error_t error) {
    output->print(getIdentStrP(str_update));
    if (error == OTA_AUTH_ERROR) {
        output->print(getStrP(str_auth));
    } else if (error == OTA_BEGIN_ERROR) {
        output->print(getStrP(str_begin));
    } else if (error == OTA_CONNECT_ERROR) {
        output->println(getStrP(str_connect));
    } else if (error == OTA_RECEIVE_ERROR) {
        output->println(getStrP(str_receive));
    } else if (error == OTA_END_ERROR) {
        output->println(getStrP(str_end));
    };
    output->println(getStrP(str_error));
}

void OTAUpdate::loop() {
    if (active) {
        ota->handle();
    }
}
