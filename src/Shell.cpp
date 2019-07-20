#include "Shell.h"
#include "wireless.h"

Shell::Shell()
    : in_buf((size_t)INPUT_MAX_LENGTH),
      cc_buf((size_t)32),
      prev_buf((size_t)INPUT_MAX_LENGTH),
      state(ST_INACTIVE),
      echoEnabled(false),
      welcomeEnabled(false) {}

void Shell::setParser(SimpleCLI *parser) { this->parser = parser; }

void Shell::setTermul(Termul *term) { this->t = term; }

Termul *Shell::getTerm() { return this->t; }

void Shell::setOnQuit(QuitShellEventHandler eventHandler) {
    onQuitShellEvent = eventHandler;
}

void Shell::setOnStart(StartShellEventHandler eventHandler) {
    onStartShellEvent = eventHandler;
}

void Shell::start() {
    t->printf_P(str_cli_hint);
    t->println();
}

void Shell::enableWelcome(bool enabled) { welcomeEnabled = enabled; }

void Shell::enableEcho(bool enabled) { echoEnabled = enabled; }

void Shell::onStart() {
    if (!onStartShellEvent) {
#ifdef DEBUG_SHELL
        debug->println("[shell] OnStartShell not set");
#endif
        return;
    }

    if (onStartShellEvent(t)) {
#ifdef DEBUG_SHELL
        debug->println("[shell] OnStartShell");
#endif
        state = ST_NORMAL;
        if (welcomeEnabled) {
            welcome();
        }
        prompt();
        return;
    }

#ifdef DEBUG_SHELL
    debug->println("[shell] denied");
#endif
    t->print(F("access denied"));
}

void Shell::onCancel() {
#ifdef DEBUG_SHELL
    debug->print("[shell] cancel");
#endif
    in_buf.clear();
    t->clear_line();
}

void Shell::onQuit() {
#ifdef DEBUG_SHELL
    debug->print("[shell] quit");
#endif
    t->println();
    if (onQuitShellEvent) onQuitShellEvent();
}

void Shell::loop() {
    if (!t) return;

    while (t->available()) {
        int ch = t->read();
        // IGNORE
        if (ch == CHAR_NULL || ch == 255) continue;
#ifdef DEBUG_SHELL
        debug->printf("#%d", int(ch));
#endif
        // WHEN INACTIVE
        if (state == ST_INACTIVE) {
            // ENTER PRESSED
            if (ch == CHAR_CR) onStart();
            continue;
        }
        // ESC SEQUENCE OR ESC KEY PRESS
        if (state == ST_ESC_SEQ) {
            if (ch == CHAR_ESC) {
                // ESC PRESSED TWICE
                if (in_buf.empty()) {
                    // QUIT
                    onQuit();
                    state = ST_INACTIVE;
                } else {
                    // CLEAR INPUT
                    onCancel();
                    state = ST_NORMAL;
                }
            } else {
                // ESC SEQUENCE RECEIVING
                cc_buf.insert(ch);
                if (cc_buf.length() >= 3) {
#ifdef DEBUG_SHELL
                    debug->printf("[shell] esc seq: %d", cc_buf.get());
#endif
                    cc_buf.clear();
                    state = ST_NORMAL;
                }
            }
            continue;
        }

        if (state == ST_CTRL_SEQ) {
            cc_buf.insert(ch);
            if (cc_buf.length() >= 2) {
#ifdef DEBUG_SHELL
                debug->printf("[shell] ctrl seq: %d", cc_buf.get());
#endif
                cc_buf.clear();
                state = ST_NORMAL;
            }
            continue;
        }

        // TEXT INPUT
        if (state == ST_NORMAL) {
            onInput(ch);
        }
    }
}

void Shell::onInput(const char ch) {
    if (ch == CHAR_ESC) {
        state = ST_ESC_SEQ;
        return;
    }

    if (ch == CHAR_TAB) {
        if (!prev_buf.empty()) {
            in_buf = prev_buf;
            t->println();
            prompt();
            t->print(in_buf.c_str());
        }
    }

    if (ch == 195) {
        state = ST_CTRL_SEQ;
        return;
    }

    if (ch == CHAR_CR) {
#ifdef DEBUG_SHELL
        debug->print("[CR]");
#endif
        t->println();
        if (!in_buf.empty()) {
            parser->parse(in_buf.c_str());
            while (parser->available()) {
                Command cmd = parser->getCmd();
                // TODO History
                t->println();
                cmd.run();
            }
            prev_buf = in_buf;
            prev_buf.insert('\x00');
            in_buf.clear();
        }
        prompt();
        return;
    }

    if (ch == CHAR_BS || ch == CHAR_DEL) {
#ifdef DEBUG_SHELL
        debug->printf("[DEL]");
#endif
        if (!in_buf.empty()) {
            in_buf.del();
            t->del();
        }
        return;
    }

    if (isprint(ch)) {
#ifdef DEBUG_SHELL
        debug->print(ch);
#endif
        in_buf.insert(ch);
        if (echoEnabled) {
            t->write(ch);
        }
        return;
    }
}

void Shell::welcome() {
    char title[SCREEN_WIDTH + 1];
    strcpy(title, APPNAME " v" FW_VERSION);
    uint8_t width = SCREEN_WIDTH / 2;
    str_utils::addPaddingTo(title, str_utils::CENTER, width, ' ');
    char decor[width + 1];
    str_utils::strOfChar(decor, '#', width);
    t->println(decor);
    t->println(title);
    t->println(decor);
    t->printf("echo: %s ", echoEnabled ? "yes" : "no");
    t->println();
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