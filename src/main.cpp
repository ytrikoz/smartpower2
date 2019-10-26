#include "main.h"

#include "App.h"

void setup() {
    // Setup serial
    USE_SERIAL.begin(115200);
    USE_SERIAL.println();
#ifdef SERIAL_DEBUG
    USE_SERIAL.setDebugOutput(true);
#endif
    // Try pushing frequency to 160MHz.
    // system_update_cpu_freq(SYS_CPU_160MHZ);
    system_update_cpu_freq(SYS_CPU_80MHZ);
    app.init(&USE_SERIAL);
    app.start();
}

void loop() { app.loop(); }
