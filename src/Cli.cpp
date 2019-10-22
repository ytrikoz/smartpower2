#include "Cli.h"
#include "Global.h"

#include "Actions/PowerAvg.h"
#include "Actions/WakeOnLan.h"

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
void onWol(cmd *c);
void onRestart(cmd *c);

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
    ACTION_UPTIME,
    ACTION_WOL
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
    cmdGet, cmdRm, cmdClock, cmdPlot, cmdLog, cmdWol, cmdRestart;

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

String getIpStr(Command &command) {
    return command.getArgument("ip").getValue();
}

String getMacStr(Command &command) {
    return command.getArgument("mac").getValue();
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
    } else if (strcasecmp_P(str.c_str(), str_wol) == 0) {
        return ACTION_WOL;
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
    cmdSystem.addPositionalArgument("value", "0");
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

    cmdWol = cli->addCommand("wol");
    cmdWol.addArgument("ip", "");
    cmdWol.addArgument("mac", "");
    cmdWol.setCallback(Cli::onWol);

    cmdRestart = cli->addCommand("restart");
    cmdRestart.addArgument("value", "");
    cmdRestart.setCallback(Cli::onRestart);
}

void onLog(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    String paramStr = getParamStr(cmd);
    PsuLogger *logger = app.getPsu()->getLogger();
    bool handled = false;
    if (action == ACTION_PRINT) {
        String paramStr = getParamStr(cmd);
        if (!paramStr.length()) {
            logger->printDiag(out);
            return;
        }
        if (paramStr.indexOf("p") != -1) {
            logger->print(PsuLogEnum::POWER, out);
            handled = true;
        }
        if (paramStr.indexOf("v") != -1) {
            logger->print(PsuLogEnum::VOLTAGE, out);
            handled = true;
        }
        if (paramStr.indexOf("i") != -1) {
            logger->print(PsuLogEnum::CURRENT, out);
            handled = true;
        }
        if (paramStr.indexOf("w") != -1) {
            logger->print(PsuLogEnum::WATTSHOURS, out);
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
        if (app.saveConfig()) {
            app.restart(3);
        }
        return;
    default:
        String actionStr = getActionStr(cmd);
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
        app.getPsu()->powerOn();
        break;
    }
    case ACTION_OFF: {
        app.getPsu()->powerOff();
        break;
    }
    default: {
        String actionStr = getActionStr(cmd);
        print_unknown_action(out, actionStr);
        return;
    }
    }
}

void onWol(cmd *c) {
    Command cmd(c);
    String ipStr = getIpStr(cmd);
    String macStr = getMacStr(cmd);
    Actions::WakeOnLan(out).exec(ipStr, macStr);
}

void onRestart(cmd *c) {
    Command cmd(c);
    String value = getValueStr(cmd);
    int delay = !value.length() ? 3 : value.toInt();
    app.restart(delay);
}

void onSystem(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    String value = getValueStr(cmd);
    switch (action) {
    case ACTION_BACKLIGHT: {
        bool enabled = value.toInt();
        println_nameP_value(out, str_backlight, getOnOffStr(enabled));
        app.getDisplay()->enableBacklight(enabled);
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
        return;
    }
}

void onClock(cmd *c) {
    Command cmd(c);
    String action = getActionStr(cmd);
    String paramStr = getParamStr(cmd);
    if (action.equals("set")) {
        tm tm;
        if (TimeUtils::encodeTime(paramStr.c_str(), tm)) {
            DateTime dt = DateTime(tm);
            out->println(dt);
        } else {
            out->print(StrUtils::getStrP(str_invalid));
            out->print(' ');
            out->print(StrUtils::getStrP(str_time));
            out->print(' ');
            out->println(paramStr);
        }
    }
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
    } else if (modStr.equals("app") || modStr.equals("")) {
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

void onPlot(cmd *c) { Command cmd(c); }

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

void onCommandError(cmd_error *e) {
    CommandError cmdError(e);
    out->println(cmdError.toString().c_str());
}

} // namespace Cli