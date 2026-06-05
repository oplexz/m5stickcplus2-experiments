#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <Button.h>

enum class ScreenId
{
    None,
    List,
    IrMenu,
    Brightness,
    IrBruteforce,
    IrSend,
    IrRepeat,
    LampRemote,
    BleKeyboard,
};

struct Inputs
{
    Button&  up;
    Button&  down;
    Button&  select;
    uint32_t now;
};

class Screen
{
public:
    virtual void     onEnter()          = 0;
    virtual ScreenId update(Inputs& in) = 0;
    virtual         ~Screen()           = default;
};

#endif
