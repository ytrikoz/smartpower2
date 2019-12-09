#include "Cli.h"

#include <SimpleCLI.h>

#include "Actions/PowerAvg.h"
#include "Actions/WakeOnLan.h"

#include "main.h"
#include "CrashReport.h"

using namespace PrintUtils;
using namespace StrUtils;
using namespace TimeUtils;

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
void onRun(cmd *c);
void onLs(cmd *c);
void onCrash(cmd *c);

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
    ACTION_TIME,
    ACTION_WOL,
    ACTION_LIST,
    ACTION_CLEAR,
    ACTION_TEST
};

Command cmdConfig, cmdPower, cmdShow, cmdSystem, cmdHelp, cmdPrint, cmdSet,
    cmdGet, cmdRm, cmdClock, cmdPlot, cmdLog, cmdWol, cmdRestart, cmdRun, cmdLs,
    cmdCrash;

Print *out = NULL;

Print *getOutput() {
    if (!out)
        out = &Serial;
    return out;
}

const String getActionStr(Command &command) {
    return command.getArgument("action").getValue();
}

const String getParamStr(Command &command) {
    return command.getArgument("param").getValue();
}

const String getIpStr(Command &command) {
    return command.getArgument("ip").getValue();
}

const String getMacStr(Command &command) {
    return command.getArgument("mac").getValue();
}

const String getModStr(Command &command) {
    return command.getArgument("mod").getValue();
}

const String getFileStr(Command &command) {
    return command.getArgument("file").getValue();
}

const String getPathStr(Command &command) {
    return command.getArgument("path").getValue();
}

const String getItemStr(Command &command) {
    return command.getArgument("item").getValue();
}

const String getValueStr(Command &command) {
    return command.getArgument(FPSTR(str_value)).getValue();
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
    } else if (strcasecmp_P(str.c_str(), str_time) == 0) {
        return ACTION_TIME;
    } else if (strcasecmp_P(str.c_str(), str_wol) == 0) {
        return ACTION_WOL;
    } else if (strcasecmp_P(str.c_str(), str_list) == 0) {
        return ACTION_LIST;
    } else if (strcasecmp_P(str.c_str(), str_clear) == 0) {
        return ACTION_CLEAR;
    } else if (strcasecmp_P(str.c_str(), str_test) == 0) {
        return ACTION_TEST;
    }
    return ACTION_UNKNOWN;
}  // namespace Cli

bool active() { return out != NULL; }

void setOutput(Print *p) {
    if (out != NULL) {
        print_shell_interrupted(out);
        out = NULL;
    }
    out = p;
}

void close() {
    if (out != NULL) {
        print_shell_exit(out);
        out = NULL;
    }
}

void init() {
    // if (BootWatcher::isSafeBooMode())

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
    cmdShow.addPositionalArgument("mod", "app");
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
    cmdClock.addPositionalArgument("action", "time");
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

    cmdRun = cli->addCommand("run");
    cmdRun.addPositionalArgument("file");
    cmdRun.setCallback(Cli::onRun);

    cmdLs = cli->addCommand("ls");
    cmdLs.addPositionalArgument("path");
    cmdLs.setCallback(Cli::onLs);

    cmdCrash = cli->addCommand("crash");
    cmdCrash.addPositionalArgument("action", "list");
    cmdCrash.addPositionalArgument("item", "*");
    cmdCrash.setCallback(Cli::onCrash);
}

void onLog(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    String paramStr = getParamStr(cmd);
    PsuLogHelper *logger = app.getPsuLog();
    bool handled = false;
    if (action == ACTION_PRINT) {
        String paramStr = getParamStr(cmd);
        if (!paramStr.length()) {
            logger->print(out);
            return;
        }
        if (paramStr.indexOf("v") != -1) {
            logger->print(out, VOLTAGE);
            handled = true;
        }
        if (paramStr.indexOf("i") != -1) {
            logger->print(out, CURRENT);
            handled = true;
        }
        if (!handled)
            print_unknown_action(out, paramStr);
    }
    out->println();
}

void onHelp(cmd *c) {
    Command cmd(c);
    println(out, cli->toString().c_str());
}

void onConfig(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    switch (action) {
        case ACTION_PRINT:
            app.config()->printTo(*out);
            return;
        case ACTION_RESET:
            app.config()->setDefault();
            break;
        case ACTION_SAVE:
            app.config()->save();
            break;
        case ACTION_LOAD:
            app.config()->load();
            break;
        case ACTION_APPLY:
            if (app.config()->save())
                app.restart(3);
            return;
        default:
            String actionStr = getActionStr(cmd);
            PrintUtils::print_unknown_action(out, actionStr);
            return;
    }
    println_done(out);
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
            app.psu()->powerOn();
            break;
        }
        case ACTION_OFF: {
            app.psu()->powerOff();
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
            app.lcd()->enableBacklight(enabled);
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
    CommandAction action = getAction(cmd);
    String paramStr = getParamStr(cmd);
    switch (action) {
        case ACTION_UPTIME: {
            char buf[16];
            TimeUtils::format_elapsed_time(buf, app.clock()->getUptime());
            println(out, buf);
            break;
        }
        case ACTION_TIME: {
            time_t local = app.clock()->getLocal();
            println(out, TimeUtils::format_time(local));
            break;
        }
        default: {
            print_unknown_action(out, getActionStr(cmd));
            return;
        }
    }

    // tm tm;
    // if (TimeUtils::encodeTime(paramStr.c_str(), tm)) {
    //     DateTime dt = DateTime(tm);
    //     out->println(dt);
    // } else {
    //     out->print(StrUtils::getStrP(str_invalid));
    //     out->print(' ');
    //     out->print(StrUtils::getStrP(str_time));
    //     out->print(' ');
    //     out->println(paramStr);
    // }
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
    Config *cfg = app.params();
    if (cfg->getConfig(paramStr.c_str(), param, size)) {
        if (cfg->setValueAsString(param, valueStr)) {
            PrintUtils::println_done(out);
        }
    } else {
        PrintUtils::print_unknown_param(out, paramStr);
    }
}

void onGet(cmd *c) {
    Command cmd(c);
    String paramStr = getParamStr(cmd);
    Config *cfg = app.params();
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
    if (modStr == "loop") {
        LoopWatcherState state = loopWatcher->getState();
        switch (state) {
            case CAPTURE_IDLE:
                print(out, FPSTR(str_capture));
                print(out, loopWatcher->getDuration());
                println(out, FPSTR(str_ms));
                loopWatcher->start();
                return;
            case CAPTURE_IN_PROGRESS:
                print(out, FPSTR(str_capturing));
                print(out, loopWatcher->getDuration());
                println(out, FPSTR(str_ms));
                return;
            case CAPTURE_DONE:
                app.printLoopCapture(out);
                loopWatcher->setIdle();
                return;
        };
        return;
    } else if (modStr == "app") {
        app.printDiag(out);
        return;
    }

    auto mod = app.getModuleByName(modStr.c_str());
    if (mod)
        mod->printDiag(out);
    else
        print_not_found(out, modStr);
}  // namespace Cli

void onPlot(cmd *c) { Command cmd(c); }

void onPrint(cmd *c) {
    Command cmd(c);
    auto file = getFileStr(cmd);
    if (SPIFFS.exists(file)) {
        auto f = SPIFFS.open(file, "r");
        while (f.available())
            print(out, f.readString());
        f.close();
    } else {
        print_file_not_found(out, file);
    }
}

void onLs(cmd *c) {
    Command cmd(c);
    String str = getPathStr(cmd);
    String path = FSUtils::asDir(str);
    uint8_t max_level = FSUtils::getNestedLevel(path);
    auto dir = SPIFFS.openDir(path);
    while (dir.next()) {
        String name = dir.fileName();
        if (FSUtils::getNestedLevel(name) > max_level)
            continue;
        println(out, dir.fileName(), '\t', prettyBytes(dir.fileSize()));
    }
}

void onRemove(cmd *c) {
    Command cmd(c);
    String name = getFileStr(cmd);
    if (SPIFFS.exists(name)) {
        print(out, FPSTR(str_file));
        if (SPIFFS.remove(name)) {
            println(out, FPSTR(str_deleted));
        } else {
            println(out, FPSTR(str_failed));
        }
    } else {
        print_file_not_found(out, name);
    }
}

void onRun(cmd *c) {
    Command cmd(c);
    auto name = getFileStr(cmd);
    auto file = StringFile(name);
    auto data = file.get();
    if (file.read()) {
        if (data->available()) {
            String buf;
            while (data->pop(buf)) {
                app.shell()->run(buf);
            }
        }
    } else {
        println(out, FPSTR(str_failed));
    }
}

void onCrash(cmd *c) {
    Command cmd(c);
    String item = getItemStr(cmd);
    switch (getAction(cmd)) {
        case ACTION_LIST: {
            FSUtils::printDir(out, CRASH_ROOT);
            break;
        }
        case ACTION_PRINT: {
            File f = SPIFFS.open(item, "r");
            if (!f)
                print_not_found(out, item);
            else
                CrashReport(f).printTo(*out);
            break;
        }
        case ACTION_CLEAR:
            FSUtils::clearDir(out, CRASH_ROOT);
            break;
        case ACTION_TEST: {
            int val = 0;
            int res = 1 / val;
            println(out, res);
        } break;
        case ACTION_UNKNOWN:
        default:
            print_unknown_action(out, getActionStr(cmd));
            break;
    }
}

void onCommandError(cmd_error *e) {
    CommandError cmdError(e);
    out->println(cmdError.toString());
}

}  // namespace Cli