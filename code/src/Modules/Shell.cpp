#include "Modules/Shell.h"

#include "Cli/Cli.h"

namespace Modules {

Shell::Shell(): Module() {};

void Shell::setShell(Cli::CommandShell* shell) {
    shell_ = shell;
    shell_->setTerm(term_);
}

bool Shell::onInit() {    
    Cli::Terminal* term = new Cli::Terminal();
    term->enableControlCodes(false);
    term->enableEcho();       
    shell_ = new Cli::CommandShell(Cli::get());
    shell_->showGreetings();    
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
    return  shell_ != nullptr && shell_->active();
}

}

 