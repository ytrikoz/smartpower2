#include "cli.h"

TerminalWriter* output;
Command cmdShow, cmdSystem, cmdHelp, cmdPrint, cmdSet, cmdGet, cmdRm;

bool is_cli_active() { return (output); }

void init_cli() {
    cli = new SimpleCLI();
    // Error
    cli->setErrorCallback(onCommandError);
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
    cmdRm.setCallback(onRMCommand);
}

bool start_cli(TerminalWriter* writer) {
    if (output) quit_cli();
    output = writer;
    return true;
}

void quit_cli() {
    output->println();
    output->print(F("Interrupted by another connection established"));
    output = nullptr;
}

void print_P(PGM_P str) {
    char buf[OUTPUT_MAX_LENGTH];
    strcpy_P(buf, str);
    output->print(buf);
}

void onGetConfigParameter(const char* name, const char* value) {
    char buf[OUTPUT_MAX_LENGTH];
    sprintf_P(buf, strf_config_param_value, name, value);
    output->print(buf);
}

void onConfigParameterChanged(const char* name, const char* old_value,
                              const char* new_value) {
    char buf[OUTPUT_MAX_LENGTH];
    sprintf_P(buf, strf_config_param_changed, name, old_value, new_value);
    output->print(buf);
}

void onUnknownCommandItem(const char* command, const char* item) {
    char buf[128];
    sprintf_P(buf, strf_unknown_command_item, item, command);
    output->print(buf);
}

void onUnknownConfigParameter(const char* param) {
    char buf[128];
    sprintf_P(buf, strf_config_unknown_param, param);
    output->print(buf);
}

void onUnknownActionParameter(const char* param, const char* action) {
    output->printf_P(strf_unknown_action_param, param, action);
}

void onCommandResult(PGM_P strP, const char* filename) {
    output->printf_P(strP, filename);
    output->println();
}

bool isPositive(String str) {
    return str.equals("on") || str.equals("+") || str.equals("yes");
}

bool isNegative(String str) {
    return str.equals("off") || str.equals("-") || str.equals("no");
}

void onSystemCommandDone(String& action, String& param) {
    output->print(action.c_str());
    output->print(' ');
    output->print(param.c_str());
    output->print(": ");
    print_P(str_done);
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

void onSystemCommand(cmd* c) {
    Command cmd(c);
    String action = cmd.getArgument("action").getValue();
    String param = cmd.getArgument("param").getValue();

    if (action.equals("reset")) {
        if (param.equals("config")) {
            config->reset();
        } else {
            onUnknownActionParameter(param.c_str(), action.c_str());
            return;
        }
    } else if (action.equals("save")) {
        if (param.equals("config")) {
            config->save();
        } else {
            onUnknownActionParameter(param.c_str(), action.c_str());
            return;
        }
    } else if (action.equals("load")) {
        if (param.equals("config")) {
            config->reload();
        } else {
            onUnknownActionParameter(param.c_str(), action.c_str());
            return;
        }
    } else if (action.equals("restart")) {
        output->println();
        setup_restart_timer(param.toInt());
        return;
    } else if (action.equals("power")) {
        if (isPositive(param)) {
            set_power_state(POWER_ON);
        } else if (isNegative(param)) {
            set_power_state(POWER_OFF);
        } else {
            onUnknownActionParameter(param.c_str(), action.c_str());           
        }
        return;
    } else {
        print_P(str_avaible_system_actions);
        output->println();
        return;
    }

    onSystemCommandDone(action, param);
}

void onSetCommand(cmd* c) {
    Command cmd(c);
    String name = cmd.getArgument("param").getValue();
    String value = cmd.getArgument("value").getValue();

    Config* runtime = config->getData();
    Parameter param;
    size_t old_size;
    if (runtime->getParameter(name.c_str(), param, old_size)) {
        char old[old_size];
        strcpy(old, runtime->getStrValue(param));
        if (runtime->setValue(param, value.c_str())) {
            onConfigParameterChanged(name.c_str(), old, value.c_str());
        } else {
            output->print(F("no changes"));
        }
    } else {
        char buf[OUTPUT_MAX_LENGTH];
        sprintf_P(buf, strf_set_param, name.c_str());
        output->print(buf);
    }
    output->println();
}

void onGetCommand(cmd* c) {
    Command cmd(c);
    String name = cmd.getArgument("param").getValue();

    Config* runtime = config->getData();
    Parameter param;
    size_t value_size;
    if (runtime->getParameter(name.c_str(), param, value_size)) {
        char value[value_size];
        strcpy(value, runtime->getStrValue(param));

        onGetConfigParameter(name.c_str(), value);
    } else {
        onUnknownConfigParameter(name.c_str());
    }
    output->println();
}

void onShowCommand(cmd* c) {
    Command cmd(c);
    String item = cmd.getArgument("item").getValue();
    if (item.equals("ip")) {
        output->println(getHostIPInfo().c_str());
    } else if (item.equals("clients")) {
        char buf[OUTPUT_MAX_LENGTH];
        sprintf(buf, "wifi: %s", getConnectedStationInfo().c_str());
        output->println(buf);
        sprintf(buf, "http: %d", get_http_clients_count());
        output->println(buf);
        sprintf(buf, "telnet: %d", get_telnet_clients_count());
        output->println(buf);

    } else if (item.equals("power")) {
        char buf[OUTPUT_MAX_LENGTH];
        sprintf(buf, "power %s", get_power_state() == POWER_ON ? "on" : "off");
        output->println(buf);
    } else if (item.equals("network")) {
        output->println(getNetworkInfoJson().c_str());
    } else if (item.equals("info")) {
        output->println(getSystemInfoJson().c_str());
    } else if (item.equals("config")) {
        output->println(config->getConfigJson().c_str());
    } else if (item.equals("status")) {
        char buf[OUTPUT_MAX_LENGTH];
        sprintf(buf, "LPS %lu max %lums", get_lps(), get_longest_loop());
        output->println(buf);
        sprintf(buf, "heap %s", getHeapStatistic().c_str());
        output->println(buf);
    } else {
        onUnknownCommandItem(cmd.getName().c_str(), item.c_str());
    }
}

void onPrintCommand(cmd* c) {
    Command cmd(c);
    String file = cmd.getArgument("file").getValue();
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

void onRMCommand(cmd* c) {
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