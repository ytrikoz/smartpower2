#pragma once

#include <SimpleCLI.h>
#include <Stream.h>

#include "Consts.h"
#include "StrUtils.h"
#include "SysInfo.h"
#include "SystemClock.h"
#include "Termul.h"
#include "Wireless.h"

class Shell {
   public:
    Shell(SimpleCLI *cli, Termul *t);
    void loop();
    void enableWelcome(bool enabled = true);    
    Termul *getTerm();
    bool isActive();
   private:
    void onOpen();
    void onInput(const char *);
    void onTabPress();
    void onClose();    
    
    void print_prompt();
    void print_welcome();

    SimpleCLI *cli;
    Termul *t;
    char prevInput[INPUT_MAX_LENGTH];
    bool welcomeEnabled;
    bool active;
};