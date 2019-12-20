#pragma once

#include "Consts.h"
#include "Utils/PrintUtils.h"
#include "Utils/FSUtils.h"

enum BootMode { BT_ERROR,
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

    void setOutput(Print* p) {
        out_ = p;
    }

    void init() {
        initSerial();

        PrintUtils::print_ident(out_, FPSTR(str_wire));
        initWire();
        PrintUtils::println(out_, FPSTR(str_done));

        fs_ = initFS();
        switch (fs_) {
            case FS_NORMAL:
                mode_ = SPIFFS.exists(BOOT_FLAG) ? BT_SAFE : BT_NORMAL;
                break;
            case FS_EMPTY:
                mode_ = BT_NORMAL;
                break;
            case FS_ERROR:
                mode_ = BT_ERROR;
                break;
        }
    }

    bool isSafeMode() { return mode_ == BT_SAFE; }

    BootMode getMode() { return mode_; }

    void start() {
        PrintUtils::print_ident(out_, FPSTR(str_boot));
        PrintUtils::print(out_, getBootModeStr(mode_));
        PrintUtils::println(out_);
        setBootFlag();
    }

    void end() {
        removeBootFlag();
    }

   private:
    void setBootFlag() {
        File f = SPIFFS.open(BOOT_FLAG, "w");
        f.println(APP_VERSION);
        f.flush();
        f.close();
    }

    void removeBootFlag() {
        SPIFFS.remove(BOOT_FLAG);
    }

    void initSerial() {
        Serial.begin(115200);
        Serial.println();
    }

    void initWire() {
        Wire.begin(I2C_SDA, I2C_SCL);
    }

    FSState initFS() {
        FSState res = FS_ERROR;
        PrintUtils::print_ident(out_, FPSTR(str_spiffs));
        if (SPIFFS.begin()) {
            res = FS_NORMAL;
        } else {
            PrintUtils::print(out_, FPSTR(str_format));
            if (FSUtils::formatFS()) {
                res = FS_EMPTY;
            } else {
                PrintUtils::print(out_, FPSTR(str_error));
            }
        }
        out_->println(FPSTR(str_done));
        return res;
    }

    String getBootModeStr(BootMode mode) const {
        String str;
        switch (mode_) {
            case BT_ERROR:
                str = F("ERROR");
                break;
            case BT_SAFE:
                str = F("SAFE");
                break;
            case BT_NORMAL:
                str = F("NORMAL");
            default:
                break;
        }
        return str;
    }

   private:
    Print* out_;
    BootMode mode_;
    FSState fs_;
    CrashRepState report_;
};