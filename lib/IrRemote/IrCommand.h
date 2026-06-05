#ifndef IR_COMMAND_H
#define IR_COMMAND_H

#include <Arduino.h>

struct IrCommand
{
    const char* name;
    uint8_t     address;
    uint8_t     command;
};

#endif
