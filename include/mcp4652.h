#pragma once

#include "Arduino.h"

#define MCP4652_I2C_ADDR	0x2c
#define WRITE_WIPER0		(0b0000<<4)
#define WRITE_WIPER1		(0b0001<<4)
#define WRITE_TCON		(0b0100<<4)

#define CMD_WRITE		(0b00<<2)
#define CMD_INC			(0b01<<2)
#define CMD_DEC			(0b10<<2)
#define CMD_READ		(0b11<<2)

void mcp4652_init(void);
void mcp4652_write(unsigned char addr, unsigned char value);

