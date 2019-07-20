#pragma once

#include "Stream.h"

#include <SystemClock.h>
#include <SimpleCLI.h>
#include "Buffer.h"
#include "Termul.h"
#include "consts.h"
#include "str_utils.h"
#include "sysinfo.h"

typedef std::function<bool(Termul *)> StartShellEventHandler;
typedef std::function<void()> QuitShellEventHandler;

class Shell {
   public:  
    Shell();
    ~Shell();    
    void setTermul(Termul *term);
    Termul *getTerm();
    void setParser(SimpleCLI *cli);
    void setOnStart(StartShellEventHandler eventHandler);
    void setOnQuit(QuitShellEventHandler eventHandler);
    void enableEcho(bool enabled = true);
    void enableWelcome(bool enabled = true);
    void start();
    void loop();
    void prompt();
    void welcome();

   private:
#ifdef DEBUG_SHELL_INPUT
    Print *debug = &USE_DEBUG_SERIAL;
#endif
    StartShellEventHandler onStartShellEvent;
    QuitShellEventHandler onQuitShellEvent;

    void onInput(const char ch);
    void onStart();
    void onCancel();
    void onQuit();
    void onDelete();

    SimpleCLI *parser;
    Termul *t;
    EOLCode eol;
    Buffer in_buf, cc_buf, prev_buf;
    State state;
    bool echoEnabled;
    bool welcomeEnabled;
};
