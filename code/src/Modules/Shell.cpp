#include "Modules/Shell.h"

#include "Cli/CommandRunner.h"

namespace Modules {

Shell::Shell(): Module() {};

void Shell::setShell(CommandShell* shell) {
    shell_ = shell;
    shell_->setTerm(term_);
}

bool Shell::onInit() {    
    Terminal* term = new Terminal();
    term->enableControlCodes(false);
    term->enableEcho();       
    shell_ = new CommandShell(Cli::get());
    shell_->enableWelcome();    
    shell_->setTerm(term);
    return true;
}

bool Shell::onStart() {
    shell_->term()->setStream(&Serial);
    return true;
}

void Shell::onStop() {
    shell_->term()->setStream(nullptr);
}

void Shell::onLoop() {      
    if (shell_ != nullptr) shell_->loop(); 
}

bool Shell::isOpen() { 
    return  shell_ != nullptr && shell_->isOpen();
}

}

