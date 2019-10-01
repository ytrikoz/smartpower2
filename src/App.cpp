#include "App.h"

#include "AppUtils.h"
#include "Cli.h"
#include "Global.h"
#include "PrintUtils.h"
#include "PsuLogger.h"

using namespace PrintUtils;

App app;

SystemClock *App::getClock() { return rtc; }

void App::printDiag(const AppModuleEnum module, Print *p) {
    AppModule *mod = appMod[module];
    if (mod) {
        mod->printDiag(p);
    } else {
        p->println(StrUtils::getStrP(str_disabled, false));
    }
}

PsuState *App::getPsuState() { return psu->getPsuState(); }

Psu *App::getPsu() { return psu; }

Display *App::getDisplay() { return display; }

LoopLogger *App::getLoopLogger() { return loopLogger; }

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
    case MOD_HTTP:
    case MOD_NETSVC:
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
        if (!mod)
            continue;
        else {
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
        update_display();
        displayUpdated = now;
    }

    loopLogger->loop();
}

App::App() {
    memset(appMod, 0, sizeof(appMod[0]) * 13);
    loopLogger = new LoopLogger();
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

void App::printConfig(Print *p) { env->printTo(*p); }

void App::resetConfig() { env->setDefault(); }

void App::loadConfig() { env->loadConfig(); }

bool App::saveConfig() { return env->saveConfig(); }

void App::restart(uint8_t seconds) {
    reboot = seconds;
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
    if (reboot && (--reboot == 0))
        system_restart();
}

void App::start() {
#ifdef DEBUG_APP_MOD
    dbg->println("[app] start()");
#endif

    env = new ConfigHelper();

    print_welcome(out, " Welcome ", APP_NAME " v" APP_VERSION,
                  " " BUILD_DATE " ");

    start(MOD_BTN);

    start(MOD_LED);

#ifndef DISABLE_LCD
    start(MOD_LCD);
#endif
    delay_print(&USE_SERIAL, StrUtils::getIdentStr(str_wait, false).c_str(), 5);

    boot_progress(0, BUILD_DATE);

    start(MOD_CLOCK);

    start(MOD_PSU);

    boot_progress(40, "<WIFI>");

    Wireless::start_wifi();

    boot_progress(80, "<INIT>");

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
    boot_progress(100, "<COMPLETE>");
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
    float _voltage = psu->getOutputVoltage();
    if (env->setOutputVoltage(_voltage)) {
        return env->saveConfig();
    }
    return false;
}

bool App::setBootPowerState(BootPowerState state) {
    if (env->setBootPowerState(state)) {
        return env->saveConfig();
    }
    return false;
}

void App::init(Print *p) {
    this->out = this->dbg = this->err = p;
    this->reboot = 0;
    this->boot_per = 0;
    this->displayUpdated = 0;

    Wire.begin(I2C_SDA, I2C_SCL);
    SPIFFS.begin();

#ifdef DEBUG_APP_MOD
    dbg->println(ESP.getResetReason());
    dbg->println(ESP.getResetInfo());
#endif
}

AppModule *App::getInstance(const AppModuleEnum module) {
    if (!appMod[module]) {
        switch (module) {
        case MOD_BTN:
            appMod[module] = btn = new Button();
            btn->setOnClicked([this]() { psu->togglePower(); });
            btn->setOnHold([this](unsigned long time) {
                if (time > ONE_SECOND_ms * 5) {
                    leds->set(Led::POWER_LED, Led::BLINK_ERROR);
                }
            });
            btn->setOnHoldRelease(
                [this](unsigned long time) { refresh_power_led(); });
            break;
        case MOD_CLOCK:
            appMod[module] = rtc = new SystemClock();
            break;
        case MOD_LED:
            appMod[module] = leds = new Led::Leds();
            break;
        case MOD_PSU:
            appMod[module] = psu = new Psu();
            psuLogger = new PsuLogger();
            psu->setLogger(psuLogger);
            psu->setOnTogglePower([]() { sendPageState(PG_HOME); });
            psu->setOnError([this](String &str) {
                leds->set(Led::POWER_LED, Led::BLINK_ERROR);
                load_screen_message(StrUtils::getStrP(str_error).c_str(),
                                    str.c_str());
                psu->powerOff();
            });
            psu->setOnAlert([this](String &str) {
                leds->set(Led::POWER_LED, Led::BLINK_ERROR);
                load_screen_message(StrUtils::getStrP(str_alert).c_str(),
                                    str.c_str());
            });
            psu->setOnPowerOn([this]() {
                leds->set(Led::POWER_LED, Led::BLINK);
                load_screen_psu_pvi();
                if (display)
                    display->unlock();
            });
            psu->setOnPowerOff([this]() {
                if (app.getPsuState()->getStatus(PSU_OK)) {
                    leds->set(Led::POWER_LED, Led::STAY_ON);
                    size_t size = psuLogger->getLog(VOLTAGE_LOG)->size();
                    if (size > 1024)
                        size = 1024;

                    if (size > 0) {
                        float val[size];
                        psuLogger->getLogValues(VOLTAGE_LOG, val, size);
                        if (display) {
                            size_t cols =
                                fill_data(display->getData(), val, size);
                            display->drawPlot(8 - cols);
                            display->lock(15000);
                        };
                    };
                }
            });
            break;
        case MOD_LCD:
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
#ifdef DEBUG_APP_MOD
    String str = getModuleName(module);
    dbg->printf("[app] start %s", str.c_str());
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
#ifdef DEBUG_APP_MOD
    String str = getModuleName(module);
    dbg->printf("[app] stop %s", str.c_str());
    dbg->println();
#endif
    AppModule *obj = getInstance(module);
    if (obj) {
        obj->stop();
    }
}

void App::boot_progress(uint8_t per, const char *payload) {
    display->setScreen(SCREEN_BOOT, 0);
    if (per == 0)
        display->drawTextCenter(LCD_ROW_1, payload);
    while (per - boot_per > 0) {
        boot_per += 5;
        if (display)
            display->drawProgressBar(LCD_ROW_2, per);
        delay(250);
    }
    if (payload != NULL)
        display->drawTextCenter(LCD_ROW_1, payload);
}

void App::send_psu_data_to_clients() {
    if (getPsuState()->getPower(POWER_ON)) {
        PsuInfo pi = psu->getInfo();
        if (Wireless::hasNetwork()) {
#ifndef DISABLE_TELNET
            if (get_telnet_clients_count() && !shell->isActive()) {
                String data = pi.toString();
                data += '\r';
                telnet->write(data.c_str());
            }
#endif
#ifndef DISABLE_HTTP
            if (get_http_clients_count()) {
                String data = String(TAG_PVI);
                data += pi.toString();
                sendToClients(data, PG_HOME);
            }
#endif
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
    if (psu) {
        leds->set(Led::POWER_LED, psu->getPsuState()->getPower(POWER_ON)
                                      ? Led::BLINK
                                      : Led::STAY_ON);
    } else {
        leds->set(Led::POWER_LED, Led::BLINK_ERROR);
    }
}