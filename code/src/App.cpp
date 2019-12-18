#include "App.h"

#include "Cli/Cli.h"
#include "CrashReport.h"
#include "PsuLogHelper.h"
#include "LoopTiming.h"

using namespace PrintUtils;
using namespace StrUtils;

App::App() : networkChanged_(true), restartFlag_(false), restartCountdown_(0) {}

String App::getName(uint8_t index) const {
    char buf[16];
    PGM_P strP = define[index].name;
    strncpy_P(buf, strP, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\x00';
    return String(buf);
}

bool App::getByName(const char *str, ModuleEnum &module) const {
    for (uint8_t i = 0; i < APP_MODULES; ++i) {
        if (getName(i).equalsIgnoreCase(str)) {
            module = ModuleEnum(i);
            return true;
        }
    }
    return false;
}

Module *App::getInstance(uint8_t index) const {
    return define[index].obj;
}

Module *App::getInstanceByName(const String &name) {
    Module *res = nullptr;
    ModuleEnum mod;
    if (getByName(name.c_str(), mod)) {
        res = getInstance(mod);
    }
    return res;
}

void App::instanceMods() {
    define[MOD_BTN].obj = new Modules::Button(POWER_BTN_PIN);
    define[MOD_CLOCK].obj = new Modules::Clock();
    define[MOD_WEB].obj = new Modules::Web();
    define[MOD_DISPLAY].obj = new Modules ::Display();
    define[MOD_LED].obj = new Modules ::Led();
    define[MOD_NETSVC].obj = new NetworkService();
    define[MOD_PSU].obj = new Modules::Psu(this);
    define[MOD_SHELL].obj = new Modules ::Shell();
    define[MOD_TELNET].obj = new Modules ::Telnet(TELNET_PORT);
    define[MOD_UPDATE].obj = new Modules::OTAUpdate(OTA_PORT);
    define[MOD_SYSLOG].obj = new Modules ::Syslog(SYSLOG_PORT);
}

void App::initMods() {
    for (uint8_t i = 0; i < APP_MODULES; ++i) {
        Module *obj = getInstance(i);
        obj->setOutput(out_);
        obj->setConfig(params());
        if (!obj->init()) {
            PrintUtils::print(out_, FPSTR(str_failed), getName(i));
            PrintUtils::println(out_);
        }
    }
}

void App::setupMods() {
    clock()->setOnChange([this](const time_t local, double diff) {
        PrintUtils::print_ident(out_, FPSTR(str_clock));
        PrintUtils::print(out_, FPSTR(str_updated), TimeUtils::format_elapsed_full(diff));
        PrintUtils::println(out_);
    });

    btn()->setOnClick([this]() { if (psu()) psu()->togglePower(); });
    btn()->setOnHold([this](time_t time) {if (time >= HOLD_TIME_TO_RESET_s) restart(3); });

    psu()->setOnData([this](PsuData data) {if (display()) display()->refresh(); });
    psu()->setOnStateChange([this](PsuState state, PsuStatus status) {
        web()->sendPageState(PG_HOME);
        switch (status) {
            case PSU_OK:
                led()->set(RED_LED, psu()->checkState(POWER_ON) ? BLINK : LIGHT_ON);
                break;
            case PSU_ALERT:
                led()->set(RED_LED, BLINK_ALERT);
                break;
            case PSU_ERROR:
                led()->set(RED_LED, BLINK_ERROR);
        }
        display()->refresh();
    });

    telnet()->setEventHandler([this](TelnetEventType et, WiFiClient *client) {
        PrintUtils::print_ident(out_, FPSTR(str_telnet));
        switch (et) {
            case CLIENT_CONNECTED:
                PrintUtils::print(out_, FPSTR(str_connected), prettyIpAddress(client->remoteIP(), client->remotePort()));
                break;
            case CLIENT_DISCONNECTED:
                PrintUtils::print(out_, FPSTR(str_disconnected));
                break;
        }
        PrintUtils::println(out_);
        refresh_wifi_led();
    });
}

void App::onPsuData(PsuData &item) {
    if(powerlog_) powerlog_->onPsuData(item);

    if (hasNetwork_) {
        if (telnet() && telnet()->hasClient()) {
            String data = item.toString();
            data += '\r';
            telnet()->sendData(data.c_str());
        }
        if (web()->getClients()) {
            String data = String(TAG_PVI);
            data += item.toString();
            web()->sendToClients(data, PG_HOME);
        }
    }
}

void App::setPowerlog(PsuLogHelper *powerlog) {   
    powerlog_ = powerlog;
}

void App::setConfig(ConfigHelper *config) {
    config_ = config;

    config_->get()->setOnChange(
        [this](ConfigItem param, const char *value) {
            onConfigParamChange(param, value);
        });
}

void App::onConfigParamChange(ConfigItem param, const char *value) {
    PrintUtils::print_ident(out_, FPSTR(str_config));
    PrintUtils::print(out_, param, value);
    PrintUtils::println(out_);
}

void App::begin() {
    instanceMods();
    initMods();
    setupMods();
    displayProgress(0, BUILD_DATE);

    displayProgress(40, "<WIFI>");

    Wireless::start();

    displayProgress(80, "<INIT>");

    Wireless::setOnNetworkStatusChange(
        [this](bool network, unsigned long time) {
            PrintUtils::print_ident(out_, FPSTR(str_app));
            out_->printf("network %s (%.2f sec)\n", getUpDownStr(network).c_str(), (float)time / ONE_SECOND_ms);
            networkMode_ = Wireless::getMode();
            hasNetwork_ = network;
            networkChanged_ = true;
        });

    displayProgress(100, "<COMPLETE>");

    display()->refresh();
}

void App::startSafe() {
    start(MOD_SHELL);
    shell()->init();
}

void App::loopSafe() { shell()->loop(); }

void App::loop(LoopTimer *looptimer) {   
    for (size_t i = 0; i < APP_MODULES; ++i) {
        auto *obj = getInstance(i);
        if (obj == nullptr) continue;
        if ((!hasNetwork_ && define[i].network_mode) || (hasNetwork_ && networkMode_ < define[i].network_mode)) {
            obj->stop();
            continue;
        } else {
            if (obj->start()) {
                if (looptimer) auto timer = looptimer->start(i);
                obj->loop();
            }
            delay(0);
        }
    }
    if (networkChanged_) refresh_wifi_led();
    handleRestart();

    networkChanged_ = false;
}

size_t App::printDiag(Print *p) {
    DynamicJsonDocument doc(2048);
    doc[FPSTR(str_heap)] = SysInfo::getHeapStats();
    doc[FPSTR(str_file)] = SysInfo::getFSStats();
    doc[FPSTR(str_wifi)] = getNetworkModeStr(networkMode_);
    doc[FPSTR(str_network)] = hasNetwork_;

    for (uint8_t i = 0; i < APP_MODULES; ++i) {
        auto *obj = getInstance(i);
        JsonVariant mod_diag = doc.createNestedObject(getName(i));
        if (obj) {
            mod_diag[FPSTR(str_state)] = obj->getStateStr();
            obj->onDiag(mod_diag);
        } else {
            mod_diag[FPSTR(str_error)] = FPSTR(str_error);
        }
    }
    size_t n = serializeJsonPretty(doc, *p);
    return n += p->println();
}

void App::restart(time_t time) {
    restartCountdown_ = time;
    restartFlag_ = true;
    restartUpdated_ = millis();
    if (restartCountdown_) {
        char buf[32];
        sprintf_P(buf, msg_restart_countdown, restartCountdown_);
        PrintUtils::print(out_, buf);
    }
}

void App::handleRestart() {
    if (!restartFlag_) return;

    unsigned long now = millis();
    if (millis_passed(restartUpdated_, now) >= ONE_SECOND_ms) {
        restartUpdated_ = now;
        restartCountdown_--;
        if (restartCountdown_ <= 3) {
            refresh_power_led();
            refresh_wifi_led();
        }
        if (restartCountdown_ == 0)
            system_restart();
    }
}

void App::displayProgress(uint8_t progress, const char *message) {
    if (display()->isEnabled()) display()->showProgress(progress, message);
}

bool App::setOutputVoltageAsDefault() {
    float v = psu()->getVoltage();
    if (config_->setOutputVoltage(v))
        return config_->save();
    return false;
}

bool App::setBootPowerState(BootPowerState value) {
    bool res = false;
    if (config_->setBootPowerState(value))
        res = config_->save();
    return res;
}

// NetowrkActivity web()->getClients() || (telnet() && telnet()->hasClient())
// Restart process restartFlag_ && restartCountdown_ <= 3
void App::refresh_wifi_led() {
    LedSignal mode = LIGHT_OFF;
    if (restartFlag_ && restartCountdown_ <= 3)
        mode = BLINK_ERROR;
    else if (hasNetwork_) {
        mode = LIGHT_ON;
        if (hasNetworkActivty_)
            mode = BLINK;
    }
    led()->set(BLUE_LED, mode);
}

void App::refresh_power_led() {
    LedSignal mode = LIGHT_ON;
    if (restartFlag_ && restartCountdown_ <= 3)
        mode = BLINK_ERROR;
    else if (psu()) {
        if (psu()->checkStatus(PSU_OK))
            mode = psu()->checkState(POWER_ON) ? BLINK : LIGHT_ON;
        else
            mode = BLINK_ALERT;
    }
    led()->set(RED_LED, mode);
}

void App::printPlot(PlotData *data, Print *p) {
    char tmp[PLOT_ROWS * 8 + 1];
    for (uint8_t x = 0; x < data->size; ++x) {
        uint8_t y = compress(data, x);
        strfill(tmp, '*', y);
        p->printf("#%d %2.4f ", x + 1, data->columns[x]);
        p->print(tmp);
        p->println();
    }
    // if (state == POWER_OFF) {
    //     size_t size = constrain(
    //         logger->getSize(PsuLogEnum::VOLTAGE), 0, 1024);
    //     if (size > 0) {
    //         PlotData data;
    //         float tmp[size];
    //         logger->getValues(PsuLogEnum::VOLTAGE, tmp, size);
    //         size_t cols = group(&data, tmp, size);
    //         display->showPlot(&data, cols);
    //     };
    // }
}

Modules::Display *App::display() {
    return (Modules::Display *)define[MOD_DISPLAY].obj;
}

Modules::Button *App::btn() {
    return (Modules::Button *)define[MOD_BTN].obj;
}

Modules::Clock *App::clock() {
    return (Modules::Clock *)define[MOD_CLOCK].obj;
}

Modules::Led *App::led() {
    return (Modules::Led *)define[MOD_LED].obj;
}

Modules::Shell *App::shell() {
    return (Modules::Shell *)define[MOD_SHELL].obj;
}

Modules::Telnet *App::telnet() {
    return (Modules::Telnet *)define[MOD_TELNET].obj;
}

Modules::Web *App::web() {
    return (Modules::Web *)define[MOD_WEB].obj;
}

Modules::Psu *App::psu() {
    return (Modules::Psu *)define[MOD_PSU].obj;
}
