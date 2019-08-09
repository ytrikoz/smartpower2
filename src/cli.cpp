#include "cli.h"

#include "str_utils.h"

#include "Shell/ClockSet.h"
#include "Shell/ParameterlessCommand.h"
#include "Shell/ShowClients.h"
#include "Shell/ShowClock.h"
#include "Shell/ShowDiag.h"
#include "Shell/ShowNtp.h"
#include "Shell/ShowStatus.h"
#include "Shell/ShowWifi.h"

namespace CLI {

Print* output;

Command cmdPower, cmdShow, cmdSystem, cmdHelp, cmdPrint, cmdSet, cmdGet, cmdRm,
    cmdClock;

bool active() { return output != nullptr; }

void open(Print* p) {
    if (output != nullptr) {
        output->print(FPSTR(str_session_interrupted));
        output->println();
    }
    output = p;
}

void init() {
    cli = new SimpleCLI();
    // Error
    cli->setErrorCallback(onCommandError);
    // Power
    cmdPower = cli->addCommand("power");
    cmdPower.addPositionalArgument("action");
    cmdPower.addPositionalArgument("param", "");
    cmdPower.setCallback(handlePowerCommand);
    // Help
    cmdHelp = cli->addCommand("help");
    cmdHelp.setCallback(onHelpCommand);
    // Print
    cmdPrint = cli->addCommand("print");
    cmdPrint.addPositionalArgument("file");
    cmdPrint.setCallback(onPrintCommand);
    // System
    cmdSystem = cli->addCommand("system");
    cmdSystem.addPositionalArgument("action");
    cmdSystem.addPositionalArgument("param", "");
    cmdSystem.setCallback(onSystemCommand);
    // Show
    cmdShow = cli->addCommand("show");
    cmdShow.addPositionalArgument("item");
    cmdShow.setCallback(onShowCommand);
    // Set
    cmdSet = cli->addCommand("set");
    cmdSet.addPositionalArgument("param");
    cmdSet.addPositionalArgument("value");
    cmdSet.setCallback(onSetCommand);
    // Get
    cmdGet = cli->addCommand("get");
    cmdGet.addPositionalArgument("param");
    cmdGet.setCallback(onGetCommand);
    // Rm
    cmdRm = cli->addCommand("rm");
    cmdRm.addPositionalArgument("file");
    cmdRm.setCallback(onFileRemoveCommand);
    // Clock
    cmdClock = cli->addCommand("clock");
    cmdClock.addPositionalArgument("action");
    cmdClock.addPositionalArgument("param", "");
    cmdClock.setCallback(onClockCommand);
}

void close() {
    output->print(FPSTR(str_cli_hint));
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

void unknownActionParam(const char* param, const char* action) {
    output->printf_P(strf_unknown_action_param, param, action);
}

void unknownAction(String actionStr)
{
    output->printf_P(strf_unknown_action, actionStr.c_str());
    output->println();
}

void onCommandResult(PGM_P strP, const char* filename) {
    output->printf_P(strP, filename);
    output->println();
}

void onCommandDone(String& action, String& param) {
    output->print(action.c_str());
    output->print(' ');
    output->print(param.c_str());
    output->print(": ");
    output->print(FPSTR(str_done));
    output->println();
}

void onCommandError(cmd_error* e) {
    CommandError cmdError(e);
    output->println(cmdError.toString().c_str());
}

void onHelpCommand(cmd* c) {
    Command cmd(c);
    output->println(cli->toString().c_str());
}

String getCommandAction(Command* c) {
    return c->getArgument("action").getValue();
}

String getCommandParam(Command* c) {
    return c->getArgument("param").getValue();
}

String getCommandFile(Command* c) {
       return c->getArgument("action").getValue();
}

String getCommandItem(Command* c) { return c->getArgument("item").getValue(); }

String getCommandValue(Command* c) {
    return c->getArgument("value").getValue();
}

void handlePowerCommand(cmd* c) {
    Command cmd(c);
    String action = getCommandAction(&cmd);
    String param = getCommandParam(&cmd);

    if (action.equals("status")) {
        psuLog->printDiag(output);
    } else if (action.equals("log")) {
        size_t num = param.equals("") ? 8: atoi(param.c_str());
        psuLog->printLast(num);        
    } else if (str_utils::isMeanYes(action)) {
        psu->setState(POWER_ON);
    } else if (str_utils::isMeanNo(action)) {
        psu->setState(POWER_OFF);
    } else {
        unknownAction(action);        
    };
}

void onClockCommand(cmd* c) {
    Command cmd(c);
    String action = getCommandAction(&cmd);
    String param = getCommandParam(&cmd);
    if (action.equals("set")) {
        shell::clockSet->Execute(output);
    }
}

void onWifiScanCommand(cmd* c) {
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

void onSystemCommand(cmd* c) {
    Command cmd(c);
    String action = getCommandAction(&cmd);
    String param = getCommandParam(&cmd);

    if (action.equals("reset")) {
        if (param.equals("config")) {
            config->reset();
        } else {
            unknownActionParam(param.c_str(), action.c_str());
            return;
        }
    } else if (action.equals("save")) {
        if (param.equals("config")) {
            config->save();
        } else {
            unknownActionParam(param.c_str(), action.c_str());
            return;
        }
    } else if (action.equals("load")) {
        if (param.equals("config")) {
            config->reload();
        } else {
            unknownActionParam(param.c_str(), action.c_str());
            return;
        }
    } else if (action.equals("restart")) {
        setup_restart_timer(param.toInt());
        return;
    } else {
        output->print(FPSTR(str_avaible_system_actions));
        output->println();
        return;
    }
    onCommandDone(action, param);
}

void onSetCommand(cmd* c) {
    Command cmd(c);
    String name = getCommandParam(&cmd);
    String value = getCommandValue(&cmd);

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

void onGetCommand(cmd* c) {
    Command cmd(c);
    String name = getCommandParam(&cmd);
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

void onShowCommand(cmd* c) {
    Command cmd(c);
    String item = getCommandItem(&cmd);
    if (item.equals("clients")) {
        shell::showClients->Execute(output);
    } else if (item.equals("clock")) {
        shell::showClock->Execute(output);
    } else if (item.equals("power")) {
        output->print(FPSTR(str_psu));
        output->println(psu->getStateDescription());
    } else if (item.equals("network")) {
        output->println(getNetworkInfoJson().c_str());
    } else if (item.equals("info")) {
        output->println(getSystemInfoJson().c_str());
    } else if (item.equals("config")) {
        output->println(config->getConfigJson().c_str());
    } else if (item.equals("status")) {
        shell::showStatus->Execute(output);
    } else if (item.equals("ntp")) {
        shell::showNtp->Execute(output);
    } else if (item.equals("diag")) {
        shell::showDiag->Execute(output);
    } else if (item.equals("wifi")) {
        shell::showWifi->Execute(output);
    } else {
        unknownCommandItem(cmd.getName().c_str(), item.c_str());
    }
}

void onPrintCommand(cmd* c) {
    Command cmd(c);
    String file = getCommandFile(&cmd);
    if (SPIFFS.exists(file)) {
        File f = SPIFFS.open(file, "r");
        onCommandResult(strf_file_print, file.c_str());
        while (f.available()) {
            output->println(f.readString().c_str());
        }

        f.close();
    } else {
        onCommandResult(strf_file_not_found, file.c_str());
    }
}

void onFileRemoveCommand(cmd* c) {
    Command cmd(c);
    String file = cmd.getArgument("file").getValue();
    if (SPIFFS.exists(file)) {
        if (SPIFFS.remove(file)) {
            onCommandResult(strf_file_deleted, file.c_str());
        }
    } else {
        onCommandResult(strf_file_not_found, file.c_str());
    }
}
}  // namespace CLI