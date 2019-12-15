#include "Modules/Shell.h"

#include "Cli.h"
#include <main.h>

namespace Modules {

Shell::Shell(SimpleCLI* cli_): Module(), cli_(cli_) {};

void Shell::setTerminal(Terminal* term, CommandShell* shell) {
    if (shell == nullptr)
        shell = new CommandShell(this);
    shell->setTerminal(term);
}

void Shell::setRemote(Terminal* term) {
    setTerminal(term, remoteShell_);
}

void Shell::setLocal(Terminal* term) {
    setTerminal(term, localShell_);
}

bool Shell::onInit() {    
    localShell_ = new CommandShell(this);
    localShell_->enableWelcome();    

    localTerm_ = new Terminal(&Serial);
    localTerm_->enableControlCodes(false);
    localTerm_->enableEcho();       

    return true;
}

bool Shell::onStart() {
    setTerminal(localTerm_, localShell_);    
    return true;
}

void Shell::onStop() {
    setRemote(nullptr);
    setLocal(nullptr);
}

void Shell::onLoop() {      
    if (localShell_ != nullptr) localShell_->loop(); 
}

void Shell::run(const char *cmdStr) { return run(cmdStr, localTerm_); }

void Shell::run(const char *cmdStr, Print* output) { 
    Print* prev = Cli::setOutput(output);

    cli_->parse(cmdStr);
    while (cli_->available()) {
        cli_->getCmd().run();
        if (cli_->getError()) {
            break;
        }
    }

    Cli::setOutput(prev);
}

bool Shell::isOpen() { 
    return  localShell_ != nullptr && localShell_->isOpen();
}

}

