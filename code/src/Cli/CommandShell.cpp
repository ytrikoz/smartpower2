#include "Cli/CommandShell.h"

#include "Utils/StrUtils.h"

using namespace StrUtils;
using namespace TimeUtils;

namespace Cli {

CommandShell::CommandShell(Cli::Runner *runner) : term_(nullptr), runner_(runner), path_('\\'), active_(false), greetings_(false), farewell_(false) {}

void CommandShell::setTerm(Cli::Terminal *term) {
    term_ = term;
    if (term_ != nullptr) {
        term_->setOnEvent([this](TerminalEventEnum event, Stream *out) {
            switch (event) {
                case EVENT_OPEN: {
                    onOpen(out);
                    break;
                }
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
        term_->setOnReadLine([this](const char *str) { onData(str); });
    }
}

Terminal *CommandShell::term() {
    return term_;
}

void CommandShell::loop() {
    if (term_ != nullptr) term_->loop();
}

bool CommandShell::active() { return active_; }

void CommandShell::showGreetings(bool enabled) { greetings_ = enabled; }

void CommandShell::showFarewell(bool enabled) { farewell_ = enabled; }

void CommandShell::onOpen(Print *out) {
    if (greetings_) printGreetings(out);
    printPrompt(out);
    active_ = true;
}

void CommandShell::onClose(Print *out) {
    out->println();
    if (farewell_) printFarewell(out);
    active_ = false;
}

void CommandShell::onHistory(Print *out) {
    if (!history_.size()) return;

    if (term_->getLine().available()) {
        out->println();
        printPrompt(out);
    }
    String str;
    if (getLastInput(str)) {
        setEditLine(str);
    };
}

void CommandShell::onData(const char *str) {
    addHistory(str);

    runner_->run(str, term_);

    printPrompt(term_);
}

void CommandShell::clearHistory() { history_.reset(); }

void CommandShell::setEditLine(const String &str) {
    term_->setLine((const uint8_t *)str.c_str(), str.length());
}

bool CommandShell::getLastInput(String &str) {
    return history_.pop_back(str);
}

void CommandShell::addHistory(const char *str) {
    String buf;
    if (str == NULL || !strlen(str) || (history_.peek(buf) && buf.equals(str)))
        return;
    history_.push(str);
}

size_t CommandShell::printGreetings(Print *p) {
    return p->println(FPSTR(msg_greetings));
}

size_t CommandShell::printFarewell(Print *p) {
    return p->println(FPSTR(str_exit_bye));
}

size_t CommandShell::printPrompt(Print *p) {
    size_t n = 0;
    n += p->print(path_);
    n += p->print('>');
    n += p->print(' ');
    return n;
}

}  // namespace Cli