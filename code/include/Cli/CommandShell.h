#pragma once

#include <Arduino.h>
#include <SimpleCLI.h>

#include "Cli/Cli.h"
#include "Cli/Terminal.h"

namespace Cli {

class CommandShell {
  public:
    CommandShell(Cli::Runner* runner);    
    void setTerm(Cli::Terminal* term);
    Cli::Terminal* term();
    void enableWelcome(bool = true);

    void clearHistory();
    void addHistory(const char *);
    bool getHistoryInput(String &);
    void setEditBuffer(String &);
    bool isOpen();
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
    Cli::Runner *runner_;
    Cli::Terminal *term_;

    std::vector<String> history;
    uint8_t history_size;
    uint8_t history_pos;
};

}