#pragma once

#include <ArduinoOTA.h>
#include <Updater.h>
#include <FS.h>

#include "AppModule.h"
#include "Strings.h"

class OTAUpdate : public AppModule {
   public:
    OTAUpdate();
    bool begin();    
    void loop();
   private:
    void handleStart(void);
    void handleProgress(unsigned int progress, unsigned int total);
    void handleEnd();
    void handleError(ota_error_t error);
    private:
    ArduinoOTAClass *ota;
    bool active;
    unsigned int update_progress;
};
