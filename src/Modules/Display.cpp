#include "Modules/Display.h"

#include "App.h"
#include "PsuUtils.h"

using namespace StrUtils;

Display::Display() : AppModule(MOD_DISPLAY) {
    lcd = new LcdDisplay();
    lockTimeout = lockUpdated = 0;
}

void Display::setScreen(ScreenEnum value) {
    if (activeScreen == value)
        return;

    if (!lcd->connect())
        return;

    if (locked())
        return;

    switch (value) {
    case SCREEN_CLEAR:
        lcd->loadBank(BANK_NONE);
    case SCREEN_BOOT:
        lcd->loadBank(BANK_PROGRESS);
        break;
    case SCREEN_PSU:
        lcd->loadBank(BANK_NONE);
    case SCREEN_PLOT:
        break;
    case SCREEN_TEXT:
        lcd->loadBank(BANK_NONE);
        break;
    case SCREEN_MESSAGE:
        break;
    }

    lcd->clear();
    screenUpdated = lastScroll = 0;
    activeScreen = value;
}

bool Display::begin() {
    bool res = lcd->connect();
    if (res) {
        lcd->turnOn();
        say_strP(str_ready);
    } else {
        say_strP(str_failed);
    }
    return res;
}

void Display::showProgress(uint8_t per, const char *str) {
    setScreen(SCREEN_BOOT);
    static uint8_t last = 0;
    if (per == 0)
        lcd->drawTextCenter(LCD_ROW_1, str);
    lcd->drawProgressBar(LCD_ROW_2, per);
    while (per > last) {
        last += 5;
        delay(250);
        lcd->drawProgressBar(LCD_ROW_2, per);
    }
    if (last > 0)
        lcd->drawTextCenter(LCD_ROW_1, str);
    last = per;
    if (per == 100)
        refresh();
}

void Display::enableBacklight(bool value) {
    if (!lcd->connect())
        return;
    if (value)
        lcd->turnOn();
    else
        lcd->turnOff();
    backlight = value;
}

void Display::setConfig(Config *config) {
    backlight = config->getValueAsBool(BACKLIGHT);
}

void Display::clear() { activeScreen = SCREEN_CLEAR; }

void Display::refresh() {
#ifdef DEBUG_DISPLAY
    DEBUG.println("refresh()");
#endif
    setScreen(SCREEN_TEXT);
    Psu *psu = app.getPsu();
    if (psu->checkState(POWER_ON)) {
        PsuStatus status = psu->getStatus();
        switch (status) {
        case PSU_OK:
            load_psu_info(&screen);
            return;
        case PSU_ALERT:
            load_message(
                &screen, StrUtils::getStrP(str_alert).c_str(),
                StrUtils::getStrP(getAlertStrP(psu->getAlert())).c_str());
            return;
        case PSU_ERROR:
            load_message(
                &screen, StrUtils::getStrP(str_error).c_str(),
                StrUtils::getStrP(getErrorStrP(psu->getError())).c_str());
            return;
        }
    }

    Wireless::Mode mode = Wireless::getMode();
    switch (mode) {
    case Wireless::WLAN_STA:
        load_wifi_sta(&screen);
        return;
    case Wireless::WLAN_AP:
        load_wifi_ap(&screen);
        return;
    case Wireless::WLAN_AP_STA:
        load_wifi_ap_sta(&screen);
        return;
    case Wireless::WLAN_OFF:
        load_ready(&screen);
        return;
    }
}

void Display::load_wifi_sta(Screen *obj) {
    obj->set(0, "WIFI> ", Wireless::getConnectionStatus().c_str());
    obj->set(1, "STA> ", Wireless::hostSTA_SSID().c_str());
    obj->set(2, "IP> ", Wireless::hostIP().toString().c_str());
    obj->set(3, "RSSI> ", Wireless::RSSIInfo().c_str());
    obj->setCount(4);
};

void Display::load_wifi_ap(Screen *obj) {
    obj->set(0, "AP> ", Wireless::hostAP_SSID().c_str());
    obj->set(1, "PWD> ", Wireless::hostAP_Password().c_str());
    obj->setCount(2);
};

void Display::load_wifi_ap_sta(Screen *obj) {
    obj->set(0, "WIFI> ", Wireless::getConnectionStatus().c_str());
    obj->set(1, "AP> ", Wireless::hostAP_SSID().c_str());
    obj->set(2, "STA> ", Wireless::hostSTA_SSID().c_str());
    obj->set(3, "IP AP> ", Wireless::hostAP_IP().toString().c_str());
    obj->set(4, "IP STA> ", Wireless::hostSTA_IP().toString().c_str());
    obj->set(5, "RSSI> ", Wireless::RSSIInfo().c_str());
    obj->setCount(6);
};

void Display::load_psu_info(Screen *obj) {
    Psu *psu = app.getPsu();
    String str = String(psu->getV(), 3);
    if (str.length() == 5)
        str += " ";
    str += "V ";
    str += String(psu->getI(), 3);
    str += " A ";
    obj->set(0, str.c_str());

    double watt = psu->getP();
    str = String(watt, (watt < 10) ? 3 : 2);
    str += " W ";
    double rwatth = psu->getWh();
    if (rwatth < 1000) {
        str += String(rwatth, rwatth < 10 ? 3 : rwatth < 100 ? 2 : 1);
        str += " Wh";
    } else {
        str += String(rwatth / 1000, 3);
        str += "KWh";
    }
    obj->set(1, str.c_str());
    obj->setCount(2);
    obj->moveFirst();
}

void Display::load_ready(Screen *obj) {
    obj->set(0, "READY> ");
    obj->setCount(1);
    obj->moveFirst();
}

void Display::load_message(Screen *obj, const char *header,
                           const char *message) {
    obj->set(0, header);
    obj->set(1, message);
    obj->setCount(2);
    obj->moveFirst();
}

void Display::showMessage(const char *header, const char *message) {
    load_message(&screen, header, message);
}

void Display::showPlot(PlotData *data, size_t cols) {
    lcd->drawPlot(data, 8 - cols);
    lock(15000);
}

void Display::loop() {
    if (activeScreen == SCREEN_CLEAR || activeScreen == SCREEN_BOOT)
        return;

    unsigned long now = millis();

    if (locked(now))
        return;

    if (millis_passed(screenUpdated, now) >= LCD_UPDATE_INTERVAL) {
        lcd->drawScreen(&screen);
        screenUpdated = now;
    }

    if (screen.count() > LCD_ROWS) {
        if (millis_passed(lastScroll, now) >= LCD_SCROLL_INTERVAL) {
            screen.next();
            lastScroll = now;
        }
    }
}

void Display::unlock() { lockTimeout = 0; }

void Display::lock(unsigned long time) {
#ifdef DEBUG_DISPLAY
    DEBUG.printf("lock %lu", time);
    DEBUG.println();
#endif
    lockTimeout = time;
    lockUpdated = millis();
}

bool Display::locked(void) { return locked(millis()); }

bool Display::locked(unsigned long now) {
    if (lockTimeout > 0) {
        unsigned long passed = millis_passed(lockUpdated, now);
        if (lockTimeout > passed) {
            lockTimeout -= passed;
        } else {
            lockTimeout = 0;
        }
        lockUpdated = now;
    }
    return lockTimeout;
}
