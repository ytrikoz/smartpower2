#include "Cli.h"

#include "Global.h"

#include "Actions/Backlight.h"
#include "Actions/ClockSet.h"
#include "Actions/PlotPrint.h"
#include "Actions/PowerAvg.h"
#include "Actions/PowerLog.h"
#include "Actions/PowerOff.h"
#include "Actions/PowerOn.h"
#include "Actions/ShowClients.h"
#include "Actions/ShowClock.h"
#include "Actions/ShowDiag.h"
#include "Actions/ShowLoop.h"
#include "Actions/ShowNtp.h"
#include "Actions/ShowStatus.h"
#include "Actions/ShowWifi.h"

#include "StrUtils.h"

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
    ACTION_UPTIME
};

Command cmdConfig, cmdPower, cmdShow, cmdSystem, cmdHelp, cmdPrint, cmdSet,
    cmdGet, cmdRm, cmdClock, cmdPlot, cmdLog;

Print* output;

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
    return command.getArgument("value").getValue();
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

bool active() { return output != NULL; }

void open(Print* p) {
    if (output != NULL) output->println(FPSTR(msg_session_interrupted));
    output = p;
}

void init() {
    cli = new SimpleCLI();
    // Error
    cli->setErrorCallback(onCommandError);
    // Power
    cmdPower = cli->addCommand("power");
    cmdPower.addPositionalArgument("action", "status");
    cmdPower.addPositionalArgument("param", "0");
    cmdPower.setCallback(Cli::onPower);
    // Config
    cmdConfig = cli->addCommand("config");
    cmdConfig.addPositionalArgument("action", "print");
    cmdConfig.addPositionalArgument("param", "");
    cmdConfig.setCallback(Cli::onConfig);
    // Help
    cmdHelp = cli->addCommand("help");
    cmdHelp.setCallback(Cli::onHelp);
    // Print
    cmdPrint = cli->addCommand("print");
    cmdPrint.addPositionalArgument("file");
    cmdPrint.setCallback(Cli::onPrint);
    // System
    cmdSystem = cli->addCommand("system");
    cmdSystem.addPositionalArgument("action", "uptime");
    cmdSystem.addPositionalArgument("param", "");
    cmdSystem.setCallback(Cli::onSystem);
    // Show
    cmdShow = cli->addCommand("show");
    cmdShow.addPositionalArgument("item", "status");
    cmdShow.setCallback(Cli::onShow);
    // Set
    cmdSet = cli->addCommand("set");
    cmdSet.addPositionalArgument("param");
    cmdSet.addPositionalArgument("value");
    cmdSet.setCallback(Cli::onSet);
    // Get
    cmdGet = cli->addCommand("get");
    cmdGet.addPositionalArgument("param");
    cmdGet.setCallback(Cli::onGet);
    // rm
    cmdRm = cli->addCommand("rm");
    cmdRm.addPositionalArgument("file");
    cmdRm.setCallback(Cli::onRemove);
    // Clock
    cmdClock = cli->addCommand("clock");
    cmdClock.addPositionalArgument("action");
    cmdClock.addPositionalArgument("param", "");
    cmdClock.setCallback(Cli::onClock);
    // Plot
    cmdPlot = cli->addCommand("plot");
    cmdPlot.addPositionalArgument("action", "print");
    cmdPlot.addPositionalArgument("param", "");
    cmdPlot.setCallback(Cli::onPlot);
    // Log
    cmdPlot = cli->addCommand("log");
    cmdPlot.addPositionalArgument("action", "status");
    cmdPlot.addPositionalArgument("param", "-1");
    cmdPlot.setCallback(Cli::onLog);
}

void close() {
    output->print(FPSTR(str_shell_start_hint));
    output->println();
    output = nullptr;
}

void onGetConfigParameter(const char* name, const char* value) {
    char buf[INPUT_MAX_LENGTH];
    sprintf_P(buf, strf_config_param_value, name, value);
    output->println(buf);
}

void onConfigParameterChanged(const char* name, const char* old_value,
                              const char* new_value) {
    char buf[OUTPUT_MAX_LENGTH];
    sprintf_P(buf, strf_config_param_changed, new_value, name);
    output->print(buf);
}

void unknownCommandItem(const char* command, const char* item) {
    char buf[128];
    sprintf_P(buf, strf_unknown_command_item, item, command);
    output->println(buf);
}

void unknownConfigParam(const char* param) {
    char buf[128];
    sprintf_P(buf, strf_config_unknown_param, param);
    output->println(buf);
}

void print_unknown_action(String str) {
    output->printf_P(msgf_unknown_action, str.c_str());
    output->println();
}

void print_done(Print* p) {
    p->print(FPSTR(str_done));
    p->println();
};

void print_done(String& action, String& param) {
    output->print(action.c_str());
    output->print(' ');
    output->print(param.c_str());
    output->print(": ");
}

void onIOResult(PGM_P str, const char* filename) {
    output->printf_P(str, filename);
    output->println();
}

void onCommandError(cmd_error* e) {
    CommandError cmdError(e);
    output->println(cmdError.toString().c_str());
}

void onLog(cmd* c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    if (action == ACTION_STATUS) {
        psuLog->printDiag(output);
    } else {
        print_unknown_action(getActionStr(cmd));
    }
}

void onHelp(cmd* c) {
    Command cmd(c);
    output->println(cli->toString().c_str());
}

void onConfig(cmd* c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    switch (action) {
        case ACTION_PRINT:
            config->printTo(output);
            break;
        case ACTION_RESET:
            config->reset();
            print_done(output);
            break;
        case ACTION_SAVE:
            config->save();
            print_done(output);
            break;
        case ACTION_LOAD:
            config->reload();
            print_done(output);
            break;
        case ACTION_APPLY:
            config->save();
            setup_restart_timer();
            break;
        default:
            print_unknown_action(getActionStr(cmd));
            break;
    }
}

void onPower(cmd* c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    switch (action) {
        case ACTION_STATUS: {
            psu->printDiag(output);
            break;
        }
        case ACTION_AVG: {
            String param = getParamStr(cmd);
            Actions::PowerAvg(param.toInt()).exec(output);
            break;
        }
        case ACTION_LOG: {
            String param = getParamStr(cmd);
            Actions::PowerLog(param.toInt()).exec(output);
            break;
        }
        case ACTION_ON: {
            Actions::PowerOn().exec(output);
            break;
        }
        case ACTION_OFF: {
            Actions::PowerOff().exec(output);
            break;
        }
        default: {
            print_unknown_action(getActionStr(cmd));
            return;
        }
    }
}

void onSystem(cmd* c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    String param = getParamStr(cmd);

    switch (action) {
        case ACTION_RESTART: {
            output->print(FPSTR(msg_system_restart));
            output->println();
            setup_restart_timer(param.toInt());
            break;
        }
        case ACTION_BACKLIGHT: {
            bool enabled = param.length() == 0 ? true : param.toInt();
            Actions::Backlight(enabled).exec(output);
            break;
        }
        case ACTION_UPTIME: {
            output->println(rtc.getUptimeFormated());
            break;
        }
        default: {
            print_unknown_action(getActionStr(cmd));
            return;
        }
    }
}

void onClock(cmd* c) {
    Command cmd(c);
    String action = getActionStr(cmd);
    String param = getParamStr(cmd);
    if (action.equals("set")) {
        Actions::ClockSet(param).exec(output);
    }
}

void onWifiScan(cmd* c) {
    Command cmd(c);
    output->print(FPSTR(str_wifi));
    output->print(FPSTR(str_scanning));

    int8_t n = WiFi.scanNetworks();
    output->print(str_wifi);
    if (n == 0) {
        output->print(str_network_not_found);
        output->println();
    }
    for (int i = 0; i < n; ++i) {
        // Print SSID and RSSI for each network found
        output->print(FPSTR(str_wifi));
        output->printf_P(strf_wifi_scan_results, i + 1, WiFi.SSID(i).c_str(),
                         WiFi.RSSI(i));
    }
    output->println();
}

void onSet(cmd* c) {
    Command cmd(c);
    String name = getParamStr(cmd);
    String value = getValueStr(cmd);
    Config* cfg = config->getConfig();
    Parameter param;
    size_t old_size;
    if (cfg->getParameter(name.c_str(), param, old_size)) {
        char old[old_size];
        strcpy(old, cfg->getStrValue(param));
        if (cfg->setValue(param, value.c_str())) {
            onConfigParameterChanged(name.c_str(), old, value.c_str());
        } else {
            output->printf_P(strf_config_param_unchanged, name.c_str());
        }
    } else {
        unknownConfigParam(name.c_str());
    }
    output->println();
}

void onGet(cmd* c) {
    Command cmd(c);
    String name = getParamStr(cmd);
    Config* cfg = config->getConfig();
    Parameter param;
    size_t value_size;
    if (cfg->getParameter(name.c_str(), param, value_size)) {
        char value[value_size];
        strcpy(value, cfg->getStrValue(param));
        onGetConfigParameter(name.c_str(), value);
    } else {
        unknownConfigParam(name.c_str());
    }
}

void onShow(cmd* c) {
    Command cmd(c);
    String item = getItemStr(cmd);
    if (item.equals("clients")) {
        Actions::ShowClients().exec(output);
    } else if (item.equals("clock")) {
        Actions::ShowClock().exec(output);
    } else if (item.equals("power")) {
        psu->printDiag(output);
    } else if (item.equals("log")) {
        psuLog->printDiag(output);
    } else if (item.equals("network")) {
        output->println(getNetworkInfoJson().c_str());
    } else if (item.equals("info")) {
        output->println(getSystemInfoJson().c_str());
    } else if (item.equals("status")) {
        Actions::ShowStatus().exec(output);
    } else if (item.equals("loop")) {
        Actions::ShowLoop().exec(output);
    } else if (item.equals("ntp")) {
        Actions::ShowNtp().exec(output);
    } else if (item.equals("diag")) {
        Actions::ShowDiag().exec(output);
    } else if (item.equals("wifi")) {
        Actions::ShowWifi().exec(output);
    } else {
        unknownCommandItem(cmd.getName().c_str(), item.c_str());
    }
}


void onPlot(cmd* c) {
    Command cmd(c);
    PlotData* data = display->getData();
    for (uint8_t x = 0; x < display->getData()->size; ++x) {
        uint8_t y = map_to_plot_min_max(data, x);
        char tmp[PLOT_ROWS * 8 + 1];
        StrUtils::strfill(tmp, '*', y);
        output->printf("#%d %2.4f ", x + 1, data->cols[x]);
        output->print(tmp);
        output->println();
    }
}

void onPrint(cmd* c) {
    Command cmd(c);
    String file = getFileStr(cmd);
    if (SPIFFS.exists(file)) {
        File f = SPIFFS.open(file, "r");
        while (f.available()) output->println(f.readString());
        f.close();
    } else {
        onIOResult(strf_file_not_found, file.c_str());
    }
}

void onRemove(cmd* c) {
    Command cmd(c);
    String file = getFileStr(cmd);
    if (SPIFFS.exists(file)) {
        if (SPIFFS.remove(file)) onIOResult(strf_file_deleted, file.c_str());        
    } else {
        onIOResult(strf_file_not_found, file.c_str());
    }
}

}  // namespace Cli