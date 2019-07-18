#include "Termul.h"

#include "sysinfo.h"

Termul::Termul() {
    this->eol = CRLF;
    this->cc = false;
}

void Termul::setStream(Stream *s)
{
    this->s = s;
}

void Termul::setEOL(EOLCode code)
{
    this->eol = code;
}

void Termul::setControlCodes(bool enabled) {
    cc = enabled;
}

bool Termul::available()
{
    return s->available();
}

int Termul::read()
{
    return s->read();
}

void Termul::clear_line()
{
    if (cc) {
        print(CHAR_CR);
        print(ESC_CLEAR_EOL);
    } else {
        println();
    }
}

void Termul::del() {
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
            write('\x0d');
            write('\x0a');
            n = 2;
            break;
        case LF:
            write('\x0a');
            n = 1;
            break;
        case LFCR:
            write('\x0a');
            write('\x0d');
            n = 2;
            break;
        case CR:
            write('\x0d');
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