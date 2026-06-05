#include "NecSender.h"

NecSender::NecSender(uint16_t irPin)
: irSender_(irPin)
{}

void NecSender::begin()
{
    irSender_.begin();
}

void NecSender::send(uint8_t address, uint8_t command)
{
    // Standard NEC 32-bit frame: addr | ~addr | cmd | ~cmd
    uint32_t code = (static_cast<uint32_t>(address)  << 24) |
                    (static_cast<uint32_t>(~address & 0xFF) << 16) |
                    (static_cast<uint32_t>(command)   << 8)  |
                    static_cast<uint32_t>(~command & 0xFF);

    irSender_.sendNEC(code, 32);
}
