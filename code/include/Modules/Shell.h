#pragma once

#include <Arduino.h>

#include "Core/Module.h"
#include "Cli/CommandShell.h"

namespace Modules { 

class Console : public Module {
  public:
    Console();
  public:
    void setShell(Cli::CommandShell* shell);
    bool isOpen();
    void sendData(const String&);
  protected:
    bool onInit() override;
    bool onStart() override;
    void onStop()  override;
    void onLoop() override;
  
  private:
    Cli::CommandShell* shell_;     
    Cli::Terminal* term_;
};

}