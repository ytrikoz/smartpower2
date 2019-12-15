#pragma once

#include <ArduinoOTA.h>
#include <FS.h>
#include <Updater.h>

#include "Module.h"
#include "Strings.h"

class OTAUpdate : public Module {
   public:
    OTAUpdate(uint16_t port);
    
   protected:
    bool isCompatible(NetworkMode value) override { return value != NetworkMode::NETWORK_OFF; }
    bool isNetworkDepended() override { return true; }
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
