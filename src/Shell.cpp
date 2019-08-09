#include "Shell.h"

#include "cli.h"

Shell::Shell() {
    memset(&prevInput, 0, sizeof(prevInput));
    active = false;
}

void Shell::setParser(SimpleCLI* parser) { this->parser = parser; }

void Shell::setTermul(Termul* term) {
    term->setOnStart([this]() { this->onOpen(); });
    term->setOnQuit([this]() { this->onClose(); });
    term->setOnInput([this](const char* str) { this->onInput(str); });
    term->setOnTab([this]() { this->onTabPress(); });
    this->t = term;
}

bool Shell::isActive() { return this->active; }

Termul* Shell::getTerm() { return this->t; }

void Shell::enableWelcome(bool enabled) { welcomeEnabled = enabled; }

void Shell::onOpen() {
#ifdef DEBUG_SHELL
    DEBUG.print("[shell] onOpen");
#endif
    CLI::open(t);
    if (welcomeEnabled) welcome();
    prompt();
    active = true;
}

void Shell::onClose() {
#ifdef DEBUG_SHELL
    DEBUG.print("[shell] onClose");
#endif
    t->println();
    CLI::close();
    active = false;
}

void Shell::loop() {
    if (!t) return;
    t->read();
}

void Shell::onTabPress() {
#ifdef DEBUG_SHELL
    DEBUG.printf("[shell] onTabPress()");
#endif
    if (strlen(prevInput) > 0) {
        if (t->input()->length() > 0) {
            t->println();
            prompt();
        }
        t->input()->set(prevInput);
        t->print(prevInput);
    }
}

void Shell::onInput(const char* str) {
#ifdef DEBUG_SHELL
    DEBUG.printf("[shell] onInput(%s)", str);
#endif
    parser->parse(str);
    while (parser->available()) {
        t->println();
        Command cmd = parser->getCmd();
        cmd.run();
    }
    strcpy(prevInput, str);
    prompt();
}

void Shell::welcome() {
#ifdef DEBUG_SHELL
    DEBUG.print("[shell] welcome");
#endif
    char title[SCREEN_WIDTH + 1];
    strcpy(title, APPNAME " v" FW_VERSION);
    uint8_t width = SCREEN_WIDTH / 2;
    str_utils::strwithpad(title, str_utils::CENTER, width, ' ');
    char tmp[width + 1];
    str_utils::strfill(tmp, '#', width);

    t->println(tmp);
    t->println(title);
    t->println(tmp);
}

void Shell::prompt() {
    char buf[OUTPUT_MAX_LENGTH + 1];
    strcpy(buf, rtc.getLocalFormated().c_str());
    uint8_t len = strlen(buf);
    buf[len] = CHAR_SP;
    buf[len + 1] = '\x00';
    strcat(buf, wireless::hostName().c_str());
    strcat(buf, " > ");
    t->print(buf);
}


