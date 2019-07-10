#pragma once

#include <Stream.h>

#include "TerminalWriter.h"
#include "global.h"
#include "consts.h"

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

static const char get_config_paramenter[] PROGMEM = "'%s'='%s'";
static const char config_parameter_changed[] PROGMEM = "%s сhanged '%s' -> '%s'";
static const char unknown_command_item[] PROGMEM = "Unknown item '%s' for command '%s'";
static const char unknown_action_param[] PROGMEM = "Unknown param '%s' for action '%s'";
static const char unknown_config_param[] PROGMEM = "Unknown config param '%s'";
static const char str_avaible_system_actions[] PROGMEM = "Available actions are: reset, load, save, restart, power";
static const char str_file_not_found[] PROGMEM = "File not found";
static const char str_done[] PROGMEM = "done";