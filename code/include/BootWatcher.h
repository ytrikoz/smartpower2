#pragma once

#include "Consts.h"
#include "PrintUtils.h"
#include "FSUtils.h"

#include "Hardware/ina231.h"
#include "Hardware/mcp4652.h"

enum BootMode { BT_STOP,
                BT_SAFE,
                BT_NORMAL };

enum FSState { FS_ERROR,
               FS_EMPTY,
               FS_NORMAL };

enum CrashRepState { CR_DISABLED,
                     CR_ENABLED,
                     CR_FULL };

class BootWatcher {
   public:
    BootWatcher() {}

    void init() {
        initSerial();

        initWire();

        initFS();

        switch (fs_) {
            case FS_NORMAL:
                mode_ = SPIFFS.exists(BOOT_FLAG) ? BT_SAFE : BT_NORMAL;
                break;
            case FS_EMPTY:
                mode_ = BT_NORMAL;
                break;
            case FS_ERROR:
                mode_ = BT_STOP;
                break;
        }
        SPIFFS.remove(BOOT_FLAG);        
    }

    bool isSafeMode() { return mode_ == BT_SAFE; }

    BootMode getMode() { return mode_; }

    void start() {
        File f = SPIFFS.open(BOOT_FLAG, "w");
        f.println(APP_VERSION);
        f.flush();
        f.close();

        PrintUtils::println(&Serial, FPSTR(str_boot), getBootModeStr(mode_));
    }

    bool end() {
        SPIFFS.remove(BOOT_FLAG);
        return true;
    }

   private:

    void initSerial() {
        Serial.begin(115200);
        Serial.println();
    }

    void initWire() {
        PrintUtils::print_ident(&Serial, FPSTR(str_wire));
        Wire.begin(I2C_SDA, I2C_SCL);
        PrintUtils::println(&Serial, FPSTR(str_done));
    }

    void initFS() {
        fs_ = FS_ERROR;
        PrintUtils::print_ident(&Serial, FPSTR(str_spiffs));
        if (SPIFFS.begin()) {
            fs_ = FS_NORMAL;
        } else {
            PrintUtils::print(&Serial, FPSTR(str_format));
            if (FSUtils::formatFS()) {
                fs_ = FS_EMPTY;
            } else {
                PrintUtils::print(&Serial, FPSTR(str_error));
            }
        }
        PrintUtils::println(&Serial, FPSTR(str_done));
    }

    String getBootModeStr(BootMode mode) const {
        String str;
        switch (mode_) {
            case BT_STOP:
                str = F("FATAL ERROR");
                break;
            case BT_SAFE:
                str = F("SAFE MODE");
                break;             
            case BT_NORMAL:
                str = F("COMPLETE");                   
            default:
                break;
        }
        return str;
    }

   private:
    BootMode mode_;
    FSState fs_;
    CrashRepState report_;
};
