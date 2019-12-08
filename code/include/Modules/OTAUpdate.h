#pragma once

#include <ArduinoOTA.h>
#include <FS.h>
#include <Updater.h>

#include "AppModule.h"
#include "Strings.h"

class OTAUpdate : public AppModule {
   public:
    bool isCompatible(Wireless::NetworkMode value) override {
        return value != Wireless::NETWORK_OFF;
    }

    bool isNetworkDepended() override { return true; }

   public:
    OTAUpdate() : AppModule(MOD_UPDATE) {
        port_ = OTA_PORT;
    };

   protected:
    bool onInit() override;
    bool onStart() override;
    void onStop() override;
    void onLoop() override;

   private:
    void handleArduinoOTAClassOnStart(void);
    void handleProgress(unsigned int progress, unsigned int total);
    void handleEnd();
    void handleError(ota_error_t error);

   private:
    uint16_t port_;
    String host_;
    ArduinoOTAClass *ota;
    unsigned int update_progress;
};
