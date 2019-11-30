#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <user_interface.h>

extern "C" {
class BootWatcher {
  public:
    BootWatcher();
    void init();
    bool lastBootError();
    bool isSafeBootMode();
    void setBootFlag();
    bool getBootFlag();

  private:
    void clearEEPROM();
    void readEEPROM();

  private:
    EEPROMClass *eeprom;
    rst_info *info;
    uint8_t bootMode;
    bool safeBoot;
};
}