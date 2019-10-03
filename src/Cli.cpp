#include "Cli.h"
#include "Global.h"

#include "Actions/ClockSetTime.h"
#include "Actions/LogPrint.h"
#include "Actions/PlotPrint.h"
#include "Actions/PowerAvg.h"

#include "PrintUtils.h"
#include "StrUtils.h"
#include "TimeUtils.h"

using namespace StrUtils;
using namespace PrintUtils;

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

Print *out = NULL;

Print *getOutput() {
    if (!out) {
        out = &Serial;
    }
    return out;
}

String getActionStr(Command &command) {
    return command.getArgument("action").getValue();
}
String getParamStr(Command &command) {
    return command.getArgument("param").getValue();
}

String getModStr(Command &command) {
    return command.getArgument("mod").getValue();
}
String getFileStr(Command &command) {
    return command.getArgument("file").getValue();
}
String getItemStr(Command &command) {
    return command.getArgument("item").getValue();
}
String getValueStr(Command &command) {
    return command.getArgument("value").getValue();
}

CommandAction getAction(Command &cmd) {
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

void setOutput(Print *p) {
    if (out != NULL) {
        PrintUtils::print_shell_interrupted(out);
        out = NULL;
    }
    out = p;
}

void close() {
    if (out != NULL) {
        PrintUtils::print_shell_quit(out);
        out = NULL;
    }
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
    cmdSystem.addPositionalArgument("value", "");
    cmdSystem.setCallback(Cli::onSystem);

    cmdShow = cli->addCommand("show");
    cmdShow.addPositionalArgument("mod", "");
    cmdShow.setCallback(Cli::onShow);

    cmdSet = cli->addCommand("set");
    cmdSet.addPositionalArgument("param");
    cmdSet.addPositionalArgument("value");
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

void onCommandError(cmd_error *e) {
    CommandError cmdError(e);
    out->println(cmdError.toString().c_str());
}

void onLog(cmd *c) {
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
        if (!handled)
            print_unknown_action(out, paramStr);
    }
    out->println();
}

void onHelp(cmd *c) {
    Command cmd(c);
    out->println(cli->toString().c_str());
}

void onConfig(cmd *c) {
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
        if (app.saveConfig())
            app.restart(3);
        break;
    default:
        PrintUtils::print_unknown_action(out, actionStr);
        return;
    }
    PrintUtils::print_done(out);
}

void onPower(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    switch (action) {
    case ACTION_AVG: {
        String paramStr = getParamStr(cmd);
        Actions::PowerAvg(paramStr.toInt()).exec(out);
        break;
    }
    case ACTION_ON: {
        if (app.getPsuState()->getPower(POWER_OFF))
            app.getPsu()->togglePower();
        break;
    }
    case ACTION_OFF: {
        if (app.getPsuState()->getPower(POWER_ON))
            app.getPsu()->togglePower();
        break;
    }
    default: {
        String actionStr = getActionStr(cmd);
        print_unknown_action(out, actionStr);
        return;
    }
    }
}

void onSystem(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    String paramStr = getParamStr(cmd);

    switch (action) {
    case ACTION_RESTART: {
        uint8_t delay = paramStr.toInt();
        app.restart(delay);
        break;
    }
    case ACTION_BACKLIGHT: {
        bool enabled = paramStr.length() == 0 ? true : paramStr.toInt();
        print_nameP_value(out, str_backlight, getEnabledStr(enabled));
        if (enabled)
            app.getDisplay()->backlightOn();
        else
            app.getDisplay()->backlightOff();
        break;
    }
    case ACTION_UPTIME: {
        char buf[16];
        out->println(
            TimeUtils::getTimeFormated(buf, app.getClock()->getUptime()));
        break;
    }
    default:
        String actionStr = getActionStr(cmd);
        print_unknown_action(out, actionStr);
        out->println();
        return;
    }
}

void onClock(cmd *c) {
    Command cmd(c);
    String action = getActionStr(cmd);
    String paramStr = getParamStr(cmd);
    if (action.equals("set"))
        Actions::ClockSetTime(paramStr).exec(out);
}

void onWifiScan(cmd *c) {
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

void onSet(cmd *c) {
    Command cmd(c);
    String paramStr = getParamStr(cmd);
    String valueStr = getValueStr(cmd);
    ConfigItem param;
    size_t size;
    Config *cfg = app.getConfig();
    if (cfg->getConfig(paramStr.c_str(), param, size)) {
        if (cfg->setValueString(param, valueStr)) {
            PrintUtils::print_done(out);
        }
    } else {
        PrintUtils::print_unknown_param(out, paramStr);
    }
}

void onGet(cmd *c) {
    Command cmd(c);
    String paramStr = getParamStr(cmd);
    Config *cfg = app.getConfig();
    ConfigItem param;
    size_t size = 0;
    if (cfg->getConfig(paramStr.c_str(), param, size)) {
        char buf[size + 1];
        setstr(buf, cfg->getValueAsString(param), size + 1);
        print_param_value(out, paramStr.c_str(), buf);
    } else {
        print_unknown_param(out, paramStr);
    }
}

void onShow(cmd *c) {
    Command cmd(c);
    String modStr = getModStr(cmd);
    if (modStr.equals("loop")) {
        LoopLogger *logger = app.getLoopLogger();
        LoopLoggerState state = logger->getState();
        switch (state) {
        case CAPTURE_IDLE:
            out->print(StrUtils::getStrP(str_start));
            out->print(StrUtils::getStrP(str_capture));
            out->printf_P(strf_lu_ms, logger->getDuration());
            out->println();
            app.getLoopLogger()->start();
            return;
        case CAPTURE_IN_PROGRESS:
            out->print(StrUtils::getStrP(str_capturing));
            out->printf_P(strf_lu_ms, logger->getDuration());
            out->println();
            return;
        case CAPTURE_DONE:
            app.printLoopCapture(out);
            logger->setIdle();
            return;
        };
        return;
    } else if (modStr.equals("app")) {
        app.printDiag(out);
        return;
    }
    AppModuleEnum mod;
    if (app.getModule(modStr.c_str(), mod)) {
        app.printDiag(out, mod);
    } else {
        print_s_not_found(out, modStr);
    }
}

void onPlot(cmd *c) {
    Command cmd(c);
    PlotData *data = app.getDisplay()->getData();
    for (uint8_t x = 0; x < app.getDisplay()->getData()->size; ++x) {
        uint8_t y = map_to_plot_min_max(data, x);
        char tmp[PLOT_ROWS * 8 + 1];
        StrUtils::strfill(tmp, '*', y);
        out->printf("#%d %2.4f ", x + 1, data->cols[x]);
        out->print(tmp);
        out->println();
    }
}

void onPrint(cmd *c) {
    Command cmd(c);
    String file = getFileStr(cmd);
    if (SPIFFS.exists(file)) {
        File f = SPIFFS.open(file, "r");
        while (f.available())
            out->println(f.readString());
        f.close();
    } else {
        PrintUtils::print_file_not_found(out, file);
    }
}

void onRemove(cmd *c) {
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

} // namespace Cli