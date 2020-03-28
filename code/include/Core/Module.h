#pragma once

#include "Config.h"
#include "Wireless.h"

#include "Utils/ModuleUtils.h"
#include "Utils/PrintUtils.h"
#include "Utils/StrUtils.h"
#include "Utils/FSUtils.h"

class Module {
   public:
    Module() : modState_(ModuleState::STATE_INIT) {}

    void setOutput(Print *p) { out_ = p; }

    void setConfig(Config *c) { config_ = c; }

    bool configChange(const ConfigItem param, const String &value) {
        return onConfigChange(param, value);
    }

    bool getScreenItem(String **item, size_t &count) {
        item = nullptr;
        count = 0;
        return false;
    }

    bool init(bool force = false) {
        if (modState_ > ModuleState::STATE_INIT_COMPLETE) return true;
        if (modState_ == ModuleState::STATE_INIT_FAILED && !force) return false;

        modState_ = onInit() ? STATE_INIT_COMPLETE : ModuleState::STATE_INIT_FAILED;
        return modState_ == ModuleState::STATE_INIT_COMPLETE;
    }

    bool start(bool force = false) {
        if (modState_ == ModuleState::STATE_ACTIVE) return true;
        if (modState_ == ModuleState::STATE_START_FAILED && !force) return false;

        if (modState_ < ModuleState::STATE_INIT_COMPLETE) {
            if (!init(force)) return false;
        }
        modState_ = onStart() ? STATE_ACTIVE : ModuleState::STATE_START_FAILED;
        return modState_ == ModuleState::STATE_ACTIVE;
    }

    void stop() {
        if (modState_ < ModuleState::STATE_ACTIVE) return;
        onStop();
        modState_ = ModuleState::STATE_INIT_COMPLETE;
    };

    void end() {
        if (modState_ < ModuleState::STATE_INIT_FAILED) return;
        onEnd();
        modState_ = ModuleState::STATE_INIT;
    };

    void loop() {
        if (modState_ == ModuleState::STATE_ACTIVE || start()) onLoop();
    };

    size_t printDiag(void) {
        return printDiag(out_);
    };

    size_t printDiag(Print *p) {
        DynamicJsonDocument doc(512);
        doc[FPSTR(str_state)] = (uint8_t)modState_;
        if (failed()) {
            doc[FPSTR(str_error)] = modError_.toString().c_str();
        }
        JsonVariant obj = doc.as<JsonObject>();
        onDiag(obj);
        return serializeJson(doc, *p);
    };

    void clearError() {
        modError_ = Error::none();
    }

    Error getError() {
        return modError_;
    }

    ModuleState getNoduleState() {
        return modState_;
    }

    virtual void onDiag(const JsonObject &obj) {}

    virtual bool failed() const {
        return modError_.code() != 0;
    }

    virtual bool ok() const {
        return modError_.code() == 0;
    }

    virtual String getStateStr() {
        String str = String(FPSTR(str_ok));
        if (failed()) {
            str = getError().toString();
        }
        return str;
    }

   protected:
    virtual bool onConfigChange(const ConfigItem item, const String &value) {
        return true;
    }

    virtual bool onInit() { return true; };

    virtual void onEnd(){};

    virtual bool onStart() { return true; }

    virtual void onStop(){};

    virtual void onLoop() = 0;

   protected:
    void setError(Error e) {
        modError_ = e;
    }

    void setError(ErrorCode code) {
        setError(Error(code, ERROR_CRITICAL));
    }
    
    void setAlert(ErrorCode code) {
        setError(Error(code, ERROR_ALERT));
    }

    void setError(ErrorCode code, const char *desc) {
        setError(Error(code, desc));
    }
    
    void setErrorf_P(const ErrorCode code, PGM_P strf, const char *arg) {
        char buf[OUTPUT_MAX_LENGTH];
        sprintf_P(buf, strf, arg);
        setError(code, buf);
    }        

    void setSerializeError(const char *json) {
        setErrorf_P(ERROR_SERIALIZE, strf_json_serilialize, json);
    }

    void setInvalidParamError(const ConfigItem param) {
        setErrorf_P(INVALID_PARAM, strf_invalid_param, config_->name(param).c_str());
    }

   protected:
    Print *out_;
    Config *config_;

   private:
    Error modError_;
    ModuleState modState_;
};
