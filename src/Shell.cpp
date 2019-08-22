#include "Shell.h"

#include "Cli.h"

using StrUtils::strfill;
using StrUtils::strpadd;
using StrUtils::setstr;

Shell::Shell(SimpleCLI *cli, Termul *t) {
    memset(&prevInput, 0, sizeof(prevInput));
    
    this->cli = cli;
    
    this->t = t;
    this->t->setOnStart([this]() { this->onOpen(); });
    this->t->setOnQuit([this]() { this->onClose(); });
    this->t->setOnInput([this](const char* str) { this->onInput(str); });
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

void Shell::onTabPress() {
#ifdef DEBUG_SHELL
    DEBUG.println("[shell] onTabPress");
#endif
    if (strlen(prevInput) > 0) {
        if (t->input()->length() > 0) {
            t->println();
            print_prompt();
        }
        t->input()->set(prevInput);
        t->print(prevInput);
    }
}

void Shell::onInput(const char* str) {
#ifdef DEBUG_SHELL
    DEBUG.printf("[shell] onInput(%s)", str);
#endif
    cli->parse(str);
    while (cli->available()) {
        t->println();
        cli->getCmd().run();
    }
    setstr(prevInput, str, sizeof(prevInput));
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
    strcpy(buf, rtc.getLocalFormated().c_str());
    uint8_t x = strlen(buf);
    buf[x] = '>';
    buf[++x] = CHAR_SP;
    buf[++x] = '\x00';
    t->print(buf);
}
