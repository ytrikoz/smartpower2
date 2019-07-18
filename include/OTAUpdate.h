#pragma once
#include <ArduinoOTA.h>
#include <FS.h>

#include "consts.h"

class OTAUpdate {
   public:
    OTAUpdate();
    void setOutput(Print *p);
    bool begin(const char *hostname, uint16_t port = 8266);
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
};
