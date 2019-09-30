#include "Modules/ShellMod.h"

#include "Global.h"

ShellMod::ShellMod() : AppModule(MOD_SHELL) {
    Cli::init();
    active = false;
}

void ShellMod::loop() {
    if (active)
        shell.loop();
}

bool ShellMod::begin() {
    shell.setParser(cli);
    active = setLocal();
    return active;
}

bool ShellMod::setLocal() {
    local.enableControlCodes(false);
    local.enableEcho();
    local.setConsole(&USE_SERIAL);

    shell.enableWelcome();
    shell.setTerminal(&local);

    active = true;

    return active;
}

bool ShellMod::setRemote(Stream *stream) {
    remote.enableControlCodes();
    remote.enableEcho(false);
    remote.setConsole(stream);

    shell.enableWelcome();
    shell.setTerminal(&remote);

    active = true;

    return active;
}