#include "Cli/Terminal.h"

#include "PrintUtils.h"
#include "SysInfo.h"

using namespace PrintUtils;

namespace Cli {

Terminal::Terminal(Stream *stream) : stream_(stream){};

void Terminal::setStream(Stream *stream) {
    stream_ = stream;
}

Terminal::~Terminal() {}

void Terminal::setOnReadLine(TerminalInputEventHandler h) { inputHandler_ = h; }

void Terminal::setOnEvent(TerminalEventHandler h) { eventHandler_ = h; }

bool Terminal::available() {
    return stream_ != nullptr ? stream_->available() : false;
}

void Terminal::setEOL(EOLType eol) { this->eol = eol; }

void Terminal::enableEcho(bool enabled) { this->echoEnabled = enabled; }

void Terminal::enableColors(bool enabled) { this->colorEnabled = enabled; }

void Terminal::enableControlCodes(bool enabled) {
    this->controlCodesEnabled = enabled;
}

void Terminal::quit() {}

void Terminal::loop() {
    if (stream_ == nullptr || !stream_->available()) return;

    // uint8_t startY = curY;
    // uint8_t startX = curX;
    sint8_t moveX = 0;
    sint8_t moveY = 0;

    char c = stream_->read();

    lastReceived = millis();

    if (state == ST_INACTIVE) {
        // wait for CR
        if (c == CHAR_CR) {
            if (eventHandler_) {
                eventHandler_(EVENT_OPEN, stream_);
                state = ST_NORMAL;
            }
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
            while (!stream_->available() &&
                   !(timeout = millis_since(lastReceived) > 10)) {
                delay(0);
            }
            if (timeout) {
                state = ST_NORMAL;
                break;
            }
            lastReceived = millis();
            c = stream_->read();
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
            if (!line.available()) {
                // QUIT
                state = ST_INACTIVE;
                if (eventHandler_)
                    eventHandler_(EVENT_CLOSE, stream_);
            } else {
                // CLEAR
                line.clear();
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
                if (inputHandler_)
                    inputHandler_(line.c_str());
                line.clear();
                moveY++;
                break;
            case CHAR_TAB:
                if (eventHandler_)
                    eventHandler_(EVENT_TAB, stream_);
                return;
            case KEY_LEFT:
                if (line.prev())
                    moveX--;
                break;
            case KEY_RIGHT:
                if (line.next())
                    moveX++;
                break;
            case KEY_HOME:
                moveX = -1 * line.home();
                break;
            case KEY_END:
                moveX = line.end();
                break;
            case CHAR_BS:
            case KEY_DEL:
                if (line.backspace()) {
                    backsp();
                    moveX--;
                }
                break;
            default:
                // printable ascii 7bit or printable 8bit ISO8859
                if ((c & '\x7F') >= 32 && (c & '\x7F') < 127)
                    if (line.write(c)) {
                        if (echoEnabled)
                            write(c);
                        moveX++;
                    }
                break;
        }

        // if (controlCodesEnabled)
        // move(startY + moveY, startX + moveX);
    }
}

bool Terminal::setLine(const uint8_t *ptr, size_t size) {
    line.clear();
    return line.write(ptr, size);
}

CharBuffer &Terminal::getLine() { return this->line; }

void Terminal::start() {
    if (controlCodesEnabled)
        initscr();
    println();
}

void Terminal::initscr() {
    write_P(SEQ_LOAD_G1);
    attrset(A_NORMAL);
    move(0, 0);
    clear();
}

void Terminal::attrset(const uint16_t attr) {
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

void Terminal::clear() { write_P(SEQ_CLEAR); }

void Terminal::clear_line() {
    write(CHAR_CR);
    write_P(ESC_CLEAR_EOL);
}

void Terminal::move(uint8_t y, uint8_t x) {
    write_P(SEQ_CSI);
    writeByDigit(y + 1);
    write(';');
    writeByDigit(x + 1);
    write('H');
    curY = y;
    curX = x;
}

void Terminal::writeByDigit(uint8_t i) {
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

void Terminal::backsp() {
    write(CHAR_BS);
    write(CHAR_SPACE);
    write(CHAR_BS);
}

size_t Terminal::println(const char *str) {
    size_t n = print(str);
    return n += println();
}

size_t Terminal::println(void) {
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

size_t Terminal::write(uint8_t ch) {
    size_t n = 0;
    if (stream_)
        n = stream_->write(ch);
    return n;
}

size_t Terminal::write_P(PGM_P str) {
    uint8_t ch;
    size_t n = 0;
    while ((ch = pgm_read_byte(str + n)) != '\x0') {
        stream_->write(ch);
        n++;
    }
    return n;
}

size_t Terminal::write(const uint8_t *buf, size_t size) {
    size_t n = 0;
    while (size--) {
        if (stream_->write(*buf++))
            n++;
        else
            break;
    }
    return n;
}

}  // namespace Cli