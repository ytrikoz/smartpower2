#include "CommandShell.h"

#include "main.h"
#include "Cli.h"

using namespace StrUtils;
using namespace TimeUtils;


CommandShell::CommandShell(CommandRunner *runner): open_(false), runner_(runner) {};

void CommandShell::setTerminal(Terminal *term) {
    term_ = term;
    if (term != nullptr) {
        term->setOnEvent([this](TerminalEventEnum event,  Print* out) {    
            switch (event)
            {
            case EVENT_OPEN:            
                onOpen(out);
                break;
            case EVENT_CLOSE:            
                onClose(out); 
                break;
            case EVENT_TAB:
                onHistory(out);
                break;        
            default:
                break;
            }
        });
        term->setOnReadLine([this](const char *str) { onData(str); });
    }
}

bool CommandShell::isOpen() { return open_; }

void CommandShell::enableWelcome(bool enabled) { welcome_ = enabled; }

void CommandShell::onOpen(Print* out) {
    if (welcome_) {
        print_welcome(out);
    }    
    print_prompt(out);
    open_ = true;
}

void CommandShell::onClose(Print* out) {
    print_shell_exit(out);
    open_ = false;
}

void CommandShell::onHistory(Print* out) {
    if (!history.size()) return;

    if (term_->getLine().available()) {
        out->println();
        print_prompt(out);
    }
    String str;
    if (getLastInput(str)) {
        setEditBuffer(str);
    };
}

void CommandShell::onData(const char *str) {    
    addHistory(str);

    runner_->run(str, term_);

    print_prompt(term_);
}

void CommandShell::clearHistory() { history.clear(); }

void CommandShell::setEditBuffer(String &str) {
    term_->setLine((const uint8_t *)str.c_str(), str.length());
    term_->print(str);
}

bool CommandShell::getLastInput(String &str) {
    if (history.size()) {
        str = String(history.back());
        history.pop_back();
        return true;
    }
    return false;
}

void CommandShell::addHistory(const char *str) {
    if (str == NULL)
        return;
    if (history.size() && history.back().equals(str))
        return;
    String _new(str);
    if (_new.length()) {
        history.push_back(_new);
        if (history.size() > SHELL_HISTORY_SIZE)
            history.pop_back();
    }
}

void CommandShell::loop() {
    if (term_ != nullptr) term_->loop();
}

size_t CommandShell::print_welcome(Print *p) {
    return 0;
}

size_t CommandShell::print_shell_exit(Print *p) { 
    return p->println(FPSTR(msg_shell_exit)); 
}

size_t CommandShell::print_prompt(Print *p) {
    char buf[64];
    if (app.clock()) strcpy(buf, getTimeStr(app.clock()->getLocal(), true).c_str());

    size_t n = strlen(buf);
    buf[n] = '>';
    buf[++n] = '\x20';
    buf[++n] = '\x00';
    return p->print(buf);
}
