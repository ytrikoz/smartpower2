#pragma once

#include "Consts.h"
#include "Utils/PrintUtils.h"
#include "Utils/FSUtils.h"

enum BootMode { BT_ERROR,
                BT_SAFE,
                BT_NORMAL };

enum RestartReasonClass { REASON_NORMAL,
                         REASON_FAILURE };

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

    bool init() {
        initSerial();

        PrintUtils::print_ident(out_, FPSTR(str_wire));
        initWire();
        PrintUtils::println(out_, FPSTR(str_done));

        fs_ = initFS();
        switch (fs_) {
            case FS_NORMAL:
                mode_ = SPIFFS.exists(BOOT_FLAG) && getRestartReason() == REASON_FAILURE ? BT_SAFE : BT_NORMAL;
                break;
            case FS_EMPTY:
                mode_ = BT_NORMAL;
                break;
            case FS_ERROR:
                mode_ = BT_ERROR;
                break;
        }
        return mode_ == BT_NORMAL;
    }

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
    RestartReasonClass getRestartReason() {
        rst_info* info = ESP.getResetInfoPtr();
        RestartReasonClass res = REASON_FAILURE;
        switch (info->reason) {
            // normal startup by power on
            case REASON_DEFAULT_RST:
            // software restart ,system_restart , GPIO status won’t change
            case REASON_SOFT_RESTART:
            // wake up from deep-sleep
            case REASON_DEEP_SLEEP_AWAKE:
            //  external system reset
            case REASON_EXT_SYS_RST:
                res = RestartReasonClass::REASON_NORMAL;
                break;
            // hardware watch dog reset
            case REASON_WDT_RST:
            // exception reset, GPIO status won’t change
            case REASON_EXCEPTION_RST:
            // software watch dog reset, GPIO status won’t change
            case REASON_SOFT_WDT_RST:
                res =  RestartReasonClass::REASON_FAILURE;
                break;
        }
        PrintUtils::print_ident(out_, FPSTR(str_boot));
        PrintUtils::print(out_, FPSTR(str_restart), ESP.getResetReason());
        PrintUtils::println(out_);
        return res;
    }

    void setBootFlag() {
        File f = SPIFFS.open(BOOT_FLAG, "w");
        f.println(APP_VERSION __DATE__ __TIME__);
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