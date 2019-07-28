#include "Termul.h"
#include "mcurses.h"
#include "sysinfo.h"

Termul::Termul(Stream *s) {
    this->s = s;
    this->in_buf = new Buffer(INPUT_MAX_LENGTH);
}

Termul::~Termul() { delete[] in_buf; }

void Termul::setStream(Stream *s) { this->s = s; }
bool Termul::available() { return s->available(); }

void Termul::setOnTab(TermulEventHandler handler) { onTabPressed = handler; }
void Termul::setOnInput(TermulInputEventHandler handler) {
    onInputEvent = handler;
}
void Termul::setOnStart(TermulEventHandler handler) { onStartEvent = handler; }
void Termul::setOnQuit(TermulEventHandler handler) { onQuitEvent = handler; }

void Termul::setEOL(EOLCode code) { this->eol = code; }
void Termul::enableEcho(bool enabled) { this->echoEnabled = enabled; }
void Termul::enableControlCodes(bool enabled) {
    this->controlCodesEnabled = enabled;
}

void Termul::quit() {}

void Termul::read() {
    uint8_t startY = curY;
    uint8_t startX = curX;
    sint8_t moveX = 0;

    while (s->available()) {
        int ch = s->read();

        // WHEN INACTIVE
        if (state == ST_INACTIVE) {
            // ENTER PRESSED
            if (ch == CHAR_CR) {
                if (onStartEvent) onStartEvent();
                state = ST_NORMAL;
            }
            // IGNORE
            continue;
        }
        if (ch == CHAR_LF || ch == CHAR_NULL || ch == CHAR_BIN) continue;
#ifdef DEBUG_TERMUL
        DEBUG.printf("#%d", int(ch));
#endif
        // ESC SEQUENCE
        if (state == ST_ESC_SEQ) {
            if (millis() - lastControlCodeRecived > 100) {
                if (ch == CHAR_ESC) {
                    cc_index = 0;
                    if (in_buf->empty()) {
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
                        in_buf->clear();
                        state = ST_NORMAL;
                        continue;
                    }
                } else {
                    state = ST_NORMAL;
                }
            } else {
                // ESC SEQUENCE RECEIVING
                cc_buf[cc_index] = ch;
                if ((ch == '[') || ((ch >= 'A' && ch <= 'Z') || ch == '~')) {
                    cc_index++;
                }
                cc_buf[cc_index] = '\x00';

                uint8_t idx;
                for (idx = 0; idx < MAX_FN_KEY; idx++) {
                    if (!strcmp(cc_buf, fn_keys[idx])) {
                        ch = idx + 0x80;
                        state = ST_NORMAL;
                        break;
                    }
                }

                lastControlCodeRecived = millis();

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
            if (ch == CHAR_ESC) {
                state = ST_ESC_SEQ;
                continue;
            }
            if (ch == CHAR_CR) {
                println();
                if (onInputEvent) onInputEvent(in_buf->c_str());
                in_buf->clear();
                continue;
            }
            if (ch == CHAR_TAB) {
                if (onTabPressed) onTabPressed();
                continue;
            }

            switch (ch) {
                case CHAR_LEFT:
                    in_buf->prev();
                    moveX--;
                    break;
                case CHAR_RIGHT:
                    in_buf->next();
                    moveX++;
                    break;
                case CHAR_HOME:
                    moveX = -1 * in_buf->length();
                    in_buf->first();
                    break;
                case CHAR_END:
                    moveX = in_buf->length();
                    in_buf->last();
                    break;
                case CHAR_BS:
                case CHAR_DEL:
                    if (in_buf->length() > 0) {
                        backsp();
                        in_buf->backsp();
                    }
                    break;
                default:
                    // printable ascii 7bit or printable 8bit ISO8859
                    if ((in_buf->length() < in_buf->size()) &&
                        (ch & '\x7F') >= 32 && (ch & '\x7F') < 127) {
                        in_buf->insert(ch);
                        if (echoEnabled) {
                            write(ch);
                        }
                    }
            }
            if (controlCodesEnabled) move(startY, startX + moveX);
        }
    }
}

Buffer *Termul::input() { return this->in_buf; }

void Termul::start() {
    if (controlCodesEnabled) initscr();
    write_P(str_cli_hint);
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