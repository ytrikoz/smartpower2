#include "Modules/OTAUpdate.h"

#include "Utils/PrintUtils.h"

#include "Utils/StrUtils.h"

#include "Wireless.h"

#define OTA_FLASH 0
#define OTA_FS 100

namespace Modules {

OTAUpdate::OTAUpdate(uint16_t port): port_(port){};

bool OTAUpdate::onInit() {
    return true;
}

bool OTAUpdate::onStart() {
    ota = new ArduinoOTAClass();
    ota->setRebootOnSuccess(false);
    ota->onStart([this]() { this->handleArduinoOTAClassOnStart(); });
    ota->onProgress([this](unsigned int progress, unsigned int total) {
        this->handleProgress(progress, total);
    });
    ota->onEnd([this]() { this->handleEnd(); });
    ota->onError([this](ota_error_t error) { this->handleError(error); });
    ota->setPassword("admin");
    // arduinoOTA->setPasswordHash("21232f297a57a5a743894a0e4a801fc3"); //
    // MD5(admin)
    return true;
    // ota->setHostname(Wireless::hostName().c_str());
    ota->setPort(port_);
    ota->begin(false);
    return true;
}

void OTAUpdate::onStop() {
    delete ota;
}

void OTAUpdate::handleArduinoOTAClassOnStart(void) {
    PrintUtils::print_ident(out_, FPSTR(str_update));
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
    PrintUtils::print(out_, FPSTR(str_update), FPSTR(strP));
    PrintUtils::println(out_);
}

void OTAUpdate::handleEnd() {
    PrintUtils::print_ident(out_, FPSTR(str_update));
    PrintUtils::print(out_, FPSTR(str_complete));
    PrintUtils::println(out_);
}

void OTAUpdate::handleProgress(unsigned int progress, unsigned int total) {
    if (progress % 5 == 0) {
        PrintUtils::print_ident(out_, FPSTR(str_update));
        PrintUtils::print(out_, FPSTR(strf_progress), progress, '%');
        PrintUtils::println(out_);
    }
}

void OTAUpdate::handleError(ota_error_t error) {
    PrintUtils::print_ident(out_, FPSTR(str_update));
    PGM_P strP = str_unknown;
    switch (error) {
        case OTA_AUTH_ERROR:
            strP = str_auth;
            break;
        case OTA_BEGIN_ERROR:
            strP = str_begin;
            break;
        case OTA_CONNECT_ERROR:
            strP = str_connect;
            break;
        case OTA_RECEIVE_ERROR:
            strP = str_receive;
            break;
        case OTA_END_ERROR:
            strP = str_end;
            break;
        default:
            break;
    }
    PrintUtils::print(out_, FPSTR(strP), FPSTR(str_error));
    PrintUtils::println(out_);
}

void OTAUpdate::onLoop() { ota->handle(); }

}