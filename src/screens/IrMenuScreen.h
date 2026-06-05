#ifndef IR_MENU_SCREEN_H
#define IR_MENU_SCREEN_H

#include "Screen.h"
#include <ScrollList.h>
#include "../input/HoldButton.h"

class IrMenuScreen : public Screen
{
public:
    static constexpr int kLampIndex       = 0;
    static constexpr int kBruteforceIndex = 1;
    static constexpr int kSendIndex       = 2;
    static constexpr int kRepeatIndex     = 3;

    IrMenuScreen(ScrollList& list, Button& selectBtn, uint32_t longPressMs)
        : list_(list), holdSelect_(selectBtn, longPressMs) {}

    void onEnter() override
    {
        holdSelect_.reset();
        list_.draw();
    }

    ScreenId update(Inputs& in) override
    {
        bool updated = false;
        if (in.up.pressed())   updated = list_.moveUp(true);
        if (in.down.pressed()) updated = list_.moveDown(true) || updated;

        switch (holdSelect_.poll(in.now))
        {
            case HoldButton::Event::ShortPress:
                switch (list_.selectedIndex())
                {
                    case kLampIndex:       return ScreenId::LampRemote;
                    case kBruteforceIndex: return ScreenId::IrBruteforce;
                    case kSendIndex:       return ScreenId::IrSend;
                    case kRepeatIndex:     return ScreenId::IrRepeat;
                }
                break;
            case HoldButton::Event::LongPress:
                return ScreenId::List;
            default:
                break;
        }

        if (updated) list_.draw();
        return ScreenId::None;
    }

private:
    ScrollList& list_;
    HoldButton  holdSelect_;
};

#endif
