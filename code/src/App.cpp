#include "App.h"

#include "Cli/Cli.h"
#include "Powerlog.h"
#include "LoopTiming.h"

using namespace PrintUtils;
using namespace StrUtils;

App::App() : exitState_(STATE_NORMAL), exitFlag_(false), systemEvent_(true), networkEvent_(false), psuEvent_(false) {}

void App::initMods() {
    for (uint8_t i = 0; i < MODULES_COUNT; ++i) {
        Module *obj = getInstance(i);
        obj->setOutput(out_);
        obj->setConfig(config_->get());
        if (!obj->init()) {
            PrintUtils::print_ident(out_, getName(i));
            PrintUtils::print(out_, FPSTR(str_init), FPSTR(str_failed));
            PrintUtils::print(out_, obj->getError());
            PrintUtils::println(out_);
        }
    }
}

void App::setupMods() {
    /*
    * Clock
    */
    clock()->setOnChange([this](const time_t local, double diff) {
        PrintUtils::print_ident(out_, FPSTR(str_clock));
        PrintUtils::print(out_, FPSTR(str_updated), TimeUtils::format_elapsed_full(diff));
        PrintUtils::println(out_);
    });
    /*
    * Button
    */
    btn()->setOnClick([this]() { if (psu()) psu()->togglePower(); });
    btn()->setOnHold([this](time_t time) {
        if (time >= HOLD_TIME_RESTART_s && exitState_ < STATE_RESTART) {
            PrintUtils::print_ident(out_, FPSTR(str_app));
            PrintUtils::println(out_, FPSTR(str_restart));
            exitState_ = STATE_RESTART;
        }
        if (time >= HOLD_TIME_RESET_s && exitState_ < STATE_RESET) {
            PrintUtils::print_ident(out_, FPSTR(str_app));
            PrintUtils::println(out_, FPSTR(str_reset));
            exitState_ = STATE_RESET;
        }
        systemEvent_ = exitState_ != STATE_NORMAL;
    });
    btn()->setholdReleaseEvent([this](time_t time) {
        exitFlag_ = exitState_ != STATE_NORMAL;
    });
    /*
    * Power
    */
    psu()->setOnData(this);
    psu()->setOnStatusChange([this](const PsuStatus status, const String description) {
        PrintUtils::print_ident(out_, FPSTR(str_psu));
        PrintUtils::print(out_, description);
        PrintUtils::println(out_);
        onPsuStatusChange(status);
    });
    psu()->setOnStateChange([this](const PsuState state, const String description) {
        PrintUtils::print_ident(out_, FPSTR(str_psu));
        PrintUtils::print(out_, description);
        PrintUtils::println(out_);
        onPsuStateChange(state);
    });
    /*
    * Telnet
    */
    telnet()->setEventHandler([this](TelnetEventType et, WiFiClient *client) {
        PrintUtils::print_ident(out_, FPSTR(str_telnet));
        bool conn = (et == CLIENT_CONNECTED);
        if (conn) {
            PrintUtils::print(out_, prettyIp(client->remoteIP(), client->remotePort()));
        } else {
            PrintUtils::print(out_, FPSTR(str_disconnected));
        }
        PrintUtils::println(out_);
        onTelnetStatusChange(conn);
    });

    /*
    * Syslog
    */
    syslog()->setSource(out_);
};

void App::systemRestart() {
    exitState_ = STATE_RESTART;
    exitFlag_ = true;
}

void App::systemReset() {
    exitState_ = STATE_RESET;
    exitFlag_ = true;
}

void App::onPsuStateChange(PsuState state) {
    psuEvent_ = true;
}

void App::onPsuStatusChange(PsuStatus status) {
    psuEvent_ = true;
}

void App::onPsuData(PsuData &item) {
    if (display()) display()->refresh();

    if (powerlog_) powerlog_->onPsuData(item);

    if (console() && !console()->isOpen()) {
        char buf[32];
        memset(buf, 0, 32);
        size_t size = item.toPretty(buf);
        buf[size] = '\r';
        buf[size + 1] = '\x00';
        console()->sendData(buf, size + 1);
    }

    if (hasNetwork_) {
        if (telnet() && telnet()->hasClient() && !telnet()->isShellActive()) {
            String data = item.toJson();
            data += '\r';
            telnet()->sendData(data);
        }
        if (web()->getClients()) {
            String data = item.toJson();
            web()->broadcast(data, PAGE_HOME);
        }
    }
}

void App::onConfigChange(const ConfigItem param, const String &value) {
    for (uint8_t i = 0; i < MODULES_COUNT; ++i) {
        Module *obj = getInstance(i);
        if (obj) obj->configChange(param, value);
    }
}

void App::onTelnetStatusChange(bool clients) {
    networkEvent_ = true;
    telnetClients_ = clients;
}

void App::onWebStatusChange(bool clients) {
    networkEvent_ = true;
    webClients_ = clients;
}

void App::onNetworkStatusChange(bool network, NetworkMode mode) {
    networkEvent_ = true;
    hasNetwork_ = network;
    networkMode_ = mode;
    if (!network) webClients_ = telnetClients_ = 0;
}

void App::begin() {
    initMods();
    displayProgress(0, BUILD_DATE);
    setupMods();
    displayProgress(80, "<INIT>");
    displayProgress(100, "<COMPLETE>");
    display()->refresh();
}

void App::startSafe() {
    start(MOD_CONSOLE);
    console()->init();
}

void App::loopSafe() { console()->loop(); }

AppState App::loop(LoopTimer *looper) {
    for (size_t i = 0; i < MODULES_COUNT; ++i) {
        auto *obj = getInstance(i);
        if (!obj) continue;
        if (exitFlag_) {
            obj->end();
            continue;
        }
        if ((!hasNetwork_ && modules_[i].network) ||
            (hasNetwork_ && networkMode_ == NetworkMode::NETWORK_AP && modules_[i].network == NetworkMode::NETWORK_STA)) {
            obj->stop();
            continue;
        } else {
            if (obj->start()) {
                if (looper) looper->start(i);
                obj->loop();
                if (looper) looper->end();
            }
            delay(0);
        }
    }

    if (psuEvent_) {
        web()->updatePage(PAGE_HOME);
        display()->refresh();
    }

    if (systemEvent_ || networkEvent_)
        refreshBlue();

    if (systemEvent_ || psuEvent_)
        refreshRed();

    networkEvent_ = systemEvent_ = psuEvent_ = false;

    if (exitFlag_)
        return exitState_;

    return STATE_NORMAL;
}

size_t App::printDiag(Print *p) {
    DynamicJsonDocument doc(2048);
    doc[FPSTR(str_heap)] = SysInfo::getHeapStats();
    doc[FPSTR(str_file)] = FSUtils::getFSUsed();
    doc[FPSTR(str_wifi)] = NetUtils::getMode();
    doc[FPSTR(str_network)] = hasNetwork_;

    for (uint8_t i = 0; i < MODULES_COUNT; ++i) {
        auto *obj = getInstance(i);
        JsonVariant mod_root = doc.createNestedObject(getName(i));
        if (obj) {
            mod_root[FPSTR(str_state)] = (uint8_t)obj->getNoduleState();
            if (obj->failed()) {
                mod_root[FPSTR(str_error)] = obj->getError().toString();
            }
            obj->onDiag(mod_root);
        } else {
            mod_root[FPSTR(str_state)] = FPSTR(str_error);
        }
    }
    size_t n = serializeJsonPretty(doc, *p);
    return n += p->println();
}

void App::displayProgress(uint8_t progress, const char *message) {
    if (display() && (display()->isEnabled()))
        display()->showProgress(progress, message);
}

void App::refreshBlue() {
    LedSignal mode = LIGHT_OFF;
    if (systemEvent_ && exitState_ >= STATE_RESTART) {
        mode = BLINK_ERROR;
    } else if (hasNetwork_) {
        mode = LIGHT_ON;
        if (webClients_ || telnetClients_)
            mode = BLINK;
    }
    led()->set(BLUE_LED, mode);
}

void App::refreshRed() {
    LedSignal mode = LIGHT_ON;
    if (systemEvent_ && exitState_ >= STATE_RESET) {
        mode = BLINK_ERROR;
    } else if (psu()->getStatus() == PSU_OK) {
        mode = psu()->getState() == POWER_ON ? BLINK : LIGHT_ON;
    } else {
        mode = BLINK_ALERT;
    }
    led()->set(RED_LED, mode);
}

void App::setModules(ModuleDef *objs) {
    modules_ = objs;
}

void App::setPowerlog(PowerLog *powerlog) {
    powerlog_ = powerlog;
}

void App::setConfig(ConfigHelper *config) {
    config_ = config;
    config_->get()->setOnChange(
        [this](ConfigItem param, const String &value) {
            PrintUtils::print_ident(out_, FPSTR(str_config));
            PrintUtils::print(out_, config_->get()->name(param), value);
            PrintUtils::println(out_);
            onConfigChange(param, value);
        });
}

void App::setWireless(Wireless *obj) {
    wireless_ = obj;
    wireless_->setOnStatusChange(
        [this](bool network, unsigned long time) {
            PrintUtils::print_ident(out_, FPSTR(str_app));
            PrintUtils::println(out_, NetUtils::getStatusStr(network));
            onNetworkStatusChange(network, wireless_->getMode());
        });
}

String App::getName(uint8_t index) const {
    char buf[16];
    PGM_P strP = modules_[index].name;
    strncpy_P(buf, strP, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\x00';
    return String(buf);
}

bool App::getByName(const char *str, ModuleEnum &module) const {
    for (uint8_t i = 0; i < MODULES_COUNT; ++i) {
        if (getName(i).equalsIgnoreCase(str)) {
            module = ModuleEnum(i);
            return true;
        }
    }
    return false;
}

Module *App::getInstance(uint8_t index) const {
    return modules_[index].obj;
}

Module *App::getInstanceByName(const String &name) {
    Module *res = nullptr;
    ModuleEnum mod;
    if (getByName(name.c_str(), mod)) {
        res = getInstance(mod);
    }
    return res;
}

Modules::Display *App::display() {
    return (Modules::Display *)modules_[MOD_DISPLAY].obj;
}

Modules::Button *App::btn() {
    return (Modules::Button *)modules_[MOD_BTN].obj;
}

Modules::Clock *App::clock() {
    return (Modules::Clock *)modules_[MOD_CLOCK].obj;
}

Modules::Led *App::led() {
    return (Modules::Led *)modules_[MOD_LED].obj;
}

Modules::Console *App::console() {
    return (Modules::Console *)modules_[MOD_CONSOLE].obj;
}

Modules::Telnet *App::telnet() {
    return (Modules::Telnet *)modules_[MOD_TELNET].obj;
}

Modules::Web *App::web() {
    return (Modules::Web *)modules_[MOD_WEB].obj;
}

Modules::Psu *App::psu() {
    return (Modules::Psu *)modules_[MOD_PSU].obj;
}

Modules::Syslog *App::syslog() {
    return (Modules::Syslog *)modules_[MOD_SYSLOG].obj;
}
