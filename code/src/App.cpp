#include "App.h"

#include "AppUtils.h"
#include "Cli.h"
#include "CrashReport.h"

using namespace AppUtils;
using namespace PrintUtils;
using namespace StrUtils;

String App::name(ModuleEnum module) const {
    char buf[16];
    PGM_P strP = (char *)pgm_read_ptr(&(define[module].name));
    strncpy_P(buf, strP, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\x00';
    return String(buf);
}

bool App::get(const char *str, ModuleEnum &module) const {
    for (uint8_t i = 0; i < APP_MODULES; ++i)
        if (name(ModuleEnum(i)) == str) {
            module = ModuleEnum(i);
            return true;
        }
    return false;
}

Module *App::instance(ModuleEnum module) {
    if (!appMod[module]) {
        switch (module) {
            case MOD_CLOCK: {
                appMod[module] = new Modules::Clock();
                clock()->setOnChange([this](const time_t local, double diff) {
                    print_ident(out_, FPSTR(str_clock));
                    println(out_, FPSTR(str_updated), TimeUtils::format_elapsed_full(diff));
                });
                break;
            }
            case MOD_BTN: {
                appMod[module] = new Modules::Button(POWER_BTN_PIN);
                btn()->setOnClick([this]() { if (psu()) psu()->togglePower(); });
                btn()->setOnHold([this](time_t time) {if (time >= HOLD_TIME_TO_RESET_s) restart(3); });
                break;
            }
            case MOD_LED: {
                appMod[module] = new Modules::Led();
                break;
            }
            case MOD_SYSLOG: {
                appMod[module] = new SyslogModule();
                break;
            }
            case MOD_PSU: {
                appMod[module] = new PsuModule(this);
                psuLog = new PsuLogHelper();
                break;
            }
            case MOD_DISPLAY: {
                appMod[module] = new Display();
                break;
            }
            case MOD_WEB: {
                appMod[module] = new Modules::Web();
                break;
            }
            case MOD_SHELL: {
                appMod[module] = new Modules::Shell(Cli::get());
                break;
            }
            case MOD_NETSVC: {
                appMod[module] = new NetworkService();
                break;
            }
            case MOD_TELNET: {
                appMod[module] = new Modules::Telnet();
                telnet()->setEventHandler([this](TelnetEventType et, WiFiClient *client) {
                    PrintUtils::print_ident(out_, FPSTR(str_telnet));
                    switch (et) {
                        case CLIENT_CONNECTED:                        
                            PrintUtils::println(out_, FPSTR(str_connected), prettyIpAddress(client->remoteIP(), client->remotePort()));
                            shell()->setRemote(telnet()->getTerminal());
                            break;
                        case CLIENT_DISCONNECTED:
                            shell()->setRemote(nullptr);
                            PrintUtils::println(out_, FPSTR(str_disconnected));
                            break;
                    }    
                    refresh_wifi_led();
                    return true;
                });        
                break;
            }
            case MOD_UPDATE: {
                appMod[module] = new OTAUpdate(OTA_PORT);
                break;
            }
        }
        appMod[module]->setOutput(out_);
        appMod[module]->setConfig(params());
    }

    return appMod[module];
}

void App::log(PsuData &item) {
    psuLog->log(item);

    if (Wireless::hasNetwork()) {
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

App::App():networkChanged(false), restartFlag_(false), restartCountdown_(0) {
    memset(appMod, 0, sizeof(appMod[0]) * APP_MODULES);
}

void App::init() {    
    loopLogger = new LoopWatcher();
    configHelper = new ConfigHelper();
    Cli::init();
    initModules();
}

void App::startSafe() {
    start(MOD_SHELL);
    shell()->init();
}

void App::loopSafe() { shell()->loop(); }

void App::loop() {
    for (size_t i = 0; i < APP_MODULES; ++i) {
        auto *obj = module(i);
        if (obj) {
            if (obj->isNetworkDepended() && (!Wireless::hasNetwork() || !obj->isCompatible(Wireless::getMode())))
                continue;
            LiveTimer timer = loopLogger->onExecute(ModuleEnum(i));
            obj->loop();
            delay(0);
        }
    }

    handleRestart();
    delay(0);

    if (networkChanged) {
        restartNetworkDependedModules(Wireless::getMode(), Wireless::hasNetwork());
        refresh_wifi_led();
        networkChanged = false;
    }
    delay(0);

    loopLogger->loop();
}

size_t App::printDiag(Print *p) {
    DynamicJsonDocument doc(256);

    doc[FPSTR(str_heap)] = SysInfo::getHeapStats();
    doc[FPSTR(str_ap)] = Wireless::AP_Clients();
    doc[FPSTR(str_http)] = web()->getClients();
    doc[FPSTR(str_telnet)] = getBoolStr(telnet()->hasClient());

    size_t n = serializeJsonPretty(doc, *p);
    return n += print_ln(p);
}

size_t App::printDiag(Print *p, const ModuleEnum mod) {
    Module *obj = appMod[mod];
    if (obj) {
        return obj->printDiag(p);
    } else {
        return print(p, FPSTR(str_disabled));
    }
}

void App::printLoopCapture(Print *p) {
   
}

void App::restart(time_t time) {
    restartCountdown_ = time;
    restartFlag_ = true;
    restartUpdated_ = millis();
    if (restartCountdown_) {
        char buf[32];
        sprintf_P(buf, msg_restart_countdown, restartCountdown_);
        PrintUtils::println(out_, buf);
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
    if (lcd()->isEnabled()) lcd()->showProgress(progress, message);
}

void App::begin() {
    instance(MOD_DISPLAY);

    displayProgress(0, BUILD_DATE);

    instance(MOD_BTN);
    instance(MOD_LED);
    instance(MOD_CLOCK);
    instance(MOD_PSU);
    instance(MOD_SHELL);
    displayProgress(40, "<WIFI>");

    Wireless::start();

    displayProgress(80, "<INIT>");

    Wireless::setOnNetworkStatusChange(
        [this](bool hasNetwork, unsigned long time) {
            char buf[8];
            out_->print(StrUtils::getIdentStrP(str_app));
            out_->printf("network %s (%.2f sec)\n",
                         StrUtils::getUpDownStr(buf, hasNetwork),
                         (float)time / ONE_SECOND_ms);
            networkChanged = true;
        });

    displayProgress(100, "<COMPLETE>");

    lcd()->refresh();

    psu()->setOnData([this](PsuData data) {
        if (lcd())
            lcd()->refresh();
    });

    psu()->setOnStateChange([this](PsuState state, PsuStatus status) {
        web()->sendPageState(PG_HOME);
        switch (status) {
            case PSU_OK: {
                led()->set(RED_LED,
                           psu()->checkState(POWER_ON) ? BLINK : LIGHT_ON);
                break;
            }
            case PSU_ALERT: {
                led()->set(RED_LED, BLINK_ALERT);
                break;
            }
            case PSU_ERROR: {
                led()->set(RED_LED, BLINK_ERROR);
            }
            default: {
                break;
            }
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
        lcd()->refresh();
    });
}

void App::restartNetworkDependedModules(NetworkMode networkMode, bool hasNetwork) {
    for (size_t i = 0; i < APP_MODULES; ++i) {
        auto obj = module(i);
        if (!obj->isNetworkDepended())
            continue;
        if (obj->isCompatible(networkMode)) {
            if (hasNetwork)
                obj->start();
            else
                obj->stop();
        }
    }
}

bool App::setOutputVoltageAsDefault() {
    float v = psu()->getVoltage();
    if (config()->setOutputVoltage(v))
        return config()->save();
    return false;
}

bool App::setBootPowerState(BootPowerState value) {
    bool res = false;
    if (config()->setBootPowerState(value))
        res = config()->save();
    return res;
}

void App::refresh_wifi_led() {
    LedSignal mode = LIGHT_OFF;
    if (restartFlag_ && restartCountdown_ <= 3)
        mode = BLINK_ERROR;
    else if (Wireless::hasNetwork()) {
        mode = LIGHT_ON;
        if (web()->getClients() || (telnet() && telnet()->hasClient()))
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
}
