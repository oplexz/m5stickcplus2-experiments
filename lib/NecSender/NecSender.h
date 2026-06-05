#ifndef NEC_SENDER_H
#define NEC_SENDER_H

#include <Arduino.h>
#include <IRsend.h>

// Owns a single IRsend instance and encapsulates NEC-protocol frame building.
// Replaces four identical addr|~addr|cmd|~cmd patterns spread across the IR
// modules.
class NecSender
{
public:
    explicit NecSender(uint16_t irPin);

    void begin();
    void send(uint8_t address, uint8_t command);

private:
    IRsend irSender_;
};

#endif
