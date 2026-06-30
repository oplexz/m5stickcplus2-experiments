#ifndef BLE_KEYBOARD_SCREEN_H
#define BLE_KEYBOARD_SCREEN_H

#include "Screen.h"
#include <BleKeyboard.h>
#include "../input/HoldButton.h"
#include "../input/RepeatButton.h"

class BleKeyboardScreen : public Screen
{
public:
    BleKeyboardScreen(BleKeyboard& keyboard,
                      Button& upBtn, Button& downBtn, Button& selectBtn,
                      uint32_t repeatDelayMs, uint32_t repeatIntervalMs,
                      uint32_t longPressMs)
    : keyboard_(keyboard)
    , repeatUp_(upBtn,   repeatDelayMs, repeatIntervalMs)
    , repeatDown_(downBtn, repeatDelayMs, repeatIntervalMs)
    , holdSelect_(selectBtn, longPressMs)
    {}

    void onEnter() override
    {
        repeatUp_.reset();
        repeatDown_.reset();
        holdSelect_.reset();
        keyboard_.start();
    }

    ScreenId update(Inputs& in) override
    {
        keyboard_.tick();

        // Up/Down scroll the scenario list (ignored while running).
        if (repeatUp_.poll(in.now))   keyboard_.selectPrev();
        if (repeatDown_.poll(in.now)) keyboard_.selectNext();

        switch (holdSelect_.poll(in.now))
        {
            case HoldButton::Event::ShortPress:
                if (keyboard_.isRunning())
                    keyboard_.cancel();
                else
                    keyboard_.activate(in.now);
                break;

            case HoldButton::Event::LongPress:
                keyboard_.cancel();
                keyboard_.stop();
                return ScreenId::List;

            default:
                break;
        }
        return ScreenId::None;
    }

private:
    BleKeyboard& keyboard_;
    RepeatButton repeatUp_;
    RepeatButton repeatDown_;
    HoldButton   holdSelect_;
};

#endif
