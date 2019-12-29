#include "Cli/Cli.h"

#include "Actions/Avg.h"
#include "Actions/WakeOnLan.h"

#include "Global.h"
#include "main.h"
#include "CrashReport.h"
#include "Cli/CliRunner.h"
#include "PowerLog.h"

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
    ACTION_DIFF,
    ACTION_ADD,
    ACTION_DELETE,
};

Command cmdAvg, cmdConfig, cmdPower, cmdShow, cmdExec, cmdHelp, cmdPrint, cmdSet,
    cmdGet, cmdRm, cmdClock, cmdPlot, cmdLog, cmdWol, cmdRestart, cmdRun, cmdLs,
    cmdCrash, cmdLed, cmdSyslog, cmdWifi, cmdHost;

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

void onExec(cmd *c);
void onSet(cmd *c);
void onGet(cmd *c);

void onAvg(cmd *c);
void onConfig(cmd *c);
void onPower(cmd *c);
void onShow(cmd *c);
void onHelp(cmd *c);
void onHost(cmd *c);
void onPrint(cmd *c);
void onPlot(cmd *c);
void onRemove(cmd *c);
void onWiFi(cmd *c);
void onWifiDiag(cmd *c);
void onClock(cmd *c);
void onLog(cmd *c);
void onWol(cmd *c);
void onRestart(cmd *c);
void onHost(cmd *c);
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

void onExec(cmd *c) {
    Command cmd(c);
    String item = getItemStr(cmd);
    String param = getParamStr(cmd);
    String value = getValueStr(cmd);
    Module *obj = app.getInstanceByName(item);
    if (obj) {
        if (!obj->execute(param, value)) {
            PrintUtils::print(out_, obj->getError());
            PrintUtils::println(out_);
        }
    } else {
        PrintUtils::println_unknown_item(out_, item);
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
    } else if (strcasecmp_P(str.c_str(), str_add) == 0) {
        return ACTION_ADD;
    } else if (strcasecmp_P(str.c_str(), str_delete) == 0) {        
        return ACTION_DELETE;
    }
    return ACTION_UNKNOWN;
}  // namespace Cli

void init() {
    // if (BootWatcher::isSafeBooMode())
    cli_ = new SimpleCLI();

    cli_->setErrorCallback(onCommandError);

    cmdHelp = cli_->addCommand("help");
    cmdHelp.setCallback(Cli::onHelp);

    cmdConfig = cli_->addCommand("config");
    cmdConfig.addPositionalArgument("action", "print");
    cmdConfig.addPositionalArgument("param", "");
    cmdConfig.setCallback(Cli::onConfig);

    cmdPower = cli_->addCommand("power");
    cmdPower.addPositionalArgument("action", "");
    cmdPower.setCallback(Cli::onPower);

    cmdAvg = cli_->addCommand("avg");
    cmdAvg.addPositionalArgument("value", "");
    cmdAvg.setCallback(Cli::onAvg);

    cmdPrint = cli_->addCommand("print");
    cmdPrint.addPositionalArgument("path");
    cmdPrint.setCallback(Cli::onPrint);

    cmdExec = cli_->addCommand("exec");
    cmdExec.addPositionalArgument("item", "");
    cmdExec.addPositionalArgument("param", "");
    cmdExec.addPositionalArgument("value", "");
    cmdExec.setCallback(Cli::onExec);

    cmdShow = cli_->addCommand("show");
    cmdShow.addPositionalArgument("item", "app");
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
    cmdWol.addPositionalArgument("param", "");
    cmdWol.addPositionalArgument("value", "");
    cmdWol.setCallback(Cli::onWol);

    cmdRestart = cli_->addCommand("restart");
    cmdRestart.setCallback([](cmd *c) { app.systemRestart(); });

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
    cmdCrash.addPositionalArgument("item", "");
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

    cmdWifi = cli_->addCommand("wifi");
    cmdWifi.addPositionalArgument("action", "scan");
    cmdWifi.setCallback(Cli::onWiFi);

    cmdHost = cli_->addCommand("host");
    cmdHost.addPositionalArgument("action", "show");
    cmdHost.addPositionalArgument("param", "");
    cmdHost.addPositionalArgument("name", "");
}

void onHost(cmd *c) {
    Command cmd(c);
    String action = getActionStr(cmd);
    String param = getParamStr(cmd);
    String value = getValueStr(cmd);
    switch (getAction(cmd)) {
        case ACTION_SHOW:
            // Print var/host
            FSUtils::print(out_, FS_HOST_VAR);
            break;
        case ACTION_ADD:
            // Add record to var/host
            // FSUtils::writeString(FS_HOST_VAR, param + " " + value);
            break;
        case ACTION_DELETE:
            // Delete record from var/host
            break;
        default:
            println_unknown_action(out_, action);
            break;
    }
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
        mainlog.onDiag(out_);
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
            powerlog->print(out_, PowerLogEnum::VOLTAGE);
            handled = true;
        }
        if (paramStr.indexOf("i") != -1) {
            powerlog->print(out_, PowerLogEnum::CURRENT);
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
            config->setDefaultConfig();
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
            for (uint8_t i = 0; i < PARAMS_COUNT; ++i) {
                ConfigItem item = ConfigItem(i);
                if (strcmp(cfg->value(item), cfg->getDefault(item))) {
                    PrintUtils::print(out_, cfg->name(item), cfg->value(item));
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
    PrintUtils::println(out_, FPSTR(str_done));
}

void onPower(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    switch (action) {
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

void onAvg(cmd *c) {
    Command cmd(c);
    String value = getValueStr(cmd);
    if (value == "") {
        Actions::Avg::print_config(out_);
    } else {
        Actions::Avg::set(out_, value.toInt());
    }
}

void onWol(cmd *c) {
    Command cmd(c);
    String param = getParamStr(cmd);
    String value = getValueStr(cmd);
    Actions::WakeOnLan::send(out_, param, value);
}

void onClock(cmd *c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    String paramStr = getParamStr(cmd);
    switch (action) {
        case ACTION_UPTIME: {
            char buf[16];
            TimeUtils::format_elapsed_full(app.clock()->uptime());
            PrintUtils::print(out_, buf);
            PrintUtils::println(out_);
            break;
        }
        case ACTION_TIME: {
            time_t local = app.clock()->local();
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

void onWiFi(cmd *c) {
    Command cmd(c);
    if (wireless->isScanning()) {
        PrintUtils::println(out_, FPSTR(str_scanning));
        return;
    }
    String actionStr = getActionStr(cmd);
    if (actionStr == "scan") {
        wireless->startWiFiScan(true);
    } else if (actionStr == "list") {
        FSUtils::print(out_, "/var/networks");
    };
}

void onSet(cmd *c) {
    Command cmd(c);
    String param = getParamStr(cmd);
    String value = getValueStr(cmd);
    if (config->get()->setByName(param.c_str(), value.c_str()) >= 0)
        PrintUtils::println(out_, FPSTR(str_done));
    else
        PrintUtils::println_unknown_param(out_, param);
}

void onGet(cmd *c) {
    Command cmd(c);
    String param = getParamStr(cmd);
    ConfigItem item;
    size_t size = 0;
    if (app.params()->getParamByName(param.c_str(), item, size)) {
        size_t len = param.length() + size + 4 + 1;
        char *buf = new char(len);
        sprintf(buf, "%s=\"%s\"", param.c_str(), app.params()->value(item));
        PrintUtils::println(out_, buf);
    } else {
        PrintUtils::println_unknown_param(out_, param);
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
    String item = getItemStr(cmd);
    if (item == "app") {
        app.printDiag(out_);
    } else {
        ModuleEnum mod;
        if (app.getByName(item.c_str(), mod)) {
            app.getInstance(mod)->printDiag(out_);
        } else {
            PrintUtils::println_unknown_item(out_, item);
        }
    }
}

void onPlot(cmd *c) {
    Command cmd(c);
    size_t size = powerlog->getSize(VOLTAGE);
    if (!size) return;
    PlotSummary summary;
    float *values = new float[size];
    powerlog->fill(VOLTAGE, values, size);
    size_t cols = group(&summary, values, size);
    app.display()->showPlot(&summary, cols);

    char tmp[PLOT_ROWS * 8 + 1];
    for (size_t x = 0; x < summary.size; ++x) {
        size_t y = compress(&summary, x);
        StrUtils::strfill(tmp, '*', y);
        PrintUtils::print(out_, x + 1, summary.columns[x]);
        PrintUtils::println(out_, tmp);
    }
}

void onPrint(cmd *c) {
    Command cmd(c);
    auto path = getPathStr(cmd);
    if (FSUtils::exist(path)) {
        FSUtils::print(out_, path);
    } else {
        print_file_not_found(out_, path);
    }
}

void onLs(cmd *c) {
    Command cmd(c);
    String str = getPathStr(cmd);
    String path = FSUtils::asDir(str.c_str());
    uint8_t level = FSUtils::getNestedLevel(path);
    auto dir = SPIFFS.openDir(path);
    while (dir.next()) {
        String name = dir.fileName();
        if (FSUtils::getNestedLevel(name) <= level) {
            PrintUtils::print(out_, dir.fileName(), '\t', prettyBytes(dir.fileSize()));
            PrintUtils::println(out_);
        }
    }
}

void onRemove(cmd *c) {
    Command cmd(c);
    String name = getPathStr(cmd);
    PrintUtils::print(out_, FPSTR(str_file));
    if (FSUtils::exist(name)) {
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
            FSUtils::printFileList(out_, CRASH_ROOT);
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
            FSUtils::rmDir(out_, CRASH_ROOT);
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