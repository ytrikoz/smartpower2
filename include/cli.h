#pragma once

#include "Shell/ParameterlessCommand.h"

namespace CLI {

void init();
bool open(Print *output);
void close();
bool active();

void onCommandError(cmd_error *e);
void onHelpCommand(cmd *c);
void onPrintCommand(cmd *c);
void onRMCommand(cmd *c);
void onShowCommand(cmd *c);
void onSetCommand(cmd *c);
void onGetCommand(cmd *c);
void onSystemCommand(cmd *c);
void onWifiScanCommand(cmd *c);
void onWifiDiagCommand(cmd *c);
void onClockCommand(cmd *c);

void print_P(const char*);
void onGetConfigParameter(const char*, const char*);
void onConfigParameterChanged(const char*, const char*, const char*);
void onUnknownCommandItem(const char*, const char*);
void onUnknownConfigParameter(const char*);
void onCommandDone(String&, String&);
void onCommandResult(const char*, const char*);
void onUnknownActionParameter(const char*, const char*);
bool isPositive(String);
bool isNegative(String);

}  // namespace CLI