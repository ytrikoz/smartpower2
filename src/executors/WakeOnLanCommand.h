#pragma once

#include "consts.h"
#include "global.h"

namespace executors {

void WakeOnLanCommand::Execute(Print *p)
{


}

size_t WakeOnLanCommand::printTo(Print& p) const {
    size_t res = p.println("wol");
    return res;
}

void WakeOnLanCommand::sendWOL(IPAddress addr, WiFiUDP udp, byte * mac,  size_t size_of_mac) {
    byte preamble[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    byte i;

    udp.beginPacket(addr, 9); //sending packet at 9, 
	
    udp.write(preamble, sizeof preamble);
    
    for (i = 0; i < 16; i++)
	{
        udp.write(mac, size_of_mac);
	}
    udp.endPacket();
}

}