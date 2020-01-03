#include "Modules/Display.h"

#include "Modules/Psu.h"
#include "Utils/TimeUtils.h"
#include "Global.h"

namespace Modules {

bool Display::isEnabled() {
    return lcd && lcd->connected();
}

bool Display::onInit() {
    lcd = new LcdDisplay();
    lastUpdated = lockTimeout = lockUpdated = 0;
    backlight_ = config_->asBool(BACKLIGHT);
    backlightTime_ = 0;
    bool res = lcd->connect();
    if (res) {
        enableBacklight(backlight_);
    } else {
        setError(ERROR_INIT, String(FPSTR(str_not_found)).c_str());
    }
    return res;
}

void Display::showProgress(uint8_t per, const char *str) {
    lcd->loadBank(BANK_PROGRESS);
    static uint8_t last = 0;
    if (per == 0) lcd->drawTextCenter(LCD_ROW_1, str);
    lcd->drawProgressBar(LCD_ROW_2, last);
    while (per > last) {
        last += 5;
        delay(250);
        lcd->drawProgressBar(LCD_ROW_2, last);
    }
    if (last > 0) lcd->drawTextCenter(LCD_ROW_1, str);
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

void Display::clear(void) { 
    activeScreen = SCREEN_CLEAR; 
}

void Display::show_network(const NetworkMode mode) {
    lcd->loadBank(BANK_NONE);
    size_t n = 0;
    screen.set(n++, "READY> ", TimeUtils::format_time(app.clock()->local()));
    if (mode == NETWORK_AP) {
        screen.set(n++, "SSID ", NetUtils::getApSsid());
        screen.set(n++, "PWD ", NetUtils::getApPasswd());
    } else if (mode == NETWORK_STA) {
        screen.set(n++, "WiFi ", NetUtils::getStaStatus());
        screen.set(n++, "RSSI ", NetUtils::getRssi());
    } else if (mode == NETWORK_AP_STA) {
        screen.set(n++, "WiFi ", NetUtils::getStaStatus());
        screen.set(n++, "RSSI ", NetUtils::getRssi());
        screen.set(n++, "STA ", NetUtils::getStaSsid());
        screen.set(n++, "STA IP ", NetUtils::getStaIp().toString());
        screen.set(n++, "AP ", NetUtils::getApSsid());
        screen.set(n++, "AP IP ", NetUtils::getApIp().toString());
    }
    screen.setCount(n);
    screen.moveFirst();
}

void Display::show_psu_data(const PsuData &data) {
    setScreen(SCREEN_PSU);
    String str = String(data.V, 3);
    if (str.length() == 5) str += " ";
    str += "V ";
    str += String(data.I, 3);
    str += " A ";
    screen.set(0, str.c_str());

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
    screen.set(1, str.c_str());
    screen.setCount(2);
    screen.moveFirst();
}

void Display::show_psu_stat() {
    screen.set(0, "MAX> ");
    screen.set(1, "MIN> ");
    screen.setCount(2);
    screen.moveFirst();
}

void Display::setScreen(ScreenEnum value) {
    if (activeScreen == value) return;
    screenUpdated = lastScroll = screenRedraw = millis();
    activeScreen = value;
    lcd->clear();
}

void Display::show_message(const char *header, const char *message) {
    setScreen(SCREEN_MESSAGE);
    screen.set(0, header);
    screen.set(1, message);
    screen.setCount(2);
    screen.moveFirst();
}

void Display::showPlot(PlotSummary *data, size_t cols) {
    lcd->drawPlot(data, 8 - cols);
    lock(15000);
}

void Display::onLoop() {
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

void Display::lock(unsigned long time) {
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

bool Display::onConfigChange(const ConfigItem param, const String &value) {
    if (param == BACKLIGHT) {
        bool enable = String(value).toInt();
        enableBacklight(enable);
    }
    return true;
}

}  // namespace Modules