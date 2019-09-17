#include "Termul.h"

#include <mcurses.h>

#include "ArrayBuffer.h"
#include "SysInfo.h"

Termul::Termul(Stream *s) {
    this->input = new EditBuffer(INPUT_MAX_LENGTH);
    setStream(s);
}

bool Termul::available() { return s->available(); }
void Termul::setStream(Stream *s) { this->s = s; }

void Termul::setOnTab(TermulEventHandler handler) { onTabPressed = handler; }
void Termul::setOnInput(TermulInputEventHandler handler) {
    onInputEvent = handler;
}
void Termul::setOnStart(TermulEventHandler handler) { onStartEvent = handler; }
void Termul::setOnQuit(TermulEventHandler handler) { onQuitEvent = handler; }

void Termul::setEOL(EOLCode code) { this->eol = code; }
void Termul::enableEcho(bool enabled) { this->echoEnabled = enabled; }
void Termul::enableColors(bool enabled) { this->colorEnabled = enabled; }
void Termul::enableControlCodes(bool enabled) {
    this->controlCodesEnabled = enabled;
}

void Termul::quit() {}

void Termul::read() {
    uint8_t startY = curY;
    uint8_t startX = curX;
    sint8_t moveX = 0;

    while (s->available()) {
        int c = s->read();
        unsigned long now = millis();
        // when inactive
        if (state == ST_INACTIVE) {
            // wait for cr
            if (c == CHAR_CR) {
                if (onStartEvent) onStartEvent();
                state = ST_NORMAL;
            }
            // or ignore all other
            continue;
        }
        if (c == CHAR_LF || c == CHAR_NULL || c == CHAR_BIN) continue;
#ifdef DEBUG_TERMUL
        DEBUG.printf("#%d", int(ch));
#endif
        // ESC SEQUENCE
        if (state == ST_ESC_SEQ) {
            if (now - lastReceived >= 100) {
                if (c == CHAR_ESC) {
                    cc_index = 0;
                    if (!input->available()) {
                        // QUIT
                        if (onQuitEvent) onQuitEvent();
                        state = ST_INACTIVE;
                        return;
                    } else {
                        // CLEAR
                        if (controlCodesEnabled) {
                            clear_line();
                        } else {
                            println();
                        }
                        input->clear();
                        state = ST_NORMAL;
                        continue;
                    }
                } else {
                    state = ST_NORMAL;
                }
            } else {
                // ESC SEQUENCE RECEIVING
                cc_buf[cc_index] = c;
                if ((c == '[') || ((c >= 'A' && c <= 'Z') || c == '~')) {
                    cc_index++;
                }
                cc_buf[cc_index] = '\x00';

                uint8_t idx;
                for (idx = 0; idx < MAX_FN_KEY; idx++) {
                    if (!strcmp(cc_buf, fn_keys[idx])) {
                        c = idx + 0x80;
                        state = ST_NORMAL;
                        break;
                    }
                }

                lastReceived = now;

                if (state == ST_NORMAL) {
#ifdef DEBUG_TERMUL
                    DEBUG.printf("[termul] esc seq %s", cc_buf);
#endif
                } else {
                    continue;
                }
            }
        }

        // WHEN NORMAL
        if (state == ST_NORMAL) {
            if (c == CHAR_ESC) {
                state = ST_ESC_SEQ;
                continue;
            }
            if (c == CHAR_CR) {
                println();
                if (onInputEvent) onInputEvent(input->c_str());
                input->clear();
                continue;
            }
            if (c == CHAR_TAB) {
                if (onTabPressed) onTabPressed();
                continue;
            }

            switch (c) {
                case CHAR_LEFT:
                    input->prev();
                    moveX--;
                    break;
                case CHAR_RIGHT:
                    input->next();
                    moveX++;
                    break;
                case CHAR_HOME:
                    moveX = -1 * input->available();
                    input->first();
                    break;
                case CHAR_END:
                    moveX = input->available();
                    input->last();
                    break;
                case CHAR_BS:
                case CHAR_DEL:
                    if (input->available()) {
                        backsp();
                        input->onBackspace();
                    }
                    break;
                default:
                    // printable ascii 7bit or printable 8bit ISO8859
                    if ((c & '\x7F') >= 32 && (c & '\x7F') < 127)
                        if (input->write((uint8_t)c) && echoEnabled) write(c);
                    break;
            }
            if (controlCodesEnabled) move(startY, startX + moveX);
        }
    }
}

bool Termul::setEditBuffer(const uint8_t *bytes, size_t size) {
    input->clear();
    size_t max_len = input->free() - 1;
    if (size > max_len) size = max_len;
    return input->write(bytes, size);
    //((const uint8_t*) str, strlen(str));
}

EditBuffer *Termul::getEditBuffer() { return this->input; }

void Termul::start() {
    if (controlCodesEnabled) initscr();
    write_P(msg_shell_start_hint);
    println();
}

void Termul::initscr() {
    write_P(SEQ_LOAD_G1);
    attrset(A_NORMAL);
    clear();
    move(0, 0);
}

void Termul::attrset(const uint16_t attr) {
    uint8_t idx;

    if (attr != this->attr) {
        this->write_P(SEQ_ATTRSET);

        idx = (attr & F_COLOR) >> 8;

        if (idx >= 1 && idx <= 8) {
            this->write_P(SEQ_ATTRSET_FCOLOR);
            this->write(idx - 1 + '0');
        }

        idx = (attr & B_COLOR) >> 12;

        if (idx >= 1 && idx <= 8) {
            this->write_P(SEQ_ATTRSET_BCOLOR);
            this->write(idx - 1 + '0');
        }

        if (attr & A_REVERSE) this->write_P(SEQ_ATTRSET_REVERSE);
        if (attr & A_UNDERLINE) this->write_P(SEQ_ATTRSET_UNDERLINE);
        if (attr & A_BLINK) this->write_P(SEQ_ATTRSET_BLINK);
        if (attr & A_BOLD) this->write_P(SEQ_ATTRSET_BOLD);
        if (attr & A_DIM) this->write_P(SEQ_ATTRSET_DIM);
        this->write('m');
        this->attr = attr;
    }
}

void Termul::clear() { write_P(SEQ_CLEAR); }

void Termul::clear_line() {
    write(CHAR_CR);
    write_P(ESC_CLEAR_EOL);
}

void Termul::move(uint8_t y, uint8_t x) {
    write_P(SEQ_CSI);
    write(y + '0');
    write(';');
    write(x + '0');
    write('H');
}

void Termul::backsp() {
    write(CHAR_BS);
    write(CHAR_SP);
    write(CHAR_BS);
}

size_t Termul::println(const char *str) {
    size_t n = print(str);
    n += println();
    return n;
}

size_t Termul::println(void) {
    size_t n = 0;
    switch (eol) {
        case CRLF:
            write(CHAR_CR);
            write(CHAR_LF);
            n = 2;
            break;
        case LF:
            write(CHAR_LF);
            n = 1;
            break;
        case LFCR:
            write(CHAR_LF);
            write(CHAR_CR);
            n = 2;
            break;
        case CR:
            write(CHAR_CR);
            n = 1;
            break;
    }
    return n;
}

size_t Termul::write(uint8_t ch) {
    if (s) {
        s->write(ch);
        return 1;
    }
    return 0;
}

size_t Termul::write_P(PGM_P str) {
    uint8_t ch;
    size_t n = 0;
    while ((ch = pgm_read_byte(str)) != '\x00') {
        s->write(ch);
        str++;
        n++;
    }
    return n;
}

size_t Termul::write(const uint8_t *buffer, size_t size) {
    size_t n = 0;
    while (size--) {
        if (s->write(*buffer++))
            n++;
        else
            break;
    }
    return n;
}