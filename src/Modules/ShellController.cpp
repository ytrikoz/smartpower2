#include "Modules/ShellController.h"

#include "Global.h"

ShellController::ShellController() : AppModule(MOD_SHELL) {
    Cli::init();
    active = false;
}

void ShellController::loop() {
    if (active)
        shell.loop();
}

bool ShellController::begin() {
    shell.setParser(cli);
    shell.setTerminal(&term);
    active = setLocal();
    return active;
}

bool ShellController::setLocal() {
    term.enableControlCodes(false);
    term.enableEcho();
    term.setConsole(&USE_SERIAL);

    shell.enableWelcome(false);

    active = true;

    return active;
}

bool ShellController::setRemote(Stream *s) {
    term.enableControlCodes();
    term.enableEcho(false);
    term.setConsole(s);

    shell.enableWelcome();

    active = true;

    return active;
}