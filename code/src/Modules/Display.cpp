#include "Modules/Display.h"

#include "Modules/Psu.h"

#include "Global.h"

namespace Modules {

bool Display::isEnabled() {
    return lcd && lcd->isEnabled();
}

void Display::setScreen(ScreenEnum value) {
    if (activeScreen == value)
        return;

    if (!lcd->connect())
        return;

    if (locked())
        return;

    switch (value) {
        case SCREEN_BOOT:
            lcd->loadBank(BANK_PROGRESS);
            break;
        case SCREEN_CLEAR:
        case SCREEN_PSU:
        case SCREEN_PSU_STAT:
        case SCREEN_READY:
        case SCREEN_PLOT:
        case SCREEN_AP:
        case SCREEN_AP_STA:
        case SCREEN_STA:
            lcd->loadBank(BANK_NONE);
            break;
        case SCREEN_MESSAGE:
            break;
    }

    lcd->clear();
    screenUpdated = lastScroll = screenRedraw = millis();
    activeScreen = value;
}

bool Display::onInit() {
    lcd = new LcdDisplay();
    lastUpdated = lockTimeout = lockUpdated = 0;
    backlight_ = config_->getValueAsBool(BACKLIGHT);
    backlightTime_ = 0;
    bool result = lcd->connect();
    if (result) {
        enableBacklight(backlight_);
    }
    return result;
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
}

bool Display::enableBacklight(const bool enabled, const time_t time) {
    if (!isEnabled()) return false;

    if (enabled)
        lcd->turnOn();
    else
        lcd->turnOff();
    backlight_ = enabled;
    backlightTime_ = time;
    return true;
}

void Display::clear(void) { activeScreen = SCREEN_CLEAR; }

void Display::refresh(void) {
    Modules::Psu *psu = app.psu();
    if (psu->getState() == POWER_ON) {
        setScreen(SCREEN_PSU);
        PsuStatus status = psu->getStatus();
        switch (status) {
            case PSU_OK:
                load_psu_info(&screen);
                return;
            case PSU_ALERT:
                load_message(&screen, FPSTR(str_alert), getAlertStr(psu->getAlert()));
                return;
            case PSU_ERROR:
                load_message(&screen, FPSTR(str_error), getErrorStr(psu->getError()));
                return;
        }
    }

    NetworkMode mode = Wireless::getMode();
    switch (mode) {
        case NetworkMode::NETWORK_OFF:
            setScreen(SCREEN_READY);
            load_ready(&screen);
            return;
        case NetworkMode::NETWORK_STA:
            setScreen(SCREEN_STA);
            load_wifi_sta(&screen);
            return;
        case NetworkMode::NETWORK_AP:
            setScreen(SCREEN_AP);
            load_wifi_ap(&screen);
            return;
        case NetworkMode::NETWORK_AP_STA:
            setScreen(SCREEN_AP_STA);
            load_wifi_ap_sta(&screen);
            return;
    }
}

void Display::load_wifi_sta(Screen *obj) {
    size_t n = 0;
    obj->set(n++, "WIFI> ", Wireless::getSTAStatus());
    obj->set(n++, "STA> ", Wireless::hostSTA_SSID());
    obj->set(n++, "IP> ", Wireless::hostSTA_IP().toString());
    obj->set(n++, "RSSI> ", Wireless::hostSTA_RSSI());
    obj->set(n++, "CLK> ", getTimeStr(app.clock()->getLocal(), true));
    obj->setCount(n);
};

void Display::load_wifi_ap(Screen *obj) {
    size_t n = 0;
    obj->set(n++, "AP> ", Wireless::AP_SSID());
    obj->set(n++, "PWD> ", Wireless::AP_Password());
    obj->set(n++, "CLK> ", getTimeStr(app.clock()->getLocal(), true));
    obj->setCount(n);
};

void Display::load_wifi_ap_sta(Screen *obj) {
    size_t n = 0;
    obj->set(n++, "WIFI> ", Wireless::getSTAStatus());
    obj->set(n++, "RSSI> ", Wireless::hostSTA_RSSI());
    obj->set(n++, "STA> ", Wireless::hostSTA_SSID());
    obj->set(n++, "AP> ", Wireless::AP_SSID());
    obj->set(n++, "IP AP> ", Wireless::AP_IP().toString());
    obj->set(n++, "IP STA> ", Wireless::hostSTA_IP().toString());
    obj->set(n++, "CLK> ", getTimeStr(app.clock()->getLocal(), true));
    obj->setCount(n);
};

void Display::load_psu_info(Screen *obj) {
    PsuData data = app.psu()->getInfo();
    String str = String(data.V, 3);
    if (str.length() == 5) str += " ";
    str += "V ";
    str += String(data.I, 3);
    str += " A ";
    obj->set(0, str.c_str());

    double p = data.P;
    str = String(p, p < 10 ? 3 : 2);
    str += " W ";

    double total = data.Wh;
    if (total < 1000) {
        str += String(total, total < 10 ? 3 : total < 100 ? 2 : 1);
        str += " Wh";
    } else {
        total = total / 1000;
        str += String(total, 3);
        str += "KWh";
    }
    obj->set(1, str.c_str());
    obj->setCount(2);
    obj->moveFirst();
}  // namespace Modules

void Display::load_ready(Screen *obj) {
    obj->set(0, "READY> ");
    obj->set(1, "CLK> ", getTimeStr(app.clock()->getLocal(), true).c_str());
    obj->setCount(2);
    obj->moveFirst();
}

void Display::load_psu_stat(Screen *obj) {
    obj->set(0, "MAX> ");
    obj->set(1, "MIN> ");
    obj->setCount(2);
    obj->moveFirst();
}

void Display::load_message(Screen *obj, String header, String message) {
    load_message(obj, header.c_str(), message.c_str());
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

void Display::updateScreen(void) {
    switch (activeScreen) {
        case SCREEN_CLEAR:
        case SCREEN_BOOT:
        case SCREEN_PLOT:
        case SCREEN_MESSAGE:
        case SCREEN_PSU:
            return;
        case SCREEN_STA:
            load_wifi_sta(&screen);
            return;
        case SCREEN_AP:
            load_wifi_ap(&screen);
            return;
        case SCREEN_AP_STA:
            load_wifi_ap_sta(&screen);
            return;
        case SCREEN_READY:
            load_ready(&screen);
            return;
        case SCREEN_PSU_STAT:
            load_psu_stat(&screen);
    }
}

void Display::onLoop() {
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
            updateScreen();
            screen.next();
            lastScroll = now;
        }
    }
}

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

void Display::unlock(void) { lockTimeout = 0; }

bool Display::onConfigChange(const ConfigItem param, const char* value) {
    if (param == BACKLIGHT) {
        bool enable = String(value).toInt();
        enableBacklight(enable);
    }
    return true;
}
}  // namespace Modules