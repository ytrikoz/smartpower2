#pragma once

#include "EditBuffer.h"
#include "CommonTypes.h"
#include "Strings.h"
#include "SystemClock.h"

#define CHAR_NULL '\x00'
#define CHAR_BEL '\x07'
#define CHAR_BS '\x08'
#define CHAR_TAB '\x09'
#define CHAR_LF '\x0a'
#define CHAR_CR '\x0d'
#define CHAR_SP '\x20'
#define CHR_ZERO '\x30'
#define CHAR_DEL '\x7f'
#define CHAR_DOWN '\x80'
#define CHAR_UP '\x81'
#define CHAR_LEFT '\x82'
#define CHAR_RIGHT '\x83'
#define CHAR_HOME '\x84'
#define CHAR_END '\x89'
#define CHAR_LT '\x8b'
#define CHAR_CSI '\x9b'
#define CHAR_ESC '\x1b'
#define CHAR_BIN '\xFF'

#define CHAR_KEY_F1 0x8B                // Function key F1
#define CHAR_KEY_F(n) (KEY_F1 + (n)-1)  // Space for additional 12 function keys

#define ESC_CURSOR_HOME "\x1b[H"
#define ESC_SAVE_CURSOR "\x1b[s"
#define ESC_UNSAVE_CURSOR "\x1b[u"
#define ESC_SAVE_CURSOR_AND_ATTRS "\x1b[7"
#define ESC_RESTORE_CURSOR_AND_ATTRS "\x1b[8"

#define ESC_CLEAR "\x1b[2J"
#define ESC_CLEAR_BOTTOM "\x1b[J"
#define ESC_CLEAR_EOL "\x1b[0K"

#define ESCF_CURSOR_BACKWARD "\x1b[%dD"
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

#define MAX_FN_KEY ((CHAR_KEY_F1 + 12) - 0x80)

typedef std::function<void()> TermulEventHandler;

typedef std::function<void(const char *)> TermulInputEventHandler;

class Termul : public Print {
   public:
    Termul(Stream *s);
    ~Termul();
    void setStream(Stream *s);
    void setEOL(EOLCode code);
    void enableControlCodes(bool enabled = true);
    void enableEcho(bool enabled = true);
    void enableColors(bool enabled = false);
    void setOnInput(TermulInputEventHandler handler);
    void setOnTab(TermulEventHandler handler);
    void setOnQuit(TermulEventHandler handler);
    void setOnStart(TermulEventHandler handler);
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
    bool available();
    void read();
    void start();
    void quit();
    void initscr();
    void attrset(uint16_t attr);

   private:
    void move(uint8_t y, uint8_t x);
    TermulEventHandler onTabPressed;
    TermulInputEventHandler onInputEvent;
    TermulEventHandler onStartEvent;
    TermulEventHandler onQuitEvent;

    uint8_t attr = '\xff';
    uint8_t curY = '\xff';
    uint8_t curX = '\xff';

    unsigned long lastReceived = 0;
    State state = ST_INACTIVE;
    Stream *s;
    EditBuffer *input;
    char cc_buf[32] = {0};
    size_t cc_index = 0;
    bool colorEnabled = false;
    bool controlCodesEnabled = false;
    bool echoEnabled = false;

    EOLCode eol = CRLF;

    const char *fn_keys[MAX_FN_KEY] = {
        "B",  // KEY_DOWN                 0x80                // Down arrow key
        "A",  // KEY_UP                   0x81                // Up arrow key
        "D",  // KEY_LEFT                 0x82                // Left arrow key
        "C",  // KEY_RIGHT                0x83                // Right arrow key
        "1~",   // KEY_HOME                 0x84                // Home key
        "3~",   // KEY_DC                   0x85                // Delete
                // character key
        "2~",   // KEY_IC                   0x86                // Ins
                // char/toggle ins mode key
        "6~",   // KEY_NPAGE                0x87                // Next-page key
        "5~",   // KEY_PPAGE                0x88                // Previous-page
                // key
        "4~",   // KEY_END                  0x89                // End key
        "Z",    // KEY_BTAB                 0x8A                // Back tab key
        "[A",   // KEY_F(1)                 0x8B                // Function key
                // F1
        "[B",   // KEY_F(2)                 0x8C                // Function key
                // F2
        "[C",   // KEY_F(3)                 0x8D                // Function key
                // F3
        "[D",   // KEY_F(4)                 0x8E                // Function key
                // F4
        "[E",   // KEY_F(5)                 0x8F                // Function key
                // F5
        "17~",  // KEY_F(6)                 0x90                // Function key
                // F6
        "18~",  // KEY_F(7)                 0x91                // Function key
                // F7
        "19~",  // KEY_F(8)                 0x92                // Function key
                // F8
        "20~",  // KEY_F(9)                 0x93                // Function key
                // F9
        "21~",  // KEY_F(10)                0x94                // Function key
                // F10
        "23~",  // KEY_F(11)                0x95                // Function key
                // F11
        "24~"   // KEY_F(12)                0x96                // Function key
                // F12
    };
};
