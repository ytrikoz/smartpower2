#include <TerminalWriter.h>

TerminalWriter::TerminalWriter(Print *p) {
    output = p;
    eol = CRLF;
    ctrlCodesEnabled = false;
}

void TerminalWriter::enableCtrlCodes(bool enabled) {
    ctrlCodesEnabled = enabled;
}

void TerminalWriter::clear_eol() {
    if (ctrlCodesEnabled) {
        print(CHAR_CR);
        print(ESC_CLEAR_EOL);
    } else {
        println();
    }
    prompt();
}

void TerminalWriter::del() {
    write(CHAR_BS);
    write(CHAR_SPACE);
    write(CHAR_BS);
}

void TerminalWriter::prompt() {
    char buf[OUTPUT_MAX_LENGTH + 1];
    memset(&buf[0], '\x00', sizeof(buf[0]) * OUTPUT_MAX_LENGTH + 1);
    strcpy(buf, rtc.getLocalFormated().c_str());

    if (isWiFiActive()) {
        buf[strlen(buf)] = ' ';
        buf[strlen(buf)+1] = '\x00';
        strcat(buf, wifi_station_get_hostname());
    }
    strcat(buf, " > ");
    print(buf);
}

size_t TerminalWriter::println(const char *str) {
    size_t n = print(str);
    n += println();
    return n;
}

size_t TerminalWriter::println(void) {
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

size_t TerminalWriter::write(uint8_t ch) {
    if (output) {
        output->write(ch);
        return 1;
    }
    return 0;
}

size_t TerminalWriter::write(const uint8_t *buffer, size_t size) {
    size_t n = 0;
    while (size--) {
        if (output->write(*buffer++))
            n++;
        else
            break;
    }
    return n;
}