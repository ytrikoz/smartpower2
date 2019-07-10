#include "Shell.h"

Shell::Shell(Stream *inout) {
    this->inout = inout;
    terminal = new TerminalWriter(inout);
    in_buf = new Buffer(INPUT_MAX_LENGTH);
    cc_buf = new Buffer(32);
    echoEnabled = false;

    state = ST_INACTIVE;
}

TerminalWriter *Shell::getTerminal() { return terminal; }

void Shell::setParser(SimpleCLI *parser) { this->parser = parser; }

void Shell::setOnQuit(QuitShellEventHandler eventHandler) {
    onQuitShellEvent = eventHandler;
}

void Shell::setOnStart(StartShellEventHandler eventHandler) {
    onStartShellEvent = eventHandler;
}

void Shell::start() {
    terminal->print(FPSTR(msg_cli_hint));
    terminal->println();
}

void Shell::setEOL(EOLMarker eol) { this->eol = eol; }

void Shell::enableEcho(bool enabled) { echoEnabled = enabled; }

void Shell::onStart() {
    if (!onStartShellEvent) {
#ifdef DEBUG_SHELL
        debug->println("[shell] disabled");
#endif
        return;
    }

    if (onStartShellEvent(terminal)) {
#ifdef DEBUG_SHELL
        debug->println("[shell] start");
#endif
        state = ST_NORMAL;
        terminal->prompt();
        return;
    }

#ifdef DEBUG_SHELL
    debug->println("[shell] denied");
#endif
    terminal->print(F("access denied"));
}

void Shell::onCancel() {
#ifdef DEBUG_SHELL
    debug->print("[shell] cancel");
#endif
    in_buf->clear();
    terminal->clear_eol();
}

void Shell::onQuit() {
#ifdef DEBUG_SHELL
    debug->print("[shell] quit");
#endif
    terminal->println();
    if (onQuitShellEvent) onQuitShellEvent();
    terminal->print(FPSTR(msg_cli_hint));
    terminal->println();
}

void Shell::loop() {
    while (inout->available()) {
        int ch = inout->read();

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
        if (state == ST_ESC_SEQ) {  // ESC PRESSED TWICE
            if (ch == CHAR_ESC) {
                if (in_buf->empty()) {
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
                cc_buf->insert(ch);
                if (cc_buf->length() >= 3) {
#ifdef DEBUG_SHELL
                    debug->printf("[shell] esc seq: %d", cc_buf.get());
#endif
                    cc_buf->clear();
                    state = ST_NORMAL;
                }
            }
            continue;
        }

        if (state == ST_CTRL_SEQ) {
            cc_buf->insert(ch);
            if (cc_buf->length() >= 2) {
#ifdef DEBUG_SHELL
                debug->printf("[shell] ctrl seq: %d", cc_buf.get());
#endif
                cc_buf->clear();
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

    if (ch == 195) {
        state = ST_CTRL_SEQ;
        return;
    }

    if (ch == CHAR_CR) {
#ifdef DEBUG_SHELL
        debug->print("[CR]");
#endif
        terminal->println();
        if (!in_buf->empty()) {
            char line[INPUT_MAX_LENGTH];
            strncpy(line, in_buf->get(), INPUT_MAX_LENGTH);
            parser->parse(line);
            while (parser->available()) {
                Command cmd = parser->getCmd();
                // TODO History
                terminal->println();
                cmd.run();
            }
            in_buf->clear();
        }
        terminal->prompt();
        return;
    }

    if (ch == CHAR_BS || ch == CHAR_DEL) {
#ifdef DEBUG_SHELL
        debug->printf("[DEL]");
#endif
        if (!in_buf->empty()) {
            in_buf->del();
            terminal->del();
        }
        return;
    }

    if (isprint(ch)) {
#ifdef DEBUG_SHELL
        debug->print(ch);
#endif
        in_buf->insert(ch);
        if (echoEnabled) {
            terminal->write(ch);
        }
        return;
    }
}
