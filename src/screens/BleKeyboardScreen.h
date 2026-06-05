#ifndef BLE_KEYBOARD_SCREEN_H
#define BLE_KEYBOARD_SCREEN_H

#include "Screen.h"
#include <BleKeyboard.h>
#include "../input/HoldButton.h"

class BleKeyboardScreen : public Screen
{
public:
    BleKeyboardScreen(BleKeyboard& keyboard, Button& selectBtn, uint32_t longPressMs)
        : keyboard_(keyboard), holdSelect_(selectBtn, longPressMs) {}

    void onEnter() override
    {
        holdSelect_.reset();
        keyboard_.start();
    }

    ScreenId update(Inputs& in) override
    {
        keyboard_.tick();

        switch (holdSelect_.poll(in.now))
        {
            case HoldButton::Event::ShortPress:
                keyboard_.runMacro();
                break;
            case HoldButton::Event::LongPress:
                keyboard_.stop();
                return ScreenId::List;
            default:
                break;
        }
        return ScreenId::None;
    }

private:
    BleKeyboard& keyboard_;
    HoldButton   holdSelect_;
};

#endif
