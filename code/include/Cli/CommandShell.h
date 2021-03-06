#pragma once

#include <Arduino.h>
#include <SimpleCLI.h>

#include "Cli/Cli.h"
#include "Cli/Terminal.h"
#include "Core/CircularBuffer.h"

namespace Cli {

class CommandShell {
   public:
    CommandShell(Cli::Runner *runner);

    void setTerm(Cli::Terminal *term);
    Cli::Terminal *term();

    void showGreetings(bool = true);
    void showFarewell(bool = true);

    void clearHistory();
    void addHistory(const char *);
    bool getHistoryInput(String &);
    void setEditLine(const String &);
    bool active();
    void loop();

   private:
    size_t printGreetings(Print *);
    size_t printFarewell(Print *);
    size_t printPrompt(Print *);

    void onOpen(Print *out);
    void onClose(Print *out);
    void onData(const char *);
    void onHistory(Print *out);
    bool getLastInput(String &);

   private:
    CircularBuffer<String, SHELL_HISTORY_SIZE> history_;
    Cli::Terminal *term_;
    Cli::Runner *runner_;
    String path_;
    bool active_;
    bool greetings_;
    bool farewell_;
};

}  // namespace Cli