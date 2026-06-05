#ifndef LAMP_REMOTE_SCREEN_H
#define LAMP_REMOTE_SCREEN_H

#include "Screen.h"
#include <IrRemote.h>
#include "../input/HoldButton.h"

class LampRemoteScreen : public Screen
{
public:
    LampRemoteScreen(IrRemote& remote, Button& selectBtn, uint32_t longPressMs)
        : remote_(remote), holdSelect_(selectBtn, longPressMs) {}

    void onEnter() override
    {
        holdSelect_.reset();
        remote_.draw();
    }

    ScreenId update(Inputs& in) override
    {
        if (in.up.pressed())   { if (remote_.moveUp(true))   remote_.draw(); }
        if (in.down.pressed()) { if (remote_.moveDown(true)) remote_.draw(); }

        switch (holdSelect_.poll(in.now))
        {
            case HoldButton::Event::ShortPress:
                remote_.sendSelected();
                break;
            case HoldButton::Event::LongPress:
                return ScreenId::IrMenu;
            default:
                break;
        }
        return ScreenId::None;
    }

private:
    IrRemote&  remote_;
    HoldButton holdSelect_;
};

#endif
