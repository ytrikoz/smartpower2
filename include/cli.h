#pragma once

#include "global.h"
#include "Print.h"

void init_cli();
bool start_cli(Print *output);
void quit_cli();
bool is_cli_active();

void onCommandError(cmd_error *e);
void onHelpCommand(cmd *c);
void onPrintCommand(cmd *c);
void onRMCommand(cmd *c);
void onShowCommand(cmd *c);
void onSetCommand(cmd *c);
void onGetCommand(cmd *c);
void onSystemCommand(cmd *c);
void onSystemWifiScanCommand(cmd *c);
void onSystemWifiDiagCommand(cmd* c);