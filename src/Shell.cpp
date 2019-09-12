#include "Shell.h"

#include "Cli.h"

using StrUtils::setstr;
using StrUtils::strfill;
using StrUtils::strpadd;

Shell::Shell(SimpleCLI* cli, Termul* t) {
    this->cli = cli;

    this->t = t;
    this->t->setOnStart([this]() { this->onOpen(); });
    this->t->setOnQuit([this]() { this->onClose(); });
    this->t->setOnInput([this](const char* str) { this->onLineInput(str); });
    this->t->setOnTab([this]() { this->onTabPress(); });

    active = false;
}

bool Shell::isActive() { return this->active; }

Termul* Shell::getTerm() { return this->t; }

void Shell::enableWelcome(bool enabled) { welcomeEnabled = enabled; }

void Shell::onOpen() {
#ifdef DEBUG_SHELL
    DEBUG.print("[shell] onOpen");
#endif
    Cli::open(t);
    if (welcomeEnabled) print_welcome();
    print_prompt();
    active = true;
}

void Shell::clearHistory() {
    history.clear();
}

void Shell::onClose() {
#ifdef DEBUG_SHELL
    DEBUG.println("[shell] onClose");
#endif
    t->println();
    Cli::close();
    active = false;
}

void Shell::loop() {
    if (t) t->read();
}

void Shell::setEditBuffer(String& str) {
    t->setEditBuffer((const uint8_t*) str.c_str(), str.length());
    t->print(str);
}

void Shell::onTabPress() {
#ifdef DEBUG_SHELL
    DEBUG.println("[shell] onTabPress");
#endif
    if (history.size() > 0) {
        if (t->getEditBuffer()->available()) {
            t->println();
            print_prompt();
        }
        String str;
        if (getLastInput(str)) { 
            setEditBuffer(str); 
        };        
    }
}

bool Shell::getLastInput(String& str) {
    if (history.size() > 0) {
        str = String(history.back());
        history.pop_back();
        return true;
    }
    return false;
}

void Shell::addHistory(const char* str) {
    if (str == NULL) return;
    String buf(str);
    if (buf.length()) {
        history.push_back(buf);
        if (history.size() > SHELL_HISTORY_SIZE) history.pop_back();
    } 
}

void Shell::onLineInput(const char* str) {
#ifdef DEBUG_SHELL
    DEBUG.printf("[shell] onLineInput(%s)", str);
#endif
    cli->parse(str);
    while (cli->available()) {
        t->println();
        cli->getCmd().run();
    }
    addHistory(str);
    print_prompt();
}

void Shell::print_welcome() {
#ifdef DEBUG_SHELL
    DEBUG.print("[shell] print_welcome");
#endif
    char title[SCREEN_WIDTH + 1];
    strcpy(title, APPNAME " v" FW_VERSION);
    uint8_t width = SCREEN_WIDTH / 2;
    strpadd(title, StrUtils::CENTER, width, ' ');
    char tmp[width + 1];
    strfill(tmp, '#', width);

    t->println(tmp);
    t->println(title);
    t->println(tmp);
}

void Shell::print_prompt() {
#ifdef DEBUG_SHELL
    DEBUG.print("[shell] print_prompt");
#endif
    char buf[OUTPUT_MAX_LENGTH];
    strcpy(buf, rtc.getLocalTimeStr().c_str());
    uint8_t x = strlen(buf);
    buf[x] = '>';
    buf[++x] = CHAR_SP;
    buf[++x] = '\x00';
    t->print(buf);
}
