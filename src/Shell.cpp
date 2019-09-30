#include "Shell.h"

#include "App.h"
#include "Cli.h"

using StrUtils::setstr;
using StrUtils::strfill;
using StrUtils::strpadd;

Shell::Shell() { active = false; }

void Shell::setParser(SimpleCLI *parser) { cli = parser; }

void Shell::setTerminal(Termul *term) {
    t = term;
    t->setOnOpen([this]() { this->onSessionOpen(); });
    t->setOnClose([this]() { this->onSessionClose(); });
    t->setOnReadLine([this](const char *str) { this->onSessionData(str); });
    t->setOnTabKey([this]() { this->requestHistoryHandler(); });
}

bool Shell::isActive() { return this->active; }

Termul *Shell::getTerminal() { return this->t; }

void Shell::enableWelcome(bool enabled) { welcomeEnabled = enabled; }

void Shell::onSessionOpen() {
#ifdef DEBUG_SHELL
    DEBUG.println("[shell] onSessionOpen");
#endif
    Cli::setOutput(t);
    if (welcomeEnabled)
        print_welcome(t);
    print_prompt(t);
    active = true;
}

void Shell::onSessionClose() {
#ifdef DEBUG_SHELL
    DEBUG.println("[shell] onSessionClose");
#endif
    Cli::close();
    active = false;
}

void Shell::loop() {
    if (t)
        t->loop();
}

void Shell::clearHistory() { history.clear(); }

void Shell::setEditBuffer(String &str) {
    t->setEditBuffer((const uint8_t *)str.c_str(), str.length());
    t->print(str);
}

void Shell::requestHistoryHandler() {
#ifdef DEBUG_SHELL
    DEBUG.println("[shell] requestHistoryHandler");
#endif
    if (history.size() > 0) {
        if (t->getEditBuffer()->available()) {
            t->println();
            print_prompt(t);
        }
        String str;
        if (getLastInput(str)) {
            setEditBuffer(str);
        };
    }
}

bool Shell::getLastInput(String &str) {
    if (history.size() > 0) {
        str = String(history.back());
        history.pop_back();
        return true;
    }
    return false;
}

void Shell::addHistory(const char *str) {
    if (str == NULL)
        return;
    String buf(str);
    if (buf.length()) {
        history.push_back(buf);
        if (history.size() > SHELL_HISTORY_SIZE)
            history.pop_back();
    }
}

void Shell::onSessionData(const char *str) {
#ifdef DEBUG_SHELL
    DEBUG.printf("[shell] onSessionData(%s)", str);
#endif
    cli->parse(str);
    while (cli->available()) {
        t->println();
        cli->getCmd().run();
    }
    addHistory(str);
    print_prompt(t);
}

size_t Shell::print_welcome(Print *p) {
#ifdef DEBUG_SHELL
    DEBUG.print("[shell] print_welcome");
#endif
    return 0;
}

size_t Shell::print_prompt(Print *p) {
#ifdef DEBUG_SHELL
    DEBUG.print("[shell] print_prompt");
#endif
    char buf[64] = {0};
    if (app.getClock())
        strcpy(buf,
               TimeUtils::getTimeFormated(buf, app.getClock()->getLocal()));
    size_t n = strlen(buf);
    buf[n] = '>';
    buf[++n] = '\x20';
    buf[++n] = '\x00';
    return p->print(buf);
}
