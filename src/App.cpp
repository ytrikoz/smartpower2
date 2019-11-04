#include "App.h"

#include "AppUtils.h"
#include "Cli.h"
#include "Global.h"
#include "PrintUtils.h"
#include "PsuLogger.h"
#include "WebPanel.h"

using namespace AppUtils;
using namespace PrintUtils;
using namespace StrUtils;

App::App() {
    memset(appMod, 0, sizeof(&appMod[0]) * APP_MODULES);
    loopLogger = new LoopLogger();
}

void App::loop() {
    for (uint8_t i = 0; i < APP_MODULES; ++i) {
        AppModule *mod = getInstance(AppModuleEnum(i));
        if (mod) {
            if (Wireless::hasNetwork() ||
                !isNetworkDepended(AppModuleEnum(i))) {
                LiveTimer timer = loopLogger->onExecute(AppModuleEnum(i));
                mod->loop();
            }
        }
        delay(0);
    }

    unsigned long now = millis();
    if (millis_passed(lastUpdated, now) > ONE_SECOND_ms) {
        handle_restart();
        send_psu_data_to_clients();
        lastUpdated = now;
    }

    loopLogger->loop();
}

size_t App::printDiag(Print *p) {
    size_t n = print(p, SysInfo::getHeapStats().c_str());
    n += print_nameP_value(p, str_http, WebPanel::get_http_clients_count());
    n += print_nameP_value(p, str_telnet, telnet && telnet->hasClient());
    n += println_nameP_value(p, str_ap, SysInfo::getAPClientsNum());
    return n;
}

size_t App::printDiag(Print *p, const AppModuleEnum module) {
    AppModule *mod = appMod[module];
    if (mod) {
        return mod->printDiag(p);
    } else {
        return print(p, FPSTR(str_disabled));
    }
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

bool App::isNetworkDepended(AppModuleEnum module) {
    switch (module) {
    case MOD_NETSVC:
    case MOD_HTTP:
    case MOD_UPDATE:
    case MOD_NTP:
    case MOD_TELNET:
    case MOD_SYSLOG:
        return true;
    default:
        return false;
    }
}

ConfigHelper *App::getEnv() { return env; }

void App::restart(uint8_t time_s) {
    reboot = time_s;
    if (reboot > 0) {
        leds->set(Led::POWER_LED, Led::BLINK_ERROR);
        leds->set(Led::WIFI_LED, Led::BLINK_ERROR);
        char buf[32];
        sprintf_P(buf, msg_restart_countdown, reboot);
        out->println(buf);
    } else {
        refresh_power_led();
    }
}

void App::handle_restart() {
    if (reboot == 1)
        system_restart();
    if (reboot > 1)
        reboot--;
}

void App::start() {
    env = new ConfigHelper();
    print_welcome(out, " Welcome ", APP_NAME " v" APP_VERSION,
                  " " BUILD_DATE " ");
    start(MOD_BTN);
    start(MOD_LED);
    start(MOD_DISPLAY);
    print_delay(&USE_SERIAL, getIdentStr(str_wait, false).c_str(), 5);
    display->showProgress(0, BUILD_DATE);
    start(MOD_CLOCK);
    start(MOD_PSU);
    display->showProgress(40, "<WIFI>");
    Wireless::start();
    display->showProgress(80, "<INIT>");

    Wireless::setOnNetworkStatusChange(
        [this](bool hasNetwork, unsigned long time) {
            char buf[8];
            out->print(StrUtils::getIdentStrP(str_app));
            out->printf("network %s (%.2f sec)",
                        StrUtils::getUpDownStr(buf, hasNetwork),
                        (float)time / ONE_SECOND_ms);
            out->println();
            refresh_wifi_led();
            refresh_network_modules(hasNetwork);
        });

    display->showProgress(100, "<COMPLETE>");

    display->refresh();

    psu->setOnPsuInfo([this](PsuInfo info) { display->refresh(); });

    psu->setOnStateChange([this](PsuState state, PsuStatus status) {
#ifdef DEBUG_DISPLAY
        DEBUG.printf("onStateChange(%d, %d)", state, status);
        DEBUG.println();
#endif
        WebPanel::sendPageState(PG_HOME);

        switch (status) {
        case PSU_OK:
            leds->set(Led::POWER_LED,
                      psu->checkState(POWER_ON) ? Led::BLINK : Led::STAY_ON);
            break;
        case PSU_ALERT:
            leds->set(Led::POWER_LED, Led::BLINK_ALERT);
            break;
        case PSU_ERROR:
            leds->set(Led::POWER_LED, Led::BLINK_ERROR);
        default:
            break;
        }
        display->refresh();
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
    });

    start(MOD_SHELL);
}

void App::refresh_network_modules(bool hasNetwork) {
    for (uint8_t i = 0; i < APP_MODULES; ++i) {
        auto mod = AppModuleEnum(i);
        if (!isNetworkDepended(mod))
            continue;
        if (hasNetwork)
            start(mod);
        else
            stop(mod);
        delay(0);
    }
}

bool App::setOutputVoltageAsDefault() {
    float v = psu->getVoltage();
    if (env->setOutputVoltage(v))
        return env->saveConfig();
    return false;
}

bool App::setBootPowerState(BootPowerState value) {
    bool res = false;
    if (env->setBootPowerState(value))
        res = env->saveConfig();
    return res;
}

void App::init(Print *p) {
    out = dbg = err = p;
    reboot = 0;
    lastUpdated = 0;

    Wire.begin(I2C_SDA, I2C_SCL);
#ifdef DEBUG_APP
    dbg->println(ESP.getResetReason());
    dbg->println(ESP.getResetInfo());
#endif
}

bool App::getModule(String &str, AppModuleEnum &mod) {
    return getModule(str.c_str(), mod);
}

bool App::getModule(const char *name, AppModuleEnum &mod) {
    for (uint8_t i = 0; i < APP_MODULES; ++i) {
        char *strP = (char *)pgm_read_ptr(&(strP_module[i]));
        if (strcmp_P(name, strP) == 0) {
            mod = AppModuleEnum(i);
            return true;
        }
    }
    return false;
}

AppModule *App::getInstance(const AppModuleEnum mod) {
    if (!appMod[mod]) {
        switch (mod) {
        case MOD_SYSLOG: {
            appMod[mod] = syslog = new SyslogClient();
            break;
        }
        case MOD_BTN: {
            appMod[mod] = btn = new Button();
            btn->setOnClicked([this]() { psu->togglePower(); });
            btn->setOnHold([this](unsigned long time) {
                if (time > ONE_SECOND_ms * 5)
                    leds->set(Led::POWER_LED, Led::BLINK_ERROR);
            });
            btn->setOnHoldRelease(
                [this](unsigned long time) { refresh_power_led(); });
            break;
        }
        case MOD_CLOCK:
            appMod[mod] = rtc = new SystemClock();
            break;
        case MOD_LED:
            appMod[mod] = leds = new Led::Leds();
            break;
        case MOD_PSU:
            appMod[mod] = psu = new Psu();
            logger = new PsuLogger();
            psu->setLogger(logger);
            break;
        case MOD_DISPLAY:
            appMod[mod] = display = new Display();
            break;
        case MOD_HTTP:
            appMod[mod] = http = new WebService();
            WebPanel::init();
            http->setOnClientConnection(WebPanel::onHttpClientConnect);
            http->setOnClientDisconnected(WebPanel::onHttpClientDisconnect);
            http->setOnClientData(WebPanel::onHttpClientData);
            break;
        case MOD_SHELL:
            appMod[mod] = shell = new ShellMod();
            break;
        case MOD_NETSVC:
            appMod[mod] = netsvc = new NetworkService();
            break;
        case MOD_NTP: {
            appMod[mod] = ntp = new NtpClient();
            ntp->setOnResponse([this](const EpochTime &epoch) {
                if (rtc)
                    rtc->setEpoch(epoch, true);
            });
            break;
        }
        case MOD_TELNET:
            appMod[mod] = telnet = new TelnetServer();
            telnet->setEventHandler([this](TelnetEventType et, Stream *client) {
                switch (et) {
                case CLIENT_DATA:
                    println(out, FPSTR(str_unhandled));
                    return true;
                case CLIENT_CONNECTED:
                    shell->setRemote(client);
                    break;
                case CLIENT_DISCONNECTED:
                    shell->setSerial();
                    break;
                default:
                    println(out, FPSTR(str_unhandled));
                    break;
                }
                refresh_wifi_led();
                return true;
            });
            break;
        case MOD_UPDATE:
            appMod[mod] = ota = new OTAUpdate();
            break;
        }
    }
    return appMod[mod];
}

bool App::start(AppModuleEnum module) {
#ifdef DEBUG_APP
    dbg->printf("[app] > %d", module);
    dbg->println();
#endif
    AppModule *obj = getInstance(module);
    if (obj) {
        obj->setOutput(out);
        obj->init(env->get());
        return obj->begin();
    } else {
        return false;
    }
}

void App::stop(AppModuleEnum module) {
#ifdef DEBUG_APP
    dbg->printf("[app] X %d", module);
    dbg->println();
#endif
    if (AppModule *obj = getInstance(module))
        obj->stop();
}

void App::send_psu_data_to_clients() {
    if (!psu->checkState(POWER_ON))
        return;
    PsuInfo pi = psu->getInfo();
    if (Wireless::hasNetwork()) {
        if ((telnet && telnet->hasClient()) && !shell->isActive()) {
            String data = pi.toString();
            data += '\r';
            telnet->write(data.c_str());
        }
        if (WebPanel::get_http_clients_count()) {
            String data = String(TAG_PVI);
            data += pi.toString();
            WebPanel::sendToClients(data, PG_HOME);
        }
    }
}

void App::refresh_wifi_led() {
    Led::LedMode mode = Led::STAY_OFF;
    if (Wireless::hasNetwork()) {
        mode = Led::STAY_ON;
        if (WebPanel::get_http_clients_count() ||
            (telnet && telnet->hasClient()))
            mode = Led::BLINK;
    }
    leds->set(Led::WIFI_LED, mode);
}

void App::refresh_power_led() {
    Led::LedMode mode = Led::STAY_OFF;
    if (psu) {
        if (psu->checkStatus(PSU_OK))
            mode = psu->checkState(POWER_ON) ? Led::BLINK : Led::STAY_ON;
        else
            mode = Led::BLINK_ALERT;
    }
    leds->set(Led::POWER_LED, mode);
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

ShellMod *App::getShell() { return shell; }

Config *App::getConfig() { return env->get(); }

Display *App::getDisplay() { return display; }

Psu *App::getPsu() { return psu; }

WebService *App::getHttp() { return http; }

LoopLogger *App::getLoopLogger() { return loopLogger; }

SystemClock *App::getClock() { return rtc; }
