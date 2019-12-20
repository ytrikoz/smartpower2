#include "Modules/Shell.h"

#include "Cli/Cli.h"

namespace Modules {

Console::Console(): Module() {};

void Console::setShell(Cli::CommandShell* shell) {
    shell_ = shell;
    shell_->setTerm(term_);
}

bool Console::onInit() {    
    Cli::Terminal* term = new Cli::Terminal();
    term->enableControlCodes(false);
    term->enableEcho();       
    shell_ = new Cli::CommandShell(Cli::get());
    shell_->showGreetings();    
    shell_->setTerm(term);
    return true;
}

bool Console::onStart() {
    shell_->term()->setStream(&Serial);
    return true;
}

void Console::onStop() {
    shell_->term()->setStream(nullptr);
}

void Console::onLoop() {      
    if (shell_ != nullptr) shell_->loop(); 
}

bool Console::isOpen() { 
    return  shell_ != nullptr && shell_->active();
}

void Console::sendData(const String& data) {
    Serial.print(data);
}

}

 