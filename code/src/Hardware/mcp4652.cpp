#include "mcp4652.h"

#include <Wire.h>

bool initialized = false;

void mcp4652_write(unsigned char addr, unsigned char value) {
    unsigned char cmd_byte = 0;
    cmd_byte |= (addr | CMD_WRITE);

    Wire.beginTransmission(MCP4652_I2C_ADDR);
    Wire.write(cmd_byte);
    Wire.write(value);
    Wire.endTransmission();
}

void mcp4652_init(unsigned char value) {
    mcp4652_write(WRITE_TCON_ADDR, 0x0B);
    mcp4652_write(WRITE_WIPER0_ADDR, value);
    mcp4652_write(WRITE_WIPER1_ADDR, 0x00);
    initialized = true;
}


void mcp4652_set(unsigned char value) {
    if (initialized)
        mcp4652_write(WRITE_WIPER0_ADDR, value);
    else
        mcp4652_init(value);
}