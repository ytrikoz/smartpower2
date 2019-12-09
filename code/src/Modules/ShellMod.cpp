#include "Modules/ShellMod.h"

#include "Cli.h"
#include <main.h>

bool ShellMod::onInit() {
    Cli::init();
    shell.setParser(cli);
    return true;
}

bool ShellMod::onStart() {
    setSerial();
    return true;
}

void ShellMod::onLoop() { shell.loop(); }

void ShellMod::setSerial() {
    local.enableControlCodes(false);
    local.enableEcho();
    local.setConsole(&Serial);

    shell.enableWelcome();
    shell.setTerminal(&local);
}

void ShellMod::setRemote(Stream *stream) {
    remote.enableControlCodes();
    remote.enableEcho(false);
    remote.setConsole(stream);

    shell.enableWelcome();
    shell.setTerminal(&remote);
}

bool ShellMod::run(const String& cmd) { return run(cmd.c_str()); }

bool ShellMod::run(const char *cmd) { return shell.run(cmd); }

bool ShellMod::isActive() { return shell.isActive(); }