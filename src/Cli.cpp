#include "Cli.h"
#include "Global.h"

#include "Actions/ClockSetTime.h"
#include "Actions/LogPrint.h"
#include "Actions/PlotPrint.h"
#include "Actions/PowerAvg.h"
#include "Actions/PowerOff.h"
#include "Actions/PowerOn.h"
#include "Actions/ShowClients.h"
#include "Actions/ShowClock.h"
#include "Actions/ShowDiag.h"
#include "Actions/ShowLoop.h"
#include "Actions/ShowNtp.h"
#include "Actions/ShowPsu.h"
#include "Actions/ShowStatus.h"
#include "Actions/ShowWifi.h"
#include "Actions/SwitchBacklight.h"

#include "PrintUtils.h"
#include "StrUtils.h"
#include "TimeUtils.h"

namespace Cli {

void onCommandError(cmd_error *e);
void onConfig(cmd *c);
void onPower(cmd *c);
void onShow(cmd *c);
void onHelp(cmd *c);
void onPrint(cmd *c);
void onPlot(cmd *c);
void onRemove(cmd *c);
void onSet(cmd *c);
void onGet(cmd *c);
void onSystem(cmd *c);
void onWifiScan(cmd *c);
void onWifiDiag(cmd *c);
void onClock(cmd *c);
void onLog(cmd *c);

void onGetConfigParameter(const char *, const char *);
void onConfigParameterChanged(const char *, const char *, const char *);
void onIOResult(const char *, const char *);

enum CommandAction {
    ACTION_UNKNOWN,
    ACTION_PRINT,
    ACTION_RESET,
    ACTION_SAVE,
    ACTION_LOAD,
    ACTION_APPLY,
    ACTION_STATUS,
    ACTION_LOG,
    ACTION_AVG,
    ACTION_ON,
    ACTION_OFF,
    ACTION_RESTART,
    ACTION_BACKLIGHT,
    ACTION_UPTIME
};

enum CommandItems {
    ITEM_CLIENT,
    ITEM_CLOCK,
    ITEM_POWER,
    ITEM_LOG,
    ITEM_NETWORK,
    ITEM_INFO,
    ITEM_STATUS,
    ITEM_LOOP,
    ITEM_NTP,
    ITEM_DIAG,
    ITEM_WIFI,
};

Command cmdConfig, cmdPower, cmdShow, cmdSystem, cmdHelp, cmdPrint, cmdSet,
    cmdGet, cmdRm, cmdClock, cmdPlot, cmdLog;

Print* out = NULL;

Print* getOutput() {
    if (!out) { out = &Serial; }
    return out;
 }

String getActionStr(Command& command) {
    return command.getArgument("action").getValue();
}
String getParamStr(Command& command) {
    return command.getArgument("param").getValue();
}
String getFileStr(Command& command) {
    return command.getArgument("file").getValue();
}
String getItemStr(Command& command) {
    return command.getArgument("item").getValue();
}
String getValueStr(Command& command) {
    return command.getArgument("valueStr").getValue();
}

CommandAction getAction(Command& cmd) {
    String str = getActionStr(cmd);
    if (strcasecmp_P(str.c_str(), str_print) == 0) {
        return ACTION_PRINT;
    } else if (strcasecmp_P(str.c_str(), str_reset) == 0) {
        return ACTION_RESET;
    } else if (strcasecmp_P(str.c_str(), str_save) == 0) {
        return ACTION_SAVE;
    } else if (strcasecmp_P(str.c_str(), str_load) == 0) {
        return ACTION_LOAD;
    } else if (strcasecmp_P(str.c_str(), str_apply) == 0) {
        return ACTION_APPLY;
    } else if (strcasecmp_P(str.c_str(), str_status) == 0) {
        return ACTION_STATUS;
    } else if (strcasecmp_P(str.c_str(), str_log) == 0) {
        return ACTION_LOG;
    } else if (strcasecmp_P(str.c_str(), str_avg) == 0) {
        return ACTION_AVG;
    } else if (strcasecmp_P(str.c_str(), str_on) == 0) {
        return ACTION_ON;
    } else if (strcasecmp_P(str.c_str(), str_off) == 0) {
        return ACTION_OFF;
    } else if (strcasecmp_P(str.c_str(), str_restart) == 0) {
        return ACTION_RESTART;
    } else if (strcasecmp_P(str.c_str(), str_backlight) == 0) {
        return ACTION_BACKLIGHT;
    } else if (strcasecmp_P(str.c_str(), str_uptime) == 0) {
        return ACTION_UPTIME;
    }
    return ACTION_UNKNOWN;
}

bool active() { return out != NULL; }

void setOutput(Print* p) {
    if (out != NULL) {
        out->println(StrUtils::getStrP(msg_session_interrupted));
        out = NULL;
    }
    out = p;
}

void close() {
    out->println(StrUtils::getStrP(msg_shell_start_hint, false));
    out = NULL;
}

void init() {
    cli = new SimpleCLI();
    cli->setErrorCallback(onCommandError);

    cmdPower = cli->addCommand("power");
    cmdPower.addPositionalArgument("action", "status");
    cmdPower.addPositionalArgument("param", "0");
    cmdPower.setCallback(Cli::onPower);

    cmdConfig = cli->addCommand("config");
    cmdConfig.addPositionalArgument("action", "print");
    cmdConfig.addPositionalArgument("param", "");
    cmdConfig.setCallback(Cli::onConfig);

    cmdHelp = cli->addCommand("help");
    cmdHelp.setCallback(Cli::onHelp);

    cmdPrint = cli->addCommand("print");
    cmdPrint.addPositionalArgument("file");
    cmdPrint.setCallback(Cli::onPrint);

    cmdSystem = cli->addCommand("system");
    cmdSystem.addPositionalArgument("action", "uptime");
    cmdSystem.addPositionalArgument("param", "");
    cmdSystem.setCallback(Cli::onSystem);

    cmdShow = cli->addCommand("show");
    cmdShow.addPositionalArgument("item", "status");
    cmdShow.setCallback(Cli::onShow);

    cmdSet = cli->addCommand("set");
    cmdSet.addPositionalArgument("param");
    cmdSet.addPositionalArgument("valueStr");
    cmdSet.setCallback(Cli::onSet);

    cmdGet = cli->addCommand("get");
    cmdGet.addPositionalArgument("param");
    cmdGet.setCallback(Cli::onGet);

    cmdRm = cli->addCommand("rm");
    cmdRm.addPositionalArgument("file");
    cmdRm.setCallback(Cli::onRemove);

    cmdClock = cli->addCommand("clock");
    cmdClock.addPositionalArgument("action");
    cmdClock.addPositionalArgument("param", "");
    cmdClock.setCallback(Cli::onClock);

    cmdPlot = cli->addCommand("plot");
    cmdPlot.addPositionalArgument("action", "print");
    cmdPlot.addPositionalArgument("param", "");
    cmdPlot.setCallback(Cli::onPlot);

    cmdLog = cli->addCommand("log");
    cmdLog.addPositionalArgument("action", "print");
    cmdLog.addPositionalArgument("param", "");
    cmdLog.setCallback(Cli::onLog);
}

void onGetConfigParameter(const char* name, const char* valueStr) {
    char buf[INPUT_MAX_LENGTH];
    sprintf(buf, "%s=\"%s\"", name, valueStr);
    out->println(buf);
}

void onConfigParameterChanged(const char* paramStr, const char* old_value,
                              const char* new_value) {
    char buf[OUTPUT_MAX_LENGTH];
    sprintf_P(buf, strf_config_param_changed, new_value, paramStr);
    getOutput()->print(buf);
}

void print_file_not_found(Print* p, String& fileStr) {
    p->print(StrUtils::getStrP(str_file));
    p->print(StrUtils::getQuotedStr(fileStr));
    p->print(StrUtils::getStrP(str_not));
    p->print(StrUtils::getStrP(str_found));
}

void println(Print* p) { p->println(); }

void println() {
    if (out) println(out);
}

void print_done(Print* p) { p->print(StrUtils::getStrP(str_done)); };

void print_done(String& action, String& param) {
    out->print(action.c_str());
    out->print(' ');
    out->print(param.c_str());
    out->print(": ");
}

void onCommandError(cmd_error* e) {
    CommandError cmdError(e);
    out->println(cmdError.toString().c_str());
}

void onLog(cmd* c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    bool handled = false;
    if (action == ACTION_PRINT) {
        String paramStr = getParamStr(cmd);
        if (!paramStr.length()) {
            psuLogger->printDiag(out);
            return;
        }
        if (paramStr.indexOf("p") != -1) {
            psuLogger->print(out, POWER_LOG);
            handled = true;
        }
        if (paramStr.indexOf("v") != -1) {
            psuLogger->print(out, VOLTAGE_LOG);
            handled = true;
        }
        if (paramStr.indexOf("i") != -1) {
            psuLogger->print(out, CURRENT_LOG);
            handled = true;
        }
        if (paramStr.indexOf("wh") != -1) {
            psuLogger->print(out, WATTSHOURS_LOG);
            handled = true;
        }
        if (!handled) PrintUtils::print_unknown_param(out, paramStr);
    }
    out->println();
}

void onHelp(cmd* c) {
    Command cmd(c);
    out->println(cli->toString().c_str());
}

void onConfig(cmd* c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    String actionStr = getActionStr(cmd);

    switch (action) {
        case ACTION_PRINT:
            app.printConfig(out);
            return;
        case ACTION_RESET:
            app.resetConfig();
            break;
        case ACTION_SAVE:
            app.saveConfig();
            break;
        case ACTION_LOAD:
            app.loadConfig();
            break;
        case ACTION_APPLY:
            if (app.saveConfig()) app.restart(3);
            break;
        default:
            PrintUtils::print_unknown_action(out, actionStr);
            println();
            return;
    }
    print_done(out);
    println();
}

void onPower(cmd* c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    String actionStr = getActionStr(cmd);
    switch (action) {
        case ACTION_AVG: {
            String param = getParamStr(cmd);
            Actions::PowerAvg(param.toInt()).exec(out);
            break;
        }
        case ACTION_ON: {
            Actions::PowerOn().exec(out);
            break;
        }
        case ACTION_OFF: {
            Actions::PowerOff().exec(out);
            break;
        }
        default: {
            PrintUtils::print_unknown_action(out, actionStr);
            out->println();
            return;
        }
    }
}

void onSystem(cmd* c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    String param = getParamStr(cmd);
    String actionStr = getActionStr(cmd);

    switch (action) {
        case ACTION_RESTART: {
            out->print(FPSTR(msg_system_restart));
            out->println();
            app.restart(param.toInt());
            break;
        }
        case ACTION_BACKLIGHT: {
            bool enabled = param.length() == 0 ? true : param.toInt();
            Actions::SwitchBacklight(enabled).exec(out);
            break;
        }
        case ACTION_UPTIME: {
            char buf[16];
            out->println(
                TimeUtils::getTimeFormated(buf, app.getClock()->getUptime()));
            break;
        }
        default:
            PrintUtils::print_unknown_action(out, actionStr);
            out->println();
            return;
    }
}

void onClock(cmd* c) {
    Command cmd(c);
    String action = getActionStr(cmd);
    String param = getParamStr(cmd);
    if (action.equals("set")) Actions::ClockSetTime(param).exec(out);
}

void onWifiScan(cmd* c) {
    Command cmd(c);
    out->print(StrUtils::getIdentStrP(str_wifi));
    out->print(StrUtils::getStrP(str_scanning));
    int8_t n = WiFi.scanNetworks();
    if (n == 0) {
        out->print(StrUtils::getStrP(str_network));
        out->print(StrUtils::getStrP(str_not));
        out->print(StrUtils::getStrP(str_found));
    }
    out->println();
    for (int i = 0; i < n; ++i) {
        out->print(StrUtils::getIdentStrP(str_wifi));
        out->printf("#%d %s %d", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
    }
    out->println();
}

void onSet(cmd* c) {
    Command cmd(c);
    String paramStr = getParamStr(cmd);
    String valueStr = getValueStr(cmd);
    ConfigItem param;
    size_t size;
    Config* cfg = app.getConfig();
    if (cfg->getConfig(paramStr.c_str(), param, size)) {
        char buf[size];
        StrUtils::setstr(buf, cfg->getValueAsString(param), size);
        if (cfg->setValueString(param, valueStr.c_str())) {
            onConfigParameterChanged(paramStr.c_str(), buf, valueStr.c_str());
        } else {
            out->printf_P(strf_config_param_unchanged, paramStr.c_str());
        }
    } else {
        PrintUtils::print_unknown_param(out, paramStr);
    }
    out->println();
}

void onGet(cmd* c) {
    Command cmd(c);
    String paramStr = getParamStr(cmd);
    Config* cfg = app.getConfig();
    ConfigItem param;
    size_t value_size = 0;
    if (cfg->getConfig(paramStr.c_str(), param, value_size)) {
        char valueStr[value_size + 1];
        StrUtils::setstr(valueStr, cfg->getValueAsString(param),
                         value_size + 1);
        onGetConfigParameter(paramStr.c_str(), valueStr);
    } else {
        PrintUtils::print_unknown_param(out, paramStr);
        out->println();
    }
}

void onShow(cmd* c) {
    Command cmd(c);
    String item = getItemStr(cmd);
    if (item.equals("clients")) {
        Actions::ShowClients().exec(out);
    } else if (item.equals("clock")) {
        Actions::ShowClock().exec(out);
    } else if (item.equals("power")) {
        Actions::ShowPsu().exec(out);
    } else if (item.equals("log")) {
        psuLogger->printDiag(out);
    } else if (item.equals("network")) {
        out->println(getNetworkInfoJson().c_str());
    } else if (item.equals("system")) {
        out->println(getSystemInfoJson().c_str());
    } else if (item.equals("status")) {
        Actions::ShowStatus().exec(out);
    } else if (item.equals("loop")) {
        Actions::ShowLoop().exec(out);
    } else if (item.equals("ntp")) {
        Actions::ShowNtp().exec(out);
    } else if (item.equals("wifi")) {
        Actions::ShowWifi().exec(out);
    } else {
        PrintUtils::print_unknown_item(out, item);
    }
}

void onPlot(cmd* c) {
    Command cmd(c);
    PlotData* data = app.getDisplay()->getData();
    for (uint8_t x = 0; x < app.getDisplay()->getData()->size; ++x) {
        uint8_t y = map_to_plot_min_max(data, x);
        char tmp[PLOT_ROWS * 8 + 1];
        StrUtils::strfill(tmp, '*', y);
        out->printf("#%d %2.4f ", x + 1, data->cols[x]);
        out->print(tmp);
        out->println();
    }
}

void onPrint(cmd* c) {
    Command cmd(c);
    String file = getFileStr(cmd);
    if (SPIFFS.exists(file)) {
        File f = SPIFFS.open(file, "r");
        while (f.available()) out->println(f.readString());
        f.close();
    } else {
        print_file_not_found(out, file);
    }
}

void onRemove(cmd* c) {
    Command cmd(c);
    String file = getFileStr(cmd);
    if (SPIFFS.exists(file)) {
        out->print(StrUtils::getStrP(str_file));
        out->print(file);
        if (SPIFFS.remove(file)) {
            out->print(StrUtils::getStrP(str_deleted));
        } else {
            out->print(StrUtils::getStrP(str_failed));
        }
    } else {
        out->print(StrUtils::getStrP(str_not));
        out->print(StrUtils::getStrP(str_found));
    }
}

}  // namespace Cli