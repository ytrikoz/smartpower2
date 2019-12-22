#include "Cli/Cli.h"

#include "Actions/PowerAvg.h"
#include "Actions/WakeOnLan.h"

#include "Global.h"
#include "main.h"
#include "CrashReport.h"
#include "Cli/CliRunner.h"

using namespace PrintUtils;
using namespace StrUtils;
using namespace TimeUtils;

namespace Cli {

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
    ACTION_SHOW,
    ACTION_DIFF
};

Command cmdConfig, cmdPower, cmdShow, cmdSystem, cmdHelp, cmdPrint, cmdSet,
    cmdGet, cmdRm, cmdClock, cmdPlot, cmdLog, cmdWol, cmdRestart, cmdRun, cmdLs,
    cmdCrash, cmdLed, cmdSyslog;

SimpleCLI *cli_ = nullptr;
Runner *runner_ = nullptr;
Print *out_ = nullptr;

Print *setOutput(Print *out) {
    Print *prev = out_;
    out_ = out;
    return prev;
}

Runner *get() {
    if (runner_ == nullptr) runner_ = new CliRunner(cli_);
    return runner_;
}

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
void onLoop(cmd *c);
void onSyslog(cmd *c);

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

    Module *mod = app.getInstanceByName(modStr);
    if (mod) {
        if (!mod->execute(paramStr, valueStr)) {
            PrintUtils::println_done(out_);
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
    } else if (strcasecmp_P(str.c_str(), str_diff) == 0) {
        return ACTION_DIFF;
    }
    return ACTION_UNKNOWN;
}  // namespace Cli

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
    cmdPrint.addPositionalArgument("path");
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
    cmdRm.addPositionalArgument("path");
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
    cmdRun.addPositionalArgument("path");
    cmdRun.setCallback(Cli::onRun);

    cmdLs = cli_->addCommand("ls");
    cmdLs.addPositionalArgument("path");
    cmdLs.setCallback(Cli::onLs);

    cmdLs = cli_->addCommand("loop");
    cmdLs.addPositionalArgument("action", "show");
    cmdLs.setCallback(Cli::onLoop);

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

    cmdSyslog = cli_->addCommand("syslog");
    cmdSyslog.addPositionalArgument("action", "print");
    cmdSyslog.addPositionalArgument("param", "");
    cmdSyslog.setCallback(Cli::onSyslog);
}

void onLed(cmd *c) {
    Command cmd(c);
    String actionStr = getActionStr(cmd);
    String itemStr = getItemStr(cmd);
    String paramStr = getParamStr(cmd);
    String valueStr = getValueStr(cmd);

    switch (getAction(cmd)) {
        case ACTION_SHOW:
            app.led()->printDiag(out_);
            break;
        case ACTION_CONFIG:
            app.led()->config(LedEnum(itemStr.toInt()), LedConfigItem(paramStr.toInt()), valueStr.toInt());
            break;
        case ACTION_SET:
            app.led()->set(LedEnum(itemStr.toInt()), LedSignal(paramStr.toInt()));
            break;
        default:
            println_unknown_action(out_, actionStr);
            break;
    }
}

void onSyslog(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    if (action == ACTION_PRINT) {
        syslog.onDiag(out_);
    } else {
        println_unknown_action(out_, getActionStr(cmd));
    }
}

void onLog(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    String paramStr = getParamStr(cmd);
    bool handled = false;
    if (action == ACTION_PRINT) {
        String paramStr = getParamStr(cmd);
        if (!paramStr.length()) {
            powerlog->print(out_);
            return;
        }
        if (paramStr.indexOf("v") != -1) {
            powerlog->print(out_, VOLTAGE);
            handled = true;
        }
        if (paramStr.indexOf("i") != -1) {
            powerlog->print(out_, CURRENT);
            handled = true;
        }
        if (!handled)
            println_unknown_action(out_, paramStr);
    }
    out_->println();
}

void onHelp(cmd *c) {
    Command cmd(c);
    PrintUtils::print(out_, cli_->toString());
    PrintUtils::println(out_);
}

void onConfig(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    switch (action) {
        case ACTION_PRINT:
            config->printTo(*out_);
            return;
        case ACTION_RESET:
            config->setDefaultParams();
            break;
        case ACTION_SAVE:
            config->save();
            break;
        case ACTION_LOAD:
            config->load();
            break;
        case ACTION_APPLY:
            if (config->save())
                app.systemRestart();
            return;
        case ACTION_DIFF: {
            Config *cfg = config->get();
            for (uint8_t i = 0; i < CONFIG_ITEMS; ++i) {
                ConfigItem item = ConfigItem(i);
                if (strcmp(cfg->getValue(item), cfg->getValueDefault(item))) {
                    PrintUtils::print(out_, cfg->getParamName(item), cfg->getValue(item));
                    PrintUtils::println(out_);
                }
            }
            return;
        }
        default:
            String actionStr = getActionStr(cmd);
            PrintUtils::println_unknown_action(out_, actionStr);
            return;
    }
    PrintUtils::println_done(out_);
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
            println_unknown_action(out_, actionStr);
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
    app.systemRestart();
}

void onClock(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    String paramStr = getParamStr(cmd);
    switch (action) {
        case ACTION_UPTIME: {
            char buf[16];
            TimeUtils::format_elapsed_full(app.clock()->getUptime());
            PrintUtils::print(out_, buf);
            PrintUtils::println(out_);
            break;
        }
        case ACTION_TIME: {
            time_t local = app.clock()->getLocal();
            PrintUtils::print(out_, TimeUtils::format_time(local));
            PrintUtils::println(out_);
            break;
        }
        default: {
            PrintUtils::println_unknown_action(out_, getActionStr(cmd));
            return;
        }
    }
}

void onWifiScan(cmd *c) {
    Command cmd(c);
    PrintUtils::print(out_, FPSTR(str_scanning));
    int8_t n = WiFi.scanNetworks();
    if (n == 0)
        print_not_found(out_, FPSTR(str_network));
    else
        for (int i = 0; i < n; ++i)
            out_->printf("#%d %s %d\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
}

void onSet(cmd *c) {
    Command cmd(c);
    String param = getParamStr(cmd);
    String value = getValueStr(cmd);
    if (config->setParam(param.c_str(), value.c_str()) >= 0)
        PrintUtils::println_done(out_);
    else
        PrintUtils::println_unknown_param(out_, param);
}

void onGet(cmd *c) {
    Command cmd(c);
    String paramStr = getParamStr(cmd);
    ConfigItem paramItem;
    size_t valueSize = 0;
    if (app.params()->getConfig(paramStr, paramItem, valueSize)) {
        size_t len = paramStr.length() + valueSize + 4 + 1;
        char *buf = new char(len);
        sprintf(buf, "%s=\"%s\"", paramStr.c_str(), app.params()->getValue(paramItem));
        PrintUtils::print(out_, buf);
        PrintUtils::println(out_);
    } else {
        PrintUtils::println_unknown_param(out_, paramStr);
    }
}

void onLoop(cmd *c) {
    Command cmd(c);
    if (!loopTimer)
        loopTimer = new LoopTimer();
    switch (loopTimer->getState()) {
        case CAPTURE_IDLE:
            PrintUtils::print(out_, FPSTR(str_start), FPSTR(str_capture));
            PrintUtils::println(out_);
            loopTimer->start();
            break;
        case CAPTURE_PROGRESS:
            PrintUtils::print(out_, FPSTR(str_progress));
            PrintUtils::println(out_);
            break;
        case CAPTURE_DONE: {
            PrintUtils::println(out_, FPSTR(str_done));
            PrintUtils::print(out_, loopTimer->getData());
            loopTimer->idle();
            loopTimer = nullptr;
            break;
        }
    }
}

void onShow(cmd *c) {
    Command cmd(c);
    String modStr = getModStr(cmd);
    if (modStr == "app") {
        app.printDiag(out_);
    } else {
        ModuleEnum mod;
        if (app.getByName(modStr.c_str(), mod)) {
            app.getInstance(mod)->printDiag(out_);
        } else {
            println_unknown_module(out_, modStr);
        }
    }
}

void onPlot(cmd *c) {
    Command cmd(c);
    if (app.psu()->getState() == POWER_OFF) {
        size_t size = constrain(
            powerlog->getSize(PsuLogEnum::VOLTAGE), 0, 1024);
        if (size > 0) {
            PlotData data;
            float raw[size];
            powerlog->getValues(PsuLogEnum::VOLTAGE, raw, size);
            size_t cols = group(&data, raw, size);
            app.display()->showPlot(&data, cols);

            char tmp[PLOT_ROWS * 8 + 1];
            for (uint8_t x = 0; x < data.size; ++x) {
                uint8_t y = compress(&data, x);
                StrUtils::strfill(tmp, '*', y);
                out_->printf("#%d %2.4f ", x + 1, data.columns[x]);
                out_->print(tmp);
                out_->println();
            }
        };
    }
}

void onPrint(cmd *c) {
    Command cmd(c);
    auto file = getPathStr(cmd);
    if (FSUtils::exists(file)) {
        FSUtils::print(out_, file);
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
        PrintUtils::print(out_, dir.fileName(), '\t', prettyBytes(dir.fileSize()));
        PrintUtils::println(out_);
    }
}

void onRemove(cmd *c) {
    Command cmd(c);
    String name = getPathStr(cmd);
    PrintUtils::print(out_, FPSTR(str_file));
    if (FSUtils::exists(name)) {
        if (SPIFFS.remove(name)) {
            PrintUtils::println(out_, FPSTR(str_deleted));
        } else {
            PrintUtils::println(out_, FPSTR(str_failed));
        }
    } else {
        PrintUtils::print_not_found(out_, name);
    }
}

void onRun(cmd *c) {
    Command cmd(c);
    auto name = getPathStr(cmd);
    auto file = StringFile(name);
    auto data = file.get();
    if (file.read()) {
        if (data->available()) {
            String buf;
            while (data->pop(buf)) {
                Cli::get()->run(buf.c_str(), out_);
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
            PrintUtils::print(out_, res);
            PrintUtils::println(out_);
            break;
        }
        case ACTION_UNKNOWN:
        default:
            PrintUtils::println_unknown_action(out_, getActionStr(cmd));
            break;
    }
}

void onCommandError(cmd_error *e) {
    CommandError cmdError(e);
    PrintUtils::print(out_, cmdError.toString());
    PrintUtils::println(out_);
}

}  // namespace Cli