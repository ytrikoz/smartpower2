#include "App.h"

#include "Cli/Cli.h"
#include "Powerlog.h"
#include "LoopTiming.h"

using namespace PrintUtils;
using namespace StrUtils;

App::App() : networkEvent_(true), restartFlag_(false), restartCountdown_(0) {}

void App::instanceMods() {
    modules[MOD_LED].obj = new Modules ::Led();
    modules[MOD_BTN].obj = new Modules::Button(POWER_BTN_PIN);
    modules[MOD_CLOCK].obj = new Modules::Clock();
    modules[MOD_PSU].obj = new Modules::Psu();
    modules[MOD_DISPLAY].obj = new Modules ::Display();
    modules[MOD_CONSOLE].obj = new Modules ::Console();
    modules[MOD_NETSVC].obj = new NetworkService();
    modules[MOD_TELNET].obj = new Modules ::Telnet(TELNET_PORT);
    modules[MOD_UPDATE].obj = new Modules::OTAUpdate(OTA_PORT);
    modules[MOD_SYSLOG].obj = new Modules ::Syslog(SYSLOG_PORT);
    modules[MOD_WEB].obj = new Modules::Web();
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
    btn()->setOnHold([this](time_t time) {if (time >= HOLD_TIME_TO_RESET_s) restart(3); });
    /*
    * Power
    */
    psu()->setOnData(this);
    psu()->setOnStatusChange([this](PsuStatus status) {
        PrintUtils::print_ident(out_, FPSTR(str_power));
        PrintUtils::print(out_, getStatusStr(status));
        PrintUtils::println(out_);
        onPsuStatusChange(status);
    });
    psu()->setOnStateChange([this](PsuState state) {
        PrintUtils::print_ident(out_, FPSTR(str_power));
        PrintUtils::print(out_, getPsuStateStr(state));
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
            PrintUtils::print(out_, prettyIpAddress(client->remoteIP(), client->remotePort()));
        } else {
            PrintUtils::print(out_, FPSTR(str_disconnected));
        }
        PrintUtils::println(out_);
        onTelnetStatusChange(conn);
    });
}

void App::onPsuStateChange(PsuState state) {
    psuEvent_ = true;
    psuState_ = state;
    display()->refresh();
}

void App::onPsuStatusChange(PsuStatus status) {
    psuEvent_ = true;
    psuStatus_ = status;
    web()->sendPageState(PG_HOME);
    display()->refresh();
}

void App::onPsuData(PsuData &item) {
    if (display()) display()->refresh();

    if (powerlog_) powerlog_->onPsuData(item);

    if (console() && !console()->isOpen()) {
        String data = item.toString();
        data += '\r';
        console()->sendData(data);
    }

    if (hasNetwork_) {
        if (telnet() && telnet()->hasClient() && !telnet()->isShellActive()) {
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

void App::onConfigChange(ConfigItem param, const char *value) {
    PrintUtils::print_ident(out_, FPSTR(str_app));
    PrintUtils::print(out_, param, value);
    PrintUtils::println(out_);
}

void App::onTelnetStatusChange(bool clients) {
    networkEvent_ = true;
    telnetClients_ = clients;
}

void App::onWebStatusChange(bool clients) {
    networkEvent_ = true;
    webClients_ = clients;
}

void App::onNetworkStatusChange(bool connected, NetworkMode mode) {
    networkEvent_ = true;
    hasNetwork_ = connected;
    networkMode_ = mode;
    if (!connected) webClients_ = telnetClients_ = 0;
}

void App::begin() {
    instanceMods();
    initMods();

    displayProgress(0, BUILD_DATE);

    setupMods();
    displayProgress(40, "<WIFI>");

    Wireless::setOutput(out_);
    Wireless::start();

    displayProgress(80, "<INIT>");

    Wireless::setOnNetworkStatusChange(
        [this](bool network, unsigned long time) {
            PrintUtils::print_ident(out_, FPSTR(str_app));
            PrintUtils::print(out_, FPSTR(str_network), getUpDownStr(network));
            PrintUtils::print(out_, TimeUtils::format_elapsed_full((double)time / 1000));
            PrintUtils::println(out_);
            onNetworkStatusChange(network, Wireless::getMode());
        });

    displayProgress(100, "<COMPLETE>");

    display()->refresh();
}

void App::startSafe() {
    start(MOD_CONSOLE);
    console()->init();
}

void App::loopSafe() { console()->loop(); }

void App::loop(LoopTimer* looper) {
    if (restartFlag_) {
        unsigned long now = millis();
        if (millis_passed(restartUpdated_, now) >= ONE_SECOND_ms) {
            restartUpdated_ = now;
            if (restartCountdown_ == 3) {
                PrintUtils::print(out_, FPSTR(msg_restart));
                systemEvent_ = true;
            }
            restartCountdown_--; 
            if (restartCountdown_ == 0)
                system_restart();           
        }
    }

    for (size_t i = 0; i < APP_MODULES; ++i) {
        auto *obj = getInstance(i);
        if (!obj) {
            continue;
        }
        if ((!hasNetwork_ && modules[i].network) || (hasNetwork_ && networkMode_ < modules[i].network)) {
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

    if (networkEvent_ || systemEvent_) {
        refreshBlueLed();
    }

    if (psuEvent_ || systemEvent_)
        refreshRedLed();

    networkEvent_ = systemEvent_ = psuEvent_ = false;
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
            mod_diag[FPSTR(str_state)] = obj->getModuleStateStr();
            obj->onDiag(mod_diag);
        } else {
            mod_diag[FPSTR(str_error)] = FPSTR(str_error);
        }
    }
    size_t n = serializeJsonPretty(doc, *p);
    return n += p->println();
}

void App::restart(time_t time) {
    restartFlag_ = true;
    restartCountdown_ = time;
    restartUpdated_ = millis();
}

void App::displayProgress(uint8_t progress, const char *message) {
    if (display() && (display()->isEnabled()))
        display()->showProgress(progress, message);
}


void App::setOutputVoltage(float value) {
    config_->get()->setValueFloat(OUTPUT_VOLTAGE, value);
    psu()->setOutputVoltage(value);
}

bool App::setOutputVoltageAsDefault() {
    return config_->save();
}

bool App::setBootPowerState(BootPowerState value) {
    bool res = false;
    if (config_->setBootPowerState(value))
        res = config_->save();
    return res;
}

void App::refreshBlueLed() {
    LedSignal mode = LIGHT_OFF;
    if (restartFlag_) {
        mode = BLINK_ERROR;
    } else if (hasNetwork_) {
        mode = LIGHT_ON;
        if (webClients_ || telnetClients_)
            mode = BLINK;
    }
    led()->set(BLUE_LED, mode);
}

void App::refreshRedLed() {
    LedSignal mode = LIGHT_ON;
    if (restartFlag_) {
        mode = BLINK_ERROR;
    } else if (psuStatus_ == PSU_OK) {
        mode = psuState_ == POWER_ON ? BLINK : LIGHT_ON;
    } else {
        mode = BLINK_ALERT;
    }
    led()->set(RED_LED, mode);
}


void App::setPowerlog(PowerLog *powerlog) {
    powerlog_ = powerlog;
}

void App::setConfig(ConfigHelper *config) {
    config_ = config;
    config_->get()->setOnChange(
        [this](ConfigItem param, const char *value) {
            onConfigChange(param, value);
        });
}

String App::getName(uint8_t index) const {
    char buf[16];
    PGM_P strP = modules[index].name;
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
    return modules[index].obj;
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
    return (Modules::Display *)modules[MOD_DISPLAY].obj;
}

Modules::Button *App::btn() {
    return (Modules::Button *)modules[MOD_BTN].obj;
}

Modules::Clock *App::clock() {
    return (Modules::Clock *)modules[MOD_CLOCK].obj;
}

Modules::Led *App::led() {
    return (Modules::Led *)modules[MOD_LED].obj;
}

Modules::Console *App::console() {
    return (Modules::Console *)modules[MOD_CONSOLE].obj;
}

Modules::Telnet *App::telnet() {
    return (Modules::Telnet *)modules[MOD_TELNET].obj;
}

Modules::Web *App::web() {
    return (Modules::Web *)modules[MOD_WEB].obj;
}

Modules::Psu *App::psu() {
    return (Modules::Psu *)modules[MOD_PSU].obj;
}
