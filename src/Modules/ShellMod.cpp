#include "Modules/ShellMod.h"

#include "Global.h"

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

bool ShellMod::run(const char *cmdStr) { return shell.run(cmdStr); }

bool ShellMod::isActive() { return shell.isActive(); }