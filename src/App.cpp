#include "App.h"

#include "AppUtils.h"
#include "Cli.h"
#include "CrashReport.h"
#include "Global.h"
#include "PrintUtils.h"
#include "PsuLogger.h"
#include "WebPanel.h"

using namespace AppUtils;
using namespace PrintUtils;
using namespace StrUtils;

App::App() {
    memset(&appMod, 0, sizeof(appMod[0]) * APP_MODULES);
}

void App::init(Print *p) {
    out = dbg = err = p;
    restartCountdown_ = 0;
    restartFlag_ = false;
    loopLogger = new LoopLogger();
    configHelper = new ConfigHelper();
}

void App::startSafe() {
    print_welcome(out, " SAFE MODE ", APP_NAME " v" APP_VERSION,
                  " " BUILD_DATE " ");
    shell()->init();
    shell()->setSerial();
}

void App::loopSafe() { shell()->loop(); }

void App::loop() {
    for (uint8_t i = 0; i < APP_MODULES; ++i) {
        AppModule *mod = getModule(AppModuleEnum(i));
        if (mod) {
            if (mod->isNetworkDepended() &&
                (!Wireless::hasNetwork() ||
                 !mod->isCompatible(Wireless::getMode())))
                continue;
            LiveTimer timer = loopLogger->onExecute(AppModuleEnum(i));
            mod->loop();
            delay(0);
        }
    }
    yield();

    handleRestart();
    yield();

    if (networkChanged) {
        restartNetworkDependedModules(Wireless::getMode(), Wireless::hasNetwork());
        refresh_wifi_led();
        networkChanged = false;
    }
    yield();

    loopLogger->loop();
}

size_t App::printDiag(Print *p) {
    size_t n = println(p, SysInfo::getHeapStats().c_str());
    n += print_nameP_value(p, str_http, web()->getClients());
    n += print_nameP_value(p, str_telnet, getBoolStr(telnet()->hasClient()).c_str());
    n += println_nameP_value(p, str_ap, SysInfo::getAPClientsNum());
    return n;
}

size_t App::printDiag(Print *p, const AppModuleEnum module) {
    AppModule *mod = appMod[module];
    if (mod)
        return mod->printDiag(p);
    else
        return print(p, FPSTR(str_disabled));
}

void App::printLoopCapture(Print *p) {
    LoopCapture *cap = loopLogger->getCapture();
    print(out, FPSTR(str_capture));
    print(out, cap->duration);
    println(out, FPSTR(str_ms));

    unsigned long time_range = 2;
    for (uint8_t i = 0; i < cap->counters_size; ++i) {
        if (cap->counter[i] > 0) {
            if (time_range > cap->longest)
                p->printf_P(strf_lu_ms, cap->longest);
            else
                p->printf_P(strf_lu_ms, time_range);
            p->print('\t');
            p->println(cap->counter[i]);
        }
        time_range *= 2;
    }

    if (cap->overrange > 0) {
        p->print(StrUtils::getStrP(str_over));
        p->printf_P(strf_lu_ms, time_range / 2);
        p->print('\t');
        p->println(cap->overrange);
        p->print('\t');
        p->printf_P(strf_lu_ms, cap->longest);
        p->println();
    }

    p->print(StrUtils::getStrP(str_total));
    p->print('\t');
    p->print(cap->total);
    p->print('\t');
    p->println((float)cap->duration / cap->total);

    float total_modules_time = 0;
    for (uint8_t i = 0; i < cap->modules_size; ++i)
        total_modules_time += floor((float)cap->module[i] / ONE_MILLISECOND_mi);

    float system_time = cap->duration - total_modules_time;

    for (uint8_t i = 0; i < cap->modules_size; ++i) {
        p->print(appMod[i]->getName());
        p->print('\t');
        float load =
            (float)cap->module[i] / ONE_MILLISECOND_mi / total_modules_time;
        p->printf_P(strf_per, load * 100);
        p->println();
    }
    p->print(StrUtils::getStrP(str_system));
    p->print('\t');
    p->printf_P(strf_per, ((float)(system_time / cap->duration) * 100));
    p->println();
}

void App::restart(time_t time) {
    restartCountdown_ = time;
    restartFlag_ = true;
    restartUpdated_ = millis();
    if (restartCountdown_) {
        char buf[32];
        sprintf_P(buf, msg_restart_countdown, restartCountdown_);
        PrintUtils::println(out, buf);
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

void App::start() {
    //print_delay(&INFO, getIdentStrP(str_wait, false).c_str(), 5);

    start(MOD_DISPLAY);

    displayProgress(0, BUILD_DATE);

    start(MOD_BTN);
    start(MOD_LED);
    start(MOD_CLOCK);
    start(MOD_PSU);
    start(MOD_SHELL);
    displayProgress(40, "<WIFI>");

    Wireless::start();

    displayProgress(80, "<INIT>");

    Wireless::setOnNetworkStatusChange(
        [this](bool hasNetwork, unsigned long time) {
            char buf[8];
            out->print(StrUtils::getIdentStrP(str_app));
            out->printf("network %s (%.2f sec)\n",
                        StrUtils::getUpDownStr(buf, hasNetwork),
                        (float)time / ONE_SECOND_ms);
            networkChanged = true;
        });
    displayProgress(100, "<COMPLETE>");

    lcd()->refresh();

    psu()->setOnPsuInfo([this](PsuInfo info) {
        if (lcd())
            lcd()->refresh();
    });

    psu()->setOnStateChange([this](PsuState state, PsuStatus status) {
        web()->sendPageState(PG_HOME);
        switch (status) {
            case PSU_OK: {
                led()->set(POWER_LED,
                           psu()->checkState(POWER_ON) ? BLINK : STAY_ON);
                break;
            }
            case PSU_ALERT: {
                led()->set(POWER_LED, BLINK_ALERT);
                break;
            }
            case PSU_ERROR: {
                led()->set(POWER_LED, BLINK_ERROR);
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

void App::restartNetworkDependedModules(Wireless::NetworkMode networkMode, bool hasNetwork) {
    for (size_t i = 0; i < APP_MODULES; ++i) {
        auto mod = getModule(AppModuleEnum(i));
        if (!mod->isNetworkDepended())
            continue;
        if (mod->isCompatible(networkMode)) {
            if (hasNetwork)
                mod->start();
            else
                mod->stop();
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

AppModule *App::getModuleByName(const char *name) {
    for (uint8_t i = 0; i < APP_MODULES; ++i)
        if (strcmp_P(name, (char *)pgm_read_ptr(&(mod_name[i]))) == 0) {
            return getModule(AppModuleEnum(i));
        }
    return 0;
}

AppModule *App::getModule(const AppModuleEnum mod) {
    if (!appMod[mod]) {
        switch (mod) {
            case MOD_BTN: {
                appMod[mod] = new ButtonMod();
                btn()->setOnClick([this]() { if (psu()) psu()->togglePower(); });
                btn()->setOnHold([this](time_t time) {if (time >= HOLD_TIME_TO_RESET_s) restart(3); });
                break;
            }
            case MOD_SYSLOG: {
                appMod[mod] = new SyslogMod();
                break;
            }
            case MOD_CLOCK: {
                appMod[mod] = new ClockMod();
                clock()->setOnChange([this](time_t local, double drift) {
                    print_ident(out, FPSTR(str_clock));
                    char buf[32];
                    TimeUtils::format_elapsed_time(buf, drift);
                    println(out, ctime(&local), buf);
                });

                break;
            }
            case MOD_LED: {
                appMod[mod] = new LedMod();
                break;
            }
            case MOD_PSU: {
                appMod[mod] = new Psu();
                logger = new PsuLogger();
                psu()->setLogger(logger);
                break;
            }
            case MOD_DISPLAY: {
                appMod[mod] = new Display();
                break;
            }
            case MOD_WEB: {
                appMod[mod] = new WebMod();
                break;
            }
            case MOD_SHELL: {
                appMod[mod] = new ShellMod();
                break;
            }
            case MOD_NETSVC: {
                appMod[mod] = new NetworkService();
                break;
            }
            case MOD_TELNET: {
                appMod[mod] = new TelnetServer();
                telnet()->setEventHandler([this](TelnetEventType et, Stream *client) {
                    switch (et) {
                        case CLIENT_DATA:
                            println(out, FPSTR(str_unhandled));
                            return true;
                        case CLIENT_CONNECTED:
                            shell()->setRemote(client);
                            break;
                        case CLIENT_DISCONNECTED:
                            shell()->setSerial();
                            break;
                        default:
                            println(out, FPSTR(str_unhandled));
                            break;
                    }
                    refresh_wifi_led();
                    return true;
                });
                break;
            }
            case MOD_UPDATE: {
                appMod[mod] = new OTAUpdate();
                break;
            }
        }
        appMod[mod]->setOutput(out);
        appMod[mod]->setConfig(params());
        appMod[mod]->init();
    }

    return appMod[mod];
}

bool App::start(const AppModuleEnum mod) {
    auto obj = getModule(mod);
    return obj && obj->start();
}

void App::stop(AppModuleEnum mod) {
    auto obj = getModule(mod);
    if (obj) obj->stop();
}

void App::send_psu_data_to_clients() {
    PsuInfo pi = psu()->getInfo();
    if (Wireless::hasNetwork()) {
        if ((telnet() && telnet()->hasClient()) && !shell()->isActive()) {
            String data = pi.toString();
            data += '\r';
            telnet()->write(data.c_str());
        }
        if (web()->getClients()) {
            String data = String(TAG_PVI);
            data += pi.toString();
            web()->sendToClients(data, PG_HOME);
        }
    }
}

void App::refresh_wifi_led() {
    LedMode mode = STAY_OFF;
    if (restartFlag_ && restartCountdown_ <= 3)
        mode = BLINK_ERROR;
    else if (Wireless::hasNetwork()) {
        mode = STAY_ON;
        if (web()->getClients() || (telnet() && telnet()->hasClient()))
            mode = BLINK;
    }
    led()->set(WIFI_LED, mode);
}

void App::refresh_power_led() {
    LedMode mode = STAY_OFF;
    if (restartFlag_ && restartCountdown_ <= 3)
        mode = BLINK_ERROR;
    else if (psu()) {
        if (psu()->checkStatus(PSU_OK))
            mode = psu()->checkState(POWER_ON) ? BLINK : STAY_ON;
        else
            mode = BLINK_ALERT;
    }
    led()->set(POWER_LED, mode);
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

LedMod *App::led() { return (LedMod *)appMod[MOD_LED]; }

ButtonMod *App::btn() { return (ButtonMod *)appMod[MOD_BTN]; }

TelnetServer *App::telnet() { return (TelnetServer *)appMod[MOD_TELNET]; }

Psu *App::psu() { return (Psu *)appMod[MOD_PSU]; }

ClockMod *App::clock() { return (ClockMod *)appMod[MOD_CLOCK]; }

ShellMod *App::shell() { return (ShellMod *)appMod[MOD_SHELL]; }

ConfigHelper *App::config() { return configHelper; }

Config *App::params() { return configHelper->get(); }

Display *App::lcd() { return (Display *)appMod[MOD_DISPLAY]; }

WebMod *App::web() { return (WebMod *)appMod[MOD_WEB]; }

LoopLogger *App::getLoopLogger() { return loopLogger; }
