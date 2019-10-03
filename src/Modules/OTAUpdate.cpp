#include "Modules/OTAUpdate.h"

#include "StrUtils.h"
#include "Wireless.h"

#define OTA_FLASH 0
#define OTA_FS 100

OTAUpdate::OTAUpdate() : AppModule(MOD_UPDATE) {
    ota = new ArduinoOTAClass();
    active = false;
}

bool OTAUpdate::begin() {
    String host = Wireless::getConfigHostname();
    ota->setRebootOnSuccess(false);
    ota->setHostname(host.c_str());
    ota->setPort(OTA_PORT);
    ota->onStart([this]() { this->handleStart(); });
    ota->onProgress([this](unsigned int progress, unsigned int total) {
        this->handleProgress(progress, total);
    });
    ota->onEnd([this]() { this->handleEnd(); });
    ota->onError([this](ota_error_t error) { this->handleError(error); });
    // arduinoOTA->setPassword("admin");
    // arduinoOTA->setPasswordHash("21232f297a57a5a743894a0e4a801fc3"); //
    // MD5(admin)
    ota->begin(false);

    say_strP(str_host, (char *)host.c_str());
    say_strP(str_port, OTA_PORT);

    return true;
}

void OTAUpdate::handleStart(void) {
    out->print(StrUtils::getStrP(str_start));

    update_progress = 0;
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
    out->print(StrUtils::getStrP(strP));
    out->println();
}

void OTAUpdate::handleEnd() {
    out->print(StrUtils::getIdentStrP(str_update));
    out->print(StrUtils::getStrP(str_update));
    out->println(StrUtils::getStrP(str_complete));
}

void OTAUpdate::handleProgress(unsigned int progress, unsigned int total) {
    if (progress - update_progress >= 10) {
        update_progress = progress;
        out->printf_P(strf_progress, progress);
    }
}

void OTAUpdate::handleError(ota_error_t error) {
    out->print(StrUtils::getIdentStrP(str_update));
    if (error == OTA_AUTH_ERROR) {
        out->print(StrUtils::getStrP(str_auth));
    } else if (error == OTA_BEGIN_ERROR) {
        out->print(StrUtils::getStrP(str_begin));
    } else if (error == OTA_CONNECT_ERROR) {
        out->println(StrUtils::getStrP(str_connect));
    } else if (error == OTA_RECEIVE_ERROR) {
        out->println(StrUtils::getStrP(str_receive));
    } else if (error == OTA_END_ERROR) {
        out->println(StrUtils::getStrP(str_end));
    };
    out->println(StrUtils::getStrP(str_error));
}

void OTAUpdate::loop() {
    if (active) {
        ota->handle();
    }
}
