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
    void setEditBuffer(String& str);
    void clearHistory();
    void addHistory(const char* str);
    bool getHistoryInput(String& str);
   private:
    bool getLastInput(String& str);
    void onOpen();
    void onLineInput(const char *);
    void onTabPress();
    void onClose();        
    void print_prompt();
    void print_welcome();

    SimpleCLI *cli;
    Termul *t;
    std::vector<String> history;
    uint8_t history_size;
    uint8_t history_pos;
    bool welcomeEnabled;
    bool active;
};