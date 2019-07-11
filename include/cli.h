#pragma once

#include <Stream.h>

#include "TerminalWriter.h"
#include "consts.h"
#include "global.h"

extern void init_cli();
extern bool start_cli(TerminalWriter *writer);
extern void quit_cli();
extern bool is_cli_active();

void onCommandError(cmd_error *e);
void onHelpCommand(cmd *c);
void onPrintCommand(cmd *c);
void onRMCommand(cmd *c);
void onShowCommand(cmd *c);
void onSetCommand(cmd *c);
void onGetCommand(cmd *c);
void onSystemCommand(cmd *c);
