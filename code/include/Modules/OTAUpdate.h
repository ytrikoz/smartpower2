#pragma once

#include <ArduinoOTA.h>
#include <FS.h>
#include <Updater.h>

#include "Core/Module.h"
#include "Strings.h"

namespace Modules {

class OTAUpdate : public Module {
   public:
    OTAUpdate(uint16_t port);
    
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

}