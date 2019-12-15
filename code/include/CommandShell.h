#pragma once

#include <Arduino.h>
#include <SimpleCLI.h>

#include "Terminal.h"

class CommandRunner {
  public:
  virtual void run(const char*, Print*) = 0;
};

class CommandShell {
  public:
    CommandShell(CommandRunner*);
    void setTerminal(Terminal *);
    void enableWelcome(bool = true);

    void clearHistory();
    void addHistory(const char *);
    bool getHistoryInput(String &);
    void setEditBuffer(String &);
    bool isOpen();
    bool run(const char *cmdStr);
    void loop();
  private:
    void onOpen(Print* out);
    void onClose(Print* out);
    void onData(const char *);
    void onHistory(Print* out);
    bool getLastInput(String &);
    
    size_t print_prompt(Print *);
    size_t print_welcome(Print *);
    size_t print_shell_exit(Print *);

  private:
    bool open_;
    bool welcome_;
    CommandRunner *runner_;
    Terminal *term_;
    std::vector<String> history;
    uint8_t history_size;
    uint8_t history_pos;
};