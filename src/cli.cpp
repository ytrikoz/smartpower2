#include "Cli.h"

#include "Actions/Actions.h"
#include "Actions/ClockSet.h"
#include "Actions/PowerAvg.h"
#include "Actions/ShowClients.h"
#include "Actions/ShowClock.h"
#include "Actions/ShowDiag.h"
#include "Actions/ShowNtp.h"
#include "Actions/ShowStatus.h"
#include "Actions/ShowWifi.h"

#include "StrUtils.h"

namespace Cli {

Command cmdConfig, cmdPower, cmdShow, cmdSystem, cmdHelp, cmdPrint, cmdSet,
    cmdGet, cmdRm, cmdClock;

Print* output;

bool active() { return output != NULL; }

void open(Print* p) {
    if (output != NULL) output->println(FPSTR(str_session_interrupted));
    output = p;
}

void init() {
    cli = new SimpleCLI();
    // Error
    cli->setErrorCallback(onCommandError);
    // Power
    cmdPower = cli->addCommand("power");
    cmdPower.addPositionalArgument("action", "status");
    cmdPower.addPositionalArgument("param", "");
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
    cmdSystem.addPositionalArgument("action");
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
}

void close() {
    output->print(FPSTR(str_shell_start_hint));
    output->println();
    output = nullptr;
}

String getActionStr(Command& c) { return c.getArgument("action").getValue(); }

String getParamStr(Command& c) { return c.getArgument("param").getValue(); }

String getCommandFile(Command& c) { return c.getArgument("action").getValue(); }

String getCommandItem(Command& c) { return c.getArgument("item").getValue(); }

String getCommandValue(Command& c) { return c.getArgument("value").getValue(); }

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

void unknownActionParam(const char* param, const char* action) {
    output->printf_P(strf_unknown_action_param, param, action);
}

void unknownAction(Command& c) {
    const char* str = getActionStr(c).c_str();
    output->printf_P(strf_unknown_action, str);
    output->println();
}

void print_done(Command& c) {
    // String str = getActionStr(c);
    // output->print(str.c_str());
    // output->print(' ');
    output->print(FPSTR(str_done));
    output->println();
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
    ACTION_OFF
};

CommandAction getAction(Command& c) {
    String str = c.getArgument("action").getValue();
    str += ' ';
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
    }
    return ACTION_UNKNOWN;
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
            print_done(cmd);
            break;
        case ACTION_SAVE:
            config->save();
            print_done(cmd);
            break;
        case ACTION_LOAD:
            config->reload();
            print_done(cmd);
            break;
        case ACTION_APPLY:
            config->save();
            setup_restart_timer();
            break;
        default:
            unknownAction(cmd);
            return;
    }
}

void onPower(cmd* c) {
    Command cmd(c);
    CommandAction action = getAction(cmd);
    String param = getParamStr(cmd);
    switch (action) {
        case ACTION_STATUS: {
            psu->printDiag(output);
            psuLog->printDiag(output);
            break;
        }
        case ACTION_AVG: {
            size_t num = param.equals("") ? 1 : atoi(param.c_str());
            Actions::PowerAvg("avg", num).exec(output);
            print_done(cmd);
            break;
        }
        case ACTION_LOG: {
            if (psuLog->empty()) {
                output->println(FPSTR(str_empty));
            } else {
                size_t num = param.equals("") ? 3 : atoi(param.c_str());
                if (num > psuLog->size()) num = psuLog->size();
                psuLog->printLast(output, num);
            }
            break;
        }
        case ACTION_ON: {
            psu->setState(POWER_ON);
            break;
        }
        case ACTION_OFF: {
            psu->setState(POWER_OFF);
            break;
        }
        default: {
            unknownAction(cmd);
            break;
        }
    }
}

void onClock(cmd* c) {
    Command cmd(c);
    String action = getActionStr(cmd);
    String param = getParamStr(cmd);
    if (action.equals("set")) {
        Actions::ClockSet().exec(output);
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

void onSystem(cmd* c) {
    Command cmd(c);
    String action = getActionStr(cmd);
    String param = getParamStr(cmd);

    if (action.equals("restart")) {
        setup_restart_timer(param.toInt());
        return;
    } else {
        output->print(FPSTR(str_avaible_system_actions));
        output->println();
        return;
    }
    print_done(action, param);
}

void onSet(cmd* c) {
    Command cmd(c);
    String name = getParamStr(cmd);
    String value = getCommandValue(cmd);

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
        char buf[OUTPUT_MAX_LENGTH];
        sprintf_P(buf, strf_set_s, name.c_str());
        output->print(buf);
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
    String item = getCommandItem(cmd);
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
    } else if (item.equals("config")) {
        ;
    } else if (item.equals("status")) {
        Actions::showStatus->exec(output);
    } else if (item.equals("ntp")) {
        Actions::showNtp->exec(output);
    } else if (item.equals("diag")) {
        Actions::showDiag->exec(output);
    } else if (item.equals("wifi")) {
        Actions::showWifi->exec(output);
    } else {
        unknownCommandItem(cmd.getName().c_str(), item.c_str());
    }
}

void onPrint(cmd* c) {
    Command cmd(c);
    String file = getCommandFile(cmd);
    if (SPIFFS.exists(file)) {
        File f = SPIFFS.open(file, "r");
        onIOResult(strf_file_print, file.c_str());
        while (f.available()) {
            output->println(f.readString().c_str());
        }
        f.close();
    } else {
        onIOResult(strf_file_not_found, file.c_str());
    }
}

void onRemove(cmd* c) {
    Command cmd(c);
    String file = cmd.getArgument("file").getValue();
    if (SPIFFS.exists(file)) {
        if (SPIFFS.remove(file)) {
            onIOResult(strf_file_deleted, file.c_str());
        }
    } else {
        onIOResult(strf_file_not_found, file.c_str());
    }
}
}  // namespace Cli