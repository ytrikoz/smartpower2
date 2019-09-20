#pragma once

#include "CommonTypes.h"
#include "EditBuffer.h"
#include "Strings.h"
#include "SystemClock.h"

#define CHAR_NULL 0x00
#define CHAR_BEL 0x07
#define CHAR_BS 0x08
#define CHAR_SPACE 0x20
#define CHAR_TAB 0x09
#define CHAR_LF 0x0a
#define CHAR_CR 0x0d
#define CHR_ZERO 0x30

#define KEY_DEL 0x7f
#define KEY_DOWN 0x80
#define KEY_UP 0x81
#define KEY_LEFT 0x82
#define KEY_RIGHT 0x83
#define KEY_HOME 0x84
#define KEY_INS 0x86
#define KEY_PAGE_DOWN 0x87
#define KEY_PAGE_UP 0x88
#define KEY_END 0x89
#define CHAR_LT 0x8b
#define CHAR_CSI 0x9b
#define CHAR_ESC 0x1b
#define CHAR_BIN 0xFF

#define ESC_CURSOR_HOME "\x1b[H"
#define ESC_SAVE_CURSOR "\x1b[s"
#define ESC_UNSAVE_CURSOR "\x1b[u"
#define ESC_SAVE_CURSOR_AND_ATTRS "\x1b[7"
#define ESC_RESTORE_CURSOR_AND_ATTRS "\x1b[8"

#define ESC_CLEAR "\x1b[2J"
#define ESC_CLEAR_BOTTOM "\x1b[J"
#define ESC_CLEAR_EOL "\x1b[0K"

#define ESC_CURSOR_UP "\x1b[1A"
#define ESC_CURSOR_DOWN "\x1b[1B"
#define ESC_CURSOR_FORWARD "\x1b[1C"
#define ESC_CURSOR_BACKWARD "\x1b[1D"

#define SEQ_CSI PSTR("\033[")       // code introducer
#define SEQ_LOAD_G1 PSTR("\033)0")  // load G1 character set
#define SEQ_CLEAR PSTR("\033[2J")   // clear screen
#define SEQ_ATTRSET PSTR("\033[0")  // set attributes, e.g. "\033[0;7;1m"

#define SEQ_ATTRSET_BOLD PSTR(";1")       // bold
#define SEQ_ATTRSET_DIM PSTR(";2")        // dim
#define SEQ_ATTRSET_FCOLOR PSTR(";3")     // forground color
#define SEQ_ATTRSET_UNDERLINE PSTR(";4")  // underline
#define SEQ_ATTRSET_BCOLOR PSTR(";4")     // background color
#define SEQ_ATTRSET_BLINK PSTR(";5")      // blink
#define SEQ_ATTRSET_REVERSE PSTR(";7")    // reverse

typedef std::function<void()> TermulEventHandler;

typedef std::function<void(const char *)> TermulInputEventHandler;

class Termul : public Print {
   public:
    Termul(Stream *console);
    ~Termul();
    void setConsole(Stream *);
    void setEOL(EOLType code);
    void enableControlCodes(bool enabled = true);
    void enableEcho(bool enabled = true);
    void enableColors(bool enabled = true);

    void setOnOpen(TermulEventHandler handler);
    void setOnClose(TermulEventHandler handler);
    void setOnReadLine(TermulInputEventHandler handler);
    void setOnTabKey(TermulEventHandler handler);

    bool setEditBuffer(const uint8_t *bytes, size_t size);
    EditBuffer *getEditBuffer();
    void backsp();
    void clear();
    void clear_line();
    size_t println(const char *str);
    size_t println(void);
    size_t write_P(PGM_P str);
    size_t write(uint8_t c);
    size_t write(const uint8_t *buf, size_t size);
    void writeByDigit(uint8_t i);
    bool available();
    void loop();
    void start();
    void quit();
    void initscr();
    void attrset(uint16_t attr);

   private:
    void move(uint8_t y, uint8_t x);
    TermulEventHandler onTabPressed;
    TermulInputEventHandler onInputEvent;
    TermulEventHandler onStartHandler;
    TermulEventHandler onQuitEvent;

    uint8_t attr = 0xff;
    uint8_t curY = 0xff;
    uint8_t curX = 0xff;

    unsigned long lastReceived = 0;
    State state = ST_INACTIVE;
    Stream *s;
    EditBuffer *editor;
    char cc_buf[32] = {0};
    size_t cc_pos = 0;
    bool colorEnabled = false;
    bool controlCodesEnabled = false;
    bool echoEnabled = false;

    EOLType eol = CRLF;

    struct ControlCode {
        const char *cc;
        const char ch;
    };

    ControlCode codeMap[10] = {
        {"G", KEY_HOME},       // 71 Home key
        {"H", KEY_UP},         // 72 Up arrow
        {"I", KEY_PAGE_UP},    // 73 PageUp
        {"K", KEY_LEFT},       // 75 Left arrow
        {"M", KEY_RIGHT},      // 77 Right arrow
        {"O", KEY_END},        // 79 End key
        {"P", KEY_DOWN},       // 80 Down arrow
        {"Q", KEY_PAGE_DOWN},  // 81 PageDown
        {"R", KEY_INS},        // 82 Insert
        {"S", KEY_DEL},        // 83 Delete
    };
};
