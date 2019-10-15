#include "App.h"

#include "AppUtils.h"
#include "Cli.h"
#include "Global.h"
#include "PrintUtils.h"
#include "PsuLogger.h"

using namespace PrintUtils;
using namespace StrUtils;
App app;

Psu *App::getPsu() { return psu; }
WebService *App::getHttp() { return http; }
LoopLogger *App::getLoopLogger() { return loopLogger; }
SystemClock *App::getClock() { return rtc; }

size_t App::printDiag(Print *p, const AppModuleEnum module) {
    AppModule *mod = appMod[module];
    if (mod) {
        return mod->printDiag(p);
    } else {
        return print(p, FPSTR(str_disabled));
    }
}

size_t App::printDiag(Print *p) {
    size_t n = p->println(getHeapStat());
    if (Wireless::getMode() != Wireless::WLAN_STA)
        n += print_nameP_value(p, str_wifi, getConnectedStationInfo().c_str());
    n += print_nameP_value(p, str_http, get_http_clients_count());
    n += print_nameP_value(p, str_telnet, get_telnet_clients_count());
    return n;
}

void App::printLoopCapture(Print *p) {
    LoopCapture *cap = loopLogger->getCapture();

    p->print(StrUtils::getStrP(str_capture));
    p->printf_P(strf_lu_ms, cap->duration);
    p->println();

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
        return true;
    default:
        return false;
    }
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
    if (millis_passed(displayUpdated, now) > ONE_SECOND_ms) {
        handle_restart();
        display->refresh();
        send_psu_data_to_clients();
        displayUpdated = now;
    }

    loopLogger->loop();
}

App::App() {
    memset(appMod, 0, sizeof(&appMod[0]) * APP_MODULES);
    loopLogger = new LoopLogger();
}

void App::printConfig(Print *p) { env->printTo(*p); }

void App::resetConfig() { env->setDefault(); }

void App::loadConfig() { env->loadConfig(); }

bool App::saveConfig() { return env->saveConfig(); }

void App::restart(uint8_t time_s) {
    reboot = time_s;
    if (reboot > 0) {
        leds->set(Led::POWER_LED, Led::BLINK_ERROR);
        leds->set(Led::WIFI_LED, Led::BLINK_ERROR);
        char buf[16];
        sprintf_P(buf, msg_restart_in_d_seconds, reboot);
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

    delay_print(&USE_SERIAL, getIdentStr(str_wait, false).c_str(), 5);

    display->showProgress(0, BUILD_DATE);

    start(MOD_CLOCK);

    start(MOD_PSU);

    display->showProgress(40, "<WIFI>");

    Wireless::start_wifi();

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
    start(MOD_SHELL);

    display->showProgress(100, "<COMPLETE>");
}

void App::refresh_network_modules(bool hasNetwork) {
    for (uint8_t i = 0; i < APP_MODULES; ++i) {
        auto mod = AppModuleEnum(i);
        if (!isNetworkDepended(mod))
            continue;
        if (hasNetwork) {
            start(mod);
        } else {
            stop(mod);
        }
        delay(0);
    }
}

bool App::setOutputVoltageAsDefault() {
    float v = psu->getVoltage();
    if (env->setOutputVoltage(v)) {
        return env->saveConfig();
    }
    return false;
}

bool App::setBootPowerState(BootPowerState value) {
    bool res = false;
    if (env->setBootPowerState(value)) {
        res = env->saveConfig();
    }
    return res;
}

void App::init(Print *p) {
    out = dbg = err = p;
    reboot = 0;
    displayUpdated = 0;

    Wire.begin(I2C_SDA, I2C_SCL);
    SPIFFS.begin();

#ifdef DEBUG_APP
    dbg->println(ESP.getResetReason());
    dbg->println(ESP.getResetInfo());
#endif
}

bool App::getModule(String &str, AppModuleEnum &mod) {
    return getModule(str.c_str(), mod);
}

bool App::getModule(const char *str, AppModuleEnum &mod) {
    for (uint8_t i = 0; i < APP_MODULES; ++i) {
        char *strP = (char *)pgm_read_ptr(&(strP_module[i]));
        if (strcmp_P(str, strP) == 0) {
            mod = AppModuleEnum(i);
            return true;
        }
    }
    return false;
}

AppModule *App::getInstance(const AppModuleEnum module) {
    if (!appMod[module]) {
        switch (module) {
        case MOD_BTN: {
            appMod[module] = btn = new Button();
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
            appMod[module] = rtc = new SystemClock();
            break;
        case MOD_LED:
            appMod[module] = leds = new Led::Leds();
            break;
        case MOD_PSU:
            appMod[module] = psu = new Psu();
            logger = new PsuLogger();
            psu->setLogger(logger);

            psu->setOnStatusChange([this](PsuStatus status, String &str) {
                switch (status) {
                case PSU_OK:
                    leds->set(Led::POWER_LED, psu->checkState(POWER_ON)
                                                  ? Led::BLINK
                                                  : Led::STAY_ON);
                    break;
                case PSU_ALERT:
                    leds->set(Led::POWER_LED, Led::BLINK_ALERT);
                    break;
                case PSU_ERROR:
                    leds->set(Led::POWER_LED, Led::BLINK_ERROR);
                default:
                    break;
                }
            });

            psu->setOnStateChange([this](PsuState state) {
                sendPageState(PG_HOME);

                leds->set(Led::POWER_LED,
                          state == POWER_ON ? Led::BLINK : Led::STAY_ON);

                if (state == POWER_OFF) {
                    size_t size = constrain(
                        logger->getSize(PsuLogEnum::VOLTAGE), 0, 1024);
                    if (size > 0) {
                        PlotData data;
                        float tmp[size];
                        logger->getValues(PsuLogEnum::VOLTAGE, tmp, size);
                        size_t cols = group(&data, tmp, size);
                        display->showPlot(&data, cols);
                    };
                }
            });
            break;
        case MOD_DISPLAY:
            appMod[module] = display = new Display();
            break;
        case MOD_HTTP:
            appMod[module] = http = new WebService();
            http->setOnClientConnection(onHttpClientConnect);
            http->setOnClientDisconnected(onHttpClientDisconnect);
            http->setOnClientData(onHttpClientData);
            break;
        case MOD_SHELL:
            appMod[module] = shell = new ShellMod();
            break;
        case MOD_NETSVC:
            appMod[module] = discovery = new NetworkService();
            break;
        case MOD_NTP: {
            appMod[module] = ntp = new NtpClient();
            ntp->setOnResponse([this](const EpochTime &epoch) {
                if (rtc)
                    rtc->setEpoch(epoch, true);
            });
            break;
        }
        case MOD_TELNET:
            appMod[module] = telnet = new TelnetServer();
            telnet->setOnClientConnect([this](Stream *stream) {
                out->print(StrUtils::getIdentStrP(str_telnet));
                out->println(StrUtils::getStrP(str_connected));
                shell->setRemote(stream);
                refresh_wifi_led();
                return true;
            });
            telnet->setOnCLientDisconnect([this]() {
                out->print(StrUtils::getIdentStrP(str_telnet));
                out->println(StrUtils::getStrP(str_disconnected));
                shell->setLocal();
                refresh_wifi_led();
            });
            break;
        case MOD_UPDATE:
            appMod[module] = ota = new OTAUpdate();
            break;
        }
    }
    return appMod[module];
}

Config *App::getConfig() { return env->get(); }

bool App::start(AppModuleEnum module) {
#ifdef DEBUG_APP
    dbg->printf("[app] > %d", module);
    dbg->println();
#endif
    AppModule *obj = getInstance(module);
    if (obj) {
        obj->setOutput(out);
        obj->init(getConfig());
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
    AppModule *obj = getInstance(module);
    if (obj) {
        obj->stop();
    }
}

uint8_t App::get_telnet_clients_count() {
    return telnet ? telnet->hasClientConnected() : 0;
}

void App::send_psu_data_to_clients() {
    PsuInfo pi = psu->getInfo();
    if (Wireless::hasNetwork()) {
        if (get_telnet_clients_count() && !shell->isActive()) {
            String data = pi.toString();
            data += '\r';
            telnet->write(data.c_str());
        }
        if (get_http_clients_count()) {
            String data = String(TAG_PVI);
            data += pi.toString();
            sendToClients(data, PG_HOME);
        }
    }
}

void App::refresh_wifi_led() {
    Led::LedMode mode = Led::STAY_OFF;
    if (Wireless::hasNetwork()) {
        mode = get_http_clients_count() || get_telnet_clients_count()
                   ? Led::BLINK
                   : Led::STAY_ON;
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

Display *App::getDisplay() { return display; }

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

uint8_t App::get_http_clients_count() {
    uint8_t result = 0;
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
        if (clients[i].connected)
            result++;
    return result;
}

String App::getNetworkConfig() {
    String res = String(SET_NETWORK);
    res += getConfig()->getValueAsByte(WIFI);
    res += ',';
    res += getConfig()->getValueAsString(SSID);
    res += ',';
    res += getConfig()->getValueAsString(PASSWORD);
    res += ',';
    res += getConfig()->getValueAsBool(DHCP);
    res += ',';
    res += getConfig()->getValueAsString(IPADDR);
    res += ',';
    res += getConfig()->getValueAsString(NETMASK);
    res += ',';
    res += getConfig()->getValueAsString(GATEWAY);
    res += ',';
    res += getConfig()->getValueAsString(DNS);
    return res;
}
