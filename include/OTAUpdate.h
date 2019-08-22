#pragma once

#include <ArduinoOTA.h>
#include <Updater.h>
#include <FS.h>

#include "Strings.h"

class OTAUpdate {
   public:
    OTAUpdate();
    void setOutput(Print *p);
    bool begin(const char *hostname, uint16_t port = OTA_PORT);
    void loop();
    void end();

   private:
    ArduinoOTAClass *ota;
    Print *output;
    bool active;
    void onStart(void);
    void onProgress(unsigned int progress, unsigned int total);
    void onEnd();
    void onError(ota_error_t error);
    unsigned int total_progress;
};
