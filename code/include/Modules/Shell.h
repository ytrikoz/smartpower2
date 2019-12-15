#pragma once

#include <Arduino.h>

#include "Module.h"
#include "CommandShell.h"

namespace Modules { 

class Shell : public Module, CommandRunner{
  public:
    Shell(SimpleCLI* cli_);
  public:
    bool isOpen();
    void setRemote(Terminal* term);
    void setLocal(Terminal* term);    
    void run(const char*);
    void run(const char*, Print*) override;
  protected:
    void setTerminal(Terminal* term, CommandShell* shell);
    bool onInit() override;
    bool onStart() override;
    void onStop()  override;
    void onLoop() override;

  private:
    SimpleCLI* cli_;
    CommandShell* localShell_;     
    CommandShell* remoteShell_;
    Terminal* localTerm_;
    Terminal* remoteTerm_;
};

}