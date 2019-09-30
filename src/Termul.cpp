#include "Termul.h"

#include <mcurses.h>

#include "ArrayBuffer.h"
#include "SysInfo.h"

Termul::Termul() { this->editor = new EditBuffer(INPUT_MAX_LENGTH); }

Termul::Termul(Stream *console) : Termul() { setConsole(console); }

void Termul::setConsole(Stream *console) { this->s = console; }

bool Termul::available() { return s->available(); }

void Termul::setOnTabKey(TermulEventHandler handler) { onTabPressed = handler; }

void Termul::setOnReadLine(TermulInputEventHandler handler) {
    onInputEvent = handler;
}
void Termul::setOnOpen(TermulEventHandler handler) { onStartHandler = handler; }

void Termul::setOnClose(TermulEventHandler handler) { onQuitEvent = handler; }

void Termul::setEOL(EOLType eol) { this->eol = eol; }

void Termul::enableEcho(bool enabled) { this->echoEnabled = enabled; }

void Termul::enableColors(bool enabled) { this->colorEnabled = enabled; }

void Termul::enableControlCodes(bool enabled) {
    this->controlCodesEnabled = enabled;
}

void Termul::quit() {}

void Termul::loop() {
    if ((!s) || (!s->available()))
        return;

    uint8_t startY = curY;
    uint8_t startX = curX;
    sint8_t moveX = 0;

    char c = s->read();
    lastReceived = millis();

    // when inactive
    if (state == ST_INACTIVE) {
        // wait for cr
        if (c == CHAR_CR) {
            if (onStartHandler)
                onStartHandler();
            state = ST_NORMAL;
        }
        // or ignore all other
        return;
    }

    if (c == CHAR_LF || c == CHAR_NULL || c == CHAR_BIN)
        return;

    // Esc
    if (c == CHAR_ESC || c == 195) {
        state = ST_ESC_SEQ;
        cc_pos = 0;
        for (size_t i = 0; i < 2; ++i) {
            bool timeout = false;
            while (!s->available() &&
                   !(timeout = millis_since(lastReceived) > 50)) {
                delay(0);
            }
            if (timeout) {
                state = ST_NORMAL;
                break;
            }
            lastReceived = millis();
            c = s->read();
            cc_buf[cc_pos] = c;
            if ((c == '[') || ((c >= 'A' && c <= 'Z') || c == '~')) {
                cc_pos++;
                cc_buf[++cc_pos] = '\x00';
            }
        }
        uint8_t i;
        for (i = 0; i < 10; ++i) {
            if (strcmp(cc_buf, codeMap[i].cc) == 0) {
                c = codeMap[i].ch;
                state = ST_NORMAL;
            }
        }
    }

    if (state == ST_ESC_SEQ) {
        state = ST_NORMAL;
        return;
    }

    // WHEN NORMAL
    if (state == ST_NORMAL) {
        if (c == CHAR_ESC) {
            if (!editor->available()) {
                // QUIT
                state = ST_INACTIVE;
                if (onQuitEvent)
                    onQuitEvent();
            } else {
                // CLEAR
                editor->clear();
                if (controlCodesEnabled) {
                    clear_line();
                } else {
                    println();
                }
            }
            return;
        }

        switch (c) {
        case CHAR_CR:
            println();
            if (onInputEvent)
                onInputEvent(editor->c_str());
            editor->clear();
            return;
        case CHAR_TAB:
            if (onTabPressed)
                onTabPressed();
            return;
        case KEY_LEFT:
            if (editor->prev())
                moveX--;
            break;
        case KEY_RIGHT:
            if (editor->next())
                moveX++;
            break;
        case KEY_HOME:
            moveX = -1 * editor->available();
            editor->first();
            break;
        case KEY_END:
            moveX = editor->available();
            editor->last();
            break;
        case CHAR_BS:
        case KEY_DEL:
            if (editor->backspace()) {
                backsp();
            }
            break;
        default:
            // printable ascii 7bit or printable 8bit ISO8859
            if ((c & '\x7F') >= 32 && (c & '\x7F') < 127)
                if (editor->write((uint8_t)c))
                    if (echoEnabled)
                        write(c);
            break;
        }

        // if (controlCodesEnabled) move(startY, startX + moveX);
    }
}

bool Termul::setEditBuffer(const uint8_t *bytes, size_t size) {
    editor->clear();
    size_t max_len = editor->free() - 1;
    if (size > max_len)
        size = max_len;
    return editor->write(bytes, size);
}

EditBuffer *Termul::getEditBuffer() { return this->editor; }

void Termul::start() {
    if (controlCodesEnabled)
        initscr();
    println();
}

void Termul::initscr() {
    write_P(SEQ_LOAD_G1);
    attrset(A_NORMAL);
    clear();
    move(0, 0);
}

void Termul::attrset(const uint16_t attr) {
    uint8_t i;

    if (attr != this->attr) {
        this->write_P(SEQ_ATTRSET);

        i = (attr & F_COLOR) >> 8;

        if (i >= 1 && i <= 8) {
            this->write_P(SEQ_ATTRSET_FCOLOR);
            this->write(i - 1 + '0');
        }

        i = (attr & B_COLOR) >> 12;

        if (i >= 1 && i <= 8) {
            this->write_P(SEQ_ATTRSET_BCOLOR);
            this->write(i - 1 + '0');
        }

        if (attr & A_REVERSE)
            this->write_P(SEQ_ATTRSET_REVERSE);
        if (attr & A_UNDERLINE)
            this->write_P(SEQ_ATTRSET_UNDERLINE);
        if (attr & A_BLINK)
            this->write_P(SEQ_ATTRSET_BLINK);
        if (attr & A_BOLD)
            this->write_P(SEQ_ATTRSET_BOLD);
        if (attr & A_DIM)
            this->write_P(SEQ_ATTRSET_DIM);
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
    writeByDigit(y + 1);
    write(';');
    writeByDigit(x + 1);
    write('H');
}

void Termul::writeByDigit(uint8_t i) {
    uint8_t ii;
    if (i >= 10) {
        if (i >= 100) {
            ii = i / 100;
            write(ii + '0');
            i -= 100 * ii;
        }
        ii = i / 10;
        write(ii + '0');
        i -= 10 * ii;
    }
    write(i + '0');
}

void Termul::backsp() {
    write(CHAR_BS);
    write(CHAR_SPACE);
    write(CHAR_BS);
}

size_t Termul::println(const char *str) {
    size_t n = print(str);
    return n += println();
}

size_t Termul::println(void) {
    size_t n = 0;
    switch (eol) {
    case CRLF:
        n += write(CHAR_CR);
        n += write(CHAR_LF);
        break;
    case LF:
        n += write(CHAR_LF);
        break;
    case LFCR:
        n += write(CHAR_LF);
        n += write(CHAR_CR);
        break;
    case CR:
        n += write(CHAR_CR);
        break;
    }
    return n;
}

size_t Termul::write(uint8_t ch) {
    if (s) {
        return s->write(ch);
    }
    return 0;
}

size_t Termul::write_P(PGM_P str) {
    uint8_t ch;
    size_t n = 0;
    while ((ch = pgm_read_byte(str + n)) != '\x00') {
        s->write(ch);
        n++;
    }
    return n;
}

size_t Termul::write(const uint8_t *buf, size_t size) {
    size_t n = 0;
    while (size--) {
        if (s->write(*buf++))
            n++;
        else
            break;
    }
    return n;
}