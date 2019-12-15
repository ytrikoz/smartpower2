#include "Cli.h"

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
void onLed(cmd *c);

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
    ACTION_TEST,
    ACTION_SET,
    ACTION_CONTROL,
    ACTION_CONFIG,
    ACTION_SHOW
};

Command cmdConfig, cmdPower, cmdShow, cmdSystem, cmdHelp, cmdPrint, cmdSet,
    cmdGet, cmdRm, cmdClock, cmdPlot, cmdLog, cmdWol, cmdRestart, cmdRun, cmdLs,
    cmdCrash, cmdLed;

SimpleCLI* cli_;

Print *out_ = nullptr;

const String getActionStr(Command &command) {
    return command.getArgument(FPSTR(str_action)).getValue();
}

const String getParamStr(Command &command) {
    return command.getArgument(FPSTR(str_param)).getValue();
}

const String getIpStr(Command &command) {
    return command.getArgument(FPSTR(str_ip)).getValue();
}

const String getMacStr(Command &command) {
    return command.getArgument(FPSTR(str_mac)).getValue();
}

const String getModStr(Command &command) {
    return command.getArgument(FPSTR(str_mod)).getValue();
}

const String getFileStr(Command &command) {
    return command.getArgument(FPSTR(str_file)).getValue();
}

const String getPathStr(Command &command) {
    return command.getArgument(FPSTR(str_path)).getValue();
}

const String getItemStr(Command &command) {
    return command.getArgument(FPSTR(str_item)).getValue();
}

const String getValueStr(Command &command) {
    return command.getArgument(FPSTR(str_value)).getValue();
}

void onSystem(cmd *c) {
    Command cmd(c);
    String modStr = getModStr(cmd);
    String paramStr = getParamStr(cmd);
    String valueStr = getValueStr(cmd);

    Module *mod = app.module(modStr);
    if (mod) {
        if (mod->execute(paramStr, valueStr)) {
            println_done(out_);
        }
    } else {
        println_unknown_module(out_, modStr);
    }
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
    } else if (strcasecmp_P(str.c_str(), str_config) == 0) {
        return ACTION_CONFIG;
    } else if (strcasecmp_P(str.c_str(), str_set) == 0) {
        return ACTION_SET;
    } else if (strcasecmp_P(str.c_str(), str_show) == 0) {
        return ACTION_SHOW;
    }
    return ACTION_UNKNOWN;
}  // namespace Cli

bool active() { return out_ != nullptr; }

Print* setOutput(Print *out) {
    Print* prev = out_;
    out_ = out;
    return prev;
}

Print* getOutput(Print *p) {
    return out_;
}


SimpleCLI* get() {
    return cli_;
}

void init() {
    // if (BootWatcher::isSafeBooMode())
    cli_ = new SimpleCLI();

    cli_->setErrorCallback(onCommandError);

    cmdPower = cli_->addCommand("power");
    cmdPower.addPositionalArgument("action", "status");
    cmdPower.addPositionalArgument("param", "0");
    cmdPower.setCallback(Cli::onPower);

    cmdConfig = cli_->addCommand("config");
    cmdConfig.addPositionalArgument("action", "print");
    cmdConfig.addPositionalArgument("param", "");
    cmdConfig.setCallback(Cli::onConfig);

    cmdHelp = cli_->addCommand("help");
    cmdHelp.setCallback(Cli::onHelp);

    cmdPrint = cli_->addCommand("print");
    cmdPrint.addPositionalArgument("file");
    cmdPrint.setCallback(Cli::onPrint);

    cmdSystem = cli_->addCommand("system");
    cmdSystem.addPositionalArgument("mod", "");
    cmdSystem.addPositionalArgument("param", "");
    cmdSystem.addPositionalArgument("value", "");
    cmdSystem.setCallback(Cli::onSystem);

    cmdShow = cli_->addCommand("show");
    cmdShow.addPositionalArgument("mod", "app");
    cmdShow.setCallback(Cli::onShow);

    cmdSet = cli_->addCommand("set");
    cmdSet.addPositionalArgument("param");
    cmdSet.addPositionalArgument("value");
    cmdSet.setCallback(Cli::onSet);

    cmdGet = cli_->addCommand("get");
    cmdGet.addPositionalArgument("param");
    cmdGet.setCallback(Cli::onGet);

    cmdRm = cli_->addCommand("rm");
    cmdRm.addPositionalArgument("file");
    cmdRm.setCallback(Cli::onRemove);

    cmdClock = cli_->addCommand("clock");
    cmdClock.addPositionalArgument("action", "time");
    cmdClock.addPositionalArgument("param", "");
    cmdClock.setCallback(Cli::onClock);

    cmdPlot = cli_->addCommand("plot");
    cmdPlot.addPositionalArgument("action", "print");
    cmdPlot.addPositionalArgument("param", "");
    cmdPlot.setCallback(Cli::onPlot);

    cmdLog = cli_->addCommand("log");
    cmdLog.addPositionalArgument("action", "print");
    cmdLog.addPositionalArgument("param", "");
    cmdLog.setCallback(Cli::onLog);

    cmdWol = cli_->addCommand("wol");
    cmdWol.addArgument("ip", "");
    cmdWol.addArgument("mac", "");
    cmdWol.setCallback(Cli::onWol);

    cmdRestart = cli_->addCommand("restart");
    cmdRestart.addArgument("value", "");
    cmdRestart.setCallback(Cli::onRestart);

    cmdRun = cli_->addCommand("run");
    cmdRun.addPositionalArgument("file");
    cmdRun.setCallback(Cli::onRun);

    cmdLs = cli_->addCommand("ls");
    cmdLs.addPositionalArgument("path");
    cmdLs.setCallback(Cli::onLs);

    cmdCrash = cli_->addCommand("crash");
    cmdCrash.addPositionalArgument("action", "list");
    cmdCrash.addPositionalArgument("item", "*");
    cmdCrash.setCallback(Cli::onCrash);

    cmdLed = cli_->addCommand("led");
    cmdLed.addPositionalArgument("action", "show");
    cmdLed.addPositionalArgument("item", "");
    cmdLed.addPositionalArgument("param", "");
    cmdLed.addPositionalArgument("value", "");
    cmdLed.setCallback(Cli::onLed);
}

void onLed(cmd *c) {
    Command cmd(c);
    String actionStr = getActionStr(cmd);
    String itemStr = getItemStr(cmd);
    String paramStr = getParamStr(cmd);
    String valueStr = getValueStr(cmd);

    switch (getAction(cmd)) {
        case ACTION_SHOW:
            app.led()->onDiag(out_);
            break;
        case ACTION_CONFIG:
            app.led()->config(LedEnum(itemStr.toInt()), LedParamEnum(paramStr.toInt()), valueStr.toInt());
            break;
        case ACTION_SET:
            app.led()->set(LedEnum(itemStr.toInt()), LedSignal(paramStr.toInt()));
            break;
        default:
            print_unknown_action(out_, actionStr);
            break;
    }
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
            logger->print(out_);
            return;
        }
        if (paramStr.indexOf("v") != -1) {
            logger->print(out_, VOLTAGE);
            handled = true;
        }
        if (paramStr.indexOf("i") != -1) {
            logger->print(out_, CURRENT);
            handled = true;
        }
        if (!handled)
            print_unknown_action(out_, paramStr);
    }
    out_->println();
}

void onHelp(cmd *c) {
    Command cmd(c);
    println(out_, cli_->toString().c_str());
}

void onConfig(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    switch (action) {
        case ACTION_PRINT:
            app.config()->printTo(*out_);
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
            PrintUtils::print_unknown_action(out_, actionStr);
            return;
    }
    println_done(out_);
}

void onPower(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    switch (action) {
        case ACTION_AVG: {
            String paramStr = getParamStr(cmd);
            Actions::PowerAvg(paramStr.toInt()).exec(out_);
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
            print_unknown_action(out_, actionStr);
            return;
        }
    }
}

void onWol(cmd *c) {
    Command cmd(c);
    String ipStr = getIpStr(cmd);
    String macStr = getMacStr(cmd);
    Actions::WakeOnLan(out_).exec(ipStr, macStr);
}

void onRestart(cmd *c) {
    Command cmd(c);
    String value = getValueStr(cmd);
    int delay = !value.length() ? 3 : value.toInt();
    app.restart(delay);
}

void onClock(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    String paramStr = getParamStr(cmd);
    switch (action) {
        case ACTION_UPTIME: {
            char buf[16];
            TimeUtils::format_elapsed_full(app.clock()->getUptime());
            println(out_, buf);
            break;
        }
        case ACTION_TIME: {
            time_t local = app.clock()->getLocal();
            println(out_, TimeUtils::format_time(local));
            break;
        }
        default: {
            print_unknown_action(out_, getActionStr(cmd));
            return;
        }
    }

    // tm tm;
    // if (TimeUtils::encodeTime(paramStr.c_str(), tm)) {
    //     DateTime dt = DateTime(tm);
    //     out_->println(dt);
    // } else {
    //     out_->print(StrUtils::getStrP(str_invalid));
    //     out_->print(' ');
    //     out_->print(StrUtils::getStrP(str_time));
    //     out_->print(' ');
    //     out_->println(paramStr);
    // }
}

void onWifiScan(cmd *c) {
    Command cmd(c);
    out_->print(StrUtils::getIdentStrP(str_wifi));
    out_->print(StrUtils::getStrP(str_scanning));
    int8_t n = WiFi.scanNetworks();
    if (n == 0) {
        out_->print(StrUtils::getStrP(str_network));
        out_->print(StrUtils::getStrP(str_not));
        out_->print(StrUtils::getStrP(str_found));
    }
    out_->println();
    for (int i = 0; i < n; ++i) {
        out_->print(StrUtils::getIdentStrP(str_wifi));
        out_->printf("#%d %s %d", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
    }
    out_->println();
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
            PrintUtils::println_done(out_);
        }
    } else {
        PrintUtils::print_unknown_param(out_, paramStr);
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
        print_param_value(out_, paramStr.c_str(), buf);
    } else {
        print_unknown_param(out_, paramStr);
    }
}

void onShow(cmd *c) {
    Command cmd(c);
    String modStr = getModStr(cmd);
    if (modStr == "loop") {
        LoopWatcher *loopWatcher = app.watcher();
        LoopWatcherState state = loopWatcher->getState();
        switch (state) {
            case CAPTURE_IDLE:
                print(out_, FPSTR(str_capture));
                print(out_, loopWatcher->getDuration());
                println(out_, FPSTR(str_ms));
                loopWatcher->start();
                return;
            case CAPTURE_IN_PROGRESS:
                print(out_, FPSTR(str_capturing));
                print(out_, loopWatcher->getDuration());
                println(out_, FPSTR(str_ms));
                return;
            case CAPTURE_DONE: {
                LoopCapture *cap = loopWatcher->getCapture();
                print(out_, FPSTR(str_duration), cap->duration);
                size_t time_range = 2;
                for (size_t i = 0; i < cap->counters_size; ++i) {
                    if (cap->counter[i])
                        println(out_, time_range > cap->longest ? time_range : cap->longest, '\t', cap->counter[i]);
                    time_range *= 2;
                }

                if (cap->overrange)
                    println(out_, FPSTR(str_over), time_range / 2, '\t', cap->overrange, '\t', strf_lu_ms, cap->longest);

                println(out_, FPSTR(str_total), '\t', cap->total, '\t', (float)cap->duration / cap->total);

                float total_modules_time = 0;
                for (uint8_t i = 0; i < cap->modules_size; ++i)
                    total_modules_time += floor((float)cap->module[i] / ONE_MILLISECOND_mi);

                for (uint8_t i = 0; i < cap->modules_size; ++i) {
                    float load = (float)cap->module[i] / ONE_MILLISECOND_mi / total_modules_time;
                    println(out_, app.name(ModuleEnum(i)), '\t', load);
                }

                float system_time = cap->duration - total_modules_time;
                println(out_, FPSTR(str_other), '\t', (float)system_time / cap->duration * 100);

                loopWatcher->setIdle();
                return;
            }
        }
    } else if (modStr == "app") {
        app.printDiag(out_);
        return;
    }
}

void onPlot(cmd *c) { Command cmd(c); }

void onPrint(cmd *c) {
    Command cmd(c);
    auto file = getFileStr(cmd);
    if (SPIFFS.exists(file)) {
        auto f = SPIFFS.open(file, "r");
        while (f.available())
            print(out_, f.readString());
        f.close();
    } else {
        print_file_not_found(out_, file);
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
        println(out_, dir.fileName(), '\t', prettyBytes(dir.fileSize()));
    }
}

void onRemove(cmd *c) {
    Command cmd(c);
    String name = getFileStr(cmd);
    if (SPIFFS.exists(name)) {
        print(out_, FPSTR(str_file));
        if (SPIFFS.remove(name)) {
            println(out_, FPSTR(str_deleted));
        } else {
            println(out_, FPSTR(str_failed));
        }
    } else {
        print_file_not_found(out_, name);
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
                app.shell()->run(buf.c_str());
            }
        }
    } else {
        println(out_, FPSTR(str_failed));
    }
}

void onCrash(cmd *c) {
    Command cmd(c);
    String item = getItemStr(cmd);
    switch (getAction(cmd)) {
        case ACTION_LIST: {
            FSUtils::printDir(out_, CRASH_ROOT);
            break;
        }
        case ACTION_PRINT: {
            File f = SPIFFS.open(item, "r");
            if (!f)
                print_not_found(out_, item);
            else
                CrashReport(f).printTo(*out_);
            break;
        }
        case ACTION_CLEAR: {
            FSUtils::clearDir(out_, CRASH_ROOT);
            break;
        }
        case ACTION_TEST: {
            int val = 0;
            int res = 1 / val;
            println(out_, res);
            break;
        }
        case ACTION_UNKNOWN:
        default:
            print_unknown_action(out_, getActionStr(cmd));
            break;
    }
}

void onCommandError(cmd_error *e) {
    CommandError cmdError(e);
    out_->println(cmdError.toString());
}

}  // namespace Cli