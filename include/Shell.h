#pragma once
#include "Stream.h"

#include <SimpleCLI.h>

#include "Buffer.h"
#include "TerminalWriter.h"

#include "consts.h"
#include "ip_utils.h"
#include "str_utils.h"
#include "sysinfo.h"

typedef enum { ST_INACTIVE, ST_NORMAL, ST_ESC_SEQ, ST_CTRL_SEQ } State;

typedef std::function<bool(TerminalWriter *)> StartShellEventHandler;
typedef std::function<void()> QuitShellEventHandler;

static const char msg_cli_hint[] PROGMEM = "Press \"enter\" to command prompt";

class Shell {
   public:
    Shell(Stream *input);
    void setTerminal(TerminalWriter *terminal);
    void setParser(SimpleCLI *cli);
    void setOnStart(StartShellEventHandler eventHandler);
    void setOnQuit(QuitShellEventHandler eventHandler);
    void enableEcho(bool enabled = true);
    void setEOL(EOLMarker marker);
    void start();
    void loop();
    TerminalWriter *getTerminal();

   private:
#ifdef DEBUG_SHELL_INPUT
    Print *debug = &USE_DEBUG_SERIAL;
#endif

    void onInput(const char ch);
    void onStart();
    void onCancel();
    void onQuit();
    void onDelete();

    TerminalWriter *terminal;
    State state;
    bool echoEnabled;

    EOLMarker eol;

    SimpleCLI *parser;

    Stream *inout;
    Buffer *in_buf, *cc_buf;

    StartShellEventHandler onStartShellEvent;
    QuitShellEventHandler onQuitShellEvent;

    void print_prompt();

    void _clear();
    void _clearEOL();
    void _redraw(const char *str);
    void _cursor(MoveDirection move, uint8_t n = 1);
};
