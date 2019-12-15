#pragma once

#include "Runner.h"

#include "SimpleCLI.h"

namespace Cli {

class CliRunner : public Runner {
   public:
    CliRunner(SimpleCLI* cli) : cli_(cli){};

    void run(const char* command, Print* output) override {
        Print* prev = Cli::setOutput(output);
        cli_->parse(command);
        while (cli_->available()) {
            cli_->getCmd().run();
            if (cli_->getError()) {
                break;
            }
        }
        Cli::setOutput(prev);
    }

   private:
    SimpleCLI* cli_;
};

}