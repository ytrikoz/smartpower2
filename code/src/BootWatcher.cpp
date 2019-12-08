#include "BootWatcher.h"

#include "Consts.h"
#include "Esp.h"
#include <FS.h>

BootWatcher::BootWatcher() {
    eeprom = new EEPROMClass();
    bootMode = ESP.getBootMode();
    info = ESP.getResetInfoPtr();
}

bool BootWatcher::isSafeBootMode() { return safeBoot; }

void BootWatcher::setBootFlag() {}

bool BootWatcher::getBootFlag() {
    bool res = SPIFFS.exists(FS_START_FLAG);
    return res;
    // void init() { eeprom = new EEPROMClass(); }
    // bool lastBootError();
}

bool BootWatcher::lastBootError() { return getBootFlag(); }

void BootWatcher::clearEEPROM() {
    eeprom->begin(512);
    for (int i = 0; i < 512; i++)
        eeprom->write(i, 0);
    eeprom->end();

    if (eeprom->commit()) {
        Serial.println("EEPROM successfully committed");
    } else {
        Serial.println("ERROR! EEPROM commit failed");
    }
}

void BootWatcher::readEEPROM() {
    eeprom->begin(512);
    int address = 0;
    byte value;
    value = eeprom->read(address);

    Serial.print(address);
    Serial.print("\t");
    Serial.print(value, DEC);
    Serial.println();

    // advance to the next address of the EEPROM
    address = address + 1;

    // there are only 512 bytes of EEPROM, from 0 to 511, so if we're
    // on address 512, wrap around to address 0
    if (address == 512) {
        address = 0;
    }
}
/*
eeprom->begin(size) before you start reading or writing, between 4 and 4096
bytes.
eeprom->write does not write to flash immediately,
eeprom->commit() whenever you wish to save changes to flash.
eeprom->end() will also commit, and will release the RAM copy of EEPROM
contents.
EEPROM library uses one sector of flash located just after the SPIFFS.
*/